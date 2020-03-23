import { UNI1Target } from "./UNI1";
import { Varying } from "./target";

export class TGOLTarget extends UNI1Target {
    public id = "TGOL"

    constructor() {
        super()

        this.varyings.push(
            {
                id: "cell_coords_x",
                opcode: 0x20,
                readable: true,
                writable: false
            },
            {
                id: "cell_coords_y",
                opcode: 0x21,
                readable: true,
                writable: false
            },
            {
                id: "cell_state",
                opcode: 0x22,
                readable: true,
                writable: false
            }
        )
    }
}