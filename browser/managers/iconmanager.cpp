#include "iconmanager.h"
#include <QPainter>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWebEngineUrlRequestJob>
#include "core/thewebschemehandler.h"

QIcon IconManager::getIcon(QString iconName, QColor tint, QSize size)
{
    QImage image = QIcon::fromTheme(iconName, QIcon(QStringLiteral(":/icons/%1.svg").arg(iconName))).pixmap(size).toImage();
    tintImage(image, tint);
    return QIcon(QPixmap::fromImage(image));
}

void IconManager::tintImage(QImage& image, QColor tint)
{
    int failNum = 0;
    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            QColor pixelCol = image.pixelColor(x, y);
            if ((pixelCol.blue() > pixelCol.green() - 10 && pixelCol.blue() < pixelCol.green() + 10) &&
                    (pixelCol.green() > pixelCol.red() - 10 && pixelCol.green() < pixelCol.red() + 10)) {
            } else {
                failNum++;
            }
        }
    }

    if (failNum < (image.size().width() * image.size().height()) / 8) {
        QPainter painter(&image);
        painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter.fillRect(0, 0, image.width(), image.height(), tint);
        painter.end();
    }
}

IconManager::IconManager(QObject *parent) : QObject(parent)
{

}
