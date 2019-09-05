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
#include "securityinfopermissionswitch.h"
#include "ui_securityinfopermissionswitch.h"

#include <QUrl>

struct SecurityInfoPermissionSwitchPrivate {
    FeatureManager* mgr;
    QUrl originUrl;
    FeatureManager::Feature feature;
};

SecurityInfoPermissionSwitch::SecurityInfoPermissionSwitch(FeatureManager* mgr, QUrl originUrl, FeatureManager::Feature feature, FeatureManager::FeatureAllowed defaultAllowed, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SecurityInfoPermissionSwitch)
{
    ui->setupUi(this);

    d = new SecurityInfoPermissionSwitchPrivate();
    d->mgr = mgr;
    d->originUrl = originUrl;
    d->feature = feature;

    ui->featureOn->setChecked(defaultAllowed == FeatureManager::Allow);

    switch (feature) {
        case FeatureManager::Notifications:
            ui->featureName->setText(tr("Notifications"));
            break;
        case FeatureManager::ScreenRecord:
            ui->featureName->setText(tr("Screen Recording"));
            break;
        case FeatureManager::Unknown:
            break;
        case FeatureManager::Location:
            ui->featureName->setText(tr("Location"));
            break;
        case FeatureManager::Camera:
            ui->featureName->setText(tr("Camera"));
            break;
        case FeatureManager::Microphone:
            ui->featureName->setText(tr("Microphone"));
            break;
        case FeatureManager::LastFeature:
            break;

    }
}

SecurityInfoPermissionSwitch::~SecurityInfoPermissionSwitch()
{
    delete ui;
    delete d;
}

void SecurityInfoPermissionSwitch::on_featureOn_toggled(bool checked)
{
    if (checked) {
        d->mgr->setFeatureAllowed(d->originUrl, d->feature, FeatureManager::Allow);
    } else {
        d->mgr->setFeatureAllowed(d->originUrl, d->feature, FeatureManager::Deny);
    }
}
