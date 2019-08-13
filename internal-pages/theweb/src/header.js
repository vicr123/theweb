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

export default Header;
