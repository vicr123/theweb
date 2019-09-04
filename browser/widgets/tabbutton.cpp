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
#include "tabbutton.h"
#include <QPainter>
#include <tcircularspinner.h>
#include <tvariantanimation.h>
#include <QGraphicsOpacityEffect>

struct TabButtonPrivate {
    QColor backgroundCol;
    QColor foregroundCol;

//    int loadProgress = 0;
//    bool showLoadProgress = false;
    tVariantAnimation* loadProgress;
    tVariantAnimation* showLoadProgress;
    tVariantAnimation* spinnerOpacity;

    tCircularSpinner* spinner;
    QGraphicsOpacityEffect* spinnerOpacityEffect;
};

TabButton::TabButton(QWidget *parent) : QPushButton(parent)
{
    d = new TabButtonPrivate();

    d->backgroundCol = this->palette().color(QPalette::Window);
    d->foregroundCol = this->palette().color(QPalette::WindowText);

    d->loadProgress = new tVariantAnimation();
    d->loadProgress->setStartValue(0);
    d->loadProgress->setEndValue(0);
    d->loadProgress->setDuration(500);
    d->loadProgress->setEasingCurve(QEasingCurve::OutCubic);
    connect(d->loadProgress, &tVariantAnimation::valueChanged, this, QOverload<>::of(&TabButton::update));

    d->showLoadProgress = new tVariantAnimation();
    d->showLoadProgress->setStartValue(0.0);
    d->showLoadProgress->setEndValue(0.0);
    d->showLoadProgress->setDuration(500);
    d->showLoadProgress->setEasingCurve(QEasingCurve::OutCubic);
    connect(d->showLoadProgress, &tVariantAnimation::valueChanged, this, QOverload<>::of(&TabButton::update));

    d->spinnerOpacity = new tVariantAnimation();
    d->spinnerOpacity->setStartValue(-1.0);
    d->spinnerOpacity->setEndValue(1.0);
    d->spinnerOpacity->setDuration(500);
//    d->spinnerOpacity->setEasingCurve(QEasingCurve::OutCubic);
    connect(d->spinnerOpacity, &tVariantAnimation::valueChanged, this, QOverload<>::of(&TabButton::update));

    d->spinner = new tCircularSpinner(this);
    d->spinner->resize(this->iconSize());
    d->spinner->setParent(this);

    d->spinnerOpacityEffect = new QGraphicsOpacityEffect(d->spinner);
    d->spinner->setGraphicsEffect(d->spinnerOpacityEffect);
}

TabButton::~TabButton() {
    d->loadProgress->deleteLater();
    d->showLoadProgress->deleteLater();
    d->spinnerOpacity->deleteLater();
    d->spinner->deleteLater();
    delete d;
}

QSize TabButton::sizeHint() const
{
    QSize sizeHint = QPushButton::sizeHint();
    sizeHint.setHeight(qMax(this->fontMetrics().height(), this->iconSize().height()) + SC_DPI(12));
    return sizeHint;
}

QColor TabButton::color()
{
    return d->backgroundCol.darker();
}

QColor TabButton::foregroundColor()
{
    return d->foregroundCol;
}

void TabButton::setLoadProgress(int progress, bool show)
{
    d->loadProgress->stop();
    if (d->showLoadProgress->currentValue() == 0) {
        d->loadProgress->setStartValue(progress);
        d->loadProgress->setEndValue(progress);
        d->loadProgress->setCurrentTime(0);
    } else {
        d->loadProgress->setStartValue(d->loadProgress->currentValue());
        d->loadProgress->setEndValue(progress);
        d->loadProgress->start();
    }

    if (d->showLoadProgress->endValue() == 0.0 && show) {
        d->showLoadProgress->stop();
        d->showLoadProgress->setStartValue(d->showLoadProgress->currentValue());
        d->showLoadProgress->setEndValue(1.0);
        d->showLoadProgress->start();

        d->spinnerOpacity->stop();
        if (d->spinnerOpacity->currentValue() != d->spinnerOpacity->endValue()) {
            d->spinnerOpacity->setDirection(tVariantAnimation::Forward);
            d->spinnerOpacity->start();
        }
    } else if (d->showLoadProgress->endValue() == 1.0 && !show) {
        d->showLoadProgress->stop();
        d->showLoadProgress->setStartValue(d->showLoadProgress->currentValue());
        d->showLoadProgress->setEndValue(0.0);
        d->showLoadProgress->start();

        d->spinnerOpacity->stop();
        if (d->spinnerOpacity->currentValue() != d->spinnerOpacity->startValue()) {
            d->spinnerOpacity->setDirection(tVariantAnimation::Backward);
            d->spinnerOpacity->start();
        }
    }
    this->update();
}

void TabButton::setIcon(QIcon icon)
{
    QPushButton::setIcon(icon);

    QPixmap pm = icon.pixmap(this->iconSize());

    qulonglong red = 0, green = 0, blue = 0;

    int totalPixels = 0;
    QImage im = pm.toImage();
    for (int i = 0; i < pm.width(); i++) {
        for (int j = 0; j < pm.height(); j++) {
            QColor c = im.pixelColor(i, j);
            if (c.alpha() != 0) {
                //i = pm.width();
                //j = pm.height();

                //c.setAlpha(255);
                //QPalette pal = coverWidget->palette();
                //pal.setColor(QPalette::Window, c.darker(150));
                //coverWidget->setPalette(pal);
                red += c.red();
                green += c.green();
                blue += c.blue();
                totalPixels++;
            }
        }
    }

    QColor background, foreground;
    int averageCol = (background.red() + background.green() + background.blue()) / 3;

    if (totalPixels == 0) {
        background = QPalette().color(QPalette::Window);
        foreground = QPalette().color(QPalette::WindowText);
    } else {
        background = QColor(red / totalPixels, green / totalPixels, blue / totalPixels);

        if (averageCol < 127) {
            background = background.darker(150);
            foreground = Qt::white;
        } else {
            background = background.lighter(150);
            foreground = Qt::black;
        }
    }

    tVariantAnimation* bgAnim = new tVariantAnimation(this);
    bgAnim->setStartValue(d->backgroundCol);
    bgAnim->setEndValue(background);
    bgAnim->setEasingCurve(QEasingCurve::OutCubic);
    bgAnim->setDuration(500);
    connect(bgAnim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
        QPalette pal = this->palette();

        pal.setColor(QPalette::Button, value.value<QColor>());
        pal.setColor(QPalette::ButtonText, foreground);
        this->setPalette(pal);
        d->backgroundCol = value.value<QColor>();
        d->foregroundCol = foreground;

        emit paletteUpdated();
    });
    connect(bgAnim, &tVariantAnimation::finished, bgAnim, &tVariantAnimation::deleteLater);
    bgAnim->start();
}

void TabButton::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPalette pal = this->palette();
    QRect rect(0, 0, this->width(), this->height());

    painter.setPen(Qt::transparent);

    QBrush brush = d->backgroundCol;
    QPen textPen = d->foregroundCol;

    /*if (button->state & QStyle::State_HasFocus) {
        brush = QBrush(pal.color(QPalette::Button).lighter(125));
    }

    if (button->state & QStyle::State_MouseOver) {
        brush = QBrush(pal.color(QPalette::Button).lighter());
    }*/

    if (this->isChecked()) {
        brush = d->backgroundCol.darker();
    }
    painter.setBrush(brush);
    painter.drawRect(rect);

    QString text = this->text();

    int widthNeeded = 0;
    widthNeeded += this->fontMetrics().horizontalAdvance(text);
    if (!this->icon().isNull()) widthNeeded += SC_DPI(6) + this->iconSize().width();

    QRect contentRect;
    contentRect.setWidth(widthNeeded);
    contentRect.setHeight(qMax(this->icon().isNull() ? 0 : this->iconSize().height(), this->fontMetrics().height()));
    contentRect.moveCenter(QPoint(this->width() / 2, this->height() / 2));

    bool overflow = false;
    if (contentRect.left() < SC_DPI(9)) {
        overflow = true;
        contentRect.moveLeft(SC_DPI(9));
    }

    QRect textRect;
    textRect.setHeight(this->fontMetrics().height());
    textRect.setWidth(this->fontMetrics().horizontalAdvance(text));
    textRect.moveLeft(contentRect.left());
    textRect.moveTop(contentRect.top() + contentRect.height() / 2 - textRect.height() / 2);

    if (!this->icon().isNull() || d->spinnerOpacity->currentValue() >= 0) {
        QRect iconRect;
        iconRect.setSize(this->iconSize());
        iconRect.moveLeft(contentRect.left());
        iconRect.moveTop(contentRect.top() + contentRect.height() / 2 - iconRect.height() / 2);

        textRect.moveLeft(iconRect.right() + SC_DPI(6));

        painter.save();
        if (d->spinnerOpacity->currentValue() >= 0) {
            d->spinnerOpacityEffect->setOpacity(d->spinnerOpacity->currentValue().toDouble());
            d->spinner->setGeometry(iconRect);
//            d->spinner->render(&painter, iconRect.topLeft());
        } else {
            d->spinnerOpacityEffect->setOpacity(0);
            painter.setOpacity(qAbs(d->spinnerOpacity->currentValue().toDouble()));

            QIcon icon = this->icon();
            QImage image = icon.pixmap(this->iconSize()).toImage();
            image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);

            painter.drawImage(iconRect, image);
        }
        painter.restore();
    }

    //Draw text
    painter.setPen(textPen);
    painter.drawText(textRect, Qt::AlignCenter, text.remove("&"));

    if (overflow) {
        //Draw overflow fade
        QLinearGradient gradient;
        gradient.setStart(this->width() - SC_DPI(20), 0);
        gradient.setFinalStop(this->width(), 0);

        QColor firstColor = brush.color();
        firstColor.setAlpha(0);
        gradient.setColorAt(0, firstColor);
        gradient.setColorAt(1, brush.color());

        painter.setBrush(gradient);
        painter.setPen(Qt::transparent);
        painter.drawRect(this->width() - SC_DPI(20), 0, SC_DPI(20), this->height());
    }

    if (d->showLoadProgress->currentValue() != 0) {
        //Draw progress indication
        painter.setPen(Qt::transparent);
        painter.setBrush(QColor(0, 150, 0));
        painter.setOpacity(d->showLoadProgress->currentValue().toDouble());
        painter.drawRect(0, this->height() - SC_DPI(3), static_cast<int>(this->width() * static_cast<float>(d->loadProgress->currentValue().toFloat() / 100)), this->height() - SC_DPI(3));
    }


}
