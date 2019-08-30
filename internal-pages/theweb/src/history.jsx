import React from 'react';
import { initReactI18next, Translation } from 'react-i18next';

import { api } from './api.js'

import { Header, MiniHeader } from './textblocks.jsx'
import { VerticalLayout, HorizontalLine } from './layouts.jsx'

class History extends React.Component {
    constructor(props) {
        super(props);
        
        this.state = {
            historyItems: []
        };
        
        this.retrieveHistory();
    }
    
    async retrieveHistory() {
        this.setState({
            historyItems: JSON.parse(await api.get("theweb://api/history"))
        });
    }
    
    generateHistoryItemByDate(items) {
        let elements = [];
        
        for (let hItem of items) {
            let onClickHandler = () => {
                window.location = hItem.url;
            }
            
            elements.push(<div className="ListItem HistoryItem" onClick={onClickHandler}>
                <span className="title">{hItem.title}</span>&nbsp;·&nbsp; 
                <span className="url">{new URL(hItem.url).host}</span>
                <div style={{"flex-grow": "1"}} />
                <span className="date">{new Date(hItem.accessed).toLocaleTimeString()}</span>
            </div>)
        }
        
        return <VerticalLayout noBorder={true} noSpacing={true} >
            <div className="Content">
                <MiniHeader title={new Date(items[0].accessed).toLocaleDateString(undefined, {
                    dateStyle: "full",
                    weekday: "long",
                    year: "numeric",
                    month: "long",
                    day: "numeric"
                })} />
            </div>
            {elements}
            <HorizontalLine />
        </VerticalLayout>
    }
    
    historyItems() {
        //Create array of array of history items by date
        let itemsByDate = [];
        let currentItems = [];
        
        for (let hItem of this.state.historyItems) {
            if (currentItems.length == 0) {
                //Special case for first history item
                currentItems.push(hItem);
                continue;
            }
            
            let accessed = new Date(hItem.accessed);
            let compareTo = new Date(currentItems[0].accessed);
            
            if (accessed.getDate() == compareTo.getDate() && accessed.getMonth() == compareTo.getMonth() && accessed.getFullYear() == compareTo.getFullYear()) {
//             if (accessed.getUTCHours() == compareTo.getUTCHours()) {
                currentItems.push(hItem);
            } else {
                itemsByDate.push(currentItems);
                currentItems = [
                    hItem
                ];
            }
        }
        
        if (currentItems.length !== 0) itemsByDate.push(currentItems);
        if (itemsByDate.length == 0) return <div></div> //Special case for when there is no history present
        
        let elements = [];
        for (let items of itemsByDate) {
            elements.push(this.generateHistoryItemByDate(items));
        }
        return elements;
    }
    
    render() {
        return <Translation>{(t, {i18n}) =>
            <div>
                <Header title={t('HISTORY_TITLE')} sticky={true} />
                <VerticalLayout noBorder={true} scrollable={true} noSpacing={true}>
                    {this.historyItems()}
                </VerticalLayout>
            </div>
        }</Translation>
    }
}

export default History;