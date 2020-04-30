/*
 * ff_ffpipeline_vdec.c
 *
 * Copyright (c) 2003 Bilibili
 * Copyright (c) 2003 Fabrice Bellard
 * Copyright (c) 2014 Zhang Rui <bbcallen@gmail.com>
 *
 * This file is part of ijkPlayer.
 *
 * ijkPlayer is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * ijkPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with ijkPlayer; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "ff_ffpipenode.h"
#include <stdlib.h>
#include <string.h>

IJKFF_Pipenode *ffpipenode_alloc(size_t opaque_size)
{
    IJKFF_Pipenode *node = (IJKFF_Pipenode*) calloc(1, sizeof(IJKFF_Pipenode));
    if (!node)
        return NULL;

    node->opaque = calloc(1, opaque_size);
    if (!node->opaque) {
        free(node);
        return NULL;
    }

    node->mutex = SDL_CreateMutex();
    if (node->mutex == NULL) {
        free(node->opaque);
        free(node);
        return NULL;
    }

    return node;
}

void ffpipenode_free(IJKFF_Pipenode *node)
{
    if (!node)
        return;

    if (node->func_destroy) {
        node->func_destroy(node);
    }

    SDL_DestroyMutexP(&node->mutex);

    free(node->opaque);
    memset(node, 0, sizeof(IJKFF_Pipenode));
    free(node);
}

void ffpipenode_free_p(IJKFF_Pipenode **node)
{
    if (!node)
        return;

    ffpipenode_free(*node);
    *node = NULL;
}

int ffpipenode_run_sync(IJKFF_Pipenode *node)
{
    /*函数指针, 解耦各个平台 解码差异
     node->func_run_sync(node)的node来源于ffp->node_vdec
     
     ffp->node_vdec来源于pipeline->func_open_video_decoder(pipeline, ffp)
     
     而pipeline来源于最开始init播放器的时候的_mediaPlayer = ijkmp_ios_create(media_player_msg_loop);安卓平台可能就不是这个函数了。也就是pipeline从播放器初始化的时候就不一样了，然后node就不同了，node的func_run_sync也跟着不同了。
     iOS里面做了VideoToolbox和ffmpeg解码的区分。
     。
     */
    return node->func_run_sync(node);
}

int ffpipenode_flush(IJKFF_Pipenode *node)
{
    if (!node || !node->func_flush)
        return 0;

    return node->func_flush(node);
}
