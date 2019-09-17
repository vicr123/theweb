#include "downloadspopoveritem.h"
#include "ui_downloadspopoveritem.h"

#include <the-libs_global.h>
#include <QFileIconProvider>
#include "managers/downloadmanager.h"
#include "managers/iconmanager.h"

struct DownloadsPopoverItemPrivate {
    DownloadManagerItem* item;
};

DownloadsPopoverItem::DownloadsPopoverItem(DownloadManagerItem* item, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadsPopoverItem)
{
    ui->setupUi(this);
    d = new DownloadsPopoverItemPrivate();
    d->item = item;

    ui->cancelButton->setIcon(IconManager::getIcon("media-playback-stop", this->palette(), SC_DPI_T(QSize(16, 16), QSize)));

    connect(d->item, &DownloadManagerItem::progressChanged, this, &DownloadsPopoverItem::updateData);
    connect(d->item, &DownloadManagerItem::stateChanged, this, &DownloadsPopoverItem::updateData);
    connect(d->item, &DownloadManagerItem::isPausedChanged, this, &DownloadsPopoverItem::updateData);
    updateData();
}

DownloadsPopoverItem::~DownloadsPopoverItem()
{
    delete ui;
    delete d;
}

void DownloadsPopoverItem::updateData()
{
    ui->filenameLabel->setText(d->item->displayFileName().toUpper());

    if (d->item->isPaused()) {
        ui->pauseButton->setIcon(IconManager::getIcon("media-playback-start", this->palette(), SC_DPI_T(QSize(16, 16), QSize)));
    } else {
        ui->pauseButton->setIcon(IconManager::getIcon("media-playback-pause", this->palette(), SC_DPI_T(QSize(16, 16), QSize)));
    }

    QFileIconProvider ic;
    QIcon icon = ic.icon(QFileInfo(d->item->fileName()));
    if (icon.isNull()) icon = ic.icon(QFileIconProvider::File);
    if (icon.isNull()) icon = QIcon::fromTheme("unknown");
    ui->fileIcon->setPixmap(icon.pixmap(SC_DPI_T(QSize(32, 32), QSize)));

    QLocale locale;
    switch (d->item->state()) {
        case DownloadManagerItem::DownloadRequested:
            ui->statusLabel->setText(tr("Waiting"));
            ui->progressBar->setMaximum(0);
            ui->progressBar->setVisible(true);
            ui->cancelButton->setVisible(true);
            ui->pauseButton->setVisible(true);
            ui->pauseButton->setEnabled(false);
            break;
        case DownloadManagerItem::DownloadInProgress:
            ui->progressBar->setMaximum(d->item->totalBytes());
            ui->progressBar->setValue(d->item->receivedBytes());
            ui->statusLabel->setText((d->item->isPaused() ? tr("Paused") + " · " : "") + tr("%1 of %2").arg(locale.formattedDataSize(d->item->receivedBytes())).arg(locale.formattedDataSize(d->item->totalBytes())));
            ui->progressBar->setVisible(true);
            ui->cancelButton->setVisible(true);
            ui->pauseButton->setVisible(true);
            ui->pauseButton->setEnabled(true);
            break;
        case DownloadManagerItem::DownloadProcessing:
            ui->statusLabel->setText(tr("Finishing up"));
            ui->progressBar->setMaximum(0);
            ui->progressBar->setVisible(true);
            ui->cancelButton->setVisible(false);
            ui->pauseButton->setVisible(false);
            break;
        case DownloadManagerItem::DownloadCompleted:
            ui->statusLabel->setText(tr("Done"));
            ui->progressBar->setVisible(false);
            ui->cancelButton->setVisible(false);
            ui->pauseButton->setVisible(false);
            break;
        case DownloadManagerItem::DownloadCancelled:
            ui->statusLabel->setText(tr("Cancelled"));
            ui->progressBar->setVisible(false);
            ui->cancelButton->setVisible(false);
            ui->pauseButton->setVisible(false);
            break;
        case DownloadManagerItem::DownloadInterrupted:
            ui->statusLabel->setText(tr("Failed"));
            ui->progressBar->setVisible(false);
            ui->cancelButton->setVisible(false);
            ui->pauseButton->setVisible(false);
    }
}

void DownloadsPopoverItem::on_cancelButton_clicked()
{
    d->item->cancel();
}

void DownloadsPopoverItem::on_pauseButton_clicked()
{
    if (d->item->isPaused()) {
        d->item->resume();
    } else {
        d->item->pause();
    }
}
