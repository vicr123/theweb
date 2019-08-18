import React from 'react';
import './App.css';
import { Header } from './textblocks.jsx'
import ListItem from './listitem.jsx'
import { HorizontalLine } from './layouts.jsx'

import { Translation } from 'react-i18next';

class Sidebar extends React.Component {
    constructor(props) {
        super(props);
    }
    
    render() {
        let _ = (paneName) => {
            return () => this.props.onChange(paneName);
        }
        
        let sel = (paneName) => {
            return this.props.currentPane === paneName ? "true" : "false";
        }
        
        return <Translation>{(t, {i18n}) =>
            <div className="Sidebar">
                <Header title="theWeb" />
                <ListItem icon="sys:bookmark-new" text={t('BOOKMARKS_TITLE')} />
                <ListItem icon="sys:downloads" text={t('DOWNLOADS_TITLE')} />
                <HorizontalLine />
                <ListItem icon="sys:configure" text={t('SETTINGS_TITLE')} onClick={_("settings")} selected={sel("settings")} />
                <ListItem icon="sys:help-about" text={t('ABOUT_TITLE')} onClick={_("about")} selected={sel("about")} />
            </div>
        }</Translation>
    }
}

export default Sidebar;
