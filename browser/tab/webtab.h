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
#ifndef WEBTAB_H
#define WEBTAB_H

#include <QWidget>
#include <QUrl>

namespace Ui {
    class WebTab;
}

class QSslCertificate;
class QWebEngineProfile;
class WebPage;
class QWebEngineHistory;
struct WebTabPrivate;
class WebTab : public QWidget
{
        Q_OBJECT

    public:
        explicit WebTab(WebPage* page = nullptr, QWidget* parent = nullptr);
        ~WebTab();

        QWidget* getTabButton();
        QUrl currentUrl();
        QString currentTitle();
        QWebEngineHistory* history();
        QWebEngineProfile* profile();

        QSslCertificate pageCertificate();

        QColor tabColor();
        QColor tabForegroundColor();

    public slots:
        void activated();
        void deactivated();
        void close();

        void goBack();
        void goForward();
        void navigate(QUrl url);
        void reload();

        void leaveFullScreen();
        void openDevtools();

    signals:
        void spawnTab(WebTab* tab);
        void urlChanged(QUrl url);
        void requestActivate();
        void closeTab();
        void sslStateChanged();
        void iconChanged();
        void titleChanged();

        void goFullScreen(bool fullscreen);
        void resized();

    private slots:
        void on_reloadAfterRenderCrashButton_clicked();

        void on_dangerousMoreDetailsButton_toggled(bool checked);

        void on_disregardDangerousWarningButton_clicked();

        void on_goBackAfterDeceptive_clicked();

        void on_stackedWidget_currentChanged(int arg1);

    private:
        Ui::WebTab *ui;
        WebTabPrivate* d;

        void resizeEvent(QResizeEvent *event);

        QString pageTitle();
};

#endif // WEBTAB_H
