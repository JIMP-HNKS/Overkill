"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const error_1 = require("../error");
class Target {
    constructor() {
        this.id = "NULL";
        this.varyings = [];
        this.instructions = [];
        this.labels = [];
        this.instrMap = [];
    }
    reset() {
        this.labels = [];
        this.instrMap = [];
    }
    getLabelAddress(label) {
        const l = this.labels.find(lab => lab.id === label);
        if (!l)
            return null;
        let offset = 0;
        for (let i = 0; i < l.address; i++) {
            offset += this.instrMap[i].size;
        }
        return offset;
    }
    preprocess(lines) {
        const instructionLines = [];
        let pos = 0;
        lines.forEach(lineref => {
            // Remove comments
            let line = lineref.data;
            line = line.replace(/\/\/.*$/, "").trim();
            if (/^[a-z_][a-z0-9_]*\:$/i.test(line)) {
                this.labels.push({
                    id: line.substring(0, line.length - 1),
                    address: pos
                });
            }
            else if (line.length) {
                instructionLines.push(lineref);
                pos++;
            }
        });
        return instructionLines;
    }
    createInstructionMap(lines) {
        lines.forEach(lineref => {
            const line = lineref.data;
            const args = line.split(/\s+/);
            const instr = this.instructions.find(ins => ins.name === args[0]);
            if (!instr)
                throw new error_1.OvkError(`Instruction ${args[0]} is not supported by the selected target.`, lineref);
            this.instrMap.push(instr);
        });
    }
    process(lines) {
        const idBuffer = Buffer.from("OVKL" + this.id);
        const instrBuffers = [];
        this.createInstructionMap(lines);
        lines.forEach(lineref => {
            const line = lineref.data;
            const args = line.split(/\s+/);
            const instr = this.instructions.find(ins => ins.name === args[0]);
            if (!instr)
                throw new error_1.OvkError(`Instruction ${args[0]} is not supported by the selected target.`, lineref);
            instrBuffers.push(instr.write(this, args.slice(1), lineref));
        });
        return Buffer.concat([idBuffer, ...instrBuffers]);
    }
}
exports.Target = Target;
