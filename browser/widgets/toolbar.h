#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QWidget>

namespace Ui {
    class Toolbar;
}

struct ToolbarPrivate;
class WebTab;
class QMenu;
class Toolbar : public QWidget
{
        Q_OBJECT

    public:
        explicit Toolbar(QWidget *parent = nullptr);
        ~Toolbar();

        void setMenu(QMenu* menu);
        void setMenuIcon(QIcon icon);
        void setCurrentTab(WebTab* tab);

        void setAsOblivion();

    public slots:
        void focusBar();

    private slots:
        void updateInformation();

        void on_backButton_clicked();

        void on_forwardButton_clicked();

        void on_reloadButton_clicked();

        void on_downloadsButton_clicked();

    private:
        Ui::Toolbar *ui;
        ToolbarPrivate* d;

        void updateIcons();
};

#endif // TOOLBAR_H
