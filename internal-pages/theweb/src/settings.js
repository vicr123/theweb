import React from 'react';
import './App.css';
import Header from './header.js'
import ListItem from './listitem.js'

class SettingsSidebar extends React.Component {
    render() {
        let _ = (paneName) => {
            return () => {};
        }
        
        return <div className="Sidebar">
            <Header title="Settings" />
            <ListItem text="General" onClick={_("settings")} />
            <ListItem text="Something Else" />
        </div>
    }
}

class Settings extends React.Component {
    render() {
        return <div className="App">
            <SettingsSidebar />
            <div className="AppContainer">
                <Header title="General" />
            </div>
        </div>
    }
}

export default Settings;
