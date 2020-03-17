#include <stdio.h>

#include "lib/overkill.h"

int main(int argc, char **argv) {
    ovk_program_pt prog = NULL;

    if (ovk_load_program("../compiler/test.ovk", &prog)) {
        printf("Error!\n");
    }

    printf("Ok! Target: %s, size: %d\n", prog->target_id, prog->program_size);

    ovk_free_program(prog);

    return 0;
}