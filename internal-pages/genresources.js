#!/usr/bin/node

const fs = require('fs');
const path = require('path');
const sep = path.sep;

if (process.argv.length != 5) {
    console.log(`Usage: genresources.js path-to-output output-name relative-path-to-resources`);
}

const root = process.argv[2] + sep;

let xml = [];
xml.push('<!DOCTYPE RCC>');
xml.push('<RCC version="1.0">');
xml.push('<qresource prefix="/scheme/theweb">');

//Enumerate files
function walkPath(path, rootFolderPath) {
    let files = fs.readdirSync(path);
    for (let file of files) {
        let filePath = path + sep + file;
        if (fs.statSync(filePath).isDirectory()) {
            walkPath(filePath, rootFolderPath);
        } else {
            let relativePathToRoot = filePath.substr(root.length);
            let relativePathToFolder = filePath.substr(rootFolderPath.length + 1);
            xml.push(`<file alias="${relativePathToFolder}">${relativePathToRoot}</file>`);
        }
    }
}

walkPath(root + process.argv[4], root + process.argv[4]);

xml.push('</qresource>');
xml.push('</RCC>');
fs.writeFileSync(root + process.argv[3], xml.join(''));