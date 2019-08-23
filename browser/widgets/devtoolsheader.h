#ifndef DEVTOOLSHEADER_H
#define DEVTOOLSHEADER_H

#include <QWidget>

namespace Ui {
    class DevtoolsHeader;
}

class DevtoolsHeader : public QWidget
{
        Q_OBJECT

    public:
        explicit DevtoolsHeader(QWidget *parent = nullptr);
        ~DevtoolsHeader();

    signals:
        void closeDevtools();

    private slots:
        void on_closeButton_clicked();

    private:
        Ui::DevtoolsHeader *ui;
};

#endif // DEVTOOLSHEADER_H
