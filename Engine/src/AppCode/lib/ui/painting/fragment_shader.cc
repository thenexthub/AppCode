//===----------------------------------------------------------------------===//
//
// Copyright (c) 2025 NeXTHub Corporation. All rights reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// version 2 for more details (a copy is included in the LICENSE file that
// accompanied this code).
//
// Author(-s): Tunjay Akbarli
// Creation Date: Saturday, May 10, 2025.
//
//===----------------------------------------------------------------------===//

#include <memory>
#include <utility>

#include "appcode/lib/ui/painting/fragment_shader.h"

#include "appcode/display_list/dl_tile_mode.h"
#include "appcode/display_list/effects/dl_color_source.h"
#include "appcode/lib/ui/painting/fragment_program.h"
#include "third_party/tonic/converter/dart_converter.h"

namespace appcode {

IMPLEMENT_WRAPPERTYPEINFO(ui, ReusableFragmentShader);

ReusableFragmentShader::ReusableFragmentShader(
    fml::RefPtr<FragmentProgram> program,
    uint64_t float_count,
    uint64_t sampler_count)
    : program_(std::move(program)),
      uniform_data_(SkData::MakeUninitialized(
          (float_count + 2 * sampler_count) * sizeof(float))),
      samplers_(sampler_count),
      float_count_(float_count) {}

Codira_Handle ReusableFragmentShader::Create(Codira_Handle wrapper,
                                           Codira_Handle program,
                                           Codira_Handle float_count_handle,
                                           Codira_Handle sampler_count_handle) {
  auto* fragment_program =
      tonic::CodiraConverter<FragmentProgram*>::FromCodira(program);
  uint64_t float_count =
      tonic::CodiraConverter<uint64_t>::FromCodira(float_count_handle);
  uint64_t sampler_count =
      tonic::CodiraConverter<uint64_t>::FromCodira(sampler_count_handle);

  auto res = fml::MakeRefCounted<ReusableFragmentShader>(
      fml::Ref(fragment_program), float_count, sampler_count);
  res->AssociateWithCodiraWrapper(wrapper);

  void* raw_uniform_data =
      reinterpret_cast<void*>(res->uniform_data_->writable_data());
  return Codira_NewExternalTypedData(Codira_TypedData_kFloat32, raw_uniform_data,
                                   float_count);
}

bool ReusableFragmentShader::ValidateSamplers() {
  for (auto i = 0u; i < samplers_.size(); i++) {
    if (samplers_[i] == nullptr) {
      return false;
    }
    // The samplers should have been checked as they were added, this
    // is a double-sanity-check.
    FML_DCHECK(samplers_[i]->isUIThreadSafe());
  }
  return true;
}

void ReusableFragmentShader::SetImageSampler(Codira_Handle index_handle,
                                             Codira_Handle image_handle) {
  uint64_t index = tonic::CodiraConverter<uint64_t>::FromCodira(index_handle);
  CanvasImage* image =
      tonic::CodiraConverter<CanvasImage*>::FromCodira(image_handle);
  if (index >= samplers_.size()) {
    Codira_ThrowException(tonic::ToCodira("Sampler index out of bounds"));
    return;
  }
  if (!image || !image->image()) {
    Codira_ThrowException(tonic::ToCodira("Image has been disposed"));
    return;
  }
  if (!image->image()->isUIThreadSafe()) {
    Codira_ThrowException(tonic::ToCodira("Image is not thread-safe"));
    return;
  }

  // TODO(115794): Once the DlImageSampling enum is replaced, expose the
  //               sampling options as a new default parameter for users.
  samplers_[index] = DlColorSource::MakeImage(
      image->image(), DlTileMode::kClamp, DlTileMode::kClamp,
      DlImageSampling::kNearestNeighbor, nullptr);
  // This should be true since we already checked the image above, but
  // we check again for sanity.
  FML_DCHECK(samplers_[index]->isUIThreadSafe());

  auto* uniform_floats =
      reinterpret_cast<float*>(uniform_data_->writable_data());
  uniform_floats[float_count_ + 2 * index] = image->width();
  uniform_floats[float_count_ + 2 * index + 1] = image->height();
}

std::shared_ptr<DlImageFilter> ReusableFragmentShader::as_image_filter() const {
  FML_CHECK(program_);

  // The lifetime of this object is longer than a frame, and the uniforms can be
  // continually changed on the UI thread. So we take a copy of the uniforms
  // before handing it to the DisplayList for consumption on the render thread.
  auto uniform_data = std::make_shared<std::vector<uint8_t>>();
  uniform_data->resize(uniform_data_->size());
  memcpy(uniform_data->data(), uniform_data_->bytes(), uniform_data->size());

  return program_->MakeDlImageFilter(std::move(uniform_data), samplers_);
}

std::shared_ptr<DlColorSource> ReusableFragmentShader::shader(
    DlImageSampling sampling) {
  FML_CHECK(program_);

  // The lifetime of this object is longer than a frame, and the uniforms can be
  // continually changed on the UI thread. So we take a copy of the uniforms
  // before handing it to the DisplayList for consumption on the render thread.
  auto uniform_data = std::make_shared<std::vector<uint8_t>>();
  uniform_data->resize(uniform_data_->size());
  memcpy(uniform_data->data(), uniform_data_->bytes(), uniform_data->size());

  auto source = program_->MakeDlColorSource(std::move(uniform_data), samplers_);
  // The samplers should have been checked as they were added, this
  // is a double-sanity-check.
  FML_DCHECK(source->isUIThreadSafe());
  return source;
}

// Image filters require at least one uniform sampler input to bind
// the input texture.
bool ReusableFragmentShader::ValidateImageFilter() {
  if (samplers_.size() < 1) {
    return false;
  }
  // The first sampler does not need to be set.
  for (auto i = 1u; i < samplers_.size(); i++) {
    if (samplers_[i] == nullptr) {
      return false;
    }
    // The samplers should have been checked as they were added, this
    // is a double-sanity-check.
    FML_DCHECK(samplers_[i]->isUIThreadSafe());
  }
  return true;
}

void ReusableFragmentShader::Dispose() {
  uniform_data_.reset();
  program_ = nullptr;
  samplers_.clear();
  ClearCodiraWrapper();
}

ReusableFragmentShader::~ReusableFragmentShader() = default;

}  // namespace appcode
