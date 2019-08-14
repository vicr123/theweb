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
#include "certificateerrorpane.h"
#include "ui_certificateerrorpane.h"

#include <QWebEngineCertificateError>
#include <the-libs_global.h>

struct CertificateErrorPanePrivate {
    QEventLoop loop;
};

CertificateErrorPane::CertificateErrorPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CertificateErrorPane)
{
    ui->setupUi(this);
    d = new CertificateErrorPanePrivate();

    this->setMaximumWidth(SC_DPI(700));
    connect(this, &CertificateErrorPane::returnToBrowser, &d->loop, &QEventLoop::quit);
}

CertificateErrorPane::~CertificateErrorPane()
{
    delete ui;
    delete d;
}

bool CertificateErrorPane::resolveError(const QWebEngineCertificateError& certificateError)
{
    ui->primaryMessage->setText(tr("Looks like we can't get to the real %1. For your security, we've stopped this connection.").arg("<b>" + certificateError.url().host() + "</b>"));
    ui->hstsWarning->setVisible(!certificateError.isOverridable());
    ui->continueButton->setVisible(certificateError.isOverridable());

    if (certificateError.isOverridable()) {
        ui->continueExplanation->setText(tr("Only continue to this page if you understand the risks involved. Don't enter any sensitive info, because attackers may be watching."));
    } else {
        ui->continueExplanation->setText(tr("This page enforces HSTS, so you may not continue to this page at this time."));
    }

    ui->whatsWrongButton->setChecked(false);
    ui->extraInformationPane->setVisible(false);

    QString reason;
    switch (certificateError.error()) {
        case QWebEngineCertificateError::CertificateDateInvalid:
            reason = tr("presented us with an expired certificate");
            break;
        case QWebEngineCertificateError::CertificateCommonNameInvalid:
            reason = tr("presented us with a certificate for another website");
            break;
        case QWebEngineCertificateError::CertificateContainsErrors:
        case QWebEngineCertificateError::CertificateInvalid:
            reason = tr("presented us with an invalid certificate");
            break;
        case QWebEngineCertificateError::CertificateRevoked:
            reason = tr("presented us with a revoked certificate");
            ui->continueExplanation->setText(tr("The site presented a revoked certificate, so you may not continue to this page at this time."));
            ui->hstsWarning->setVisible(false);
            break;
        case QWebEngineCertificateError::CertificateAuthorityInvalid:
            reason = tr("presented us with an untrusted certificate; the issuer of this certificate is not in your system's CA store");
            break;
        default:
            reason = "\"" + certificateError.errorDescription() + "\"";
    }
    ui->extraInfoLabel->setText(tr("We weren't able to verify that you're connecting to the real %1 because it %2.").arg("<b>" + certificateError.url().host() + "</b>").arg(reason));


    emit showPane();
    int result = d->loop.exec();
    emit returnToBrowser();
    return result;
}

QSize CertificateErrorPane::sizeHint() const
{
    QSize size = QWidget::sizeHint();
    size.setWidth(SC_DPI(700));
    return size;
}

void CertificateErrorPane::on_backButton_clicked()
{
    QMetaObject::invokeMethod(this, "goBack", Qt::QueuedConnection);
    //emit goBack();
    d->loop.exit(0);
}

void CertificateErrorPane::on_whatsWrongButton_toggled(bool checked)
{
    ui->extraInformationPane->setVisible(checked);
}

void CertificateErrorPane::on_continueButton_clicked()
{
    d->loop.exit(1);
}
