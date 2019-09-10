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
#include "historymanager.h"

#include <QMap>
#include <QWebEngineProfile>

#include <QDebug>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>

struct HistoryManagerPrivate {
    static QMap<QWebEngineProfile*, HistoryManager*> managers;

    QWebEngineProfile* profile;
    QSqlDatabase db;
};

QMap<QWebEngineProfile*, HistoryManager*> HistoryManagerPrivate::managers;

HistoryManager* HistoryManager::managerFor(QWebEngineProfile* profile)
{
    HistoryManager* mgr = HistoryManagerPrivate::managers.value(profile, nullptr);
    if (mgr == nullptr) {
        mgr = new HistoryManager(profile);
        HistoryManagerPrivate::managers.insert(profile, mgr);
    }
    return mgr;
}

void HistoryManager::addEntry(QUrl url, QString title) {
    if (url.scheme() == "theweb") return; //Ignore internal pages
    QString urlText = url.toString(QUrl::FullyEncoded);
    qint64 dateTime = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();

    QSqlQuery query(d->db);
    query.prepare("INSERT INTO history(url, title, date) VALUES(:url, :title, :date)");
    query.bindValue(":url", urlText);
    query.bindValue(":title", title);
    query.bindValue(":date", dateTime);
    query.exec();
}

QList<HistoryManager::HistoryEntry> HistoryManager::historyEntries(QDateTime latest, QDateTime earliest)
{
    QList<HistoryManager::HistoryEntry> entries;

    QSqlQuery query(d->db);
    query.prepare("SELECT * FROM history WHERE date <= :latest AND date >= :earliest ORDER BY date DESC");
    query.bindValue(":latest", latest.toMSecsSinceEpoch());
    query.bindValue(":earliest", earliest.toMSecsSinceEpoch());
    query.exec();

    while (query.next()) {
        HistoryEntry entry;
        entry.url = QUrl(query.value("url").toString());
        entry.accessTime = QDateTime::fromMSecsSinceEpoch(query.value("date").toLongLong());
        entry.pageTitle = query.value("title").toString();
        entries.append(entry);
    }

    return entries;
}

void HistoryManager::clearHistory()
{
    d->db.exec("DELETE FROM history");
}

void HistoryManager::clearHistory(QDateTime latest, QDateTime earliest)
{
    QSqlQuery query(d->db);
    query.prepare("DELETE FROM history WHERE date <= :latest AND date >= :earliest");
    query.bindValue(":latest", latest.toMSecsSinceEpoch());
    query.bindValue(":earliest", earliest.toMSecsSinceEpoch());
    query.exec();
}

QList<HistoryManager::HistoryEntry> HistoryManager::searchHistory(QString queryString) {
    QList<HistoryManager::HistoryEntry> entries;

    QSqlQuery query(d->db);
    query.prepare("SELECT DISTINCT * FROM history WHERE url LIKE '%'||:query||'%' OR title LIKE '%'||:query||'%' ORDER BY date DESC");
    query.bindValue(":query", queryString);
    query.exec();

    while (query.next()) {
        HistoryEntry entry;
        entry.url = QUrl(query.value("url").toString());
        entry.accessTime = QDateTime::fromMSecsSinceEpoch(query.value("date").toLongLong());
        entry.pageTitle = query.value("title").toString();
        entries.append(entry);
    }

    return entries;
}

HistoryManager::HistoryManager(QWebEngineProfile* profile) : QObject(nullptr)
{
    d = new HistoryManagerPrivate();
    d->profile = profile;

    d->db = QSqlDatabase::addDatabase("QSQLITE", "db_history_" + profile->storageName());
    d->db.setDatabaseName(QDir::cleanPath(profile->persistentStoragePath() + "/history"));
    if (!d->db.open()) {
        qDebug() << "Database error!";
        return;
    }

    //Create tables if required
    d->db.exec("CREATE TABLE IF NOT EXISTS history(id INTEGER PRIMARY KEY AUTOINCREMENT, url TEXT, title TEXT, date INTEGER)");
}
