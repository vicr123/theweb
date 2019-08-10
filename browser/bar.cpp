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
#include "tab/webtab.h"

struct BarPrivate {
    QPointer<WebTab> currentTab;
};

Bar::Bar(QWidget *parent) : QLineEdit(parent)
{
    d = new BarPrivate();

    connect(this, &Bar::returnPressed, this, [=] {
        d->currentTab->navigate(QUrl::fromUserInput(this->text()));
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

    if (this->hasFocus()) {
        this->setText(tab->currentUrl().toString());
    } else {
        this->setText(tab->currentUrl().host());
    }
}

void Bar::updateInformation() {
    if (!this->hasFocus()) {
        this->setText(d->currentTab->currentUrl().host());
    }
}

void Bar::focusInEvent(QFocusEvent* event)
{
    QLineEdit::focusInEvent(event);
    this->setCursor(QCursor(Qt::IBeamCursor));
    this->setText(d->currentTab->currentUrl().toString());
    QTimer::singleShot(0, this, &Bar::selectAll);
}

void Bar::focusOutEvent(QFocusEvent* event)
{
    QLineEdit::focusOutEvent(event);
    this->setCursor(QCursor(Qt::ArrowCursor));
    this->setText(d->currentTab->currentUrl().host());
}
