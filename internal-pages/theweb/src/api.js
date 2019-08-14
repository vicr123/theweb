class Api {
    get(url) {
        return new Promise((res, rej) => {
            let request = new XMLHttpRequest();
            request.addEventListener("load", () => {
                res(request.responseText);
            });
            request.addEventListener("error", () => {
                rej();
            });
            request.open("GET", url);
            request.send();
        });
    }
    
    lang() {
        return this.get("theweb://api/lang");
    }
}

class Settings extends Api {
    getSettings() {
        return this.get("theweb://api/settings").then((res) => {
            return JSON.parse(res);
        });
    }
    
    value(key, defaultValue) {
        return this.getSettings().then((settings) => {
            if (settings.hasOwnProperty(key)) {
                return settings[key];
            } else {
                return defaultValue;
            }
        });
    }
    
    setValue(key, value) {
        return new Promise((res, rej) => {
            let request = new XMLHttpRequest();
            request.addEventListener("load", () => {
                res();
            });
            request.addEventListener("error", () => {
                rej();
            });
            request.open("GET", `theweb://api/settings/set?key=${encodeURIComponent(key)}&value=${encodeURIComponent(value)}`);
            request.send();
        });
    }
}

let api = new Api();
let settings = new Settings();

export { api };
export { settings };