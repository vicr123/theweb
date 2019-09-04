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
#include "featuremanager.h"

#include <QMap>
#include <QWebEngineProfile>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUrl>

#include <QDebug>

struct FeatureManagerPrivate {
    static QMap<QWebEngineProfile*, FeatureManager*> managers;

    QWebEngineProfile* profile;
    QSqlDatabase db;
};

QMap<QWebEngineProfile*, FeatureManager*> FeatureManagerPrivate::managers;

FeatureManager* FeatureManager::managerFor(QWebEngineProfile* profile) {
    FeatureManager* mgr = FeatureManagerPrivate::managers.value(profile, nullptr);
    if (mgr == nullptr) {
        mgr = new FeatureManager(profile);
        FeatureManagerPrivate::managers.insert(profile, mgr);
    }
    return mgr;
}

FeatureManager::FeatureAllowed FeatureManager::isFeatureAllowed(QUrl originUrl, FeatureManager::Feature feature)
{
    if (feature == Unknown || feature == LastFeature) return Ask;

    QSqlQuery query(d->db);
    query.prepare(QString("SELECT allow FROM %1 WHERE id = :siteId").arg(featureTable(feature)));
    query.bindValue(":siteId", originSiteId(originUrl));
    query.exec();

    if (query.next()) {
        if (query.value("allow").toInt() == 0) {
            return Deny;
        } else {
            return Allow;
        }
    } else {
        return Ask;
    }
}

void FeatureManager::setFeatureAllowed(QUrl originUrl, FeatureManager::Feature feature, FeatureManager::FeatureAllowed allowed)
{
    if (feature == Unknown || feature == LastFeature) return;

    QSqlQuery query(d->db);
    if (allowed == Ask) {
        query.prepare(QString("DELETE FROM %1 WHERE id = :siteId").arg(featureTable(feature)));
    } else {
        query.prepare(QString("INSERT OR REPLACE INTO %1(id, allow) VALUES(:siteId, :allow)").arg(featureTable(feature)));
        if (allowed == Allow) {
            query.bindValue(":allow", 1);
        } else {
            query.bindValue(":allow", 0);
        }
    }
    query.bindValue(":siteId", originSiteId(originUrl));
    query.exec();
}

QList<FeatureManager::Feature> FeatureManager::allFeatures()
{
    QList<Feature> f;
    for (int i = 0; i < FeatureManager::LastFeature; i++) {
        f.append(static_cast<Feature>(i));
    }
    return f;
}

FeatureManager::FeatureManager(QWebEngineProfile* profile) : QObject(nullptr)
{
    d = new FeatureManagerPrivate();
    d->profile = profile;

    d->db = QSqlDatabase::addDatabase("QSQLITE", "db_features_" + profile->storageName());
    d->db.setDatabaseName(QDir::cleanPath(profile->persistentStoragePath() + "/features"));
    if (!d->db.open()) {
        qDebug() << "Database error!";
        return;
    }

    //Enable foreign keys
    d->db.exec("PRAGMA foreign_keys = ON");

    //Create tables if required
    d->db.exec("CREATE TABLE IF NOT EXISTS features(id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, origin TEXT)");

    for (int i = Location; i < LastFeature; i++) {
        //Using .arg is safe from SQL injection here
        d->db.exec(QString("CREATE TABLE IF NOT EXISTS %1(id INTEGER NOT NULL PRIMARY KEY, allow INTEGER, CONSTRAINT fk_%1 FOREIGN KEY (id) REFERENCES features(id) ON DELETE CASCADE)").arg(featureTable(static_cast<Feature>(i))));
    }
}

int FeatureManager::originSiteId(QUrl site) {
    //Get the site ID
    QSqlQuery idQuery(d->db);
    idQuery.prepare("SELECT id FROM features WHERE origin = :origin");
    idQuery.bindValue(":origin", site.toString());
    idQuery.exec();

    if (idQuery.next()) {
        return idQuery.value("id").toInt();
    } else {
        //This is a new site so we need to insert it into the feature table
        QSqlQuery insertQuery(d->db);
        insertQuery.prepare("INSERT INTO features(origin) VALUES(:origin)");
        insertQuery.bindValue(":origin", site.toString());
        insertQuery.exec();

        return insertQuery.lastInsertId().toInt();
    }
}

QString FeatureManager::featureTable(FeatureManager::Feature feature)
{
    switch (feature) {
        case FeatureManager::Unknown:
            return "";
        case FeatureManager::Location:
            return QStringLiteral("location");
        case FeatureManager::Camera:
            return QStringLiteral("camera");
        case FeatureManager::Microphone:
            return QStringLiteral("microphone");
        case FeatureManager::LastFeature:
            return "";
    }
    return "";
}
