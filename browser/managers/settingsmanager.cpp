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
            case QVariant::String:
                rootObj.insert(key, settingValue.toString());
                break;
            default:
                qDebug() << "Setting" << key << "has unknown type!";
                rootObj.insert(key, settingValue.toString());
        }
    }
    return QJsonDocument(rootObj).toJson();
}

void SettingsManager::set(QString key, QVariant value)
{
    d->settings.setValue(key, value);
    d->settings.sync();
}

void SettingsManager::initialize()
{
    Q_ASSERT(d == nullptr);
    d = new SettingsManagerPrivate();
}

SettingsManager::SettingsManager(QObject *parent) : QObject(parent)
{

}

