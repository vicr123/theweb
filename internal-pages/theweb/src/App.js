import React from 'react';
import './App.css';
import Settings from './settings.js'
import Sidebar from './sidebar.js'
import Header from './header.js'

class App extends React.Component {
    constructor(props) {
        super(props);
        let s = {};
        const path = window.location.pathname.substr(2);
        if (path.startsWith("settings")) {
            s.currentPane = "settings";
        } else {
            s.currentPane = "invalid";
        }
        this.state = s;
    }
    
    componentDidMount() {
        window.addEventListener("popstate", this.popState);
    }
    
    popState(e) {
        
    }
    
    componentNode() {
        switch (this.state.currentPane) {
            case "settings":
                return <Settings />;
            default:
                return <div>
                    <Header title="Invalid Page" />
                    <div className="Content">
                        There was a problem. Try going to a different page.
                    </div>
                </div>
        }
    }
    
    changePane(pane) {
        this.setState({
            currentPane: pane
        });
    }
    
    render() {
        return <div className="App">
            <Sidebar onChange={(paneName) => this.changePane(paneName)} currentPane={this.state.currentPane} />
            <div className="AppContainer">
                {this.componentNode()}
            </div>
        </div>
    }
}

export default App;
