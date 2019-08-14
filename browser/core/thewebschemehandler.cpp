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

#include <QUrlQuery>
#include <QBuffer>
#include <QDebug>
#include <QWebEngineUrlRequestJob>
#include <QFile>
#include <QMimeDatabase>
#include "managers/settingsmanager.h"

thewebSchemeHandler::thewebSchemeHandler(QObject *parent) : QWebEngineUrlSchemeHandler(parent)
{

}

void thewebSchemeHandler::requestStarted(QWebEngineUrlRequestJob* job)
{
    QUrl url = job->requestUrl();

    if (url.host() == "api") {
        QBuffer* buf = new QBuffer(job);
        if (url.path() == "/settings") {
            buf->setData(SettingsManager::getJson());
        } else if (url.path() == "/settings/set") {
            QUrlQuery query(url.query());
            QString key = query.queryItemValue("key", QUrl::FullyDecoded);
            QString value = query.queryItemValue("value", QUrl::FullyDecoded);
            SettingsManager::set(key, value);
            buf->setData("{\"Status\":\"OK\"}");
        } else if (url.path() == "/lang") {
            buf->setData(QLocale().name().toUtf8());
        }
        buf->open(QBuffer::ReadOnly);
        job->reply("application/json", buf);
        return;
    }

    QStringList tryFiles;
    if (url.path() == "") {
        if (url.host() == "settings") {
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
