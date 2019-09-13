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
#include "singleinstancemanager.h"

#include <QApplication>
#include <QStandardPaths>
#include <QLockFile>
#include <QLocalServer>
#include <QLocalSocket>
#include <QJsonObject>
#include <QJsonDocument>

#define Q_OS_MAC

struct SingleInstanceManagerPrivate {
    QLockFile* lock;
    QLocalServer* server = nullptr;

    QString serverName(qint64 pid) {
        return QStringLiteral("com.vicr123.theWeb.Application.SingleInstance-%1").arg(pid);
    }

    bool shouldRun = true;
};

SingleInstanceManager::SingleInstanceManager(QObject *parent) : QObject(parent)
{
    d = new SingleInstanceManagerPrivate();

#ifndef Q_OS_MAC
    //Attempt to lock the lock file
    d->lock = new QLockFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/run.lock");
    d->lock->setStaleLockTime(0);
    if (!d->lock->tryLock()) {
        d->shouldRun = false;
    }

    //Remove any stale servers
    QLocalServer::removeServer(d->serverName(QApplication::applicationPid()));

    //Start up a local server
    d->server = new QLocalServer();
    d->server->setSocketOptions(QLocalServer::UserAccessOption);
    connect(d->server, &QLocalServer::newConnection, this, [=] {
        QLocalSocket* socket = d->server->nextPendingConnection();
        connect(socket, &QLocalSocket::disconnected, this, [=] {
            QVariantMap options = QJsonDocument::fromBinaryData(socket->readAll()).object().toVariantMap();
            emit optionsRequested(options);
            socket->deleteLater();
        });
    });
    d->server->listen(d->serverName(QApplication::applicationPid()));
#endif
}

SingleInstanceManager::~SingleInstanceManager()
{
#ifndef Q_OS_MAC
    if (d->server != nullptr) {
        d->server->close();
        d->server->deleteLater();
    }

    if (d->lock->isLocked()) d->lock->unlock();
#endif

    delete d;
}

bool SingleInstanceManager::shouldRun()
{
#ifdef Q_OS_MAC
    return true;
#else
    return d->shouldRun;
#endif
}

void SingleInstanceManager::handOverControl(QVariantMap options)
{
#ifndef Q_OS_MAC
    //Talk to the server
    qint64 pid;
    d->lock->getLockInfo(&pid, nullptr, nullptr);

    QLocalSocket socket;
    socket.connectToServer(d->serverName(pid));
    socket.waitForConnected();
    socket.write(QJsonDocument(QJsonObject::fromVariantMap(options)).toBinaryData());
    socket.waitForBytesWritten();
    socket.close();
#endif
}
