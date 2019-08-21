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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVariantMap>

namespace Ui {
    class MainWindow;
}

class WebTab;
struct MainWindowPrivate;
class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QVariantMap options = QVariantMap(), QWidget *parent = nullptr);
        ~MainWindow();

        void newTab();
        void newTab(WebTab* tab);

    private slots:
        void on_tabs_currentChanged(int arg1);

        void on_actionGoBack_triggered();

        void on_actionGoForward_triggered();

        void on_actionNewTab_triggered();

        void on_toolButton_clicked();

        void on_actionReload_triggered();

        void on_actionSettings_triggered();

        void on_actionExit_triggered();

        void on_actionNew_Window_triggered();

        void on_actionNew_Oblivion_Window_triggered();

private:
        Ui::MainWindow *ui;
        MainWindowPrivate* d;

        WebTab* currentTab();
};

#endif // MAINWINDOW_H
