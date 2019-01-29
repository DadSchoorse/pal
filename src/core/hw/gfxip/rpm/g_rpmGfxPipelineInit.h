/*
 ***********************************************************************************************************************
 *
 *  Copyright (c) 2019 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 **********************************************************************************************************************/

#pragma once

#include "pal.h"

namespace Pal
{

class GraphicsPipeline;
class GfxDevice;

// RPM Graphics States. Used to index into RsrcProcMgr::m_pGraphicsStates array
enum RpmGfxPipeline : uint32
{
    Copy2xMsaaDepth          = 0,
    Copy2xMsaaDepthStencil   = 1,
    Copy2xMsaaStencil        = 2,
    Copy4xMsaaDepth          = 3,
    Copy4xMsaaDepthStencil   = 4,
    Copy4xMsaaStencil        = 5,
    Copy8xMsaaDepth          = 6,
    Copy8xMsaaDepthStencil   = 7,
    Copy8xMsaaStencil        = 8,
    DccDecompress            = 9,
    DepthExpand              = 10,
    DepthResummarize         = 11,
    DepthSlowDraw            = 12,
    FastClearElim            = 13,
    FmaskDecompress          = 14,
    Copy_32ABGR              = 15,
    Copy_32GR                = 16,
    Copy_32R                 = 17,
    Copy_FP16                = 18,
    Copy_SINT16              = 19,
    Copy_SNORM16             = 20,
    Copy_UINT16              = 21,
    Copy_UNORM16             = 22,
    ResolveFixedFunc_32ABGR  = 23,
    ResolveFixedFunc_32GR    = 24,
    ResolveFixedFunc_32R     = 25,
    ResolveFixedFunc_FP16    = 26,
    ResolveFixedFunc_SINT16  = 27,
    ResolveFixedFunc_SNORM16 = 28,
    ResolveFixedFunc_UINT16  = 29,
    ResolveFixedFunc_UNORM16 = 30,
    ScaledCopy2d_32ABGR      = 31,
    ScaledCopy2d_32GR        = 32,
    ScaledCopy2d_32R         = 33,
    ScaledCopy2d_FP16        = 34,
    ScaledCopy2d_SINT16      = 35,
    ScaledCopy2d_SNORM16     = 36,
    ScaledCopy2d_UINT16      = 37,
    ScaledCopy2d_UNORM16     = 38,
    ScaledCopy3d_32ABGR      = 39,
    ScaledCopy3d_32GR        = 40,
    ScaledCopy3d_32R         = 41,
    ScaledCopy3d_FP16        = 42,
    ScaledCopy3d_SINT16      = 43,
    ScaledCopy3d_SNORM16     = 44,
    ScaledCopy3d_UINT16      = 45,
    ScaledCopy3d_UNORM16     = 46,
    SlowColorClear0_32ABGR   = 47,
    SlowColorClear0_32GR     = 48,
    SlowColorClear0_32R      = 49,
    SlowColorClear0_FP16     = 50,
    SlowColorClear0_SINT16   = 51,
    SlowColorClear0_SNORM16  = 52,
    SlowColorClear0_UINT16   = 53,
    SlowColorClear0_UNORM16  = 54,
    SlowColorClear1_32ABGR   = 55,
    SlowColorClear1_32GR     = 56,
    SlowColorClear1_32R      = 57,
    SlowColorClear1_FP16     = 58,
    SlowColorClear1_SINT16   = 59,
    SlowColorClear1_SNORM16  = 60,
    SlowColorClear1_UINT16   = 61,
    SlowColorClear1_UNORM16  = 62,
    SlowColorClear2_32ABGR   = 63,
    SlowColorClear2_32GR     = 64,
    SlowColorClear2_32R      = 65,
    SlowColorClear2_FP16     = 66,
    SlowColorClear2_SINT16   = 67,
    SlowColorClear2_SNORM16  = 68,
    SlowColorClear2_UINT16   = 69,
    SlowColorClear2_UNORM16  = 70,
    SlowColorClear3_32ABGR   = 71,
    SlowColorClear3_32GR     = 72,
    SlowColorClear3_32R      = 73,
    SlowColorClear3_FP16     = 74,
    SlowColorClear3_SINT16   = 75,
    SlowColorClear3_SNORM16  = 76,
    SlowColorClear3_UINT16   = 77,
    SlowColorClear3_UNORM16  = 78,
    SlowColorClear4_32ABGR   = 79,
    SlowColorClear4_32GR     = 80,
    SlowColorClear4_32R      = 81,
    SlowColorClear4_FP16     = 82,
    SlowColorClear4_SINT16   = 83,
    SlowColorClear4_SNORM16  = 84,
    SlowColorClear4_UINT16   = 85,
    SlowColorClear4_UNORM16  = 86,
    SlowColorClear5_32ABGR   = 87,
    SlowColorClear5_32GR     = 88,
    SlowColorClear5_32R      = 89,
    SlowColorClear5_FP16     = 90,
    SlowColorClear5_SINT16   = 91,
    SlowColorClear5_SNORM16  = 92,
    SlowColorClear5_UINT16   = 93,
    SlowColorClear5_UNORM16  = 94,
    SlowColorClear6_32ABGR   = 95,
    SlowColorClear6_32GR     = 96,
    SlowColorClear6_32R      = 97,
    SlowColorClear6_FP16     = 98,
    SlowColorClear6_SINT16   = 99,
    SlowColorClear6_SNORM16  = 100,
    SlowColorClear6_UINT16   = 101,
    SlowColorClear6_UNORM16  = 102,
    SlowColorClear7_32ABGR   = 103,
    SlowColorClear7_32GR     = 104,
    SlowColorClear7_32R      = 105,
    SlowColorClear7_FP16     = 106,
    SlowColorClear7_SINT16   = 107,
    SlowColorClear7_SNORM16  = 108,
    SlowColorClear7_UINT16   = 109,
    SlowColorClear7_UNORM16  = 110,
    ResolveDepth             = 111,
    ResolveDepthCopy         = 112,
    ResolveStencil           = 113,
    ResolveStencilCopy       = 114,
    RpmGfxPipelineCount
};

// We only care about 8 of the 10 export formats. We should never expect ZERO, and although 32_AR used by Pal, it
// appears in GetPsExportFormat we can't actually get to that export format with the image formats that Pal supports.
constexpr uint32 NumExportFormats = 8;

// Starting user-data entry which the RPM vertex shaders use for exporting a constant depth.
constexpr uint32 RpmVsDepthOut = 0;

// Starting user-data entry which the RPM vertex shaders use for exporting slice id.
constexpr uint32 RpmVsSliceOffset = RpmVsDepthOut + 1;

// Starting user-data entry which the RPM pixel shaders can use.
constexpr uint32 RpmPsFirstUserData = (RpmVsDepthOut + 1);

// Starting user-data entry which the RPM clear pixel shaders can use.
constexpr uint32 RpmPsClearFirstUserData = (RpmVsSliceOffset + 1);

// We support separate depth and stencil resolves
constexpr uint32 NumDepthStencilResolveTypes = 2;

Result CreateRpmGraphicsPipelines(GfxDevice* pDevice, GraphicsPipeline** pPipelineMem);

} // Pal
