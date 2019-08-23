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
#include "webpage.h"

#include <QWebEngineCertificateError>
#include <QMenu>
#include <QWebEngineContextMenuData>
#include <tpopover.h>
#include "managers/iconmanager.h"
#include "popovers/jsalert.h"
#include "popovers/jsconfirm.h"
#include "certificateerrorpane.h"

struct WebPagePrivate {
    QWidget* parent;
    CertificateErrorPane* certErrorPane;
};

WebPage::WebPage(QWidget *parent) : QWebEnginePage(parent)
{
    d = new WebPagePrivate();
    d->parent = parent;
}

WebPage::WebPage(QWebEngineProfile* profile, QWidget* parent) : QWebEnginePage(profile, parent)
{
    d = new WebPagePrivate();
    d->parent = parent;
}

WebPage::~WebPage()
{
    delete d;
}

void WebPage::setParent(QWidget *parent)
{
    d->parent = parent;
}

void WebPage::setCertificateErrorPane(CertificateErrorPane* pane)
{
    d->certErrorPane = pane;
}

QMenu* WebPage::createStandardContextMenu()
{
    QMenu* menu = new QMenu();
    QWebEngineContextMenuData cxData;

    QColor iconTint = menu->palette().color(QPalette::WindowText);
    QSize iconSize = SC_DPI_T(QSize(16, 16), QSize);
    menu->addSection(tr("For Tab"));
    menu->addAction(IconManager::getIcon("go-previous", iconTint, iconSize), tr("Back"), this, std::bind(&WebPage::triggerAction, this, Back, false));
    menu->addAction(IconManager::getIcon("go-previous", iconTint, iconSize), tr("Forward"), this, std::bind(&WebPage::triggerAction, this, Forward, false));
    menu->addAction(IconManager::getIcon("go-previous", iconTint, iconSize), tr("Reload"), this, std::bind(&WebPage::triggerAction, this, Reload, false));
    menu->addSeparator();
    menu->addAction(IconManager::getIcon("go-previous", iconTint, iconSize), tr("View Source"), this, std::bind(&WebPage::triggerAction, this, ViewSource, false));
    menu->addSeparator();
    menu->addAction(IconManager::getIcon("go-previous", iconTint, iconSize), tr("Inspect Element"), this, [=] {
        emit openDevtools();
        this->triggerAction(InspectElement);
    });

    return menu;
}

QWebEnginePage* WebPage::createWindow(WebPage::WebWindowType type) {
    WebPage* page = new WebPage(this->profile(), d->parent);
    emit spawnTab(page, type);
    return page;
}

bool WebPage::certificateError(const QWebEngineCertificateError& certificateError)
{
    //Only show certificate errors for the main page, reject other errors
    //if (certificateError.url() != this->url()) return false;
    return d->certErrorPane->resolveError(certificateError);
}

void WebPage::javaScriptAlert(const QUrl& securityOrigin, const QString& msg)
{
    QEventLoop loop;

    JsAlert* alert = new JsAlert(msg);
    tPopover* popover = new tPopover(alert);
    popover->setPopoverSide(tPopover::Bottom);
    popover->setPopoverWidth(alert->sizeHint().height());
    connect(alert, &JsAlert::dismiss, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, alert, &JsAlert::deleteLater);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, &loop, &QEventLoop::quit);
    popover->show(d->parent);
    loop.exec();
}

bool WebPage::javaScriptConfirm(const QUrl& securityOrigin, const QString &msg)
{
    QEventLoop loop;

    JsConfirm* alert = new JsConfirm(msg);
    tPopover* popover = new tPopover(alert);
    popover->setPopoverSide(tPopover::Bottom);
    popover->setPopoverWidth(alert->sizeHint().height());
    popover->setDismissable(false);
    connect(alert, &JsConfirm::accept, popover, &tPopover::dismiss);
    connect(alert, &JsConfirm::accept, &loop, std::bind(&QEventLoop::exit, &loop, 1));
    connect(alert, &JsConfirm::reject, popover, &tPopover::dismiss);
    connect(alert, &JsConfirm::reject, &loop, std::bind(&QEventLoop::exit, &loop, 0));
    connect(popover, &tPopover::dismissed, alert, &JsAlert::deleteLater);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    popover->show(d->parent);
    return loop.exec();
}
