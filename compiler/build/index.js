"use strict";
// todo
// run with npx . <args>
Object.defineProperty(exports, "__esModule", { value: true });
const yargs = require("yargs");
const fs_extra_1 = require("fs-extra");
const lineutil_1 = require("./lineutil");
const error_1 = require("./error");
const UNI1_1 = require("./targets/UNI1");
const supportedTargets = [
    new UNI1_1.UNI1Target()
];
const files = yargs.argv._;
if (!files.length) {
    console.error(`Overkill error\n\tNo input files.`);
    process.exit();
}
let targetId = "UNI1";
if (yargs.argv.target)
    targetId = yargs.argv.target;
const target = supportedTargets.find(t => t.id === targetId);
if (!target) {
    console.error(`Overkill error\n\tUnsupported target ${targetId}.`);
    process.exit();
}
files.forEach(async (file) => {
    const extPos = file.lastIndexOf(".");
    const newName = file.substring(0, extPos < 0 ? file.length : extPos) + ".ovk";
    target.reset();
    try {
        const lines = await lineutil_1.getLines(file);
        const instrLines = target.preprocess(lines);
        const buf = target.process(instrLines);
        await fs_extra_1.writeFile(newName, buf);
    }
    catch (e) {
        if (e instanceof error_1.OvkError) {
            console.error(`Overkill error - in ${e.line.file}, line ${e.line.lineNumber}:\n\t${e.line.data}\n\t${e.message}`);
        }
        else {
            console.error(`General error:\n\t${e}`);
            process.exit();
        }
    }
});
