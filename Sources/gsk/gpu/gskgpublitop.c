#include "config.h"

#include "gskgpublitopprivate.h"

#include "gskglimageprivate.h"
#include "gskgpuprintprivate.h"
#ifdef GDK_RENDERING_VULKAN
#include "gskvulkanimageprivate.h"
#endif

typedef struct _GskGpuBlitOp GskGpuBlitOp;

struct _GskGpuBlitOp
{
  GskGpuOp op;

  GskGpuImage *src_image;
  GskGpuImage *dest_image;
  cairo_rectangle_int_t src_rect;
  cairo_rectangle_int_t dest_rect;
  GskGpuBlitFilter filter;
};

static void
gsk_gpu_blit_op_finish (GskGpuOp *op)
{
  GskGpuBlitOp *this = (GskGpuBlitOp *) op;

  g_object_unref (this->src_image);
  g_object_unref (this->dest_image);
}

static void
gsk_gpu_blit_op_print (GskGpuOp    *op,
                       GskGpuFrame *frame,
                       GString     *string,
                       guint        indent)
{
  GskGpuBlitOp *this = (GskGpuBlitOp *) op;

  gsk_gpu_print_op (string, indent, "blit");
  gsk_gpu_print_int_rect (string, &this->dest_rect);
  gsk_gpu_print_newline (string);
}

#ifdef GDK_RENDERING_VULKAN
static GskGpuOp *
gsk_gpu_blit_op_vk_command (GskGpuOp              *op,
                            GskGpuFrame           *frame,
                            GskVulkanCommandState *state)
{
  GskGpuBlitOp *this = (GskGpuBlitOp *) op;
  VkImageLayout src_layout, dest_layout;
  VkFilter filter;

  src_layout = gsk_vulkan_image_get_vk_image_layout (GSK_VULKAN_IMAGE (this->src_image));
  if (src_layout != VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR &&
      src_layout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL &&
      src_layout != VK_IMAGE_LAYOUT_GENERAL)
    {
      gsk_vulkan_image_transition (GSK_VULKAN_IMAGE (this->src_image),
                                   state->semaphores,
                                   state->vk_command_buffer,
                                   VK_PIPELINE_STAGE_TRANSFER_BIT,
                                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                   VK_ACCESS_TRANSFER_READ_BIT);
      src_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    }

  dest_layout = gsk_vulkan_image_get_vk_image_layout (GSK_VULKAN_IMAGE (this->dest_image));
  if (dest_layout != VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR &&
      dest_layout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
      dest_layout != VK_IMAGE_LAYOUT_GENERAL)
    {
      gsk_vulkan_image_transition (GSK_VULKAN_IMAGE (this->dest_image),
                                   state->semaphores,
                                   state->vk_command_buffer,
                                   VK_PIPELINE_STAGE_TRANSFER_BIT,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   VK_ACCESS_TRANSFER_WRITE_BIT);
      dest_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    }
  
  switch (this->filter)
    {
      default:
        g_assert_not_reached ();
      case GSK_GPU_BLIT_LINEAR:
        filter = VK_FILTER_LINEAR;
        break;

      case GSK_GPU_BLIT_NEAREST:
      filter = VK_FILTER_NEAREST;
      break;
    }

  vkCmdBlitImage (state->vk_command_buffer,
                  gsk_vulkan_image_get_vk_image (GSK_VULKAN_IMAGE (this->src_image)),
                  src_layout,
                  gsk_vulkan_image_get_vk_image (GSK_VULKAN_IMAGE (this->dest_image)),
                  dest_layout,
                  1,
                  &(VkImageBlit) {
                      .srcSubresource = {
                          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                          .mipLevel = 0,
                          .baseArrayLayer = 0,
                          .layerCount = 1
                      },
                      .srcOffsets = {
                          {
                              .x = this->src_rect.x,
                              .y = this->src_rect.y,
                              .z = 0,
                          },
                          {
                              .x = this->src_rect.x + this->src_rect.width,
                              .y = this->src_rect.y + this->src_rect.height,
                              .z = 1
                          }
                      },
                      .dstSubresource = {
                          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                          .mipLevel = 0,
                          .baseArrayLayer = 0,
                          .layerCount = 1
                      },
                      .dstOffsets = {
                          {
                              .x = this->dest_rect.x,
                              .y = this->dest_rect.y,
                              .z = 0,
                          },
                          {
                              .x = this->dest_rect.x + this->dest_rect.width,
                              .y = this->dest_rect.y + this->dest_rect.height,
                              .z = 1,
                          }
                      },
                  },
                  filter);

  return op->next;
}
#endif

static GskGpuOp *
gsk_gpu_blit_op_gl_command (GskGpuOp          *op,
                            GskGpuFrame       *frame,
                            GskGLCommandState *state)
{
  GskGpuBlitOp *this = (GskGpuBlitOp *) op;
  GLenum filter;

  gsk_gl_image_bind_framebuffer_target (GSK_GL_IMAGE (this->src_image), GL_READ_FRAMEBUFFER);
  gsk_gl_image_bind_framebuffer_target (GSK_GL_IMAGE (this->dest_image), GL_DRAW_FRAMEBUFFER);

  switch (this->filter)
    {
      default:
        g_assert_not_reached ();
      case GSK_GPU_BLIT_LINEAR:
        filter = GL_LINEAR;
        break;

      case GSK_GPU_BLIT_NEAREST:
        filter = GL_NEAREST;
        break;
    }

  glDisable (GL_SCISSOR_TEST);
  glBlitFramebuffer (this->src_rect.x,
                     this->src_rect.y,
                     this->src_rect.x + this->src_rect.width,
                     this->src_rect.y + this->src_rect.height,
                     this->dest_rect.x,
                     state->flip_y ? state->flip_y - this->dest_rect.y - this->dest_rect.height
                            : this->dest_rect.y,
                     this->dest_rect.x + this->dest_rect.width,
                     state->flip_y ? state->flip_y - this->dest_rect.y
                            : this->dest_rect.y + this->dest_rect.height,
                     GL_COLOR_BUFFER_BIT,
                     filter);
  glEnable (GL_SCISSOR_TEST);

  return op->next;
}

static const GskGpuOpClass GSK_GPU_BLIT_OP_CLASS = {
  GSK_GPU_OP_SIZE (GskGpuBlitOp),
  GSK_GPU_STAGE_PASS,
  gsk_gpu_blit_op_finish,
  gsk_gpu_blit_op_print,
#ifdef GDK_RENDERING_VULKAN
  gsk_gpu_blit_op_vk_command,
#endif
  gsk_gpu_blit_op_gl_command
};

void
gsk_gpu_blit_op (GskGpuFrame                 *frame,
                 GskGpuImage                 *src_image,
                 GskGpuImage                 *dest_image,
                 const cairo_rectangle_int_t *src_rect,
                 const cairo_rectangle_int_t *dest_rect,
                 GskGpuBlitFilter             filter)
{
  GskGpuBlitOp *this;

  g_assert (gsk_gpu_image_get_flags (src_image) & GSK_GPU_IMAGE_BLIT);
  g_assert (filter != GSK_GPU_BLIT_LINEAR || (gsk_gpu_image_get_flags (src_image) & GSK_GPU_IMAGE_FILTERABLE) == GSK_GPU_IMAGE_FILTERABLE);
  g_assert ((gsk_gpu_image_get_flags (dest_image) & GSK_GPU_IMAGE_RENDERABLE) == GSK_GPU_IMAGE_RENDERABLE);

  this = (GskGpuBlitOp *) gsk_gpu_op_alloc (frame, &GSK_GPU_BLIT_OP_CLASS);

  this->src_image = g_object_ref (src_image);
  this->dest_image = g_object_ref (dest_image);
  this->src_rect = *src_rect;
  this->dest_rect = *dest_rect;
  this->filter = filter;
}
