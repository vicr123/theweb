import React from 'react';
import './App.css';

class VerticalLayout extends React.Component {
    constructor(props) {
        super(props);
    }
    
    className() {
        let cn = ["directionalContent", "vertical"];
        if (!this.props.noBorder) cn.push("Content");
        if (this.props.scrollable) cn.push("Scrollable");
        if (this.props.noSpacing) cn.push("NoSpacing")
        return cn.join(" ");
    }
    
    render() {
        return <div className={this.className()} style={this.props.style}>
            {this.props.children}
        </div>
    }
}

class HorizontalLayout extends React.Component {
    constructor(props) {
        super(props);
    }
    
    className() {
        let cn = ["directionalContent", "horizontal"];
        if (!this.props.noBorder) cn.push("Content");
        if (this.props.scrollable) cn.push("Scrollable");
        if (this.props.noSpacing) cn.push("NoSpacing")
        return cn.join(" ");
    }
    
    render() {
        return <div className={this.className()} style={this.props.style}>
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

class VerticalLine extends React.Component {
    constructor(props) {
        super(props);
    }
    
    render() {
        return <div className="verticalLine" />
    }
}

export { VerticalLayout };
export { HorizontalLayout };
export { HorizontalLine };
export { VerticalLine };