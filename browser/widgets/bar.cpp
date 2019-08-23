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
#include <the-libs_global.h>
#include "tab/webtab.h"
#include "securitychunk.h"

struct BarPrivate {
    QPointer<WebTab> currentTab;
//    SecurityChunk* securityChunk;
};

Bar::Bar(QWidget *parent) : QLineEdit(parent)
{
    d = new BarPrivate();

//    d->securityChunk = new SecurityChunk(this);
//    d->securityChunk->setParent(this);
//    d->securityChunk->move(0, 0);
//    d->securityChunk->setFixedHeight(this->height());
//    d->securityChunk->setVisible(true);
//    connect(d->securityChunk, &SecurityChunk::resized, this, [=] {
//        if (d->securityChunk->isVisible()) this->setContentsMargins(d->securityChunk->width(), 0, 0, 0);
//    });

    this->setFrame(false);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(this, &Bar::returnPressed, this, [=] {
        d->currentTab->navigate(QUrl::fromUserInput(this->text()));
        d->currentTab->setFocus();
    });

    this->setCursor(QCursor(Qt::ArrowCursor));
    this->setPlaceholderText(tr("Search or enter web address"));
}

Bar::~Bar() {
    delete d;
}

#include <QDebug>
void Bar::setCurrentTab(WebTab* tab)
{
    //Disconnect all signals from the current tab
    if (!d->currentTab.isNull()) {
        disconnect(d->currentTab, nullptr, this, nullptr);
    }

    d->currentTab = tab;

    if (tab == nullptr) {
        this->setText("");
    } else {
        connect(tab, &WebTab::urlChanged, this, &Bar::updateInformation);

        if (tab->currentUrl().scheme() == "theweb" && tab->currentUrl().host() == "newtab") {
            this->setText("");
        } else if (this->hasFocus()) {
            this->setText(tab->currentUrl().toString());
        } else {
            this->setText(tab->currentUrl().host());
        }
    }
}

void Bar::updateInformation() {
    if (!this->hasFocus()) {
        if (d->currentTab.isNull() || d->currentTab->currentUrl().scheme() == "theweb" && d->currentTab->currentUrl().host() == "newtab") {
            this->setText("");
        } else {
            this->setText(d->currentTab->currentUrl().host());
        }
    }

    this->update();
}

void Bar::focusInEvent(QFocusEvent* event)
{
    QLineEdit::focusInEvent(event);
    this->setCursor(QCursor(Qt::IBeamCursor));
    if (d->currentTab.isNull() || d->currentTab->currentUrl().scheme() == "theweb" && d->currentTab->currentUrl().host() == "newtab") {
        this->setText("");
    } else {
        this->setText(d->currentTab->currentUrl().toString());
    }
//    d->securityChunk->setVisible(false);
    QTimer::singleShot(0, this, &Bar::selectAll);
}

void Bar::focusOutEvent(QFocusEvent* event)
{
    QLineEdit::focusOutEvent(event);
    this->setCursor(QCursor(Qt::ArrowCursor));
    if (d->currentTab.isNull() || d->currentTab->currentUrl().scheme() == "theweb" && d->currentTab->currentUrl().host() == "newtab") {
        this->setText("");
    } else {
        this->setText(d->currentTab->currentUrl().host());
    }
    this->setSelection(0, 0);
//    d->securityChunk->setVisible(true);
}

void Bar::paintEvent(QPaintEvent* event)
{
    QLineEdit::paintEvent(event);
}

void Bar::resizeEvent(QResizeEvent* event)
{

}
