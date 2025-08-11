#include "config.h"

#include "gskvulkanbufferprivate.h"

#include "gskvulkandeviceprivate.h"
#include "gskvulkanmemoryprivate.h"

struct _GskVulkanBuffer
{
  GskGpuBuffer parent_instance;

  GskVulkanDevice *device;

  VkBuffer vk_buffer;

  GskVulkanAllocator *allocator;
  GskVulkanAllocation allocation;
};

G_DEFINE_TYPE (GskVulkanBuffer, gsk_vulkan_buffer, GSK_TYPE_GPU_BUFFER)

static void
gsk_vulkan_buffer_finalize (GObject *object)
{
  GskVulkanBuffer *this = GSK_VULKAN_BUFFER (object);

  vkDestroyBuffer (gsk_vulkan_device_get_vk_device (this->device),
                   this->vk_buffer,
                   NULL);

  gsk_vulkan_free (this->allocator, &this->allocation);
  gsk_vulkan_allocator_unref (this->allocator);

  g_object_unref (this->device);

  G_OBJECT_CLASS (gsk_vulkan_buffer_parent_class)->finalize (object);
}

static guchar *
gsk_vulkan_buffer_map (GskGpuBuffer *buffer)
{
  GskVulkanBuffer *this = GSK_VULKAN_BUFFER (buffer);

  return this->allocation.map;
}

static void
gsk_vulkan_buffer_unmap (GskGpuBuffer *buffer,
                         gsize         size)
{
}

static void
gsk_vulkan_buffer_class_init (GskVulkanBufferClass *klass)
{
  GskGpuBufferClass *buffer_class = GSK_GPU_BUFFER_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  buffer_class->map = gsk_vulkan_buffer_map;
  buffer_class->unmap = gsk_vulkan_buffer_unmap;

  gobject_class->finalize = gsk_vulkan_buffer_finalize;
}

static void
gsk_vulkan_buffer_init (GskVulkanBuffer *this)
{
}

static GskGpuBuffer *
gsk_vulkan_buffer_new_internal (GskVulkanDevice   *device,
                                gsize              size,
                                VkBufferUsageFlags usage)
{
  VkMemoryRequirements requirements;
  GskVulkanBuffer *this;
  gsize memory_index;

  this = g_object_new (GSK_TYPE_VULKAN_BUFFER, NULL);

  this->device = g_object_ref (device);

  GSK_VK_CHECK (vkCreateBuffer, gsk_vulkan_device_get_vk_device (device),
                                &(VkBufferCreateInfo) {
                                    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                    .size = size,
                                    .flags = 0,
                                    .usage = usage,
                                    .sharingMode = VK_SHARING_MODE_EXCLUSIVE
                                },
                                NULL,
                                &this->vk_buffer);

  vkGetBufferMemoryRequirements (gsk_vulkan_device_get_vk_device (device),
                                 this->vk_buffer,
                                 &requirements);
  
  memory_index = gsk_vulkan_device_find_allocator (device,
                                                   requirements.memoryTypeBits,
                                                   GSK_VULKAN_MEMORY_MAPPABLE,
                                                   GSK_VULKAN_MEMORY_MAPPABLE |
                                                   VK_MEMORY_PROPERTY_HOST_CACHED_BIT);
  this->allocator = gsk_vulkan_device_get_allocator (device, memory_index);
  gsk_vulkan_allocator_ref (this->allocator);

  gsk_vulkan_alloc (this->allocator,
                    requirements.size,
                    requirements.alignment,
                    &this->allocation);

  gsk_gpu_buffer_setup (GSK_GPU_BUFFER (this), this->allocation.size);

  GSK_VK_CHECK (vkBindBufferMemory, gsk_vulkan_device_get_vk_device (device),
                                    this->vk_buffer,
                                    this->allocation.vk_memory,
                                    this->allocation.offset);

  return GSK_GPU_BUFFER (this);
}

GskGpuBuffer *
gsk_vulkan_buffer_new_vertex (GskVulkanDevice *device,
                              gsize            size)
{
  return gsk_vulkan_buffer_new_internal (device, size,
                                         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
                                         | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

GskGpuBuffer *
gsk_vulkan_buffer_new_storage (GskVulkanDevice *device,
                               gsize            size)
{
  return gsk_vulkan_buffer_new_internal (device, size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
}

GskGpuBuffer *
gsk_vulkan_buffer_new_write (GskVulkanDevice *device,
                             gsize            size)
{
  return gsk_vulkan_buffer_new_internal (device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
}

GskGpuBuffer *
gsk_vulkan_buffer_new_read (GskVulkanDevice *device,
                            gsize            size)
{
  return gsk_vulkan_buffer_new_internal (device, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
}

VkBuffer
gsk_vulkan_buffer_get_vk_buffer (GskVulkanBuffer *this)
{
  return this->vk_buffer;
}

