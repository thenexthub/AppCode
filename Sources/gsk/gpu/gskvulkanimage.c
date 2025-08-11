#include "config.h"

#include "gskvulkanimageprivate.h"

#include "gskgpuutilsprivate.h"
#include "gskvulkanbufferprivate.h"
#include "gskvulkanframeprivate.h"
#include "gskvulkanmemoryprivate.h"
#include "gskvulkanycbcrprivate.h"

#include "gdk/gdkdisplayprivate.h"
#include "gdk/gdkdmabuftextureprivate.h"
#include "gdk/gdkvulkancontextprivate.h"
#include "gdk/gdkmemoryformatprivate.h"
#include "gdk/gdkvulkancontextprivate.h"

#include <fcntl.h>
#include <string.h>
#ifdef HAVE_DMABUF
#include <linux/dma-buf.h>
#endif

struct _GskVulkanImage
{
  GskGpuImage parent_instance;

  GskVulkanDevice *device;

  VkFormat vk_format;
  VkImageTiling vk_tiling;
  VkImageUsageFlags vk_usage;
  VkImage vk_image;
  VkImageView vk_image_view;
  VkFramebuffer vk_framebuffer;
  VkImageView vk_framebuffer_image_view;
  GskVulkanYcbcr *ycbcr;
  VkSemaphore vk_semaphore;
  uint64_t vk_semaphore_wait;
  struct {
    VkDescriptorSet vk_descriptor_set;
    gsize pool_id;
  } descriptor_sets[GSK_GPU_SAMPLER_N_SAMPLERS];

  VkPipelineStageFlags vk_pipeline_stage;
  VkImageLayout vk_image_layout;
  VkAccessFlags vk_access;

  GskVulkanAllocator *allocator;
  GskVulkanAllocation allocation;
};

G_DEFINE_TYPE (GskVulkanImage, gsk_vulkan_image, GSK_TYPE_GPU_IMAGE)

static gboolean
gsk_component_mapping_is_framebuffer_compatible (const VkComponentMapping *components)
{
  if ((components->r != VK_COMPONENT_SWIZZLE_R && components->r != VK_COMPONENT_SWIZZLE_IDENTITY) ||
      (components->g != VK_COMPONENT_SWIZZLE_G && components->g != VK_COMPONENT_SWIZZLE_IDENTITY) ||
      (components->b != VK_COMPONENT_SWIZZLE_B && components->b != VK_COMPONENT_SWIZZLE_IDENTITY) ||
      (components->a != VK_COMPONENT_SWIZZLE_A && components->a != VK_COMPONENT_SWIZZLE_IDENTITY))
    return FALSE;

  return TRUE;
}

static gboolean
gsk_vulkan_get_ycbcr_flags (GskGpuConversion               conv,
                            VkSamplerYcbcrModelConversion *out_model,
                            VkSamplerYcbcrRange           *out_range)
{
  switch (conv)
    {
    case GSK_GPU_CONVERSION_NONE:
      *out_model = VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY;
      *out_range = VK_SAMPLER_YCBCR_RANGE_ITU_FULL;
      return FALSE;

    case GSK_GPU_CONVERSION_SRGB:
      *out_model = VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY;
      *out_range = VK_SAMPLER_YCBCR_RANGE_ITU_FULL;
      return FALSE;

    case GSK_GPU_CONVERSION_BT601:
      *out_model = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_601;
      *out_range = VK_SAMPLER_YCBCR_RANGE_ITU_FULL;
      return TRUE;

    case GSK_GPU_CONVERSION_BT601_NARROW:
      *out_model = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_601;
      *out_range = VK_SAMPLER_YCBCR_RANGE_ITU_NARROW;
      return TRUE;

    case GSK_GPU_CONVERSION_BT709:
      *out_model = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_709;
      *out_range = VK_SAMPLER_YCBCR_RANGE_ITU_FULL;
      return TRUE;

    case GSK_GPU_CONVERSION_BT709_NARROW:
      *out_model = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_709;
      *out_range = VK_SAMPLER_YCBCR_RANGE_ITU_NARROW;
      return TRUE;

    case GSK_GPU_CONVERSION_BT2020:
      *out_model = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020;
      *out_range = VK_SAMPLER_YCBCR_RANGE_ITU_FULL;
      return TRUE;

    case GSK_GPU_CONVERSION_BT2020_NARROW:
      *out_model = VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020;
      *out_range = VK_SAMPLER_YCBCR_RANGE_ITU_NARROW;
      return TRUE;

    default:
      g_assert_not_reached ();
      return FALSE;
    }
}

static gboolean
gsk_vulkan_device_supports_format (GskVulkanDevice   *device,
                                   VkFormat           format,
                                   uint64_t           modifier,
                                   guint              n_planes,
                                   VkImageTiling      tiling,
                                   VkImageUsageFlags  usage,
                                   gsize              width,
                                   gsize              height,
                                   GskGpuImageFlags  *out_flags)
{
  VkDrmFormatModifierPropertiesEXT drm_mod_properties[100];
  VkDrmFormatModifierPropertiesListEXT drm_properties;
  VkPhysicalDevice vk_phys_device;
  VkFormatProperties2 properties;
  VkImageFormatProperties2 image_properties;
  VkFormatFeatureFlags features;
  VkResult res;
  gsize i;

  vk_phys_device = gsk_vulkan_device_get_vk_physical_device (device);

  drm_properties = (VkDrmFormatModifierPropertiesListEXT) {
    .sType = VK_STRUCTURE_TYPE_DRM_FORMAT_MODIFIER_PROPERTIES_LIST_EXT,
    .drmFormatModifierCount = G_N_ELEMENTS (drm_mod_properties),
    .pDrmFormatModifierProperties = drm_mod_properties,
  };
  properties = (VkFormatProperties2) {
    .sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2,
    .pNext = (tiling != VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT) ? NULL : &drm_properties
  };
  vkGetPhysicalDeviceFormatProperties2 (vk_phys_device,
                                        format,
                                        &properties);

  switch ((int) tiling)
    {
      case VK_IMAGE_TILING_OPTIMAL:
        features = properties.formatProperties.optimalTilingFeatures;
        break;
      case VK_IMAGE_TILING_LINEAR:
        features = properties.formatProperties.linearTilingFeatures;
        break;
      case VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT:
        features = 0;
        for (i = 0; i < drm_properties.drmFormatModifierCount; i++)
          {
            if (drm_mod_properties[i].drmFormatModifier == modifier &&
                drm_mod_properties[i].drmFormatModifierPlaneCount == n_planes)
              {
                features = drm_mod_properties[i].drmFormatModifierTilingFeatures;
                break;
              }
          }
        if (features == 0)
          return FALSE;
        break;
      default:
        return FALSE;
    }

  if (!(features & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
    return FALSE;

  image_properties = (VkImageFormatProperties2) {
    .sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2,
  };
  res = vkGetPhysicalDeviceImageFormatProperties2 (vk_phys_device,
                                                   &(VkPhysicalDeviceImageFormatInfo2) {
                                                     .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2,
                                                     .format = format,
                                                     .type = VK_IMAGE_TYPE_2D,
                                                     .tiling = tiling,
                                                     .usage = usage,
                                                     .flags = 0,
                                                     .pNext = (tiling != VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT) ? NULL : &(VkPhysicalDeviceImageDrmFormatModifierInfoEXT) {
                                                         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_DRM_FORMAT_MODIFIER_INFO_EXT,
                                                         .drmFormatModifier = modifier,
                                                         .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                                         .queueFamilyIndexCount = 1,
                                                         .pQueueFamilyIndices = (uint32_t[1]) { gsk_vulkan_device_get_vk_queue_family_index (device) },
                                                      }
                                                   },
                                                   &image_properties);
  if (res != VK_SUCCESS)
    return FALSE;

  if (image_properties.imageFormatProperties.maxExtent.width < width ||
      image_properties.imageFormatProperties.maxExtent.height < height)
    return FALSE;

  *out_flags = 0;
  if (features & VK_FORMAT_FEATURE_BLIT_SRC_BIT)
    *out_flags |= GSK_GPU_IMAGE_BLIT;
  if (features & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)
    *out_flags |= GSK_GPU_IMAGE_FILTERABLE;
  if (features & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT)
    *out_flags |= GSK_GPU_IMAGE_RENDERABLE;
  if (features & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT)
    *out_flags |= GSK_GPU_IMAGE_DOWNLOADABLE;
  if (image_properties.imageFormatProperties.maxMipLevels >= gsk_gpu_mipmap_levels (width, height) &&
      (*out_flags & (GSK_GPU_IMAGE_BLIT | GSK_GPU_IMAGE_FILTERABLE | GSK_GPU_IMAGE_RENDERABLE)) == (GSK_GPU_IMAGE_BLIT | GSK_GPU_IMAGE_FILTERABLE | GSK_GPU_IMAGE_RENDERABLE))
    *out_flags |= GSK_GPU_IMAGE_CAN_MIPMAP;

  return TRUE;
}

static void
gsk_vulkan_image_create_view (GskVulkanImage            *this,
                              VkFormat                   vk_format,
                              const VkComponentMapping  *vk_components,
                              VkSamplerYcbcrConversion   vk_conversion)
{
  GSK_VK_CHECK (vkCreateImageView, gsk_vulkan_device_get_vk_device (this->device),
                                 &(VkImageViewCreateInfo) {
                                     .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                     .image = this->vk_image,
                                     .viewType = VK_IMAGE_VIEW_TYPE_2D,
                                     .format = vk_format,
                                     .components = vk_conversion ? (VkComponentMapping) { 0, }
                                                                 : *vk_components,
                                     .subresourceRange = {
                                         .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                         .baseMipLevel = 0,
                                         .levelCount = VK_REMAINING_MIP_LEVELS,
                                         .baseArrayLayer = 0,
                                         .layerCount = 1,
                                     },
                                     .pNext = vk_conversion == VK_NULL_HANDLE ? NULL : &(VkSamplerYcbcrConversionInfo) {
                                         .sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO,
                                         .conversion = vk_conversion
                                     }
                                 },
                                 NULL,
                                 &this->vk_image_view);
}

static gboolean
gsk_vulkan_device_check_format (GskVulkanDevice          *device,
                                VkFormat                  vk_format,
                                const VkComponentMapping *vk_components,
                                GskGpuImageFlags          required_flags,
                                VkImageTiling             vk_tiling,
                                VkImageUsageFlags         vk_usage,
                                gsize                     width,
                                gsize                     height,
                                VkImageTiling            *out_tiling,
                                GskGpuImageFlags         *out_flags)
{
#define CHECK_FLAGS (GSK_GPU_IMAGE_BLIT | GSK_GPU_IMAGE_FILTERABLE | GSK_GPU_IMAGE_RENDERABLE | GSK_GPU_IMAGE_CAN_MIPMAP)
  GskGpuImageFlags flags;

  if (vk_format == VK_FORMAT_UNDEFINED)
    return FALSE;

  if (vk_usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT &&
      !gsk_component_mapping_is_framebuffer_compatible (vk_components))
    return FALSE;

  if (gsk_vulkan_device_supports_format (device,
                                         vk_format,
                                         0, 1,
                                         vk_tiling, vk_usage,
                                         width, height,
                                         &flags) &&
      ((flags & required_flags & CHECK_FLAGS) == (required_flags & CHECK_FLAGS)))
    {
      *out_tiling = vk_tiling;
      *out_flags = flags;
      return TRUE;
    }

  if (vk_tiling == VK_IMAGE_TILING_LINEAR &&
      gsk_vulkan_device_supports_format (device,
                                         vk_format,
                                         0, 1,
                                         VK_IMAGE_TILING_OPTIMAL, vk_usage,
                                         width, height,
                                         &flags) &&
      ((flags & required_flags & CHECK_FLAGS) == (required_flags & CHECK_FLAGS)))
    {
      *out_tiling = VK_IMAGE_TILING_OPTIMAL;
      *out_flags = flags;
      return TRUE;
    }

  return FALSE;
#undef CHECK_FLAGS
}

static gboolean
gsk_vulkan_device_supports_conversion (GskVulkanDevice *device,
                                       GskGpuConversion conv)
{
  switch (conv)
    {
    case GSK_GPU_CONVERSION_SRGB:
    case GSK_GPU_CONVERSION_NONE:
      break;

    case GSK_GPU_CONVERSION_BT601:
    case GSK_GPU_CONVERSION_BT601_NARROW:
    case GSK_GPU_CONVERSION_BT709:
    case GSK_GPU_CONVERSION_BT709_NARROW:
    case GSK_GPU_CONVERSION_BT2020:
    case GSK_GPU_CONVERSION_BT2020_NARROW:
      if (!gsk_vulkan_device_has_feature (device, GDK_VULKAN_FEATURE_YCBCR))
        return FALSE;
      break;

    default:
      break;
    }

  return TRUE;
}

static GskVulkanImage *
gsk_vulkan_image_new (GskVulkanDevice           *device,
                      GdkMemoryFormat            format,
                      GskGpuConversion           conv,
                      GskGpuImageFlags           required_flags,
                      gsize                      width,
                      gsize                      height,
                      VkImageTiling              tiling,
                      VkImageUsageFlags          usage,
                      VkPipelineStageFlags       stage,
                      VkImageLayout              layout,
                      VkAccessFlags              access,
                      VkMemoryPropertyFlags      memory)
{
  VkMemoryRequirements requirements;
  GskVulkanImage *this;
  VkDevice vk_device;
  GskGpuImageFlags flags;
  gboolean try_srgb;
  VkFormat vk_format, vk_srgb_format;
  VkComponentMapping vk_components;
  VkSamplerYcbcrConversion vk_conversion;
  VkSamplerYcbcrModelConversion vk_model;
  VkSamplerYcbcrRange vk_range;
  GdkShaderOp shader_op;
  gboolean needs_conversion;
  gsize memory_index;

  g_assert (width > 0 && height > 0);

  if (!gsk_vulkan_device_supports_conversion (device, conv))
    conv = GSK_GPU_CONVERSION_NONE;

  vk_srgb_format = VK_FORMAT_UNDEFINED;
  try_srgb = conv == GSK_GPU_CONVERSION_SRGB;

  /* First, try the actual format */
  vk_format = gdk_memory_format_vk_format (format, &vk_components, &needs_conversion);
  if (try_srgb)
    vk_srgb_format = gdk_memory_format_vk_srgb_format (format);
  if (gsk_vulkan_device_check_format (device, vk_srgb_format, &vk_components, required_flags,
                                      tiling, usage, width, height,
                                      &tiling, &flags))
    {
      vk_format = vk_srgb_format;
    }
  else if (!gsk_vulkan_device_check_format (device, vk_format, &vk_components, required_flags,
                                            tiling, usage, width, height,
                                            &tiling, &flags))
    {
      GdkMemoryFormat rgba_format;
      GdkSwizzle rgba_swizzle;

      /* Second, try the potential RGBA format */
      if (gdk_memory_format_get_rgba_format (format, &rgba_format, &rgba_swizzle))
        {
          vk_format = gdk_memory_format_vk_format (rgba_format, &vk_components, &needs_conversion);
          gdk_swizzle_to_vk_component_mapping (rgba_swizzle, &vk_components);
        }
      else
        vk_format = VK_FORMAT_UNDEFINED;
      if (try_srgb && vk_format != VK_FORMAT_UNDEFINED)
        vk_srgb_format = gdk_memory_format_vk_srgb_format (rgba_format);
      else
        vk_srgb_format = VK_FORMAT_UNDEFINED;
      if (gsk_vulkan_device_check_format (device, vk_srgb_format, &vk_components, required_flags,
                                          tiling, usage, width, height,
                                          &tiling, &flags))
        {
          vk_format = vk_srgb_format;
        }
      else if (!gsk_vulkan_device_check_format (device, vk_format, &vk_components, required_flags,
                                                tiling, usage, width, height,
                                                &tiling, &flags))
        {
          const GdkMemoryFormat *fallbacks;
          gsize i;

          /* Next, try the fallbacks */
          fallbacks = gdk_memory_format_get_fallbacks (format);
          for (i = 0; fallbacks[i] != -1; i++)
            {
              vk_format = gdk_memory_format_vk_format (fallbacks[i], &vk_components, &needs_conversion);
              if (try_srgb)
                vk_srgb_format = gdk_memory_format_vk_srgb_format (fallbacks[i]);
              if (gsk_vulkan_device_check_format (device, vk_srgb_format, &vk_components, required_flags,
                                                  tiling, usage, width, height,
                                                  &tiling, &flags))
                {
                  vk_format = vk_srgb_format;
                  format = fallbacks[i];
                  break;
                }
              else if (gsk_vulkan_device_check_format (device, vk_format, &vk_components, required_flags,
                                                       tiling, usage, width, height,
                                                       &tiling, &flags))
                {
                  format = fallbacks[i];
                  break;
                }
            }

          /* No format found. Likely, the width/height are too big */
          if (fallbacks[i] == -1)
            return NULL;
        }
    }

  shader_op = gdk_memory_format_get_default_shader_op (format);

  if (vk_format != vk_srgb_format && conv == GSK_GPU_CONVERSION_SRGB)
    conv = GSK_GPU_CONVERSION_NONE;

  if (((flags & (GSK_GPU_IMAGE_FILTERABLE | GSK_GPU_IMAGE_RENDERABLE | GSK_GPU_IMAGE_BLIT | GSK_GPU_IMAGE_CAN_MIPMAP)) !=
       (GSK_GPU_IMAGE_FILTERABLE | GSK_GPU_IMAGE_RENDERABLE | GSK_GPU_IMAGE_BLIT | GSK_GPU_IMAGE_CAN_MIPMAP)) ||
      shader_op != GDK_SHADER_DEFAULT ||
      !(required_flags & GSK_GPU_IMAGE_CAN_MIPMAP))
    flags &= ~GSK_GPU_IMAGE_CAN_MIPMAP;

  if (!gsk_component_mapping_is_framebuffer_compatible (&vk_components) || needs_conversion)
    flags &= ~GSK_GPU_IMAGE_BLIT;

  vk_device = gsk_vulkan_device_get_vk_device (device);

  this = g_object_new (GSK_TYPE_VULKAN_IMAGE, NULL);

  this->device = g_object_ref (device);
  this->vk_format = vk_format;
  this->vk_tiling = tiling;
  this->vk_usage = usage;
  this->vk_pipeline_stage = stage;
  this->vk_image_layout = layout;
  this->vk_access = access;

  if (gsk_vulkan_get_ycbcr_flags (conv, &vk_model, &vk_range) || needs_conversion)
    {
      this->ycbcr = gsk_vulkan_ycbcr_get (device,
                                          &(GskVulkanYcbcrInfo) {
                                              .vk_format = vk_format,
                                              .vk_components = vk_components,
                                              .vk_ycbcr_model = vk_model,
                                              .vk_ycbcr_range = vk_range,
                                          });
      gsk_vulkan_ycbcr_ref (this->ycbcr);
      vk_conversion = gsk_vulkan_ycbcr_get_vk_conversion (this->ycbcr);
      flags |= GSK_GPU_IMAGE_EXTERNAL;
      shader_op = GDK_SHADER_DEFAULT;
    }
  else
    vk_conversion = VK_NULL_HANDLE;

  gsk_gpu_image_setup (GSK_GPU_IMAGE (this), flags, conv, shader_op, format, width, height);

  GSK_VK_CHECK (vkCreateImage, vk_device,
                                &(VkImageCreateInfo) {
                                    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                                    .flags = 0,
                                    .imageType = VK_IMAGE_TYPE_2D,
                                    .format = vk_format,
                                    .extent = { width, height, 1 },
                                    .mipLevels = (flags & GSK_GPU_IMAGE_CAN_MIPMAP) ? gsk_gpu_mipmap_levels (width, height) : 1,
                                    .arrayLayers = 1,
                                    .samples = VK_SAMPLE_COUNT_1_BIT,
                                    .tiling = tiling,
                                    .usage = usage |
                                             (flags & (GSK_GPU_IMAGE_BLIT | GSK_GPU_IMAGE_DOWNLOADABLE) ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0),
                                    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                    .initialLayout = this->vk_image_layout,
                                },
                                NULL,
                                &this->vk_image);

  vkGetImageMemoryRequirements (vk_device,
                                this->vk_image,
                                &requirements);

  memory_index = gsk_vulkan_device_find_allocator (device,
                                                   requirements.memoryTypeBits,
                                                   0,
                                                   tiling == VK_IMAGE_TILING_LINEAR ? GSK_VULKAN_MEMORY_MAPPABLE : 0);
  this->allocator = gsk_vulkan_device_get_allocator (device, memory_index);
  gsk_vulkan_allocator_ref (this->allocator);

  gsk_vulkan_alloc (this->allocator,
                    requirements.size,
                    requirements.alignment,
                    &this->allocation);

  GSK_VK_CHECK (vkBindImageMemory, vk_device,
                                   this->vk_image,
                                   this->allocation.vk_memory,
                                   this->allocation.offset);

  gsk_vulkan_image_create_view (this, vk_format, &vk_components, vk_conversion);

  return this;
}

GskGpuImage *
gsk_vulkan_image_new_for_upload (GskVulkanDevice *device,
                                 gboolean         with_mipmap,
                                 GdkMemoryFormat  format,
                                 GskGpuConversion conv,
                                 gsize            width,
                                 gsize            height)
{
  GskVulkanImage *this;

  this = gsk_vulkan_image_new (device,
                               format,
                               conv,
                               0, //with_mipmap ? (GSK_GPU_IMAGE_CAN_MIPMAP | GSK_GPU_IMAGE_RENDERABLE | GSK_GPU_IMAGE_FILTERABLE) : 0,
                               width,
                               height,
                               VK_IMAGE_TILING_LINEAR,
                               VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                               VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                               VK_IMAGE_USAGE_SAMPLED_BIT,
                               VK_PIPELINE_STAGE_TRANSFER_BIT,
                               VK_IMAGE_LAYOUT_PREINITIALIZED,
                               VK_ACCESS_TRANSFER_WRITE_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  return GSK_GPU_IMAGE (this);
}

static gboolean
gsk_vulkan_image_can_map (GskVulkanImage *this)
{
  if (GSK_DEBUG_CHECK (STAGING))
    return FALSE;

  if (this->vk_tiling != VK_IMAGE_TILING_LINEAR)
    return FALSE;

  if (this->vk_image_layout != VK_IMAGE_LAYOUT_PREINITIALIZED &&
      this->vk_image_layout != VK_IMAGE_LAYOUT_GENERAL)
    return FALSE;

  if ((this->allocation.memory_flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) == 0)
    return FALSE;

  return this->allocation.map != NULL;
}

guchar *
gsk_vulkan_image_get_data (GskVulkanImage  *this,
                           GdkMemoryLayout *out_layout)
{ 
  const VkImageAspectFlags aspect_flags[3] = { VK_IMAGE_ASPECT_PLANE_0_BIT, VK_IMAGE_ASPECT_PLANE_1_BIT, VK_IMAGE_ASPECT_PLANE_2_BIT };
  GskGpuImage *image = GSK_GPU_IMAGE (this);
  VkSubresourceLayout image_layout;
  VkDevice vk_device;
  gsize i, n_planes;

  if (!gsk_vulkan_image_can_map (this))
    return NULL;

  out_layout->format = gsk_gpu_image_get_format (image);
  out_layout->width = gsk_gpu_image_get_width (image);
  out_layout->height = gsk_gpu_image_get_height (image);
  out_layout->size = this->allocation.size;
  n_planes = gdk_memory_format_get_n_planes (out_layout->format);
  vk_device = gsk_vulkan_device_get_vk_device (this->device);

  for (i = 0; i < n_planes; i++)
    {
      vkGetImageSubresourceLayout (vk_device,
                                   this->vk_image,
                                   &(VkImageSubresource) {
                                       .aspectMask = n_planes == 1 ? VK_IMAGE_ASPECT_COLOR_BIT : aspect_flags[i],
                                       .mipLevel = 0,
                                       .arrayLayer = 0
                                   },
                                   &image_layout);
      out_layout->planes[i].offset = image_layout.offset;
      out_layout->planes[i].stride = image_layout.rowPitch;
    }

  return this->allocation.map;
}

GskGpuImage *
gsk_vulkan_image_new_for_swapchain (GskVulkanDevice  *device,
                                    VkImage           image,
                                    VkFormat          format,
                                    GdkMemoryFormat   memory_format,
                                    gsize             width,
                                    gsize             height)
{
  GskVulkanImage *this;
  GskGpuImageFlags flags = 0;
  GskGpuConversion conv;

  this = g_object_new (GSK_TYPE_VULKAN_IMAGE, NULL);

  this->device = g_object_ref (device);
  this->vk_tiling = VK_IMAGE_TILING_OPTIMAL;
  this->vk_image = image;
  this->vk_format = format;
  this->vk_pipeline_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  this->vk_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
  this->vk_access = 0;

  if (format == gdk_memory_format_vk_srgb_format (memory_format))
    conv = GSK_GPU_CONVERSION_SRGB;
  else
    conv = GSK_GPU_CONVERSION_NONE;

  /* FIXME: The flags here are very suboptimal */
  gsk_gpu_image_setup (GSK_GPU_IMAGE (this), flags, conv, GDK_SHADER_DEFAULT, memory_format, width, height);

  gsk_vulkan_image_create_view (this,
                                format,
                                &(VkComponentMapping) {
                                  VK_COMPONENT_SWIZZLE_R,
                                  VK_COMPONENT_SWIZZLE_G,
                                  VK_COMPONENT_SWIZZLE_B,
                                  VK_COMPONENT_SWIZZLE_A
                                },
                                VK_NULL_HANDLE);

  return GSK_GPU_IMAGE (this);
}

GskGpuImage *
gsk_vulkan_image_new_for_atlas (GskVulkanDevice *device,
                                gsize            width,
                                gsize            height)
{
  GskVulkanImage *this;

  this = gsk_vulkan_image_new (device,
                               GDK_MEMORY_DEFAULT,
                               GSK_GPU_CONVERSION_NONE,
                               GSK_GPU_IMAGE_FILTERABLE | GSK_GPU_IMAGE_RENDERABLE,
                               width,
                               height,
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                               VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                               VK_IMAGE_LAYOUT_UNDEFINED,
                               0,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  return GSK_GPU_IMAGE (this);
}

GskGpuImage *
gsk_vulkan_image_new_for_offscreen (GskVulkanDevice *device,
                                    gboolean         with_mipmap,
                                    GdkMemoryFormat  preferred_format,
                                    gboolean         try_srgb,
                                    gsize            width,
                                    gsize            height)
{
  GskVulkanImage *this;

  this = gsk_vulkan_image_new (device,
                               preferred_format,
                               try_srgb ? GSK_GPU_CONVERSION_SRGB : GSK_GPU_CONVERSION_NONE,
                               GSK_GPU_IMAGE_RENDERABLE |
                               (with_mipmap ? GSK_GPU_IMAGE_CAN_MIPMAP | GSK_GPU_IMAGE_FILTERABLE : 0),
                               width,
                               height,
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                               VK_IMAGE_USAGE_SAMPLED_BIT |
                               VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                               VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                               VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                               VK_IMAGE_LAYOUT_UNDEFINED,
                               VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  return GSK_GPU_IMAGE (this);
}

#ifdef HAVE_DMABUF
static gboolean
gsk_vulkan_device_check_dmabuf_format (GskVulkanDevice          *device,
                                       VkFormat                  vk_format,
                                       const VkComponentMapping *vk_components,
                                       gsize                     width,
                                       gsize                     height,
                                       uint64_t                  modifiers[100],
                                       GskGpuImageFlags         *out_flags,
                                       gsize                    *out_n_modifiers)

{
  const VkFormatFeatureFlags required = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT;
  VkDrmFormatModifierPropertiesEXT drm_mod_properties[100];
  VkDrmFormatModifierPropertiesListEXT drm_properties;
  VkPhysicalDevice vk_phys_device;
  VkFormatProperties2 properties;
  VkImageFormatProperties2 image_properties;
  GskGpuImageFlags flags;
  gsize i, n_modifiers;
  VkResult res;

  drm_properties = (VkDrmFormatModifierPropertiesListEXT) {
    .sType = VK_STRUCTURE_TYPE_DRM_FORMAT_MODIFIER_PROPERTIES_LIST_EXT,
    .drmFormatModifierCount = G_N_ELEMENTS (drm_mod_properties),
    .pDrmFormatModifierProperties = drm_mod_properties,
  };
  properties = (VkFormatProperties2) {
    .sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2,
    .pNext = &drm_properties
  };

  if (vk_format == VK_FORMAT_UNDEFINED)
    return FALSE;

  if (!gsk_component_mapping_is_framebuffer_compatible (vk_components))
    return FALSE;

  vk_phys_device = gsk_vulkan_device_get_vk_physical_device (device);

  vkGetPhysicalDeviceFormatProperties2 (vk_phys_device,
                                        vk_format,
                                        &properties);

  flags = GSK_GPU_IMAGE_BLIT | GSK_GPU_IMAGE_FILTERABLE | GSK_GPU_IMAGE_RENDERABLE | GSK_GPU_IMAGE_DOWNLOADABLE;
  n_modifiers = 0;
  for (i = 0; i < drm_properties.drmFormatModifierCount; i++)
    {
      if ((drm_mod_properties[i].drmFormatModifierTilingFeatures & required) != required)
        continue;

      image_properties = (VkImageFormatProperties2) {
        .sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2,
      };
      res = vkGetPhysicalDeviceImageFormatProperties2 (vk_phys_device,
                                                       &(VkPhysicalDeviceImageFormatInfo2) {
                                                         .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2,
                                                         .format = vk_format,
                                                         .type = VK_IMAGE_TYPE_2D,
                                                         .tiling = VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT,
                                                         .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                                         .flags = 0,
                                                         .pNext = &(VkPhysicalDeviceImageDrmFormatModifierInfoEXT) {
                                                             .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_DRM_FORMAT_MODIFIER_INFO_EXT,
                                                             .drmFormatModifier = drm_mod_properties[i].drmFormatModifier,
                                                             .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                                             .queueFamilyIndexCount = 1,
                                                             .pQueueFamilyIndices = (uint32_t[1]) { gsk_vulkan_device_get_vk_queue_family_index (device) },
                                                          }
                                                       },
                                                       &image_properties);
      if (res != VK_SUCCESS)
        continue;

      if (image_properties.imageFormatProperties.maxExtent.width < width ||
          image_properties.imageFormatProperties.maxExtent.height < height)
        continue;

      /* we could check the real used format after creation, but for now: */
      if ((drm_mod_properties[i].drmFormatModifierTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT) == 0)
        flags &= ~GSK_GPU_IMAGE_BLIT;
      if ((drm_mod_properties[i].drmFormatModifierTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) == 0)
        flags &= ~GSK_GPU_IMAGE_FILTERABLE;
      if ((drm_mod_properties[i].drmFormatModifierTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT) == 0)
        flags &= ~GSK_GPU_IMAGE_RENDERABLE;
      if ((drm_mod_properties[i].drmFormatModifierTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT) == 0)
        flags &= ~GSK_GPU_IMAGE_DOWNLOADABLE;

      modifiers[n_modifiers++] = drm_mod_properties[i].drmFormatModifier;
    }

  if (n_modifiers == 0)
    return FALSE;

  *out_flags = flags;
  *out_n_modifiers = n_modifiers;
  return TRUE;
}

GskGpuImage *
gsk_vulkan_image_new_dmabuf (GskVulkanDevice *device,
                             GdkMemoryFormat  format,
                             gboolean         try_srgb,
                             gsize            width,
                             gsize            height)
{
  uint64_t modifiers[100];
  VkDevice vk_device;
  VkFormat vk_format, vk_srgb_format;
  VkComponentMapping vk_components;
  VkMemoryRequirements requirements;
  VkSamplerYcbcrConversion vk_conversion;
  gsize memory_index;
  GskVulkanImage *this;
  VkResult res;
  gsize n_modifiers;
  GskGpuImageFlags flags;
  GskGpuConversion conv;
  GdkShaderOp shader_op;
  gboolean needs_conversion;

  if (!gdk_has_feature (GDK_FEATURE_DMABUF) ||
      !gsk_vulkan_device_has_feature (device, GDK_VULKAN_FEATURE_DMABUF))
    return NULL;

  vk_srgb_format = VK_FORMAT_UNDEFINED;

  vk_device = gsk_vulkan_device_get_vk_device (device);

  /* First, try the actual format */
  vk_format = gdk_memory_format_vk_format (format, &vk_components, &needs_conversion);
  if (try_srgb)
    vk_srgb_format = gdk_memory_format_vk_srgb_format (format);
  if (gsk_vulkan_device_check_dmabuf_format (device, vk_srgb_format, &vk_components, width, height,
                                             modifiers, &flags, &n_modifiers))
    {
      vk_format = vk_srgb_format;
    }
  else if (!gsk_vulkan_device_check_dmabuf_format (device, vk_format, &vk_components, width, height,
                                                   modifiers, &flags, &n_modifiers))
    {
      /* Second, try the potential RGBA format, but as a fallback */
      GdkMemoryFormat rgba_format;
      GdkSwizzle rgba_swizzle;
      if (gdk_memory_format_get_rgba_format (format, &rgba_format, &rgba_swizzle))
        {
          vk_format = gdk_memory_format_vk_format (rgba_format, &vk_components, &needs_conversion);
          gdk_swizzle_to_vk_component_mapping (rgba_swizzle, &vk_components);
          if (try_srgb)
            vk_srgb_format = gdk_memory_format_vk_srgb_format (format);
        }
      else
        vk_format = vk_srgb_format = VK_FORMAT_UNDEFINED;
      if (gsk_vulkan_device_check_dmabuf_format (device, vk_srgb_format, &vk_components, width, height,
                                                 modifiers, &flags, &n_modifiers))
        {
          vk_format = vk_srgb_format;
          format = rgba_format;
        }
      else if (gsk_vulkan_device_check_dmabuf_format (device, vk_format, &vk_components, width, height,
                                                      modifiers, &flags, &n_modifiers))
        {
          format = rgba_format;
        }
      else
        {
          const GdkMemoryFormat *fallbacks;
          gsize i;

          /* Next, try the fallbacks */
          fallbacks = gdk_memory_format_get_fallbacks (format);
          for (i = 0; fallbacks[i] != -1; i++)
            {
              vk_format = gdk_memory_format_vk_format (fallbacks[i], &vk_components, &needs_conversion);
              if (try_srgb)
                vk_srgb_format = gdk_memory_format_vk_srgb_format (format);
              if (gsk_vulkan_device_check_dmabuf_format (device, vk_srgb_format, &vk_components, width, height,
                                                         modifiers, &flags, &n_modifiers))
                {
                  vk_format = vk_srgb_format;
                  format = fallbacks[i];
                  break;
                }
              else if (gsk_vulkan_device_check_dmabuf_format (device, vk_format, &vk_components, width, height,
                                                              modifiers, &flags, &n_modifiers))
                {
                  format = fallbacks[i];
                  break;
                }
            }

          /* No format found. Likely, the width/height are too big */
          if (fallbacks[i] == -1)
            return NULL;
        }
    }

  this = g_object_new (GSK_TYPE_VULKAN_IMAGE, NULL);

  this->device = g_object_ref (device);
  this->vk_tiling = VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT;
  this->vk_format = vk_format;
  this->vk_pipeline_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  this->vk_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
  this->vk_access = 0;

  flags |= GSK_GPU_IMAGE_EXTERNAL;

  if (vk_format == vk_srgb_format)
    conv = GSK_GPU_CONVERSION_SRGB;
  else
    conv = GSK_GPU_CONVERSION_NONE;

  if (!gsk_component_mapping_is_framebuffer_compatible (&vk_components) || needs_conversion)
    flags &= ~GSK_GPU_IMAGE_BLIT;

  if (needs_conversion)
    shader_op = GDK_SHADER_DEFAULT;
  else
    shader_op = gdk_memory_format_get_default_shader_op (format);

  gsk_gpu_image_setup (GSK_GPU_IMAGE (this),
                       flags,
                       conv,
                       shader_op,
                       format,
                       width, height);

  res = vkCreateImage (vk_device,
                       &(VkImageCreateInfo) {
                           .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                           .flags = 0,
                           .imageType = VK_IMAGE_TYPE_2D,
                           .format = vk_format,
                           .extent = { width, height, 1 },
                           .mipLevels = 1,
                           .arrayLayers = 1,
                           .samples = VK_SAMPLE_COUNT_1_BIT,
                           .tiling = VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT,
                           .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                    (flags & (GSK_GPU_IMAGE_BLIT | GSK_GPU_IMAGE_DOWNLOADABLE) ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0),
                           .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                           .initialLayout = this->vk_image_layout,
                           .pNext = &(VkExternalMemoryImageCreateInfo) {
                               .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO,
                               .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
                               .pNext = &(VkImageDrmFormatModifierListCreateInfoEXT) {
                                   .sType = VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_LIST_CREATE_INFO_EXT,
                                   .drmFormatModifierCount = n_modifiers,
                                   .pDrmFormatModifiers = modifiers
                               }
                           },
                       },
                       NULL,
                       &this->vk_image);
  if (res != VK_SUCCESS)
    {
      gsk_vulkan_handle_result (res, "vkCreateImage");
      return NULL;
    }

  vkGetImageMemoryRequirements (vk_device,
                                this->vk_image,
                                &requirements);

  memory_index = gsk_vulkan_device_find_allocator (device,
                                                   requirements.memoryTypeBits,
                                                   0,
                                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  this->allocator = gsk_vulkan_device_get_external_allocator (device);
  gsk_vulkan_allocator_ref (this->allocator);

  gsk_vulkan_alloc (this->allocator,
                    requirements.size,
                    requirements.alignment,
                    &this->allocation);

  GSK_VK_CHECK (vkAllocateMemory, vk_device,
                                  &(VkMemoryAllocateInfo) {
                                      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                      .allocationSize = requirements.size,
                                      .memoryTypeIndex = memory_index,
                                      .pNext = &(VkExportMemoryAllocateInfo) {
                                          .sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO,
                                          .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
                                      }
                                  },
                                  NULL,
                                  &this->allocation.vk_memory);

  GSK_VK_CHECK (vkBindImageMemory, vk_device,
                                   this->vk_image,
                                   this->allocation.vk_memory,
                                   this->allocation.offset);

  if (needs_conversion)
    {
      this->ycbcr = gsk_vulkan_ycbcr_get (device,
                                          &(GskVulkanYcbcrInfo) {
                                               .vk_format = vk_format,
                                               .vk_components = vk_components,
                                               .vk_ycbcr_model = VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY,
                                               .vk_ycbcr_range = VK_SAMPLER_YCBCR_RANGE_ITU_FULL,
                                          });
      gsk_vulkan_ycbcr_ref (this->ycbcr);
      vk_conversion = gsk_vulkan_ycbcr_get_vk_conversion (this->ycbcr);
    }
  else
    vk_conversion = VK_NULL_HANDLE;

  gsk_vulkan_image_create_view (this, vk_format, &vk_components, vk_conversion);

  return GSK_GPU_IMAGE (this);
}

GskGpuImage *
gsk_vulkan_image_new_for_dmabuf (GskVulkanDevice *device,
                                 gsize            width,
                                 gsize            height,
                                 const GdkDmabuf *dmabuf,
                                 gboolean         premultiplied,
                                 GskGpuConversion conv)
{
  GskVulkanImage *this;
  VkDevice vk_device;
  VkFormat vk_format;
  VkComponentMapping vk_components;
  VkSamplerYcbcrConversion vk_conversion;
  VkSamplerYcbcrModelConversion model;
  VkSamplerYcbcrRange range;
  PFN_vkGetMemoryFdPropertiesKHR func_vkGetMemoryFdPropertiesKHR;
  gsize i;
  int fd;
  VkResult res;
  GdkMemoryFormat format;
  GskGpuImageFlags flags;
  GdkShaderOp shader_op;
  gboolean is_yuv, needs_conversion;

  if (!gsk_vulkan_device_has_feature (device, GDK_VULKAN_FEATURE_DMABUF))
    {
      GDK_DEBUG (DMABUF, "Vulkan does not support dmabufs");
      return NULL;
    }

  if (gdk_memory_format_find_by_dmabuf_fourcc (dmabuf->fourcc, premultiplied, &format, &is_yuv))
    {
      vk_format = gdk_memory_format_vk_format (format, &vk_components, &needs_conversion);
    }
  else
    {
      GDK_DEBUG (DMABUF, "GTK's Vulkan doesn't support fourcc %.4s", (char *) &dmabuf->fourcc);
      return NULL;
    }

  vk_device = gsk_vulkan_device_get_vk_device (device);
  func_vkGetMemoryFdPropertiesKHR = (PFN_vkGetMemoryFdPropertiesKHR) vkGetDeviceProcAddr (vk_device, "vkGetMemoryFdPropertiesKHR");

  /* FIXME: Add support for disjoint images */
  if (gdk_dmabuf_is_disjoint (dmabuf))
    {
      GDK_DEBUG (DMABUF, "FIXME: Add support for disjoint dmabufs to Vulkan");
      return NULL;
    }

  if (!gsk_vulkan_device_supports_format (device,
                                          vk_format,
                                          dmabuf->modifier,
                                          dmabuf->n_planes,
                                          VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT,
                                          VK_IMAGE_USAGE_SAMPLED_BIT,
                                          width, height,
                                          &flags))
    {
      GDK_DEBUG (DMABUF, "Vulkan driver does not support format %.4s::%016llx with %u planes",
                 (char *) &dmabuf->fourcc, (unsigned long long) dmabuf->modifier, dmabuf->n_planes);
      return NULL;
    }

  this = g_object_new (GSK_TYPE_VULKAN_IMAGE, NULL);

  this->device = g_object_ref (device);
  this->vk_tiling = VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT;
  this->vk_format = vk_format;
  this->vk_pipeline_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  this->vk_image_layout = VK_IMAGE_LAYOUT_GENERAL;
  this->vk_access = 0;

  res  = vkCreateImage (vk_device,
                        &(VkImageCreateInfo) {
                            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                            .flags = 0, //disjoint ? VK_IMAGE_CREATE_DISJOINT_BIT : 0,
                            .imageType = VK_IMAGE_TYPE_2D,
                            .format = vk_format,
                            .extent = { width, height, 1 },
                            .mipLevels = 1,
                            .arrayLayers = 1,
                            .samples = VK_SAMPLE_COUNT_1_BIT,
                            .tiling = VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT,
                            .usage = VK_IMAGE_USAGE_SAMPLED_BIT |
                                     (flags & (GSK_GPU_IMAGE_BLIT | GSK_GPU_IMAGE_DOWNLOADABLE) ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0),
                            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                            .initialLayout = this->vk_image_layout,
                            .pNext = &(VkExternalMemoryImageCreateInfo) {
                                .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO,
                                .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
                                .pNext = &(VkImageDrmFormatModifierExplicitCreateInfoEXT) {
                                    .sType = VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_EXPLICIT_CREATE_INFO_EXT,
                                    .drmFormatModifier = dmabuf->modifier,
                                    .drmFormatModifierPlaneCount = dmabuf->n_planes,
                                    .pPlaneLayouts = (VkSubresourceLayout[4]) {
                                        {
                                            .offset = dmabuf->planes[0].offset,
                                            .rowPitch = dmabuf->planes[0].stride,
                                        },
                                        {
                                            .offset = dmabuf->planes[1].offset,
                                            .rowPitch = dmabuf->planes[1].stride,
                                        },
                                        {
                                            .offset = dmabuf->planes[2].offset,
                                            .rowPitch = dmabuf->planes[2].stride,
                                        },
                                        {
                                            .offset = dmabuf->planes[3].offset,
                                            .rowPitch = dmabuf->planes[3].stride,
                                        },
                                    },
                                }
                            },
                        },
                        NULL,
                        &this->vk_image);
  if (res != VK_SUCCESS)
    {
      GDK_DEBUG (DMABUF, "vkCreateImage() failed: %s", gdk_vulkan_strerror (res));
      return NULL;
    }

  if (needs_conversion)
    flags |= GSK_GPU_IMAGE_EXTERNAL;
  if (needs_conversion || !gsk_component_mapping_is_framebuffer_compatible (&vk_components))
    flags &= ~GSK_GPU_IMAGE_BLIT;

  if (gsk_vulkan_get_ycbcr_flags (conv, &model, &range) || needs_conversion)
    {
      this->ycbcr = gsk_vulkan_ycbcr_get (device,
                                          &(GskVulkanYcbcrInfo) {
                                              .vk_format = vk_format,
                                              .vk_components = vk_components,
                                              .vk_ycbcr_model = model,
                                              .vk_ycbcr_range = range,
                                          });
      gsk_vulkan_ycbcr_ref (this->ycbcr);
      vk_conversion = gsk_vulkan_ycbcr_get_vk_conversion (this->ycbcr);
      shader_op = GDK_SHADER_DEFAULT;
    }
  else
    {
      vk_conversion = VK_NULL_HANDLE;
      shader_op = gdk_memory_format_get_default_shader_op (format);
    }

  gsk_gpu_image_setup (GSK_GPU_IMAGE (this),
                       flags,
                       conv,
                       shader_op,
                       format,
                       width, height);

  this->allocator = gsk_vulkan_device_get_external_allocator (device);
  gsk_vulkan_allocator_ref (this->allocator);

  fd = fcntl (dmabuf->planes[0].fd, F_DUPFD_CLOEXEC, (int) 3);
  if (fd < 0)
    {
      GDK_DEBUG (DMABUF, "Vulkan failed to dup() fd: %s", g_strerror (errno));
      vkDestroyImage (vk_device, this->vk_image, NULL);
      return NULL;
    }

  for (i = 0; i < 1 /* disjoint ? dmabuf->n_planes : 1 */; i++)
    {
      VkMemoryFdPropertiesKHR fd_props = {
          .sType = VK_STRUCTURE_TYPE_MEMORY_FD_PROPERTIES_KHR,
      };
      VkMemoryRequirements2 requirements = {
          .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
      };
      gsize memory_index;

      GSK_VK_CHECK (func_vkGetMemoryFdPropertiesKHR, vk_device,
                                                     VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
                                                     fd,
                                                     &fd_props);

      vkGetImageMemoryRequirements2 (vk_device,
                                     &(VkImageMemoryRequirementsInfo2) {
                                         .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2,
                                         .image = this->vk_image,
                                         //.pNext = !disjoint ? NULL : &(VkImagePlaneMemoryRequirementsInfo) {
                                         //    .sType = VK_STRUCTURE_TYPE_IMAGE_PLANE_MEMORY_REQUIREMENTS_INFO,
                                         //    .planeAspect = aspect_flags[i]
                                         //},
                                     },
                                     &requirements);

      if (gsk_vulkan_device_has_feature (device, GDK_VULKAN_FEATURE_SEMAPHORE_IMPORT))
        {
          int sync_file_fd = gdk_dmabuf_export_sync_file (fd, DMA_BUF_SYNC_READ);
          if (sync_file_fd >= 0)
            {
              PFN_vkImportSemaphoreFdKHR func_vkImportSemaphoreFdKHR;
              func_vkImportSemaphoreFdKHR = (PFN_vkImportSemaphoreFdKHR) vkGetDeviceProcAddr (vk_device, "vkImportSemaphoreFdKHR");

              GSK_VK_CHECK (vkCreateSemaphore, vk_device,
                                               &(VkSemaphoreCreateInfo) {
                                                   .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                                               },
                                               NULL,
                                               &this->vk_semaphore);

              GSK_VK_CHECK (func_vkImportSemaphoreFdKHR, vk_device,
                                                         &(VkImportSemaphoreFdInfoKHR) {
                                                             .sType = VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_FD_INFO_KHR,
                                                             .handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_SYNC_FD_BIT,
                                                             .flags = VK_SEMAPHORE_IMPORT_TEMPORARY_BIT,
                                                             .semaphore = this->vk_semaphore,
                                                             .fd = sync_file_fd,
                                                         });
            }
        }

      memory_index = gsk_vulkan_device_find_allocator (device,
                                                       fd_props.memoryTypeBits,
                                                       0,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      gsk_vulkan_alloc (this->allocator,
                        requirements.memoryRequirements.size,
                        requirements.memoryRequirements.alignment,
                        &this->allocation);
      GSK_VK_CHECK (vkAllocateMemory, vk_device,
                                      &(VkMemoryAllocateInfo) {
                                          .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                          .allocationSize = requirements.memoryRequirements.size,
                                          .memoryTypeIndex = memory_index,
                                          .pNext = &(VkImportMemoryFdInfoKHR) {
                                              .sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR,
                                              .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
                                              .fd = fd,
                                              .pNext = &(VkMemoryDedicatedAllocateInfo) {
                                                  .sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO,
                                                  .image = this->vk_image,
                                              }
                                          }
                                      },
                                      NULL,
                                      &this->allocation.vk_memory);
    }

#if 1
  GSK_VK_CHECK (vkBindImageMemory2, gsk_vulkan_device_get_vk_device (this->device),
                                    1,
                                    &(VkBindImageMemoryInfo) {
                                        .sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
                                        .image = this->vk_image,
                                        .memory = this->allocation.vk_memory,
                                        .memoryOffset = this->allocation.offset,
                                    });
#else
  GSK_VK_CHECK (vkBindImageMemory2, gsk_vulkan_device_get_vk_device (this->device),
                                    dmabuf->n_planes,
                                    (VkBindImageMemoryInfo[4]) {
                                        {
                                            .sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
                                            .image = this->vk_image,
                                            .memory = this->allocation.vk_memory,
                                            .memoryOffset = dmabuf->planes[0].offset,
                                            .pNext = &(VkBindImagePlaneMemoryInfo) {
                                                .sType = VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO,
                                                .planeAspect = VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT,
                                            },
                                        },
                                        {
                                            .sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
                                            .image = this->vk_image,
                                            .memory = this->allocation.vk_memory,
                                            .memoryOffset = dmabuf->planes[1].offset,
                                            .pNext = &(VkBindImagePlaneMemoryInfo) {
                                                .sType = VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO,
                                                .planeAspect = VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT,
                                            },

                                        },
                                        {
                                            .sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
                                            .image = this->vk_image,
                                            .memory = this->allocation.vk_memory,
                                            .memoryOffset = dmabuf->planes[2].offset,
                                            .pNext = &(VkBindImagePlaneMemoryInfo) {
                                                .sType = VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO,
                                                .planeAspect = VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT,
                                            },
                                        },
                                        {
                                            .sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
                                            .image = this->vk_image,
                                            .memory = this->allocation.vk_memory,
                                            .memoryOffset = dmabuf->planes[3].offset,
                                            .pNext = &(VkBindImagePlaneMemoryInfo) {
                                                .sType = VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO,
                                                .planeAspect = VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT,
                                            },
                                        }
                                    });
#endif

  gsk_vulkan_image_create_view (this, vk_format, &vk_components, vk_conversion);

  GDK_DEBUG (DMABUF, "Vulkan uploaded %zux%zu %.4s:%016llx %sdmabuf",
             width, height,
             (char *) &dmabuf->fourcc, (unsigned long long) dmabuf->modifier,
             is_yuv ? "YUV " : "");

  return GSK_GPU_IMAGE (this);
}

static void
close_the_fd (gpointer the_fd)
{
  close (GPOINTER_TO_INT (the_fd));
}

static guint
gsk_vulkan_image_get_n_planes (GskVulkanImage *this,
                               guint64         modifier)
{
  VkDrmFormatModifierPropertiesEXT drm_mod_properties[100];
  VkDrmFormatModifierPropertiesListEXT drm_properties = {
    .sType = VK_STRUCTURE_TYPE_DRM_FORMAT_MODIFIER_PROPERTIES_LIST_EXT,
    .drmFormatModifierCount = G_N_ELEMENTS (drm_mod_properties),
    .pDrmFormatModifierProperties = drm_mod_properties,
  };
  VkFormatProperties2 properties = {
    .sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2,
    .pNext = &drm_properties
  };
  gsize i;

  vkGetPhysicalDeviceFormatProperties2 (gsk_vulkan_device_get_vk_physical_device (this->device),
                                        this->vk_format,
                                        &properties);
  
  for (i = 0; i < drm_properties.drmFormatModifierCount; i++)
    {
      if (drm_mod_properties[i].drmFormatModifier == modifier)
        return drm_mod_properties[i].drmFormatModifierPlaneCount;
    }

  g_return_val_if_reached (0);
}

GdkTexture *
gsk_vulkan_image_to_dmabuf_texture (GskVulkanImage *this,
                                    GdkColorState  *color_state)
{
  GskGpuImage *image = GSK_GPU_IMAGE (this);
  GdkDmabufTextureBuilder *builder;
  GError *error = NULL;
  PFN_vkGetImageDrmFormatModifierPropertiesEXT func_vkGetImageDrmFormatModifierPropertiesEXT;
  PFN_vkGetMemoryFdKHR func_vkGetMemoryFdKHR;
  VkImageDrmFormatModifierPropertiesEXT properties = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_PROPERTIES_EXT,
  };
  VkSubresourceLayout layout;
  VkDevice vk_device;
  GdkTexture *texture;
  GdkMemoryFormat format;
  VkResult res;
  guint32 fourcc;
  int fd;
  guint plane, n_planes;

  if (!(gsk_gpu_image_get_flags (image) & GSK_GPU_IMAGE_EXTERNAL))
    return FALSE;
 
  format = gsk_gpu_image_get_format (image);
  fourcc = gdk_memory_format_get_dmabuf_rgb_fourcc (format);
  if (fourcc == 0)
    fourcc = gdk_memory_format_get_dmabuf_yuv_fourcc (format);
  if (fourcc == 0)
    return FALSE;

  vk_device = gsk_vulkan_device_get_vk_device (this->device);

  func_vkGetImageDrmFormatModifierPropertiesEXT = (PFN_vkGetImageDrmFormatModifierPropertiesEXT)
    vkGetDeviceProcAddr (vk_device, "vkGetImageDrmFormatModifierPropertiesEXT");
  func_vkGetMemoryFdKHR = (PFN_vkGetMemoryFdKHR) vkGetDeviceProcAddr (vk_device, "vkGetMemoryFdKHR");
  res = GSK_VK_CHECK (func_vkGetImageDrmFormatModifierPropertiesEXT, vk_device, this->vk_image, &properties);
  if (res != VK_SUCCESS)
    return FALSE;
  n_planes = gsk_vulkan_image_get_n_planes (this, properties.drmFormatModifier);
  if (n_planes == 0 || n_planes > GDK_DMABUF_MAX_PLANES)
    return FALSE;
  res = GSK_VK_CHECK (func_vkGetMemoryFdKHR, vk_device,
                                             &(VkMemoryGetFdInfoKHR) {
                                                 .sType = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR,
                                                 .memory = this->allocation.vk_memory,
                                                 .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
                                             },
                                             &fd);
  if (res != VK_SUCCESS)
    return FALSE;

  builder = gdk_dmabuf_texture_builder_new ();
  gdk_dmabuf_texture_builder_set_display (builder, gsk_gpu_device_get_display (GSK_GPU_DEVICE (this->device)));
  gdk_dmabuf_texture_builder_set_width (builder, gsk_gpu_image_get_width (image));
  gdk_dmabuf_texture_builder_set_height (builder, gsk_gpu_image_get_height (image));
  gdk_dmabuf_texture_builder_set_fourcc (builder, fourcc);
  gdk_dmabuf_texture_builder_set_modifier (builder, properties.drmFormatModifier);
  gdk_dmabuf_texture_builder_set_premultiplied (builder, gdk_memory_format_alpha (format) != GDK_MEMORY_ALPHA_STRAIGHT);
  gdk_dmabuf_texture_builder_set_n_planes (builder, n_planes);
  gdk_dmabuf_texture_builder_set_color_state (builder, color_state);
  
  for (plane = 0; plane < n_planes; plane++)
    {
      static const VkImageAspectFlagBits aspect[GDK_DMABUF_MAX_PLANES] = {
        VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT,
        VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT,
        VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT,
        VK_IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT
      };
      vkGetImageSubresourceLayout (vk_device,
                                   this->vk_image,
                                   &(VkImageSubresource) {
                                       .aspectMask = aspect[plane],
                                       .mipLevel = 0,
                                       .arrayLayer = 0
                                   },
                                   &layout);
      gdk_dmabuf_texture_builder_set_fd (builder, plane, fd);
      gdk_dmabuf_texture_builder_set_stride (builder, plane, layout.rowPitch);
      gdk_dmabuf_texture_builder_set_offset (builder, plane, layout.offset);
    }

  texture = gdk_dmabuf_texture_builder_build (builder, close_the_fd, GINT_TO_POINTER (fd), &error);
  g_object_unref (builder);
  if (texture == NULL)
    {
      GDK_DEBUG (VULKAN, "Failed to create dmabuf texture: %s", error->message);
      g_clear_error (&error);
      close (fd);
      return NULL;
    }

  gsk_gpu_image_toggle_ref_texture (GSK_GPU_IMAGE (this), texture);

  return texture;
}
#endif

#ifdef GDK_WINDOWING_WIN32
static gboolean
gsk_vulkan_is_same_device (VkDevice           vk_device,
                           ID3D12DeviceChild *child)
{
  /* FIXME: implement */
  return TRUE;
}

GskGpuImage *
gsk_vulkan_image_new_for_d3d12resource (GskVulkanDevice *device,
                                        ID3D12Resource  *resource,
                                        HANDLE           resource_handle,
                                        ID3D12Fence     *fence,
                                        HANDLE           fence_handle,
                                        guint64          fence_wait,
                                        gboolean         premultiplied)
{
  GskVulkanImage *this;
  VkDevice vk_device;
  VkFormat vk_format;
  VkComponentMapping vk_components;
  VkSamplerYcbcrConversion vk_conversion;
  PFN_vkGetMemoryWin32HandlePropertiesKHR func_vkGetMemoryWin32HandlePropertiesKHR;
  VkResult res;
  GdkMemoryFormat format;
  GdkShaderOp shader_op;
  GskGpuImageFlags flags;
  D3D12_RESOURCE_DESC desc;
  gsize memory_index;
  gboolean needs_conversion;
  VkMemoryRequirements2 requirements = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
  };
  VkMemoryWin32HandlePropertiesKHR handle_properties = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_WIN32_HANDLE_PROPERTIES_KHR,
  };

  if (!gsk_vulkan_device_has_feature (device, GDK_VULKAN_FEATURE_WIN32))
    {
      GDK_DEBUG (D3D12, "Vulkan does not support D3D12Resource import");
      return NULL;
    }

  vk_device = gsk_vulkan_device_get_vk_device (device);
  func_vkGetMemoryWin32HandlePropertiesKHR = (PFN_vkGetMemoryWin32HandlePropertiesKHR) vkGetDeviceProcAddr (vk_device, "vkGetMemoryWin32HandlePropertiesKHR");
  ID3D12Resource_GetDesc (resource, &desc);

  if (!gsk_vulkan_is_same_device (vk_device, (ID3D12DeviceChild *) resource))
    {
      GDK_DEBUG (D3D12, "Resource is from a different device");
      return NULL;
    }

  if (!gdk_memory_format_find_by_dxgi_format (desc.Format, premultiplied, &format))
    {
      GDK_DEBUG (D3D12, "Unsupported DXGI format %u", desc.Format);
      return NULL;
    }

  vk_format = gdk_memory_format_vk_format (format, &vk_components, &needs_conversion);
  if (vk_format == VK_FORMAT_UNDEFINED)
    {
      GDK_DEBUG (D3D12, "GTK's Vulkan doesn't support DXGI format %u", desc.Format);
      return NULL;
    }

  if (needs_conversion && !gsk_vulkan_device_has_feature (device, GDK_VULKAN_FEATURE_YCBCR))
    {
      GDK_DEBUG (D3D12, "Vulkan driver cannot import DXGI format %u because it lacks YCbCr support", desc.Format);
      return NULL;
    }

  if (!gsk_vulkan_device_supports_format (device,
                                          vk_format,
                                          0, 1,
                                          VK_IMAGE_TILING_OPTIMAL,
                                          VK_IMAGE_USAGE_SAMPLED_BIT,
                                          desc.Width, desc.Height,
                                          &flags))
    {
      GDK_DEBUG (D3D12, "Vulkan driver does not support DXGI format %u", desc.Format);
      return NULL;
    }

  if (needs_conversion)
    shader_op = GDK_SHADER_DEFAULT;
  else
    shader_op = gdk_memory_format_get_default_shader_op (format);

  flags &= ~(GSK_GPU_IMAGE_CAN_MIPMAP | GSK_GPU_IMAGE_RENDERABLE);
  if (desc.MipLevels)
    flags |= GSK_GPU_IMAGE_CAN_MIPMAP | GSK_GPU_IMAGE_MIPMAP;

  this = g_object_new (GSK_TYPE_VULKAN_IMAGE, NULL);

  this->device = g_object_ref (device);
  this->vk_tiling = VK_IMAGE_TILING_OPTIMAL;
  this->vk_format = vk_format;
  this->vk_pipeline_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  this->vk_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
  this->vk_access = 0;

  res  = vkCreateImage (vk_device,
                        &(VkImageCreateInfo) {
                            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                            .flags = 0,
                            .imageType = VK_IMAGE_TYPE_2D,
                            .format = vk_format,
                            .extent = { desc.Width, desc.Height, 1 },
                            .mipLevels = desc.MipLevels,
                            .arrayLayers = 1,
                            .samples = VK_SAMPLE_COUNT_1_BIT,
                            .tiling = this->vk_tiling,
                            .usage = VK_IMAGE_USAGE_SAMPLED_BIT |
                                     (flags & (GSK_GPU_IMAGE_BLIT | GSK_GPU_IMAGE_DOWNLOADABLE) ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0),
                            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                            .initialLayout = this->vk_image_layout,
                            .pNext = &(VkExternalMemoryImageCreateInfo) {
                                .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO,
                                .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE_BIT,
                            },
                        },
                        NULL,
                        &this->vk_image);
  if (res != VK_SUCCESS)
    {
      GDK_DEBUG (D3D12, "vkCreateImage() failed: %s", gdk_vulkan_strerror (res));
      return NULL;
    }

  gsk_gpu_image_setup (GSK_GPU_IMAGE (this),
                       flags |
                       (needs_conversion ? GSK_GPU_IMAGE_EXTERNAL : 0) |
                       (desc.MipLevels > 1 ? GSK_GPU_IMAGE_CAN_MIPMAP | GSK_GPU_IMAGE_MIPMAP : 0),
                       shader_op,
                       GSK_GPU_CONVERSION_NONE,
                       format,
                       desc.Width, desc.Height);

  vkGetImageMemoryRequirements2 (vk_device,
                                 &(VkImageMemoryRequirementsInfo2) {
                                     .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2,
                                     .image = this->vk_image,
                                 },
                                 &requirements);
	// Vulkan memory import
	GSK_VK_CHECK (func_vkGetMemoryWin32HandlePropertiesKHR, vk_device,
                                                          VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE_BIT,
                                                          resource_handle,
                                                          &handle_properties);

  memory_index = gsk_vulkan_device_find_allocator (device,
                                                   handle_properties.memoryTypeBits,
                                                   0,
                                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  this->allocator = gsk_vulkan_device_get_external_allocator (device);
  gsk_vulkan_allocator_ref (this->allocator);

  gsk_vulkan_alloc (this->allocator,
                    requirements.memoryRequirements.size,
                    requirements.memoryRequirements.alignment,
                    &this->allocation);

  GSK_VK_CHECK (vkAllocateMemory, vk_device,
                                  &(VkMemoryAllocateInfo) {
                                      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                      .allocationSize = requirements.memoryRequirements.size,
                                      .memoryTypeIndex = memory_index,
                                      .pNext = &(VkImportMemoryWin32HandleInfoKHR) {
                                          .sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR,
                                          .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE_BIT,
                                          .handle = resource_handle,
                                          .pNext = &(VkMemoryDedicatedAllocateInfo) {
                                              .sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO,
                                              .image = this->vk_image,
                                          }
                                      }
                                  },
                                  NULL,
                                  &this->allocation.vk_memory);

  GSK_VK_CHECK (vkBindImageMemory2, gsk_vulkan_device_get_vk_device (this->device),
                                    1,
                                    &(VkBindImageMemoryInfo) {
                                        .sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
                                        .image = this->vk_image,
                                        .memory = this->allocation.vk_memory,
                                        .memoryOffset = this->allocation.offset,
                                    });

  if (gsk_vulkan_device_has_feature (device, GDK_VULKAN_FEATURE_WIN32_SEMAPHORE) && fence)
    {
      PFN_vkImportSemaphoreWin32HandleKHR func_vkImportSemaphoreWin32HandleKHR;
      func_vkImportSemaphoreWin32HandleKHR = (PFN_vkImportSemaphoreWin32HandleKHR) vkGetDeviceProcAddr (vk_device, "vkImportSemaphoreWin32HandleKHR");

      GSK_VK_CHECK (vkCreateSemaphore, vk_device,
                                        &(VkSemaphoreCreateInfo) {
                                            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                                            .pNext = &(VkSemaphoreTypeCreateInfo) {
                                                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
                                                .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
                                            },
                                        },
                                        NULL,
                                        &this->vk_semaphore);

      GSK_VK_CHECK (func_vkImportSemaphoreWin32HandleKHR, vk_device,
                                                          &(VkImportSemaphoreWin32HandleInfoKHR) {
                                                              .sType = VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_WIN32_HANDLE_INFO_KHR,
                                                              .handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D12_FENCE_BIT,
                                                              .semaphore = this->vk_semaphore,
                                                              .handle = fence_handle,
                                                          });
      this->vk_semaphore_wait = fence_wait;
    }

  if (needs_conversion)
    {
      this->ycbcr = gsk_vulkan_ycbcr_get (device,
                                          &(GskVulkanYcbcrInfo) {
                                              .vk_format = vk_format,
                                              .vk_components = vk_components,
                                              .vk_ycbcr_model = VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY,
                                              .vk_ycbcr_range = VK_SAMPLER_YCBCR_RANGE_ITU_FULL,
                                          });
      gsk_vulkan_ycbcr_ref (this->ycbcr);
      vk_conversion = gsk_vulkan_ycbcr_get_vk_conversion (this->ycbcr);
    }
  else
    vk_conversion = VK_NULL_HANDLE;

  gsk_vulkan_image_create_view (this,
                                vk_format,
                                &vk_components,
                                vk_conversion);

  GDK_DEBUG (D3D12, "Vulkan uploaded %ux%u resource of %sformat %u",
             (guint) desc.Width, (guint) desc.Height,
             needs_conversion ? "YUV " : "",
             desc.Format);

  return GSK_GPU_IMAGE (this);
}
#endif

static void
gsk_vulkan_image_get_projection_matrix (GskGpuImage       *image,
                                        graphene_matrix_t *out_projection)
{
  graphene_matrix_t scale_z;

  GSK_GPU_IMAGE_CLASS (gsk_vulkan_image_parent_class)->get_projection_matrix (image, out_projection);

  graphene_matrix_init_from_float (&scale_z,
                                   (float[16]) {
                                       1,   0,   0,   0,
                                       0,   1,   0,   0,
                                       0,   0, 0.5,   0,
                                       0,   0, 0.5,   1
                                   });

  graphene_matrix_multiply (out_projection, &scale_z, out_projection);
}

static void
gsk_vulkan_image_finalize (GObject *object)
{
  GskVulkanImage *this = GSK_VULKAN_IMAGE (object);
  VkDevice vk_device;
  gsize i;

  vk_device = gsk_vulkan_device_get_vk_device (this->device);

  g_clear_pointer (&this->ycbcr, gsk_vulkan_ycbcr_unref);

  for (i = 0; i < GSK_GPU_SAMPLER_N_SAMPLERS; i++)
    {
      if (this->descriptor_sets[i].vk_descriptor_set)
        gsk_vulkan_device_free_descriptor (this->device,
                                           this->descriptor_sets[i].pool_id,
                                           this->descriptor_sets[i].vk_descriptor_set);
    }

  if (this->vk_framebuffer != VK_NULL_HANDLE)
    vkDestroyFramebuffer (vk_device, this->vk_framebuffer, NULL);

  if (this->vk_framebuffer_image_view != VK_NULL_HANDLE &&
      this->vk_framebuffer_image_view != this->vk_image_view)
    vkDestroyImageView (vk_device, this->vk_framebuffer_image_view, NULL);

  if (this->vk_image_view != VK_NULL_HANDLE)
    vkDestroyImageView (vk_device, this->vk_image_view, NULL);

  if (this->vk_semaphore != VK_NULL_HANDLE)
    vkDestroySemaphore (vk_device, this->vk_semaphore, NULL);

  /* memory is NULL for for_swapchain() images, where we don't own
   * the VkImage */
  if (this->allocator)
    {
      vkDestroyImage (vk_device, this->vk_image, NULL);
      gsk_vulkan_free (this->allocator, &this->allocation);
      gsk_vulkan_allocator_unref (this->allocator);
    }

  g_object_unref (this->device);

  G_OBJECT_CLASS (gsk_vulkan_image_parent_class)->finalize (object);
}

static void
gsk_vulkan_image_class_init (GskVulkanImageClass *klass)
{
  GskGpuImageClass *image_class = GSK_GPU_IMAGE_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  image_class->get_projection_matrix = gsk_vulkan_image_get_projection_matrix;

  object_class->finalize = gsk_vulkan_image_finalize;
}

static void
gsk_vulkan_image_init (GskVulkanImage *this)
{
}

VkFramebuffer
gsk_vulkan_image_get_vk_framebuffer (GskVulkanImage *this,
                                     VkRenderPass    render_pass)
{
  VkDevice vk_device;

  if (this->vk_framebuffer)
    return this->vk_framebuffer;

  vk_device = gsk_vulkan_device_get_vk_device (this->device);

  if (gsk_gpu_image_get_flags (GSK_GPU_IMAGE (this)) & GSK_GPU_IMAGE_CAN_MIPMAP)
    {
      GSK_VK_CHECK (vkCreateImageView, vk_device,
                                     &(VkImageViewCreateInfo) {
                                         .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                         .image = this->vk_image,
                                         .viewType = VK_IMAGE_VIEW_TYPE_2D,
                                         .format = this->vk_format,
                                         .subresourceRange = {
                                             .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                             .baseMipLevel = 0,
                                             .levelCount = 1,
                                             .baseArrayLayer = 0,
                                             .layerCount = 1,
                                         }
                                     },
                                 NULL,
                                 &this->vk_framebuffer_image_view);
    }
  else
    {
      this->vk_framebuffer_image_view = this->vk_image_view;
    }

  GSK_VK_CHECK (vkCreateFramebuffer, vk_device,
                                     &(VkFramebufferCreateInfo) {
                                         .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                                         .renderPass = render_pass,
                                         .attachmentCount = 1,
                                         .pAttachments = (VkImageView[1]) {
                                             this->vk_framebuffer_image_view,
                                         },
                                         .width = gsk_gpu_image_get_width (GSK_GPU_IMAGE (this)),
                                         .height = gsk_gpu_image_get_height (GSK_GPU_IMAGE (this)),
                                         .layers = 1
                                     },
                                     NULL,
                                     &this->vk_framebuffer);

  return this->vk_framebuffer;
}

VkDescriptorSet
gsk_vulkan_image_get_vk_descriptor_set (GskVulkanImage *this,
                                        GskGpuSampler   sampler)
{
  if (!this->descriptor_sets[sampler].vk_descriptor_set)
    {
      this->descriptor_sets[sampler].vk_descriptor_set =
        gsk_vulkan_device_allocate_descriptor (this->device,
                                               this->ycbcr ? gsk_vulkan_ycbcr_get_vk_descriptor_set_layout (this->ycbcr)
                                                           : gsk_vulkan_device_get_vk_image_set_layout (this->device),
                                               &this->descriptor_sets[sampler].pool_id);

      vkUpdateDescriptorSets (gsk_vulkan_device_get_vk_device (this->device),
                              1,
                              &(VkWriteDescriptorSet) {
                                  .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                  .dstSet = this->descriptor_sets[sampler].vk_descriptor_set,
                                  .dstBinding = 0,
                                  .dstArrayElement = 0,
                                  .descriptorCount = 1,
                                  .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                  .pImageInfo = &(VkDescriptorImageInfo) {
                                      .sampler = this->ycbcr ? gsk_vulkan_ycbcr_get_vk_sampler (this->ycbcr)
                                                             : gsk_vulkan_device_get_vk_sampler (this->device, sampler),
                                      .imageView = this->vk_image_view,
                                      .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                                  },
                              },
                              0,
                              NULL);
    }

  return this->descriptor_sets[sampler].vk_descriptor_set;
}

GskVulkanYcbcr *
gsk_vulkan_image_get_ycbcr (GskVulkanImage *this)
{
  return this->ycbcr;
}

VkImage
gsk_vulkan_image_get_vk_image (GskVulkanImage *this)
{
  return this->vk_image;
}

VkImageView
gsk_vulkan_image_get_vk_image_view (GskVulkanImage *this)
{
  return this->vk_image_view;
}

VkPipelineStageFlags
gsk_vulkan_image_get_vk_pipeline_stage (GskVulkanImage *this)
{
  return this->vk_pipeline_stage;
}

VkImageLayout
gsk_vulkan_image_get_vk_image_layout (GskVulkanImage *this)
{
  return this->vk_image_layout;
}

VkAccessFlags
gsk_vulkan_image_get_vk_access (GskVulkanImage *this)
{
  return this->vk_access;
}

void
gsk_vulkan_image_set_vk_image_layout (GskVulkanImage       *this,
                                      VkPipelineStageFlags  stage,
                                      VkImageLayout         image_layout,
                                      VkAccessFlags         access)
{
  this->vk_pipeline_stage = stage;
  this->vk_image_layout = image_layout;
  this->vk_access = access;
}

void
gsk_vulkan_image_transition (GskVulkanImage       *this,
                             GskVulkanSemaphores  *semaphores,
                             VkCommandBuffer       command_buffer,
                             VkPipelineStageFlags  stage,
                             VkImageLayout         image_layout,
                             VkAccessFlags         access)
{
  if (this->vk_pipeline_stage == stage &&
      this->vk_image_layout == image_layout &&
      this->vk_access == access)
    return;

  if (this->vk_pipeline_stage == VK_IMAGE_LAYOUT_GENERAL &&
      this->vk_semaphore)
    {
      gsk_vulkan_semaphores_add_wait (semaphores,
                                      this->vk_semaphore,
                                      this->vk_semaphore_wait,
                                      stage);
    }

  vkCmdPipelineBarrier (command_buffer,
                        this->vk_pipeline_stage,
                        stage,
                        0,
                        0, NULL,
                        0, NULL,
                        1, &(VkImageMemoryBarrier) {
                            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                            .srcAccessMask = this->vk_access,
                            .dstAccessMask = access,
                            .oldLayout = this->vk_image_layout,
                            .newLayout = image_layout,
                            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                            .image = this->vk_image,
                            .subresourceRange = {
                              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                              .baseMipLevel = 0,
                              .levelCount = VK_REMAINING_MIP_LEVELS,
                              .baseArrayLayer = 0,
                              .layerCount = 1
                            },
                        });

  gsk_vulkan_image_set_vk_image_layout (this, stage, image_layout, access);
}

VkFormat
gsk_vulkan_image_get_vk_format (GskVulkanImage *this)
{
  return this->vk_format;
}

