"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const fs_extra_1 = require("fs-extra");
async function getLines(file) {
    const data = await fs_extra_1.readFile(file, "utf-8");
    return data.split(/\r?\n/).map((line, i) => {
        return {
            data: line,
            file,
            lineNumber: i + 1
        };
    });
}
exports.getLines = getLines;
