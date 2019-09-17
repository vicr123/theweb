#ifndef ICONMANAGER_H
#define ICONMANAGER_H

#include <QObject>
#include <QSize>
#include <QIcon>

class IconManager : public QObject
{
        Q_OBJECT
    public:
        static QIcon getIcon(QString iconName, QColor tint, QSize size = QSize(24, 24));
        static QIcon getIcon(QString iconName, QPalette palette, QSize size = QSize(24, 24));
        static void tintImage(QImage &image, QColor tint);

    signals:

    public slots:

    private:
        explicit IconManager(QObject *parent = nullptr);

};

#endif // ICONMANAGER_H
