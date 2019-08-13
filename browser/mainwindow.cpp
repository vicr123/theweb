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
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QShortcut>
#include "tab/webtab.h"
#include "bar.h"

struct MainWindowPrivate {
    Bar* bar;

    QShortcut* leaveFullScreenShortcut;
    WebTab* fullScreenTab = nullptr;
    Qt::WindowStates stateBeforeFullScreen;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    d = new MainWindowPrivate();

    #ifdef Q_OS_MAC
    #else
        QMenu* menu = new QMenu();
        menu->addAction(ui->actionNewTab);
        menu->addSeparator();
        menu->addAction(ui->actionGoBack);
        menu->addAction(ui->actionGoForward);
        menu->addAction(ui->actionReload);

        QToolButton* menuButton = new QToolButton();
        menuButton->setMenu(menu);
        menuButton->setPopupMode(QToolButton::InstantPopup);
        menuButton->setIcon(QIcon::fromTheme("theweb", QIcon(":/icons/theweb.svg")));
        ui->mainToolBar->insertWidget(ui->actionGoBack, menuButton);
        ui->menuBar->setVisible(false);
    #endif

    d->bar = new Bar();
    ui->mainToolBar->addWidget(d->bar);

    d->leaveFullScreenShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(d->leaveFullScreenShortcut, &QShortcut::activated, this, [=] {
        if (d->fullScreenTab != nullptr) d->fullScreenTab->leaveFullScreen();
    });
    d->leaveFullScreenShortcut->setEnabled(false);

    newTab();
}

MainWindow::~MainWindow()
{
    delete d;
    delete ui;
}

void MainWindow::newTab()
{
    newTab(new WebTab());
}

void MainWindow::newTab(WebTab* tab)
{
    ui->tabs->addWidget(tab);
    ui->tabs->setCurrentWidget(tab);

    ui->tabLayout->addWidget(tab->getTabButton());

    connect(tab, &WebTab::requestActivate, this, [=] {
        ui->tabs->setCurrentWidget(tab);
    });
    connect(tab, &WebTab::goFullScreen, this, [=](bool fullscreen) {
        if (fullscreen) {
            d->stateBeforeFullScreen = this->windowState() & ~Qt::WindowFullScreen;
            d->fullScreenTab = tab;
            ui->tabFrame->setVisible(false);
            ui->mainToolBar->setVisible(false);
            d->leaveFullScreenShortcut->setEnabled(true);
            this->showFullScreen();
        } else {
            ui->tabFrame->setVisible(true);
            ui->mainToolBar->setVisible(true);
            d->leaveFullScreenShortcut->setEnabled(false);
            this->setWindowState(d->stateBeforeFullScreen);
            d->fullScreenTab = nullptr;
        }
    });
    connect(tab, &WebTab::spawnTab, this, QOverload<WebTab*>::of(&MainWindow::newTab));
}

void MainWindow::on_tabs_currentChanged(int arg1)
{
    WebTab* currentTab = static_cast<WebTab*>(ui->tabs->widget(arg1));
    currentTab->activated();
    d->bar->setCurrentTab(currentTab);
}

void MainWindow::on_actionGoBack_triggered()
{
    currentTab()->goBack();
}

void MainWindow::on_actionGoForward_triggered()
{
    currentTab()->goForward();
}

WebTab* MainWindow::currentTab()
{
    return static_cast<WebTab*>(ui->tabs->currentWidget());
}

void MainWindow::on_actionNewTab_triggered()
{
    this->newTab();
}

void MainWindow::on_toolButton_clicked()
{
    currentTab()->close();
}

void MainWindow::on_actionReload_triggered()
{
    currentTab()->reload();
}
