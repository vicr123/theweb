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
    SecurityChunk* securityChunk;
};

Bar::Bar(QWidget *parent) : QLineEdit(parent)
{
    d = new BarPrivate();

    d->securityChunk = new SecurityChunk(this);
    d->securityChunk->setParent(this);
    d->securityChunk->move(0, 0);
    d->securityChunk->setFixedHeight(this->height());
    d->securityChunk->setVisible(true);
    connect(d->securityChunk, &SecurityChunk::resized, this, [=] {
        if (d->securityChunk->isVisible()) this->setContentsMargins(d->securityChunk->width(), 0, 0, 0);
    });

    this->setFrame(false);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(this, &Bar::returnPressed, this, [=] {
        d->currentTab->navigate(QUrl::fromUserInput(this->text()));
        d->currentTab->setFocus();
    });

    this->setCursor(QCursor(Qt::ArrowCursor));
}

Bar::~Bar() {
    delete d;
}

void Bar::setCurrentTab(WebTab* tab)
{
    //Disconnect all signals from the current tab
    if (!d->currentTab.isNull()) {
        disconnect(d->currentTab, nullptr, this, nullptr);
    }

    d->currentTab = tab;
    connect(tab, &WebTab::urlChanged, this, &Bar::updateInformation);
    connect(tab, &WebTab::loadProgressChanged, this, &Bar::updateInformation);
    connect(tab, &WebTab::sslStateChanged, this, &Bar::updateInformation);

    if (this->hasFocus()) {
        this->setText(tab->currentUrl().toString());
    } else {
        this->setText(tab->currentUrl().host());
    }
    d->securityChunk->setCurrentCertificate(d->currentTab->currentUrl(), d->currentTab->pageCertificate());
}

void Bar::updateInformation() {
    if (!this->hasFocus()) {
        this->setText(d->currentTab->currentUrl().host());
    }

    d->securityChunk->setCurrentCertificate(d->currentTab->currentUrl(), d->currentTab->pageCertificate());
    this->update();
}

void Bar::focusInEvent(QFocusEvent* event)
{
    QLineEdit::focusInEvent(event);
    this->setCursor(QCursor(Qt::IBeamCursor));
    this->setText(d->currentTab->currentUrl().toString());
    d->securityChunk->setVisible(false);
    this->setContentsMargins(0, 0, 0, 0);
    QTimer::singleShot(0, this, &Bar::selectAll);
}

void Bar::focusOutEvent(QFocusEvent* event)
{
    QLineEdit::focusOutEvent(event);
    this->setCursor(QCursor(Qt::ArrowCursor));
    this->setText(d->currentTab->currentUrl().host());
    d->securityChunk->setVisible(true);
    this->setContentsMargins(d->securityChunk->width(), 0, 0, 0);
}

void Bar::paintEvent(QPaintEvent*event)
{
    QPainter painter(this);

    if (d->currentTab->isLoading()) {
        painter.setPen(Qt::transparent);
        painter.setBrush(QColor(0, 150, 0));
        painter.drawRect(0, this->height() - SC_DPI(3), static_cast<int>(this->width() * static_cast<float>(d->currentTab->loadProgress()) / 100), this->height() - SC_DPI(3));
    }
    QLineEdit::paintEvent(event);
}

void Bar::resizeEvent(QResizeEvent* event)
{
    d->securityChunk->setFixedHeight(this->height());
}
