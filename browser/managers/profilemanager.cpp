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
#include "profilemanager.h"

#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include "core/thewebschemehandler.h"
#include "core/urlinterceptor.h"

struct ProfileManagerPrivate {
    QWebEngineProfile* defaultProfile = nullptr;
};

ProfileManagerPrivate* ProfileManager::d = new ProfileManagerPrivate();

QWebEngineProfile*ProfileManager::defaultProfile()
{
    if (d->defaultProfile == nullptr) {
        d->defaultProfile = new QWebEngineProfile("theweb-default-profile");
        d->defaultProfile->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
        d->defaultProfile->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
        //Only on Qt 5.13 or higher
        d->defaultProfile->setUseForGlobalCertificateVerification();
#endif

        UrlInterceptor* interceptor = new UrlInterceptor();
        d->defaultProfile->setUrlRequestInterceptor(interceptor);

        thewebSchemeHandler* schemeHandler = new thewebSchemeHandler();
        d->defaultProfile->installUrlSchemeHandler("theweb", schemeHandler);

        //Edit the current user agent and insert theWeb before QtWebEngine
        QString userAgent = d->defaultProfile->httpUserAgent();
        if (userAgent.contains("QtWebEngine/")) {
            userAgent.insert(userAgent.indexOf("QtWebEngine/"), "theWeb/15.0 ");
            d->defaultProfile->setHttpUserAgent(userAgent);
        }
    }
    return d->defaultProfile;
}

ProfileManager::ProfileManager(QObject *parent) : QObject(parent)
{

}
