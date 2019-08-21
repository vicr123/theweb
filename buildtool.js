#!/usr/bin/node

const fs = require('fs');
const path = require('path');
const util = require('util');
const child_process = require('child_process');
const os = require('os');

let options = {};

const exec = util.promisify(child_process.exec);
const execFile = util.promisify(child_process.execFile);
const readdir = util.promisify(fs.readdir);

/**
 * Spawns a child process
 *
 * @param {string} path - The path of the process to run
 * @param {string} args - The arguments to pass to the process
 * @param {object} opts - Options to pass to child_process.spawn
 */
const spawn = (path, args, opts) => {
    return new Promise(function(resolve, reject) {
        let proc = child_process.spawn(path, args, opts);
        
        if (options.debug) {
            proc.stdout.pipe(process.stdout);
            proc.stderr.pipe(process.stderr);
        } else {
            proc.stdout.on('data', function(data) {});
            proc.stderr.on('data', function(data) {});
        }
        
        proc.on('close', (code) => {
            if (code == 0) {
                resolve();
            } else {
                reject(code);
            }
        });
    });
}


/**
 * Spawns a child process and shows a progress bar while the process is running
 *
 * @param {array} message - Messages to show whilst the process is running
 * @param {string} path - The path of the process to run
 * @param {string} args - The arguments to pass to the process
 * @param {object} opts - Options to pass to child_process.spawn
 */
const spawnWithMessage = async (message, path, args, opts) => {
    let interval;
    try {
        let state = 0;
        
        if (options.debug) {
            logProgress(message[0]);
        } else {
            logProgress(message[0], true);
            
            interval = setInterval(() => {
                const barLength = 20;         
                const thumbLength = 4;   
                state++;
                if (state >= barLength + thumbLength) state = 0;
    
                let progressBar = "[";
                if (state < 4) {
                    progressBar += "#".repeat(state);
                    progressBar += " ".repeat(barLength - state);
                } else if (state > barLength) {
                    progressBar += " ".repeat(state - thumbLength);
                    progressBar += "#".repeat(thumbLength - (state - barLength));
                } else {
                    progressBar += " ".repeat(state - thumbLength);
                    progressBar += "#".repeat(thumbLength);
                    progressBar += " ".repeat(barLength - state);
                }
                progressBar += "]";
                
                logProgress(`${message[0]} ${progressBar}`, true);
            }, 200);
        }
        
        await spawn(path, args, opts);
        
        if (!options.debug) {
            clearInterval(interval);
            process.stdout.clearLine();
        }
        logProgress(`✔️ ${message[1]}`);
    } catch (err) {
        if (!options.debug) clearInterval(interval);
        throw err; //re-throw error
    }
}

//Helper functions

/**
 * Returns coloured text for outputting to the console.
 * 
 * @param {string} text - Text to be coloured
 * @param {string} color - The color to color the text
 * @return {string} An ANSI encoding of the coloured text
 */
function colorText(text, color) {
    if (options.noCol == true) return text;
    
    switch (color) {
        case "red":
            return `\x1b[31m${text}\x1b[0m`;
        case "green":
            return `\x1b[32m${text}\x1b[0m`;
        case "yellow":
            return `\x1b[33m${text}\x1b[0m`;
        case "blue":
            return `\x1b[34m${text}\x1b[0m`;
        case "magenta":
            return `\x1b[35m${text}\x1b[0m`;
        case "cyan":
            return `\x1b[36m${text}\x1b[0m`;
        case "white":
            return `\x1b[37m${text}\x1b[0m`;
        case "brwhite":
            return `\x1b[37m\x1b[1m${text}\x1b[0m`;
    }
}

/**
 * Returns whether this platform is Windows.
 *
 * @return {boolean} True if this platform is Windows
 */
function isWin() {
    return os.platform() == "win32";
}

/**
 * Logs a prefixed message to the console.
 * 
 * @param {string} message - Message to send to the console
 * @param {boolean} returnToStart - Whether to return to the start of the line or create a new line afterwards
 */
function logProgress(message, returnToStart = false) {
    process.stdout.write(`${colorText("BUILDTOOL:", "cyan")} ${message}`);
    if (returnToStart) {
        process.stdout.write("\r");
    } else {
        process.stdout.write("\n");
    }
}

/**
 * Logs a prefixed error to the console.
 * 
 * @param {string} message - Message to send to the console
 */
function logError(message) {
    logProgress(`${colorText("ERR", "red")} ${message}`);
}

options = {
    "buildDirectory": path.normalize(process.cwd() + "/build"),
    "mode": "build",
    "qmakeArgs": "",
    "projectPath": process.cwd(),
    "npmPath": isWin() ? `"C:\\Program Files\\nodejs\\npm.cmd"` : "npm",
    "qmakePath": isWin() ? `qmake.exe` : "qmake",
    "makePath": isWin() ? "nmake" : "make",
    "noCol": false,
    "debug": false
}

// Build Steps

/**
 * Prints the build configuration
 */
function printConfiguration() {
    console.log(colorText("BUILD CONFIGURATION", "cyan"));
    console.log(`${colorText("Build Directory", "green")}        ${options.buildDirectory}`);
    console.log(`${colorText("Build Mode", "green")}             ${options.mode}`);
    console.log(`${colorText("Project Directory", "green")}      ${options.projectPath}`);
    console.log(`${colorText("QMake command", "green")}          ${options.qmakePath}`);
    console.log();
}

/**
 * Changes to the build directory
 */
function cdBuildDirectory() {
    if (!fs.existsSync(options.buildDirectory)) {
        logProgress("Build directory does not exist. Creating it now.");
        fs.mkdirSync(options.buildDirectory);
    }
    process.chdir(options.buildDirectory);
}

/**
 * Generates internal-pages project resource files
 */
async function genInternalPages() {
    let initialDir = process.cwd();
    process.chdir(options.projectPath + "/internal-pages/theweb");
    
    let installNpm = async () => {
        try {
            await spawnWithMessage(["Installing npm dependencies for internal-pages", "npm dependencies installed for internal-pages"], options.npmPath, ["install"], {shell: true});
        } catch (err) {
            logError("Couldn't install npm dependencies for internal-pages.");
            process.exit(1);
        }
    }
    
    let buildReact = async () => {
        try {
            await spawnWithMessage(["Building Production React site", "Production React site built"], options.npmPath, ["run", "build"], {shell: true});
        } catch (err) {
            logError("Couldn't build production react site for internal-pages.");
            process.exit(1);
        }
    }
    
    let packageResources = async () => {
        try {
            await spawnWithMessage(["Packaging production React site", "React site packaged"], process.argv[0], ["genresources.js", `${options.projectPath}/internal-pages`, "scheme.qrc", "theweb/build"], {shell: false});
        } catch (err) {
            logError("Couldn't package qrc file for internal-pages.");
            process.exit(1);
        }
    }
    
    if (!fs.existsSync(options.projectPath + "/internal-pages/theweb/node_modules")) await installNpm();
    await buildReact();
    
    process.chdir(options.projectPath + "/internal-pages");
    await packageResources();

    process.chdir(initialDir);
}

/**
 * Runs QMake in the build directory
 */
async function runQmake() {
    try {
        let args = [path.normalize(`${options.projectPath}/theWeb.pro`)];
        if (options.qmakeArgs !== "") args.push(options.qmakeArgs);
        await spawnWithMessage(["Running QMake", "QMake generated Makefile"], options.qmakePath, args, {shell: false});
    } catch (err) {
        logError("Couldn't run qmake.");
        process.exit(1);
    }
}

/**
 * Builds makefile in the build directory
 */
async function build() {
    try {
        await spawnWithMessage(["Building theWeb", "theWeb built"], options.makePath, [], {shell: false});
    } catch (err) {
        logError("Couldn't build theWeb.");
        process.exit(1);
    }
}

/**
 * Prints help information to the console
 */
function printHelp() {
    console.log(`Usage: ${process.argv[0]} ${process.argv[1]} [options]`);
    console.log("theWeb Build Tool");
    console.log();
    console.log("Options:");
    console.log("  --help                    Show this help menu");
    console.log("  --buildDir [directory]    Change the build directory");
    console.log("  --clean                   Clean the build directory");
    console.log("  --install                 Install the built app");
    console.log("  --internalpages           Just generate the internal pages");
    console.log("  --qmakeArgs [args]        Set arguments to be passed to QMake");
    console.log("  --npmPath [path]          Change the path to npm");
    console.log("  --makePath [path]         Change the path to make/nmake/jom");
    console.log("  --noCol                   Disable colour output");
    console.log("  --dryrun                  Show the build configuration only");
    console.log("  --debug                   Enable debugging output");
}

/**
 * Main entry point
 */
async function run() {
    //Parse command line args
    for (let i = 2; i < process.argv.length; i++) {
        let arg = process.argv[i];
        let nextArg = process.argv.length > i + 1 ? process.argv[i + 1] : null;
        
        let setOpt = (key) => {
            if (nextArg) {
                options[key] = nextArg;
                i++;
            } else {
                printHelp();
                process.exit(1);
            }
        };
        
        switch (arg) {
            case "--help":
                printHelp();
                process.exit(0);
            case "--buildDir":
                setOpt("buildDirectory");
                break;
            case "--clean":
                options.mode = "clean";
                break;
            case "--install":
                options.mode = "install";
                break;
            case "--dryrun":
                options.mode = "dry";
                break;
            case "--internalpages":
                options.mode = "internalpages";
                break;
            case "--qmakeArgs":
                setOpt("qmakeArgs");
                break;
            case "--npmPath":
                setOpt("npmPath");
                break;
            case "--qmakePath":
                setOpt("qmakePath");
                break;
            case "--debug":
                options.debug = true;
                break;
            case "--noCol":
                options.noCol = true;
                break;
            default:
                console.log(`Unrecognised argument: ${arg}`);
                console.log(`For more details, type ${process.argv[0]} ${process.argv[1]} --help`);
                process.exit(1);
        }
    }
    
    printConfiguration();
    
    //Build theWeb
    switch (options.mode) {
        case "build":
            await genInternalPages();
            cdBuildDirectory();
            await runQmake();
            await build();
            break;
        case "internalpages":
            await genInternalPages();
            break;
        case "dry":
            //Do nothing
            break;
        default:
            logError("This mode is unimplemented. Watch this space!");
    }
}

run();