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
#include "jsconfirm.h"
#include "ui_jsconfirm.h"

JsConfirm::JsConfirm(QString message, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JsConfirm)
{
    ui->setupUi(this);

#ifdef Q_OS_MAC
    ui->okButton->setVisible(false);
    ui->noButton->setVisible(false);
    ui->backButton->setVisible(false);
#else
    ui->macPane->setVisible(false);
#endif

    ui->messageLabel->setText(message);
}

JsConfirm::~JsConfirm()
{
    delete ui;
}

void JsConfirm::on_okButton_clicked()
{
    emit accept();
}

void JsConfirm::on_noButton_clicked()
{
    emit reject();
}
