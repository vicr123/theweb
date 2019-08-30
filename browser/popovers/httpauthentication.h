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
#ifndef HTTPAUTHENTICATION_H
#define HTTPAUTHENTICATION_H

#include <QWidget>
#include <QAuthenticator>

namespace Ui {
    class HttpAuthentication;
}

class HttpAuthentication : public QWidget
{
        Q_OBJECT

    public:
        explicit HttpAuthentication(QUrl url, QAuthenticator* authenticator, QWidget *parent = nullptr);
        ~HttpAuthentication();

    signals:
        void accept();
        void reject();

    private slots:
        void on_backButton_clicked();

        void on_okButton_clicked();

    private:
        Ui::HttpAuthentication *ui;
        QAuthenticator* authenticator;
};

#endif // HTTPAUTHENTICATION_H
