#ifndef __OVERKILL_H__
#define __OVERKILL_H__

typedef enum {
    OVK_ERR_SUCCESS = 0,
    OVK_ERR_FILEERROR = 1,
    OVK_ERR_FILENOSIGNATURE = 2,
    OVK_ERR_NULLPTR = 3
} ovk_err_t;

typedef enum {
    OVK_VARIANT_NULL = 0,
    OVK_VARIANT_READONLY = 1,
    OVK_VARIANT_WRITEONLY = 2,
    OVK_VARIANT_READWRITE = 3
} ovk_variant_access_t;

typedef struct s_ovk_context {
    unsigned int stack_length;
    unsigned int stack_pos;
    float *stack_data;

    float variant_data[256];
    ovk_variant_access_t variant_access[256];
} ovk_context_t, *ovk_context_pt;

typedef struct s_ovk_program {
    char *target_id;

    unsigned int program_size;
    unsigned int program_pos;
    char *program_data;
} ovk_program_t, *ovk_program_pt;

typedef void (*ovk_opcode_processor_pt)(ovk_context_pt, ovk_program_pt);

ovk_err_t ovk_load_program(
    const char *file_name, ovk_program_pt *out
);
ovk_err_t ovk_free_program(
    ovk_program_pt prog
);

ovk_err_t ovk_create_context(
    unsigned int stack_size, ovk_context_pt *out
);

#endif // __OVERKILL_H__