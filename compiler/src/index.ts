// todo
// run with npx . <args>

import yargs = require("yargs");
import { writeFile } from "fs-extra";
import { getLines } from "./lineutil";
import { OvkError } from "./error";
import { UNI1Target } from "./targets/UNI1";
import { Target } from "./targets/target";
import { TGOLTarget } from "./targets/TGOL";

const supportedTargets: Target[] = [
    new UNI1Target(),
    new TGOLTarget()
]
const files: string[] = yargs.argv._

if (!files.length) {
    console.error(
        `Overkill error\n\tNo input files.`
    )
    process.exit()
}

let targetId = "UNI1"
if (yargs.argv.target) targetId = yargs.argv.target as string

const target = supportedTargets.find(t => t.id === targetId)

if (!target) {
    console.error(
        `Overkill error\n\tUnsupported target ${targetId}.`
    )
    process.exit()
}

files.forEach(async (file) => {
    const extPos = file.lastIndexOf(".")
    const newName = file.substring(0, extPos < 0 ? file.length : extPos) + ".ovk"

    target.reset()

    try {
        const lines = await getLines(file)

        const instrLines = target.preprocess(lines)        
        const buf = target.process(instrLines)

        await writeFile(newName, buf)
    } catch(e) {
        if (e instanceof OvkError) {
            console.error(
                `Overkill error - in ${e.line.file}, line ${e.line.lineNumber}:\n\t${e.line.data}\n\t${e.message}`
            )
        } else {
            console.error(`General error:\n\t${e}`)
            process.exit()
        }
    }
})