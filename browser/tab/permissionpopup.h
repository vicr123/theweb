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
#ifndef PERMISSIONPOPUP_H
#define PERMISSIONPOPUP_H

#include <QWidget>

namespace Ui {
    class PermissionPopup;
}

struct PermissionPopupPrivate;
class PermissionPopup : public QWidget
{
        Q_OBJECT

    public:
        enum PermissionType {
            Unknown,
            FullScreen,
            Geolocation,
            Webcam,
            Microphone,
            WebcamAndMicrophone,
            ScreenRecord,
            ScreenAudioRecord,
            Notifications,
            MouseLock,
            RenderProcessTerminate
        };

        explicit PermissionPopup(QUrl originUrl, PermissionType type, QWidget *parent = nullptr);
        ~PermissionPopup();

        PermissionType type();

    signals:
        void allow();
        void deny();

    private slots:
        void on_allowButton_clicked();

        void on_denyButton_clicked();

        void on_dismissButton_clicked();

    private:
        Ui::PermissionPopup *ui;
        PermissionPopupPrivate* d;
};

#endif // PERMISSIONPOPUP_H
