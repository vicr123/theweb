import React from 'react';
import './App.css';

class VerticalLayout extends React.Component {
    constructor(props) {
        super(props);
    }
    
    render() {
        return <div className={"verticalContent" + (this.props.noBorder ? "" : " Content")}>
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