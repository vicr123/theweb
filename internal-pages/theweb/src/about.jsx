import React from 'react';
import './App.css';
import { Header, MiniHeader } from './textblocks.jsx'
import { VerticalLayout, HorizontalLine, HorizontalLayout, VerticalLine } from './layouts.jsx'
import ListItem from './listitem.jsx'
import { Translation } from 'react-i18next';
import { RadioButtonGroup, LineEdit, CheckBox } from './settingstoggles.jsx'

class About extends React.Component {
    render() {
        return <Translation>{(t, {i18n}) =>
            <VerticalLayout noSpacing={true} noBorder={true} style={{"flex-grow": "1"}} >
                <HorizontalLayout noBorder={true} noSpacing={true} style={{"flex-grow": "1"}} >
                    <div style={{
                        "width": "100px",
                        "background": "linear-gradient(to bottom, rgb(200, 0, 0) 0%, rgb(100, 0, 0) 100%)"
                    }} />
                    <VerticalLine />
                    <VerticalLayout noBorder={true} noSpacing={true} style={{"flex-grow": "1"}} >
                        <Header title={t('ABOUT_TITLE')} />
                        <VerticalLayout noBorder={true} noSpacing={true} >
                            <Header title="theWeb 16.0" noBorder={true} />
                            <div style={{"padding-left": "9px", "padding-bottom": "9px"}}>
                                {t("ABOUT_APPLICATION_GENERIC_NAME")}
                            </div>
                        </VerticalLayout>
                        <HorizontalLine />
                        <VerticalLayout>
                            <MiniHeader title={t("ABOUT_SOFTWARE")} />
                        </VerticalLayout>
                        <HorizontalLine />
                        <VerticalLayout>
                            <MiniHeader title={t("ABOUT_COPYRIGHT")} />
                            <div style={{"font-size": "9pt"}}>
                                <span>{t("ABOUT_COPYRIGHT_HOLDER", {year: '2019'})}</span>
                                <div style={{"height": "6px"}} />
                                <span>{t("ABOUT_COPYRIGHT_MESSAGE")}</span>
                                <div style={{"height": "6px"}} />
                                <span>{t("ABOUT_COPYRIGHT_CHROMIUM")}</span>
                            </div>
                        </VerticalLayout>
                    </VerticalLayout>
                </HorizontalLayout>
            </VerticalLayout>
        }</Translation>
    }
}

export default About;