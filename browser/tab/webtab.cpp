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

#include <QLabel>
#include <QWebEngineFullScreenRequest>
#include <QWebEngineView>
#include "widgets/tabbutton.h"
#include <QSslSocket>
#include <QTimer>
#include <QMenu>
#include "managers/profilemanager.h"
#include "managers/downloadmanager.h"
#include "managers/historymanager.h"
#include "webpage.h"
#include "permissionpopup.h"
#include "widgets/devtoolsheader.h"
#include "widgets/downloads/downloadspopoveritem.h"
#include <tpropertyanimation.h>

struct WebTabPrivate {
    QWebEngineView* view;
    QWebEngineView* devtoolsView = nullptr;
    WebPage* page;
    QPointer<TabButton> tabButton;

    int crashTimes = 0;

    QSslSocket certCheckSocket;
    QSslCertificate pageCertificate;

    HistoryManager* history;

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

    d->tabButton = new TabButton(this);
    d->tabButton->setAutoExclusive(true);
    d->tabButton->setCheckable(true);
    d->tabButton->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    connect(d->tabButton, &TabButton::clicked, this, &WebTab::requestActivate);

    this->layout()->removeWidget(ui->permissionPopupsWidget);
    ui->permissionPopupsWidget->setParent(this);
    ui->permissionPopupsWidget->setVisible(true);
    ui->permissionPopupsWidget->raise();

    d->history = HistoryManager::managerFor(page->profile());

    connect(&d->certCheckSocket, &QSslSocket::encrypted, this, [=] {
        d->pageCertificate = d->certCheckSocket.peerCertificate();

        d->certCheckSocket.abort();
        emit sslStateChanged();
    });

    if (page == nullptr) {
        d->page = new WebPage(ProfileManager::defaultProfile(), this);
        QTimer::singleShot(0, d->page, std::bind(QOverload<const QUrl&>::of(&WebPage::load), d->page, QUrl("https://www.google.com/")));
    } else {
        d->page = page;
        page->setParent(this);
    }
    d->page->setCertificateErrorPane(ui->sslErrorPane);
    connect(d->page, &WebPage::urlChanged, this, [=](QUrl url) {
        if (ui->stackedWidget->currentWidget() == ui->renderProcessErrorPage) ui->stackedWidget->setCurrentWidget(ui->webPage);

        d->certCheckSocket.abort();
        d->pageCertificate = QSslCertificate();

        if (url.scheme() == "https") {
            d->certCheckSocket.connectToHostEncrypted(url.host(), static_cast<quint16>(url.port(443)));
        }

        emit sslStateChanged();
        emit urlChanged(url);
    });
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
        d->tabButton->setLoadProgress(0, true);
    });
    connect(d->page, &WebPage::loadProgress, this, [=](int progress) {
        d->tabButton->setLoadProgress(progress, progress == 100 ? false : true);
    });
    connect(d->page, &WebPage::loadFinished, this, [=] {
        d->tabButton->setLoadProgress(100, false);

        //Add the current page to the history entry
        d->history->addEntry(d->page->url(), d->page->title());
    });
    connect(d->page, &WebPage::renderProcessTerminated, this, [=](WebPage::RenderProcessTerminationStatus status, int exitCode) {
        d->crashTimes++;
        if (d->crashTimes >= 3) {
            //Show the error pane
            d->removePermissionPopups(PermissionPopup::RenderProcessTerminate);

            switch (status) {
                case QWebEnginePage::NormalTerminationStatus:
                case QWebEnginePage::AbnormalTerminationStatus:
                case QWebEnginePage::CrashedTerminationStatus:
                    ui->renderProcessCrashTitle->setText(tr("Okay, this page is having some serious issues."));
                    ui->renderProcessCrashMessage->setText(tr("We've tried reloading it multiple times, but it keeps causing problems.\n\nUsually this is a temporary problem, so we recommend waiting a couple hours and then trying again."));
                    ui->reloadAfterRenderCrashButton->setText(tr("Reload Anyway"));
                    break;
                case QWebEnginePage::KilledTerminationStatus:
                    ui->renderProcessCrashTitle->setText(tr("Your device might be running out of memory."));
                    ui->renderProcessCrashMessage->setText(tr("Your system has stopped this webpage. Check your memory usage and then reload the page."));
                    ui->reloadAfterRenderCrashButton->setText(tr("Reload"));
                    break;
            }

            ui->stackedWidget->setCurrentWidget(ui->renderProcessErrorPage);
        } else {
            //Just reload the page
            PermissionPopup* popup = new PermissionPopup(QUrl(), PermissionPopup::RenderProcessTerminate);
            ui->permissionPopups->insertWidget(0, popup);
            connect(popup, &PermissionPopup::destroyed, this, [=] {
                d->permissionPopups.removeAll(popup);
            });
            d->permissionPopups.append(popup);

            QTimer::singleShot(0, this, [=] {
                d->page->triggerAction(WebPage::Reload);
            });
        }
    });
    connect(d->page, &WebPage::iconChanged, this, [=] {
        d->tabButton->setIcon(d->page->icon());
        emit iconChanged();
    });
    connect(d->page, &WebPage::openDevtools, this, &WebTab::openDevtools);

    connect(ui->sslErrorPane, &CertificateErrorPane::showPane, this, [=] {
        ui->stackedWidget->setCurrentWidget(ui->sslErrorPage);
    });
    connect(ui->sslErrorPane, &CertificateErrorPane::goBack, this, [=] {
        //d->view->back();
        d->view->stop();
    });
    connect(ui->sslErrorPane, &CertificateErrorPane::returnToBrowser, this, [=] {
        ui->stackedWidget->setCurrentWidget(ui->webPage);
    });

    connect(DownloadManager::instance(), &DownloadManager::downloadAdded, this, [=](DownloadManagerItem* item) {
        if (item->page() == d->page) {
            //Send the notification
            DownloadsPopoverItem* w = new DownloadsPopoverItem(item, this);
            w->setParent(this);
            w->show();

            QRect geometry;
            geometry.setSize(QSize(SC_DPI(300), w->sizeHint().height()));
            geometry.moveTopRight(QPoint(this->width(), -w->sizeHint().height()));
            w->setGeometry(geometry);

            tPropertyAnimation* anim = new tPropertyAnimation(w, "geometry");
            anim->setStartValue(geometry);
            anim->setEndValue(geometry.translated(0, geometry.height()));
            anim->setDuration(500);
            anim->setEasingCurve(QEasingCurve::OutCubic);
            connect(anim, &tPropertyAnimation::finished, this, [=] {
                if (anim->direction() == tPropertyAnimation::Forward) {
                    anim->setDirection(tPropertyAnimation::Backward);
                    anim->setEasingCurve(QEasingCurve::InCubic);
                    QTimer::singleShot(5000, anim, [=] {
                        anim->start();
                    });
                } else {
                    anim->deleteLater();
                    w->deleteLater();
                }
            });
            anim->start();

            connect(this, &WebTab::resized, w, [=] {
                QRect geometry;
                geometry.setSize(QSize(SC_DPI(300), w->sizeHint().height()));
                geometry.moveTopRight(QPoint(this->width(), -w->sizeHint().height()));

                anim->setStartValue(geometry);
                anim->setEndValue(geometry.translated(0, geometry.height()));
                anim->setCurrentTime(anim->currentTime());
            });
        }
    });

    ui->webPageSplitter->setChildrenCollapsible(false);
    ui->webPageSplitter->setHandleWidth(0);
//    ui->webPageSplitter->setSizes({1, 0});

    d->view = new QWebEngineView();
    d->view->setPage(d->page);
    d->view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(d->view, &QWebEngineView::customContextMenuRequested, this, [=](QPoint pos) {
        QMenu* menu = d->page->createStandardContextMenu();
        menu->popup(d->view->mapToGlobal(pos));
    });
    ui->webViewContainer->layout()->addWidget(d->view);
    ui->stackedWidget->setCurrentWidget(ui->webPage);
}

WebTab::~WebTab()
{
    if (!d->tabButton.isNull()) d->tabButton->deleteLater();
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

QWebEngineProfile* WebTab::profile()
{
    return d->page->profile();
}

QSslCertificate WebTab::pageCertificate()
{
    return d->pageCertificate;
}

QColor WebTab::tabColor()
{
    return d->tabButton->color();
}

QColor WebTab::tabForegroundColor()
{
    return d->tabButton->foregroundColor();
}

void WebTab::activated()
{
    d->tabButton->setChecked(true);
}

void WebTab::deactivated()
{
    d->tabButton->setChecked(false);
}

void WebTab::close()
{
    d->page->triggerAction(QWebEnginePage::RequestClose);
}

void WebTab::goBack()
{
    if (ui->stackedWidget->currentWidget() == ui->renderProcessErrorPage) ui->stackedWidget->setCurrentWidget(ui->webPage);
    d->view->back();
}

void WebTab::goForward()
{
    if (ui->stackedWidget->currentWidget() == ui->renderProcessErrorPage) ui->stackedWidget->setCurrentWidget(ui->webPage);
    d->view->forward();
}

void WebTab::navigate(QUrl url)
{
    if (ui->stackedWidget->currentWidget() == ui->renderProcessErrorPage) ui->stackedWidget->setCurrentWidget(ui->webPage);
    d->view->load(url);
}

void WebTab::reload()
{
    if (ui->stackedWidget->currentWidget() == ui->renderProcessErrorPage) ui->stackedWidget->setCurrentWidget(ui->webPage);
    d->view->reload();
}

void WebTab::leaveFullScreen()
{
    d->page->triggerAction(QWebEnginePage::ExitFullScreen);
}

void WebTab::openDevtools()
{
    if (d->devtoolsView != nullptr) return;
    QWidget* container = new QWidget();
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    container->setLayout(layout);

    DevtoolsHeader* header = new DevtoolsHeader(container);
    connect(header, &DevtoolsHeader::closeDevtools, this, [=] {
        container->deleteLater();
    });
    layout->addWidget(header);

    d->devtoolsView = new QWebEngineView(container);
    d->devtoolsView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    d->devtoolsView->page()->setInspectedPage(d->page);
    layout->addWidget(d->devtoolsView);

    ui->webPageSplitter->addWidget(container);
    ui->webPageSplitter->setSizes({this->width() / 2, this->width() / 2});
}

void WebTab::resizeEvent(QResizeEvent* event)
{
    ui->permissionPopupsWidget->move(0, 0);
    ui->permissionPopupsSpacer->changeSize(this->width(), 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->permissionPopupsWidget->setFixedWidth(this->width());

    emit resized();
}

void WebTab::on_reloadAfterRenderCrashButton_clicked()
{
    this->reload();
}
