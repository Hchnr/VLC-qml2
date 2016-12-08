/*****************************************************************************
 * ambisonics_binaural.c : Ambisonics to binaural downmixer
 *****************************************************************************
 * Copyright © 2009-2016 VLC authors and VideoLAN
 *
 * Authors: Adrien Maglo <magsoft@videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

/*****************************************************************************
 * Preamble
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <assert.h>

#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_aout.h>
#include <vlc_filter.h>
#include <vlc_vout.h>
#include <vlc_keys.h>

#include <new>
#include <vector>

#include <ambisonic/Ambisonics.h>

static int Open( vlc_object_t * );
static void Close( vlc_object_t * );

static int allocateBuffers(filter_sys_t *p_sys);
static void freeBuffers(filter_sys_t *p_sys);

vlc_module_begin()
    set_description(N_("Ambisonics to binaural downmixer"))
    set_shortname( N_("Ambisonics" ) )
    set_capability("audio converter", 1)
    set_category(CAT_AUDIO)
    set_subcategory(SUBCAT_AUDIO_MISC)
    set_callbacks(Open, Close)
    add_shortcut("ambisonics")
vlc_module_end()


#define AMB_BLOCK_TIME_LEN 1024


struct filter_sys_t
{
    filter_sys_t()
        : i_inputPTS(0)
    {}

    CAmbisonicBinauralizer binauralDecoder;
    CAmbisonicProcessor processor;

    std::vector<float> inputSamples;
    mtime_t i_inputPTS;
    unsigned i_rate;
    unsigned i_order;

    float** inBuf;
    float** outBuf;
    unsigned i_inputNb;
    unsigned i_outputNb;

    /* View point. */
    float f_teta;
    float f_phi;
    float f_roll;
};


static block_t *Mix( filter_t *p_filter, block_t *p_buf )
{
    filter_sys_t *p_sys = p_filter->p_sys;

    const audio_format_t *outfmt = &p_filter->fmt_out.audio;

    const size_t i_prevSize = p_sys->inputSamples.size();
    p_sys->inputSamples.resize(i_prevSize + p_buf->i_nb_samples * p_sys->i_inputNb);
    memcpy((char*)(p_sys->inputSamples.data() + i_prevSize), (char*)p_buf->p_buffer, p_buf->i_buffer);

    const size_t i_inputBlockSize = sizeof(float) * p_sys->i_inputNb * AMB_BLOCK_TIME_LEN;
    const size_t i_outputBlockSize = sizeof(float) * p_sys->i_outputNb * AMB_BLOCK_TIME_LEN;
    const size_t i_nbBlocks = p_sys->inputSamples.size() * sizeof(float) / i_inputBlockSize;

    block_t *p_out_buf = block_Alloc(i_outputBlockSize * i_nbBlocks);
    if (unlikely(p_out_buf == NULL))
    {
        block_Release(p_buf);
        return NULL;
    }

    p_out_buf->i_nb_samples = i_nbBlocks * AMB_BLOCK_TIME_LEN;
    if (p_sys->i_inputPTS == 0)
        p_out_buf->i_pts = p_buf->i_pts;
    else
        p_out_buf->i_pts = p_sys->i_inputPTS;
    p_out_buf->i_dts = p_out_buf->i_pts;
    p_out_buf->i_length = p_out_buf->i_nb_samples * 1000000 / p_sys->i_rate;

    float *p_dest = (float *)p_out_buf->p_buffer;
    const float *p_src = (float *)p_sys->inputSamples.data();

    for (unsigned b = 0; b < i_nbBlocks; ++b)
    {
        CBFormat inData;
        inData.Create(p_sys->i_order, true, AMB_BLOCK_TIME_LEN);

        for (unsigned i = 0; i < p_sys->i_inputNb; ++i)
        {
            for (unsigned j = 0; j < AMB_BLOCK_TIME_LEN; ++j)
            {
                float val = p_src[(b * AMB_BLOCK_TIME_LEN + j) * p_sys->i_inputNb + i];
                p_sys->inBuf[i][j] = val;
            }
            inData.InsertStream(p_sys->inBuf[i], i, AMB_BLOCK_TIME_LEN);
        }

        Orientation ori = {p_sys->f_teta, p_sys->f_roll, p_sys->f_phi};
        p_sys->processor.SetOrientation(ori);
        p_sys->processor.Refresh();
        p_sys->processor.Process(&inData, inData.GetSampleCount());

        // Compute
        p_sys->binauralDecoder.Process(&inData, p_sys->outBuf);

        // Interleave the results.
        for (unsigned i = 0; i < p_sys->i_outputNb; ++i)
            for (unsigned j = 0; j < AMB_BLOCK_TIME_LEN; ++j)
                p_dest[(b * AMB_BLOCK_TIME_LEN + j) * p_sys->i_outputNb + i] = p_sys->outBuf[i][j];
    }

    p_sys->inputSamples.erase(p_sys->inputSamples.begin(),
                              p_sys->inputSamples.begin() + i_inputBlockSize * i_nbBlocks / sizeof(float));

    assert(p_sys->inputSamples.size() < i_inputBlockSize);

    p_sys->i_inputPTS = p_out_buf->i_pts + p_out_buf->i_length;

    block_Release(p_buf);
    return p_out_buf;
}


static void Flush( filter_t *p_filter )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    p_sys->inputSamples.clear();
    p_sys->i_inputPTS = 0;
}


static void ChangeViewpoint( filter_t *p_filter, const vlc_viewpoint_t *p_vp)
{
    filter_sys_t *p_sys = (filter_sys_t *)p_filter->p_sys;

#define RAD(d) ((float) ((d) * M_PI / 180.f))
    p_sys->f_teta = -RAD(p_vp->yaw) - M_PI_2;
    p_sys->f_phi = RAD(p_vp->pitch);
    p_sys->f_roll = RAD(p_vp->roll);
#undef RAD
}


static int Open(vlc_object_t *p_this)
{
    filter_t *p_filter = (filter_t *)p_this;
    audio_format_t *infmt = &p_filter->fmt_in.audio;
    audio_format_t *outfmt = &p_filter->fmt_out.audio;

    if (infmt->channel_type != AMBISONICS_CHANNELS
        || outfmt->channel_type == AMBISONICS_CHANNELS
        || outfmt->i_physical_channels != AOUT_CHANS_STEREO)
        return VLC_EGENERIC;

    infmt->i_format = VLC_CODEC_FL32;
    infmt->i_rate = 48000;
    outfmt->i_format = VLC_CODEC_FL32;

    filter_sys_t *p_sys;
    p_sys = p_filter->p_sys = (filter_sys_t*)new(std::nothrow)filter_sys_t();
    if (p_sys == NULL)
        return VLC_ENOMEM;

    p_sys->f_teta = -M_PI_2;
    p_sys->i_rate = p_filter->fmt_in.audio.i_rate;
    p_sys->i_inputNb = p_filter->fmt_in.audio.i_channels;
    p_sys->i_outputNb = p_filter->fmt_out.audio.i_channels;

    if (allocateBuffers(p_sys) != VLC_SUCCESS)
    {
        delete p_sys;
        return VLC_ENOMEM;
    }

    p_sys->i_order = sqrt(infmt->i_channels) - 1;

    if (p_sys->i_order < 1)
    {
        msg_Err(p_filter, "Invalid number of Ambisonics channels");
        delete p_sys;
        return VLC_EGENERIC;
    }

    msg_Dbg(p_filter, "Order: %d %d", p_sys->i_order, infmt->i_channels);

    unsigned i_tailLength = 0;

    if (!p_sys->binauralDecoder.Create(p_sys->i_order, true,
            p_sys->i_rate, AMB_BLOCK_TIME_LEN, false, i_tailLength))
    {
        msg_Err(p_filter, "Error creating the binaural decoder.");
        freeBuffers(p_sys);
        delete p_sys;
        return VLC_EGENERIC;
    }
    p_sys->binauralDecoder.Reset();

    if (!p_sys->processor.Create(p_sys->i_order, true, 0))
    {
        msg_Err(p_filter, "Error creating the ambisonic processor.");
        freeBuffers(p_sys);
        delete p_sys;
        return VLC_EGENERIC;
    }

    p_filter->pf_audio_filter = Mix;
    p_filter->pf_flush = Flush;
    p_filter->pf_change_viewpoint = ChangeViewpoint;

    return VLC_SUCCESS;
}


static void Close(vlc_object_t *p_this)
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;

    freeBuffers(p_sys);
    delete p_filter->p_sys;
}


static int allocateBuffers(filter_sys_t *p_sys)
{
    #define MEMORY_ERROR() {freeBuffers(p_sys); return VLC_ENOMEM;}

    p_sys->inBuf = (float**)calloc(p_sys->i_inputNb, sizeof(float*));
    if (p_sys->inBuf == NULL)
        MEMORY_ERROR()

    for (unsigned i = 0; i < p_sys->i_inputNb; ++i)
    {
        p_sys->inBuf[i] = (float *)malloc(AMB_BLOCK_TIME_LEN * sizeof(float));
        if (p_sys->inBuf[i] == NULL)
            MEMORY_ERROR()
    }

    p_sys->outBuf = (float**)calloc(p_sys->i_outputNb, sizeof(float*));
    if (p_sys->outBuf == NULL)
        MEMORY_ERROR()

    for (unsigned i = 0; i < p_sys->i_outputNb; ++i)
    {
        p_sys->outBuf[i] = (float *)malloc(AMB_BLOCK_TIME_LEN * sizeof(float));
        if (p_sys->outBuf[i] == NULL)
            MEMORY_ERROR()
    }

    return VLC_SUCCESS;
}


static void freeBuffers(filter_sys_t *p_sys)
{
    if (p_sys->inBuf != NULL)
        for (unsigned i = 0; i < p_sys->i_inputNb; ++i)
            free(p_sys->inBuf[i]);

    if (p_sys->outBuf != NULL)
        for (unsigned i = 0; i < p_sys->i_outputNb; ++i)
            free(p_sys->outBuf[i]);
}
