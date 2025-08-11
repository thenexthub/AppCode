#pragma once

#include "gskgpudeviceprivate.h"

#include "gskdebugprivate.h"
#include "gskgpuclipprivate.h"
#include "gskvulkanmemoryprivate.h"

#include "gdk/gdkdisplayprivate.h"
#include "gdk/gdkvulkancontextprivate.h"

G_BEGIN_DECLS

/* forward declaration */
typedef struct _GskVulkanYcbcr GskVulkanYcbcr;
typedef struct _GskVulkanYcbcrInfo GskVulkanYcbcrInfo;

#define GSK_TYPE_VULKAN_DEVICE (gsk_vulkan_device_get_type ())

G_DECLARE_FINAL_TYPE(GskVulkanDevice, gsk_vulkan_device, GSK, VULKAN_DEVICE, GskGpuDevice)

GskGpuDevice *          gsk_vulkan_device_get_for_display               (GdkDisplay             *display,
                                                                         GError                **error);

gboolean                gsk_vulkan_device_has_feature                   (GskVulkanDevice        *this,
                                                                         GdkVulkanFeatures       feature) G_GNUC_PURE;

VkDevice                gsk_vulkan_device_get_vk_device                 (GskVulkanDevice        *this) G_GNUC_PURE;
VkPhysicalDevice        gsk_vulkan_device_get_vk_physical_device        (GskVulkanDevice        *this) G_GNUC_PURE;
VkQueue                 gsk_vulkan_device_get_vk_queue                  (GskVulkanDevice        *this) G_GNUC_PURE;
uint32_t                gsk_vulkan_device_get_vk_queue_family_index     (GskVulkanDevice        *this) G_GNUC_PURE;
VkCommandPool           gsk_vulkan_device_get_vk_command_pool           (GskVulkanDevice        *this) G_GNUC_PURE;
VkDescriptorSet         gsk_vulkan_device_allocate_descriptor           (GskVulkanDevice        *this,
                                                                         const VkDescriptorSetLayout layout,
                                                                         gsize                  *out_pool_id);
void                    gsk_vulkan_device_free_descriptor               (GskVulkanDevice        *this,
                                                                         gsize                   pool_id,
                                                                         VkDescriptorSet         set);
VkDescriptorSetLayout   gsk_vulkan_device_get_vk_image_set_layout       (GskVulkanDevice        *this) G_GNUC_PURE;
VkPipelineLayout        gsk_vulkan_device_create_vk_pipeline_layout     (GskVulkanDevice        *this,
                                                                         VkDescriptorSetLayout   image1_layout,
                                                                         VkDescriptorSetLayout   image2_layout);
VkPipelineLayout        gsk_vulkan_device_get_default_vk_pipeline_layout (GskVulkanDevice       *this) G_GNUC_PURE;
VkPipelineLayout        gsk_vulkan_device_get_vk_pipeline_layout        (GskVulkanDevice        *this,
                                                                         GskVulkanYcbcr         *ycbcr0,
                                                                         GskVulkanYcbcr         *ycbcr1);
VkSampler               gsk_vulkan_device_get_vk_sampler                (GskVulkanDevice        *this,
                                                                         GskGpuSampler           sampler) G_GNUC_PURE;

VkRenderPass            gsk_vulkan_device_get_vk_render_pass            (GskVulkanDevice        *this,
                                                                         VkFormat                format,
                                                                         VkAttachmentLoadOp      vk_load_op,
                                                                         VkImageLayout           from_layout,
                                                                         VkImageLayout           to_layout);
VkPipeline              gsk_vulkan_device_get_vk_pipeline               (GskVulkanDevice        *this,
                                                                         VkPipelineLayout        layout,
                                                                         const GskGpuShaderOpClass *op_class,
                                                                         GskGpuShaderFlags       flags,
                                                                         GskGpuColorStates       color_states,
                                                                         guint32                 variation,
                                                                         GskGpuBlend             blend,
                                                                         VkFormat                vk_format,
                                                                         VkRenderPass            render_pass);

GskVulkanAllocator *    gsk_vulkan_device_get_external_allocator        (GskVulkanDevice        *this);
GskVulkanAllocator *    gsk_vulkan_device_get_allocator                 (GskVulkanDevice        *this,
                                                                         gsize                   index);

gsize                   gsk_vulkan_device_find_allocator                (GskVulkanDevice        *this,
                                                                         uint32_t                allowed_types,
                                                                         VkMemoryPropertyFlags   required_flags,
                                                                         VkMemoryPropertyFlags   desired_flags);

static inline VkResult
gsk_vulkan_handle_result (VkResult    res,
                          const char *called_function)
{
  if (res != VK_SUCCESS)
    {
      g_warning ("%s(): %s (%d)", called_function, gdk_vulkan_strerror (res), res);
    }
  return res;
}

#define GSK_VK_CHECK(fn, ...) gsk_vulkan_handle_result (fn (__VA_ARGS__), G_STRINGIFY (fn))


G_END_DECLS
