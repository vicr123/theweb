import React from 'react';
import './App.css';
import { Header, MiniHeader } from './textblocks.jsx'
import { VerticalLayout, HorizontalLine } from './layouts.jsx'
import ListItem from './listitem.jsx'
import { Translation } from 'react-i18next';
import { RadioButtonGroup, LineEdit, CheckBox } from './settingstoggles.jsx'

class About extends React.Component {
    render() {
        return <Translation>{(t, {i18n}) =>
            <div>
                <Header title={t('ABOUT_TITLE')} />
                <VerticalLayout>
                    who knows something goes here
                </VerticalLayout>
            </div>
        }</Translation>
    }
}

export default About;