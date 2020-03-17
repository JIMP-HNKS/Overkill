import { LineRef } from "./lineutil";

export class OvkError {
    constructor(
        public message: string,
        public line: LineRef
    ) {}
}