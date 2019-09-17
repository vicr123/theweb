#ifndef DOWNLOADSPOPOVERITEM_H
#define DOWNLOADSPOPOVERITEM_H

#include <QWidget>

namespace Ui {
    class DownloadsPopoverItem;
}

class DownloadManagerItem;
struct DownloadsPopoverItemPrivate;
class DownloadsPopoverItem : public QWidget
{
        Q_OBJECT

    public:
        explicit DownloadsPopoverItem(DownloadManagerItem* item, QWidget *parent = nullptr);
        ~DownloadsPopoverItem();

    public slots:
        void updateData();

    private slots:
        void on_cancelButton_clicked();

        void on_pauseButton_clicked();

    private:
        Ui::DownloadsPopoverItem *ui;
        DownloadsPopoverItemPrivate* d;
};

#endif // DOWNLOADSPOPOVERITEM_H
