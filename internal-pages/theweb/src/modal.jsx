import React from 'react';
import ReactDOM from 'react-dom';
import './App.css';
import { Header } from './textblocks.jsx'
import ListItem from './listitem.jsx'
import { HorizontalLine } from './layouts.jsx'

import { Translation } from 'react-i18next';

let modalShown = false;

class Modal extends React.Component {
    constructor(props) {
        super(props);
    }
    
    iconFromTheme(iconName) {
        return "theweb://sysicons/?width=16&height=16&icons=" + encodeURIComponent("sys:" + iconName)
    }
    
    renderLeftElements() {
        if (this.props.cancelable) {
            return <div className="headerButton" onClick={Modal.unmount}>
                <img key="image" src={this.iconFromTheme("go-previous")} />
            </div>
        }
    }
    
    renderRightElements() {
        let elements = [];
        
        for (let button of this.props.buttons) {
            let icon;
            let clickHandler;
            if (button === "ok") {
                icon = "dialog-ok";
                clickHandler = this.props.onOk;
            } else if (button === "no") {
                icon = "dialog-cancel";
                clickHandler = this.props.onNo;
            } else {
                continue;
            }
            
            elements.push(<div key={button} className="headerButton" onClick={clickHandler}>
                <img key="image" src={this.iconFromTheme(icon)} />
            </div>)
        }
        
        return elements;
    }
    
    render() {
        let backgroundClickHandler = () => {
            if (this.props.cancelable) Modal.unmount();
        }
        
        let dummyHandler = (e) => {
            e.stopPropagation();
        };
        
        return <Translation>{(t, {i18n}) =>
            <div className="modalBackground" onClick={backgroundClickHandler}>
                <div className="modalBox" style={{"width": this.props.width}} onClick={dummyHandler} >
                    <Header title={this.props.title} leftElements={this.renderLeftElements()} rightElements={this.renderRightElements()}/>
                    <div>
                        {this.props.children}
                    </div>
                </div>
            </div>
        }</Translation>
    }
    
    static mount(jsx) {
        ReactDOM.render(jsx, document.getElementById('modalContainer'));
        modalShown = true;
    }
    
    static unmount() {
        ReactDOM.render(null, document.getElementById('modalContainer'));
        modalShown = false;
    }
}

export default Modal;
