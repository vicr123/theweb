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
#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebEnginePage>

struct WebPagePrivate;
class CertificateErrorPane;
class WebPage : public QWebEnginePage
{
        Q_OBJECT
    public:
        explicit WebPage(QWidget *parent);
        explicit WebPage(QWebEngineProfile *profile, QWidget *parent);
        ~WebPage();

        void setParent(QWidget* parent);
        void setCertificateErrorPane(CertificateErrorPane* pane);

        QMenu* createStandardContextMenu();

    signals:
        void openDevtools();
        void spawnTab(WebPage* page, WebWindowType type);

    public slots:

    private:
        QWebEnginePage* createWindow(WebWindowType type);
        bool certificateError(const QWebEngineCertificateError &certificateError);
        void javaScriptAlert(const QUrl &securityOrigin, const QString &msg);
        bool javaScriptConfirm(const QUrl &securityOrigin, const QString &msg);
        WebPagePrivate* d;
};

#endif // WEBPAGE_H
