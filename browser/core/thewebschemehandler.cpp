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
#include "thewebschemehandler.h"

#include <functional>
#include <QUrlQuery>
#include <QBuffer>
#include <QDebug>
#include <QWebEngineUrlRequestJob>
#include <QFile>
#include <QMimeDatabase>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDateTime>
#include <QJsonObject>
#include <QWebEngineProfile>
#include "managers/settingsmanager.h"
#include "managers/iconmanager.h"
#include "managers/historymanager.h"

#define API_PATH_FAIL QByteArray("API_FAIL")

struct thewebSchemeHandlerPrivate {
    QVariantMap options;
    QMap<QString, std::function<QByteArray(QWebEngineUrlRequestJob*)>> apiPaths;
};

thewebSchemeHandler::thewebSchemeHandler(QVariantMap options, QObject *parent) : QWebEngineUrlSchemeHandler(parent)
{
    d = new thewebSchemeHandlerPrivate();
    d->options = options;

    //Settings API paths
    d->apiPaths.insert("/settings", [=](QWebEngineUrlRequestJob* job) {
        return SettingsManager::getJson();
    });
    d->apiPaths.insert("/settings/set", [=](QWebEngineUrlRequestJob* job) {
        QUrlQuery query(job->requestUrl().query());
        QString key = query.queryItemValue("key", QUrl::FullyDecoded);
        QString value = query.queryItemValue("value", QUrl::FullyDecoded);
        SettingsManager::set(key, value);
        return "{\"Status\":\"OK\"}";
    });
    d->apiPaths.insert("/settings/clear", [=](QWebEngineUrlRequestJob* job) {
        //Reset theWeb settings back to the defaults
        SettingsManager::resetSettings();

        //Clear profile cache

        //Clear profile cookies

        return "{\"Status\":\"OK\"}";
    });

    d->apiPaths.insert("/history", [=](QWebEngineUrlRequestJob* job) {
        HistoryManager* history = HistoryManager::managerFor(options.value("profile").value<QWebEngineProfile*>());
        QUrlQuery query(job->requestUrl().query());

        QDateTime latest = QDateTime::currentDateTime(), earliest = QDateTime::currentDateTime().addYears(-1);
        if (query.hasQueryItem("latest")) {
            latest = QDateTime::fromMSecsSinceEpoch(query.queryItemValue("latest").toLongLong());
        }
        if (query.hasQueryItem("earliest")) {
            earliest = QDateTime::fromMSecsSinceEpoch(query.queryItemValue("earliest").toLongLong());
        }

        if (job->requestMethod() == "GET") {
            QJsonArray array;
            for (HistoryManager::HistoryEntry entry : history->historyEntries(latest, earliest)) {
                QJsonObject jsonEntry;
                jsonEntry.insert("url", entry.url.toString());
                jsonEntry.insert("accessed", entry.accessTime.toMSecsSinceEpoch());
                jsonEntry.insert("title", entry.pageTitle);
                array.append(jsonEntry);
            }

            return QJsonDocument(array).toJson();
        } else if (job->requestMethod() == "DELETE") {
            if (!query.hasQueryItem("latest") && !query.hasQueryItem("earliest")) {
                history->clearHistory();
            } else {
                history->clearHistory(latest, earliest);
            }
            return QByteArray("{\"Status\":\"OK\"}");
        } else {
            return API_PATH_FAIL;
        }
    });

    //Misc. API paths
    d->apiPaths.insert("/lang", [=](QWebEngineUrlRequestJob* job) {
        return QLocale().name().toUtf8();
    });
    d->apiPaths.insert("/oblivion", [=](QWebEngineUrlRequestJob* job) {
        return QString("{\"isOblivion\":%1}").arg(d->options.value("oblivion", false).toBool() ? "true" : "false").toUtf8();
    });
}

thewebSchemeHandler::~thewebSchemeHandler()
{
    delete d;
}

void thewebSchemeHandler::requestStarted(QWebEngineUrlRequestJob* job)
{
    if (job->initiator().scheme() != "theweb" && job->initiator().scheme() != "" && job->initiator().host() != "localhost") {
        job->fail(QWebEngineUrlRequestJob::RequestDenied);
        return;
    }

    QUrl url = job->requestUrl();

    if (url.host() == "api") {
        if (d->apiPaths.contains(url.path())) {
            QBuffer* buf = new QBuffer(job);
            QByteArray data = d->apiPaths.value(url.path())(job);
            if (data == API_PATH_FAIL) {
                job->fail(QWebEngineUrlRequestJob::RequestFailed);
            } else {
                buf->setData(data);
                buf->open(QBuffer::ReadOnly);
                job->reply("application/json", buf);
            }
        } else {
            job->fail(QWebEngineUrlRequestJob::UrlInvalid);
        }
        return;
    } else if (url.host() == "sysicons") {
        if (url.path() != "/") {
            job->fail(QWebEngineUrlRequestJob::UrlInvalid);
            return;
        }


        QUrlQuery query(url.query());
        QString widthStr = query.hasQueryItem("width") ? query.queryItemValue("width") : "16";
        QString heightStr = query.hasQueryItem("height") ? query.queryItemValue("height") : "16";

        bool ok;

        int width = widthStr.toInt(&ok);
        if (!ok) width = 16;

        int height = heightStr.toInt(&ok);
        if (!ok) height = 16;


        QString icons = query.queryItemValue("icons", QUrl::FullyDecoded);
        QStringList iconsToSearch = icons.split(";");
        for (QString iconName : iconsToSearch) {
            QIcon icon;
            if (iconName.startsWith("sys:")) {
                icon = IconManager::getIcon(iconName.mid(4), Qt::white, QSize(width, height));
            } else {
                icon = QIcon(iconName);
            }

            if (!icon.isNull()) {
                QBuffer* buf = new QBuffer(job);
                buf->open(QBuffer::ReadWrite);
                icon.pixmap(QSize(width, height)).save(buf, "PNG");
                buf->seek(0);
                job->reply("image/png", buf);
            }
        }

        job->fail(QWebEngineUrlRequestJob::UrlNotFound);
        return;
    }

    QStringList tryFiles;
    QStringList pageHosts = {"settings", "newtab", "history", "about"};
    if (url.path() == "") {
        if (pageHosts.contains(url.host())) {
            tryFiles.append(":/scheme/theweb/index.html");
        }
    }

    tryFiles.append(":/scheme/theweb/" + url.host() + url.path());
    if (url.path() != "") tryFiles.append(":/scheme/theweb/" + url.path());
    tryFiles.append(":/scheme/theweb/" + url.host() + ".html");

    for (QString filePath : tryFiles) {
        QFile* file = new QFile(filePath, job);
        if (file->open(QFile::ReadOnly)) {
            QMimeDatabase mimeDb;
            QMimeType type = mimeDb.mimeTypeForFile(filePath);
            job->reply(type.name().toUtf8(), file);
            return;
        }
    }
    job->fail(QWebEngineUrlRequestJob::UrlInvalid);
}
