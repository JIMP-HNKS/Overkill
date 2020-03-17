import { readFile } from "fs-extra"

export type LineRef = {
    data: string
    file: string
    lineNumber: number
}

export async function getLines(file: string): Promise<LineRef[]> {
    const data = await readFile(file, "utf-8")

    return data.split(/\r?\n/).map((line, i) => {
        return {
            data: line,
            file,
            lineNumber: i + 1
        }
    })
}