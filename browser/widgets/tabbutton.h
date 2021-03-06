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
#ifndef TABBUTTON_H
#define TABBUTTON_H

#include <QPushButton>

struct TabButtonPrivate;
class TabButton : public QPushButton
{
        Q_OBJECT
    public:
        explicit TabButton(QWidget *parent = nullptr);
        ~TabButton();

        QSize sizeHint() const;

        QColor color();
        QColor foregroundColor();

        void setLoadProgress(int progress, bool show);

    signals:
        void paletteUpdated();

    public slots:
        void setIcon(QIcon icon);

    private:
        TabButtonPrivate* d;

        void paintEvent(QPaintEvent* event);
};

#endif // TABBUTTON_H
