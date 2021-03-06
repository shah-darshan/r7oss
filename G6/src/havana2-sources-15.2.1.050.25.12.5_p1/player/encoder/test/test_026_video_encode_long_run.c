/************************************************************************
Copyright (C) 2003-2014 STMicroelectronics. All Rights Reserved.

This file is part of the Streaming Engine.

Streaming Engine is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.

Streaming Engine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with Streaming Engine; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.

The Streaming Engine Library may alternatively be licensed under a
proprietary license from ST.
************************************************************************/

#include <linux/delay.h>

// For Jiffies/
#include <linux/sched.h>

// For checking returned buffers
#include <linux/crc32.h>

#include <linux/delay.h>

#include <stm_memsrc.h>
#include <stm_memsink.h>

#include "encoder_test.h"
#include "common.h"

#include "video_buffer_traffic320yuv.h"
// video_buffer_traffic320yuv defines the following:
#define VIDEO_WIDTH       TRAFFIC_VIDEO_WIDTH
#define VIDEO_HEIGHT      TRAFFIC_VIDEO_HEIGHT
#define VIDEO_BUFFER_SIZE TRAFFIC_VIDEO_BUFFER_SIZE
#define VIDEO_SURFACE_FORMAT TRAFFIC_VIDEO_SURFACE_FORMAT

// Raster formats = bytes per pixel - all planar formats = 1
// Planar formats = total plane size as 2xfactor of main plane - all raster format = 2
// Width Alignment (bytes)
// Height Alignment (bytes) - obtained from SE DecodedBufferManager

static const int LUT[14][4] =
{
    {1, 2, 1,  1}, //    SURFACE_FORMAT_UNKNOWN,
    {1, 2, 1,  1}, //    SURFACE_FORMAT_MARKER_FRAME,
    {1, 2, 1,  1}, //    SURFACE_FORMAT_AUDIO,
    {1, 3, 16, 32},//    SURFACE_FORMAT_VIDEO_420_MACROBLOCK,
    {1, 3, 16, 32},//    SURFACE_FORMAT_VIDEO_420_PAIRED_MACROBLOCK,
    {2, 2, 4,  1}, //    SURFACE_FORMAT_VIDEO_422_RASTER,
    {1, 3, 2,  16}, //    SURFACE_FORMAT_VIDEO_420_PLANAR,
    {1, 3, 2,  16}, //    SURFACE_FORMAT_VIDEO_420_PLANAR_ALIGNED,
    {1, 4, 2,  16}, //    SURFACE_FORMAT_VIDEO_422_PLANAR,
    {4, 2, 4,  32}, //    SURFACE_FORMAT_VIDEO_8888_ARGB,
    {3, 2, 4,  32}, //    SURFACE_FORMAT_VIDEO_888_RGB,
    {2, 2, 2,  32}, //    SURFACE_FORMAT_VIDEO_565_RGB,
    {2, 2, 4,  32}, //    SURFACE_FORMAT_VIDEO_422_YUYV,
    {1, 3, 4,  32}, //    SURFACE_FORMAT_VIDEO_420_RASTER2B - Alignment may not be true in practice if input buffer not from decoder
};

// Some locals
#define FHD_WIDTH       1920
#define FHD_HEIGHT      1088
#define SD_WIDTH         720
#define VIDEO_FRAME_RATE_NUM (25)
#define VIDEO_FRAME_RATE_DEN (1)
#define TIME_PER_FRAME ((1000*VIDEO_FRAME_RATE_DEN)/VIDEO_FRAME_RATE_NUM)

// Determine the output frame size....
#define VIDEO_PITCH     (((VIDEO_WIDTH*LUT[VIDEO_SURFACE_FORMAT][0] + LUT[VIDEO_SURFACE_FORMAT][2]-1)/LUT[VIDEO_SURFACE_FORMAT][2])*LUT[VIDEO_SURFACE_FORMAT][2])
#define VIDEO_BUFFER_HEIGHT     (((VIDEO_HEIGHT + LUT[VIDEO_SURFACE_FORMAT][3]-1)/LUT[VIDEO_SURFACE_FORMAT][3])*LUT[VIDEO_SURFACE_FORMAT][3])
#define VIDEO_FRAME_SIZE    ((VIDEO_PITCH*VIDEO_BUFFER_HEIGHT*LUT[VIDEO_SURFACE_FORMAT][1])/2)
#define TEST_BUFFER_SIZE    min(VIDEO_FRAME_SIZE, VIDEO_BUFFER_SIZE)
#define VERTICAL_ALIGNMENT  (LUT[VIDEO_SURFACE_FORMAT][3])

#define COMPRESSED_BUFFER_SIZE TEST_BUFFER_SIZE //FHD_WIDTH*FHD_HEIGHT
#define VIDEO_BITRATE_MODE STM_SE_CTRL_VALUE_BITRATE_CONTROL_MODE_VBR
//#define VIDEO_BITRATE_MODE -1 //Fix qp
#define VIDEO_BITRATE 4000000
#define VIDEO_CPB_BUFFER_SIZE (VIDEO_BITRATE*2)

//#define DUMP_INPUT_BUFFER
//#define DUMP_OUTPUT_BUFFER
//#define DUMP_OUTPUT_BUFFER_CRC

#define PTS_TIME_SCALE 90000

/* Return: -1: fail; 0: pass 1: EOS */
static int PullCompressedEncode(pp_descriptor *memsinkDescriptor)
{
    int retval = 0;
    stm_se_capture_buffer_t *p_memsink_pull_buffer;
    int loop = 0;

    //Wait for encoded frame to be available for memsink
    do
    {
        loop ++;
        memsinkDescriptor->m_availableLen = 0;
        retval = stm_memsink_test_for_data(memsinkDescriptor->obj, &memsinkDescriptor->m_availableLen);

        if (retval && (-EAGAIN != retval))
        {
            pr_err("Error: %s stm_memsink_test_for_data failed (%d)\n", __func__, retval);
            return -1;
        }

        mdelay(1); //in ms

        if (loop == 100)
        {
            pr_err("Error: %s Nothing available\n", __func__);
            break;
        }
    }
    while (-EAGAIN == retval);

    //
    // setup memsink_pull_buffer
    //
    p_memsink_pull_buffer = (stm_se_capture_buffer_t *)memsinkDescriptor->m_Buffer;
    p_memsink_pull_buffer->physical_address = NULL;
    p_memsink_pull_buffer->virtual_address  = &memsinkDescriptor->m_Buffer      [sizeof(stm_se_capture_buffer_t)];
    p_memsink_pull_buffer->buffer_length    = COMPRESSED_BUFFER_SIZE           - sizeof(stm_se_capture_buffer_t);
    retval = stm_memsink_pull_data(memsinkDescriptor->obj,
                                   p_memsink_pull_buffer,
                                   p_memsink_pull_buffer->buffer_length,
                                   &memsinkDescriptor->m_availableLen);

    if (retval != 0)
    {
        pr_err("Error: %s stm_memsink_pull_data fails (%d)\n", __func__, retval);
        return -1;
    }

    // Check for EOS
    if (p_memsink_pull_buffer->u.compressed.discontinuity & STM_SE_DISCONTINUITY_EOS)
    {
        pr_info("%s EOS Detected! discontinuity 0x%x\n", __func__, p_memsink_pull_buffer->u.compressed.discontinuity);
        return 1;
    }

#ifdef DUMP_OUTPUT_BUFFER
    print_hex_dump(KERN_INFO, "OUTPUT BUFFER DUMP IN HEXADECIMAL: ", DUMP_PREFIX_OFFSET, 16, 1,
                   p_memsink_pull_buffer->virtual_address, min((unsigned) 64, memsinkDescriptor->m_availableLen), true);
#endif
#ifdef DUMP_OUTPUT_BUFFER_CRC
    pr_info("CRC Value of this frame pull is 0x%08x\n", crc32_be(crc, p_memsink_pull_buffer->virtual_address, memsinkDescriptor->m_availableLen));
#endif
    return 0;
}

static int init_stream(EncoderContext *pContext)
{
    int result = 0;

    // Create a MemSink
    result = stm_memsink_new("EncoderVideoSink", STM_IOMODE_BLOCKING_IO, KERNEL, (stm_memsink_h *)&pContext->sink[0].obj);
    if (result < 0)
    {
        pr_err("Error: %s Failed to create a new memsink\n", __func__);
        return result;
    }

    // Create Stream
    result = stm_se_encode_stream_new("EncodeStream0", pContext->encode, STM_SE_ENCODE_STREAM_ENCODING_VIDEO_H264, &pContext->video_stream[0]);
    if (result < 0)
    {
        pr_err("Error: %s Failed to create a new STM_SE_ENCODE_STREAM_ENCODING_VIDEO_H264 EncodeStream\n", __func__);
        goto stream_new_fail;
    }

    // Attach the memsink
    result = stm_se_encode_stream_attach(pContext->video_stream[0], pContext->sink[0].obj);
    if (result < 0)
    {
        pr_err("Error: %s Failed to attach encode stream to memsink for video\n", __func__);
        goto stream_attach_fail;
    }

    result = VideoEncodeEventSubscribe(pContext);
    if (result < 0)
    {
        pr_err("Error: %s - VideoEncodeEventSubscribe failed\n", __func__);
        goto subscribe_fail;
    }

    // STM_SE_CTRL_ENCODE_STREAM_BITRATE_MODE Control
    result = stm_se_encode_stream_set_control(pContext->video_stream[0], STM_SE_CTRL_ENCODE_STREAM_BITRATE_MODE, VIDEO_BITRATE_MODE);
    if (result < 0)
    {
        pr_err("Error: %s Failed to set bitrate mode control\n", __func__);
        goto set_control_fail;
    }

    // STM_SE_CTRL_ENCODE_STREAM_BITRATE Control
    result = stm_se_encode_stream_set_control(pContext->video_stream[0], STM_SE_CTRL_ENCODE_STREAM_BITRATE, VIDEO_BITRATE);
    if (result < 0)
    {
        pr_err("Error: %s Failed to set bitrate control\n", __func__);
        goto set_control_fail;
    }

    // STM_SE_CTRL_VIDEO_ENCODE_STREAM_H264_CPB_SIZE Control
    result = stm_se_encode_stream_set_control(pContext->video_stream[0], STM_SE_CTRL_VIDEO_ENCODE_STREAM_H264_CPB_SIZE, VIDEO_CPB_BUFFER_SIZE);
    if (result < 0)
    {
        pr_err("Error: %s Failed to set cpb size control\n", __func__);
        goto set_control_fail;
    }

    return 0;

set_control_fail:
    VideoEncodeEventUnsubscribe(pContext);
subscribe_fail:
    if (stm_se_encode_stream_detach(pContext->video_stream[0], pContext->sink[0].obj)) {};
stream_attach_fail:
    if (stm_se_encode_stream_delete(pContext->video_stream[0])) {};
stream_new_fail:
    if (stm_memsink_delete((stm_memsink_h)pContext->sink[0].obj)) {};

    return result;
}

static int deinit_stream(EncoderContext *pContext)
{
    int result = 0;

    result = stm_se_encode_stream_inject_discontinuity(pContext->video_stream[0], STM_SE_DISCONTINUITY_EOS);
    if (result < 0)
    {
        pr_err("Error: %s Failed to inject discontinuity EOS\n", __func__);
        goto fail_inject_discontinuity;
    }

    result = PullCompressedEncode(&pContext->sink[0]);
    if (result < 0)
    {
        pr_err("Error: %s Expected pass on testing pull compressed frame\n", __func__);
        goto fail_inject_discontinuity;
    }

    result = VideoEncodeEventUnsubscribe(pContext);
    if (result < 0)
    {
        pr_err("Error: %s - VideoEncodeEventUnsubscribe failed\n", __func__);
        goto unsubscribe_fail;
    }

    result = stm_se_encode_stream_detach(pContext->video_stream[0], pContext->sink[0].obj);
    if (result < 0)
    {
        pr_err("Error: %s Failed to detach encode stream from memsink\n", __func__);
        goto fail_stream_detach;
    }

    // Remove Stream
    result = stm_se_encode_stream_delete(pContext->video_stream[0]);
    if (result < 0)
    {
        pr_err("Error: %s Failed to delete encode stream %d\n", __func__, result);
        goto fail_stream_delete;
    }

    // Remove the memsink
    result = stm_memsink_delete((stm_memsink_h)pContext->sink[0].obj);
    if (result < 0)
    {
        pr_err("Error: %s Failed to delete memsink\n", __func__);
        return result;
    }

    return 0;

fail_inject_discontinuity:
    VideoEncodeEventUnsubscribe(pContext);
unsubscribe_fail:
    if (stm_se_encode_stream_detach(pContext->video_stream[0], pContext->sink[0].obj)) {};
fail_stream_detach:
    if (stm_se_encode_stream_delete(pContext->video_stream[0])) {};
fail_stream_delete:
    if (stm_memsink_delete((stm_memsink_h)pContext->sink[0].obj)) {};

    return result;
}

static int inject(EncoderContext *pContext, int duration)
{
    int iteration = -1;
    int result = 0;
    int frame_id = -1;
    unsigned int start_time_in_seconds;
    unsigned int stop_time_in_seconds;
    unsigned int current_time_in_seconds;
    bpa_buffer raw_video_frame;
    bpa_buffer compressed_video_frame;
    stm_se_uncompressed_frame_metadata_t descriptor;

    // Allocate Memory Buffer
    result = AllocateBuffer(TEST_BUFFER_SIZE, &raw_video_frame);
    if (result < 0)
    {
        pr_err("Error: %s Failed to allocate a physical buffer of size %d bytes to store the sequence (%d)\n", __func__, TEST_BUFFER_SIZE, result);
        return result;
    }

    result = AllocateBuffer(COMPRESSED_BUFFER_SIZE, &compressed_video_frame);
    if (result < 0)
    {
        pr_err("Error: %s Failed to allocate a physical buffer of size %d bytes to store the stream (%d)\n", __func__, COMPRESSED_BUFFER_SIZE, result);
        goto allocation_fail;
    }

    memset(compressed_video_frame.virtual, 0, COMPRESSED_BUFFER_SIZE);
    pContext->sink[0].m_Buffer = (unsigned char *)compressed_video_frame.virtual;
    pContext->sink[0].m_BufferLen = compressed_video_frame.size;

#ifdef DUMP_INPUT_BUFFER
    print_hex_dump(KERN_INFO, "INPUT BUFFER DUMP IN HEXADECIMAL: ", DUMP_PREFIX_OFFSET, 16, 1,
                   raw_video_frame.virtual, min((unsigned) 64, raw_video_frame.size), true);
#endif

    memset(&descriptor, 0, sizeof(descriptor));
    descriptor.media = STM_SE_ENCODE_STREAM_MEDIA_VIDEO;
    descriptor.video.frame_rate.framerate_num                        = VIDEO_FRAME_RATE_NUM;
    descriptor.video.frame_rate.framerate_den                        = VIDEO_FRAME_RATE_DEN;
    descriptor.video.video_parameters.width                          = VIDEO_WIDTH;
    descriptor.video.video_parameters.height                         = VIDEO_HEIGHT;
    descriptor.video.video_parameters.colorspace                     = (VIDEO_WIDTH > SD_WIDTH ? STM_SE_COLORSPACE_SMPTE170M : STM_SE_COLORSPACE_SMPTE240M);
    descriptor.video.video_parameters.aspect_ratio                   = STM_SE_ASPECT_RATIO_4_BY_3;
    descriptor.video.video_parameters.pixel_aspect_ratio_numerator   = 1;
    descriptor.video.video_parameters.pixel_aspect_ratio_denominator = 1;
    descriptor.video.video_parameters.scan_type                      = STM_SE_SCAN_TYPE_PROGRESSIVE;
    descriptor.video.pitch                                           = VIDEO_PITCH;
    descriptor.video.window_of_interest                              = (stm_se_picture_rectangle_t) { 0, 0, VIDEO_WIDTH, VIDEO_HEIGHT };
    descriptor.video.picture_type                                    = STM_SE_PICTURE_TYPE_UNKNOWN;
    descriptor.video.surface_format                                  = VIDEO_SURFACE_FORMAT;
    descriptor.video.vertical_alignment                              = VERTICAL_ALIGNMENT;
    descriptor.native_time_format = TIME_FORMAT_PTS;
    descriptor.native_time   = 0;
    descriptor.system_time   = 0;
    descriptor.discontinuity = STM_SE_DISCONTINUITY_CONTINUOUS;

    result = init_stream(pContext);
    if (result < 0)
    {
        pr_err("Error: %s Failed to init_stream\n", __func__);
        goto init_stream_fail;
    }

    pr_info("Start, jiffies = %lu, duration = %d\n", jiffies, duration);

    start_time_in_seconds = GetTimeInSeconds();
    stop_time_in_seconds = start_time_in_seconds + duration;

    do
    {
        iteration++;

        // Fill buffer
        frame_id = get_next_frame_id(frame_id, TRAFFIC_VIDEO_FRAME_NB);

        memcpy(raw_video_frame.virtual, &traffic_video_buffer[(TRAFFIC_VIDEO_BUFFER_SIZE / TRAFFIC_VIDEO_FRAME_NB) * frame_id], raw_video_frame.size);

        descriptor.native_time = (PTS_TIME_SCALE * VIDEO_FRAME_RATE_DEN / VIDEO_FRAME_RATE_NUM) * iteration;
        descriptor.system_time = jiffies;
        result = stm_se_encode_stream_inject_frame(pContext->video_stream[0], raw_video_frame.virtual, raw_video_frame.physical, raw_video_frame.size, descriptor);
        if (result < 0)
        {
            pr_err("Error: %s Failed to inject frame, iteration = %d, frame_id = %d\n", __func__, iteration, frame_id);
            goto set_control_fail;
        }

        result = PullCompressedEncode(&pContext->sink[0]);
        if (result < 0)
        {
            pr_err("Error: %s Failed to pull compressed frame, iteration = %d, frame_id = %d\n", __func__, iteration, frame_id);
            goto set_control_fail;
        }

        current_time_in_seconds = GetTimeInSeconds();

    }
    while (current_time_in_seconds < stop_time_in_seconds);

    pr_info("Stop, jiffies = %lu\n", jiffies);

    if (gVideoTest[0].FatalErrorReceived > 0)
    {
        pr_err("Error: %s Fatal error event, FatalErrorReceived = %u\n", __func__, gVideoTest[0].FatalErrorReceived);
        result = -1;
        goto set_control_fail;
    }

    gVideoTest[0].FatalErrorReceived = 0;

    result = deinit_stream(pContext);
    if (result < 0)
    {
        pr_err("Error: %s Failed to deinit_stream\n", __func__);
        goto deinit_stream_fail;
    }

    // Free BPA Memory
    result = FreeBuffer(&compressed_video_frame);
    if (result < 0)
    {
        pr_err("Error: %s Failed to free stream buffer (%d)\n", __func__, result);
    }

    pContext->sink[0].m_Buffer = NULL;
    pContext->sink[0].m_BufferLen = 0;

    result = FreeBuffer(&raw_video_frame);
    if (result < 0)
    {
        pr_err("Error: %s Failed to free sequence buffer (%d)\n", __func__, result);
    }

    return result;

set_control_fail:
    if (deinit_stream(pContext)) {};
init_stream_fail:
deinit_stream_fail:
    if (FreeBuffer(&compressed_video_frame)) {};
allocation_fail:
    if (FreeBuffer(&raw_video_frame)) {};

    return result;
}

static int video_encode_long_run(EncoderTestArg *arg)
{
    EncoderContext *pContext = &gContext[0];
    int result = 0;
    int duration = 0;

    memset(&gVideoTest, 0, sizeof(gVideoTest));
    if (arg->duration == 0)
    {
        duration = 3600;
        pr_info("Maximum test duration sets to %d seconds\n", duration);
    }
    else
    {
        duration = arg->duration;
        pr_info("Maximum test duration overwrites with %d seconds\n", duration);
    }

    // Create an Encode Object
    result = stm_se_encode_new("Encode0", &pContext->encode);
    if (result < 0)
    {
        pr_err("Error: %s Failed to create a new Encode\n", __func__);
        return result;
    }

    result = stm_se_encode_set_control(pContext->encode, STM_SE_CTRL_VIDEO_ENCODE_MEMORY_PROFILE, STM_SE_CTRL_VALUE_ENCODE_CIF_PROFILE);
    if (result < 0)
    {
        pr_err("Error: %s Failed to apply memory profile control on encode\n", __func__);
        goto set_control_fail;
    }

    result = inject(pContext, duration);
    if (result < 0)
    {
        pr_err("Error: %s Failed to inject frame\n", __func__);
        goto inject_fail;
    }

    result = stm_se_encode_delete(pContext->encode);
    if (result < 0)
    {
        pr_err("Error: %s stm_se_encode_delete returned %d\n", __func__, result);
        return result;
    }

    return result;

inject_fail:
set_control_fail:
    if (stm_se_encode_delete(pContext->encode)) {};

    return result;
}

/*** Only 40 Chars will be displayed ***/
ENCODER_TEST(video_encode_long_run, "Long run test for video encode", LONG_TEST);
