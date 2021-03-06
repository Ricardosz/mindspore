/**
 * Copyright 2020 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_NNACL_INT8_DECONV_H_
#define MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_NNACL_INT8_DECONV_H_

#include <string.h>
#include "src/runtime/kernel/arm/nnacl/pack.h"
#include "src/runtime/kernel/arm/nnacl/op_base.h"
#include "src/runtime/kernel/arm/nnacl/errorcode.h"
#include "src/runtime/kernel/arm/nnacl/conv_parameter.h"
#include "src/runtime/kernel/arm/nnacl/common_func.h"
#include "src/runtime/kernel/arm/nnacl/int8/matmul.h"

int DeConvInt8(const int8_t *input, const int8_t *weight, int32_t *output, size_t row8, size_t col8, size_t deep,
               ConvParameter *conv_param);

int DeConvPostInt8(const int32_t *src, const int32_t *bias, int32_t *tmp, int8_t *out, int output_channel,
                   ConvParameter *conv_param);

#endif  // MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_NNACL_INT8_DECONV_H_

