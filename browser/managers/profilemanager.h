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
#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include <QObject>
#include <QWebEngineProfile>
#include <QUrl>

struct ProfileManagerPrivate;
class ProfileManager : public QObject
{
        Q_OBJECT
    public:
        static QWebEngineProfile* defaultProfile();
        static QWebEngineProfile* oblivionProfile();

    signals:

    public slots:

    private:
        explicit ProfileManager(QObject *parent = nullptr);
        static void registerProfile(QWebEngineProfile* profile);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
        typedef std::unique_ptr<QWebEngineNotification> QWebEngineNotificationPtr;
        static void notificationPresenter(QWebEngineNotificationPtr notification);
#endif

        static ProfileManagerPrivate* d;
};

Q_DECLARE_METATYPE(QWebEngineProfile*);

#endif // PROFILEMANAGER_H
