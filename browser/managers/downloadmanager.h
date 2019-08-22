#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>

struct DownloadManagerPrivate;
struct DownloadManagerItemPrivate;

class QWebEngineProfile;
class QWebEngineDownloadItem;
class QWebEnginePage;

class DownloadManagerItem : public QObject
{
        Q_OBJECT
    public:
        ~DownloadManagerItem();

        enum State {
            DownloadRequested,
            DownloadInProgress,
            DownloadProcessing,
            DownloadCompleted,
            DownloadCancelled,
            DownloadInterrupted
        };

        QWebEnginePage* page();
        qint64 receivedBytes();
        qint64 totalBytes();

        State state();

        QString displayFileName();

    public slots:
        void cancel();
        void pause();
        void resume();

    signals:
        void downloading();
        void progressChanged(qint64 received, qint64 total);
        void stateChanged();

    protected:
        friend class DownloadManager;
        explicit DownloadManagerItem(QWebEngineDownloadItem* parentItem);

        void start();

    private:
        DownloadManagerItemPrivate* d;
};

class DownloadManager : public QObject
{
        Q_OBJECT
    public:
        static void registerProfile(QWebEngineProfile* profile);
        static QList<DownloadManagerItem*> getDownloads();
        static DownloadManager* instance();

    signals:
        void downloadAdded(DownloadManagerItem* item);

    public slots:

    private slots:
        void downloadRequested(QWebEngineDownloadItem* item);

    protected:
        friend struct DownloadManagerPrivate;
        explicit DownloadManager(QObject *parent = nullptr);

    private:
        void ensureInstance();
        static DownloadManagerPrivate* d;
};

#endif // DOWNLOADMANAGER_H
