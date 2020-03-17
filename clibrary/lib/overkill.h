#ifndef __OVERKILL_H__
#define __OVERKILL_H__

// UNI1 Varyings
#define OVK_UNI1_V_GLOBAL_COORDS_X      0x00
#define OVK_UNI1_V_GLOBAL_COORDS_Y      0x01
#define OVK_UNI1_V_OUT_PIXEL_RED        0x10
#define OVK_UNI1_V_OUT_PIXEL_GREEN      0x11
#define OVK_UNI1_V_OUT_PIXEL_BLUE       0x12
#define OVK_UNI1_V_RANDOM               0xF0

// UNI1 Varyings
#define OVK_UNI1_I_PUSHC                0x10
#define OVK_UNI1_I_PUSHV                0x11
#define OVK_UNI1_I_SETV0                0x20
#define OVK_UNI1_I_SETV1                0x21
#define OVK_UNI1_I_SETV                 0x22
#define OVK_UNI1_I_COPY                 0x23
#define OVK_UNI1_I_JMP                  0x30
#define OVK_UNI1_I_JMP0                 0x31
#define OVK_UNI1_I_JMP1                 0x32
#define OVK_UNI1_I_JMPN0                0x33
#define OVK_UNI1_I_JMPN1                0x34
#define OVK_UNI1_I_ADD                  0x40
#define OVK_UNI1_I_SUB                  0x41
#define OVK_UNI1_I_MUL                  0x42
#define OVK_UNI1_I_DIV                  0x43
#define OVK_UNI1_I_INT                  0x44
#define OVK_UNI1_I_FRACT                0x45
#define OVK_UNI1_I_MOD                  0x46

typedef enum {
    OVK_ERR_SUCCESS = 0,
    OVK_ERR_FILEERROR = 1,
    OVK_ERR_FILENOSIGNATURE = 2,
    OVK_ERR_NULLPTR = 3,
    OVK_ERR_RUNTIME = 4,
    OVK_ERR_STACKOVERFLOW = 5,
    OVK_ERR_STACKUNDERFLOW = 6,
    OVK_ERR_VARIANT_NOWRITE = 7,
    OVK_ERR_VARIANT_NOREAD = 8
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

    float *variant_data;
    ovk_variant_access_t *variant_access;
} ovk_context_t, *ovk_context_pt;

typedef struct s_ovk_program {
    char *target_id;

    unsigned int program_size;
    unsigned int program_pos;
    char *program_data;
} ovk_program_t, *ovk_program_pt;

typedef ovk_err_t (*ovk_opcode_processor_pt)(ovk_context_pt, ovk_program_pt);

ovk_err_t ovk_load_program(
    const char *file_name, ovk_program_pt *out
);
ovk_err_t ovk_free_program(
    ovk_program_pt prog
);

ovk_err_t ovk_create_context(
    unsigned int stack_size, ovk_context_pt *out
);
ovk_err_t ovk_free_context(
    ovk_context_pt ctx
);
ovk_err_t ovk_reset_context(
    ovk_context_pt ctx
);

ovk_err_t ovk_set_variant(
    ovk_context_pt ctx,
    char variant_index,
    float variant_value,
    ovk_variant_access_t variant_access
);
float ovk_get_variant(
    ovk_context_pt ctx,
    char variant_index
);

ovk_err_t ovk_push(
    ovk_context_pt ctx,
    float value
);
ovk_err_t ovk_pop(
    ovk_context_pt ctx,
    float *value
);

ovk_err_t ovk_run(
    ovk_context_pt ctx,
    ovk_program_pt prog,
    ovk_opcode_processor_pt process_func
);


ovk_err_t ovk_uni1_process_opcode(
    ovk_context_pt ctx,
    ovk_program_pt prog
);
ovk_err_t ovk_uni1_init_variants(
    ovk_context_pt ctx,
    float global_x, float global_y
);

#endif // __OVERKILL_H__