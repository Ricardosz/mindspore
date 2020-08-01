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
#include "src/runtime/kernel/arm/fp32/crop.h"
#include "schema/model_generated.h"
#include "src/kernel_registry.h"
#include "src/runtime/kernel/arm/opclib/fp32/crop.h"
#include "include/errorcode.h"
#include "src/runtime/runtime_api.h"

using mindspore::lite::KernelRegistrar;
using mindspore::lite::RET_ERROR;
using mindspore::lite::RET_FORMAT_ERR;
using mindspore::lite::RET_NULL_PTR;
using mindspore::lite::RET_OK;
using mindspore::schema::PrimitiveType_Crop;

namespace mindspore::kernel {
namespace {
int CropLaunch(int thread_id, LiteParallelGroupEnv *penv, void *cdata) {
  if (cdata == nullptr) {
    MS_LOG(ERROR) << "Input cdata is nullptr!";
    return RET_NULL_PTR;
  }
  auto kernel = reinterpret_cast<CropCPUKernel *>(cdata);
  return kernel->CropParallelRun(thread_id);
}
}  // namespace

int CropCPUKernel::Init() {
  schema::Format input0_format = inputs_[0]->GetFormat();
  if (input0_format != schema::Format_NCHW && input0_format != schema::Format_NHWC) {
    MS_LOG(ERROR) << "Unsupport format " << input0_format;
    return RET_FORMAT_ERR;
  }
  outputs_[0]->SetFormat(input0_format);
  return RET_OK;
}

int CropCPUKernel::CropParallelRun(int thread_id) {
  auto input = inputs_[0];
  auto output = outputs_[0];
  float *input_data = reinterpret_cast<float *>(input->Data());
  float *output_data = reinterpret_cast<float *>(output->Data());
  Crop4D(input_data, output_data, input->shape().data(), output->shape().data(),
         reinterpret_cast<CropParameter *>(opParameter));
  return RET_OK;
}

int CropCPUKernel::Run() {
  auto input = inputs_[0];
  auto output = outputs_[0];
  auto param = reinterpret_cast<CropParameter *>(opParameter);
  if (output->shape()[1] < param->op_parameter_.thread_num_) {
    float *input_data = reinterpret_cast<float *>(input->Data());
    float *output_data = reinterpret_cast<float *>(output->Data());
    Crop4DNoParallel(input_data, output_data, input->shape().data(), output->shape().data(), param);
    return RET_OK;
  }

  int ret = LiteBackendParallelLaunch(CropLaunch, this, param->op_parameter_.thread_num_);
  if (ret != RET_OK) {
    MS_LOG(ERROR) << "Crop launch fail!ret: " << ret;
    return RET_ERROR;
  }
  return RET_OK;
}

kernel::LiteKernel *CpuCropFp32KernelCreator(const std::vector<lite::tensor::Tensor *> &inputs,
                                             const std::vector<lite::tensor::Tensor *> &outputs,
                                             OpParameter *op_parameter, const lite::Context *ctx,
                                             const kernel::KernelKey &desc) {
  if (op_parameter == nullptr) {
    MS_LOG(ERROR) << "Input op_parameter is nullptr!";
    return nullptr;
  }
  if (ctx == nullptr) {
    MS_LOG(ERROR) << "Input context is nullptr!";
    return nullptr;
  }
  MS_ASSERT(desc.type == schema::PrimitiveType_Crop);
  op_parameter->thread_num_ = ctx->threadNum;
  auto *kernel = new (std::nothrow) CropCPUKernel(op_parameter, inputs, outputs);
  if (kernel == nullptr) {
    MS_LOG(ERROR) << "new CropCPUKernel fail!";
    return nullptr;
  }

  auto ret = kernel->Init();
  if (ret != RET_OK) {
    delete kernel;
    MS_LOG(ERROR) << "Init kernel failed, name: " << op_parameter->name_ << ", type: "
                  << schema::EnumNamePrimitiveType(static_cast<schema::PrimitiveType>(op_parameter->type_));
    return nullptr;
  }
  return kernel;
}

REG_KERNEL(kCPU, kNumberTypeFloat32, PrimitiveType_Crop, CpuCropFp32KernelCreator)
}  // namespace mindspore::kernel