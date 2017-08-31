// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2017 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include "memorydata.h"

namespace ncnn {

DEFINE_LAYER_CREATOR(MemoryData)

MemoryData::MemoryData()
{
    one_blob_only = true;
    support_inplace = true;
}

#if NCNN_STDIO
#if NCNN_STRING
int MemoryData::load_param(FILE* paramfp)
{
    int nscan = fscanf(paramfp, "%d %d %d",
                       &c, &h, &w);
    if (nscan != 3)
    {
        fprintf(stderr, "MemoryData load_param failed %d\n", nscan);
        return -1;
    }

    return 0;
}
#endif // NCNN_STRING
int MemoryData::load_param_bin(FILE* paramfp)
{
    fread(&c, sizeof(int), 1, paramfp);

    fread(&h, sizeof(int), 1, paramfp);

    fread(&w, sizeof(int), 1, paramfp);

    return 0;
}

int MemoryData::load_model(FILE* binfp)
{
    int nread;

    if (c != 0)
    {
        data.create(w, h, c);
    }
    else if (h != 0)
    {
        data.create(w, h);
    }
    else if (w != 0)
    {
        data.create(w);
    }
    if (data.empty())
        return -100;
    nread = fread(data, data.total() * sizeof(float), 1, binfp);
    if (nread != 1)
    {
        fprintf(stderr, "MemoryData read data failed %d\n", nread);
        return -1;
    }

    return 0;
}
#endif // NCNN_STDIO

int MemoryData::load_param(const unsigned char*& mem)
{
    c = *(int*)(mem);
    mem += 4;

    h = *(int*)(mem);
    mem += 4;

    w = *(int*)(mem);
    mem += 4;

    return 0;
}

int MemoryData::load_model(const unsigned char*& mem)
{
    if (c != 0)
    {
        data = Mat(w, h, c, (float*)mem);
    }
    else if (h != 0)
    {
        data = Mat(w, h, (float*)mem);
    }
    else if (w != 0)
    {
        data = Mat(w, (float*)mem);
    }
    mem += data.total() * sizeof(float);

    return 0;
}

int MemoryData::forward(const Mat& /*bottom_blob*/, Mat& top_blob) const
{
    top_blob = data.clone();
    if (top_blob.empty())
        return -100;

    return 0;
}

int MemoryData::forward_inplace(Mat& bottom_top_blob) const
{
    bottom_top_blob = data.clone();
    if (bottom_top_blob.empty())
        return -100;

    return 0;
}

} // namespace ncnn
