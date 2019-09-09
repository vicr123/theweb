import React from 'react';
import './App.css';

class ListItem extends React.Component {
    constructor(props) {
        super(props);
    }
    
    itemContents() {
        let contents = [];
        if (this.props.icon) {
            contents.push(<img key="image" className="sidebarImage" src={"theweb://sysicons/?width=32&height=32&icons=" + encodeURIComponent(this.props.icon)} />)
        }
        contents.push(<span key="text" class="ListItemText">{this.props.text}</span>);
        return contents;
    }
    
    render() {
        let cls = "ListItem";
        if (this.props.selected === "true") cls += " selected";
        return <div className={cls} onClick={this.props.onClick}>
            {this.itemContents()}
        </div>
    }
}

export default ListItem;
