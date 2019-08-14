import React from 'react';
import './App.css';
import Sidebar from './sidebar.jsx';
import { Header } from './textblocks.jsx'

import Settings from './settings.jsx';
import About from './about.jsx';

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
            lang: 'en'
        };
        const path = window.location.pathname.substr(2);
        if (path.startsWith("settings")) {
            s.currentPane = "settings";
        } else if (path.startsWith("about")) {
            s.currentPane = "about";
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
        window.addEventListener("popstate", this.popState);
    }
    
    popState(e) {
        
    }
    
    componentNode() {
        switch (this.state.currentPane) {
            case "settings":
                return <Settings />;
            case "about":
                return <About />
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
        return <Translation>{(t, {i18n}) => {
            if (i18n.language !== this.state.lang) i18n.changeLanguage(this.state.lang);
            return <div className="App">
                <Sidebar onChange={(paneName) => this.changePane(paneName)} currentPane={this.state.currentPane} />
                <div className="AppContainer">
                    {this.componentNode()}
                </div>
            </div>
        }}</Translation>
    }
}

export default App;
