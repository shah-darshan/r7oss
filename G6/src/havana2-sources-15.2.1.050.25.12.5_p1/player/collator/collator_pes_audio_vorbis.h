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

#ifndef H_COLLATOR_PES_AUDIO_VORBIS
#define H_COLLATOR_PES_AUDIO_VORBIS

#include "collator_pes_frame.h"

#undef  TRACE_TAG
#define TRACE_TAG "Collator_PesAudioVorbis_c"

/*
The Ogg Vorbis is very similar to the wmv collator because .ogg files
contain the audio in frame chunks.  The frame length is passed from the container parser
in a private structure. This is passed into the player using the private data
structure in the pes header.
*/

class Collator_PesAudioVorbis_c : public Collator_PesFrame_c
{
public:
    Collator_PesAudioVorbis_c(void) : Collator_PesFrame_c()
    {
        SetGroupTrace(group_collator_audio);

        Configuration.StreamIdentifierMask = PES_START_CODE_MASK;
        Configuration.StreamIdentifierCode = PES_START_CODE_PRIVATE_STREAM_1;
    }
};

#endif /* H_COLLATOR_PES_AUDIO_VORBIS */

