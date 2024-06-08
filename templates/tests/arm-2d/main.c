/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "arm_2d.h"
#include "arm_2d_helper.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DISPLAY_WIDTH  40
#define DISPLAY_HEIGHT 24

#define GREY8_BLACK  0
#define GREY8_WHITE  UINT8_MAX
#define ALPHA_OPAQUE UINT8_MAX

// Create root tile (i.e. frame) using helper macros which only work outside
// functions. Use 8-bit grayscale, the smallest supported color format.
dcl_fb(root_tile);
impl_fb(root_tile,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
        arm_2d_color_gray8_t,
        .tInfo.tColourInfo.chScheme = ARM_2D_COLOUR_GRAY8,
        .tInfo.bHasEnforcedColour = true);

static void print_tile(const arm_2d_tile_t *tile);

int main()
{
    printf("Arm-2D test\r\n");

    arm_2d_init();
    printf("Arm-2D initialized\r\n");

    printf("Initial image:\r\n");
    print_tile(&root_tile);

    // Create a simple pattern with cascading rectangles
    for (size_t i = 0; i < DISPLAY_HEIGHT / 2; i++) {
        arm_2d_region_t region = {.tLocation = {.iX = i, .iY = i},
                                  .tSize = {.iWidth = DISPLAY_WIDTH - 2 * i, .iHeight = DISPLAY_HEIGHT - 2 * i}};
        arm_2d_color_gray8_t color = {.tValue = (i % 2 == 0) ? GREY8_BLACK : GREY8_WHITE};
        arm_fsm_rt_t ret = arm_2dp_gray8_fill_colour_with_opacity(NULL, &root_tile, &region, color, ALPHA_OPAQUE);
        if (ret != arm_fsm_rt_cpl) {
            printf("Failed to fill color, error: %d\r\n", ret);
            return EXIT_FAILURE;
        }
    }

    printf("Final image:\r\n");
    print_tile(&root_tile);

    return EXIT_SUCCESS;
}

static void print_horizontal_border(uint16_t length)
{
    putchar('+');
    for (uint16_t h = 0; h < length; h++) {
        putchar('-');
    }
    putchar('+');
    putchar('\r');
    putchar('\n');
}

static void print_tile(const arm_2d_tile_t *tile)
{
    int16_t x = tile->tRegion.tLocation.iX;
    int16_t y = tile->tRegion.tLocation.iY;
    int16_t width = tile->tRegion.tSize.iWidth;
    int16_t height = tile->tRegion.tSize.iHeight;
    uint8_t *buffer = tile->pchBuffer;

    printf("x = %" PRId16 ", y = %" PRId16 "\r\n"
           "width = %" PRId16 ", height = %" PRId16 "\r\n"
           "buffer = 0x%" PRIXPTR "\r\n",
           x,
           y,
           width,
           height,
           (uintptr_t)buffer);

    print_horizontal_border(width);

    for (uint16_t v = 0; v < height; v++) {
        putchar('|');
        for (uint16_t h = 0; h < width; h++) {
            if (*buffer++ == 0) {
                putchar(' ');
            } else {
                putchar('X');
            }
        }
        putchar('|');
        putchar('\r');
        putchar('\n');
    }

    print_horizontal_border(width);
}
