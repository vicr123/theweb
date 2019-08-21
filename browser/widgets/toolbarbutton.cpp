#include "toolbarbutton.h"

#include <QPainter>
#include <QStyleOptionToolButton>

ToolbarButton::ToolbarButton(QWidget *parent) : QToolButton(parent)
{
    this->setMouseTracking(true);
}

void ToolbarButton::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(Qt::transparent);

    QStyleOptionToolButton styleOption;
    styleOption.initFrom(this);

    QColor backgroundCol = Qt::transparent;
    if (this->isDown()) {
        QColor windowCol = styleOption.palette.color(QPalette::Window);
        if ((windowCol.red() + windowCol.green() + windowCol.blue()) / 3 < 127) {
            backgroundCol = QColor(0, 0, 0, 50);
        } else {
            backgroundCol = QColor(0, 0, 0, 100);
        }
    } else if (styleOption.state & QStyle::State_MouseOver) {
        QColor windowCol = styleOption.palette.color(QPalette::Window);
        if ((windowCol.red() + windowCol.green() + windowCol.blue()) / 3 < 127) {
            backgroundCol = QColor(255, 255, 255, 50);
        } else {
            backgroundCol = QColor(0, 0, 0, 50);
        }
    }
    painter.setBrush(backgroundCol);
    painter.drawRect(0, 0, this->width(), this->height());

    QRect iconRect;
    iconRect.setSize(this->iconSize());
    iconRect.moveCenter(QPoint(this->width() / 2, this->height() / 2));
    painter.drawPixmap(iconRect, this->icon().pixmap(this->iconSize()));
}

void ToolbarButton::enterEvent(QEvent* event)
{
    this->update();
}

void ToolbarButton::leaveEvent(QEvent* event)
{
    this->update();
}
