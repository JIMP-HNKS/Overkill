#include <stdio.h>
#include <stdlib.h>
#include <png.h>

#include "lib/overkill.h"

#define PNG_WIDTH 256
#define PNG_HEIGHT 256

int main(int argc, char **argv) {
    ovk_program_pt prog = NULL;
    ovk_context_pt ctx = NULL;

    if (ovk_load_program("../compiler/test.ovk", &prog)) {
        printf("Error while loading!\n");
        return 1;
    }

    if (ovk_create_context(1024, &ctx)) {
        printf("Error while creating context!\n");
        return 1;
    }

    FILE *f = fopen("out.png", "wb");
    png_structp png_data;
    png_infop png_info;
    png_bytep png_row;

    int p_width = PNG_WIDTH;
    int p_height = PNG_HEIGHT;

    if (f == NULL) return 1; // like if you cry every time

    // Writing time i guess
    png_data = png_create_write_struct(
        PNG_LIBPNG_VER_STRING,
        NULL, NULL, NULL
    );
    if (png_data == NULL) return 2;

    // Info-chan is here with us
    png_info = png_create_info_struct(png_data);
    if (png_info == NULL) return 3;
    
    png_init_io(png_data, f);

    // Header time!
    png_set_IHDR(
        png_data, png_info,
        p_width, p_height,
        8, PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE
    );
    png_write_info(png_data, png_info);

    // Write stuff
    png_row = (png_bytep)malloc(
        3 * p_width * sizeof(png_byte)
    );

    for (int y = 0; y < p_height; y++) {
        for (int x = 0; x < p_width; x++) {
            ovk_reset_context(ctx);
            ovk_uni1_init_variants(
                ctx,
                (float)x / PNG_WIDTH,
                (float)y / PNG_HEIGHT
            );
            ovk_run(ctx, prog, ovk_uni1_process_opcode);

            png_row[x * 3] = (char)(255.0 * ovk_get_variant(ctx, OVK_UNI1_V_OUT_PIXEL_RED));
            png_row[x * 3 + 1] = (char)(255.0 * ovk_get_variant(ctx, OVK_UNI1_V_OUT_PIXEL_GREEN));
            png_row[x * 3 + 2] = (char)(255.0 * ovk_get_variant(ctx, OVK_UNI1_V_OUT_PIXEL_BLUE));
        }
        png_write_row(png_data, png_row);
    }

    png_write_end(png_data, NULL);

    // Free The Garbage
    fclose(f);
    png_free_data(
        png_data, png_info, PNG_FREE_ALL, -1
    );
    png_destroy_write_struct(
        &png_data, (png_infopp)NULL
    );
    free(png_info);
    if (png_row != NULL) free(png_row);

    ovk_free_context(ctx);
    ovk_free_program(prog);

    return 0;
}