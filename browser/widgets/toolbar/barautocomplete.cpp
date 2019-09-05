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
#include "barautocomplete.h"

#include <the-libs_global.h>
#include <QPainter>
#include <QUrlQuery>

#include "managers/profilemanager.h"
#include "managers/historymanager.h"

struct AutocompleteEntry {
    QString text;
    QUrl url;
    QString action;
    QIcon icon;
};

struct BarAutocompletePrivate {
    QString query;
    QWebEngineProfile* profile;

    QList<AutocompleteEntry> entries;
};

BarAutocomplete::BarAutocomplete(QObject *parent)
    : QAbstractListModel(parent)
{
    d = new BarAutocompletePrivate();
}

BarAutocomplete::~BarAutocomplete()
{
    delete d;
}

int BarAutocomplete::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid()) return 0;

    return d->entries.count();
}

QVariant BarAutocomplete::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    AutocompleteEntry entry = d->entries.at(index.row());
    switch (role) {
        case Qt::DecorationRole:
            return entry.icon;
        case Qt::DisplayRole:
            return entry.text;
        case Qt::UserRole:
            return entry.url;
        case Qt::UserRole + 1:
            return entry.action;
    }

    return QVariant();
}

void BarAutocomplete::setQuery(QWebEngineProfile* profile, QString query)
{
    QList<AutocompleteEntry> priorityEntries;
    QList<AutocompleteEntry> entries;
    QList<AutocompleteEntry> lowEntries;
    d->query = query;

    if (QUrl::fromUserInput(query).isValid()) {
        AutocompleteEntry entry;
        entry.url = QUrl::fromUserInput(query);
        entry.text = entry.url.toString();
        entry.action = tr("Visit Webpage");
        entry.icon = QIcon::fromTheme("go-next");

        if (query.startsWith(entry.url.scheme()) || QRegularExpression("^.+\\.[^\\.]{2,}(?!.)$").match(entry.url.host()).hasMatch()) {
            //Prefer an entry that looks like a plausible URL
            priorityEntries.append(entry);
        } else {
            lowEntries.append(entry);
        }
    }

    {
        AutocompleteEntry entry;

        QUrl searchUrl;
        searchUrl.setHost("google.com");
        searchUrl.setScheme("https");
        searchUrl.setPath("/search");

        QUrlQuery urlQuery;
        urlQuery.addQueryItem("q", query);
        searchUrl.setQuery(urlQuery);

        entry.url = searchUrl;
        entry.text = query;
        entry.action = tr("Search on %1").arg("Google");
        entry.icon = QIcon::fromTheme("edit-find");
        entries.append(entry);
    }

    QList<QUrl> seenUrls;
    HistoryManager* history = HistoryManager::managerFor(profile);
    for (HistoryManager::HistoryEntry historyEntry : history->searchHistory(query)) {
        if (!seenUrls.contains(historyEntry.url)) {
            AutocompleteEntry entry;
            entry.url = historyEntry.url;
            entry.text = historyEntry.url.toString();
            entry.action = historyEntry.pageTitle;
            entry.icon = QIcon::fromTheme("view-history");

            if (entry.url.path() == "/" && (entry.url.host().startsWith(query) || entry.url.host().startsWith("www." + query))) {
                //Prefer this entry
                priorityEntries.append(entry);
            } else {
                lowEntries.append(entry);
            }

            seenUrls.append(historyEntry.url);
        }
    }

    d->entries.clear();
    d->entries.append(priorityEntries);
    d->entries.append(entries);
    d->entries.append(lowEntries);

    dataChanged(index(0), index(rowCount()));
}

BarAutocompleteDelegate::BarAutocompleteDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void BarAutocompleteDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->save();

    QColor textPen;
    bool fadeText;
    if (option.state & QStyle::State_Selected) {
        painter->setBrush(option.palette.brush(QPalette::Highlight));
        textPen = option.palette.color(QPalette::HighlightedText);
        fadeText = false;
    } else if (option.state & QStyle::State_MouseOver) {
        QColor col = option.palette.color(QPalette::Highlight);
        col.setAlpha(127);
        painter->setBrush(col);
        textPen = option.palette.color(QPalette::HighlightedText);
        fadeText = false;
    } else {
        painter->setBrush(Qt::transparent);
        textPen = option.palette.color(QPalette::WindowText);
        fadeText = true;
    }

    painter->setPen(Qt::transparent);
    painter->drawRect(option.rect);

    painter->setPen(textPen);

    QString mainText = option.fontMetrics.elidedText(index.data().toString(), Qt::ElideRight, static_cast<int>((option.rect.width() - this->leftOffset) * 0.75));

    QRect textRect = option.rect;
    textRect.setLeft(this->leftOffset);
    textRect.setWidth(option.fontMetrics.horizontalAdvance(mainText));

    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, mainText);

    QRect iconRect;
    iconRect.setSize(SC_DPI_T(QSize(16, 16), QSize));
    iconRect.moveCenter(option.rect.center());
    iconRect.moveRight(textRect.left() - SC_DPI(6));
    painter->drawPixmap(iconRect, index.data(Qt::DecorationRole).value<QIcon>().pixmap(iconRect.size()));

    QRect supplementaryTextRect = option.rect;
    supplementaryTextRect.setLeft(textRect.right() + SC_DPI(6));

    if (fadeText) painter->setOpacity(0.5);
    painter->drawText(supplementaryTextRect, Qt::AlignLeft | Qt::AlignVCenter, index.data(Qt::UserRole + 1).toString());
    painter->restore();
}

QSize BarAutocompleteDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize sh = QStyledItemDelegate::sizeHint(option, index);
    sh.setHeight(qMax(SC_DPI(16), option.fontMetrics.height()) + SC_DPI(6));
    sh.setWidth(option.widget->width());
    return sh;
}

void BarAutocompleteDelegate::setLeftOffset(int offset)
{
    this->leftOffset = offset;
}
