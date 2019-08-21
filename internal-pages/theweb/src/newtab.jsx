import React from 'react';
import './App.css';
import { Header, MiniHeader } from './textblocks.jsx'
import { VerticalLayout, HorizontalLine } from './layouts.jsx'
import ListItem from './listitem.jsx'
import { Translation } from 'react-i18next';
import { RadioButtonGroup, LineEdit, CheckBox } from './settingstoggles.jsx'
import { api } from './api.js';

import './newtab.css'

class UrlBar extends React.Component {
    render() {
        let keyDownHandler = (e) => {
            if (e.keyCode == 13) document.location = e.target.value;
        }
        
        return <div>
            <input type="text" className="urlBar" placeholder="https://vicr123.com" onKeyDown={keyDownHandler} />
        </div>
    }
}

class NewTab extends React.Component {
    constructor(props) {
        super(props);
        
        this.state = {
            oblivion: false
        }
        
        api.isOblivion().then((isOblivion) => {
            this.setState({oblivion: isOblivion});
        });
    }
    
    oblivionComponent() {
        if (this.state.oblivion) {
            return <Translation>{(t, {i18n}) =>
                <VerticalLayout>
                    <div className="alert">
                        <MiniHeader title={t('NEWTAB_OBLIVION_TITLE')} />
                        {t('NEWTAB_OBLIVION_EXPLANATION')}
                    </div>
                </VerticalLayout>
            }</Translation>
        }
    }
    
    render() {
        return <Translation>{(t, {i18n}) =>
            <div style={{"position": "fixed", "top": "0px", "left": "0px", "bottom": "0px", "right": "0px"}}>
                {this.oblivionComponent()}
                <div className="newTabContainer">
                    <h1>{t('NEWTAB_GO_PROMPT')}</h1>
                    <UrlBar />
                </div>
            </div>
        }</Translation>
    }
}

export default NewTab;