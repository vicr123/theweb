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

#include <QDebug>
#include <QWebEngineUrlRequestJob>
#include <QFile>
#include <QMimeDatabase>

thewebSchemeHandler::thewebSchemeHandler(QObject *parent) : QWebEngineUrlSchemeHandler(parent)
{

}

void thewebSchemeHandler::requestStarted(QWebEngineUrlRequestJob* job)
{
    QStringList tryFiles;
    if (job->requestUrl().path() == "") {
        if (job->requestUrl().host() == "settings") {
            tryFiles.append(":/scheme/theweb/index.html");
        }
    }

    tryFiles.append(":/scheme/theweb/" + job->requestUrl().host() + job->requestUrl().path());
    if (job->requestUrl().path() != "") tryFiles.append(":/scheme/theweb/" + job->requestUrl().path());
    tryFiles.append(":/scheme/theweb/" + job->requestUrl().host() + ".html");

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
