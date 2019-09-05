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
#ifndef BAR_H
#define BAR_H

#include <QLineEdit>

class WebTab;
struct BarPrivate;
class Bar : public QLineEdit
{
        Q_OBJECT
    public:
        explicit Bar(QWidget *parent = nullptr);
        ~Bar();

        void setCurrentTab(WebTab* tab);
        QWidget* getAutocompleteWidget();

    signals:
        void showAutocompleteWidget();
        void hideAutocompleteWidget();

    private slots:
        void updateInformation();

    private:
        BarPrivate* d;

        void focusInEvent(QFocusEvent *event);
        void focusOutEvent(QFocusEvent *event);
        void paintEvent(QPaintEvent *event);
        void resizeEvent(QResizeEvent* event);
        void keyPressEvent(QKeyEvent *event);
        bool eventFilter(QObject* watched, QEvent* event);

        void checkFocus();

        QString unfocusedText();
        QString unfocusedText(QUrl url);
        QString focusedText();
        QString focusedText(QUrl url);

        void updateText();
};

#endif // BAR_H
