import { Target, Varying, IInstruction } from "./target";
import { LineRef } from "../lineutil";
import { OvkError } from "../error";

class UNI1JumpInstruction implements IInstruction {
    public size = 5

    constructor(
        public name: string,
        public opcode: number
    ) {}

    write(target: Target, args: string[], line: LineRef): Buffer {
        if (args.length !== 1) throw new OvkError(
            "Invalid number of arguments specified.", line
        )

        const address = target.getLabelAddress(args[0])
        if (address === null) throw new OvkError(
            `Label ${args[0]} not found.`, line
        )

        const buf = Buffer.alloc(1 + 4) // opcode + address[4]

        buf.writeInt8(this.opcode, 0)
        buf.writeInt32LE(address, 1)

        return buf
    }
}

class UNI1ZeroOpInstruction implements IInstruction {
    public size = 1

    constructor(
        public name: string,
        public opcode: number
    ) {}

    write(target: Target, args: string[], line: LineRef): Buffer {
        if (args.length !== 0) throw new OvkError(
            "Invalid number of arguments specified.", line
        )

        const buf = Buffer.alloc(1) // opcode

        buf.writeInt8(this.opcode, 0)

        return buf
    }
}

class UNI1PushcInstruction implements IInstruction {
    public size = 5

    public name = "pushc"
    public opcode = 0x10

    write(target: Target, args: string[], line: LineRef): Buffer {
        if (args.length !== 1) throw new OvkError(
            "Invalid number of arguments specified.", line
        )

        const val = +args[0]
        if (isNaN(val)) throw new OvkError(
            "The argument must be a number.", line
        )
        const buf = Buffer.alloc(1 + 4) // opcode + float

        buf.writeInt8(this.opcode, 0)
        buf.writeFloatLE(val, 1)

        return buf
    }
}

class UNI1PushvInstruction implements IInstruction {
    public size = 2

    public name = "pushv"
    public opcode = 0x11

    write(target: Target, args: string[], line: LineRef): Buffer {
        if (args.length !== 1) throw new OvkError(
            "Invalid number of arguments specified.", line
        )

        const varying = target.varyings.find(v => v.id === args[0])
        if (!varying) throw new OvkError(
            `The ${args[0]} varying is not supported by the selected target.`, line
        )
        if (!varying.readable) throw new OvkError(
            `The ${varying.id} is not readable.`, line
        )
        const buf = Buffer.alloc(1 + 1) // opcode + varying id

        buf.writeInt8(this.opcode, 0)
        buf.writeInt8(varying.opcode, 1)

        return buf
    }
}

class UNI1SetInstruction implements IInstruction {
    public size = 2

    constructor(
        public name: string,
        public opcode: number
    ) {}

    write(target: Target, args: string[], line: LineRef): Buffer {
        if (args.length !== 1) throw new OvkError(
            "Invalid number of arguments specified.", line
        )

        const varying = target.varyings.find(v => v.id === args[0])
        if (!varying) throw new OvkError(
            `The ${args[0]} varying is not supported by the selected target.`, line
        )
        if (!varying.writable) throw new OvkError(
            `The ${varying.id} is not writable.`, line
        )
        const buf = Buffer.alloc(1 + 1) // opcode + varying id

        buf.writeInt8(this.opcode, 0)
        buf.writeInt8(varying.opcode, 1)

        return buf
    }
}

export class UNI1Target extends Target {
    public id = "UNI1"
    public varyings: Varying[] = [
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
    ]
    public instructions: IInstruction[] = [
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
    ]
}