#include "devtoolsheader.h"
#include "ui_devtoolsheader.h"

#include "managers/iconmanager.h"

DevtoolsHeader::DevtoolsHeader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DevtoolsHeader)
{
    ui->setupUi(this);

    ui->closeButton->setIcon(IconManager::getIcon("window-close", this->palette().color(QPalette::WindowText), ui->closeButton->iconSize()));
}

DevtoolsHeader::~DevtoolsHeader()
{
    delete ui;
}

void DevtoolsHeader::on_closeButton_clicked()
{
    emit closeDevtools();
}
