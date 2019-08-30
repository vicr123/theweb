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
#include "httpauthentication.h"
#include "ui_httpauthentication.h"

#include <QUrl>

HttpAuthentication::HttpAuthentication(QUrl url, QAuthenticator* authenticator, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HttpAuthentication)
{
    ui->setupUi(this);

#ifdef Q_OS_MAC
    ui->okButton->setVisible(false);
    ui->backButton->setVisible(false);
#else
    ui->macPane->setVisible(false);
#endif

    if (url.scheme() == "https") {
        ui->insecureOriginWarning->setVisible(false);
    }

    ui->messageLabel->setText(tr("Log in to %1").arg(url.host()));
    ui->realmLabel->setText(tr("Server Realm: %1").arg(authenticator->realm()));
    this->authenticator = authenticator;
}

HttpAuthentication::~HttpAuthentication()
{
    delete ui;
}

void HttpAuthentication::on_backButton_clicked()
{
    *authenticator = QAuthenticator();
    emit reject();
}

void HttpAuthentication::on_okButton_clicked()
{
    authenticator->setUser(ui->usernameBox->text());
    authenticator->setPassword(ui->passwordBox->text());
    emit accept();
}
