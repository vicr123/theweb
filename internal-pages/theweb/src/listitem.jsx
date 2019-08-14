import React from 'react';
import './App.css';

class ListItem extends React.Component {
    constructor(props) {
        super(props);
    }
    
    render() {
        let cls = "ListItem";
        if (this.props.selected === "true") cls += " selected";
        return <div className={cls} onClick={this.props.onClick}>
            {this.props.text}
        </div>
    }
}

export default ListItem;
