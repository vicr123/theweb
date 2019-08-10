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
#include "permissionpopup.h"
#include "ui_permissionpopup.h"

#include <the-libs_global.h>
#include <QUrl>

struct PermissionPopupPrivate {
    PermissionPopup::PermissionType type;
};

PermissionPopup::PermissionPopup(QUrl originUrl, PermissionPopup::PermissionType type, QWidget*parent) :
    QWidget(parent),
    ui(new Ui::PermissionPopup)
{
    ui->setupUi(this);
    d = new PermissionPopupPrivate();
    d->type = type;

    QString host = "<b>" + originUrl.host() + "</b>";
    switch (type) {
        case FullScreen:
            ui->titleLabel->setText(tr("Full Screen"));
            ui->textLabel->setText(tr("%1 is now full screen.").arg(host));
            ui->permissionButtons->setVisible(false);
            ui->iconLabel->setVisible(false);
            break;
        case Geolocation:
            ui->titleLabel->setText(tr("Geolocation"));
            ui->textLabel->setText(tr("Allow %1 to access your physical location?").arg(host));
            ui->iconLabel->setPixmap(QIcon::fromTheme("gps").pixmap(SC_DPI_T(QSize(16, 16), QSize)));
            break;
        case Webcam:
            ui->titleLabel->setText(tr("Camera"));
            ui->textLabel->setText(tr("Allow %1 to access your camera?").arg(host));
            ui->iconLabel->setPixmap(QIcon::fromTheme("camera-photo").pixmap(SC_DPI_T(QSize(16, 16), QSize)));
            break;
        case Microphone:
            ui->titleLabel->setText(tr("Microphone"));
            ui->textLabel->setText(tr("Allow %1 to access your microphone?").arg(host));
            ui->iconLabel->setPixmap(QIcon::fromTheme("mic-on").pixmap(SC_DPI_T(QSize(16, 16), QSize)));
            break;
        case WebcamAndMicrophone:
            ui->titleLabel->setText(tr("Camera and Microphone"));
            ui->textLabel->setText(tr("Allow %1 to access your camera and microphone?").arg(host));
            ui->iconLabel->setPixmap(QIcon::fromTheme("camera-photo").pixmap(SC_DPI_T(QSize(16, 16), QSize)));
            break;
        case ScreenRecord:
        case ScreenAudioRecord:
            ui->titleLabel->setText(tr("Screen Recording"));
            ui->textLabel->setText(tr("Allow %1 to record your screen?").arg(host));
            ui->iconLabel->setPixmap(QIcon::fromTheme("video-display").pixmap(SC_DPI_T(QSize(16, 16), QSize)));
            break;
    }
}

PermissionPopup::~PermissionPopup()
{
    delete ui;
    delete d;
}

PermissionPopup::PermissionType PermissionPopup::type()
{
    return d->type;
}

void PermissionPopup::on_allowButton_clicked()
{
    emit allow();
    this->deleteLater();
}

void PermissionPopup::on_denyButton_clicked()
{
    emit deny();
    this->deleteLater();
}

void PermissionPopup::on_dismissButton_clicked()
{
    emit deny();
    this->deleteLater();
}
