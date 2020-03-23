"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UNI1_1 = require("./UNI1");
class TGOLTarget extends UNI1_1.UNI1Target {
    constructor() {
        super();
        this.id = "TGOL";
        this.varyings.push({
            id: "cell_coords_x",
            opcode: 0x20,
            readable: true,
            writable: false
        }, {
            id: "cell_coords_y",
            opcode: 0x21,
            readable: true,
            writable: false
        }, {
            id: "cell_state",
            opcode: 0x22,
            readable: true,
            writable: false
        });
    }
}
exports.TGOLTarget = TGOLTarget;
