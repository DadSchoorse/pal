/*
 ***********************************************************************************************************************
 *
 *  Copyright (c) 2015-2019 Advanced Micro Devices, Inc. All Rights Reserved.
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

#include "core/hw/gfxip/gfx9/gfx9Chip.h"
#include "core/gpuMemory.h"

namespace Pal
{
namespace Gfx9
{

class CmdStream;
class Device;
class ShaderRing;

// Enumerates the types of Shader Rings available.
enum class ShaderRingType : uint32
{
    ComputeScratch = 0,                // Scratch Ring for compute pipelines
    SamplePos,                         // Sample position buffer
    GfxScratch,                        // Scratch Ring for graphics pipelines
    GsVs,                              // Ring for passing vertex data between the GS & VS stage
    TfBuffer,                          // Tess-Factor Buffer
    OffChipLds,                        // Off-Chip Tessellation LDS buffers
    NumUniversal,                      // Number of Rings in a RingSet associated with a universal Queue
    NumCompute = (SamplePos + 1),      // Number of Rings in a RingSet associated with a compute Queue
};

// Enumerates the SRD's used in the per-RingSet internal table.
enum class ShaderRingSrd : uint32
{
    ScratchGraphics = 0,               // Graphics Scratch Ring
    ScratchCompute,                    // Compute Scratch Ring
    Reserved1,                         // Reserved for future use.
    Reserved2,                         // Reserved for future use.
    GsVsWrite0,                        // GS/VS Ring Write Access (Offset 0)
    GsVsWrite1,                        // GS/VS Ring Write Access (Offset 1)
    GsVsWrite2,                        // GS/VS Ring Write Access (Offset 2)
    GsVsWrite3,                        // GS/VS Ring Write Access (Offset 3)
    GsVsRead,                          // GS/VS Ring Read Access
    TessFactorBuffer,                  // Tessellation Factor Buffer
    OffChipLdsBuffer,                  // Off-chip Tessellation LDS buffer
    Reserved3,                         // Reserved for future use.
    SamplePosBuffer,                   // Sample position buffer
    Reserved4,                         // Reserved for future use.
    Reserved5,                         // Reserved for future use.
    Reserved6,                         // Reserved for future use.
    NumUniversal,                      // Number of Ring SRD's in a RingSet associated with a universal Queue
    NumCompute = (SamplePosBuffer + 1), // Number of Ring SRD's in a RingSet associated with a compute Queue
};

// Contains the largest required item-size for each Shader Ring. Note that there is one item size tracker for each ring
// in a Universal Queue's RingSet. This works because the Compute RingSet is a subset of the Universal RingSet.
struct ShaderRingItemSizes
{
    size_t itemSize[static_cast<size_t>(ShaderRingType::NumUniversal)];
    static_assert(ShaderRingType::NumUniversal >= ShaderRingType::NumCompute,
                  "The compute ring set must be a subset of the universal ring set.");
};

// =====================================================================================================================
// A ShaderRingSet object contains all of the shader Rings used by command buffers which run on a particular Queue.
// Additionally, each Ring Set also manages the PM4 image of commands which write the ring state to hardware.
class ShaderRingSet
{
public:
    virtual ~ShaderRingSet();

    virtual Result Init();
    virtual Result Validate(const ShaderRingItemSizes&  ringSizes,
                            const SamplePatternPalette& samplePatternPalette);

    // Writes the per-Ring-Set register state into the specified command stream.
    virtual uint32* WriteCommands(CmdStream* pCmdStream, uint32* pCmdSpace) const = 0;

    // Write affected registers not in the Rlc save/restore list.
    virtual uint32* WriteNonRlcRestoredRegs(CmdStream* pCmdStream, uint32* pCmdSpace) const = 0;

    size_t NumRings() const { return m_numRings; }

    size_t SrdTableSize() const { return (sizeof(BufferSrd) * m_numSrds); }
    size_t TotalMemSize() const { return SrdTableSize(); }

protected:
    ShaderRingSet(Device* pDevice, size_t numRings, size_t numSrds);

    Device*const      m_pDevice;
    const size_t      m_numRings;       // Number of shader rings contained in the set
    const size_t      m_numSrds;        // Number of SRD's in this set's table
    ShaderRing**      m_ppRings;
    BufferSrd*        m_pSrdTable;
    const GfxIpLevel  m_gfxLevel;
    BoundGpuMemory    m_srdTableMem;

private:
    PAL_DISALLOW_DEFAULT_CTOR(ShaderRingSet);
    PAL_DISALLOW_COPY_AND_ASSIGN(ShaderRingSet);
};

// Represents an "image" of the PM4 commands necessary to write a UniversalRingSet to hardware. The required register
// writes are grouped into sets based on sequential register addresses, so that we can minimize the amount of PM4 space
// needed by setting several reg's in each packet.
struct UniversalRingSetPm4Img
{
    PM4ME_NON_SAMPLE_EVENT_WRITE  vsPartialFlush;
    PM4ME_NON_SAMPLE_EVENT_WRITE  vgtFlush;

    PM4_PFP_SET_UCONFIG_REG       hdrVgtTfRingSize;
    regVGT_TF_RING_SIZE           vgtTfRingSize;

    PM4_PFP_SET_UCONFIG_REG       hdrVgtHsOffchipParam;
    regVGT_HS_OFFCHIP_PARAM       vgtHsOffchipParam;

    PM4_PFP_SET_UCONFIG_REG       hdrVgtGsVsRingSize;
    regVGT_GSVS_RING_SIZE         vgtGsVsRingSize;

    PM4_ME_SET_SH_REG             hdrLsUserData;
    uint32                        lsUserDataLo;

    PM4_ME_SET_SH_REG             hdrEsUserData;
    uint32                        esUserDataLo;

    PM4_ME_SET_SH_REG             hdrVsUserData;
    regSPI_SHADER_USER_DATA_VS_0  vsUserDataLo;

    PM4_ME_SET_SH_REG             hdrPsUserData;
    regSPI_SHADER_USER_DATA_PS_0  psUserDataLo;

    PM4_ME_SET_SH_REG             hdrComputeUserData;
    regCOMPUTE_USER_DATA_0        computeUserDataLo;

    PM4_ME_SET_CONTEXT_REG        hdrGfxScratchRingSize;
    regSPI_TMPRING_SIZE           gfxScratchRingSize;

    PM4_ME_SET_CONTEXT_REG        hdrComputeScratchRingSize;
    regCOMPUTE_TMPRING_SIZE       computeScratchRingSize;

    union
    {
        struct
        {
            PM4_PFP_SET_UCONFIG_REG       hdrVgtTfMemoryBase;
            regVGT_TF_MEMORY_BASE         vgtTfMemoryBaseLo;
            regVGT_TF_MEMORY_BASE_HI      vgtTfMemoryBaseHi;
        } gfx9;

        struct
        {
            PM4_PFP_SET_UCONFIG_REG        hdrVgtTfMemoryBaseLo;
            regVGT_TF_MEMORY_BASE_UMD      vgtTfMemoryBaseLo;
            PM4_PFP_SET_UCONFIG_REG        hdrVgtTfMemoryBaseHi;
            regVGT_TF_MEMORY_BASE_HI_UMD   vgtTfMemoryBaseHi;
        } gfx10;
    } tfMemBase;

    size_t spaceNeeded;
};

// =====================================================================================================================
// Implements a ShaderRingSet for a Universal Queue.
class UniversalRingSet : public ShaderRingSet
{
public:
    explicit UniversalRingSet(Device* pDevice);
    virtual ~UniversalRingSet() {}

    virtual Result Init() override;
    virtual Result Validate(const ShaderRingItemSizes&  ringSizes,
                            const SamplePatternPalette& samplePatternPalette) override;

    virtual uint32* WriteCommands(CmdStream* pCmdStream, uint32* pCmdSpace) const override;
    // This should be never called for Gfx9.
    virtual uint32* WriteNonRlcRestoredRegs(CmdStream* pCmdStream, uint32* pCmdSpace) const override
        { PAL_NEVER_CALLED(); return pCmdSpace; }

private:
    void BuildPm4Headers();

    // Command images used to write this Ring-Set's state to hardware.
    UniversalRingSetPm4Img m_pm4Commands;

    PAL_DISALLOW_DEFAULT_CTOR(UniversalRingSet);
    PAL_DISALLOW_COPY_AND_ASSIGN(UniversalRingSet);
};

// Represents an "image" of the PM4 commands necessary to write a ComputeRingSet to hardware. The required register
// writes are grouped into sets based on sequential register addresses, so that we can minimize the amount of PM4 space
// needed by setting several reg's in each packet.
struct ComputeRingSetPm4Img
{
    PM4_ME_SET_SH_REG       hdrComputeUserData;
    regCOMPUTE_USER_DATA_0  computeUserDataLo;

    PM4_ME_SET_CONTEXT_REG  hdrComputeScratchRingSize;
    regCOMPUTE_TMPRING_SIZE computeScratchRingSize;
};

// =====================================================================================================================
// Implements a ShaderRingSet for a Compute-only Queue.
class ComputeRingSet : public ShaderRingSet
{
public:
    explicit ComputeRingSet(Device* pDevice);
    virtual ~ComputeRingSet() {}

    virtual Result Init() override;
    virtual Result Validate(const ShaderRingItemSizes&  ringSizes,
                            const SamplePatternPalette& samplePatternPalette) override;

    virtual uint32* WriteCommands(CmdStream* pCmdStream, uint32* pCmdSpace) const override;

    // This should be never called for Gfx9.
    virtual uint32* WriteNonRlcRestoredRegs(CmdStream* pCmdStream, uint32* pCmdSpace) const override
        { PAL_NEVER_CALLED(); return pCmdSpace; }

private:
    void BuildPm4Headers();

    ComputeRingSetPm4Img m_pm4Commands; // Image of the PM4 commands used to write this Ring-Set's state to hardware.

    PAL_DISALLOW_DEFAULT_CTOR(ComputeRingSet);
    PAL_DISALLOW_COPY_AND_ASSIGN(ComputeRingSet);
};

} // Gfx9
} // Pal
