// di_timing.cpp - Video timing constant data structures.
//
// Copyright (c) 2023 Curtis Whitley
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 

#include "di_timing.h"

const OtfVideoParams otf_video_params_800x600 = {
    600, // m_active_lines
    1,   // m_vfp_lines
    4,   // m_vs_lines
    23,  // m_vbp_lines
    40,  // m_hfp_pixels
    128, // m_hs_pixels
    800, // m_active_pixels
    88,  // m_hbp_pixels
    600/2, // m_active_buffers_written     (ACT_LINES/NUM_LINES_PER_BUFFER)
    4/2, // m_vs_buffers_written           (VS_LINES/NUM_LINES_PER_BUFFER)
    40000000, // m_dma_clock_freq
    4*2, // m_dma_active_lines             (NUM_ACTIVE_BUFFERS*NUM_LINES_PER_BUFFER)
    600+1+4+23, // m_dma_total_lines       (ACT_LINES+VFP_LINES+VS_LINES+VBP_LINES)
    600/2+1+4/2+23, // m_dma_total_descr   (ACT_BUFFERS_WRITTEN+VFP_LINES+VS_BUFFERS_WRITTEN+VBP_LINES)
    (1<<VGA_HSYNC_BIT), // m_hs_on
    (0<<VGA_HSYNC_BIT), // m_hs_off
    (1<<VGA_VSYNC_BIT), // m_vs_on
    (0<<VGA_VSYNC_BIT), // m_vs_off
    (1<<VGA_HSYNC_BIT)|(1<<VGA_VSYNC_BIT), // m_syncs_on
    (0<<VGA_HSYNC_BIT)|(0<<VGA_VSYNC_BIT), // m_syncs_off
    0x00000000 // m_syncs_off_x4
};

const OtfVideoParams otf_video_params_640x480 = {
    480, // m_active_lines
    10,   // m_vfp_lines
    2,   // m_vs_lines
    33,  // m_vbp_lines
    16,  // m_hfp_pixels
    96, // m_hs_pixels
    640, // m_active_pixels
    48,  // m_hbp_pixels
    480/2, // m_active_buffers_written     (ACT_LINES/NUM_LINES_PER_BUFFER)
    4/2, // m_vs_buffers_written           (VS_LINES/NUM_LINES_PER_BUFFER)
    25175000, // m_dma_clock_freq
    4*2, // m_dma_active_lines             (NUM_ACTIVE_BUFFERS*NUM_LINES_PER_BUFFER)
    480+10+2+33, // m_dma_total_lines       (ACT_LINES+VFP_LINES+VS_LINES+VBP_LINES)
    480/2+10+4/2+33, // m_dma_total_descr    (ACT_BUFFERS_WRITTEN+VFP_LINES+VS_BUFFERS_WRITTEN+VBP_LINES)
    (0<<VGA_HSYNC_BIT), // m_hs_on
    (1<<VGA_HSYNC_BIT), // m_hs_off
    (0<<VGA_VSYNC_BIT), // m_vs_on
    (1<<VGA_VSYNC_BIT), // m_vs_off
    (0<<VGA_HSYNC_BIT)|(0<<VGA_VSYNC_BIT), // m_syncs_on
    (1<<VGA_HSYNC_BIT)|(1<<VGA_VSYNC_BIT), // m_syncs_off
    0xC0C0C0C0 // m_syncs_off_x4
};

/*const OtfVideoParams otf_video_params_640x512 = {
    512, // m_active_lines
    ,   // m_vfp_lines
    ,   // m_vs_lines
    ,  // m_vbp_lines
    ,  // m_hfp_pixels
    , // m_hs_pixels
    640, // m_active_pixels
    ,  // m_hbp_pixels
    512/2, // m_active_buffers_written     (ACT_LINES/NUM_LINES_PER_BUFFER)
    4/2, // m_vs_buffers_written           (VS_LINES/NUM_LINES_PER_BUFFER)
    25175000, // m_dma_clock_freq
    4*2, // m_dma_active_lines             (NUM_ACTIVE_BUFFERS*NUM_LINES_PER_BUFFER)
    512+++, // m_dma_total_lines       (ACT_LINES+VFP_LINES+VS_LINES+VBP_LINES)
    512/2++/2+, // m_dma_total_descr    (ACT_BUFFERS_WRITTEN+VFP_LINES+VS_BUFFERS_WRITTEN+VBP_LINES)
    , // m_hs_on
    , // m_hs_off
    , // m_vs_on
    , // m_vs_off,
    , // m_syncs_on
    , // m_syncs_off
    , // m_syncs_off_x4
};*/

const OtfVideoParams* otf_video_params = &otf_video_params_800x600;
