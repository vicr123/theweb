import React from 'react';
import './App.css';
import { Header, MiniHeader } from './textblocks.jsx'
import { VerticalLayout, HorizontalLine } from './layouts.jsx'
import ListItem from './listitem.jsx'
import { Translation } from 'react-i18next';
import { RadioButtonGroup, LineEdit, CheckBox } from './settingstoggles.jsx'
import { api, settings } from './api.js';
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
                <ListItem text={t('SETTINGS_PROFILES')} onClick={_("profiles")} selected={sel("profiles")} />
                <ListItem text={t('SETTINGS_GENERAL')} onClick={_("general")} selected={sel("general")} />
                <ListItem text={t('SETTINGS_PRIVACY')} onClick={_("privacy")} selected={sel("privacy")} />
                <ListItem text={t('SETTINGS_WEBSITES')} onClick={_("websites")} selected={sel("websites")} />
                <ListItem text={t('SETTINGS_ADVANCED')} onClick={_("advanced")} selected={sel("advanced")} />
            </div>
        }</Translation>
    }
}

class SettingsPrivacyClearDataPane extends React.Component {
    constructor(props) {
        super(props);
        
        this.state = {
            timeRange: "all",
            clearHistory: true,
            clearCookies: false,
            clearCache: false
        }
    }
    
    performClearData() {
        
        let latest = new Date();
        let earliest;
        
        switch (this.state.timeRange) {
            case "all":
                earliest = new Date(0);
                break;
            case "hour":
                earliest = new Date(latest.getTime() - (60 * 60 * 1000));
                break;
            case "day":
                earliest = new Date(latest.getTime() - (24 * 60 * 60 * 1000));
                break;
            case "week":
                earliest = new Date(latest.getTime() - (7 * 24 * 60 * 60 * 1000));
                break;
            case "month":
                earliest = new Date(latest.getTime() - (4 * 7 * 24 * 60 * 60 * 1000));
                break;
        }
        
        if (this.state.clearHistory) {
            if (this.state.timeRange == "all") {
                api.delete("theweb://api/history");
            } else {
                api.delete(`theweb://api/history?earliest=${earliest.getTime()}&latest=${latest.getTime()}`);
            }
        }
        
        Modal.unmount();
    }
    
    render() {
        let timeRangeChangeHandler = (e) => {
            this.setState({
                timeRange: e.target.value
            });
        }
        let clearHistoryChangeHandler = (e) => {
            this.setState({
                clearHistory: e.target.checked
            });
        }
        let clearCookiesChangeHandler = (e) => {
            this.setState({
                clearCookies: e.target.checked
            });
        }
        let clearCacheChangeHandler = (e) => {
            this.setState({
                clearCache: e.target.checked
            });
        }
        
        return <Translation>{(t, {i18n}) =>
            <Modal title={t("SETTINGS_PRIVACY_CLEAR")} cancelable={true} buttons={["ok"]} onOk={this.performClearData.bind(this)} width="400px">
                <VerticalLayout>
                    <MiniHeader title={t("SETTINGS_PRIVACY_CLEAR_TIME_RANGE_TITLE")} />
                    <select value={this.state.timeRange} onChange={timeRangeChangeHandler}>
                        <option value="hour">{t("SETTINGS_PRIVACY_CLEAR_TIME_RANGE_HOUR_TITLE")}</option>
                        <option value="day">{t("SETTINGS_PRIVACY_CLEAR_TIME_RANGE_DAY_TITLE")}</option>
                        <option value="week">{t("SETTINGS_PRIVACY_CLEAR_TIME_RANGE_WEEK_TITLE")}</option>
                        <option value="month">{t("SETTINGS_PRIVACY_CLEAR_TIME_RANGE_MONTH_TITLE")}</option>
                        <option value="all">{t("SETTINGS_PRIVACY_CLEAR_TIME_RANGE_ALL_TITLE")}</option>
                    </select>
                </VerticalLayout>
                <HorizontalLine />
                <VerticalLayout>
                    <MiniHeader title={t("SETTINGS_PRIVACY_CLEAR_ITEMS_TITLE")} />
                    <label>
                        <input type="checkbox" checked={this.state.clearHistory} onChange={clearHistoryChangeHandler} />
                        {t('SETTINGS_PRIVACY_CLEAR_HISTORY')}
                    </label>
                    <label>
                        <input disabled={true} type="checkbox" checked={this.state.clearCookies} onChange={clearCookiesChangeHandler} />
                        {t('SETTINGS_PRIVACY_CLEAR_COOKIES')}
                    </label>
                    <label>
                        <input disabled={true} type="checkbox" checked={this.state.clearCache} onChange={clearCacheChangeHandler} />
                        {t('SETTINGS_PRIVACY_CLEAR_CACHE')}
                    </label>
                </VerticalLayout>
            </Modal>
        }</Translation>;
    }
}

class SettingsProfilesPane extends React.Component {
    render() {
        return <Translation>{(t, {i18n}) =>
            <div>
                <Header title={t('SETTINGS_PROFILES')} />
                <VerticalLayout>
                    <p>{t('SETTINGS_PROFILES_COMING_SOON')}</p>
                </VerticalLayout>
            </div>
        }</Translation>
    }
}

class SettingsPrivacyPane extends React.Component {
    clearData() {
        Modal.mount(<SettingsPrivacyClearDataPane />);
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
            case "profiles":
                return <SettingsProfilesPane />;
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
