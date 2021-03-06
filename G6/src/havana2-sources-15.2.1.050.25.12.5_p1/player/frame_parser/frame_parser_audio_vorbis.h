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

#ifndef H_FRAME_PARSER_AUDIO_VORBIS
#define H_FRAME_PARSER_AUDIO_VORBIS

#include "vorbis_audio.h"
#include "frame_parser_audio.h"

#undef TRACE_TAG
#define TRACE_TAG "FrameParser_AudioVorbis_c"

typedef enum
{
    VorbisNoHeaders                     = 0x00,
    VorbisIdentificationHeader          = 0x01,
    VorbisCommentHeader                 = 0x02,
    VorbisSetupHeader                   = 0x04,
    VorbisAllHeaders                    = 0x07,
} VorbisHeader_t;


/// Frame parser for Ogg Vorbis audio
class FrameParser_AudioVorbis_c : public FrameParser_Audio_c
{
public:
    FrameParser_AudioVorbis_c(void);
    ~FrameParser_AudioVorbis_c(void);

    // FrameParser class functions

    FrameParserStatus_t   Connect(Port_c *Port);

    // Stream specific functions

    FrameParserStatus_t   ReadHeaders(void);

    FrameParserStatus_t   ResetReferenceFrameList(void);
    FrameParserStatus_t   PurgeQueuedPostDecodeParameterSettings(void);
    FrameParserStatus_t   PrepareReferenceFrameList(void);
    FrameParserStatus_t   ProcessQueuedPostDecodeParameterSettings(void);
    FrameParserStatus_t   GeneratePostDecodeParameterSettings(void);
    FrameParserStatus_t   UpdateReferenceFrameList(void);

    FrameParserStatus_t   ProcessReverseDecodeUnsatisfiedReferenceStack(void);
    FrameParserStatus_t   ProcessReverseDecodeStack(void);
    FrameParserStatus_t   PurgeReverseDecodeUnsatisfiedReferenceStack(void);
    FrameParserStatus_t   PurgeReverseDecodeStack(void);
    FrameParserStatus_t   TestForTrickModeFrameDrop(void);

private:
    VorbisAudioParsedFrameHeader_t      ParsedFrameHeader;

    VorbisAudioStreamParameters_t      *StreamParameters;
    VorbisAudioStreamParameters_t       CurrentStreamParameters;
    VorbisAudioFrameParameters_t       *FrameParameters;

    unsigned int                        StreamHeadersRead;

    FrameParserStatus_t ReadStreamHeaders(void);

    DISALLOW_COPY_AND_ASSIGN(FrameParser_AudioVorbis_c);
};

#endif
