#pragma once

#include "gdkcolorstate.h"

#include "gdkcicpparamsprivate.h"
#include "gdkdebugprivate.h"
#include "gdkmemoryformatprivate.h"
#include "gdkrgba.h"

typedef enum
{
  GDK_COLOR_STATE_ID_SRGB,
  GDK_COLOR_STATE_ID_SRGB_LINEAR,
  GDK_COLOR_STATE_ID_REC2100_PQ,
  GDK_COLOR_STATE_ID_REC2100_LINEAR,

  GDK_COLOR_STATE_N_IDS
} GdkColorStateId;

typedef enum
{
  GDK_BUILTIN_COLOR_STATE_ID_OKLAB,
  GDK_BUILTIN_COLOR_STATE_ID_OKLCH,

  GDK_BUILTIN_COLOR_STATE_N_IDS
} GdkBuiltinColorStateId;

typedef struct _GdkColorStateClass GdkColorStateClass;

struct _GdkColorState
{
  const GdkColorStateClass *klass;
  gatomicrefcount ref_count;

  GdkMemoryDepth depth;
  GdkColorState *rendering_color_state;
  GdkColorState *rendering_color_state_linear;
};

/* Note: this may be the source or the target colorstate */
typedef void            (* GdkFloatColorConvert)(GdkColorState  *this,
                                                 float         (*values)[4],
                                                 gsize           n_values);

struct _GdkColorStateClass
{
  void                  (* free)                (GdkColorState  *this);
  gboolean              (* equal)               (GdkColorState  *this,
                                                 GdkColorState  *other);
  const char *          (* get_name)            (GdkColorState  *this);
  GdkColorState *       (* get_no_srgb_tf)      (GdkColorState  *this);
  GdkFloatColorConvert  (* get_convert_to)      (GdkColorState  *this,
                                                 GdkColorState  *target);
  GdkFloatColorConvert  (* get_convert_from)    (GdkColorState  *this,
                                                 GdkColorState  *source);
  const GdkCicp *       (* get_cicp)            (GdkColorState  *this);
  void                  (* clamp)               (GdkColorState  *this,
                                                 const float     src[4],
                                                 float           dest[4]);
};

typedef struct _GdkDefaultColorState GdkDefaultColorState;

struct _GdkDefaultColorState
{
  GdkColorState parent;

  const char *name;
  GdkColorState *no_srgb;
  GdkFloatColorConvert convert_to[GDK_COLOR_STATE_N_IDS];
  void (* clamp) (GdkColorState  *this,
                  const float     src[4],
                  float           dest[4]);

  GdkCicp cicp;
};

typedef struct _GdkBuiltinColorState GdkBuiltinColorState;

struct _GdkBuiltinColorState
{
  GdkColorState parent;

  const char *name;
  GdkFloatColorConvert convert_to[GDK_COLOR_STATE_N_IDS];
  GdkFloatColorConvert convert_from[GDK_COLOR_STATE_N_IDS];
};

extern GdkDefaultColorState gdk_default_color_states[GDK_COLOR_STATE_N_IDS];
extern GdkBuiltinColorState gdk_builtin_color_states[GDK_BUILTIN_COLOR_STATE_N_IDS];

#define GDK_COLOR_STATE_SRGB           ((GdkColorState *) &gdk_default_color_states[GDK_COLOR_STATE_ID_SRGB])
#define GDK_COLOR_STATE_SRGB_LINEAR    ((GdkColorState *) &gdk_default_color_states[GDK_COLOR_STATE_ID_SRGB_LINEAR])
#define GDK_COLOR_STATE_REC2100_PQ     ((GdkColorState *) &gdk_default_color_states[GDK_COLOR_STATE_ID_REC2100_PQ])
#define GDK_COLOR_STATE_REC2100_LINEAR ((GdkColorState *) &gdk_default_color_states[GDK_COLOR_STATE_ID_REC2100_LINEAR])
#define GDK_COLOR_STATE_OKLAB          ((GdkColorState *) &gdk_builtin_color_states[GDK_BUILTIN_COLOR_STATE_ID_OKLAB])
#define GDK_COLOR_STATE_OKLCH          ((GdkColorState *) &gdk_builtin_color_states[GDK_BUILTIN_COLOR_STATE_ID_OKLCH])
#define GDK_COLOR_STATE_YUV            (gdk_color_state_yuv ())

#define GDK_IS_DEFAULT_COLOR_STATE(c) ((GdkDefaultColorState *) (c) >= &gdk_default_color_states[0] && \
                                       (GdkDefaultColorState *) (c) < &gdk_default_color_states[GDK_COLOR_STATE_N_IDS])
#define GDK_DEFAULT_COLOR_STATE_ID(c) ((GdkColorStateId) (((GdkDefaultColorState *) c) - gdk_default_color_states))
#define GDK_IS_BUILTIN_COLOR_STATE(c) ((GdkBuiltinColorState *) (c) >= &gdk_builtin_color_states[0] && \
                                       (GdkBuiltinColorState *) (c) < &gdk_builtin_color_states[GDK_BUILTIN_COLOR_STATE_N_IDS])
#define GDK_BUILTIN_COLOR_STATE_ID(c) ((GdkBuiltinColorStateId) (((GdkBuiltinColorState *) c) - gdk_builtin_color_states))

GdkColorState * gdk_color_state_yuv                     (void);
const char *    gdk_color_state_get_name                (GdkColorState          *this);
GdkColorState * gdk_color_state_get_no_srgb_tf          (GdkColorState          *this);

GdkColorState * gdk_color_state_new_for_cicp            (const GdkCicp          *cicp,
                                                         GError                **error);

void            gdk_color_state_clamp                   (GdkColorState          *this,
                                                         const float             src[4],
                                                         float                   dest[4]);

static inline GdkColorState *
gdk_color_state_get_rendering_color_state (GdkColorState *this)
{
  if (GDK_DEBUG_CHECK (HDR))
    this = GDK_COLOR_STATE_REC2100_PQ;

  if (!GDK_DEBUG_CHECK (LINEAR))
    return this->rendering_color_state;

  return this->rendering_color_state_linear;
}

static inline GdkMemoryDepth
gdk_color_state_get_depth (GdkColorState *this)
{
  if (!GDK_DEBUG_CHECK (LINEAR) && this->depth == GDK_MEMORY_U8_SRGB)
    return GDK_MEMORY_U8;

  return this->depth;
}

static inline GdkColorState *
gdk_color_state_get_by_id (GdkColorStateId id)
{
  return (GdkColorState *) &gdk_default_color_states[id];
}

#define gdk_color_state_ref(this) _gdk_color_state_ref (this)
static inline GdkColorState *
_gdk_color_state_ref (GdkColorState *this)
{
  if (GDK_IS_DEFAULT_COLOR_STATE (this) ||
      GDK_IS_BUILTIN_COLOR_STATE (this))
    return this;

  g_atomic_ref_count_inc (&this->ref_count);

  return this;
}

#define gdk_color_state_unref(this) _gdk_color_state_unref (this)
static inline void
_gdk_color_state_unref (GdkColorState *this)
{
  if (GDK_IS_DEFAULT_COLOR_STATE (this) ||
      GDK_IS_BUILTIN_COLOR_STATE (this))
    return;

  if (g_atomic_ref_count_dec (&this->ref_count))
    this->klass->free (this);
}

#define gdk_color_state_equal(a,b) _gdk_color_state_equal ((a), (b))
static inline gboolean
_gdk_color_state_equal (GdkColorState *this,
                        GdkColorState *other)
{
  if (this == other)
    return TRUE;

  if (this->klass != other->klass)
    return FALSE;

  return this->klass->equal (this, other);
}

#define gdk_color_state_equivalent(a,b) _gdk_color_state_equivalent ((a), (b))
static inline gboolean
_gdk_color_state_equivalent (GdkColorState *this,
                             GdkColorState *other)
{
  const GdkCicp *cicp1;
  const GdkCicp *cicp2;

  if (this == other)
    return TRUE;

  cicp1 = this->klass->get_cicp (this);
  cicp2 = this->klass->get_cicp (other);

  return gdk_cicp_equivalent (cicp1, cicp2);
}

/* Note: the functions returned from this expect the source
 * color state to be passed as this
 */
static inline GdkFloatColorConvert
gdk_color_state_get_convert_to (GdkColorState *this,
                                GdkColorState *target)
{
  return this->klass->get_convert_to (this, target);
}

/* Note: the functions returned from this expect the target
 * color state to be passed as this
 */
static inline GdkFloatColorConvert
gdk_color_state_get_convert_from (GdkColorState *this,
                                  GdkColorState *source)
{
  return this->klass->get_convert_from (this, source);
}

static inline const GdkCicp *
gdk_color_state_get_cicp (GdkColorState *this)
{
  return this->klass->get_cicp (this);
}

static inline void
gdk_color_state_convert_color (GdkColorState *src_cs,
                               const float    src[4],
                               GdkColorState *dest_cs,
                               float          dest[4])
{
  GdkFloatColorConvert convert = NULL;
  GdkFloatColorConvert convert2 = NULL;

  dest[0] = src[0];
  dest[1] = src[1];
  dest[2] = src[2];
  dest[3] = src[3];

  if (gdk_color_state_equal (src_cs, dest_cs))
    return;

  convert = gdk_color_state_get_convert_to (src_cs, dest_cs);

  if (!convert)
    convert2 = gdk_color_state_get_convert_from (dest_cs, src_cs);

  if (!convert && !convert2)
    {
      GdkColorState *connection = GDK_COLOR_STATE_REC2100_LINEAR;
      convert = gdk_color_state_get_convert_to (src_cs, connection);
      convert2 = gdk_color_state_get_convert_from (dest_cs, connection);
    }

  if (convert)
    convert (src_cs,  (float(*)[4]) dest, 1);

  if (convert2)
    convert2 (dest_cs, (float(*)[4]) dest, 1);
}

static inline void
gdk_color_state_from_rgba (GdkColorState *this,
                           const GdkRGBA *rgba,
                           float          out_color[4])
{
  gdk_color_state_convert_color (GDK_COLOR_STATE_SRGB,
                                 (const float *) rgba,
                                 this,
                                 out_color);
}

