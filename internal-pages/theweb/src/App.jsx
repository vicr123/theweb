import React from 'react';
import './App.css';
import Sidebar from './sidebar.jsx';
import { Header } from './textblocks.jsx'

import Settings from './settings.jsx';
import About from './about.jsx';
import NewTab from './newtab.jsx';
import History from './history.jsx';

import i18n from 'i18next';
import XHR from 'i18next-xhr-backend';
import { initReactI18next, Translation } from 'react-i18next';

import { api } from './api.js';

i18n.use(XHR)
    .use(initReactI18next)
    .init({
        fallbackLng: ["en", false],
        ns: ['translation'],
        backend: {
            loadPath: process.env.PUBLIC_URL + '/translations/{{lng}}/{{ns}}.json'
        },
        react: {
            useSuspense: false
        }
    });

class App extends React.Component {
    constructor(props) {
        super(props);
        let s = {
            lang: 'en',
            showSidebar: true
        };
        const path = window.location.pathname.substr(2);
        if (path.startsWith("settings")) {
            s.currentPane = "settings";
        } else if (path.startsWith("about")) {
            s.currentPane = "about";
        } else if (path.startsWith("history")) {
            s.currentPane = "history";
        } else if (path.startsWith("newtab")) {
            s.currentPane = "newtab";
            s.showSidebar = false;
        } else {
            s.currentPane = "invalid";
        }
        
        this.state = s;
    }
    
    componentDidMount() {
        console.log("Loading language");
        api.lang().then(lang => {
            this.setState({
                lang: lang
            });
        }).catch(() => {
            console.log("Language not obtainable");
        });
        window.addEventListener("popstate", this.popState.bind(this));
    }
    
    popState(e) {
        if (document.location.pathname.startsWith("//settings")) {
            this.setState({
                currentPane: "settings"
            });
        } else if (document.location.pathname.startsWith("//about")) {
            this.setState({
                currentPane: "about"
            });
        }
    }
    
    componentNode() {
        return <Translation>{(t, {i18n}) => {
            switch (this.state.currentPane) {
                case "settings":
                    document.title = t('SETTINGS_TITLE');
                    return <Settings />;
                case "about":
                    document.title = t('ABOUT_TITLE');
                    return <About />
                case "newtab":
                    document.title = t('NEWTAB_TITLE');
                    return <NewTab />
                case "history":
                    document.title = t('HISTORY_TITLE');
                    return <History />
                default:
                    document.title = "theWeb";
                    return <div>
                        <Header title={t('INVALID_PAGE_TITLE')} />
                        <div className="Content">
                            {t('INVALID_PAGE_CONTENT')}
                        </div>
                    </div>
            }
        }}</Translation>
    }
    
    changePane(pane) {
        try {
            switch (pane) {
                case "settings":
                    window.history.pushState({}, "", "theweb://settings");
                    break;
                case "about":
                    window.history.pushState({}, "", "theweb://about");
                    break;
                case "history":
                    window.history.pushState({}, "", "theweb://history");
                    break;
            }
        } catch (err) {
            //Disregard
        }
        
        this.setState({
            currentPane: pane
        });
    }
    
    sidebarElement() {
        if (this.state.showSidebar) {
            return <Sidebar onChange={(paneName) => this.changePane(paneName)} currentPane={this.state.currentPane} />;
        }
    }
    
    render() {
        return <Translation>{(t, {i18n}) => {
            if (i18n.language !== this.state.lang) i18n.changeLanguage(this.state.lang);
            return <div className="App">
                <div id="modalContainer" />
                {this.sidebarElement()}
                <div className="AppContainer Scrollable">
                    {this.componentNode()}
                </div>
            </div>
        }}</Translation>
    }
}

export default App;
