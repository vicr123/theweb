import React from 'react';
import './App.css';

class Header extends React.Component {
    constructor(props) {
        super(props);
    }
    
    className() {
        let cn = ["header"];
        if (this.props.sticky) cn.push("sticky");
        if (this.props.noBorder) cn.push("noBorder");
        return cn.join(" ");
    }
    
    render() {
        return <div className={this.className()} >
            {this.props.leftElements}
            <div className="headerText">{this.props.title}</div>
            {this.props.rightElements}
        </div>
    }
}

class MiniHeader extends React.Component {
    constructor(props) {
        super(props);
    }
    
    render() {
        return <div className="miniheader">
            {this.props.title}
        </div>
    }
}

class PaddedText extends React.Component {
    constructor(props) {
        super(props);
    }
    
    render() {
        return <div className="Content">
            {this.props.title}
        </div>
    }
}

export { Header };
export { MiniHeader };
export { PaddedText };
