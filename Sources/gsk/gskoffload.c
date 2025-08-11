/* gskoffload.c
 *
 * Copyright 2023 Red Hat, Inc.
 *
 * This file is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "config.h"

#include "gskoffloadprivate.h"

#include "gskrendernode.h"
#include "gskrectprivate.h"
#include "gskroundedrectprivate.h"
#include "gsktransformprivate.h"
#include "gskdebugprivate.h"
#include "gskrendernodeprivate.h"
#include "gdksurfaceprivate.h"
#include "gdkrgbaprivate.h"

#include <graphene.h>

typedef struct
{
  GskRoundedRect rect;
  guint is_rectilinear     : 1;
  guint is_fully_contained : 1;
  guint is_empty           : 1;
  guint is_complex         : 1;
} Clip;

struct _GskOffload
{
  GdkSurface *surface;
  GskOffloadInfo *subsurfaces;
  gsize n_subsurfaces;

  GSList *transforms;
  GSList *clips;

  Clip *current_clip;

  GskOffloadInfo *last_info;
};

static GdkDihedral
find_texture_transform (GskTransform *transform)
{
  GdkDihedral dihedral;
  float sx, sy, dx, dy;

  g_assert (gsk_transform_get_fine_category (transform) >= GSK_FINE_TRANSFORM_CATEGORY_2D_DIHEDRAL);

  gsk_transform_to_dihedral (transform, &dihedral, &sx, &sy, &dx, &dy);

  return dihedral;
}

static gboolean
color_is_black (const GdkColor *color)
{
  return color->red < 255.f / 65535.f &&
         color->green < 255.f / 65535.f &&
         color->blue < 255.f / 65535.f &&
         color->alpha > 65280.f / 65535.f;
}

static GdkTexture *
find_texture_to_attach (GskOffload          *this,
                        const GskRenderNode *subsurface_node,
                        graphene_rect_t     *out_texture_rect,
                        graphene_rect_t     *out_source_rect,
                        gboolean            *has_background,
                        GdkDihedral         *out_texture_transform)
{
  GdkSubsurface *subsurface;
  const GskRenderNode *node;
  gboolean has_clip = FALSE;
  graphene_rect_t clip;
  GskTransform *transform = NULL;
  GdkTexture *ret = NULL;

  *has_background = FALSE;
  *out_texture_transform = GDK_DIHEDRAL_NORMAL;

  subsurface = gsk_subsurface_node_get_subsurface (subsurface_node);
  node = subsurface_node;

  for (;;)
    {
      switch ((int) GSK_RENDER_NODE_TYPE (node))
        {
        case GSK_DEBUG_NODE:
          node = gsk_debug_node_get_child (node);
          break;

        case GSK_SUBSURFACE_NODE:
          node = gsk_subsurface_node_get_child (node);
          break;

        case GSK_CONTAINER_NODE:
          if (gsk_container_node_get_n_children (node) == 1)
            {
              node = gsk_container_node_get_child (node, 0);
              break;
            }
          else if (gsk_container_node_get_n_children (node) == 2)
            {
              GskRenderNode *child = gsk_container_node_get_child (node, 0);
              graphene_rect_t bounds;

              gsk_transform_transform_bounds (transform, &child->bounds, &bounds);
              if (GSK_RENDER_NODE_TYPE (child) == GSK_COLOR_NODE &&
                  gsk_rect_equal (&bounds, &subsurface_node->bounds) &&
                  color_is_black (gsk_color_node_get_gdk_color (child)))
                {
                  *has_background = TRUE;
                  node = gsk_container_node_get_child (node, 1);
                  break;
                }
            }

          GDK_DISPLAY_DEBUG (gdk_surface_get_display (this->surface), OFFLOAD,
                             "[%p] 🗙 Too much content, container with %d children",
                             subsurface, gsk_container_node_get_n_children (node));
          goto out;

        case GSK_TRANSFORM_NODE:
          {
            GskTransform *t = gsk_transform_node_get_transform (node);

            if (gsk_transform_get_fine_category (t) < GSK_FINE_TRANSFORM_CATEGORY_2D_DIHEDRAL)
              {
                char *s = gsk_transform_to_string (t);
                GDK_DISPLAY_DEBUG (gdk_surface_get_display (this->surface), OFFLOAD,
                                   "[%p] 🗙 Transform %s is not dihedral",
                                   subsurface, s);
                g_free (s);
                goto out;
              }

            if (has_clip)
              {
                GskTransform *inv = gsk_transform_invert (gsk_transform_ref (t));
                gsk_transform_transform_bounds (inv, &clip, &clip);
                gsk_transform_unref (inv);
              }

            transform = gsk_transform_transform (transform, gsk_transform_ref (t));

            node = gsk_transform_node_get_child (node);
          }
          break;

        case GSK_CLIP_NODE:
          {
            const graphene_rect_t *c = gsk_clip_node_get_clip (node);

            if (has_clip)
              {
                if (!gsk_rect_intersection (c, &clip, &clip))
                  {
                    GDK_DISPLAY_DEBUG (gdk_surface_get_display (this->surface), OFFLOAD,
                                       "[%p] 🗙 Empty clip", subsurface);
                    goto out;
                  }
              }
            else
              {
                gsk_transform_transform_bounds (transform, &node->bounds, out_texture_rect);
                clip = *c;
                has_clip = TRUE;
              }

            node = gsk_clip_node_get_child (node);
          }
          break;

        case GSK_TEXTURE_NODE:
          {
            GdkTexture *texture = gsk_texture_node_get_texture (node);
            int width, height;

            if (gsk_transform_get_fine_category (transform) < GSK_FINE_TRANSFORM_CATEGORY_2D_DIHEDRAL)
              {
                char *s = gsk_transform_to_string (transform);
                GDK_DISPLAY_DEBUG (gdk_surface_get_display (this->surface), OFFLOAD,
                                   "[%p] 🗙 Transform %s is not dihedral",
                                   subsurface, s);
                g_free (s);
                goto out;
              }

            width = gdk_texture_get_width (texture);
            height = gdk_texture_get_height (texture);

            *out_texture_transform = find_texture_transform (transform);

            if (gdk_dihedral_swaps_xy (*out_texture_transform))
              {
                int tmp = width;
                width = height;
                height = tmp;
              }

            if (has_clip)
              {
                float dx = node->bounds.origin.x;
                float dy = node->bounds.origin.y;
                float sx = width / node->bounds.size.width;
                float sy = height / node->bounds.size.height;

                gsk_rect_intersection (&node->bounds, &clip, &clip);

                out_source_rect->origin.x = (clip.origin.x - dx) * sx;
                out_source_rect->origin.y = (clip.origin.y - dy) * sy;
                out_source_rect->size.width = clip.size.width * sx;
                out_source_rect->size.height = clip.size.height * sy;
              }
            else
              {
                gsk_transform_transform_bounds (transform, &node->bounds, out_texture_rect);
                out_source_rect->origin.x = 0;
                out_source_rect->origin.y = 0;
                out_source_rect->size.width = width;
                out_source_rect->size.height = height;
              }

            ret = texture;
            goto out;
          }

        default:
          GDK_DISPLAY_DEBUG (gdk_surface_get_display (this->surface), OFFLOAD,
                             "[%p] 🗙 Only textures supported (found %s)",
                             subsurface, g_type_name_from_instance ((GTypeInstance *) node));
          goto out;
        }
    }

out:
  g_clear_pointer (&transform, gsk_transform_unref);

  return ret;
}

static void
push_transform (GskOffload   *this,
                GskTransform *transform)
{
  if (this->transforms)
    {
      GskTransform *t = this->transforms->data;
      t = gsk_transform_transform (gsk_transform_ref (t), transform);
      this->transforms = g_slist_prepend (this->transforms, t);
    }
  else
    this->transforms = g_slist_prepend (NULL, gsk_transform_ref (transform));
}

static void
pop_transform (GskOffload *this)
{
  GSList *l = this->transforms;
  GskTransform *t = l->data;

  g_assert (this->transforms != NULL);

  this->transforms = this->transforms->next;

  g_slist_free_1 (l);
  gsk_transform_unref (t);
}

static inline void
transform_bounds (GskOffload            *this,
                  const graphene_rect_t *bounds,
                  graphene_rect_t       *rect)
{
  GskTransform *t = this->transforms ? this->transforms->data : NULL;

  gsk_transform_transform_bounds (t, bounds, rect);
}

static inline gboolean
transform_rounded_rect (GskOffload       *this,
                        const GskRoundedRect *rect,
                        GskRoundedRect       *out_rect)
{
  GskTransform *t = this->transforms ? this->transforms->data : NULL;
  GdkDihedral dihedral;
  float sx, sy, dx, dy;
  GskRoundedRect tmp;

  if (gsk_transform_get_fine_category (t) < GSK_FINE_TRANSFORM_CATEGORY_2D_DIHEDRAL)
    return FALSE;

  gsk_transform_to_dihedral (t, &dihedral, &sx, &sy, &dx, &dy);
  gsk_rounded_rect_dihedral (&tmp, rect, dihedral);
  gsk_rounded_rect_scale_affine (out_rect, &tmp, sx, sy, dx, dy);

  return TRUE;
}

static void
push_rect_clip (GskOffload           *this,
                const GskRoundedRect *rect)
{
  Clip *clip = g_new0 (Clip, 1);
  clip->rect = *rect;
  clip->is_rectilinear = gsk_rounded_rect_is_rectilinear (rect);
  clip->is_empty = (rect->bounds.size.width == 0 || rect->bounds.size.height == 0);

  this->clips = g_slist_prepend (this->clips, clip);
  this->current_clip = this->clips->data;
}

static void
push_empty_clip (GskOffload *this)
{
  push_rect_clip (this, &GSK_ROUNDED_RECT_INIT (0, 0, 0, 0));
}

static void
push_contained_clip (GskOffload *this)
{
  Clip *current_clip = this->clips->data;
  Clip *clip = g_new0 (Clip, 1);

  clip->rect = current_clip->rect;
  clip->is_rectilinear = TRUE;
  clip->is_fully_contained = TRUE;

  this->clips = g_slist_prepend (this->clips, clip);
  this->current_clip = this->clips->data;
}

static void
push_complex_clip (GskOffload *this)
{
  Clip *current_clip = this->clips->data;
  Clip *clip = g_new0 (Clip, 1);

  clip->rect = current_clip->rect;
  clip->is_complex = TRUE;

  this->clips = g_slist_prepend (this->clips, clip);
  this->current_clip = this->clips->data;
}

static void
pop_clip (GskOffload *this)
{
  GSList *l = this->clips;
  Clip *clip = l->data;

  g_assert (this->clips != NULL);

  this->clips = this->clips->next;
  if (this->clips)
    this->current_clip = this->clips->data;

  g_slist_free_1 (l);
  g_free (clip);
}

static inline void
rounded_rect_get_inner (const GskRoundedRect *rect,
                        graphene_rect_t      *inner)
{
  float left = MAX (rect->corner[GSK_CORNER_TOP_LEFT].width, rect->corner[GSK_CORNER_BOTTOM_LEFT].width);
  float right = MAX (rect->corner[GSK_CORNER_TOP_RIGHT].width, rect->corner[GSK_CORNER_BOTTOM_RIGHT].width);
  float top = MAX (rect->corner[GSK_CORNER_TOP_LEFT].height, rect->corner[GSK_CORNER_TOP_RIGHT].height);
  float bottom = MAX (rect->corner[GSK_CORNER_BOTTOM_LEFT].height, rect->corner[GSK_CORNER_BOTTOM_RIGHT].height);

  inner->origin.x = rect->bounds.origin.x + left;
  inner->size.width = rect->bounds.size.width - (left + right);

  inner->origin.y = rect->bounds.origin.y + top;
  inner->size.height = rect->bounds.size.height - (top + bottom);
}

static inline gboolean
interval_contains (float p1, float w1,
                   float p2, float w2)
{
  if (p2 < p1)
    return FALSE;

  if (p2 + w2 > p1 + w1)
    return FALSE;

  return TRUE;
}

static gboolean
update_clip (GskOffload            *this,
             const graphene_rect_t *transformed_bounds)
{
  gboolean no_clip = FALSE;
  gboolean rect_clip = FALSE;

  if (this->current_clip->is_fully_contained ||
      this->current_clip->is_empty ||
      this->current_clip->is_complex)
    return FALSE;

  if (!gsk_rect_intersects (&this->current_clip->rect.bounds, transformed_bounds))
    {
      push_empty_clip (this);
      return TRUE;
    }

  if (this->current_clip->is_rectilinear)
    {
      if (gsk_rect_contains_rect (&this->current_clip->rect.bounds, transformed_bounds))
        no_clip = TRUE;
      else
        rect_clip = TRUE;
    }
  else if (gsk_rounded_rect_contains_rect (&this->current_clip->rect, transformed_bounds))
    {
      no_clip = TRUE;
    }
  else
    {
      graphene_rect_t inner;

      rounded_rect_get_inner (&this->current_clip->rect, &inner);

      if (interval_contains (inner.origin.x, inner.size.width,
                             transformed_bounds->origin.x, transformed_bounds->size.width) ||
          interval_contains (inner.origin.y, inner.size.height,
                             transformed_bounds->origin.y, transformed_bounds->size.height))
        rect_clip = TRUE;
    }

  if (no_clip)
    {
      /* This node is completely contained inside the clip.
       * Record this fact on the clip stack, so we don't do
       * more work for child nodes.
       */
      push_contained_clip (this);
      return TRUE;
    }
  else if (rect_clip && !this->current_clip->is_rectilinear)
    {
      graphene_rect_t rect;

      /* The clip gets simpler for this node */

      gsk_rect_intersection (&this->current_clip->rect.bounds, transformed_bounds, &rect);
      push_rect_clip (this, &GSK_ROUNDED_RECT_INIT_FROM_RECT (rect));
      return TRUE;
    }

  return FALSE;
}

static GskOffloadInfo *
find_subsurface_info (GskOffload    *this,
                      GdkSubsurface *subsurface)
{
  for (gsize i = 0; i < this->n_subsurfaces; i++)
    {
      GskOffloadInfo *info = &this->subsurfaces[i];
      if (info->subsurface == subsurface)
        return info;
    }

  return NULL;
}

static void
visit_node (GskOffload    *this,
            GskRenderNode *node)
{
  gboolean has_clip;
  graphene_rect_t transformed_bounds;

  transform_bounds (this, &node->bounds, &transformed_bounds);

  for (gsize i = 0; i < this->n_subsurfaces; i++)
    {
      GskOffloadInfo *info = &this->subsurfaces[i];

      if (info->can_raise)
        {
          if (gsk_rect_intersects (&transformed_bounds, &info->texture_rect) ||
              gsk_rect_intersects (&transformed_bounds, &info->background_rect))
            {
              GskRenderNodeType type = GSK_RENDER_NODE_TYPE (node);

              if (type != GSK_CONTAINER_NODE &&
                  type != GSK_TRANSFORM_NODE &&
                  type != GSK_CLIP_NODE &&
                  type != GSK_ROUNDED_CLIP_NODE &&
                  type != GSK_DEBUG_NODE)
                {
                  GDK_DISPLAY_DEBUG (gdk_surface_get_display (this->surface), OFFLOAD,
                                     "[%p]   Lowering because a %s overlaps",
                                     info->subsurface,
                                     g_type_name_from_instance ((GTypeInstance *) node));
                  info->can_raise = FALSE;
                }
            }
        }
    }

  has_clip = update_clip (this, &transformed_bounds);

  switch (GSK_RENDER_NODE_TYPE (node))
    {
    case GSK_BORDER_NODE:
    case GSK_CONIC_GRADIENT_NODE:
    case GSK_LINEAR_GRADIENT_NODE:
    case GSK_REPEATING_LINEAR_GRADIENT_NODE:
    case GSK_RADIAL_GRADIENT_NODE:
    case GSK_REPEATING_RADIAL_GRADIENT_NODE:
    case GSK_TEXT_NODE:
    case GSK_TEXTURE_NODE:
    case GSK_TEXTURE_SCALE_NODE:
    case GSK_CAIRO_NODE:
    case GSK_COLOR_NODE:
    case GSK_INSET_SHADOW_NODE:
    case GSK_OUTSET_SHADOW_NODE:
    case GSK_GL_SHADER_NODE:
    case GSK_BLEND_NODE:
    case GSK_BLUR_NODE:
    case GSK_COLOR_MATRIX_NODE:
    case GSK_OPACITY_NODE:
    case GSK_CROSS_FADE_NODE:
    case GSK_SHADOW_NODE:
    case GSK_REPEAT_NODE:
    case GSK_MASK_NODE:
    case GSK_FILL_NODE:
    case GSK_STROKE_NODE:
      break;

    case GSK_CLIP_NODE:
      {
        const graphene_rect_t *clip = gsk_clip_node_get_clip (node);
        graphene_rect_t transformed_clip;
        GskRoundedRect intersection;

        transform_bounds (this, clip, &transformed_clip);

        if (this->current_clip->is_rectilinear)
          {
            memset (&intersection.corner, 0, sizeof intersection.corner);
            gsk_rect_intersection (&transformed_clip,
                                   &this->current_clip->rect.bounds,
                                   &intersection.bounds);

            push_rect_clip (this, &intersection);
            visit_node (this, gsk_clip_node_get_child (node));
            pop_clip (this);
          }
        else
          {
            GskRoundedRectIntersection result;

            result = gsk_rounded_rect_intersect_with_rect (&this->current_clip->rect,
                                                           &transformed_clip,
                                                           &intersection);

            if (result == GSK_INTERSECTION_EMPTY)
              push_empty_clip (this);
            else if (result == GSK_INTERSECTION_NONEMPTY)
              push_rect_clip (this, &intersection);
            else
              push_complex_clip (this);
            visit_node (this, gsk_clip_node_get_child (node));
            pop_clip (this);
          }
      }
      break;

    case GSK_ROUNDED_CLIP_NODE:
      {
        const GskRoundedRect *clip = gsk_rounded_clip_node_get_clip (node);
        GskRoundedRect transformed_clip;

        if (!transform_rounded_rect (this, clip, &transformed_clip))
          {
            GDK_DISPLAY_DEBUG (gdk_surface_get_display (this->surface), OFFLOAD,
                               "🗙 Non-dihedral transform, giving up");
          }
        else if (this->current_clip->is_rectilinear)
          {
            GskRoundedRect intersection;
            GskRoundedRectIntersection result;

            result = gsk_rounded_rect_intersect_with_rect (&transformed_clip,
                                                           &this->current_clip->rect.bounds,
                                                           &intersection);

            if (result == GSK_INTERSECTION_EMPTY)
              push_empty_clip (this);
            else if (result == GSK_INTERSECTION_NONEMPTY)
              push_rect_clip (this, &intersection);
            else
              goto complex_clip;
            visit_node (this, gsk_rounded_clip_node_get_child (node));
            pop_clip (this);
          }
        else
          {
complex_clip:
            if (gsk_rounded_rect_contains_rect (&this->current_clip->rect, &transformed_clip.bounds))
              push_rect_clip (this, &transformed_clip);
            else
              push_complex_clip (this);
            visit_node (this, gsk_rounded_clip_node_get_child (node));
            pop_clip (this);
          }
      }
      break;

    case GSK_TRANSFORM_NODE:
      push_transform (this, gsk_transform_node_get_transform (node));
      visit_node (this, gsk_transform_node_get_child (node));
      pop_transform (this);
      break;

    case GSK_CONTAINER_NODE:
      for (gsize i = 0; i < gsk_container_node_get_n_children (node); i++)
        visit_node (this, gsk_container_node_get_child (node, i));
      break;

    case GSK_DEBUG_NODE:
      visit_node (this, gsk_debug_node_get_child (node));
      break;

    case GSK_SUBSURFACE_NODE:
      {
        GdkSubsurface *subsurface = gsk_subsurface_node_get_subsurface (node);
        GskTransform *transform;

        GskOffloadInfo *info = find_subsurface_info (this, subsurface);

        transform = this->transforms ? (GskTransform *) this->transforms->data : NULL;

        if (info == NULL)
          {
            GDK_DISPLAY_DEBUG (gdk_surface_get_display (this->surface), OFFLOAD,
                               "[%p] 🗙 Unknown subsurface",
                               subsurface);
          }
        else if (!this->current_clip->is_fully_contained)
          {
            GDK_DISPLAY_DEBUG (gdk_surface_get_display (this->surface), OFFLOAD,
                               "[%p] 🗙 Clipped",
                               subsurface);
          }
        else if (gsk_transform_get_fine_category (transform) < GSK_FINE_TRANSFORM_CATEGORY_2D_DIHEDRAL)
          {
            GDK_DISPLAY_DEBUG (gdk_surface_get_display (this->surface), OFFLOAD,
                               "[%p] 🗙 Non-dihedral transform",
                               subsurface);
          }
        else
          {
            gboolean has_background;
            float sx, sy, dx, dy;
            GdkDihedral context_transform;
            GdkDihedral inner_transform;

            gsk_transform_to_dihedral (transform, &context_transform, &sx, &sy, &dx, &dy);

            info->texture = find_texture_to_attach (this, node, &info->texture_rect, &info->source_rect, &has_background, &inner_transform);
            if (info->texture)
              {
                info->transform = gdk_dihedral_combine (context_transform, inner_transform);
                info->can_offload = TRUE;
                info->can_raise = TRUE;
                transform_bounds (this, &info->texture_rect, &info->texture_rect);
                info->has_background = has_background;
                transform_bounds (this, &node->bounds, &info->background_rect);
                info->place_above = this->last_info ? this->last_info->subsurface : NULL;
                this->last_info = info;
              }
          }
      }
      break;

    case GSK_NOT_A_RENDER_NODE:
    default:
      g_assert_not_reached ();
    break;
    }

  if (has_clip)
    pop_clip (this);
}

GskOffload *
gsk_offload_new (GdkSurface     *surface,
                 GskRenderNode  *root,
                 cairo_region_t *diff)
{
  GskOffload *this;

  this = g_new0 (GskOffload, 1);

  this->surface = surface;
  this->transforms = NULL;
  this->clips = NULL;

  this->last_info = NULL;

  this->n_subsurfaces = gdk_surface_get_n_subsurfaces (this->surface);
  this->subsurfaces = g_new0 (GskOffloadInfo, this->n_subsurfaces);

  for (gsize i = 0; i < this->n_subsurfaces; i++)
    {
      GskOffloadInfo *info = &this->subsurfaces[i];
      graphene_rect_t rect;

      info->subsurface = gdk_surface_get_subsurface (this->surface, i);
      info->was_offloaded = gdk_subsurface_get_texture (info->subsurface) != NULL;
      info->was_above = gdk_subsurface_is_above_parent (info->subsurface);
      info->had_background = gdk_subsurface_get_background_rect (info->subsurface, &rect);
    }

  if (this->n_subsurfaces > 0)
    {
      push_rect_clip (this, &GSK_ROUNDED_RECT_INIT (0, 0,
                                                    gdk_surface_get_width (surface),
                                                    gdk_surface_get_height (surface)));

      visit_node (this, root);

      pop_clip (this);
    }

  for (gsize i = 0; i < this->n_subsurfaces; i++)
    {
      GskOffloadInfo *info = &this->subsurfaces[i];
      graphene_rect_t old_bounds;
      graphene_rect_t bounds;

      gdk_subsurface_get_bounds (info->subsurface, &old_bounds);

      if (info->can_offload)
        {
          if (info->can_raise)
            info->is_offloaded = gdk_subsurface_attach (info->subsurface,
                                                        info->texture,
                                                        &info->source_rect,
                                                        &info->texture_rect,
                                                        info->transform,
                                                        info->has_background ? &info->background_rect : NULL,
                                                        TRUE, NULL);
          else
            info->is_offloaded = gdk_subsurface_attach (info->subsurface,
                                                        info->texture,
                                                        &info->source_rect,
                                                        &info->texture_rect,
                                                        info->transform,
                                                        info->has_background ? &info->background_rect : NULL,
                                                        info->place_above != NULL,
                                                        info->place_above);
        }
      else
        {
          info->is_offloaded = FALSE;
          if (info->was_offloaded)
            gdk_subsurface_detach (info->subsurface);
        }

      info->is_above = info->is_offloaded && gdk_subsurface_is_above_parent (info->subsurface);

      gdk_subsurface_get_bounds (info->subsurface, &bounds);

      if (info->is_offloaded != info->was_offloaded ||
          info->is_above != info->was_above ||
          (info->is_offloaded && !gsk_rect_equal (&bounds, &old_bounds)))
        {
          /* We changed things, need to invalidate everything */
          cairo_rectangle_int_t rect;

          if (info->is_offloaded)
            {
              gsk_rect_to_cairo_grow (&bounds, &rect);
              cairo_region_union_rectangle (diff, &rect);
            }
          if (info->was_offloaded)
            {
              gsk_rect_to_cairo_grow (&old_bounds, &rect);
              cairo_region_union_rectangle (diff, &rect);
            }
        }

    }

  return this;
}

void
gsk_offload_free (GskOffload *this)
{
  g_free (this->subsurfaces);
  g_free (this);
}

GskOffloadInfo *
gsk_offload_get_subsurface_info (GskOffload    *this,
                                 GdkSubsurface *subsurface)
{
  return find_subsurface_info (this, subsurface);
}
