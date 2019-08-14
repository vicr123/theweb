import React from 'react';
import './App.css';

import { settings } from './api.js';
import { VerticalLayout } from './layouts.jsx';

let lastId = 0;
function getNextId() {
    let retval = `el${lastId}`;
    lastId++;
    return retval;
}

class SettingToggle extends React.Component {
    constructor(props) {
        super(props);
        
        this.state = {
            elId: getNextId(),
            currentValue: this.props.defaultValue,
            dummy: this.props.settingsKey === null
        }
        
        if (this.props.settingsKey !== null) {
            settings.value(this.props.settingsKey, this.props.defaultValue).then((value) => {
                this.setState({
                    currentValue: value
                });
            });
        }
    }
    
    setValue(value) {
        this.setState({
            currentValue: value
        });
        
        if (!this.state.dummy) {
            settings.setValue(this.props.settingsKey, value);
        }
    }
    
    render() {
        return <div style={{"background-color": "red", "border": "1px solid yellow", "padding": "3px"}}>I am an unimplemented setting toggle. !!!IMPLEMENT ME!!!</div>
    }
}

class RadioButtonGroup extends SettingToggle {
    componentDidMount() {
    
    }
    
    generateElements() {
        let elements = [];
        for (const option of this.props.options) {
            let onChangeHandler = (e) => {
                if (e.target.value) {
                    this.setValue(option.value);
                }
            };
            
            elements.push(<div key={option.value + this.state.elId}>
                <label key={option.value + this.state.elId + "label"}>
                    <input type="radio" key={option.value + this.state.elId + "radio"} name={this.state.elId} value={option.value} checked={this.state.currentValue === option.value ? true : false} onChange={onChangeHandler} />
                    {option.text}
                </label>
            </div>)
        }
        return elements;
    }
    
    render() {
        return <VerticalLayout noBorder='true'>
            {this.generateElements()}
        </VerticalLayout>
    }
}

class LineEdit extends SettingToggle {
    constructor(props) {
        super(props);
    }
    
    getInputType() {
        switch (this.props.validation) {
            case "url":
                return "url";
            default:
                return "text";
        }
    }
    
    render() {
        let onChangeHandler = (e) => {
            this.setState({
                currentValue: e.target.value
            });
        };
        let focusOutHandler = (e) => {
            this.setValue(e.target.value);
        };
        
        return <input type={this.getInputType()} value={this.state.currentValue} placeholder={this.props.placeholder} onChange={onChangeHandler} onFocusOut={focusOutHandler}/>
    }
}

class CheckBox extends SettingToggle {
    render() {
        let onChangeHandler = (e) => {
            this.setValue(e.target.checked);
        };
        
        return <label>
            <input type="checkbox" checked={this.state.currentValue} onChange={onChangeHandler} />
            {this.props.text}
        </label>
    }
}

export { RadioButtonGroup };
export { LineEdit };
export { CheckBox };