#pragma once

#include <gsk/gsktypes.h>

G_BEGIN_DECLS

typedef struct _GskBoundingBox GskBoundingBox;

struct _GskBoundingBox {
  graphene_point_t min;
  graphene_point_t max;
};

static inline GskBoundingBox *
gsk_bounding_box_init (GskBoundingBox         *this,
                       const graphene_point_t *a,
                       const graphene_point_t *b)
{
  this->min.x = MIN (a->x, b->x);
  this->min.y = MIN (a->y, b->y);
  this->max.x = MAX (a->x, b->x);
  this->max.y = MAX (a->y, b->y);
  return this;
}

static inline GskBoundingBox *
gsk_bounding_box_init_copy (GskBoundingBox       *this,
                            const GskBoundingBox *src)
{
  this->min = src->min;
  this->max = src->max;
  return this;
}

static inline GskBoundingBox *
gsk_bounding_box_init_from_rect (GskBoundingBox        *this,
                                 const graphene_rect_t *bounds)
{
  this->min = bounds->origin;
  this->max.x = bounds->origin.x + bounds->size.width;
  this->max.y = bounds->origin.y + bounds->size.height;
  return this;
}

static inline void
gsk_bounding_box_expand (GskBoundingBox         *this,
                         const graphene_point_t *p)
{
  this->min.x = MIN (this->min.x, p->x);
  this->min.y = MIN (this->min.y, p->y);
  this->max.x = MAX (this->max.x, p->x);
  this->max.y = MAX (this->max.y, p->y);
}

static inline graphene_rect_t *
gsk_bounding_box_to_rect (const GskBoundingBox  *this,
                          graphene_rect_t       *rect)
{
  rect->origin = this->min;
  rect->size.width = this->max.x - this->min.x;
  rect->size.height = this->max.y - this->min.y;
  return rect;
}

static inline gboolean
gsk_bounding_box_contains_point (const GskBoundingBox   *this,
                                 const graphene_point_t *p)
{
  return this->min.x <= p->x && p->x <= this->max.x &&
         this->min.y <= p->y && p->y <= this->max.y;
}

static inline gboolean
gsk_bounding_box_contains_point_with_epsilon (const GskBoundingBox   *this,
                                              const graphene_point_t *p,
                                              float                   epsilon)
{
  return this->min.x - epsilon <= p->x && p->x <= this->max.x + epsilon &&
         this->min.y - epsilon <= p->y && p->y <= this->max.y + epsilon;
}

static inline gboolean
gsk_bounding_box_intersection (const GskBoundingBox *a,
                               const GskBoundingBox *b,
                               GskBoundingBox       *res)
{
  graphene_point_t min, max;

  min.x = MAX (a->min.x, b->min.x);
  min.y = MAX (a->min.y, b->min.y);
  max.x = MIN (a->max.x, b->max.x);
  max.y = MIN (a->max.y, b->max.y);

  if (res)
    gsk_bounding_box_init (res, &min, &max);

  return min.x <= max.x && min.y <= max.y;
}

static inline void
gsk_bounding_box_union (const GskBoundingBox *a,
                        const GskBoundingBox *b,
                        GskBoundingBox       *res)
{
  graphene_point_t min, max;

  min.x = MIN (a->min.x, b->min.x);
  min.y = MIN (a->min.y, b->min.y);
  max.x = MAX (a->max.x, b->max.x);
  max.y = MAX (a->max.y, b->max.y);

  gsk_bounding_box_init (res, &min, &max);
}

static inline void
gsk_bounding_box_get_corner (const GskBoundingBox   *b,
                             GskCorner               c,
                             graphene_point_t       *p)
{
  switch (c)
    {
    case GSK_CORNER_TOP_LEFT:
      *p = b->min;
      break;

    case GSK_CORNER_TOP_RIGHT:
      p->x = b->max.x;
      p->y = b->min.y;
      break;

    case GSK_CORNER_BOTTOM_RIGHT:
      *p = b->max;
      break;

    case GSK_CORNER_BOTTOM_LEFT:
      p->x = b->min.x;
      p->y = b->max.y;
      break;

    default:
      g_assert_not_reached ();
    }
}
