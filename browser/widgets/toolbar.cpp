#include "toolbar.h"
#include "ui_toolbar.h"

#include <the-libs_global.h>
#include <QIcon>
#include <QPointer>
#include <QSslCertificate>
#include <QMenu>
#include <QListView>
#include <QWebEngineHistory>
#include <tvariantanimation.h>
#include <tpopover.h>
#include "downloads/downloadspopover.h"
#include "managers/iconmanager.h"
#include "managers/downloadmanager.h"
#include "tab/webtab.h"
#include "tabbutton.h"

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
    ui->downloadsButton->setIconSize(iconSize);
    ui->newTabButton->setIconSize(iconSize);

    ui->auxContainer->setFixedHeight(0);
    ui->auxStack->addWidget(ui->bar->getAutocompleteWidget());

    connect(DownloadManager::instance(), &DownloadManager::downloadAdded, this, &Toolbar::updateIcons);
    updateIcons();

    QMenu* backMenu = new QMenu();
    connect(backMenu, &QMenu::aboutToShow, this, [=] {
        backMenu->clear();
        backMenu->addSection(tr("Go Back"));

        //Iterate in reverse order
        QList<QWebEngineHistoryItem> history = d->currentTab->history()->backItems(20);
        for (auto i = history.crbegin(); i != history.crend(); i++) {
            QWebEngineHistoryItem item = *i;
            backMenu->addAction(i->title(), [=] {
                d->currentTab->history()->goToItem(item);
            });
        }
    });
    ui->backButton->setMenu(backMenu);

    QMenu* forwardMenu = new QMenu();
    connect(forwardMenu, &QMenu::aboutToShow, this, [=] {
        forwardMenu->clear();
        forwardMenu->addSection(tr("Go Forward"));

        QList<QWebEngineHistoryItem> history = d->currentTab->history()->forwardItems(20);
        for (auto i = history.cbegin(); i != history.cend(); i++) {
            QWebEngineHistoryItem item = *i;
            forwardMenu->addAction(i->title(), [=] {
                d->currentTab->history()->goToItem(item);
            });
        }
    });
    ui->forwardButton->setMenu(forwardMenu);

    this->setAutoFillBackground(true);

    connect(ui->securityChunk, &SecurityChunk::toggleSecurityWidget, this, [=] {
        ui->auxStack->setCurrentWidget(ui->securityPage);

        tVariantAnimation* anim = new tVariantAnimation();
        anim->setStartValue(ui->auxContainer->height());
        anim->setEndValue(ui->auxContainer->height() == 0 ? ui->securityPage->height() : 0);
        anim->setDuration(500);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        connect(anim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
            ui->auxContainer->setFixedHeight(value.toInt());
        });
        connect(anim, &tVariantAnimation::finished, anim, &tVariantAnimation::deleteLater);
        anim->start();
    });
    connect(ui->bar, &Bar::showAutocompleteWidget, this, [=] {
        QListView* lv = qobject_cast<QListView*>(ui->bar->getAutocompleteWidget());
        ui->auxStack->setCurrentWidget(lv);

        tVariantAnimation* anim = new tVariantAnimation();
        anim->setStartValue(ui->auxContainer->height());
        anim->setEndValue(lv->sizeHintForRow(0) * 8);
        anim->setDuration(500);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        connect(anim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
            ui->bar->getAutocompleteWidget()->setFixedHeight(value.toInt());
            ui->auxContainer->setFixedHeight(value.toInt());
        });
        connect(anim, &tVariantAnimation::finished, anim, &tVariantAnimation::deleteLater);
        anim->start();
    });
    connect(ui->bar, &Bar::hideAutocompleteWidget, this, [=] {
        tVariantAnimation* anim = new tVariantAnimation();
        anim->setStartValue(ui->auxContainer->height());
        anim->setEndValue(0);
        anim->setDuration(500);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        connect(anim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
            ui->bar->getAutocompleteWidget()->setFixedHeight(value.toInt());
            ui->auxContainer->setFixedHeight(value.toInt());
        });
        connect(anim, &tVariantAnimation::finished, anim, &tVariantAnimation::deleteLater);
        anim->start();
    });
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
        disconnect(d->currentTab->getTabButton());
    }

    d->currentTab = tab;

    if (tab != nullptr) {
        connect(tab, &WebTab::urlChanged, this, &Toolbar::updateInformation);
        connect(tab, &WebTab::sslStateChanged, this, &Toolbar::updateInformation);
        connect(tab, &WebTab::titleChanged, this, &Toolbar::updateInformation);
        connect(tab, &WebTab::iconChanged, this, &Toolbar::updateIcons);
        connect(qobject_cast<TabButton*>(tab->getTabButton()), &TabButton::paletteUpdated, this, &Toolbar::updateIcons);

        ui->securityPage->setProfile(d->currentTab->profile());

        updateIcons();

        ui->bar->setCurrentTab(tab);
        ui->securityChunk->setCurrentCertificate(d->currentTab->currentUrl(), d->currentTab->pageCertificate());
        ui->securityPage->setCurrentCertificate(d->currentTab->currentUrl(), d->currentTab->pageCertificate());
        ui->backButton->setEnabled(tab->history()->canGoBack());
        ui->forwardButton->setEnabled(tab->history()->canGoForward());
        this->window()->setWindowTitle(QStringLiteral("%1 - theWeb").arg(tab->currentTitle()));
    }
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
    ui->securityPage->setCurrentCertificate(d->currentTab->currentUrl(), d->currentTab->pageCertificate());
    ui->backButton->setEnabled(d->currentTab->history()->canGoBack());
    ui->forwardButton->setEnabled(d->currentTab->history()->canGoForward());
    this->window()->setWindowTitle(QStringLiteral("%1 - theWeb").arg(d->currentTab->currentTitle()));
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
    ui->downloadsButton->setIcon(IconManager::getIcon("cloud-download", tint, iconSize));
    ui->newTabButton->setIcon(IconManager::getIcon("tab-new", tint, iconSize));

    if (DownloadManager::getDownloads().count() > 0) {
        ui->downloadsButton->setVisible(true);
    } else {
        ui->downloadsButton->setVisible(false);
    }
}

void Toolbar::on_reloadButton_clicked()
{
    d->currentTab->reload();
}

void Toolbar::on_downloadsButton_clicked()
{
    DownloadsPopover* d = new DownloadsPopover();
    tPopover* popover = new tPopover(d);
    popover->setPopoverWidth(SC_DPI(400));
    connect(d, &DownloadsPopover::dismiss, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, d, &DownloadsPopover::deleteLater);
    popover->show(this->window());
}

void Toolbar::on_newTabButton_clicked()
{
    emit newTabRequested();
}
