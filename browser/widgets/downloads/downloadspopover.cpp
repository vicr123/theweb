#include "downloadspopover.h"
#include "ui_downloadspopover.h"

#include "managers/downloadmanager.h"
#include "downloadspopoveritem.h"

DownloadsPopover::DownloadsPopover(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadsPopover)
{
    ui->setupUi(this);

    for (DownloadManagerItem* item : DownloadManager::getDownloads()) {
        DownloadsPopoverItem* w = new DownloadsPopoverItem(item, this);
        ui->downloadsList->addWidget(w);
    }
}

DownloadsPopover::~DownloadsPopover()
{
    delete ui;
}

void DownloadsPopover::on_backButton_clicked()
{
    emit dismiss();
}
