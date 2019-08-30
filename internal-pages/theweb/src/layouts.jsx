import React from 'react';
import './App.css';

class VerticalLayout extends React.Component {
    constructor(props) {
        super(props);
    }
    
    className() {
        let cn = ["verticalContent"];
        if (!this.props.noBorder) cn.push("Content");
        if (this.props.scrollable) cn.push("Scrollable");
        if (this.props.noSpacing) cn.push("NoSpacing")
        return cn.join(" ");
    }
    
    render() {
        return <div className={this.className()}>
            {this.props.children}
        </div>
    }
}

class HorizontalLine extends React.Component {
    constructor(props) {
        super(props);
    }
    
    render() {
        return <div className="horizontalLine" />
    }
}

export { VerticalLayout };
export { HorizontalLine };