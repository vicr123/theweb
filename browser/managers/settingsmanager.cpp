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
#include "settingsmanager.h"

#include <QSettings>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QUrl>

struct SettingsManagerPrivate {
    QSettings settings;
    SettingsManager* instance = new SettingsManager();
};

SettingsManagerPrivate* SettingsManager::d = nullptr;

QByteArray SettingsManager::getJson()
{
    QJsonObject rootObj;
    for (QString key : d->settings.allKeys()) {
        QVariant settingValue = d->settings.value(key);
        switch (settingValue.type()) {
            case QVariant::Bool:
                rootObj.insert(key, settingValue.toBool());
                break;
            case QVariant::Int:
                rootObj.insert(key, settingValue.toInt());
                break;
            case QVariant::StringList:
                rootObj.insert(key, QJsonArray::fromStringList(settingValue.toStringList()));
                break;
            case QVariant::String: {
                QString value = settingValue.toString();
                if (value == "true") {
                    rootObj.insert(key, true);
                } else if (value == "false") {
                    rootObj.insert(key, false);
                } else {
                    rootObj.insert(key, settingValue.toString());
                }
                break;
            }
            default:
                qDebug() << "Setting" << key << "has unknown type!";
                rootObj.insert(key, settingValue.toString());
        }
    }

    //Also return Safe Browsing support
    #ifdef SAFE_BROWSING_API_KEY
        rootObj.insert("privacy/gsbAvailable", true);
    #else
        rootObj.insert("privacy/gsbAvailable", false);
    #endif

    return QJsonDocument(rootObj).toJson();
}

void SettingsManager::set(QString key, QVariant value)
{
    d->settings.setValue(key, value);
    d->settings.sync();
}

void SettingsManager::resetSettings()
{
    d->settings.clear();
    d->settings.sync();
}

void SettingsManager::initialize()
{
    Q_ASSERT(d == nullptr);
    d = new SettingsManagerPrivate();
}

QUrl SettingsManager::getHomePage()
{
    if (d->settings.value("startup/action", "newtab").toString() == "newtab") {
        return QUrl("theweb://newtab");
    } else {
        return QUrl::fromUserInput(d->settings.value("startup/home", "https://vicr123.com/").toString());
    }
}

SettingsManager::SettingsManager(QObject *parent) : QObject(parent)
{

}

