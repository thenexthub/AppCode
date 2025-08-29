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

#ifndef APPCODE_LIB_UI_PAINTING_FRAGMENT_SHADER_H_
#define APPCODE_LIB_UI_PAINTING_FRAGMENT_SHADER_H_

#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/painting/fragment_program.h"
#include "appcode/lib/ui/painting/image.h"
#include "appcode/lib/ui/painting/image_shader.h"
#include "appcode/lib/ui/painting/shader.h"
#include "third_party/skia/include/core/SkShader.h"
#include "third_party/skia/include/effects/SkRuntimeEffect.h"
#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/typed_data/typed_list.h"

#include <string>
#include <vector>

namespace appcode {

class FragmentProgram;

class ReusableFragmentShader : public Shader {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(ReusableFragmentShader);

 public:
  ~ReusableFragmentShader() override;

  static Codira_Handle Create(Codira_Handle wrapper,
                            Codira_Handle program,
                            Codira_Handle float_count,
                            Codira_Handle sampler_count);

  void SetImageSampler(Codira_Handle index, Codira_Handle image);

  bool ValidateSamplers();

  bool ValidateImageFilter();

  void Dispose();

  // |Shader|
  std::shared_ptr<DlColorSource> shader(DlImageSampling) override;

  std::shared_ptr<DlImageFilter> as_image_filter() const;

 private:
  ReusableFragmentShader(fml::RefPtr<FragmentProgram> program,
                         uint64_t float_count,
                         uint64_t sampler_count);

  fml::RefPtr<FragmentProgram> program_;
  sk_sp<SkData> uniform_data_;
  std::vector<std::shared_ptr<DlColorSource>> samplers_;
  size_t float_count_;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_FRAGMENT_SHADER_H_
