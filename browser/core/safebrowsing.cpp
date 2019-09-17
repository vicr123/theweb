/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2019 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "safebrowsing.h"

#include <QStandardPaths>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QDir>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QHostAddress>
#include <QCryptographicHash>
#include <tapplication.h>
#include <tpromise.h>

#define SAFE_BROWSING_API_FETCH_ENDPOINT "https://safebrowsing.googleapis.com/v4/threatListUpdates:fetch"
#define SAFE_BROWSING_API_FIND_ENDPOINT "https://safebrowsing.googleapis.com/v4/fullHashes:find"

struct SafeBrowsingPrivate {
    SafeBrowsing* instance = nullptr;

    QNetworkAccessManager mgr;
    QSqlDatabase db;

    QMap<QString, QString> knownThreats;

    QSqlDatabase getDatabase(uint number, QString& dbName) {
        dbName = QStringLiteral("db_safebrowsing_%1").arg(number);
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", dbName);
        db.setDatabaseName(QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/safebrowsing"));
        if (!db.open()) {
            qDebug() << "Database error!";
        }
        return db;
    }

    QJsonObject clientObject() {
        QJsonObject client;
        client.insert("clientId", "theweb");
        client.insert("clientVersion", QApplication::applicationVersion());
        return client;
    }

    uint dbLockNum = 0;
};

class DBRemover {
    public:
        DBRemover(QString dbName = "") {
            this->dbName = dbName;
        }

        ~DBRemover() {
            if (dbName != "") QSqlDatabase::removeDatabase(dbName);
        }

        void setDb(QString dbName) {
            this->dbName = dbName;
        }

    private:
        QString dbName;
};

SafeBrowsingPrivate* SafeBrowsing::d = new SafeBrowsingPrivate();

SafeBrowsing::SafeBrowsing(QObject *parent) : QObject(parent)
{
    d->db = QSqlDatabase::addDatabase("QSQLITE", "db_safebrowsing");
    d->db.setDatabaseName(QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/safebrowsing"));
    if (!d->db.open()) {
        qDebug() << "Database error!";
        return;
    }

    //Ensure case sensitivity is on
    d->db.exec("PRAGMA case_sensitive_like=ON");

    //Create tables if required
    d->db.exec("CREATE TABLE states(threatType TEXT PRIMARY KEY, state TEXT)");
    d->db.exec("CREATE TABLE IF NOT EXISTS threats(type TEXT, hash TEXT, PRIMARY KEY (type, hash))");
    updateLists();
}

SafeBrowsing::~SafeBrowsing()
{

}

void SafeBrowsing::initialize()
{
    if (d->instance == nullptr) d->instance = new SafeBrowsing();
}

tPromise<QString>* SafeBrowsing::checkUrl(QUrl url)
{
    //Only check URLs if the user hasn't turned off GSB
    QSettings settings;
    if (!settings.value("privacy/gsb", true).toBool()) {
        return new tPromise<QString>([=](QString& error) {
            error = "Safe Browsing support switched off";
            return "";
        });
    }

    //Only check URLs if we are compiling with Safe Browsing support
#ifdef SAFE_BROWSING_API_KEY
    uint dbNumber = d->dbLockNum++;
    return new tPromise<QString>([=](QString& error) {
        DBRemover remover;

        {
            QString dbName;
            QSqlDatabase db = d->getDatabase(dbNumber, dbName);
            remover.setDb(dbName);

            QString suspiciousPartialHash;
            QString suspiciousHash;
            QString threatType;

            QString canonicalizedUrl = canonicalizeUrl(url);
            if (d->knownThreats.contains(canonicalizedUrl)) {
                //This is already a known threat
                return d->knownThreats.value(canonicalizedUrl);
            }

            QStringList exprs = suffixPrefixExpressionsForUrl(canonicalizedUrl);
            for (QString expr : exprs) {
                QString hash = QCryptographicHash::hash(expr.toUtf8(), QCryptographicHash::Sha256).toHex();

                //Query the database
                QSqlQuery query(db);
                query.prepare("SELECT * FROM threats WHERE :hash LIKE hash||'%'");
                query.bindValue(":hash", hash);
                query.exec();

                if (query.next()) {
                    //We might have a match?
                    suspiciousPartialHash = query.value("hash").toString();
                    suspiciousHash = hash;
                    threatType = query.value("type").toString();
                }
            }

            if (suspiciousPartialHash != "") {
                //Get the states
                QJsonArray states;
                QSqlQuery stateQuery = db.exec("SELECT * FROM states");
                while (stateQuery.next()) {
                    states.append(stateQuery.value("state").toString());
                }

                //Request the full hashes
                QJsonObject payload;

                payload.insert("client", d->clientObject());

                QJsonObject request;
                request.insert("threatTypes", QJsonArray({threatType}));
                request.insert("platformTypes", QJsonArray({"LINUX"})); //TODO: change depending on the platform
                request.insert("threatEntryTypes", QJsonArray({"URL"}));

                QJsonObject threatEntry;
                threatEntry.insert("hash", QString(QByteArray::fromHex(suspiciousPartialHash.toUtf8()).toBase64()));
                request.insert("threatEntries", QJsonArray({threatEntry}));

                payload.insert("threatInfo", request);
                payload.insert("clientStates", states);

                QUrl requestUrl(SAFE_BROWSING_API_FIND_ENDPOINT);
                QUrlQuery query;
                query.addQueryItem("key", SAFE_BROWSING_API_KEY);
                requestUrl.setQuery(query);

                QEventLoop loop;
                QNetworkRequest req(requestUrl);
                req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
                QNetworkReply* reply = d->mgr.post(req, QJsonDocument(payload).toJson(QJsonDocument::Compact));
                connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
                loop.exec();

                QByteArray resp = reply->readAll();
                qDebug() << resp;

                QJsonObject objResponse = QJsonDocument::fromJson(resp).object();
                QJsonArray matches = objResponse.value("matches").toArray();
                for (QJsonValue matchVal : matches) {
                    QJsonObject match = matchVal.toObject();
                    QString matchHash = match.value("threat").toObject().value("hash").toString();
                    if (suspiciousHash == QByteArray::fromBase64(matchHash.toUtf8()).toHex()) {
                        //We have a match!
                        //Cache this result in memory
                        d->knownThreats.insert(canonicalizedUrl, threatType);
                        return threatType;
                    }
                }
            }

            return QStringLiteral("");
        }
    });
#else
    return new tPromise<QString>([=](QString& error) {
        error = "Safe Browsing support not compiled";
        return "";
    });
#endif
}

QString SafeBrowsing::checkUrlLocally(QUrl url)
{
    QString canonicalizedUrl = canonicalizeUrl(url);
    return d->knownThreats.value(canonicalizedUrl, "");
}

QString SafeBrowsing::canonicalizeUrl(QUrl url)
{
    //Remove fragment portion of the URL
    url.setFragment("");

    QUrl canonicalizedUrl = url;

    //Canonicalize the path
    QString path = QUrl::fromPercentEncoding(url.path(QUrl::FullyEncoded).toUtf8()).trimmed();
    while (path != QUrl::fromPercentEncoding(path.toUtf8())) {
        path = QUrl::fromPercentEncoding(path.toUtf8());
    }
    path = QDir::cleanPath(path);
    if (path.isEmpty()) path = "/";
    canonicalizedUrl.setPath(path);

    //Canonicalize the hostname
    QString hostname = QUrl::fromPercentEncoding(url.host(QUrl::FullyEncoded).toUtf8()).trimmed();
    while (hostname != QUrl::fromPercentEncoding(hostname.toUtf8())) {
        hostname = QUrl::fromPercentEncoding(hostname.toUtf8());
    }

    hostname.replace("..", ".");
    hostname = hostname.toLower();
    canonicalizedUrl.setHost(hostname);

    QString returnUrl = canonicalizedUrl.toString(QUrl::FullyEncoded | QUrl::RemoveFragment | QUrl::RemovePort);

    return returnUrl;
}

QStringList SafeBrowsing::suffixPrefixExpressionsForUrl(QString canonicalizedUrl)
{
    QStringList expressions;
    QUrl url(canonicalizedUrl);

    QStringList hostStrings;

    QString host = url.host(QUrl::FullyEncoded);
    hostStrings.append(host); //Exact hostname

    QHostAddress addressCheck;
    if (!addressCheck.setAddress(host)) {
        //Only apply the following rules to non IP addresses
        QStringList hostnameParts = host.split(".");
        hostnameParts.takeFirst();
        while (hostnameParts.count() > 5) hostnameParts.takeFirst();
        int hostnamePartsCount = hostnameParts.count();
        for (int i = 0; i < hostnamePartsCount - 1; i++) {
            if (hostnameParts.count() == 0) break;
            hostStrings.append(hostnameParts.join("."));
            hostnameParts.takeFirst();
        }
    }

    QStringList pathStrings;
    QString path = url.path(QUrl::FullyEncoded);
    if (url.hasQuery()) pathStrings.append(path + "?" + url.query(QUrl::FullyEncoded)); //Exact path + query
    pathStrings.append(path); //Exact path without query

    //Four paths, starting from / and successively appending path components
    QString pathBuffer;
    int pathWalkerCount = 0;
    for (QChar c : path) {
        pathBuffer.append(c);
        if (c == '/') {
            if (pathStrings.contains(pathBuffer)) break;
            pathStrings.append(pathBuffer);
            pathWalkerCount++;
            if (pathWalkerCount == 4) break;
        }
    }


//    QStringList pathParts = path.split("/");
//    pathParts.takeFirst();
//    for (int i = 0; i < qMin(pathParts.count(), 4); i++) {
//        pathStrings.append("/" + pathParts.mid(0, i).join("/") + ((i + 1 == pathParts.count() && i != 0) ? "/" : ""));
//    }

    for (QString hostnamePart : hostStrings) {
        for (QString pathPart : pathStrings) {
            expressions.append(hostnamePart + pathPart);
        }
    }

    return expressions;
}

void SafeBrowsing::updateLists()
{
    updateListForThreatType("MALWARE");
    updateListForThreatType("SOCIAL_ENGINEERING");
    updateListForThreatType("UNWANTED_SOFTWARE");
    updateListForThreatType("POTENTIALLY_HARMFUL_APPLICATION");
}

void SafeBrowsing::updateListForThreatType(QString threatType)
{
    //Only update the lists if we are compiling with Safe Browsing support
#ifdef SAFE_BROWSING_API_KEY

    //Only update the lists if the user hasn't turned off GSB
    QSettings settings;
    if (!settings.value("privacy/gsb", true).toBool()) return;

    //Check the current state
    QString state;
    QSqlQuery stateQuery(d->db);
    stateQuery.prepare("SELECT state FROM states WHERE threatType=:threatType");
    stateQuery.bindValue(":threatType", threatType);
    stateQuery.exec();

    if (stateQuery.next()) {
        state = stateQuery.value("state").toString();
    }

    //Request a new list
    QJsonObject payload;

    payload.insert("client", d->clientObject());

    QJsonObject request;
    request.insert("threatType", threatType);
    request.insert("platformType", "LINUX"); //TODO: change depending on the platform
    request.insert("threatEntryType", "URL");
    request.insert("state", state);

    QJsonObject constraints;
    constraints.insert("maxUpdateEntries", 0);
    constraints.insert("maxDatabaseEntries", 0);
    constraints.insert("supportedCompressions", QJsonArray({"RAW"}));
    request.insert("constraints", constraints);

    payload.insert("listUpdateRequests", request);

    QUrl requestUrl(SAFE_BROWSING_API_FETCH_ENDPOINT);
    QUrlQuery query;
    query.addQueryItem("key", SAFE_BROWSING_API_KEY);
    requestUrl.setQuery(query);

    QNetworkRequest req(requestUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply* reply = d->mgr.post(req, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [=] {
        QByteArray resp = reply->readAll();
        QJsonObject objResponse = QJsonDocument::fromJson(resp).object();

        QJsonArray updateResponses = objResponse.value("listUpdateResponses").toArray();
        for (QJsonValue responseVal : updateResponses) {
            QJsonObject response = responseVal.toObject();
            QString checksum = response.value("checksum").toObject().value("sha256").toString();

            QString threatType = response.value("threatType").toString();
            QString responseType = response.value("responseType").toString();

            QString state = response.value("newClientState").toString();

            QVariantList removalList;
            QJsonArray removals = response.value("removals").toArray();
            for (QJsonValue removalVal : removals) {
                QJsonObject removal = removalVal.toObject();
                QJsonArray indices = removal.value("rawIndices").toObject().value("indices").toArray();
                for (QJsonValue index : indices) {
                    removalList.append(index.toInt());
                }
            }

            //Sort in descending order so we end up removing the largest index first
            //That way we don't end up messing with other indices
            std::sort(removalList.begin(), removalList.end(), [=](const QVariant& first, const QVariant& second) {
                return first.toInt() > second.toInt();
            });

            QVariantList additionList;
            QJsonArray additions = response.value("additions").toArray();
            for (QJsonValue additionVal : additions) {
                QJsonObject addition = additionVal.toObject();
                QJsonObject rawHashes = addition.value("rawHashes").toObject();

                int prefixSize = rawHashes.value("prefixSize").toInt();
                QByteArray hashes = QByteArray::fromBase64(rawHashes.value("rawHashes").toString().toUtf8());

                //Split the string up
                for (int i = 0; i < hashes.length(); i += prefixSize) {
                    additionList.append(hashes.mid(i, prefixSize).toHex());
                }
            }


            //Commit changes to the database
            uint dbNumber = d->dbLockNum++;
            (new tPromise<void>([=](QString& error) {
                QString dbName;

                {
                    QSqlDatabase db = d->getDatabase(dbNumber, dbName);

                    //Start a transaction when possible
                    while (!db.transaction()) {
                        QThread::msleep(5000);
                    }

                    //Start fresh if we need to
                    if (responseType == "FULL_UPDATE") {
                        qDebug() << "Clearing Safe Browsing database for" << threatType << "because the server requires a full update";
                        QSqlQuery query(db);
                        query.prepare("DELETE FROM threats WHERE type=:type");
                        query.bindValue(":type", threatType);
                        query.exec();
                    }

                    //Perform removals

                    QVariantList removalThreats;
                    removalThreats.reserve(removalList.count());
                    for (int i = 0; i < removalList.count(); i++) removalThreats.append(threatType);

                    QSqlQuery removalQuery(db);
                    removalQuery.prepare("DELETE FROM threats WHERE hash IN (SELECT hash FROM threats WHERE type=:threatType ORDER BY hash ASC LIMIT 1 OFFSET :removal)");
                    removalQuery.bindValue(":threatType", removalThreats);
                    removalQuery.bindValue(":removal", removalList);
                    removalQuery.execBatch();

                    //Perform additions

                    QVariantList additionThreats;
                    additionThreats.reserve(additionList.count());
                    for (int i = 0; i < additionList.count(); i++) additionThreats.append(threatType);

                    QSqlQuery insertQuery(db);
                    insertQuery.prepare("INSERT INTO threats(type, hash) VALUES(:type, :hash)");
                    insertQuery.bindValue(":type", additionThreats);
                    insertQuery.bindValue(":hash", additionList);
                    insertQuery.execBatch();

                    //Modify state
                    QSqlQuery stateUpdateQuery(db);
                    stateUpdateQuery.prepare("INSERT OR REPLACE INTO states(threatType, state) VALUES(:threatType, :state)");
                    stateUpdateQuery.bindValue(":threatType", threatType);
                    stateUpdateQuery.bindValue(":state", state);
                    stateUpdateQuery.exec();

                    db.commit();

                    //Perform validation
                    QCryptographicHash checkHash(QCryptographicHash::Sha256);
                    QSqlQuery checkQuery(db);
                    checkQuery.prepare("SELECT hash FROM threats WHERE type=:type ORDER BY hash ASC");
                    checkQuery.bindValue(":type", threatType);
                    checkQuery.exec();

                    while (checkQuery.next()) {
                        checkHash.addData(QByteArray::fromHex(checkQuery.value("hash").toString().toUtf8()));
                    }

                    if (checkHash.result() != QByteArray::fromBase64(checksum.toUtf8())) {
                        qWarning() << "Safe Browsing checksums did not match for" << threatType;

                        //TODO: Reset the state and try again
                    }
                }

                QSqlDatabase::removeDatabase(dbName);
            }));
            //TODO: Implement state
            //TODO: Implement minimum wait durations
        }
    });
#endif
}
