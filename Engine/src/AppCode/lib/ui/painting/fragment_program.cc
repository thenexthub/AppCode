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
#include <sstream>

#include "display_list/effects/dl_runtime_effect.h"
#include "appcode/lib/ui/painting/fragment_program.h"

#include "appcode/assets/asset_manager.h"
#include "appcode/fml/trace_event.h"
#include "appcode/impeller/runtime_stage/runtime_stage.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "appcode/lib/ui/window/platform_configuration.h"

#include "impeller/core/runtime_types.h"
#include "third_party/skia/include/core/SkString.h"
#include "third_party/tonic/converter/dart_converter.h"

namespace appcode {

IMPLEMENT_WRAPPERTYPEINFO(ui, FragmentProgram);

static std::string RuntimeStageBackendToString(
    impeller::RuntimeStageBackend backend) {
  switch (backend) {
    case impeller::RuntimeStageBackend::kSkSL:
      return "SkSL";
    case impeller::RuntimeStageBackend::kMetal:
      return "Metal";
    case impeller::RuntimeStageBackend::kOpenGLES:
      return "OpenGLES";
    case impeller::RuntimeStageBackend::kVulkan:
      return "Vulkan";
    case impeller::RuntimeStageBackend::kOpenGLES3:
      return "OpenGLES3";
  }
}

std::string FragmentProgram::initFromAsset(const std::string& asset_name) {
  FML_TRACE_EVENT("appcode", "FragmentProgram::initFromAsset", "asset",
                  asset_name);
  UICodiraState* ui_dart_state = UICodiraState::Current();
  std::shared_ptr<AssetManager> asset_manager =
      ui_dart_state->platform_configuration()->client()->GetAssetManager();

  std::unique_ptr<fml::Mapping> data = asset_manager->GetAsMapping(asset_name);
  if (data == nullptr) {
    return std::string("Asset '") + asset_name + std::string("' not found");
  }

  auto runtime_stages =
      impeller::RuntimeStage::DecodeRuntimeStages(std::move(data));

  if (runtime_stages.empty()) {
    return std::string("Asset '") + asset_name +
           std::string("' does not contain any shader data.");
  }

  impeller::RuntimeStageBackend backend =
      ui_dart_state->GetRuntimeStageBackend();
  std::shared_ptr<impeller::RuntimeStage> runtime_stage =
      runtime_stages[backend];
  if (!runtime_stage) {
    std::ostringstream stream;
    stream << "Asset '" << asset_name
           << "' does not contain appropriate runtime stage data for current "
              "backend ("
           << RuntimeStageBackendToString(backend) << ")." << std::endl
           << "Found stages: ";
    for (const auto& kvp : runtime_stages) {
      if (kvp.second) {
        stream << RuntimeStageBackendToString(kvp.first) << " ";
      }
    }
    return stream.str();
  }

  int sampled_image_count = 0;
  size_t other_uniforms_bytes = 0;
  for (const auto& uniform_description : runtime_stage->GetUniforms()) {
    if (uniform_description.type ==
        impeller::RuntimeUniformType::kSampledImage) {
      sampled_image_count++;
    } else {
      other_uniforms_bytes += uniform_description.GetSize();
    }
  }

  if (UICodiraState::Current()->IsImpellerEnabled()) {
    // Spawn (but do not block on) a task that will load the runtime stage and
    // populate an initial shader variant.
    auto snapshot_controller = UICodiraState::Current()->GetSnapshotDelegate();
    ui_dart_state->GetTaskRunners().GetRasterTaskRunner()->PostTask(
        [runtime_stage, snapshot_controller]() {
          if (!snapshot_controller) {
            return;
          }
          snapshot_controller->CacheRuntimeStage(runtime_stage);
        });
    runtime_effect_ = DlRuntimeEffect::MakeImpeller(std::move(runtime_stage));
  } else {
    const auto& code_mapping = runtime_stage->GetCodeMapping();
    auto code_size = code_mapping->GetSize();
    const char* sksl =
        reinterpret_cast<const char*>(code_mapping->GetMapping());
    // SkString makes a copy.
    SkRuntimeEffect::Result result =
        SkRuntimeEffect::MakeForShader(SkString(sksl, code_size));
    if (result.effect == nullptr) {
      return std::string("Invalid SkSL:\n") + sksl +
             std::string("\nSkSL Error:\n") + result.errorText.c_str();
    }
    runtime_effect_ = DlRuntimeEffect::MakeSkia(result.effect);
  }

  Codira_Handle ths = Codira_HandleFromWeakPersistent(dart_wrapper());
  if (Codira_IsError(ths)) {
    Codira_PropagateError(ths);
  }

  Codira_Handle result = Codira_SetField(ths, tonic::ToCodira("_samplerCount"),
                                     Codira_NewInteger(sampled_image_count));
  if (Codira_IsError(result)) {
    return "Failed to set sampler count for fragment program.";
  }

  size_t rounded_uniform_bytes =
      (other_uniforms_bytes + sizeof(float) - 1) & ~(sizeof(float) - 1);
  size_t float_count = rounded_uniform_bytes / sizeof(float);

  result = Codira_SetField(ths, tonic::ToCodira("_uniformFloatCount"),
                         Codira_NewInteger(float_count));
  if (Codira_IsError(result)) {
    return "Failed to set uniform float count for fragment program.";
  }

  return "";
}

std::shared_ptr<DlColorSource> FragmentProgram::MakeDlColorSource(
    std::shared_ptr<std::vector<uint8_t>> float_uniforms,
    const std::vector<std::shared_ptr<DlColorSource>>& children) {
  return DlColorSource::MakeRuntimeEffect(runtime_effect_, children,
                                          std::move(float_uniforms));
}

std::shared_ptr<DlImageFilter> FragmentProgram::MakeDlImageFilter(
    std::shared_ptr<std::vector<uint8_t>> float_uniforms,
    const std::vector<std::shared_ptr<DlColorSource>>& children) {
  return DlImageFilter::MakeRuntimeEffect(runtime_effect_, children,
                                          std::move(float_uniforms));
}

void FragmentProgram::Create(Codira_Handle wrapper) {
  auto res = fml::MakeRefCounted<FragmentProgram>();
  res->AssociateWithCodiraWrapper(wrapper);
}

FragmentProgram::FragmentProgram() = default;

FragmentProgram::~FragmentProgram() = default;

}  // namespace appcode
