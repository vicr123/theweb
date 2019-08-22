#ifndef DOWNLOADSPOPOVER_H
#define DOWNLOADSPOPOVER_H

#include <QWidget>

namespace Ui {
    class DownloadsPopover;
}

class DownloadsPopover : public QWidget
{
        Q_OBJECT

    public:
        explicit DownloadsPopover(QWidget *parent = nullptr);
        ~DownloadsPopover();

    private slots:
        void on_backButton_clicked();

    signals:
        void dismiss();

    private:
        Ui::DownloadsPopover *ui;
};

#endif // DOWNLOADSPOPOVER_H
