#pragma once

#include "gskvulkandeviceprivate.h"

#include "gdk/gdkvulkancontextprivate.h"

G_BEGIN_DECLS

typedef struct _GskVulkanYcbcrInfo GskVulkanYcbcrInfo;

struct _GskVulkanYcbcrInfo
{
  VkFormat vk_format;
  VkComponentMapping vk_components;
  VkSamplerYcbcrModelConversion vk_ycbcr_model;
  VkSamplerYcbcrRange vk_ycbcr_range;
};


void                            gsk_vulkan_ycbcr_init_cache                     (GskGpuCache                    *cache);
void                            gsk_vulkan_ycbcr_finish_cache                   (GskGpuCache                    *cache);

GskVulkanYcbcr *                gsk_vulkan_ycbcr_get                            (GskVulkanDevice                *this,
                                                                                 const GskVulkanYcbcrInfo       *info);
                                                                         
GskVulkanYcbcr *                gsk_vulkan_ycbcr_ref                            (GskVulkanYcbcr                 *this);
void                            gsk_vulkan_ycbcr_unref                          (GskVulkanYcbcr                 *this);

VkSamplerYcbcrConversion        gsk_vulkan_ycbcr_get_vk_conversion              (GskVulkanYcbcr                 *this);
VkSampler                       gsk_vulkan_ycbcr_get_vk_sampler                 (GskVulkanYcbcr                 *this);
VkDescriptorSetLayout           gsk_vulkan_ycbcr_get_vk_descriptor_set_layout   (GskVulkanYcbcr                 *this);
VkPipelineLayout                gsk_vulkan_ycbcr_get_vk_pipeline_layout         (GskVulkanYcbcr                 *this,
                                                                                 gsize                           id);


G_END_DECLS
