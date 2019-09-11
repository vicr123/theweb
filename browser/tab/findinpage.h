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
#ifndef FINDINPAGE_H
#define FINDINPAGE_H

#include <QWidget>

namespace Ui {
    class FindInPage;
}

class WebPage;
struct FindInPagePrivate;
class FindInPage : public QWidget
{
        Q_OBJECT

    public:
        explicit FindInPage(QWidget *parent = nullptr);
        ~FindInPage();

        void setWebPage(WebPage* page);
        void find(bool forwards = true);

    public slots:
        void show();
        void hide();

    private slots:
        void on_closeButton_clicked();

        void on_findNextButton_clicked();

        void on_findPreviousButton_clicked();

        void on_searchTextEdit_textChanged(const QString &arg1);

        void on_searchTextEdit_returnPressed();

    private:
        Ui::FindInPage *ui;
        FindInPagePrivate* d;
};

#endif // FINDINPAGE_H
