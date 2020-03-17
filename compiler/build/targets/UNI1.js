"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const target_1 = require("./target");
const error_1 = require("../error");
class UNI1JumpInstruction {
    constructor(name, opcode) {
        this.name = name;
        this.opcode = opcode;
        this.size = 5;
    }
    write(target, args, line) {
        if (args.length !== 1)
            throw new error_1.OvkError("Invalid number of arguments specified.", line);
        const address = target.getLabelAddress(args[0]);
        if (address === null)
            throw new error_1.OvkError(`Label ${args[0]} not found.`, line);
        const buf = Buffer.alloc(1 + 4); // opcode + address[4]
        buf.writeInt8(this.opcode, 0);
        buf.writeInt32LE(address, 1);
        return buf;
    }
}
class UNI1ZeroOpInstruction {
    constructor(name, opcode) {
        this.name = name;
        this.opcode = opcode;
        this.size = 1;
    }
    write(target, args, line) {
        if (args.length !== 0)
            throw new error_1.OvkError("Invalid number of arguments specified.", line);
        const buf = Buffer.alloc(1); // opcode
        buf.writeInt8(this.opcode, 0);
        return buf;
    }
}
class UNI1PushcInstruction {
    constructor() {
        this.size = 5;
        this.name = "pushc";
        this.opcode = 0x10;
    }
    write(target, args, line) {
        if (args.length !== 1)
            throw new error_1.OvkError("Invalid number of arguments specified.", line);
        const val = +args[0];
        if (isNaN(val))
            throw new error_1.OvkError("The argument must be a number.", line);
        const buf = Buffer.alloc(1 + 4); // opcode + float
        buf.writeInt8(this.opcode, 0);
        buf.writeFloatLE(val, 1);
        return buf;
    }
}
class UNI1PushvInstruction {
    constructor() {
        this.size = 2;
        this.name = "pushv";
        this.opcode = 0x11;
    }
    write(target, args, line) {
        if (args.length !== 1)
            throw new error_1.OvkError("Invalid number of arguments specified.", line);
        const varying = target.varyings.find(v => v.id === args[0]);
        if (!varying)
            throw new error_1.OvkError(`The ${args[0]} varying is not supported by the selected target.`, line);
        if (!varying.readable)
            throw new error_1.OvkError(`The ${varying.id} is not readable.`, line);
        const buf = Buffer.alloc(1 + 1); // opcode + varying id
        buf.writeInt8(this.opcode, 0);
        buf.writeInt8(varying.opcode, 1);
        return buf;
    }
}
class UNI1SetInstruction {
    constructor(name, opcode) {
        this.name = name;
        this.opcode = opcode;
        this.size = 2;
    }
    write(target, args, line) {
        if (args.length !== 1)
            throw new error_1.OvkError("Invalid number of arguments specified.", line);
        const varying = target.varyings.find(v => v.id === args[0]);
        if (!varying)
            throw new error_1.OvkError(`The ${args[0]} varying is not supported by the selected target.`, line);
        if (!varying.writable)
            throw new error_1.OvkError(`The ${varying.id} is not writable.`, line);
        const buf = Buffer.alloc(1 + 1); // opcode + varying id
        buf.writeInt8(this.opcode, 0);
        buf.writeInt8(varying.opcode, 1);
        return buf;
    }
}
class UNI1Target extends target_1.Target {
    constructor() {
        super(...arguments);
        this.id = "UNI1";
        this.varyings = [
            {
                id: "global_coords_x",
                opcode: 0x00,
                readable: true,
                writable: false
            },
            {
                id: "global_coords_y",
                opcode: 0x01,
                readable: true,
                writable: false
            },
            {
                id: "out_pixel_red",
                opcode: 0x10,
                readable: false,
                writable: true
            },
            {
                id: "out_pixel_green",
                opcode: 0x11,
                readable: false,
                writable: true
            },
            {
                id: "out_pixel_blue",
                opcode: 0x12,
                readable: false,
                writable: true
            },
            {
                id: "random",
                opcode: 0xFF,
                readable: true,
                writable: false
            }
        ];
        this.instructions = [
            new UNI1PushcInstruction(),
            new UNI1PushvInstruction(),
            new UNI1SetInstruction("setv0", 0x20),
            new UNI1SetInstruction("setv1", 0x21),
            new UNI1SetInstruction("setv", 0x22),
            new UNI1ZeroOpInstruction("copy", 0x23),
            new UNI1JumpInstruction("jmp", 0x30),
            new UNI1JumpInstruction("jmp0", 0x31),
            new UNI1JumpInstruction("jmp1", 0x32),
            new UNI1JumpInstruction("jmpn0", 0x33),
            new UNI1JumpInstruction("jmpn1", 0x34),
            new UNI1ZeroOpInstruction("add", 0x40),
            new UNI1ZeroOpInstruction("sub", 0x41),
            new UNI1ZeroOpInstruction("mul", 0x42),
            new UNI1ZeroOpInstruction("div", 0x43),
            new UNI1ZeroOpInstruction("int", 0x44),
            new UNI1ZeroOpInstruction("fract", 0x45),
            new UNI1ZeroOpInstruction("mod", 0x46),
        ];
    }
}
exports.UNI1Target = UNI1Target;
