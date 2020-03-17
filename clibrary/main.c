#include <stdio.h>

#include "lib/overkill.h"

int main(int argc, char **argv) {
    ovk_program_pt prog = NULL;
    ovk_context_pt ctx = NULL;

    if (ovk_load_program("../compiler/test.ovk", &prog)) {
        printf("Error while loading!\n");
        return 1;
    }

    printf("Loaded! Target: %s, size: %d\n", prog->target_id, prog->program_size);

    if (ovk_create_context(1024, &ctx)) {
        printf("Error while creating context!\n");
        return 1;
    }

    printf("Context! Stack size: %d\n", ctx->stack_length);

    ovk_uni1_init_variants(ctx, 0.0, 0.5);

    ovk_run(ctx, prog, ovk_uni1_process_opcode);

    printf("Red: %g\n", ovk_get_variant(ctx, OVK_UNI1_V_OUT_PIXEL_RED));
    printf("Green: %g\n", ovk_get_variant(ctx, OVK_UNI1_V_OUT_PIXEL_GREEN));
    printf("Blue: %g\n", ovk_get_variant(ctx, OVK_UNI1_V_OUT_PIXEL_BLUE));

    printf("\nStack data:\n");

    for (int i = 0; i <= ctx->stack_pos; i++) {
        printf("[%d]: %f\n", i, ctx->stack_data[i]);
    }

    ovk_free_context(ctx);
    ovk_free_program(prog);

    return 0;
}