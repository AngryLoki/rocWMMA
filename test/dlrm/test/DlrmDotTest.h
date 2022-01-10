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

#ifndef DLRM_DOT_TEST_H
#define DLRM_DOT_TEST_H

#include "DlrmKernelBase.h"
#include "DlrmTestParams.h"
#include <gtest/gtest.h>

struct DlrmDotTest
    : public ::testing::TestWithParam<std::tuple<typename DlrmTestParams::KernelT,
                                                 typename DlrmTestParams::ThreadBlockT,
                                                 typename DlrmTestParams::ProblemSizeT,
                                                 typename DlrmTestParams::FwdDataSizeT,
                                                 typename DlrmTestParams::BwdDataSizeT,
                                                 typename DlrmTestParams::PassDirectionT>>
{
    using Base = ::testing::TestWithParam<std::tuple<typename DlrmTestParams::KernelT,
                                                     typename DlrmTestParams::ThreadBlockT,
                                                     typename DlrmTestParams::ProblemSizeT,
                                                     typename DlrmTestParams::FwdDataSizeT,
                                                     typename DlrmTestParams::BwdDataSizeT,
                                                     typename DlrmTestParams::PassDirectionT>>;

    void SetUp() override
    {
        // Construct ProblemParams from
        // incoming gtest parameterization
        auto param       = Base::GetParam();
        auto kernel      = std::get<0>(param);
        auto threadBlock = std::get<1>(param);
        auto problemSize = std::get<2>(param);
        auto fwdDataSize = std::get<3>(param);
        auto bwdDataSize = std::get<4>(param);
        auto isBwd       = std::get<5>(param);

        ProblemParams params = {threadBlock, problemSize, fwdDataSize, bwdDataSize, isBwd};

        // Walk through kernel workflow
        kernel->setup(params);

        // run warm-up iteration based on static bool (runkernel)
    }

    virtual void RunKernel()
    {
        // Construct ProblemParams from
        // incoming gtest parameterization
        auto param  = Base::GetParam();
        auto kernel = std::get<0>(param);
        kernel->exec();
        kernel->validateResults();
        kernel->reportResults();
    }

    virtual void Warmup()
    {
        auto param  = Base::GetParam();
        auto kernel = std::get<0>(param);
        kernel->exec();
    }

    void TearDown() override
    {
        // Construct ProblemParams from
        // incoming gtest parameterization
        auto param  = Base::GetParam();
        auto kernel = std::get<0>(param);
        kernel->tearDown();
    }
};
// pass enum template values through Base::<name>

#endif // DLRM_DOT_TEST_H