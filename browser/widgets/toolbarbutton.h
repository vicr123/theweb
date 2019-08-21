#ifndef TOOLBARBUTTON_H
#define TOOLBARBUTTON_H

#include <QToolButton>

struct ToolbarButtonPrivate;
class ToolbarButton : public QToolButton
{
        Q_OBJECT
    public:
        explicit ToolbarButton(QWidget *parent = nullptr);
        ~ToolbarButton();

        void setAsOblivion();

        QSize sizeHint() const;

    signals:

    public slots:

    private:
        void paintEvent(QPaintEvent* event);
        void enterEvent(QEvent* event);
        void leaveEvent(QEvent* event);

        ToolbarButtonPrivate* d;
};

#endif // TOOLBARBUTTON_H
