#include "toolbarbutton.h"

#include <QPainter>
#include <QStyleOptionToolButton>
#include <the-libs_global.h>

struct ToolbarButtonPrivate {
    bool oblivion = false;
};

ToolbarButton::ToolbarButton(QWidget *parent) : QToolButton(parent)
{
    d = new ToolbarButtonPrivate();
    this->setMouseTracking(true);
}

ToolbarButton::~ToolbarButton()
{
    delete d;
}

void ToolbarButton::setAsOblivion()
{
    d->oblivion = true;

    QPalette pal = this->palette();
    pal.setColor(QPalette::Window, QColor(100, 0, 200));
    this->setPalette(pal);

    this->setIcon(QIcon(":/icons/theweb-mono.svg"));
}

QSize ToolbarButton::sizeHint() const
{
    QSize sh = QToolButton::sizeHint();
    if (d->oblivion) {
        sh.rwidth() += this->fontMetrics().horizontalAdvance(tr("Oblivion")) + SC_DPI(6);
    }
    return sh;
}

void ToolbarButton::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(Qt::transparent);

    QStyleOptionToolButton styleOption;
    styleOption.initFrom(this);

    if (d->oblivion) {
        painter.setBrush(styleOption.palette.color(QPalette::Window));
        painter.drawRect(0, 0, this->width(), this->height());
    }

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
    QSize originalSizeHint = QToolButton::sizeHint();
    originalSizeHint.setHeight(this->height());
    iconRect.setSize(this->iconSize());
    iconRect.moveCenter(QPoint(originalSizeHint.width() / 2, originalSizeHint.height() / 2));
    painter.drawPixmap(iconRect, this->icon().pixmap(this->iconSize()));

    if (d->oblivion) {
        painter.setFont(this->font());
        painter.setBrush(Qt::transparent);
        painter.setPen(Qt::white);
        QRect textRect;
        textRect.setWidth(this->fontMetrics().horizontalAdvance(tr("Oblivion")) + 1);
        textRect.setHeight(this->fontMetrics().height());
        textRect.moveTop(this->height() / 2 - textRect.height() / 2);
        textRect.moveLeft(iconRect.right() + SC_DPI(6));
        painter.drawText(textRect, tr("Oblivion"));
    }
}

void ToolbarButton::enterEvent(QEvent* event)
{
    this->update();
}

void ToolbarButton::leaveEvent(QEvent* event)
{
    this->update();
}
