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
#ifndef SECURITYCHUNK_H
#define SECURITYCHUNK_H

#include <QWidget>
#include <QSslCertificate>

namespace Ui {
    class SecurityChunk;
}

struct SecurityChunkPrivate;
class SecurityChunk : public QWidget
{
        Q_OBJECT

    public:
        explicit SecurityChunk(QWidget *parent = nullptr);
        ~SecurityChunk();

        void setCurrentCertificate(QSslCertificate certificate);

    signals:
        void resized();

    private:
        Ui::SecurityChunk *ui;
        SecurityChunkPrivate* d;

        void resizeEvent(QResizeEvent *event);
};

#endif // SECURITYCHUNK_H
