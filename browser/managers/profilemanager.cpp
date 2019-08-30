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

#include <tnotification.h>
#include <QUrlQuery>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QStandardPaths>
#include <QDir>
#include "core/thewebschemehandler.h"
#include "core/urlinterceptor.h"
#include "downloadmanager.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
#include <QWebEngineNotification>
#endif

struct ProfileManagerPrivate {
    QWebEngineProfile* defaultProfile = nullptr;
    QWebEngineProfile* oblivionProfile = nullptr;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
    QMap<tNotification*, std::unique_ptr<QWebEngineNotification>> notificationsMap;
#endif
};

ProfileManagerPrivate* ProfileManager::d = new ProfileManagerPrivate();

QWebEngineProfile *ProfileManager::defaultProfile()
{
    if (d->defaultProfile == nullptr) {
        d->defaultProfile = new QWebEngineProfile("theweb-default-profile");

        d->defaultProfile->setPersistentStoragePath(QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/profiles/theweb-default-profile"));
        d->defaultProfile->setCachePath(QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/profiles/theweb-default-profile"));

        d->defaultProfile->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
        d->defaultProfile->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
        d->defaultProfile->settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);

        UrlInterceptor* interceptor = new UrlInterceptor(d->defaultProfile);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
        //Only on Qt 5.13 or higher
        d->defaultProfile->setUseForGlobalCertificateVerification();
        d->defaultProfile->setUrlRequestInterceptor(interceptor);
        d->defaultProfile->setNotificationPresenter(&ProfileManager::notificationPresenter);
#else
        d->defaultProfile->setRequestInterceptor(interceptor);
#endif

        thewebSchemeHandler* schemeHandler = new thewebSchemeHandler({{"profile", QVariant::fromValue(d->defaultProfile)}});
        d->defaultProfile->installUrlSchemeHandler("theweb", schemeHandler);

        //Edit the current user agent and insert theWeb before QtWebEngine
        QString userAgent = d->defaultProfile->httpUserAgent();
        if (userAgent.contains("QtWebEngine/")) {
            userAgent.insert(userAgent.indexOf("QtWebEngine/"), "theWeb/15.0 ");
            d->defaultProfile->setHttpUserAgent(userAgent);
        }

        DownloadManager::registerProfile(d->defaultProfile);
    }
    return d->defaultProfile;
}

QWebEngineProfile *ProfileManager::oblivionProfile()
{
    if (d->oblivionProfile == nullptr) {
        //Create new in-memory profile
        d->oblivionProfile = new QWebEngineProfile();
        d->oblivionProfile->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
        d->oblivionProfile->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
        d->oblivionProfile->settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);

        UrlInterceptor* interceptor = new UrlInterceptor(d->oblivionProfile);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
        //Only on Qt 5.13 or higher
        d->defaultProfile->setUrlRequestInterceptor(interceptor);
        d->defaultProfile->setNotificationPresenter(&ProfileManager::notificationPresenter);
#else
        d->defaultProfile->setRequestInterceptor(interceptor);
#endif

        thewebSchemeHandler* schemeHandler = new thewebSchemeHandler({{"oblivion", true}, {"profile", QVariant::fromValue(d->oblivionProfile)}});
        d->oblivionProfile->installUrlSchemeHandler("theweb", schemeHandler);

        //Edit the current user agent and insert theWeb before QtWebEngine
        QString userAgent = d->oblivionProfile->httpUserAgent();
        if (userAgent.contains("QtWebEngine/")) {
            userAgent.insert(userAgent.indexOf("QtWebEngine/"), "theWeb/15.0 ");
            d->oblivionProfile->setHttpUserAgent(userAgent);
        }

        DownloadManager::registerProfile(d->oblivionProfile);
    }
    return d->oblivionProfile;
}

QList<BarEntry> ProfileManager::entriesForUserInput(QString input, QWebEngineProfile* profile)
{
    QList<BarEntry> entries;
    if (QUrl::fromUserInput(input).isValid()) {
        entries.append({QUrl::fromUserInput(input)});
    }

    QUrl searchUrl;
    searchUrl.setHost("google.com");
    searchUrl.setScheme("https");
    searchUrl.setPath("/search");

    QUrlQuery query;
    query.addQueryItem("q", input);
    searchUrl.setQuery(query);
    entries.append({searchUrl});

    return entries;
}

ProfileManager::ProfileManager(QObject *parent) : QObject(parent)
{

}


#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
void ProfileManager::notificationPresenter(QWebEngineNotificationPtr notification) {
    tNotification* n = nullptr;
    /*for (tNotification* key : d->notificationsMap.keys()) {
        if (d->notificationsMap.value(key)->matches(notification.get())) {
            //Update this notification instead
            n = key;
        }
    }*/

    if (n == nullptr) n = new tNotification();
    n->setSummary(notification->title());
    n->setText(notification->message() + QStringLiteral("\n- ") + tr("from %1").arg(notification->origin().host()));
//    n->insertAction("activate", tr("Activate"));
//    n->insertAction("stop", tr("Stop Notifications"));
//    n->insertAction("configure", tr("Configure Notifications"));
//    connect(n, &tNotification::actionClicked, [=](QString key) {
//        if (key == "activate") {
//            notification->show();
//        }
//    });
    n->post(false);

    notification->show();

//    QWebEngineNotificationPtr insertNotification;
//    insertNotification.swap(notification);
//    d->notificationsMap.insert(n, insertNotification);
}
#endif
