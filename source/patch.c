/*
 * Copyright (C) 2023 Volodymyr Atamanenko
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

/**
 * @file  patch.c
 * @brief Patching some of the .so internal functions or bridging them to native
 *        for better compatibility.
 */

#include <kubridge.h>
#include <so_util/so_util.h>
#include "utils/logger.h"
#include "reimpl/asset_manager.h"
#include <stdlib.h>
#include <string.h>

extern so_module so_mod;

#define FRAME_RECORD_SIZE 0x22

uint32_t *Load_File_Anim_patched(char *filename, int a2, int a3)
{
    AAssetManager *mgr = *(AAssetManager **)so_symbol(&so_mod, "mgr");
    AAsset *asset = AAssetManager_open(mgr, filename, AASSET_MODE_BUFFER);

    if (!asset)
    {
        l_debug("EglSample Anim File NOK");
        return NULL;
    }

    off_t file_size = AAsset_getLength(asset);
    uint8_t *filebuf = (uint8_t *)malloc(file_size);
    if (!filebuf)
    {
        AAsset_close(asset);
        return NULL;
    }
    AAsset_read(asset, filebuf, file_size);
    AAsset_close(asset);

    uint8_t *p = filebuf;

    int16_t numAnimAndOne = *(int16_t *)p;
    p += 2;
    int totalAnimations = numAnimAndOne + 1;

    int *TotalAnimation_ptr = (int *)so_symbol(&so_mod, "TotalAnimation");
    if (TotalAnimation_ptr)
        *TotalAnimation_ptr = totalAnimations;

    uint32_t **animations = (uint32_t **)malloc(sizeof(uint32_t *) * totalAnimations);

    for (int a = 0; a < totalAnimations; a++)
    {
        int16_t cols_minus1 = *(int16_t *)p;
        p += 2;
        int16_t rows_minus1 = *(int16_t *)p;
        p += 2;
        int cols = cols_minus1 + 1;
        int rows = rows_minus1 + 1;

        uint32_t *colArray = (uint32_t *)malloc(sizeof(uint32_t *) * cols);
        animations[a] = colArray;

        for (int c = 0; c < cols; c++)
        {
            uint32_t *rowArray = (uint32_t *)malloc(sizeof(uint32_t *) * rows);
            colArray[c] = (uint32_t)(uintptr_t)rowArray;

            for (int r = 0; r < rows; r++)
            {
                void *frame = malloc(FRAME_RECORD_SIZE);
                memcpy(frame, p, FRAME_RECORD_SIZE);
                p += FRAME_RECORD_SIZE;
                rowArray[r] = (uint32_t)(uintptr_t)frame;
            }
        }
    }

    l_debug("EglSample Anim File OK = %d", numAnimAndOne);

    free(filebuf);
    return (uint32_t *)animations;
}

void so_patch(void)
{
    hook_addr((uintptr_t)so_symbol(&so_mod, "_Z14Load_File_AnimPc"), (uintptr_t)&Load_File_Anim_patched);
}
