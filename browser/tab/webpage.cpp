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

    this->resetZoom();
}

WebPage::WebPage(QWebEngineProfile* profile, QWidget* parent) : QWebEnginePage(profile, parent)
{
    d = new WebPagePrivate();
    d->parent = parent;

    this->resetZoom();
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
    QWebEngineContextMenuData cxData = this->contextMenuData();

    auto addSection = [=](QString text) {
        #if defined(Q_OS_MAC) || defined(Q_OS_WIN)
            menu->addSeparator();
            QAction* a = menu->addAction(text);
            a->setEnabled(false);
        #else
            menu->addSection(text);
        #endif
    };

    QFontMetrics fm = menu->fontMetrics();
    QColor iconTint = menu->palette().color(QPalette::WindowText);
    QSize iconSize = SC_DPI_T(QSize(16, 16), QSize);

    if (cxData.misspelledWord() != "") {
        addSection(fm.elidedText(tr("For Misspelled Word \"%1\"").arg(cxData.misspelledWord()), Qt::ElideRight, SC_DPI(400)));
        for (QString word : cxData.spellCheckerSuggestions()) {
            menu->addAction(word, this, [=] {
                this->replaceMisspelledWord(word);
            });
        }
    }

    if (cxData.selectedText() != "") {
        addSection(fm.elidedText(tr("For selected text \"%1\"").arg(cxData.selectedText()), Qt::ElideRight, SC_DPI(400)));
        if (cxData.editFlags() & QWebEngineContextMenuData::CanCopy) menu->addAction(IconManager::getIcon("edit-copy", iconTint, iconSize), tr("Copy"), this, std::bind(&WebPage::triggerAction, this, Copy, false));
        if (cxData.editFlags() & QWebEngineContextMenuData::CanCut) menu->addAction(IconManager::getIcon("edit-cut", iconTint, iconSize), tr("Cut"), this, std::bind(&WebPage::triggerAction, this, Cut, false));
    }

    if (cxData.isContentEditable()) {
        addSection(tr("For Editable Box"));
        if (cxData.editFlags() & QWebEngineContextMenuData::CanPaste) menu->addAction(IconManager::getIcon("edit-paste", iconTint, iconSize), tr("Paste"), this, std::bind(&WebPage::triggerAction, this, Paste, false));
        if (cxData.editFlags() & QWebEngineContextMenuData::CanUndo) menu->addAction(IconManager::getIcon("edit-undo", iconTint, iconSize), tr("Undo"), this, std::bind(&WebPage::triggerAction, this, Undo, false));
        if (cxData.editFlags() & QWebEngineContextMenuData::CanRedo) menu->addAction(IconManager::getIcon("edit-redo", iconTint, iconSize), tr("Redo"), this, std::bind(&WebPage::triggerAction, this, Redo, false));
        if (cxData.editFlags() & QWebEngineContextMenuData::CanSelectAll) menu->addAction(IconManager::getIcon("edit-select-all", iconTint, iconSize), tr("Select All"), this, std::bind(&WebPage::triggerAction, this, SelectAll, false));
    }

    if (!cxData.linkUrl().isEmpty()) {
        addSection(fm.elidedText(tr("For link \"%1\"").arg(cxData.linkUrl().toString()), Qt::ElideRight, SC_DPI(400)));
        menu->addAction(tr("Open"), this, std::bind(&WebPage::triggerAction, this, OpenLinkInThisWindow, false));
        menu->addAction(IconManager::getIcon("tab-new", iconTint, iconSize), tr("Open in New Tab"), this, std::bind(&WebPage::triggerAction, this, OpenLinkInNewTab, false));
        menu->addAction(IconManager::getIcon("edit-copy", iconTint, iconSize), tr("Copy"), this, std::bind(&WebPage::triggerAction, this, CopyLinkToClipboard, false));
    }

    if (cxData.mediaType() == QWebEngineContextMenuData::MediaTypeNone) {
        //Do nothing here
    } else if (cxData.mediaType() == QWebEngineContextMenuData::MediaTypeImage) {
        addSection(tr("For Image"));
        menu->addAction(IconManager::getIcon("edit-copy", iconTint, iconSize), tr("Copy"), this, std::bind(&WebPage::triggerAction, this, CopyImageToClipboard, false));
        menu->addAction(IconManager::getIcon("edit-copy", iconTint, iconSize), tr("Copy Address"), this, std::bind(&WebPage::triggerAction, this, CopyImageUrlToClipboard, false));
    } else {
        switch (cxData.mediaType()) {
            case QWebEngineContextMenuData::MediaTypeVideo:
                addSection(tr("For Video"));
                break;
            case QWebEngineContextMenuData::MediaTypeAudio:
                addSection(tr("For Audio"));
                break;
            default:
                addSection(tr("For Media content"));
                break;
        }

        if (cxData.mediaFlags() & QWebEngineContextMenuData::MediaPaused) {
            menu->addAction(IconManager::getIcon("media-playback-start", iconTint, iconSize), tr("Play"), this, std::bind(&WebPage::triggerAction, this, ToggleMediaPlayPause, false));
        } else {
            menu->addAction(IconManager::getIcon("media-playback-pause", iconTint, iconSize), tr("Pause"), this, std::bind(&WebPage::triggerAction, this, ToggleMediaPlayPause, false));
        }

        menu->addAction(IconManager::getIcon("edit-copy", iconTint, iconSize), tr("Copy Address"), this, std::bind(&WebPage::triggerAction, this, CopyMediaUrlToClipboard, false));
    }

    addSection(tr("For Tab"));
    menu->addAction(IconManager::getIcon("go-previous", iconTint, iconSize), tr("Back"), this, std::bind(&WebPage::triggerAction, this, Back, false));
    menu->addAction(IconManager::getIcon("go-next", iconTint, iconSize), tr("Forward"), this, std::bind(&WebPage::triggerAction, this, Forward, false));
    menu->addAction(IconManager::getIcon("view-refresh", iconTint, iconSize), tr("Reload"), this, std::bind(&WebPage::triggerAction, this, Reload, false));
    menu->addSeparator();
    menu->addAction(IconManager::getIcon("document-print", iconTint, iconSize), tr("Print"), this, [=] {
        emit printRequested();
    });
    menu->addAction(tr("View Source"), this, std::bind(&WebPage::triggerAction, this, ViewSource, false));
    menu->addSeparator();
    menu->addAction(tr("Inspect Element"), this, [=] {
        emit openDevtools();
        this->triggerAction(InspectElement);
    });

    return menu;
}

void WebPage::resetZoom()
{
//    this->setZoomFactor(theLibsGlobal::getDPIScaling());
}

void WebPage::zoomIn()
{
//    this->setZoomFactor(this->zoomFactor() + 0.1);
}

void WebPage::zoomOut()
{
//    this->setZoomFactor(this->zoomFactor() - 0.1);
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
