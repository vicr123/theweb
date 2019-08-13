import React from 'react';
import './App.css';
import Header from './header.js'
import ListItem from './listitem.js'

class Sidebar extends React.Component {
    constructor(props) {
        super(props);
    }
    
    render() {
        let _ = (paneName) => {
            return () => this.props.onChange(paneName);
        }
        
        let sel = (paneName) => {
            return this.props.currentPane == paneName ? "true" : "false";
        }
        
        return <div className="Sidebar">
            <Header title="theWeb" />
            <ListItem text="Settings" onClick={_("settings")} selected={sel("settings")} />
            <ListItem text="Downloads" />
        </div>
    }
}

export default Sidebar;
