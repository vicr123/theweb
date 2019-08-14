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
#include "urlinterceptor.h"

#include <QSettings>

struct UrlInterceptorPrivate {
    QSettings settings;
};

UrlInterceptor::UrlInterceptor(QObject *parent) : QWebEngineUrlRequestInterceptor(parent)
{
    d = new UrlInterceptorPrivate();
}

UrlInterceptor::~UrlInterceptor()
{
    delete d;
}

void UrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo& info)
{
    if (d->settings.value("privacy/dnt", true).toBool()) {
        info.setHttpHeader("DNT", "1");
    }
}
