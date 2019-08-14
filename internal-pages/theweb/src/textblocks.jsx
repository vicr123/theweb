import React from 'react';
import './App.css';

class Header extends React.Component {
    constructor(props) {
        super(props);
    }
    
    render() {
        return <div className="header">
            {this.props.title}
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