#include "toolbar.h"
#include "ui_toolbar.h"

#include <the-libs_global.h>
#include <QIcon>
#include <QPointer>
#include <QSslCertificate>
#include "managers/iconmanager.h"
#include "tab/webtab.h"

extern void tintImage(QImage &image, QColor tint);

struct ToolbarPrivate {
    QPointer<WebTab> currentTab;
};

Toolbar::Toolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Toolbar)
{
    ui->setupUi(this);

    d = new ToolbarPrivate();

    QSize iconSize = SC_DPI_T(QSize(24, 24), QSize);
    ui->menuButton->setIconSize(iconSize);
    ui->backButton->setIconSize(iconSize);
    ui->forwardButton->setIconSize(iconSize);
    ui->reloadButton->setIconSize(iconSize);

    updateIcons();

    this->setAutoFillBackground(true);
}

Toolbar::~Toolbar()
{
    delete ui;
    delete d;
}

void Toolbar::setMenu(QMenu* menu)
{
    ui->menuButton->setMenu(menu);
}

void Toolbar::setMenuIcon(QIcon icon)
{
    ui->menuButton->setIcon(icon);
}

void Toolbar::setCurrentTab(WebTab* tab)
{
    //Disconnect all signals from the current tab
    if (!d->currentTab.isNull()) {
        disconnect(d->currentTab, nullptr, this, nullptr);
    }

    d->currentTab = tab;
    connect(tab, &WebTab::urlChanged, this, &Toolbar::updateInformation);
    connect(tab, &WebTab::sslStateChanged, this, &Toolbar::updateInformation);
    connect(tab, &WebTab::iconChanged, this, &Toolbar::updateIcons);

    updateIcons();

    ui->bar->setCurrentTab(tab);
    ui->securityChunk->setCurrentCertificate(d->currentTab->currentUrl(), d->currentTab->pageCertificate());
}

void Toolbar::setAsOblivion()
{
    ui->menuButton->setAsOblivion();
}

void Toolbar::focusBar()
{
    ui->bar->setFocus();
}

void Toolbar::updateInformation()
{
    ui->securityChunk->setCurrentCertificate(d->currentTab->currentUrl(), d->currentTab->pageCertificate());
}

void Toolbar::on_backButton_clicked()
{
    d->currentTab->goBack();
}

void Toolbar::on_forwardButton_clicked()
{
    d->currentTab->goForward();
}

void Toolbar::updateIcons()
{
    QPalette pal = this->palette();
    if (!d->currentTab.isNull()) {
        pal.setColor(QPalette::Window, d->currentTab->tabColor());
        pal.setColor(QPalette::Base, d->currentTab->tabColor());
        pal.setColor(QPalette::WindowText, d->currentTab->tabForegroundColor());
        pal.setColor(QPalette::Text, d->currentTab->tabForegroundColor());
        this->setPalette(pal);
        ui->bar->setPalette(pal);
    }

    QColor tint = pal.color(QPalette::WindowText);
    QSize iconSize = SC_DPI_T(QSize(24, 24), QSize);
    ui->backButton->setIcon(IconManager::getIcon("go-previous", tint, iconSize));
    ui->forwardButton->setIcon(IconManager::getIcon("go-next", tint, iconSize));
    ui->reloadButton->setIcon(IconManager::getIcon("view-refresh", tint, iconSize));
}

void Toolbar::on_reloadButton_clicked()
{
    d->currentTab->reload();
}
