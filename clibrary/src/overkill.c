#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../lib/overkill.h"

ovk_err_t ovk_load_program(const char *file_name, ovk_program_pt *out) {
    FILE *f = NULL;
    ovk_program_pt prog = NULL;

    char magic[4], *target_id;
    unsigned int prog_size = 0;

    f = fopen(file_name, "rb");
    if (f == NULL) {
        return OVK_ERR_FILEERROR;
    }

    fread(magic, sizeof(char), 4, f);
    if (strcmp(magic, "OVKL")) {
        fclose(f);
        return OVK_ERR_FILENOSIGNATURE;
    }

    prog = malloc(sizeof(ovk_program_t));
    if (prog == NULL) {
        fclose(f);
        return OVK_ERR_NULLPTR;
    }

    target_id = malloc(4 * sizeof(char));
    if (target_id == NULL) {
        free(prog);
        fclose(f);
        return OVK_ERR_NULLPTR;
    }

    fread(target_id, sizeof(char), 4, f);
    prog->target_id = target_id;

    fseek(f, 0, SEEK_END);
    prog_size = ftell(f) - 8;
    prog->program_size = prog_size;

    fseek(f, 8, SEEK_SET);
    prog->program_data = malloc(prog_size * sizeof(char));
    if (prog->program_data == NULL) {
        free(target_id);
        free(prog);
        fclose(f);
        return OVK_ERR_NULLPTR;
    }

    fread(prog->program_data, sizeof(char), prog_size, f);

    fclose(f);
    *out = prog;

    return OVK_ERR_SUCCESS;
}

ovk_err_t ovk_free_program(ovk_program_pt prog) {
    free(prog->program_data);
    free(prog->target_id);
    free(prog);

    return OVK_ERR_SUCCESS;
}

ovk_err_t ovk_create_context(unsigned int stack_size, ovk_context_pt *out) {
    ovk_context_pt ctx = NULL;

    *out = ctx;

    return OVK_ERR_SUCCESS;
}