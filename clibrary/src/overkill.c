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

    float *stack_data = NULL;

    float *variant_data = NULL;
    ovk_variant_access_t *variant_access = NULL;

    variant_data = calloc(256, sizeof(float));
    if (variant_data == NULL) {
        return OVK_ERR_NULLPTR;
    }

    variant_access = calloc(256, sizeof(float));
    if (variant_access == NULL) {
        free(variant_data);
        return OVK_ERR_NULLPTR;
    }

    stack_data = calloc(stack_size, sizeof(float));
    if (stack_data == NULL) {
        free(variant_access);
        free(variant_data);
        return OVK_ERR_NULLPTR;
    }

    ctx = malloc(sizeof(ovk_context_t));
    if (ctx == NULL) {
        free(stack_data);
        free(variant_access);
        free(variant_data);
        return OVK_ERR_NULLPTR;
    }

    ctx->stack_data = stack_data;
    ctx->stack_length = stack_size;
    ctx->stack_pos = 0;

    ctx->variant_access = variant_access;
    ctx->variant_data = variant_data;

    *out = ctx;

    return OVK_ERR_SUCCESS;
}

ovk_err_t ovk_free_context(ovk_context_pt ctx) {
    free(ctx->stack_data);
    free(ctx->variant_access);
    free(ctx->variant_data);
    free(ctx);

    return OVK_ERR_SUCCESS;
}

ovk_err_t ovk_reset_context(ovk_context_pt ctx) {
    ctx->stack_data[0] = 0.0;
    ctx->stack_pos = 0;

    for (int i = 0; i < 256; i++) {
        ctx->variant_data[i] = 0.0;
    }

    return OVK_ERR_SUCCESS;
}

ovk_err_t ovk_set_variant(
    ovk_context_pt ctx,
    char variant_index,
    float variant_value,
    ovk_variant_access_t variant_access
) {
    ctx->variant_data[variant_index] = variant_value;
    ctx->variant_access[variant_index] = variant_access;

    return OVK_ERR_SUCCESS;
}

float ovk_get_variant(
    ovk_context_pt ctx,
    char variant_index
) {
    return ctx->variant_data[variant_index];
}

ovk_err_t ovk_run(
    ovk_context_pt ctx,
    ovk_program_pt prog,
    ovk_opcode_processor_pt process_func
) {
    prog->program_pos = 0;

    while (prog->program_pos < prog->program_size) {
        ovk_err_t result = process_func(
            ctx, prog
        );
        if (result) return result;
    }

    return OVK_ERR_SUCCESS;
}

ovk_err_t ovk_push(
    ovk_context_pt ctx,
    float value
) {
    if (ctx->stack_pos == ctx->stack_length - 1) return OVK_ERR_STACKOVERFLOW;

    ctx->stack_pos++;
    ctx->stack_data[ctx->stack_pos] = value;

    return OVK_ERR_SUCCESS;
}

ovk_err_t ovk_pop(
    ovk_context_pt ctx,
    float *value
) {
    if (ctx->stack_pos == 0) return OVK_ERR_STACKUNDERFLOW;

    *value = ctx->stack_data[ctx->stack_pos];
    ctx->stack_pos--;

    return OVK_ERR_SUCCESS;
}

ovk_err_t ovk_uni1_process_opcode(
    ovk_context_pt ctx,
    ovk_program_pt prog
) {
    char opcode = prog->program_data[prog->program_pos];

    switch (opcode) {
        case OVK_UNI1_I_PUSHC:
            {
                float value = *((float*)(prog->program_data + prog->program_pos + 1));
                ovk_err_t err = ovk_push(ctx, value);

                if (err) return err;

                prog->program_pos += 5;
            }
            break;
        case OVK_UNI1_I_PUSHV:
            {
                char variant_id = prog->program_data[prog->program_pos + 1];
                ovk_variant_access_t access = ctx->variant_access[variant_id];

                if (access == OVK_VARIANT_NULL || access == OVK_VARIANT_WRITEONLY)
                    return OVK_ERR_VARIANT_NOREAD;

                float value = ctx->variant_data[variant_id];

                ovk_err_t err = ovk_push(ctx, value);
                if (err) return err;

                prog->program_pos += 2;
            }
            break;
        
        case OVK_UNI1_I_JMP:
            {
                unsigned int address = *((unsigned int*)(prog->program_data + prog->program_pos + 1));

                prog->program_pos = address;
            }
            break;
        case OVK_UNI1_I_JMP0:
            {
                unsigned int address = *((unsigned int*)(prog->program_data + prog->program_pos + 1));
                float value = -1.0;

                ovk_err_t err = ovk_pop(ctx, &value);
                if (err) return err;

                if (value == 0.0)
                    prog->program_pos = address;
                else
                    prog->program_pos += 5;
            }
            break;
        case OVK_UNI1_I_JMP1:
            {
                unsigned int address = *((unsigned int*)(prog->program_data + prog->program_pos + 1));
                float value = -1.0;

                ovk_err_t err = ovk_pop(ctx, &value);
                if (err) return err;

                if (value == 1.0)
                    prog->program_pos = address;
                else
                    prog->program_pos += 5;
            }
            break;
        case OVK_UNI1_I_JMPN0:
            {
                unsigned int address = *((unsigned int*)(prog->program_data + prog->program_pos + 1));
                float value = -1.0;

                ovk_err_t err = ovk_pop(ctx, &value);
                if (err) return err;

                if (value != 0.0)
                    prog->program_pos = address;
                else
                    prog->program_pos += 5;
            }
            break;
        case OVK_UNI1_I_JMPN1:
            {
                unsigned int address = *((unsigned int*)(prog->program_data + prog->program_pos + 1));
                float value = -1.0;

                ovk_err_t err = ovk_pop(ctx, &value);
                if (err) return err;

                if (value != 1.0)
                    prog->program_pos = address;
                else
                    prog->program_pos += 5;
            }
            break;
        
        case OVK_UNI1_I_COPY:
            {
                float value = 0.0;
                ovk_err_t err;
                
                err = ovk_pop(ctx, &value);
                if (err) return err;

                err = ovk_push(ctx, value);
                if (err) return err;
                err = ovk_push(ctx, value);
                if (err) return err;

                prog->program_pos++;
            }
            break;
        
        case OVK_UNI1_I_ADD:
            {
                float v1 = 0.0, v2 = 0.0;
                ovk_err_t err;

                err = ovk_pop(ctx, &v2);
                if (err) return err;
                err = ovk_pop(ctx, &v1);
                if (err) return err;

                err = ovk_push(ctx, v1 + v2);
                if (err) return err;

                prog->program_pos++;
            }
            break;
        case OVK_UNI1_I_SUB:
            {
                float v1 = 0.0, v2 = 0.0;
                ovk_err_t err;

                err = ovk_pop(ctx, &v2);
                if (err) return err;
                err = ovk_pop(ctx, &v1);
                if (err) return err;

                err = ovk_push(ctx, v1 - v2);
                if (err) return err;

                prog->program_pos++;
            }
            break;
        case OVK_UNI1_I_MUL:
            {
                float v1 = 0.0, v2 = 0.0;
                ovk_err_t err;

                err = ovk_pop(ctx, &v2);
                if (err) return err;
                err = ovk_pop(ctx, &v1);
                if (err) return err;

                err = ovk_push(ctx, v1 * v2);
                if (err) return err;

                prog->program_pos++;
            }
            break;
        case OVK_UNI1_I_DIV:
            {
                float v1 = 0.0, v2 = 0.0;
                ovk_err_t err;

                err = ovk_pop(ctx, &v2);
                if (err) return err;
                err = ovk_pop(ctx, &v1);
                if (err) return err;

                err = ovk_push(ctx, v1 / v2);
                if (err) return err;

                prog->program_pos++;
            }
            break;
        case OVK_UNI1_I_INT:
            {
                float v1 = 0.0;
                ovk_err_t err;

                err = ovk_pop(ctx, &v1);
                if (err) return err;

                err = ovk_push(ctx, (float)((int)v1));
                if (err) return err;

                prog->program_pos++;
            }
            break;
        case OVK_UNI1_I_FRACT:
            {
                float v1 = 0.0;
                ovk_err_t err;

                err = ovk_pop(ctx, &v1);
                if (err) return err;

                err = ovk_push(ctx, v1 - (float)((int)v1));
                if (err) return err;

                prog->program_pos++;
            }
            break;
        case OVK_UNI1_I_MOD:
            {
                float v1 = 0.0, v2 = 0.0;
                float div;
                ovk_err_t err;

                err = ovk_pop(ctx, &v2);
                if (err) return err;
                err = ovk_pop(ctx, &v1);
                if (err) return err;

                div = v1 / v2;
                div = (float)((int)div);

                err = ovk_push(ctx, v1 - v2 * div);
                if (err) return err;

                prog->program_pos++;
            }
            break;
        
        case OVK_UNI1_I_SETV:
            {
                char variant_id = prog->program_data[prog->program_pos + 1];
                ovk_variant_access_t access = ctx->variant_access[variant_id];

                if (access == OVK_VARIANT_READONLY || access == OVK_VARIANT_NULL)
                    return OVK_ERR_VARIANT_NOWRITE;

                float value = 0.0;
                ovk_err_t err = ovk_pop(ctx, &value);
                if (err) return err;

                ctx->variant_data[variant_id] = value;

                prog->program_pos += 2;
            }
            break;
        case OVK_UNI1_I_SETV0:
            {
                char variant_id = prog->program_data[prog->program_pos + 1];
                ovk_variant_access_t access = ctx->variant_access[variant_id];

                if (access == OVK_VARIANT_READONLY || access == OVK_VARIANT_NULL)
                    return OVK_ERR_VARIANT_NOWRITE;
                
                ctx->variant_data[variant_id] = 0.0;

                prog->program_pos += 2;
            }
            break;
        case OVK_UNI1_I_SETV1:
            {
                char variant_id = prog->program_data[prog->program_pos + 1];
                ovk_variant_access_t access = ctx->variant_access[variant_id];

                if (access == OVK_VARIANT_READONLY || access == OVK_VARIANT_NULL)
                    return OVK_ERR_VARIANT_NOWRITE;
                
                ctx->variant_data[variant_id] = 1.0;

                prog->program_pos += 2;
            }
            break;

        default:
            prog->program_pos++;
    }

    return OVK_ERR_SUCCESS;
}

ovk_err_t ovk_uni1_init_variants(
    ovk_context_pt ctx,
    float global_x, float global_y
) {
    ovk_set_variant(ctx, OVK_UNI1_V_GLOBAL_COORDS_X, global_x, OVK_VARIANT_READONLY);
    ovk_set_variant(ctx, OVK_UNI1_V_GLOBAL_COORDS_Y, global_y, OVK_VARIANT_READONLY);
    ovk_set_variant(ctx, OVK_UNI1_V_OUT_PIXEL_RED, 0.0, OVK_VARIANT_WRITEONLY);
    ovk_set_variant(ctx, OVK_UNI1_V_OUT_PIXEL_GREEN, 0.0, OVK_VARIANT_WRITEONLY);
    ovk_set_variant(ctx, OVK_UNI1_V_OUT_PIXEL_BLUE, 0.0, OVK_VARIANT_WRITEONLY);
    ovk_set_variant(ctx, OVK_UNI1_V_RANDOM, 0.0, OVK_VARIANT_READONLY);

    return OVK_ERR_SUCCESS;
}