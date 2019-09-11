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
#include "findinpage.h"
#include "ui_findinpage.h"

#include <terrorflash.h>
#include <QMenu>
#include <QShortcut>
#include "webpage.h"

struct FindInPagePrivate {
    WebPage* page;
    int runningQueries = 0;

    QAction* caseSensitiveOption;
};

FindInPage::FindInPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FindInPage)
{
    ui->setupUi(this);
    d = new FindInPagePrivate();

    QMenu* optionsMenu = new QMenu();
    d->caseSensitiveOption = optionsMenu->addAction(tr("Match case"), this, [=] {
        find(true);
    });
    d->caseSensitiveOption->setCheckable(true);
    ui->optionsButton->setMenu(optionsMenu);

    QShortcut* exitShortcut = new QShortcut(QKeySequence("ESC"), this);
    connect(exitShortcut, &QShortcut::activated, this, [=] {
        this->hide();
    });

    this->setFocusProxy(ui->searchTextEdit);
}

FindInPage::~FindInPage()
{
    delete ui;
    delete d;
}

void FindInPage::setWebPage(WebPage* page)
{
    d->page = page;
}

void FindInPage::find(bool forwards)
{
    WebPage::FindFlags findFlags;
    if (!forwards) findFlags |= WebPage::FindBackward;
    if (d->caseSensitiveOption->isChecked()) findFlags |= WebPage::FindCaseSensitively;

    QString query = ui->searchTextEdit->text();
    d->runningQueries++;
    d->page->findText(query, findFlags, [=](bool success) {
        d->runningQueries--;
        if (!success && !query.isEmpty() && d->runningQueries == 0) {
            tErrorFlash::flashError(this);
        }
    });
}

void FindInPage::show()
{
    QWidget::show();
    ui->searchTextEdit->setFocus();
    find(true);
}

void FindInPage::hide()
{
    QWidget::hide();
    d->runningQueries++;
    d->page->findText("", WebPage::FindFlags(), [=](bool success) {
        d->runningQueries--;
    }); //Clear the find options
}

void FindInPage::on_closeButton_clicked()
{
    this->hide();
}

void FindInPage::on_findNextButton_clicked()
{
    find(true);
}

void FindInPage::on_findPreviousButton_clicked()
{
    find(false);
}

void FindInPage::on_searchTextEdit_textChanged(const QString &arg1)
{
    find(true);
}

void FindInPage::on_searchTextEdit_returnPressed()
{
    find(true);
}
