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
#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <QObject>
#include <QUrl>
#include <QDateTime>

class QWebEngineProfile;
struct HistoryManagerPrivate;
class HistoryManager : public QObject
{
        Q_OBJECT
    public:
        static HistoryManager* managerFor(QWebEngineProfile* profile);

        struct HistoryEntry {
            QUrl url;
            QDateTime accessTime;
            QString pageTitle;
        };

    signals:

    public slots:
        void addEntry(QUrl url, QString title);
        QList<HistoryEntry> historyEntries(QDateTime latest = QDateTime::currentDateTimeUtc(), QDateTime earliest = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC));
        void clearHistory();
        void clearHistory(QDateTime latest, QDateTime earliest = QDateTime::fromMSecsSinceEpoch(0, Qt::UTC));
        QList<HistoryEntry> searchHistory(QString queryString);

    private:
        explicit HistoryManager(QWebEngineProfile* profile);
        HistoryManagerPrivate* d;
};

#endif // HISTORYMANAGER_H
