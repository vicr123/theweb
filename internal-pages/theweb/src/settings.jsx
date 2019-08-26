import React from 'react';
import './App.css';
import { Header, MiniHeader } from './textblocks.jsx'
import { VerticalLayout, HorizontalLine } from './layouts.jsx'
import ListItem from './listitem.jsx'
import { Translation } from 'react-i18next';
import { RadioButtonGroup, LineEdit, CheckBox } from './settingstoggles.jsx'
import { settings } from './api.js';
import Modal from './modal.jsx'

class SettingsSidebar extends React.Component {
    render() {
        let _ = (paneName) => {
            return () => this.props.onChange(paneName);
        }
        
        let sel = (paneName) => {
            return this.props.currentPane === paneName ? "true" : "false";
        }
        
        return <Translation>{(t, {i18n}) =>
            <div className="Sidebar">
                <Header title={t('SETTINGS_TITLE')} />
                <ListItem text={t('SETTINGS_GENERAL')} onClick={_("general")} selected={sel("general")} />
                <ListItem text={t('SETTINGS_PRIVACY')} onClick={_("privacy")} selected={sel("privacy")} />
                <ListItem text={t('SETTINGS_WEBSITES')} onClick={_("websites")} selected={sel("websites")} />
                <ListItem text={t('SETTINGS_ADVANCED')} onClick={_("advanced")} selected={sel("advanced")} />
            </div>
        }</Translation>
    }
}

class SettingsPrivacyPane extends React.Component {
    clearData() {
        let performClearData = () => {
            Modal.unmount();
            alert("Will clear data");
        }
        
        
        Modal.mount(<Translation>{(t, {i18n}) =>
            <Modal title={t("SETTINGS_PRIVACY_CLEAR")} cancelable={true} buttons={["ok"]} onOk={performClearData} width="400px">
                <VerticalLayout>
                    <MiniHeader title={t("SETTINGS_PRIVACY_CLEAR_TIME_RANGE_TITLE")} />
                    <select>
                        <option>{t("SETTINGS_PRIVACY_CLEAR_TIME_RANGE_HOUR_TITLE")}</option>
                        <option>{t("SETTINGS_PRIVACY_CLEAR_TIME_RANGE_DAY_TITLE")}</option>
                        <option>{t("SETTINGS_PRIVACY_CLEAR_TIME_RANGE_WEEK_TITLE")}</option>
                        <option>{t("SETTINGS_PRIVACY_CLEAR_TIME_RANGE_MONTH_TITLE")}</option>
                        <option>{t("SETTINGS_PRIVACY_CLEAR_TIME_RANGE_ALL_TITLE")}</option>
                    </select>
                </VerticalLayout>
                <HorizontalLine />
                <VerticalLayout>
                    <MiniHeader title={t("SETTINGS_PRIVACY_CLEAR_ITEMS_TITLE")} />
                    <CheckBox text={t('SETTINGS_PRIVACY_CLEAR_HISTORY')} defaultValue={true} />
                    <CheckBox text={t('SETTINGS_PRIVACY_CLEAR_COOKIES')} defaultValue={true} />
                    <CheckBox text={t('SETTINGS_PRIVACY_CLEAR_CACHE')} defaultValue={true} />
                </VerticalLayout>
            </Modal>
        }</Translation>);
    }
    
    render() {
        return <Translation>{(t, {i18n}) =>
            <div>
                <Header title={t('SETTINGS_PRIVACY')} />
                <VerticalLayout>
                    <MiniHeader title={t('SETTINGS_PRIVACY_CLEAR')} />
                    <p>{t('SETTINGS_PRIVACY_CLEAR_EXPLANATION')}</p>
                    <button onClick={this.clearData}>{t('SETTINGS_PRIVACY_CLEAR_ACTION')}</button>
                </VerticalLayout>
                <HorizontalLine />
                <VerticalLayout>
                    <MiniHeader title={t('SETTINGS_PRIVACY')} />
                    <p>{t('SETTINGS_PRIVACY_TOGGLES_EXPLANATION')}</p>
                    <CheckBox text={t('SETTINGS_PRIVACY_TOGGLES_DNT')} defaultValue={true} settingsKey="privacy/dnt" />
                </VerticalLayout>
            </div>
        }</Translation>
    }
}

class SettingsGeneralPane extends React.Component {
    render() {
        return <Translation>{(t, {i18n}) =>
            <div>
                <Header title={t('SETTINGS_GENERAL')} />
                <VerticalLayout>
                    <MiniHeader title={t('SETTINGS_GENERAL_STARTUP')} />
                    <p>{t('SETTINGS_GENERAL_STARTUP_EXPLANATION')}</p>
                    <RadioButtonGroup options={
                        [
                            {value: "homepage", text: t('SETTINGS_STARTUP_HOME_PAGE')},
                            {value: "newtab", text: t('SETTINGS_STARTUP_NEW_TAB')}
                        ]
                    } defaultValue='newtab' settingsKey='startup/action'/>
                </VerticalLayout>
                <HorizontalLine />
                <VerticalLayout>
                    <MiniHeader title={t('SETTINGS_GENERAL_HOME_PAGE')} />
                    <p>{t('SETTINGS_GENERAL_HOME_PAGE_EXPLANATION')}</p>
                    <LineEdit defaultValue="https://vicr123.com/" settingsKey="startup/home" validation="url" />
                </VerticalLayout>
            </div>
        }</Translation>
    }
}

class SettingsAdvancedPane extends React.Component {
    resetSettings() {
        let performReset = () => {
            settings.clear().then(() => {
                Modal.unmount();
            });
        }
        
        Modal.mount(<Translation>{(t, {i18n}) =>
            <Modal title={t("SETTINGS_ADVANCED_RESET_CONFIRM_TITLE")} cancelable={true} buttons={["ok"]} onOk={performReset} width="400px">
                <VerticalLayout>
                {t("SETTINGS_ADVANCED_RESET_CONFIRM_CONTENT_1")}
                <ul>
                    <li>{t("SETTINGS_ADVANCED_RESET_CONFIRM_CONTENT_2")}</li>
                    <li>{t("SETTINGS_ADVANCED_RESET_CONFIRM_CONTENT_3")}</li>
                    <li>{t("SETTINGS_ADVANCED_RESET_CONFIRM_CONTENT_4")}</li>
                </ul>
                {t("SETTINGS_ADVANCED_RESET_CONFIRM_CONTENT_5")}
                <ul>
                    <li>{t("SETTINGS_ADVANCED_RESET_CONFIRM_CONTENT_6")}</li>
                    <li>{t("SETTINGS_ADVANCED_RESET_CONFIRM_CONTENT_7")}</li>
                </ul>
                <p>{t("SETTINGS_ADVANCED_RESET_CONFIRM_CONTENT_8")}</p>
                <b>{t("SETTINGS_ADVANCED_RESET_CONFIRM_CONTENT_9")}</b>
                </VerticalLayout>
            </Modal>
        }</Translation>);
    }
    
    render() {
        return <Translation>{(t, {i18n}) =>
            <div>
                <Header title={t('SETTINGS_ADVANCED')} />
                <VerticalLayout>
                    <MiniHeader title={t('SETTINGS_ADVANCED_RESET')} />
                    <p>{t('SETTINGS_ADVANCED_RESET_EXPLANATION')}</p>
                    <button onClick={this.resetSettings}>{t('SETTINGS_ADVANCED_RESET_ACTION')}</button>
                </VerticalLayout>
            </div>
        }</Translation>
    }
}

class Settings extends React.Component {
    constructor(props) {
        super(props);
        
        this.state = {
            currentPane: "general"
        };
    }
    
    currentSettingsPane() {
        switch (this.state.currentPane) {
            case "general":
                return <SettingsGeneralPane />;
            case "privacy":
                return <SettingsPrivacyPane />;
            case "advanced":
                return <SettingsAdvancedPane />;
            default:
                return <Translation>{(t, {i18n}) => 
                    <div>
                        <Header title={t('INVALID_PAGE_TITLE')} />
                        <div className="Content">
                            {t('INVALID_PAGE_CONTENT')}
                        </div>
                    </div>
                }</Translation>
        }
    }
    
    changePane(pane) {
        this.setState({
            currentPane: pane
        });
    }
    
    render() {
        return <Translation>{(t, {i18n}) =>
            <div className="App">
                <SettingsSidebar currentPane={this.state.currentPane} onChange={(paneName) => this.changePane(paneName)} />
                <div className="AppContainer">
                    {this.currentSettingsPane()}
                </div>
            </div>
        }</Translation>
    }
}

export default Settings;
