import { LineRef } from "../lineutil"
import { OvkError } from "../error"

export type Varying = {
    id: string
    opcode: number
    readable: boolean
    writable: boolean
}

export type Label = {
    id: string
    address: number
}

export interface IInstruction {
    name: string
    opcode: number
    size: number

    write: (
        target: Target, args: string[],
        line: LineRef
    ) => Buffer
}

export class Target {
    public id = "NULL"
    public varyings: Varying[] = []
    public instructions: IInstruction[] = []
    public labels: Label[] = []
    public instrMap: IInstruction[] = []

    reset() {
        this.labels = []
        this.instrMap = []
    }

    getLabelAddress(label: string): number {
        const l = this.labels.find(lab => lab.id === label)
        if (!l) return null

        let offset = 0

        for (let i = 0; i < l.address; i++) {
            offset += this.instrMap[i].size
        }

        return offset
    }

    preprocess(lines: LineRef[]): LineRef[] {
        const instructionLines: LineRef[] = []
        let pos = 0

        lines.forEach(lineref => {
            // Remove comments
            let line = lineref.data
            line = line.replace(/\/\/.*$/, "").trim()

            if (/^[a-z_][a-z0-9_]*\:$/i.test(line)) {
                this.labels.push({
                    id: line.substring(0, line.length - 1),
                    address: pos
                })
            } else if (line.length) {
                instructionLines.push(lineref)
                pos++
            }
        })

        return instructionLines
    }

    createInstructionMap(lines: LineRef[]) {
        lines.forEach(lineref => {
            const line = lineref.data
            const args = line.split(/\s+/)

            const instr = this.instructions.find(ins => ins.name === args[0])

            if (!instr) throw new OvkError(
                `Instruction ${args[0]} is not supported by the selected target.`, lineref
            )

            this.instrMap.push(instr)
        })
    }

    process(lines: LineRef[]): Buffer {
        const idBuffer = Buffer.from("OVKL" + this.id)
        const instrBuffers: Buffer[] = []

        this.createInstructionMap(lines)

        lines.forEach(lineref => {
            const line = lineref.data
            const args = line.split(/\s+/)

            const instr = this.instructions.find(ins => ins.name === args[0])

            if (!instr) throw new OvkError(
                `Instruction ${args[0]} is not supported by the selected target.`, lineref
            )

            instrBuffers.push(
                instr.write(this, args.slice(1), lineref)
            )
        })

        return Buffer.concat([idBuffer, ...instrBuffers])
    }
}
