/*******************************************************************************
 *
 * MIT License
 *
 * Copyright 2021 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *******************************************************************************/

#ifndef WMMA_DEVICE_COL_LAYOUT_H
#define WMMA_DEVICE_COL_LAYOUT_H

#include "IOTraits.h"
#include "Layout.h"
#include "MappingUtil.h"

template <uint32_t BlockM, uint32_t BlockN, typename DataT, typename LayoutP>
__global__ void ColLayout(
    uint32_t m, uint32_t n, DataT const* in, DataT* out, uint32_t ld, DataT param1, DataT param2)
{
    enum : uint32_t
    {
        MaxVectorWidth    = VecWidthTraits<BlockM, BlockN, DataT>::MaxVectorWidth,
        ElementsPerThread = std::is_same<LayoutP, row_major>::value ? MaxVectorWidth : 1
    };

    using IOTraits = amdgcn_io_traits<BlockM, BlockN, DataT, ElementsPerThread>;
    using LayoutT  = Layout::Col<BlockM, BlockN, DataT, LayoutP, ElementsPerThread>;
    using Mapping  = MappingUtil<BlockM, BlockN, DataT, LayoutP>;

    auto baseOffset  = LayoutT::baseDataOffset(ld);
    auto iocount     = IOTraits::IOCount;
    auto matrixCoord = Mapping::matrixCoord();

    enum : uint32_t
    {
        MajorIndex = std::is_same<LayoutP, row_major>::value ? 0 : 1,
        MinorIndex = std::is_same<LayoutP, row_major>::value ? 1 : 0
    };

    for(uint32_t i = 0; i < iocount; ++i)
    {
        for(int j = 0; j < ElementsPerThread; j++)
        {
            auto index
                = (std::get<MajorIndex>(matrixCoord) * ld + std::get<MinorIndex>(matrixCoord))
                  + baseOffset + j;
            out[index] = in[index];
        }
        baseOffset += LayoutT::dataOffsetIncrement(i, ld);
    }
}
#endif // WMMA_DEVICE_COL_LAYOUT_H