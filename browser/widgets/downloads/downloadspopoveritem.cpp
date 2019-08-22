#include "downloadspopoveritem.h"
#include "ui_downloadspopoveritem.h"

#include "managers/downloadmanager.h"

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

    connect(d->item, &DownloadManagerItem::progressChanged, this, &DownloadsPopoverItem::updateData);
    connect(d->item, &DownloadManagerItem::stateChanged, this, &DownloadsPopoverItem::updateData);
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

    QLocale locale;
    switch (d->item->state()) {
        case DownloadManagerItem::DownloadRequested:
            ui->statusLabel->setText(tr("Waiting"));
            ui->progressBar->setMaximum(0);
            ui->progressBar->setVisible(true);
            ui->cancelButton->setVisible(true);
            break;
        case DownloadManagerItem::DownloadInProgress:
            ui->progressBar->setMaximum(d->item->totalBytes());
            ui->progressBar->setValue(d->item->receivedBytes());
            ui->statusLabel->setText(tr("%1 of %2").arg(locale.formattedDataSize(d->item->receivedBytes())).arg(locale.formattedDataSize(d->item->totalBytes())));
            ui->progressBar->setVisible(true);
            ui->cancelButton->setVisible(true);
            break;
        case DownloadManagerItem::DownloadProcessing:
            ui->statusLabel->setText(tr("Finishing up"));
            ui->progressBar->setMaximum(0);
            ui->progressBar->setVisible(true);
            ui->cancelButton->setVisible(false);
            break;
        case DownloadManagerItem::DownloadCompleted:
            ui->statusLabel->setText(tr("Done"));
            ui->progressBar->setVisible(false);
            ui->cancelButton->setVisible(false);
            break;
        case DownloadManagerItem::DownloadCancelled:
            ui->statusLabel->setText(tr("Cancelled"));
            ui->progressBar->setVisible(false);
            ui->cancelButton->setVisible(false);
            break;
        case DownloadManagerItem::DownloadInterrupted:
            ui->statusLabel->setText(tr("Failed"));
            ui->progressBar->setVisible(false);
            ui->cancelButton->setVisible(false);
    }
}

void DownloadsPopoverItem::on_cancelButton_clicked()
{
    d->item->cancel();
}
