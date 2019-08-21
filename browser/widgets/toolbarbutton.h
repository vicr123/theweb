#ifndef TOOLBARBUTTON_H
#define TOOLBARBUTTON_H

#include <QToolButton>

class ToolbarButton : public QToolButton
{
        Q_OBJECT
    public:
        explicit ToolbarButton(QWidget *parent = nullptr);

    signals:

    public slots:

    private:
        void paintEvent(QPaintEvent* event);
        void enterEvent(QEvent* event);
        void leaveEvent(QEvent* event);
};

#endif // TOOLBARBUTTON_H
