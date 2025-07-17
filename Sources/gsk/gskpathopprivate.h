/*
 * Copyright © 2020 Benjamin Otte
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Benjamin Otte <otte@gnome.org>
 */


#pragma once

#include "gskpath.h"
#include "gskpathbuilder.h"

G_BEGIN_DECLS

/* We assume that arrays of graphene_point_t are aligned on an 8-byte
 * boundary, which means we can use the lowest 3 bits to represent up
 * to 8 distinct path operations. */
#define GSK_PATHOP_OPERATION_MASK (0x7)

/* graphene_point_t is a struct containing two floats, so an array of
 * graphene_point_t on the stack is not necessarily 8-byte aligned
 * unless we force it to be.
 *
 * Using a union for this means the compiler will warn or error if we
 * have not handled these correctly: for example we can go from a
 * GskAlignedPoint * to a graphene_point_t * (which is always OK) with:
 *
 * GskAlignedPoint *gap = ...;
 * graphene_point_t *gpt;
 * gpt = &gap[0].pt;
 *
 * but going back the other way is not possible without a cast or a
 * compiler warning. */
typedef union
{
  graphene_point_t pt;

  /* On many platforms this will be enough to force the correct alignment. */
  guint64 alignment;

  /* Unfortunately not all platforms require guint64 to be naturally-aligned
   * (for example on i386, only 4-byte alignment is required) so we have to
   * try harder. */
#ifdef __GNUC__
  __attribute__((aligned(8))) guint64 really_aligned;
#elif defined(_MSC_VER)
  __declspec(align(8)) guint64 really_aligned;
#endif
} GskAlignedPoint;

G_STATIC_ASSERT (sizeof (GskAlignedPoint) == sizeof (graphene_point_t));
G_STATIC_ASSERT (G_ALIGNOF (GskAlignedPoint) >= GSK_PATHOP_OPERATION_MASK + 1);

typedef gpointer gskpathop;

static inline
gskpathop               gsk_pathop_encode                       (GskPathOperation        op,
                                                                 const GskAlignedPoint  *pts);
static inline
const GskAlignedPoint  *gsk_pathop_aligned_points               (gskpathop               pop);
static inline
const graphene_point_t *gsk_pathop_points                       (gskpathop               pop);
static inline
GskPathOperation        gsk_pathop_op                           (gskpathop               pop);

static inline
gboolean                gsk_pathop_foreach                      (gskpathop               pop,
                                                                 GskPathForeachFunc      func,
                                                                 gpointer                user_data);

/* included inline so tests can use them */
static inline
void                    gsk_path_builder_pathop_to              (GskPathBuilder         *builder,
                                                                 gskpathop               op);
static inline
void                    gsk_path_builder_pathop_reverse_to      (GskPathBuilder         *builder,
                                                                 gskpathop               op);

/* IMPLEMENTATION */

/* Note:
 *
 * The weight of conics is encoded as p[2].x, and the endpoint is p[3].
 * This is important, since contours store the points of adjacent
 * operations overlapping, so we can't put the weight at the end.
 */

static inline gskpathop
gsk_pathop_encode (GskPathOperation        op,
                   const GskAlignedPoint  *pts)
{
  /* g_assert (op & GSK_PATHOP_OPERATION_MASK == op); */
  g_assert ((GPOINTER_TO_SIZE (pts) & GSK_PATHOP_OPERATION_MASK) == 0);

  return GSIZE_TO_POINTER (GPOINTER_TO_SIZE (pts) | op);
}

static inline const GskAlignedPoint *
gsk_pathop_aligned_points (gskpathop pop)
{
  return GSIZE_TO_POINTER (GPOINTER_TO_SIZE (pop) & ~GSK_PATHOP_OPERATION_MASK);
}

static inline const graphene_point_t *
gsk_pathop_points (gskpathop pop)
{
  return &(gsk_pathop_aligned_points (pop)->pt);
}

static inline
GskPathOperation gsk_pathop_op (gskpathop pop)
{
  return GPOINTER_TO_SIZE (pop) & GSK_PATHOP_OPERATION_MASK;
}

static inline gboolean
gsk_pathop_foreach (gskpathop          pop,
                    GskPathForeachFunc func,
                    gpointer           user_data)
{
  switch (gsk_pathop_op (pop))
  {
    case GSK_PATH_MOVE:
      return func (gsk_pathop_op (pop), gsk_pathop_points (pop), 1, 0, user_data);

    case GSK_PATH_CLOSE:
    case GSK_PATH_LINE:
      return func (gsk_pathop_op (pop), gsk_pathop_points (pop), 2, 0, user_data);

    case GSK_PATH_QUAD:
      return func (gsk_pathop_op (pop), gsk_pathop_points (pop), 3, 0, user_data);

    case GSK_PATH_CUBIC:
      return func (gsk_pathop_op (pop), gsk_pathop_points (pop), 4, 0, user_data);

    case GSK_PATH_CONIC:
      {
        const graphene_point_t *pts = gsk_pathop_points (pop);
        return func (gsk_pathop_op (pop), (graphene_point_t[3]) { pts[0], pts[1], pts[3] }, 3, pts[2].x, user_data);
      }

    default:
      g_assert_not_reached ();
      return TRUE;
    }
}

static inline void
gsk_path_builder_pathop_to (GskPathBuilder *builder,
                            gskpathop       op)
{
  const graphene_point_t *pts = gsk_pathop_points (op);

  switch (gsk_pathop_op (op))
  {
    case GSK_PATH_MOVE:
      gsk_path_builder_move_to (builder, pts[0].x, pts[0].y);
      break;

    case GSK_PATH_CLOSE:
      gsk_path_builder_close (builder);
      break;

    case GSK_PATH_LINE:
      gsk_path_builder_line_to (builder, pts[1].x, pts[1].y);
      break;

    case GSK_PATH_QUAD:
      gsk_path_builder_quad_to (builder, pts[1].x, pts[1].y, pts[2].x, pts[2].y);
      break;

    case GSK_PATH_CUBIC:
      gsk_path_builder_cubic_to (builder, pts[1].x, pts[1].y, pts[2].x, pts[2].y, pts[3].x, pts[3].y);
      break;

    case GSK_PATH_CONIC:
      gsk_path_builder_conic_to (builder, pts[1].x, pts[1].y, pts[3].x, pts[3].y, pts[2].x);
      break;

    default:
      g_assert_not_reached ();
      break;
  }
}

static inline void
gsk_path_builder_pathop_reverse_to (GskPathBuilder *builder,
                                    gskpathop       op)
{
  const graphene_point_t *pts = gsk_pathop_points (op);

  switch (gsk_pathop_op (op))
  {
    case GSK_PATH_MOVE:
      gsk_path_builder_move_to (builder, pts[0].x, pts[0].y);
      break;

    case GSK_PATH_CLOSE:
      gsk_path_builder_line_to (builder, pts[0].x, pts[0].y);
      break;

    case GSK_PATH_LINE:
      gsk_path_builder_line_to (builder, pts[1].x, pts[1].y);
      break;

    case GSK_PATH_QUAD:
      gsk_path_builder_quad_to (builder, pts[1].x, pts[1].y, pts[0].x, pts[0].y);
      break;

    case GSK_PATH_CUBIC:
      gsk_path_builder_cubic_to (builder, pts[2].x, pts[2].y, pts[1].x, pts[1].y, pts[0].x, pts[0].y);
      break;

    case GSK_PATH_CONIC:
      gsk_path_builder_conic_to (builder, pts[1].x, pts[1].y, pts[0].x, pts[0].y, pts[2].x);
      break;

    default:
      g_assert_not_reached ();
      break;
  }
}


G_END_DECLS
