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
#ifndef FEATUREMANAGER_H
#define FEATUREMANAGER_H

#include <QObject>

class QWebEngineProfile;
struct FeatureManagerPrivate;
class FeatureManager : public QObject
{
        Q_OBJECT
    public:
        static FeatureManager* managerFor(QWebEngineProfile* profile);

        enum Feature {
            Unknown = -1,
            Location = 0,
            Camera,
            Microphone,
            LastFeature
        };

        enum FeatureAllowed {
            Allow = 0,
            Ask,
            Deny,
        };

        FeatureAllowed isFeatureAllowed(QUrl originUrl, Feature feature);
        void setFeatureAllowed(QUrl originUrl, Feature feature, FeatureAllowed allowed);

    signals:

    public slots:

    private:
        explicit FeatureManager(QWebEngineProfile* profile);

        int originSiteId(QUrl site);
        QString featureTable(Feature feature);

        FeatureManagerPrivate* d;
};

#endif // FEATUREMANAGER_H
