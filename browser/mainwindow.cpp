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
#include <QTimer>
#include <QCloseEvent>
#include "tab/webtab.h"
#include "widgets/bar.h"
#include "tab/webpage.h"
#include "managers/profilemanager.h"
#include "managers/settingsmanager.h"
#include "managers/iconmanager.h"

struct MainWindowPrivate {
//    Bar* bar;

    QShortcut* leaveFullScreenShortcut;
    WebTab* fullScreenTab = nullptr;
    Qt::WindowStates stateBeforeFullScreen;

    QWebEngineProfile* profile;
};

MainWindow::MainWindow(QVariantMap options, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    d = new MainWindowPrivate();

    d->profile = options.value("profile", QVariant::fromValue(ProfileManager::defaultProfile())).value<QWebEngineProfile*>();
    QIcon windowIcon = d->profile == ProfileManager::oblivionProfile() ? QIcon::fromTheme("theweb-oblivion", QIcon(":/icons/theweb-oblivion.svg")) : QIcon::fromTheme("theweb", QIcon(":/icons/theweb.svg"));
    this->setWindowIcon(windowIcon);

    ui->tabs->setCurrentAnimation(tStackedWidget::SlideHorizontal);

    #ifdef Q_OS_MAC
        setupMacOS();
    #else
        ui->menuBar->setVisible(false);
    #endif

    QMenu* menu = new QMenu();
    menu->addAction(ui->actionNewTab);
    menu->addAction(ui->actionNew_Window);
    menu->addAction(ui->actionNew_Oblivion_Window);
    menu->addSeparator();
    menu->addAction(ui->actionGoBack);
    menu->addAction(ui->actionGoForward);
    menu->addAction(ui->actionReload);
    menu->addSeparator();
    menu->addAction(ui->actionSettings);
    menu->addSeparator();
    menu->addAction(ui->actionExit);
    ui->toolbarWidget->setMenu(menu);
    ui->toolbarWidget->setMenuIcon(windowIcon);

    ui->closeTabButton->setIcon(IconManager::getIcon("window-close", ui->closeTabButton->palette().color(QPalette::WindowText), ui->closeTabButton->iconSize()));

    if (d->profile == ProfileManager::oblivionProfile()) ui->toolbarWidget->setAsOblivion();
    connect(ui->toolbarWidget, &Toolbar::newTabRequested, ui->actionNewTab, &QAction::trigger);

    d->leaveFullScreenShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(d->leaveFullScreenShortcut, &QShortcut::activated, this, [=] {
        if (d->fullScreenTab != nullptr) d->fullScreenTab->leaveFullScreen();
    });
    d->leaveFullScreenShortcut->setEnabled(false);

    WebPage* page = new WebPage(d->profile, nullptr);
    page->setUrl(SettingsManager::getHomePage());
    newTab(new WebTab(page));

    on_tabs_switchingFrame(0);
}

MainWindow::~MainWindow()
{
    delete d;
    delete ui;
}

void MainWindow::newTab()
{
    WebPage* page = new WebPage(d->profile, nullptr);
    page->setUrl(QUrl("theweb://newtab"));
    newTab(new WebTab(page));
    ui->toolbarWidget->focusBar();
}

void MainWindow::newTab(WebTab* tab)
{
    ui->tabs->addWidget(tab);
    ui->tabs->setCurrentWidget(tab);

    ui->tabLayout->addWidget(tab->getTabButton());

    connect(tab, &WebTab::requestActivate, this, [=] {
        ui->tabs->setCurrentWidget(tab);
        QTimer::singleShot(0, tab, QOverload<>::of(&WebTab::setFocus));
    });
    connect(tab, &WebTab::goFullScreen, this, [=](bool fullscreen) {
        if (fullscreen) {
            d->stateBeforeFullScreen = this->windowState() & ~Qt::WindowFullScreen;
            d->fullScreenTab = tab;
            ui->tabFrame->setVisible(false);
            ui->toolbarWidget->setVisible(false);
            d->leaveFullScreenShortcut->setEnabled(true);
            this->showFullScreen();
        } else {
            ui->tabFrame->setVisible(true);
            ui->toolbarWidget->setVisible(true);
            d->leaveFullScreenShortcut->setEnabled(false);
            this->setWindowState(d->stateBeforeFullScreen);
            d->fullScreenTab = nullptr;
        }
    });
    connect(tab, &WebTab::spawnTab, this, QOverload<WebTab*>::of(&MainWindow::newTab));
}

void MainWindow::on_tabs_switchingFrame(int arg1)
{
    for (int i = 0; i < ui->tabs->count(); i++) {
        static_cast<WebTab*>(ui->tabs->widget(i))->deactivated();
    }

    if (arg1 == -1) {
        this->close();
    } else {
        WebTab* currentTab = static_cast<WebTab*>(ui->tabs->widget(arg1));
        currentTab->activated();
        ui->toolbarWidget->setCurrentTab(currentTab);
    }
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

void MainWindow::on_closeTabButton_clicked()
{
    currentTab()->close();
}

void MainWindow::on_actionReload_triggered()
{
    currentTab()->reload();
}

void MainWindow::on_actionSettings_triggered()
{
    WebPage* page = new WebPage(d->profile, nullptr);
    page->setUrl(QUrl("theweb://settings"));
    this->newTab(new WebTab(page));
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::closeAllWindows();
}

void MainWindow::on_actionNew_Window_triggered()
{
    MainWindow* w = new MainWindow({
        {"profile", QVariant::fromValue(d->profile)}
    });

    w->show();
}

void MainWindow::on_actionNew_Oblivion_Window_triggered()
{
    MainWindow* w = new MainWindow({
        {"profile", QVariant::fromValue(ProfileManager::oblivionProfile())}
    });

    w->show();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (ui->tabs->count() != 0) {
        //Ignore this event for now and ask all tabs to close
        event->ignore();
        QTimer::singleShot(0, [=] {
            for (int i = 0; i < ui->tabs->count(); i++) {
                static_cast<WebTab*>(ui->tabs->widget(i))->close();
            }
        });

        //Window will automatically close when there are no tabs left
    }
}

void MainWindow::on_tabs_currentChanged(int arg1)
{
    on_tabs_switchingFrame(arg1);
}
