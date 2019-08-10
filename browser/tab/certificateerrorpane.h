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
#ifndef CERTIFICATEERRORPANE_H
#define CERTIFICATEERRORPANE_H

#include <QWidget>

namespace Ui {
    class CertificateErrorPane;
}

class QWebEngineCertificateError;
struct CertificateErrorPanePrivate;
class CertificateErrorPane : public QWidget
{
        Q_OBJECT

    public:
        explicit CertificateErrorPane(QWidget *parent = nullptr);
        ~CertificateErrorPane();

        bool resolveError(const QWebEngineCertificateError &certificateError);

        QSize sizeHint() const;

    signals:
        void showPane();
        void goBack();
        void returnToBrowser();

    private slots:
        void on_backButton_clicked();

        void on_whatsWrongButton_toggled(bool checked);

        void on_continueButton_clicked();

    private:
        Ui::CertificateErrorPane *ui;
        CertificateErrorPanePrivate* d;
};

#endif // CERTIFICATEERRORPANE_H
