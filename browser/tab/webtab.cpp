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
#include "webtab.h"
#include "ui_webtab.h"

#include <QWebEngineFullScreenRequest>
#include <QWebEngineView>
#include <QPushButton>
#include "managers/profilemanager.h"
#include "webpage.h"
#include "permissionpopup.h"

struct WebTabPrivate {
    QWebEngineView* view;
    WebPage* page;
    QPushButton* tabButton;

    bool isLoading = false;
    int loadProgress = 0;

    QList<PermissionPopup*> permissionPopups;
    void removePermissionPopups(PermissionPopup::PermissionType type) {
        for (PermissionPopup* popup : permissionPopups) {
            if (popup->type() == type) popup->deleteLater();
        }
    }
};

WebTab::WebTab(WebPage* page, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebTab)
{
    ui->setupUi(this);
    d = new WebTabPrivate();

    d->tabButton = new QPushButton();
    d->tabButton->setAutoExclusive(true);
    d->tabButton->setCheckable(true);
    d->tabButton->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    connect(d->tabButton, &QPushButton::clicked, this, &WebTab::requestActivate);

    this->layout()->removeWidget(ui->permissionPopupsWidget);
    ui->permissionPopupsWidget->setParent(this);
    ui->permissionPopupsWidget->setVisible(true);
    ui->permissionPopupsWidget->raise();

    if (page == nullptr) {
        d->page = new WebPage(ProfileManager::defaultProfile(), this);
        d->page->load(QUrl("https://www.google.com/"));
    } else {
        d->page = page;
        page->setParent(this);
    }
    d->page->setCertificateErrorPane(ui->sslErrorPane);
    connect(d->page, &WebPage::urlChanged, this, &WebTab::urlChanged);
    connect(d->page, &WebPage::titleChanged, this, [=] {
        d->tabButton->setText(d->page->title());
    });
    connect(d->page, &WebPage::windowCloseRequested, this, &WebTab::deleteLater);
    connect(d->page, &WebPage::spawnTab, this, [=](WebPage* page, WebPage::WebWindowType type) {
        WebTab* tab = new WebTab(page);
        emit spawnTab(tab);
    });
    connect(d->page, &WebPage::fullScreenRequested, this, [=](QWebEngineFullScreenRequest fsRequest) {
        if (fsRequest.toggleOn()) {
            PermissionPopup* popup = new PermissionPopup(fsRequest.origin(), PermissionPopup::FullScreen);
            ui->permissionPopups->insertWidget(0, popup);
            connect(popup, &PermissionPopup::destroyed, this, [=] {
                d->permissionPopups.removeAll(popup);
            });
            d->permissionPopups.append(popup);
        } else {
            d->removePermissionPopups(PermissionPopup::FullScreen);
        }
        emit goFullScreen(fsRequest.toggleOn());
        fsRequest.accept();
    });
    connect(d->page, &WebPage::featurePermissionRequested, this, [=](QUrl securityOrigin, WebPage::Feature feature) {
        PermissionPopup::PermissionType permission = PermissionPopup::Unknown;
        switch (feature) {
            case WebPage::Geolocation:
                permission = PermissionPopup::Geolocation;
                break;
            case WebPage::MediaAudioCapture:
                permission = PermissionPopup::Microphone;
                break;
            case WebPage::MediaVideoCapture:
                permission = PermissionPopup::Webcam;
                break;
            case WebPage::MediaAudioVideoCapture:
                permission = PermissionPopup::WebcamAndMicrophone;
                break;
            case WebPage::DesktopVideoCapture:
                permission = PermissionPopup::ScreenRecord;
                break;
            case WebPage::DesktopAudioVideoCapture:
                permission = PermissionPopup::ScreenAudioRecord;
                break;
            case WebPage::Notifications:
                permission = PermissionPopup::Notifications;
                break;
            case WebPage::MouseLock:
                permission = PermissionPopup::MouseLock;
        }

        if (permission == PermissionPopup::Unknown) {
            d->page->setFeaturePermission(securityOrigin, feature, WebPage::PermissionUnknown);
        } else {
            PermissionPopup* popup = new PermissionPopup(securityOrigin, permission);
            ui->permissionPopups->insertWidget(0, popup);
            connect(popup, &PermissionPopup::destroyed, this, [=] {
                d->permissionPopups.removeAll(popup);
            });
            connect(popup, &PermissionPopup::allow, this, [=] {
                d->page->setFeaturePermission(securityOrigin, feature, WebPage::PermissionGrantedByUser);
            });
            connect(popup, &PermissionPopup::deny, this, [=] {
                d->page->setFeaturePermission(securityOrigin, feature, WebPage::PermissionDeniedByUser);
            });
            d->permissionPopups.append(popup);
        }
    });
    connect(d->page, &WebPage::featurePermissionRequestCanceled, this, [=](QUrl securityOrigin, WebPage::Feature feature) {
        switch (feature) {
            case WebPage::Geolocation:
                d->removePermissionPopups(PermissionPopup::Geolocation);
                break;
            case WebPage::MediaAudioCapture:
                d->removePermissionPopups(PermissionPopup::Microphone);
                break;
            case WebPage::MediaVideoCapture:
                d->removePermissionPopups(PermissionPopup::Webcam);
                break;
            case WebPage::MediaAudioVideoCapture:
                d->removePermissionPopups(PermissionPopup::WebcamAndMicrophone);
                break;
            case WebPage::DesktopVideoCapture:
                d->removePermissionPopups(PermissionPopup::ScreenRecord);
                break;
            case WebPage::DesktopAudioVideoCapture:
                d->removePermissionPopups(PermissionPopup::ScreenAudioRecord);
                break;
            case WebPage::Notifications:
                d->removePermissionPopups(PermissionPopup::Notifications);
                break;
            case WebPage::MouseLock:
                d->removePermissionPopups(PermissionPopup::MouseLock);

        }
        ui->permissionPopupsWidget->setFixedHeight(ui->permissionPopupsWidget->sizeHint().height());
    });
    connect(d->page, &WebPage::loadStarted, this, [=] {
        d->isLoading = true;
        d->loadProgress = 0;
        emit loadProgressChanged();
    });
    connect(d->page, &WebPage::loadProgress, this, [=](int progress) {
        d->isLoading = true;
        d->loadProgress = progress;
        emit loadProgressChanged();
    });
    connect(d->page, &WebPage::loadFinished, this, [=] {
        d->isLoading = false;
        d->loadProgress = 0;
        emit loadProgressChanged();
    });

    connect(ui->sslErrorPane, &CertificateErrorPane::showPane, this, [=] {
        ui->stackedWidget->setCurrentWidget(ui->sslErrorPage);
    });
    connect(ui->sslErrorPane, &CertificateErrorPane::goBack, this, [=] {
        //d->view->back();
        d->view->stop();
    });
    connect(ui->sslErrorPane, &CertificateErrorPane::returnToBrowser, this, [=] {
        ui->stackedWidget->setCurrentWidget(d->view);
    });

    d->view = new QWebEngineView();
    d->view->setPage(d->page);
    ui->stackedWidget->insertWidget(0, d->view);
    ui->stackedWidget->setCurrentWidget(d->view);
}

WebTab::~WebTab()
{
    d->tabButton->deleteLater();
    d->page->deleteLater();
    d->view->deleteLater();
    delete ui;
    delete d;
}

QWidget*WebTab::getTabButton()
{
    return d->tabButton;
}

QUrl WebTab::currentUrl()
{
    return d->view->url();
}

bool WebTab::isLoading()
{
    return d->isLoading;
}

int WebTab::loadProgress()
{
    return d->loadProgress;
}

void WebTab::activated()
{
    d->tabButton->setChecked(true);
}

void WebTab::close()
{
    d->page->triggerAction(QWebEnginePage::RequestClose);
}

void WebTab::goBack()
{
    d->view->back();
}

void WebTab::goForward()
{
    d->view->forward();
}

void WebTab::navigate(QUrl url)
{
    d->view->load(url);
}

void WebTab::reload()
{
    d->view->reload();
}

void WebTab::leaveFullScreen()
{
    d->page->triggerAction(QWebEnginePage::ExitFullScreen);
}

void WebTab::resizeEvent(QResizeEvent* event)
{
    ui->permissionPopupsWidget->move(0, 0);
    ui->permissionPopupsSpacer->changeSize(this->width(), 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
}
