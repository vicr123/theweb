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
#ifndef SAFEBROWSING_H
#define SAFEBROWSING_H

#include <QObject>
#include <tpromise.h>

struct SafeBrowsingPrivate;
class SafeBrowsing : public QObject
{
        Q_OBJECT
    public:
        explicit SafeBrowsing(QObject *parent = nullptr);
        ~SafeBrowsing();

        static void initialize();
        static tPromise<QString>* checkUrl(QUrl url);
        static QString checkUrlLocally(QUrl url);

        static QString canonicalizeUrl(QUrl url);
        static QStringList suffixPrefixExpressionsForUrl(QString canonicalizedUrl);

    signals:

    public slots:

    private:
        static SafeBrowsingPrivate* d;

        void updateLists();
        void updateListForThreatType(QString threatType);
};

#endif // SAFEBROWSING_H
