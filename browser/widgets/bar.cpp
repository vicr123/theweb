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
#include "bar.h"

#include <QTimer>
#include <QPointer>
#include <QPainter>
#include <QListWidget>
#include <QKeyEvent>
#include <the-libs_global.h>
#include "tab/webtab.h"
#include "securitychunk.h"
#include "toolbar/barautocomplete.h"
#include "managers/profilemanager.h"

struct BarPrivate {
    QPointer<WebTab> currentTab;
    QListView* autocompleteWidget;
    BarAutocomplete* autocompleteModel;
    BarAutocompleteDelegate* autocompleteDelegate;

    bool autocompleteWidgetVisible = false;
    bool hasFocus = false;
};

Bar::Bar(QWidget *parent) : QLineEdit(parent)
{
    d = new BarPrivate();

    d->autocompleteModel = new BarAutocomplete(this);
    d->autocompleteDelegate = new BarAutocompleteDelegate(this);

    d->autocompleteWidget = new QListView();
    d->autocompleteWidget->setFrameShape(QListWidget::NoFrame);
    d->autocompleteWidget->setModel(d->autocompleteModel);
    d->autocompleteWidget->setItemDelegate(d->autocompleteDelegate);
    d->autocompleteWidget->installEventFilter(this);
    d->autocompleteWidget->setFocusProxy(this);
    connect(d->autocompleteWidget, &QListView::clicked, this, [=](QModelIndex index) {
        d->currentTab->navigate(index.data(Qt::UserRole).value<QUrl>());
        d->currentTab->setFocus();
    });

    this->setFrame(false);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(this, &Bar::returnPressed, this, [=] {
        d->currentTab->navigate(d->autocompleteWidget->currentIndex().data(Qt::UserRole).value<QUrl>());
        d->currentTab->setFocus();
    });
    connect(this, &Bar::textChanged, this, [=](QString text) {
        d->autocompleteModel->setQuery(d->currentTab->profile(), text);
        d->autocompleteWidget->setCurrentIndex(d->autocompleteModel->index(0));
    });

    this->setCursor(QCursor(Qt::ArrowCursor));
    this->setPlaceholderText(tr("Search or enter web address"));

    if (QApplication::layoutDirection() == Qt::LeftToRight) {
        this->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    } else {
        this->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
}

Bar::~Bar() {
    delete d->autocompleteWidget;
    delete d;
}

void Bar::setCurrentTab(WebTab* tab)
{
    //Disconnect all signals from the current tab
    if (!d->currentTab.isNull()) {
        disconnect(d->currentTab, nullptr, this, nullptr);
    }

    d->currentTab = tab;

    if (tab != nullptr) {
        connect(tab, &WebTab::urlChanged, this, &Bar::updateInformation);
    }

    if (this->hasFocus()) {
        this->setText(focusedText());
    } else {
        this->setText(unfocusedText());
    }
}

QWidget* Bar::getAutocompleteWidget()
{
    return d->autocompleteWidget;
}

void Bar::updateInformation() {
    if (!this->hasFocus()) {
        this->setText(unfocusedText());
    }

    this->update();
}

void Bar::focusInEvent(QFocusEvent* event)
{
    QTimer::singleShot(0, this, &Bar::checkFocus);
    QLineEdit::focusInEvent(event);
}

void Bar::focusOutEvent(QFocusEvent* event)
{
    QTimer::singleShot(0, this, &Bar::checkFocus);
    QLineEdit::focusOutEvent(event);
}

void Bar::paintEvent(QPaintEvent* event)
{
    QLineEdit::paintEvent(event);
}

void Bar::resizeEvent(QResizeEvent* event)
{
    d->autocompleteDelegate->setLeftOffset(QApplication::layoutDirection() == Qt::LeftToRight ? this->geometry().x() : this->geometry().right());
}

void Bar::keyPressEvent(QKeyEvent* event)
{
    QLineEdit::keyPressEvent(event);

    if (event->key() == Qt::Key_Up) {
        //Move the selected index -1
        int index = d->autocompleteWidget->currentIndex().row();
        if (index == 0) index = d->autocompleteModel->rowCount();
        index--;
        d->autocompleteWidget->setCurrentIndex(d->autocompleteModel->index(index));
    } else if (event->key() == Qt::Key_Down) {
        //Move the selected index +1
        int index = d->autocompleteWidget->currentIndex().row();
        index++;
        if (index == d->autocompleteModel->rowCount()) index = 0;
        d->autocompleteWidget->setCurrentIndex(d->autocompleteModel->index(index));
    }

    if (!d->autocompleteWidgetVisible) {
        d->autocompleteWidgetVisible = true;
        emit showAutocompleteWidget();
    }
}

bool Bar::eventFilter(QObject*watched, QEvent*event)
{
    if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut) {
        QTimer::singleShot(0, this, &Bar::checkFocus);
    }
    return false;
}

void Bar::checkFocus()
{
    if (!d->hasFocus && (this->hasFocus() || d->autocompleteWidget->hasFocus())) {
        this->setCursor(QCursor(Qt::IBeamCursor));
        this->setText(focusedText());
        QTimer::singleShot(0, this, &Bar::selectAll);

        d->hasFocus = true;
    } else if (d->hasFocus && (!this->hasFocus() && !d->autocompleteWidget->hasFocus())) {
        this->setCursor(QCursor(Qt::ArrowCursor));
        this->setText(unfocusedText());
        this->setSelection(0, 0);

        if (d->autocompleteWidgetVisible) {
            d->autocompleteWidgetVisible = false;
            emit hideAutocompleteWidget();
        }

        d->hasFocus = false;
    }
}

QString Bar::unfocusedText() {
    if (d->currentTab.isNull()) return "";
    return unfocusedText(d->currentTab->currentUrl());
}

QString Bar::unfocusedText(QUrl url)
{
    if (d->currentTab.isNull()) return "";
    if (url.scheme() == "theweb" && url.host() == "newtab") return "";

    //Special handling for the PDF viewer
    if (url.scheme() == "chrome-extension" && url.host() == "mhjfbmdgcfjbbpaeojofohoefgiehjai") {
        return unfocusedText(QUrl(url.query()));
    }
    return url.host(QUrl::FullyEncoded);
}

QString Bar::focusedText() {
    return focusedText(d->currentTab->currentUrl());
}

QString Bar::focusedText(QUrl url)
{
    if (d->currentTab.isNull()) return "";
    if (url.scheme() == "theweb" && url.host() == "newtab") return "";

    //Special handling for the PDF viewer
    if (url.scheme() == "chrome-extension" && url.host() == "mhjfbmdgcfjbbpaeojofohoefgiehjai") {
        return focusedText(QUrl(url.query()));
    }
    return url.toString(QUrl::FullyEncoded);
}
