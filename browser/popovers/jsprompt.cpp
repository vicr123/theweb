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
#include "jsprompt.h"
#include "ui_jsprompt.h"

JsPrompt::JsPrompt(QString message, QString defaultResponse, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JsPrompt)
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
    ui->responseBox->setText(defaultResponse);
}

JsPrompt::~JsPrompt()
{
    delete ui;
}

void JsPrompt::on_okButton_clicked()
{
    emit accept(ui->responseBox->text());
}

void JsPrompt::on_noButton_clicked()
{
    emit reject();
}
