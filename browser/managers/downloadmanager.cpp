#include "downloadmanager.h"

#include <QWebEngineProfile>
#include <QWebEngineDownloadItem>
#include <QTemporaryFile>
#include <QUrl>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>

struct DownloadManagerItemPrivate {
    QWebEngineDownloadItem* item;
    QString temporaryFilePath;
    QString finalFilePath;

    DownloadManagerItem::State state;
    bool completedInternalActions = false;

    QSettings settings;
};

struct DownloadManagerPrivate {
    DownloadManager* instance = new DownloadManager();

    QList<DownloadManagerItem*> downloads;
};

DownloadManagerPrivate* DownloadManager::d = new DownloadManagerPrivate();

void DownloadManager::registerProfile(QWebEngineProfile* profile)
{
    connect(profile, &QWebEngineProfile::downloadRequested, d->instance, &DownloadManager::downloadRequested);
}

QList<DownloadManagerItem*> DownloadManager::getDownloads()
{
    return d->downloads;
}

DownloadManager* DownloadManager::instance()
{
    return d->instance;
}

void DownloadManager::downloadRequested(QWebEngineDownloadItem* item)
{
    DownloadManagerItem* dItem = new DownloadManagerItem(item);
    d->downloads.append(dItem);

    connect(dItem, &DownloadManagerItem::downloading, this, [=] {
        emit downloadAdded(dItem);
    });
    dItem->start();
}

DownloadManager::DownloadManager(QObject *parent) : QObject(parent)
{

}


DownloadManagerItem::~DownloadManagerItem()
{
    delete d;
}

QWebEnginePage* DownloadManagerItem::page()
{
    return d->item->page();
}

qint64 DownloadManagerItem::receivedBytes()
{
    return d->item->receivedBytes();
}

qint64 DownloadManagerItem::totalBytes()
{
    return d->item->totalBytes();
}

DownloadManagerItem::State DownloadManagerItem::state()
{
    return d->state;
}

QString DownloadManagerItem::displayFileName()
{
    return QFileInfo(d->finalFilePath).completeBaseName();
}

void DownloadManagerItem::cancel()
{
    d->item->cancel();
}

void DownloadManagerItem::pause()
{
    d->item->pause();
}

void DownloadManagerItem::resume()
{
    d->item->resume();
}

DownloadManagerItem::DownloadManagerItem(QWebEngineDownloadItem* parentItem)
{
    d = new DownloadManagerItemPrivate();
    d->item = parentItem;

    connect(d->item, &QWebEngineDownloadItem::destroyed, this, &DownloadManagerItem::deleteLater);
    connect(this, &DownloadManagerItem::destroyed, d->item, &QWebEngineDownloadItem::deleteLater);

    connect(d->item, &QWebEngineDownloadItem::downloadProgress, this, &DownloadManagerItem::progressChanged);
    connect(d->item, &QWebEngineDownloadItem::stateChanged, this, [=](QWebEngineDownloadItem::DownloadState state) {
        switch (state) {
            case QWebEngineDownloadItem::DownloadRequested:
                d->state = DownloadRequested;
                break;
            case QWebEngineDownloadItem::DownloadInProgress:
                d->state = DownloadInProgress;
                break;
            case QWebEngineDownloadItem::DownloadCompleted:
                if (d->completedInternalActions) {
                    d->state = DownloadCompleted;
                } else {
                    d->state = DownloadProcessing;
                }
                break;
            case QWebEngineDownloadItem::DownloadCancelled:
                d->state = DownloadCancelled;
                break;
            case QWebEngineDownloadItem::DownloadInterrupted:
                d->state = DownloadInterrupted;
                break;
        }

        emit stateChanged();
    });
    connect(d->item, &QWebEngineDownloadItem::finished, this, [=] {
        //Move everything into place
        QFile::rename(d->temporaryFilePath, d->finalFilePath);
        d->completedInternalActions = true;
        d->state = DownloadCompleted;
        emit stateChanged();
    });
}

void DownloadManagerItem::start()
{
    QString fileName = d->item->url().fileName();

    QTemporaryFile tempFile;
    if (!tempFile.open()) {
        //Fail
        return;
    }
    d->temporaryFilePath = tempFile.fileName();
    tempFile.remove();


    if (d->settings.value("download/askForFileName", false).toBool()) {
        //Ask for a filename
    } else {
        //Immediately download to a file
        QDir downloadDir(d->settings.value("download/defaultDir", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)).toString());
        if (!downloadDir.exists()) {
            //Fail
            return;
        }

        int i = 0;
        QString endFile = downloadDir.absoluteFilePath(fileName);
        while (QFile(endFile).exists()) {
            endFile = QString("%1 (%2)%3").arg(downloadDir.absoluteFilePath(fileName)).arg(i);
            i++;
        }

        d->finalFilePath = endFile;
    }

    d->item->setPath(d->temporaryFilePath);
    d->item->accept();

    emit downloading();
}
