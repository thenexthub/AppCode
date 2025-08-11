#include "config.h"

#include "gskgpuclearopprivate.h"

#include "gskgpuopprivate.h"
#include "gskgpuprintprivate.h"

#include "gdk/gdkcolorstateprivate.h"

typedef struct _GskGpuClearOp GskGpuClearOp;

struct _GskGpuClearOp
{
  GskGpuOp op;

  cairo_rectangle_int_t rect;
  float color[4];
};

static void
gsk_gpu_clear_op_finish (GskGpuOp *op)
{
}

static void
gsk_gpu_clear_op_print (GskGpuOp    *op,
                        GskGpuFrame *frame,
                        GString     *string,
                        guint        indent)
{
  GskGpuClearOp *this = (GskGpuClearOp *) op;

  gsk_gpu_print_op (string, indent, "clear");
  gsk_gpu_print_int_rect (string, &this->rect);
  gsk_gpu_print_rgba (string, this->color);
  gsk_gpu_print_newline (string);
}

#ifdef GDK_RENDERING_VULKAN
static GskGpuOp *
gsk_gpu_clear_op_vk_command (GskGpuOp              *op,
                             GskGpuFrame           *frame,
                             GskVulkanCommandState *state)
{
  GskGpuClearOp *this = (GskGpuClearOp *) op;
  VkClearValue clear_value;

  memcpy (clear_value.color.float32, this->color, sizeof (float) * 4);

  vkCmdClearAttachments (state->vk_command_buffer,
                         1,
                         &(VkClearAttachment) {
                           VK_IMAGE_ASPECT_COLOR_BIT,
                           0,
                           clear_value,
                         },
                         1,
                         &(VkClearRect) {
                           {
                             { this->rect.x, this->rect.y },
                             { this->rect.width, this->rect.height },
                           },
                           0,
                           1
                         });

  return op->next;
}
#endif

static GskGpuOp *
gsk_gpu_clear_op_gl_command (GskGpuOp          *op,
                             GskGpuFrame       *frame,
                             GskGLCommandState *state)
{
  GskGpuClearOp *this = (GskGpuClearOp *) op;
  int scissor[4];

  glGetIntegerv (GL_SCISSOR_BOX, scissor);

  if (state->flip_y)
    glScissor (this->rect.x, state->flip_y - this->rect.y - this->rect.height, this->rect.width, this->rect.height);
  else
    glScissor (this->rect.x, this->rect.y, this->rect.width, this->rect.height);

  glClearColor (this->color[0], this->color[1], this->color[2], this->color[3]);
  glClear (GL_COLOR_BUFFER_BIT);

  glScissor (scissor[0], scissor[1], scissor[2], scissor[3]);

  return op->next;
}

static const GskGpuOpClass GSK_GPU_CLEAR_OP_CLASS = {
  GSK_GPU_OP_SIZE (GskGpuClearOp),
  GSK_GPU_STAGE_COMMAND,
  gsk_gpu_clear_op_finish,
  gsk_gpu_clear_op_print,
#ifdef GDK_RENDERING_VULKAN
  gsk_gpu_clear_op_vk_command,
#endif
  gsk_gpu_clear_op_gl_command
};

void
gsk_gpu_clear_op (GskGpuFrame                 *frame,
                  const cairo_rectangle_int_t *rect,
                  const float                  color[4])
{
  GskGpuClearOp *this;

  this = (GskGpuClearOp *) gsk_gpu_op_alloc (frame, &GSK_GPU_CLEAR_OP_CLASS);

  this->rect = *rect;
  memcpy (this->color, color, sizeof (float) * 4);
}
