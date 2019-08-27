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
#include "mainwindow.h"
#include <QDir>
#include <tapplication.h>
#include <QWebEngineUrlScheme>
#include "managers/settingsmanager.h"

int main(int argc, char *argv[])
{
#ifdef Q_OS_MAC
    if (QDir(tApplication::macOSBundlePath() + "/Contents/Frameworks/QtWebEngineCore.framework").exists()) {
        qputenv("QTWEBENGINEPROCESS_PATH", QString(tApplication::macOSBundlePath() + "/Contents/Frameworks/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess").toUtf8());
    }
#endif

    QWebEngineUrlScheme scheme("theweb");
    scheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
    scheme.setFlags(QWebEngineUrlScheme::LocalScheme | QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::LocalAccessAllowed | QWebEngineUrlScheme::ContentSecurityPolicyIgnored);
    QWebEngineUrlScheme::registerScheme(scheme);

    tApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    tApplication a(argc, argv);
    Q_INIT_RESOURCE(scheme);

#ifdef Q_OS_WIN
    QFont defaultFont("Segoe UI", 9);
    a.setFont(defaultFont);
#endif

    if (QDir("/usr/share/theweb").exists()) {
        a.setShareDir("/usr/share/theweb");
    } else if (QDir(QDir::cleanPath(QApplication::applicationDirPath() + "/../share/theweb/")).exists()) {
        a.setShareDir(QDir::cleanPath(QApplication::applicationDirPath() + "/../share/theweb/"));
    }
    a.installTranslators();

    a.setOrganizationName("theSuite");
    a.setOrganizationDomain("");
    a.setApplicationIcon(QIcon::fromTheme("theweb", QIcon(":/icons/theweb.svg")));
    a.setApplicationVersion("0.1");
    a.setGenericName(QApplication::translate("main", "Web Browser"));
    //a.setAboutDialogSplashGraphic(a.aboutDialogSplashGraphicFromSvg(":/icons/aboutsplash.svg"));
    a.setApplicationLicense(tApplication::Gpl3OrLater);
    a.setCopyrightHolder("Victor Tran");
    a.setCopyrightYear("2019");
    #ifdef T_BLUEPRINT_BUILD
        a.setApplicationName("theWeb Blueprint");
    #else
        a.setApplicationName("theWeb");
    #endif

    SettingsManager::initialize();

    MainWindow w;
    w.show();

    return a.exec();
}
