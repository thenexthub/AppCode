/*
 * Copyright © 2019 Benjamin Otte
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


/**
 * GskTransform: (ref-fn gsk_transform_ref) (unref-fn gsk_transform_unref)
 *
 * Describes a 3D transform.
 *
 * Unlike `graphene_matrix_t`, `GskTransform` retains the steps in how
 * a transform was constructed, and allows inspecting them. It is modeled
 * after the way CSS describes transforms.
 *
 * `GskTransform` objects are immutable and cannot be changed after creation.
 * This means code can safely expose them as properties of objects without
 * having to worry about others changing them.
 */

#include "config.h"

#include "gsktransformprivate.h"

#include "gskrectprivate.h"

/* {{{ Boilerplate */

struct _GskTransformClass
{
  gsize struct_size;
  const char *type_name;

  void                  (* finalize)            (GskTransform           *transform);
  void                  (* to_matrix)           (GskTransform           *transform,
                                                 graphene_matrix_t      *out_matrix);
  void                  (* apply_2d)            (GskTransform           *transform,
                                                 float                  *out_xx,
                                                 float                  *out_yx,
                                                 float                  *out_xy,
                                                 float                  *out_yy,
                                                 float                  *out_dx,
                                                 float                  *out_dy);
  void                  (* apply_dihedral)      (GskTransform           *transform,
                                                 GdkDihedral            *dihedral,
                                                 float                  *out_scale_x,
                                                 float                  *out_scale_y,
                                                 float                  *out_dx,
                                                 float                  *out_dy);
  void                  (* apply_affine)        (GskTransform           *transform,
                                                 float                  *out_scale_x,
                                                 float                  *out_scale_y,
                                                 float                  *out_dx,
                                                 float                  *out_dy);
  void                  (* apply_translate)     (GskTransform           *transform,
                                                 float                  *out_dx,
                                                 float                  *out_dy);
  void                  (* print)               (GskTransform           *transform,
                                                 GString                *string);
  GskTransform *        (* apply)               (GskTransform           *transform,
                                                 GskTransform           *apply_to);
  GskTransform *        (* invert)              (GskTransform           *transform,
                                                 GskTransform           *next);
  /* both matrices have the same type */
  gboolean              (* equal)               (GskTransform           *first_transform,
                                                 GskTransform           *second_transform);
};

G_DEFINE_BOXED_TYPE (GskTransform, gsk_transform,
                     gsk_transform_ref,
                     gsk_transform_unref)

static gboolean
gsk_transform_is_identity (GskTransform *this);
static GskTransform *
gsk_transform_matrix_with_category (GskTransform             *next,
                                    const graphene_matrix_t  *matrix,
                                    GskFineTransformCategory  category);

static inline gboolean
gsk_transform_has_class (GskTransform            *this,
                         const GskTransformClass *transform_class)
{
  return this != NULL && this->transform_class == transform_class;
}

/*< private >
 * gsk_transform_alloc:
 * @transform_class: class structure for this this
 * @category: The category of this transform. Will be used to initialize
 *   the result's category together with &next's category
 * @next: (transfer full) (nullable): Next transform to multiply with
 *
 * Returns: (transfer full): the newly created `GskTransform`
 */
static gpointer
gsk_transform_alloc (const GskTransformClass   *transform_class,
                     GskFineTransformCategory   category,
                     GskTransform              *next)
{
  GskTransform *this;

  g_return_val_if_fail (transform_class != NULL, NULL);

  this = g_atomic_rc_box_alloc0 (transform_class->struct_size);

  this->transform_class = transform_class;
  this->category = next ? MIN (category, next->category) : category;
  if (gsk_transform_is_identity (next))
    gsk_transform_unref (next);
  else
    this->next = next;

  return this;
}

static void
gsk_transform_finalize (GskTransform *this)
{
  this->transform_class->finalize (this);

  gsk_transform_unref (this->next);
}

/* }}} */
/* {{{ IDENTITY */

static void
gsk_identity_transform_finalize (GskTransform *transform)
{
}

static void
gsk_identity_transform_to_matrix (GskTransform      *transform,
                                  graphene_matrix_t *out_matrix)
{
  graphene_matrix_init_identity (out_matrix);
}

static void
gsk_identity_transform_apply_2d (GskTransform *transform,
                                 float        *out_xx,
                                 float        *out_yx,
                                 float        *out_xy,
                                 float        *out_yy,
                                 float        *out_dx,
                                 float        *out_dy)
{
}

static void
gsk_identity_transform_apply_dihedral (GskTransform *transform,
                                       GdkDihedral  *dihedral,
                                       float        *out_scale_x,
                                       float        *out_scale_y,
                                       float        *out_dx,
                                       float        *out_dy)
{
}

static void
gsk_identity_transform_apply_affine (GskTransform *transform,
                                     float        *out_scale_x,
                                     float        *out_scale_y,
                                     float        *out_dx,
                                     float        *out_dy)
{
}

static void
gsk_identity_transform_apply_translate (GskTransform *transform,
                                        float        *out_dx,
                                        float        *out_dy)
{
}

static void
gsk_identity_transform_print (GskTransform *transform,
                              GString      *string)
{
  g_string_append (string, "none");
}

static GskTransform *
gsk_identity_transform_apply (GskTransform *transform,
                              GskTransform *apply_to)
{
  /* We do the following to make sure inverting a non-NULL transform
   * will return a non-NULL transform.
   */
  if (apply_to)
    return apply_to;
  else
    return gsk_transform_new ();
}

static GskTransform *
gsk_identity_transform_invert (GskTransform *transform,
                               GskTransform *next)
{
  /* We do the following to make sure inverting a non-NULL transform
   * will return a non-NULL transform.
   */
  if (next)
    return next;
  else
    return gsk_transform_new ();
}

static gboolean
gsk_identity_transform_equal (GskTransform *first_transform,
                              GskTransform *second_transform)
{
  return TRUE;
}

static const GskTransformClass GSK_IDENTITY_TRANSFORM_CLASS =
{
  sizeof (GskTransform),
  "GskIdentityTransform",
  gsk_identity_transform_finalize,
  gsk_identity_transform_to_matrix,
  gsk_identity_transform_apply_2d,
  gsk_identity_transform_apply_dihedral,
  gsk_identity_transform_apply_affine,
  gsk_identity_transform_apply_translate,
  gsk_identity_transform_print,
  gsk_identity_transform_apply,
  gsk_identity_transform_invert,
  gsk_identity_transform_equal,
};

/*<private>
 * gsk_transform_is_identity:
 * @transform: (nullable): A transform
 *
 * Checks if the transform is a representation of the identity
 * transform.
 *
 * This is different from a transform like `scale(2) scale(0.5)`
 * which just results in an identity transform when simplified.
 *
 * Returns: %TRUE  if this transform is a representation of
 *   the identity transform
 **/
static gboolean
gsk_transform_is_identity (GskTransform *this)
{
  return this == NULL ||
         (this->transform_class == &GSK_IDENTITY_TRANSFORM_CLASS && gsk_transform_is_identity (this->next));
}

/* }}} */
/* {{{ MATRIX */

typedef struct _GskMatrixTransform GskMatrixTransform;

struct _GskMatrixTransform
{
  GskTransform parent;

  graphene_matrix_t matrix;
};

static void
gsk_matrix_transform_finalize (GskTransform *this)
{
}

static void
gsk_matrix_transform_to_matrix (GskTransform      *transform,
                                graphene_matrix_t *out_matrix)
{
  GskMatrixTransform *this = (GskMatrixTransform *) transform;

  graphene_matrix_init_from_matrix (out_matrix, &this->matrix);
}

static void
gsk_matrix_transform_apply_2d (GskTransform *transform,
                               float        *out_xx,
                               float        *out_yx,
                               float        *out_xy,
                               float        *out_yy,
                               float        *out_dx,
                               float        *out_dy)
{
  GskMatrixTransform *this = (GskMatrixTransform *) transform;
  graphene_matrix_t mat;

  graphene_matrix_init_from_2d (&mat,
                                *out_xx, *out_yx,
                                *out_xy, *out_yy,
                                *out_dx, *out_dy);
  graphene_matrix_multiply (&this->matrix, &mat, &mat);

  /* not using graphene_matrix_to_2d() because it may
   * fail the is_2d() check due to improper rounding */
  *out_xx = graphene_matrix_get_value (&mat, 0, 0);
  *out_yx = graphene_matrix_get_value (&mat, 0, 1);
  *out_xy = graphene_matrix_get_value (&mat, 1, 0);
  *out_yy = graphene_matrix_get_value (&mat, 1, 1);
  *out_dx = graphene_matrix_get_value (&mat, 3, 0);
  *out_dy = graphene_matrix_get_value (&mat, 3, 1);
}

static void
gsk_matrix_transform_apply_dihedral (GskTransform *transform,
                                     GdkDihedral  *out_dihedral,
                                     float        *out_scale_x,
                                     float        *out_scale_y,
                                     float        *out_dx,
                                     float        *out_dy)
{
  GskMatrixTransform *this = (GskMatrixTransform *) transform;

  switch (transform->category)
  {
    case GSK_FINE_TRANSFORM_CATEGORY_UNKNOWN:
    case GSK_FINE_TRANSFORM_CATEGORY_ANY:
    case GSK_FINE_TRANSFORM_CATEGORY_3D:
    case GSK_FINE_TRANSFORM_CATEGORY_2D:
    case GSK_FINE_TRANSFORM_CATEGORY_2D_DIHEDRAL:
    default:
      g_assert_not_reached ();
      break;

    case GSK_FINE_TRANSFORM_CATEGORY_2D_NEGATIVE_AFFINE:
    case GSK_FINE_TRANSFORM_CATEGORY_2D_AFFINE:
      *out_dx += *out_scale_x * graphene_matrix_get_x_translation (&this->matrix);
      *out_dy += *out_scale_y * graphene_matrix_get_y_translation (&this->matrix);
      *out_scale_x *= graphene_matrix_get_x_scale (&this->matrix);
      *out_scale_y *= graphene_matrix_get_y_scale (&this->matrix);
      break;

    case GSK_FINE_TRANSFORM_CATEGORY_2D_TRANSLATE:
      *out_dx += *out_scale_x * graphene_matrix_get_x_translation (&this->matrix);
      *out_dy += *out_scale_y * graphene_matrix_get_y_translation (&this->matrix);
      break;

    case GSK_FINE_TRANSFORM_CATEGORY_IDENTITY:
      break;
  }
}

static void
gsk_matrix_transform_apply_affine (GskTransform *transform,
                                   float        *out_scale_x,
                                   float        *out_scale_y,
                                   float        *out_dx,
                                   float        *out_dy)
{
  GskMatrixTransform *this = (GskMatrixTransform *) transform;

  switch (transform->category)
  {
    case GSK_FINE_TRANSFORM_CATEGORY_UNKNOWN:
    case GSK_FINE_TRANSFORM_CATEGORY_ANY:
    case GSK_FINE_TRANSFORM_CATEGORY_3D:
    case GSK_FINE_TRANSFORM_CATEGORY_2D:
    case GSK_FINE_TRANSFORM_CATEGORY_2D_DIHEDRAL:
    default:
      g_assert_not_reached ();
      break;

    case GSK_FINE_TRANSFORM_CATEGORY_2D_NEGATIVE_AFFINE:
    case GSK_FINE_TRANSFORM_CATEGORY_2D_AFFINE:
      *out_dx += *out_scale_x * graphene_matrix_get_x_translation (&this->matrix);
      *out_dy += *out_scale_y * graphene_matrix_get_y_translation (&this->matrix);
      *out_scale_x *= graphene_matrix_get_x_scale (&this->matrix);
      *out_scale_y *= graphene_matrix_get_y_scale (&this->matrix);
      break;

    case GSK_FINE_TRANSFORM_CATEGORY_2D_TRANSLATE:
      *out_dx += *out_scale_x * graphene_matrix_get_x_translation (&this->matrix);
      *out_dy += *out_scale_y * graphene_matrix_get_y_translation (&this->matrix);
      break;

    case GSK_FINE_TRANSFORM_CATEGORY_IDENTITY:
      break;
  }
}

static void
gsk_matrix_transform_apply_translate (GskTransform *transform,
                                      float        *out_dx,
                                      float        *out_dy)
{
  GskMatrixTransform *this = (GskMatrixTransform *) transform;

  switch (transform->category)
  {
    case GSK_FINE_TRANSFORM_CATEGORY_UNKNOWN:
    case GSK_FINE_TRANSFORM_CATEGORY_ANY:
    case GSK_FINE_TRANSFORM_CATEGORY_3D:
    case GSK_FINE_TRANSFORM_CATEGORY_2D:
    case GSK_FINE_TRANSFORM_CATEGORY_2D_DIHEDRAL:
    case GSK_FINE_TRANSFORM_CATEGORY_2D_NEGATIVE_AFFINE:
    case GSK_FINE_TRANSFORM_CATEGORY_2D_AFFINE:
    default:
      g_assert_not_reached ();
      break;

    case GSK_FINE_TRANSFORM_CATEGORY_2D_TRANSLATE:
      *out_dx += graphene_matrix_get_x_translation (&this->matrix);
      *out_dy += graphene_matrix_get_y_translation (&this->matrix);
      break;

    case GSK_FINE_TRANSFORM_CATEGORY_IDENTITY:
      break;
  }
}

static void
string_append_double (GString *string,
                      double   d)
{
  char buf[G_ASCII_DTOSTR_BUF_SIZE];

  g_ascii_formatd (buf, G_ASCII_DTOSTR_BUF_SIZE, "%g", d);
  g_string_append (string, buf);
}

static void
gsk_matrix_transform_print (GskTransform *transform,
                            GString      *string)
{
  GskMatrixTransform *this = (GskMatrixTransform *) transform;
  guint i;
  float f[16];

  if (transform->category >= GSK_FINE_TRANSFORM_CATEGORY_2D)
    {
      g_string_append (string, "matrix(");
      graphene_matrix_to_float (&this->matrix, f);
      string_append_double (string, f[0]);
      g_string_append (string, ", ");
      string_append_double (string, f[1]);
      g_string_append (string, ", ");
      string_append_double (string, f[4]);
      g_string_append (string, ", ");
      string_append_double (string, f[5]);
      g_string_append (string, ", ");
      string_append_double (string, f[12]);
      g_string_append (string, ", ");
      string_append_double (string, f[13]);
      g_string_append (string, ")");
    }
  else
    {
      g_string_append (string, "matrix3d(");
      graphene_matrix_to_float (&this->matrix, f);
      for (i = 0; i < 16; i++)
        {
          if (i > 0)
            g_string_append (string, ", ");
          string_append_double (string, f[i]);
        }
      g_string_append (string, ")");
    }
}

static GskTransform *
gsk_matrix_transform_apply (GskTransform *transform,
                            GskTransform *apply_to)
{
  GskMatrixTransform *this = (GskMatrixTransform *) transform;

  return gsk_transform_matrix_with_category (apply_to,
                                             &this->matrix,
                                             transform->category);
}

static GskTransform *
gsk_matrix_transform_invert (GskTransform *transform,
                             GskTransform *next)
{
  GskMatrixTransform *this = (GskMatrixTransform *) transform;
  graphene_matrix_t inverse;

  if (!graphene_matrix_inverse (&this->matrix, &inverse))
    {
      gsk_transform_unref (next);
      return NULL;
    }

  return gsk_transform_matrix_with_category (next,
                                             &inverse,
                                             transform->category);
}

static gboolean
gsk_matrix_transform_equal (GskTransform *first_transform,
                            GskTransform *second_transform)
{
  GskMatrixTransform *first = (GskMatrixTransform *) first_transform;
  GskMatrixTransform *second = (GskMatrixTransform *) second_transform;

  if (graphene_matrix_equal_fast (&first->matrix, &second->matrix))
    return TRUE;

  return graphene_matrix_equal (&first->matrix, &second->matrix);
}

static const GskTransformClass GSK_TRANSFORM_TRANSFORM_CLASS =
{
  sizeof (GskMatrixTransform),
  "GskMatrixTransform",
  gsk_matrix_transform_finalize,
  gsk_matrix_transform_to_matrix,
  gsk_matrix_transform_apply_2d,
  gsk_matrix_transform_apply_dihedral,
  gsk_matrix_transform_apply_affine,
  gsk_matrix_transform_apply_translate,
  gsk_matrix_transform_print,
  gsk_matrix_transform_apply,
  gsk_matrix_transform_invert,
  gsk_matrix_transform_equal,
};

static GskTransform *
gsk_transform_matrix_with_category (GskTransform             *next,
                                    const graphene_matrix_t  *matrix,
                                    GskFineTransformCategory  category)
{
  GskMatrixTransform *result = gsk_transform_alloc (&GSK_TRANSFORM_TRANSFORM_CLASS, category, next);

  /* We can't deal with these yet - also because lots of code gets transposing wrong */
  g_assert (category != GSK_FINE_TRANSFORM_CATEGORY_2D_DIHEDRAL);

  graphene_matrix_init_from_matrix (&result->matrix, matrix);

  return &result->parent;
}

/**
 * gsk_transform_matrix:
 * @next: (nullable) (transfer full): the next transform
 * @matrix: the matrix to multiply @next with
 *
 * Multiplies @next with the given @matrix.
 *
 * This function consumes @next. Use [method@Gsk.Transform.ref] first
 * if you want to keep it around.
 *
 * Returns: The new transform
 **/
GskTransform *
gsk_transform_matrix (GskTransform            *next,
                      const graphene_matrix_t *matrix)
{
  return gsk_transform_matrix_with_category (next, matrix, GSK_FINE_TRANSFORM_CATEGORY_UNKNOWN);
}

/* }}} */
/* {{{ TRANSLATE */

typedef struct _GskTranslateTransform GskTranslateTransform;

struct _GskTranslateTransform
{
  GskTransform parent;

  graphene_point3d_t point;
};

static void
gsk_translate_transform_finalize (GskTransform *this)
{
}

static void
gsk_translate_transform_to_matrix (GskTransform      *transform,
                                   graphene_matrix_t *out_matrix)
{
  GskTranslateTransform *this = (GskTranslateTransform *) transform;

  graphene_matrix_init_translate (out_matrix, &this->point);
}

static void
gsk_translate_transform_apply_2d (GskTransform *transform,
                                  float        *out_xx,
                                  float        *out_yx,
                                  float        *out_xy,
                                  float        *out_yy,
                                  float        *out_dx,
                                  float        *out_dy)
{
  GskTranslateTransform *this = (GskTranslateTransform *) transform;

  g_assert (this->point.z == 0.0);

  *out_dx += *out_xx * this->point.x + *out_xy * this->point.y;
  *out_dy += *out_yx * this->point.x + *out_yy * this->point.y;
}

static void
gsk_translate_transform_apply_dihedral (GskTransform *transform,
                                        GdkDihedral  *out_dihedral,
                                        float        *out_scale_x,
                                        float        *out_scale_y,
                                        float        *out_dx,
                                        float        *out_dy)
{
  GskTranslateTransform *this = (GskTranslateTransform *) transform;
  float xx, xy, yx, yy;

  g_assert (this->point.z == 0.0);

  gdk_dihedral_get_mat2 (*out_dihedral, &xx, &xy, &yx, &yy);

  *out_dx += *out_scale_x * (xx * this->point.x + xy * this->point.y);
  *out_dy += *out_scale_y * (yx * this->point.x + yy * this->point.y);
}

static void
gsk_translate_transform_apply_affine (GskTransform *transform,
                                      float        *out_scale_x,
                                      float        *out_scale_y,
                                      float        *out_dx,
                                      float        *out_dy)
{
  GskTranslateTransform *this = (GskTranslateTransform *) transform;

  g_assert (this->point.z == 0.0);

  *out_dx += *out_scale_x * this->point.x;
  *out_dy += *out_scale_y * this->point.y;
}

static void
gsk_translate_transform_apply_translate (GskTransform *transform,
                                         float        *out_dx,
                                         float        *out_dy)
{
  GskTranslateTransform *this = (GskTranslateTransform *) transform;

  g_assert (this->point.z == 0.0);

  *out_dx += this->point.x;
  *out_dy += this->point.y;
}

static GskTransform *
gsk_translate_transform_apply (GskTransform *transform,
                               GskTransform *apply_to)
{
  GskTranslateTransform *this = (GskTranslateTransform *) transform;

  return gsk_transform_translate_3d (apply_to, &this->point);
}

static GskTransform *
gsk_translate_transform_invert (GskTransform *transform,
                                GskTransform *next)
{
  GskTranslateTransform *this = (GskTranslateTransform *) transform;

  return gsk_transform_translate_3d (next, &GRAPHENE_POINT3D_INIT (-this->point.x, -this->point.y, -this->point.z));
}

static gboolean
gsk_translate_transform_equal (GskTransform *first_transform,
                               GskTransform *second_transform)
{
  GskTranslateTransform *first = (GskTranslateTransform *) first_transform;
  GskTranslateTransform *second = (GskTranslateTransform *) second_transform;

  return G_APPROX_VALUE (first->point.x, second->point.x, FLT_EPSILON) &&
         G_APPROX_VALUE (first->point.y, second->point.y, FLT_EPSILON) &&
         G_APPROX_VALUE (first->point.z, second->point.z, FLT_EPSILON);
}

static void
gsk_translate_transform_print (GskTransform *transform,
                               GString      *string)
{
  GskTranslateTransform *this = (GskTranslateTransform *) transform;

  if (this->point.z == 0)
    g_string_append (string, "translate(");
  else
    g_string_append (string, "translate3d(");

  string_append_double (string, this->point.x);
  g_string_append (string, ", ");
  string_append_double (string, this->point.y);
  if (this->point.z != 0)
    {
      g_string_append (string, ", ");
      string_append_double (string, this->point.z);
    }
  g_string_append (string, ")");
}

static const GskTransformClass GSK_TRANSLATE_TRANSFORM_CLASS =
{
  sizeof (GskTranslateTransform),
  "GskTranslateTransform",
  gsk_translate_transform_finalize,
  gsk_translate_transform_to_matrix,
  gsk_translate_transform_apply_2d,
  gsk_translate_transform_apply_dihedral,
  gsk_translate_transform_apply_affine,
  gsk_translate_transform_apply_translate,
  gsk_translate_transform_print,
  gsk_translate_transform_apply,
  gsk_translate_transform_invert,
  gsk_translate_transform_equal,
};

/**
 * gsk_transform_translate:
 * @next: (nullable) (transfer full): the next transform
 * @point: the point to translate the transform by
 *
 * Translates @next in 2-dimensional space by @point.
 *
 * This function consumes @next. Use [method@Gsk.Transform.ref] first
 * if you want to keep it around.
 *
 * Returns: (nullable): The new transform
 **/
GskTransform *
gsk_transform_translate (GskTransform           *next,
                         const graphene_point_t *point)
{
  graphene_point3d_t point3d;

  graphene_point3d_init (&point3d, point->x, point->y, 0);

  return gsk_transform_translate_3d (next, &point3d);
}

/**
 * gsk_transform_translate_3d:
 * @next: (nullable) (transfer full): the next transform
 * @point: the point to translate the transform by
 *
 * Translates @next by @point.
 *
 * This function consumes @next. Use [method@Gsk.Transform.ref] first
 * if you want to keep it around.
 *
 * Returns: (nullable): The new transform
 **/
GskTransform *
gsk_transform_translate_3d (GskTransform             *next,
                            const graphene_point3d_t *point)
{
  GskTranslateTransform *result;

  if (graphene_point3d_equal (point, graphene_point3d_zero ()))
    return next;

  if (gsk_transform_has_class (next, &GSK_TRANSLATE_TRANSFORM_CLASS))
    {
      GskTranslateTransform *t = (GskTranslateTransform *) next;
      GskTransform *r = gsk_transform_translate_3d (gsk_transform_ref (next->next),
                                                    &GRAPHENE_POINT3D_INIT(t->point.x + point->x,
                                                                           t->point.y + point->y,
                                                                           t->point.z + point->z));
      gsk_transform_unref (next);
      return r;
    }

  result = gsk_transform_alloc (&GSK_TRANSLATE_TRANSFORM_CLASS,
                                point->z == 0.0 ? GSK_FINE_TRANSFORM_CATEGORY_2D_TRANSLATE
                                                : GSK_FINE_TRANSFORM_CATEGORY_3D,
                                next);

  graphene_point3d_init_from_point (&result->point, point);

  return &result->parent;
}

/* }}} */
/* {{{ ROTATE */

typedef struct _GskRotateTransform GskRotateTransform;

struct _GskRotateTransform
{
  GskTransform parent;

  float angle;
};

static void
gsk_rotate_transform_finalize (GskTransform *this)
{
}

static inline void
_sincos (float  deg,
         float *out_s,
         float *out_c)
{
  if (deg == 90.0)
    {
      *out_c = 0.0;
      *out_s = 1.0;
    }
  else if (deg == 180.0)
    {
      *out_c = -1.0;
      *out_s = 0.0;
    }
  else if (deg == 270.0)
    {
      *out_c = 0.0;
      *out_s = -1.0;
    }
  else if (deg == 0.0)
    {
      *out_c = 1.0;
      *out_s = 0.0;
    }
  else
    {
      float angle = deg * M_PI / 180.0;

#ifdef HAVE_SINCOSF
      sincosf (angle, out_s, out_c);
#else
      *out_s = sinf (angle);
      *out_c = cosf (angle);
#endif

    }
}

static void
gsk_rotate_transform_to_matrix (GskTransform      *transform,
                                graphene_matrix_t *out_matrix)
{
  GskRotateTransform *this = (GskRotateTransform *) transform;
  float c, s;

  _sincos (this->angle, &s, &c);

  graphene_matrix_init_from_2d (out_matrix,
                                c, s,
                                -s, c,
                                0, 0);
}

static void
gsk_rotate_transform_apply_2d (GskTransform *transform,
                               float        *out_xx,
                               float        *out_yx,
                               float        *out_xy,
                               float        *out_yy,
                               float        *out_dx,
                               float        *out_dy)
{
  GskRotateTransform *this = (GskRotateTransform *) transform;
  float s, c, xx, xy, yx, yy;

  _sincos (this->angle, &s, &c);

  xx =  c * *out_xx + s * *out_xy;
  yx =  c * *out_yx + s * *out_yy;
  xy = -s * *out_xx + c * *out_xy;
  yy = -s * *out_yx + c * *out_yy;

  *out_xx = xx;
  *out_yx = yx;
  *out_xy = xy;
  *out_yy = yy;
}

static void
gsk_rotate_transform_apply_dihedral (GskTransform *transform,
                                     GdkDihedral  *out_dihedral,
                                     float        *out_scale_x,
                                     float        *out_scale_y,
                                     float        *out_dx,
                                     float        *out_dy)
{
  GskRotateTransform *this = (GskRotateTransform *) transform;
  GdkDihedral dihedral;

  dihedral = (int) this->angle / 90;
  g_assert (dihedral >= GDK_DIHEDRAL_NORMAL && dihedral < GDK_DIHEDRAL_FLIPPED);

  *out_dihedral = gdk_dihedral_combine (dihedral, *out_dihedral);
}

static GskTransform *
gsk_rotate_transform_apply (GskTransform *transform,
                            GskTransform *apply_to)
{
  GskRotateTransform *this = (GskRotateTransform *) transform;

  return gsk_transform_rotate (apply_to, this->angle);
}

static GskTransform *
gsk_rotate_transform_invert (GskTransform *transform,
                             GskTransform *next)
{
  GskRotateTransform *this = (GskRotateTransform *) transform;

  return gsk_transform_rotate (next, - this->angle);
}

static gboolean
gsk_rotate_transform_equal (GskTransform *first_transform,
                            GskTransform *second_transform)
{
  GskRotateTransform *first = (GskRotateTransform *) first_transform;
  GskRotateTransform *second = (GskRotateTransform *) second_transform;

  return G_APPROX_VALUE (first->angle, second->angle, 0.01f);
}

static void
gsk_rotate_transform_print (GskTransform *transform,
                            GString      *string)
{
  GskRotateTransform *this = (GskRotateTransform *) transform;

  g_string_append (string, "rotate(");
  string_append_double (string, this->angle);
  g_string_append (string, ")");
}

static const GskTransformClass GSK_ROTATE_TRANSFORM_CLASS =
{
  sizeof (GskRotateTransform),
  "GskRotateTransform",
  gsk_rotate_transform_finalize,
  gsk_rotate_transform_to_matrix,
  gsk_rotate_transform_apply_2d,
  gsk_rotate_transform_apply_dihedral,
  NULL,
  NULL,
  gsk_rotate_transform_print,
  gsk_rotate_transform_apply,
  gsk_rotate_transform_invert,
  gsk_rotate_transform_equal,
};

static inline float
normalize_angle (float angle)
{

  if (angle >= 0 && angle < 360)
    return angle;

  angle = fmodf (angle, 360);
  if (angle < 0)
    angle += 360;

  /* Due to precision issues we may end up with a result that is just
   * past the allowed range when rounded. For example, something like
   * -epsilon + 360 when rounded to a float may end up with 360.
   * So, we handle these cases by returning the exact value 0.
   */

  if (angle >= 360)
    angle = 0;

  g_assert (angle < 360.0);
  g_assert (angle >= 0.0);

  return angle;
}

/**
 * gsk_transform_rotate:
 * @next: (nullable) (transfer full): the next transform
 * @angle: the rotation angle, in degrees (clockwise)
 *
 * Rotates @next by an angle around the Z axis.
 *
 * The rotation happens around the origin point of (0, 0).
 *
 * This function consumes @next. Use [method@Gsk.Transform.ref] first
 * if you want to keep it around.
 *
 * Returns: (nullable): The new transform
 */
GskTransform *
gsk_transform_rotate (GskTransform *next,
                      float         angle)
{
  GskRotateTransform *result;

  angle = normalize_angle (angle);
  if (angle == 0.0f)
    return next;

  if (gsk_transform_has_class (next, &GSK_ROTATE_TRANSFORM_CLASS))
    {
      GskTransform *r  = gsk_transform_rotate (gsk_transform_ref (next->next),
                                               ((GskRotateTransform *) next)->angle + angle);
      gsk_transform_unref (next);
      return r;
    }

  result = gsk_transform_alloc (&GSK_ROTATE_TRANSFORM_CLASS,
                                fmodf (angle, 90.0) ? GSK_FINE_TRANSFORM_CATEGORY_2D
                                                    : GSK_FINE_TRANSFORM_CATEGORY_2D_DIHEDRAL,
                                next);

  result->angle = angle;

  return &result->parent;
}

/* }}} */
/* {{{ ROTATE 3D */

typedef struct _GskRotate3dTransform GskRotate3dTransform;

struct _GskRotate3dTransform
{
  GskTransform parent;

  float angle;
  graphene_vec3_t axis;
};

static void
gsk_rotate3d_transform_finalize (GskTransform *this)
{
}

static void
gsk_rotate3d_transform_to_matrix (GskTransform      *transform,
                                  graphene_matrix_t *out_matrix)
{
  GskRotate3dTransform *this = (GskRotate3dTransform *) transform;

  graphene_matrix_init_rotate (out_matrix, this->angle, &this->axis);
}

static GskTransform *
gsk_rotate3d_transform_apply (GskTransform *transform,
                              GskTransform *apply_to)
{
  GskRotate3dTransform *this = (GskRotate3dTransform *) transform;

  return gsk_transform_rotate_3d (apply_to, this->angle, &this->axis);
}

static GskTransform *
gsk_rotate3d_transform_invert (GskTransform *transform,
                               GskTransform *next)
{
  GskRotate3dTransform *this = (GskRotate3dTransform *) transform;

  return gsk_transform_rotate_3d (next, - this->angle, &this->axis);
}

static gboolean
gsk_rotate3d_transform_equal (GskTransform *first_transform,
                              GskTransform *second_transform)
{
  GskRotate3dTransform *first = (GskRotate3dTransform *) first_transform;
  GskRotate3dTransform *second = (GskRotate3dTransform *) second_transform;

  return G_APPROX_VALUE (first->angle, second->angle, 0.01f) &&
         graphene_vec3_equal (&first->axis, &second->axis);
}

static void
gsk_rotate3d_transform_print (GskTransform *transform,
                            GString      *string)
{
  GskRotate3dTransform *this = (GskRotate3dTransform *) transform;
  float f[3];
  guint i;

  g_string_append (string, "rotate3d(");
  graphene_vec3_to_float (&this->axis, f);
  for (i = 0; i < 3; i++)
    {
      string_append_double (string, f[i]);
      g_string_append (string, ", ");
    }
  string_append_double (string, this->angle);
  g_string_append (string, ")");
}

static const GskTransformClass GSK_ROTATE3D_TRANSFORM_CLASS =
{
  sizeof (GskRotate3dTransform),
  "GskRotate3dTransform",
  gsk_rotate3d_transform_finalize,
  gsk_rotate3d_transform_to_matrix,
  NULL,
  NULL,
  NULL,
  NULL,
  gsk_rotate3d_transform_print,
  gsk_rotate3d_transform_apply,
  gsk_rotate3d_transform_invert,
  gsk_rotate3d_transform_equal,
};

/**
 * gsk_transform_rotate_3d:
 * @next: (nullable) (transfer full): the next transform
 * @angle: the rotation angle, in degrees (clockwise)
 * @axis: The rotation axis
 *
 * Rotates @next @angle degrees around @axis.
 *
 * For a rotation in 2D space, use [method@Gsk.Transform.rotate]
 *
 * This function consumes @next. Use [method@Gsk.Transform.ref] first
 * if you want to keep it around.
 *
 * Returns: (nullable): The new transform
 */
GskTransform *
gsk_transform_rotate_3d (GskTransform          *next,
                         float                  angle,
                         const graphene_vec3_t *axis)
{
  GskRotate3dTransform *result;

  angle = normalize_angle (angle);
  if (angle == 0.0f)
    return next;

  if (graphene_vec3_get_x (axis) == 0.0 && graphene_vec3_get_y (axis) == 0.0)
    return gsk_transform_rotate (next, angle);

  result = gsk_transform_alloc (&GSK_ROTATE3D_TRANSFORM_CLASS,
                                GSK_FINE_TRANSFORM_CATEGORY_3D,
                                next);

  result->angle = angle;
  graphene_vec3_init_from_vec3 (&result->axis, axis);

  return &result->parent;
}

/* }}} */
/* {{{ SKEW */

typedef struct _GskSkewTransform GskSkewTransform;

struct _GskSkewTransform
{
  GskTransform parent;

  float skew_x;
  float skew_y;
};

static void
gsk_skew_transform_finalize (GskTransform *this)
{
}

#define DEG_TO_RAD(x) ((x) / 180.f * G_PI)
#define RAD_TO_DEG(x) ((x) * 180.f / G_PI)

static void
gsk_skew_transform_to_matrix (GskTransform      *transform,
                              graphene_matrix_t *out_matrix)
{
  GskSkewTransform *this = (GskSkewTransform *) transform;

  graphene_matrix_init_skew (out_matrix,
                             DEG_TO_RAD (this->skew_x),
                             DEG_TO_RAD (this->skew_y));
}

static void
gsk_skew_transform_apply_2d (GskTransform *transform,
                             float        *out_xx,
                             float        *out_yx,
                             float        *out_xy,
                             float        *out_yy,
                             float        *out_dx,
                             float        *out_dy)
{
  graphene_matrix_t sm, mat;

  gsk_skew_transform_to_matrix (transform, &sm);
  graphene_matrix_init_from_2d (&mat, *out_xx, *out_yx,
                                      *out_xy, *out_yy,
                                      *out_dx, *out_dy);

  graphene_matrix_multiply (&sm, &mat, &mat);

  *out_xx = graphene_matrix_get_value (&mat, 0, 0);
  *out_yx = graphene_matrix_get_value (&mat, 0, 1);
  *out_xy = graphene_matrix_get_value (&mat, 1, 0);
  *out_yy = graphene_matrix_get_value (&mat, 1, 1);
  *out_dx = graphene_matrix_get_value (&mat, 3, 0);
  *out_dy = graphene_matrix_get_value (&mat, 3, 1);
}

static GskTransform *
gsk_skew_transform_apply (GskTransform *transform,
                          GskTransform *apply_to)
{
  GskSkewTransform *this = (GskSkewTransform *) transform;

  return gsk_transform_skew (apply_to, this->skew_x, this->skew_y);
}

static void
gsk_skew_transform_print (GskTransform *transform,
                          GString      *string)
{
  GskSkewTransform *this = (GskSkewTransform *) transform;

  if (this->skew_y == 0)
    {
      g_string_append (string, "skewX(");
      string_append_double (string, this->skew_x);
      g_string_append (string, ")");
    }
  else if (this->skew_x == 0)
    {
      g_string_append (string, "skewY(");
      string_append_double (string, this->skew_y);
      g_string_append (string, ")");
    }
  else
    {
      g_string_append (string, "skew(");
      string_append_double (string, this->skew_x);
      g_string_append (string, ", ");
      string_append_double (string, this->skew_y);
      g_string_append (string, ")");
    }
}

static GskTransform *
gsk_skew_transform_invert (GskTransform *transform,
                           GskTransform *next)
{
  GskSkewTransform *this = (GskSkewTransform *) transform;
  float tx, ty;
  graphene_matrix_t matrix;

  tx = tanf (DEG_TO_RAD (this->skew_x));
  ty = tanf (DEG_TO_RAD (this->skew_y));

  graphene_matrix_init_from_2d (&matrix,
                                1 / (1 - tx * ty),
                                - ty / (1 - tx * ty),
                                - tx / (1 - tx * ty),
                                1 / (1 - tx * ty),
                                0, 0);
  return gsk_transform_matrix_with_category (next,
                                             &matrix,
                                             GSK_FINE_TRANSFORM_CATEGORY_2D);
}

static gboolean
gsk_skew_transform_equal (GskTransform *first_transform,
                          GskTransform *second_transform)
{
  GskSkewTransform *first = (GskSkewTransform *) first_transform;
  GskSkewTransform *second = (GskSkewTransform *) second_transform;

  return G_APPROX_VALUE (first->skew_x, second->skew_x, FLT_EPSILON) &&
         G_APPROX_VALUE (first->skew_y, second->skew_y, FLT_EPSILON);
}

static const GskTransformClass GSK_SKEW_TRANSFORM_CLASS =
{
  sizeof (GskSkewTransform),
  "GskSkewTransform",
  gsk_skew_transform_finalize,
  gsk_skew_transform_to_matrix,
  gsk_skew_transform_apply_2d,
  NULL,
  NULL,
  NULL,
  gsk_skew_transform_print,
  gsk_skew_transform_apply,
  gsk_skew_transform_invert,
  gsk_skew_transform_equal,
};

/**
 * gsk_transform_skew:
 * @next: (nullable) (transfer full): the next transform
 * @skew_x: skew factor, in degrees, on the X axis
 * @skew_y: skew factor, in degrees, on the Y axis
 *
 * Applies a skew transform.
 *
 * This function consumes @next. Use [method@Gsk.Transform.ref] first
 * if you want to keep it around.
 *
 * Returns: (nullable): The new transform
 *
 * Since: 4.6
 */
GskTransform *
gsk_transform_skew (GskTransform *next,
                    float         skew_x,
                    float         skew_y)
{
  GskSkewTransform *result;

  if (skew_x == 0 && skew_y == 0)
    return next;

  result = gsk_transform_alloc (&GSK_SKEW_TRANSFORM_CLASS,
                                GSK_FINE_TRANSFORM_CATEGORY_2D,
                                next);

  result->skew_x = skew_x;
  result->skew_y = skew_y;

  return &result->parent;
}
/* }}} */
/*  {{{ SCALE */

typedef struct _GskScaleTransform GskScaleTransform;

struct _GskScaleTransform
{
  GskTransform parent;

  float factor_x;
  float factor_y;
  float factor_z;
};

static void
gsk_scale_transform_finalize (GskTransform *this)
{
}

static void
gsk_scale_transform_to_matrix (GskTransform      *transform,
                               graphene_matrix_t *out_matrix)
{
  GskScaleTransform *this = (GskScaleTransform *) transform;

  graphene_matrix_init_scale (out_matrix, this->factor_x, this->factor_y, this->factor_z);
}

static void
gsk_scale_transform_apply_2d (GskTransform *transform,
                              float        *out_xx,
                              float        *out_yx,
                              float        *out_xy,
                              float        *out_yy,
                              float        *out_dx,
                              float        *out_dy)
{
  GskScaleTransform *this = (GskScaleTransform *) transform;

  g_assert (this->factor_z == 1.0);

  *out_xx *= this->factor_x;
  *out_yx *= this->factor_x;
  *out_xy *= this->factor_y;
  *out_yy *= this->factor_y;
}

static void
gsk_scale_transform_apply_dihedral (GskTransform *transform,
                                    GdkDihedral  *out_dihedral,
                                    float        *out_scale_x,
                                    float        *out_scale_y,
                                    float        *out_dx,
                                    float        *out_dy)
{
  GskScaleTransform *this = (GskScaleTransform *) transform;
  GdkDihedral dihedral;
  float scale_x, scale_y;

  g_assert (this->factor_z == 1.0);

  if (gdk_dihedral_swaps_xy (*out_dihedral))
    {
      scale_x = fabs (this->factor_y);
      scale_y = fabs (this->factor_x);
    }
  else
    {
      scale_x = fabs (this->factor_x);
      scale_y = fabs (this->factor_y);
    }

  if (this->factor_x >= 0)
    {
      if (this->factor_y >= 0)
        dihedral = GDK_DIHEDRAL_NORMAL;
      else
        dihedral = GDK_DIHEDRAL_FLIPPED_180;
    }
  else
    {
      if (this->factor_y >= 0)
        dihedral = GDK_DIHEDRAL_FLIPPED;
      else
        dihedral = GDK_DIHEDRAL_180;
    }

  *out_dihedral = gdk_dihedral_combine (dihedral, *out_dihedral);
  *out_scale_x *= scale_x;
  *out_scale_y *= scale_y;
}

static void
gsk_scale_transform_apply_affine (GskTransform *transform,
                                  float        *out_scale_x,
                                  float        *out_scale_y,
                                  float        *out_dx,
                                  float        *out_dy)
{
  GskScaleTransform *this = (GskScaleTransform *) transform;

  g_assert (this->factor_z == 1.0);

  *out_scale_x *= this->factor_x;
  *out_scale_y *= this->factor_y;
}

static GskTransform *
gsk_scale_transform_apply (GskTransform *transform,
                           GskTransform *apply_to)
{
  GskScaleTransform *this = (GskScaleTransform *) transform;

  return gsk_transform_scale_3d (apply_to, this->factor_x, this->factor_y, this->factor_z);
}

static GskTransform *
gsk_scale_transform_invert (GskTransform *transform,
                            GskTransform *next)
{
  GskScaleTransform *this = (GskScaleTransform *) transform;

  return gsk_transform_scale_3d (next,
                                 1.f / this->factor_x,
                                 1.f / this->factor_y,
                                 1.f / this->factor_z);
}

static gboolean
gsk_scale_transform_equal (GskTransform *first_transform,
                           GskTransform *second_transform)
{
  GskScaleTransform *first = (GskScaleTransform *) first_transform;
  GskScaleTransform *second = (GskScaleTransform *) second_transform;

  return G_APPROX_VALUE (first->factor_x, second->factor_x, FLT_EPSILON) &&
         G_APPROX_VALUE (first->factor_y, second->factor_y, FLT_EPSILON) &&
         G_APPROX_VALUE (first->factor_z, second->factor_z, FLT_EPSILON);
}

static void
gsk_scale_transform_print (GskTransform *transform,
                           GString      *string)
{
  GskScaleTransform *this = (GskScaleTransform *) transform;

  if (this->factor_z == 1.0)
    {
      g_string_append (string, "scale(");
      string_append_double (string, this->factor_x);
      if (this->factor_x != this->factor_y)
        {
          g_string_append (string, ", ");
          string_append_double (string, this->factor_y);
        }
      g_string_append (string, ")");
    }
  else
    {
      g_string_append (string, "scale3d(");
      string_append_double (string, this->factor_x);
      g_string_append (string, ", ");
      string_append_double (string, this->factor_y);
      g_string_append (string, ", ");
      string_append_double (string, this->factor_z);
      g_string_append (string, ")");
    }
}

static const GskTransformClass GSK_SCALE_TRANSFORM_CLASS =
{
  sizeof (GskScaleTransform),
  "GskScaleTransform",
  gsk_scale_transform_finalize,
  gsk_scale_transform_to_matrix,
  gsk_scale_transform_apply_2d,
  gsk_scale_transform_apply_dihedral,
  gsk_scale_transform_apply_affine,
  NULL,
  gsk_scale_transform_print,
  gsk_scale_transform_apply,
  gsk_scale_transform_invert,
  gsk_scale_transform_equal,
};

/**
 * gsk_transform_scale:
 * @next: (nullable) (transfer full): the next transform
 * @factor_x: scaling factor on the X axis
 * @factor_y: scaling factor on the Y axis
 *
 * Scales @next in 2-dimensional space by the given factors.
 *
 * Use [method@Gsk.Transform.scale_3d] to scale in all 3 dimensions.
 *
 * This function consumes @next. Use [method@Gsk.Transform.ref] first
 * if you want to keep it around.
 *
 * Returns: (nullable): The new transform
 **/
GskTransform *
gsk_transform_scale (GskTransform *next,
                     float         factor_x,
                     float         factor_y)
{
  return gsk_transform_scale_3d (next, factor_x, factor_y, 1.0);
}

/**
 * gsk_transform_scale_3d:
 * @next: (nullable) (transfer full): the next transform
 * @factor_x: scaling factor on the X axis
 * @factor_y: scaling factor on the Y axis
 * @factor_z: scaling factor on the Z axis
 *
 * Scales @next by the given factors.
 *
 * This function consumes @next. Use [method@Gsk.Transform.ref] first
 * if you want to keep it around.
 *
 * Returns: (nullable): The new transform
 **/
GskTransform *
gsk_transform_scale_3d (GskTransform *next,
                        float         factor_x,
                        float         factor_y,
                        float         factor_z)
{
  GskScaleTransform *result;
  GskFineTransformCategory category;

  if (factor_x == 1 && factor_y == 1 && factor_z == 1)
    return next;

  if (gsk_transform_has_class (next, &GSK_SCALE_TRANSFORM_CLASS))
    {
      GskScaleTransform *scale = (GskScaleTransform *) next;
      GskTransform *r = gsk_transform_scale_3d (gsk_transform_ref (next->next),
                                                scale->factor_x * factor_x,
                                                scale->factor_y * factor_y,
                                                scale->factor_z * factor_z);
      gsk_transform_unref (next);
      return r;
    }

  if (factor_z != 1.0f)
    category = GSK_FINE_TRANSFORM_CATEGORY_3D;
  else if (factor_x < 0.0f || factor_y < 0.0f)
    category = GSK_FINE_TRANSFORM_CATEGORY_2D_NEGATIVE_AFFINE;
  else
    category = GSK_FINE_TRANSFORM_CATEGORY_2D_AFFINE;

  result = gsk_transform_alloc (&GSK_SCALE_TRANSFORM_CLASS,
                                category,
                                next);

  result->factor_x = factor_x;
  result->factor_y = factor_y;
  result->factor_z = factor_z;

  return &result->parent;
}

/* }}} */
/* {{{ PERSPECTIVE */

typedef struct _GskPerspectiveTransform GskPerspectiveTransform;

struct _GskPerspectiveTransform
{
  GskTransform parent;

  float depth;
};

static void
gsk_perspective_transform_finalize (GskTransform *this)
{
}

static void
gsk_perspective_transform_to_matrix (GskTransform      *transform,
                                     graphene_matrix_t *out_matrix)
{
  GskPerspectiveTransform *this = (GskPerspectiveTransform *) transform;
  float f[16] = { 1.f, 0.f, 0.f,  0.f,
                  0.f, 1.f, 0.f,  0.f,
                  0.f, 0.f, 1.f, this->depth ? -1.f / this->depth : 0.f,
                  0.f, 0.f, 0.f,  1.f };

  graphene_matrix_init_from_float (out_matrix, f);
}


static GskTransform *
gsk_perspective_transform_apply (GskTransform *transform,
                                 GskTransform *apply_to)
{
  GskPerspectiveTransform *this = (GskPerspectiveTransform *) transform;

  return gsk_transform_perspective (apply_to, this->depth);
}

static GskTransform *
gsk_perspective_transform_invert (GskTransform *transform,
                                  GskTransform *next)
{
  GskPerspectiveTransform *this = (GskPerspectiveTransform *) transform;

  return gsk_transform_perspective (next, - this->depth);
}

static gboolean
gsk_perspective_transform_equal (GskTransform *first_transform,
                                 GskTransform *second_transform)
{
  GskPerspectiveTransform *first = (GskPerspectiveTransform *) first_transform;
  GskPerspectiveTransform *second = (GskPerspectiveTransform *) second_transform;

  return G_APPROX_VALUE (first->depth, second->depth, 0.001f);
}

static void
gsk_perspective_transform_print (GskTransform *transform,
                                 GString      *string)
{
  GskPerspectiveTransform *this = (GskPerspectiveTransform *) transform;

  g_string_append (string, "perspective(");
  string_append_double (string, this->depth);
  g_string_append (string, ")");
}

static const GskTransformClass GSK_PERSPECTIVE_TRANSFORM_CLASS =
{
  sizeof (GskPerspectiveTransform),
  "GskPerspectiveTransform",
  gsk_perspective_transform_finalize,
  gsk_perspective_transform_to_matrix,
  NULL,
  NULL,
  NULL,
  NULL,
  gsk_perspective_transform_print,
  gsk_perspective_transform_apply,
  gsk_perspective_transform_invert,
  gsk_perspective_transform_equal,
};

/**
 * gsk_transform_perspective:
 * @next: (nullable) (transfer full): the next transform
 * @depth: distance of the z=0 plane. Lower values give a more
 *   flattened pyramid and therefore a more pronounced
 *   perspective effect.
 *
 * Applies a perspective projection transform.
 *
 * This transform scales points in X and Y based on their Z value,
 * scaling points with positive Z values away from the origin, and
 * those with negative Z values towards the origin. Points
 * on the z=0 plane are unchanged.
 *
 * This function consumes @next. Use [method@Gsk.Transform.ref] first
 * if you want to keep it around.
 *
 * Returns: The new transform
 */
GskTransform *
gsk_transform_perspective (GskTransform *next,
                           float         depth)
{
  GskPerspectiveTransform *result;

  if (gsk_transform_has_class (next, &GSK_PERSPECTIVE_TRANSFORM_CLASS))
    {
      GskTransform *r = gsk_transform_perspective (gsk_transform_ref (next->next),
                                                   ((GskPerspectiveTransform *) next)->depth + depth);
      gsk_transform_unref (next);
      return r;
    }

  result = gsk_transform_alloc (&GSK_PERSPECTIVE_TRANSFORM_CLASS,
                                GSK_FINE_TRANSFORM_CATEGORY_ANY,
                                next);

  result->depth = depth;

  return &result->parent;
}

/* }}} */
/* {{{ PUBLIC API */

/**
 * gsk_transform_ref:
 * @this: (nullable): a transform
 *
 * Acquires a reference on the given transform.
 *
 * Returns: (nullable) (transfer none): the transform with an additional reference
 */
GskTransform *
gsk_transform_ref (GskTransform *this)
{
  if (this == NULL)
    return NULL;

  return g_atomic_rc_box_acquire (this);
}

/**
 * gsk_transform_unref:
 * @this: (nullable): a transform
 *
 * Releases a reference on the given transform.
 *
 * If the reference was the last, the resources associated to the @this are
 * freed.
 */
void
gsk_transform_unref (GskTransform *this)
{
  if (this == NULL)
    return;

  g_atomic_rc_box_release_full (this, (GDestroyNotify) gsk_transform_finalize);
}

/**
 * gsk_transform_print:
 * @this: (nullable): a transform
 * @string:  The string to print into
 *
 * Converts the transform into a human-readable representation.
 *
 * The result of this function can later be parsed with
 * [fn@Gsk.Transform.parse].
 */
void
gsk_transform_print (GskTransform *this,
                     GString      *string)
{
  g_return_if_fail (string != NULL);

  if (this == NULL)
    {
      g_string_append (string, "none");
      return;
    }

  if (this->next != NULL)
    {
      gsk_transform_print (this->next, string);
      g_string_append (string, " ");
    }

  this->transform_class->print (this, string);
}

/**
 * gsk_transform_to_string:
 * @this: (nullable): a transform
 *
 * Converts the transform into a human-readable string.
 *
 * The resulting string can be parsed with [fn@Gsk.Transform.parse].
 *
 * This is a wrapper around [method@Gsk.Transform.print].
 *
 * Returns: A new string for @this
 */
char *
gsk_transform_to_string (GskTransform *this)
{
  GString *string;

  string = g_string_new ("");

  gsk_transform_print (this, string);

  return g_string_free (string, FALSE);
}

/**
 * gsk_transform_to_matrix:
 * @this: (nullable): a transform
 * @out_matrix: (out caller-allocates): return location for the matrix
 *
 * Computes the 4x4 matrix for the transform.
 *
 * The previous value of @out_matrix will be ignored.
 */
void
gsk_transform_to_matrix (GskTransform      *this,
                         graphene_matrix_t *out_matrix)
{
  graphene_matrix_t m;

  if (this == NULL)
    {
      graphene_matrix_init_identity (out_matrix);
      return;
    }

  gsk_transform_to_matrix (this->next, out_matrix);
  this->transform_class->to_matrix (this, &m);
  graphene_matrix_multiply (&m, out_matrix, out_matrix);
}

/**
 * gsk_transform_to_2d:
 * @this: a 2D transform
 * @out_xx: (out): return location for the xx member
 * @out_yx: (out): return location for the yx member
 * @out_xy: (out): return location for the xy member
 * @out_yy: (out): return location for the yy member
 * @out_dx: (out): return location for the x0 member
 * @out_dy: (out): return location for the y0 member
 *
 * Converts a transform to a 2D transformation matrix.
 *
 * @this must be a 2D transformation. If you are not
 * sure, use
 *
 *     gsk_transform_get_category() >= GSK_TRANSFORM_CATEGORY_2D
 *
 * to check.
 *
 * The returned values are a subset of the full 4x4 matrix that
 * is computed by [method@Gsk.Transform.to_matrix] and have the
 * following layout:
 *
 * ```
 *   | xx yx |   |  a  b  0 |
 *   | xy yy | = |  c  d  0 |
 *   | dx dy |   | tx ty  1 |
 * ```
 *
 * This function can be used to convert between a `GskTransform`
 * and a matrix type from other 2D drawing libraries, in particular
 * Cairo.
 */
void
gsk_transform_to_2d (GskTransform *this,
                     float        *out_xx,
                     float        *out_yx,
                     float        *out_xy,
                     float        *out_yy,
                     float        *out_dx,
                     float        *out_dy)
{
  *out_xx = 1.0f;
  *out_yx = 0.0f;
  *out_xy = 0.0f;
  *out_yy = 1.0f;
  *out_dx = 0.0f;
  *out_dy = 0.0f;

  if (this == NULL)
    return;

  if (G_UNLIKELY (this->category < GSK_FINE_TRANSFORM_CATEGORY_2D))
    {
      char *s = gsk_transform_to_string (this);
      g_warning ("Given transform \"%s\" is not a 2D transform.", s);
      g_free (s);
      return;
    }

  gsk_transform_to_2d (this->next,
                       out_xx, out_yx,
                       out_xy, out_yy,
                       out_dx, out_dy);

  this->transform_class->apply_2d (this,
                                   out_xx, out_yx,
                                   out_xy, out_yy,
                                   out_dx, out_dy);
}

/**
 * gsk_transform_to_2d_components:
 * @this: a transform
 * @out_skew_x: (out): return location for the skew factor
 *   in the  x direction
 * @out_skew_y: (out): return location for the skew factor
 *   in the  y direction
 * @out_scale_x: (out): return location for the scale
 *   factor in the x direction
 * @out_scale_y: (out): return location for the scale
 *   factor in the y direction
 * @out_angle: (out): return location for the rotation angle
 * @out_dx: (out): return location for the translation
 *   in the x direction
 * @out_dy: (out): return location for the translation
 *   in the y direction
 *
 * Converts a transform to 2D transformation factors.
 *
 * To recreate an equivalent transform from the factors returned
 * by this function, use
 *
 *     gsk_transform_skew (
 *         gsk_transform_scale (
 *             gsk_transform_rotate (
 *                 gsk_transform_translate (NULL, &GRAPHENE_POINT_T (dx, dy)),
 *                 angle),
 *             scale_x, scale_y),
 *         skew_x, skew_y)
 *
 * @this must be a 2D transformation. If you are not sure, use
 *
 *     gsk_transform_get_category() >= GSK_TRANSFORM_CATEGORY_2D
 *
 * to check.
 *
 * Since: 4.6
 */
void
gsk_transform_to_2d_components (GskTransform *this,
                                float        *out_skew_x,
                                float        *out_skew_y,
                                float        *out_scale_x,
                                float        *out_scale_y,
                                float        *out_angle,
                                float        *out_dx,
                                float        *out_dy)
{
  float a, b, c, d, e, f;

  gsk_transform_to_2d (this, &a, &b, &c, &d, &e, &f);

  *out_dx = e;
  *out_dy = f;

#define sign(f) ((f) < 0 ? -1 : 1)

  if (a != 0 || b != 0)
    {
      float det = a * d - b * c;
      float r = sqrtf (a*a + b*b);

      *out_angle = RAD_TO_DEG (sign (b) * acosf (a / r));
      *out_scale_x = r;
      *out_scale_y = det / r;
      *out_skew_x = RAD_TO_DEG (atanf ((a*c + b*d) / (r*r)));
      *out_skew_y = 0;
    }
  else if (c != 0 || d != 0)
    {
      float det = a * d - b * c;
      float s = sqrtf (c*c + d*d);

      *out_angle = RAD_TO_DEG (G_PI/2 - sign (d) * acosf (-c / s));
      *out_scale_x = det / s;
      *out_scale_y = s;
      *out_skew_x = 0;
      *out_skew_y = RAD_TO_DEG (atanf ((a*c + b*d) / (s*s)));
    }
  else
    {
      *out_angle = 0;
      *out_scale_x = 0;
      *out_scale_y = 0;
      *out_skew_x = 0;
      *out_skew_y = 0;
    }
}

/**
 * gsk_transform_to_affine:
 * @this: a transform
 * @out_scale_x: (out): return location for the scale
 *   factor in the x direction
 * @out_scale_y: (out): return location for the scale
 *   factor in the y direction
 * @out_dx: (out): return location for the translation
 *   in the x direction
 * @out_dy: (out): return location for the translation
 *   in the y direction
 *
 * Converts a transform to 2D affine transformation factors.
 *
 * To recreate an equivalent transform from the factors returned
 * by this function, use
 *
 *     gsk_transform_scale (
 *         gsk_transform_translate (
 *             NULL,
 *             &GRAPHENE_POINT_T (dx, dy)),
 *         sx, sy)
 *
 * @this must be a 2D affine transformation. If you are not
 * sure, use
 *
 *     gsk_transform_get_category() >= GSK_TRANSFORM_CATEGORY_2D_AFFINE
 *
 * to check.
 */
void
gsk_transform_to_affine (GskTransform *this,
                         float        *out_scale_x,
                         float        *out_scale_y,
                         float        *out_dx,
                         float        *out_dy)
{
  *out_scale_x = 1.0f;
  *out_scale_y = 1.0f;
  *out_dx = 0.0f;
  *out_dy = 0.0f;

  if (this == NULL)
    return;

  if (G_UNLIKELY (this->category < GSK_FINE_TRANSFORM_CATEGORY_2D_NEGATIVE_AFFINE))
    {
      char *s = gsk_transform_to_string (this);
      g_warning ("Given transform \"%s\" is not an affine 2D transform.", s);
      g_free (s);
      return;
    }

  gsk_transform_to_affine (this->next,
                           out_scale_x, out_scale_y,
                           out_dx, out_dy);

  this->transform_class->apply_affine (this,
                                       out_scale_x, out_scale_y,
                                       out_dx, out_dy);
}

/*<private>
 * gsk_transform_to_dihedral:
 * @this: a transform
 * @out_dihedral: (out): return location for the dihedral transform
 * @out_scale_x: (out): return location for the scale
 *   factor in the x direction
 * @out_scale_y: (out): return location for the scale
 *   factor in the y direction
 * @out_dx: (out): return location for the translation
 *   in the x direction
 * @out_dy: (out): return location for the translation
 *   in the y direction
 *
 * Converts a transform to 2D dihedral transformation factors.
 *
 * To recreate an equivalent transform from the factors returned
 * by this function, use
 *
 *     gsk_transform_dihedral (
 *         gsk_transform_scale (
 *             gsk_transform_translate (
 *                 NULL,
 *                 &GRAPHENE_POINT_T (dx, dy)),
 *             sx, sy),
 *         dihedral)
 *
 * @this must be a 2D dihedral transformation. If you are not
 * sure, use
 *
 *     gsk_transform_get_fine_category() >= GSK_FINE_TRANSFORM_CATEGORY_2D_DIHEDRAL
 *
 * to check.
 */
void
gsk_transform_to_dihedral (GskTransform *this,
                           GdkDihedral  *out_dihedral,
                           float        *out_scale_x,
                           float        *out_scale_y,
                           float        *out_dx,
                           float        *out_dy)
{
  if (this == NULL)
    {
      *out_dihedral = GDK_DIHEDRAL_NORMAL;
      *out_scale_x = 1.0f;
      *out_scale_y = 1.0f;
      *out_dx = 0.0f;
      *out_dy = 0.0f;
      return;
    }

  g_assert (this->category >= GSK_FINE_TRANSFORM_CATEGORY_2D_DIHEDRAL);

  gsk_transform_to_dihedral (this->next,
                             out_dihedral,
                             out_scale_x, out_scale_y,
                             out_dx, out_dy);

  this->transform_class->apply_dihedral (this,
                                         out_dihedral,
                                         out_scale_x, out_scale_y,
                                         out_dx, out_dy);
}

/*< private >
 * gsk_transform_dihedral:
 * @next: (nullable) (transfer full): the next transform
 * @dihedral: dihedral tansform to apply
 *
 * Applies a dihedral transform to @next.
 *
 * This function consumes @next. Use [method@Gsk.Transform.ref] first
 * if you want to keep it around.
 *
 * Returns: (nullable): The new transform
 **/
GskTransform *
gsk_transform_dihedral (GskTransform *next,
                        GdkDihedral   dihedral)
{
  int rotate = dihedral & 3;
  int flip = dihedral & 4;

  if (flip)
    next = gsk_transform_scale (next, -1.0, 1.0);

  if (rotate)
    next = gsk_transform_rotate (next, rotate * 90.0f);

  return next;
}

/**
 * gsk_transform_to_translate:
 * @this: a transform
 * @out_dx: (out): return location for the translation
 *   in the x direction
 * @out_dy: (out): return location for the translation
 *   in the y direction
 *
 * Converts a transform to a translation operation.
 *
 * @this must be a 2D transformation. If you are not
 * sure, use
 *
 *     gsk_transform_get_category() >= GSK_TRANSFORM_CATEGORY_2D_TRANSLATE
 *
 * to check.
 */
void
gsk_transform_to_translate (GskTransform *this,
                            float        *out_dx,
                            float        *out_dy)
{
  *out_dx = 0.0f;
  *out_dy = 0.0f;

  if (this == NULL)
    return;

  if (G_UNLIKELY (this->category < GSK_FINE_TRANSFORM_CATEGORY_2D_TRANSLATE))
    {
      char *s = gsk_transform_to_string (this);
      g_warning ("Given transform \"%s\" is not an affine 2D translation.", s);
      g_free (s);

      return;
    }

  gsk_transform_to_translate (this->next, out_dx, out_dy);

  this->transform_class->apply_translate (this, out_dx, out_dy);
}

/**
 * gsk_transform_transform:
 * @next: (nullable) (transfer full): transform to apply @other to
 * @other: (nullable):  transform to apply
 *
 * Applies all the operations from @other to @next.
 *
 * This function consumes @next. Use [method@Gsk.Transform.ref] first
 * if you want to keep it around.
 *
 * Returns: (nullable): The new transform
 */
GskTransform *
gsk_transform_transform (GskTransform *next,
                         GskTransform *other)
{
  if (other == NULL)
    return next;

  if (next == NULL)
    return gsk_transform_ref (other);

  if (gsk_transform_is_identity (next))
    {
      /* ref before unref to avoid catastrophe when other == next */
      other = gsk_transform_ref (other);
      gsk_transform_unref (next);
      return other;
    }

  next = gsk_transform_transform (next, other->next);
  return other->transform_class->apply (other, next);
}

/**
 * gsk_transform_invert:
 * @this: (nullable) (transfer full): transform to invert
 *
 * Inverts the given transform.
 *
 * If @this is not invertible, `NULL` is returned.
 * Note that inverting `NULL` also returns `NULL`, which is
 * the correct inverse of `NULL`. If you need to differentiate
 * between those cases, you should check @this is not `NULL`
 * before calling this function.
 *
 * This function consumes @this. Use [method@Gsk.Transform.ref] first
 * if you want to keep it around.
 *
 * Returns: (nullable): The inverted transform
 */
GskTransform *
gsk_transform_invert (GskTransform *this)
{
  GskTransform *result = NULL;
  GskTransform *cur;

  for (cur = this; cur; cur = cur->next)
    {
      result = cur->transform_class->invert (cur, result);
      if (result == NULL)
        break;
    }

  gsk_transform_unref (this);

  return result;
}

/**
 * gsk_transform_equal:
 * @first: (nullable): the first transform
 * @second: (nullable): the second transform
 *
 * Checks two transforms for equality.
 *
 * Returns: true if the two transforms perform the same operation
 */
gboolean
gsk_transform_equal (GskTransform *first,
                     GskTransform *second)
{
  if (first == second)
    return TRUE;

  if (first == NULL)
    return gsk_transform_is_identity (second);

  if (second == NULL)
    return gsk_transform_is_identity (first);

  if (first->transform_class != second->transform_class)
    return FALSE;

  if (!gsk_transform_equal (first->next, second->next))
    return FALSE;

  return first->transform_class->equal (first, second);
}

/**
 * gsk_transform_get_category:
 * @this: (nullable): a transform
 *
 * Returns the category this transform belongs to.
 *
 * Returns: The category of the transform
 **/
GskTransformCategory
gsk_transform_get_category (GskTransform *this)
{
  if (this == NULL)
    return GSK_TRANSFORM_CATEGORY_IDENTITY;

  switch (this->category)
    {
      case GSK_FINE_TRANSFORM_CATEGORY_UNKNOWN:
        return GSK_TRANSFORM_CATEGORY_UNKNOWN;

      case GSK_FINE_TRANSFORM_CATEGORY_ANY:
        return GSK_TRANSFORM_CATEGORY_ANY;

      case GSK_FINE_TRANSFORM_CATEGORY_3D:
        return GSK_TRANSFORM_CATEGORY_3D;

      case GSK_FINE_TRANSFORM_CATEGORY_2D:
      case GSK_FINE_TRANSFORM_CATEGORY_2D_DIHEDRAL:
        return GSK_TRANSFORM_CATEGORY_2D;

      case GSK_FINE_TRANSFORM_CATEGORY_2D_NEGATIVE_AFFINE:
      case GSK_FINE_TRANSFORM_CATEGORY_2D_AFFINE:
        return GSK_TRANSFORM_CATEGORY_2D_AFFINE;

      case GSK_FINE_TRANSFORM_CATEGORY_2D_TRANSLATE:
        return GSK_TRANSFORM_CATEGORY_2D_TRANSLATE;

      case GSK_FINE_TRANSFORM_CATEGORY_IDENTITY:
        return GSK_TRANSFORM_CATEGORY_IDENTITY;

      default:
        g_return_val_if_reached (GSK_TRANSFORM_CATEGORY_UNKNOWN);
    }
}

/**
 * gsk_transform_new: (constructor):
 *
 * Creates a new identity transform.
 *
 * This function is meant to be used by language
 * bindings. For C code, this is equivalent to using `NULL`.
 *
 * Returns: A new identity transform
 */
GskTransform *
gsk_transform_new (void)
{
  return gsk_transform_alloc (&GSK_IDENTITY_TRANSFORM_CLASS, GSK_FINE_TRANSFORM_CATEGORY_IDENTITY, NULL);
}

/**
 * gsk_transform_transform_bounds:
 * @this: a transform
 * @rect: the rectangle to transform
 * @out_rect: (out caller-allocates): return location for the bounds
 *   of the transformed rectangle
 *
 * Transforms a rectangle using the given transform.
 *
 * The result is the bounding box containing the coplanar quad.
 */
void
gsk_transform_transform_bounds (GskTransform          *this,
                                const graphene_rect_t *rect,
                                graphene_rect_t       *out_rect)
{
  switch (gsk_transform_get_fine_category (this))
    {
    case GSK_FINE_TRANSFORM_CATEGORY_IDENTITY:
      graphene_rect_init_from_rect (out_rect, rect);
      break;

    case GSK_FINE_TRANSFORM_CATEGORY_2D_TRANSLATE:
      {
        float dx, dy;

        gsk_transform_to_translate (this, &dx, &dy);
        graphene_rect_init (out_rect,
                            rect->origin.x + dx,
                            rect->origin.y + dy,
                            rect->size.width,
                            rect->size.height);
      }
    break;

    case GSK_FINE_TRANSFORM_CATEGORY_2D_AFFINE:
    case GSK_FINE_TRANSFORM_CATEGORY_2D_NEGATIVE_AFFINE:
      {
        float dx, dy, scale_x, scale_y;

        gsk_transform_to_affine (this, &scale_x, &scale_y, &dx, &dy);

        graphene_rect_init (out_rect,
                            (rect->origin.x * scale_x) + dx,
                            (rect->origin.y * scale_y) + dy,
                            rect->size.width * scale_x,
                            rect->size.height * scale_y);
      }
    break;

    case GSK_FINE_TRANSFORM_CATEGORY_2D_DIHEDRAL:
      {
        GdkDihedral dihedral;
        float dx, dy, scale_x, scale_y;

        gsk_transform_to_dihedral (this, &dihedral, &scale_x, &scale_y, &dx, &dy);

        gsk_rect_dihedral (rect, dihedral, out_rect);
        graphene_rect_init (out_rect,
                            (out_rect->origin.x * scale_x) + dx,
                            (out_rect->origin.y * scale_y) + dy,
                            out_rect->size.width * scale_x,
                            out_rect->size.height * scale_y);
      }
      break;

    case GSK_FINE_TRANSFORM_CATEGORY_UNKNOWN:
    case GSK_FINE_TRANSFORM_CATEGORY_ANY:
    case GSK_FINE_TRANSFORM_CATEGORY_3D:
    case GSK_FINE_TRANSFORM_CATEGORY_2D:
    default:
      {
        graphene_matrix_t mat;

        gsk_transform_to_matrix (this, &mat);
        gsk_matrix_transform_bounds (&mat, rect, out_rect);
      }
      break;
    }
}

/**
 * gsk_transform_transform_point:
 * @this: a transform
 * @point: the point to transform
 * @out_point: (out caller-allocates): return location for
 *   the transformed point
 *
 * Transforms a point using the given transform.
 */
void
gsk_transform_transform_point (GskTransform           *this,
                               const graphene_point_t *point,
                               graphene_point_t       *out_point)
{
  switch (gsk_transform_get_fine_category (this))
    {
    case GSK_FINE_TRANSFORM_CATEGORY_IDENTITY:
      *out_point = *point;
      break;

    case GSK_FINE_TRANSFORM_CATEGORY_2D_TRANSLATE:
      {
        float dx, dy;

        gsk_transform_to_translate (this, &dx, &dy);
        out_point->x = point->x + dx;
        out_point->y = point->y + dy;
      }
    break;

    case GSK_FINE_TRANSFORM_CATEGORY_2D_AFFINE:
    case GSK_FINE_TRANSFORM_CATEGORY_2D_NEGATIVE_AFFINE:
      {
        float dx, dy, scale_x, scale_y;

        gsk_transform_to_affine (this, &scale_x, &scale_y, &dx, &dy);

        out_point->x = (point->x * scale_x) + dx;
        out_point->y = (point->y * scale_y) + dy;
      }
    break;

    case GSK_FINE_TRANSFORM_CATEGORY_2D_DIHEDRAL:
      {
        GdkDihedral dihedral;
        float xx, xy, yx, yy, dx, dy, scale_x, scale_y;

        gsk_transform_to_dihedral (this, &dihedral, &scale_x, &scale_y, &dx, &dy);
        gdk_dihedral_get_mat2 (dihedral, &xx, &xy, &yx, &yy);

        *out_point = GRAPHENE_POINT_INIT ((xx * point->x + xy * point->y) * scale_x + dx,
                                          (yx * point->x + yy * point->y) * scale_y + dy);
      }
      break;

    case GSK_FINE_TRANSFORM_CATEGORY_UNKNOWN:
    case GSK_FINE_TRANSFORM_CATEGORY_ANY:
    case GSK_FINE_TRANSFORM_CATEGORY_3D:
    case GSK_FINE_TRANSFORM_CATEGORY_2D:
    default:
      {
        graphene_matrix_t mat;

        gsk_transform_to_matrix (this, &mat);
        gsk_matrix_transform_point (&mat, point, out_point);
      }
      break;
    }
}

static guint
gsk_transform_parse_float (GtkCssParser *parser,
                           guint         n,
                           gpointer      data)
{
  float *f = data;
  double d;

  if (!gtk_css_parser_consume_number (parser, &d))
    return 0;

  f[n] = d;
  return 1;
}

static guint
gsk_transform_parse_scale (GtkCssParser *parser,
                           guint         n,
                           gpointer      data)
{
  float *f = data;
  double d;

  if (!gtk_css_parser_consume_number (parser, &d))
    return 0;

  f[n] = d;
  f[1] = d;
  return 1;
}

gboolean
gsk_transform_parser_parse (GtkCssParser  *parser,
                            GskTransform **out_transform)
{
  const GtkCssToken *token;
  GskTransform *transform = NULL;
  float f[16] = { 0, };
  gboolean parsed_something = FALSE;

  token = gtk_css_parser_get_token (parser);
  if (gtk_css_token_is_ident (token, "none"))
    {
      gtk_css_parser_consume_token (parser);
      *out_transform = NULL;
      return TRUE;
    }

  while (TRUE)
    {
      if (gtk_css_token_is_function (token, "matrix"))
        {
          graphene_matrix_t matrix;
          if (!gtk_css_parser_consume_function (parser, 6, 6, gsk_transform_parse_float, f))
            goto fail;

          graphene_matrix_init_from_2d (&matrix, f[0], f[1], f[2], f[3], f[4], f[5]);
          transform = gsk_transform_matrix_with_category (transform,
                                                          &matrix,
                                                          GSK_FINE_TRANSFORM_CATEGORY_2D);
        }
      else if (gtk_css_token_is_function (token, "matrix3d"))
        {
          graphene_matrix_t matrix;
          if (!gtk_css_parser_consume_function (parser, 16, 16, gsk_transform_parse_float, f))
            goto fail;

          graphene_matrix_init_from_float (&matrix, f);
          transform = gsk_transform_matrix (transform, &matrix);
        }
      else if (gtk_css_token_is_function (token, "perspective"))
        {
          if (!gtk_css_parser_consume_function (parser, 1, 1, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_perspective (transform, f[0]);
        }
      else if (gtk_css_token_is_function (token, "rotate") ||
               gtk_css_token_is_function (token, "rotateZ"))
        {
          if (!gtk_css_parser_consume_function (parser, 1, 1, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_rotate (transform, f[0]);
        }
      else if (gtk_css_token_is_function (token, "rotate3d"))
        {
          graphene_vec3_t axis;

          if (!gtk_css_parser_consume_function (parser, 4, 4, gsk_transform_parse_float, f))
            goto fail;

          graphene_vec3_init (&axis, f[0], f[1], f[2]);
          transform = gsk_transform_rotate_3d (transform, f[3], &axis);
        }
      else if (gtk_css_token_is_function (token, "rotateX"))
        {
          if (!gtk_css_parser_consume_function (parser, 1, 1, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_rotate_3d (transform, f[0], graphene_vec3_x_axis ());
        }
      else if (gtk_css_token_is_function (token, "rotateY"))
        {
          if (!gtk_css_parser_consume_function (parser, 1, 1, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_rotate_3d (transform, f[0], graphene_vec3_y_axis ());
        }
      else if (gtk_css_token_is_function (token, "scale"))
        {
          if (!gtk_css_parser_consume_function (parser, 1, 2, gsk_transform_parse_scale, f))
            goto fail;

          transform = gsk_transform_scale (transform, f[0], f[1]);
        }
      else if (gtk_css_token_is_function (token, "scale3d"))
        {
          if (!gtk_css_parser_consume_function (parser, 3, 3, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_scale_3d (transform, f[0], f[1], f[2]);
        }
      else if (gtk_css_token_is_function (token, "scaleX"))
        {
          if (!gtk_css_parser_consume_function (parser, 1, 1, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_scale (transform, f[0], 1.f);
        }
      else if (gtk_css_token_is_function (token, "scaleY"))
        {
          if (!gtk_css_parser_consume_function (parser, 1, 1, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_scale (transform, 1.f, f[0]);
        }
      else if (gtk_css_token_is_function (token, "scaleZ"))
        {
          if (!gtk_css_parser_consume_function (parser, 1, 1, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_scale_3d (transform, 1.f, 1.f, f[0]);
        }
      else if (gtk_css_token_is_function (token, "translate"))
        {
          f[1] = 0.f;
          if (!gtk_css_parser_consume_function (parser, 1, 2, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_translate (transform, &GRAPHENE_POINT_INIT (f[0], f[1]));
        }
      else if (gtk_css_token_is_function (token, "translate3d"))
        {
          if (!gtk_css_parser_consume_function (parser, 3, 3, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_translate_3d (transform, &GRAPHENE_POINT3D_INIT (f[0], f[1], f[2]));
        }
      else if (gtk_css_token_is_function (token, "translateX"))
        {
          if (!gtk_css_parser_consume_function (parser, 1, 1, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_translate (transform, &GRAPHENE_POINT_INIT (f[0], 0.f));
        }
      else if (gtk_css_token_is_function (token, "translateY"))
        {
          if (!gtk_css_parser_consume_function (parser, 1, 1, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_translate (transform, &GRAPHENE_POINT_INIT (0.f, f[0]));
        }
      else if (gtk_css_token_is_function (token, "translateZ"))
        {
          if (!gtk_css_parser_consume_function (parser, 1, 1, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_translate_3d (transform, &GRAPHENE_POINT3D_INIT (0.f, 0.f, f[0]));
        }
      else if (gtk_css_token_is_function (token, "skew"))
        {
          if (!gtk_css_parser_consume_function (parser, 2, 2, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_skew (transform, f[0], f[1]);
        }
      else if (gtk_css_token_is_function (token, "skewX"))
        {
          if (!gtk_css_parser_consume_function (parser, 1, 1, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_skew (transform, f[0], 0);
        }
      else if (gtk_css_token_is_function (token, "skewY"))
        {
          if (!gtk_css_parser_consume_function (parser, 1, 1, gsk_transform_parse_float, f))
            goto fail;

          transform = gsk_transform_skew (transform, 0, f[0]);
        }
      else
        {
          break;
        }

      parsed_something = TRUE;
      token = gtk_css_parser_get_token (parser);
    }

  if (!parsed_something)
    {
      gtk_css_parser_error_syntax (parser, "Expected a transform");
      goto fail;
    }

  *out_transform = transform;
  return TRUE;

fail:
  gsk_transform_unref (transform);
  *out_transform = NULL;
  return FALSE;
}

/**
 * gsk_transform_parse:
 * @string: the string to parse
 * @out_transform: (out): return location for the transform
 *
 * Parses a given into a transform.
 *
 * Strings printed via [method@Gsk.Transform.to_string]
 * can be read in again successfully using this function.
 *
 * If @string does not describe a valid transform, false
 * is returned and `NULL` is put in @out_transform.
 *
 * Returns: true if @string described a valid transform
 */
gboolean
gsk_transform_parse (const char    *string,
                     GskTransform **out_transform)
{
  GtkCssParser *parser;
  GBytes *bytes;
  gboolean result;

  g_return_val_if_fail (string != NULL, FALSE);
  g_return_val_if_fail (out_transform != NULL, FALSE);

  bytes = g_bytes_new_static (string, strlen (string));
  parser = gtk_css_parser_new_for_bytes (bytes, NULL, NULL, NULL, NULL);

  result = gsk_transform_parser_parse (parser, out_transform);

  if (result && !gtk_css_parser_has_token (parser, GTK_CSS_TOKEN_EOF))
    {
      g_clear_pointer (out_transform, gsk_transform_unref);
      result = FALSE;
    }
  gtk_css_parser_unref (parser);
  g_bytes_unref (bytes);

  return result;
}

/* Some of the graphene_matrix_transform apis yield unexpected
 * results with projective matrices, since they silently drop
 * the w component, so we provide working alternatives here.
 */
void
gsk_matrix_transform_point (const graphene_matrix_t *m,
                            const graphene_point_t  *p,
                            graphene_point_t        *res)
{
  graphene_vec4_t vec4;
  float w;

  graphene_vec4_init (&vec4, p->x, p->y, 0.0f, 1.0f);
  graphene_matrix_transform_vec4 (m, &vec4, &vec4);

  w = graphene_vec4_get_w (&vec4);
  res->x = graphene_vec4_get_x (&vec4) / w;
  res->y = graphene_vec4_get_y (&vec4) / w;
}

void
gsk_matrix_transform_point3d (const graphene_matrix_t  *m,
                              const graphene_point3d_t *p,
                              graphene_point3d_t       *res)
{
  graphene_vec4_t vec4;
  float w;

  graphene_vec4_init (&vec4, p->x, p->y, 0.0f, 1.0f);
  graphene_matrix_transform_vec4 (m, &vec4, &vec4);

  w = graphene_vec4_get_w (&vec4);
  res->x = graphene_vec4_get_x (&vec4) / w;
  res->y = graphene_vec4_get_y (&vec4) / w;
  res->z = graphene_vec4_get_z (&vec4) / w;
}

void
gsk_matrix_transform_rect (const graphene_matrix_t *m,
                           const graphene_rect_t   *r,
                           graphene_quad_t         *res)
{
  graphene_point_t ret[4];
  graphene_rect_t rr;

  graphene_rect_normalize_r (r, &rr);

#define TRANSFORM_POINT(matrix, rect, corner, out_p)   do {\
  graphene_vec4_t __s; \
  graphene_point_t __p; \
  float w; \
  graphene_rect_get_ ## corner (rect, &__p); \
  graphene_vec4_init (&__s, __p.x, __p.y, 0.f, 1.f); \
  graphene_matrix_transform_vec4 (matrix, &__s, &__s); \
  w = graphene_vec4_get_w (&__s); \
  out_p.x = graphene_vec4_get_x (&__s) / w; \
  out_p.y = graphene_vec4_get_y (&__s) / w;           } while (0)

  TRANSFORM_POINT (m, &rr, top_left, ret[0]);
  TRANSFORM_POINT (m, &rr, top_right, ret[1]);
  TRANSFORM_POINT (m, &rr, bottom_right, ret[2]);
  TRANSFORM_POINT (m, &rr, bottom_left, ret[3]);

#undef TRANSFORM_POINT

  graphene_quad_init (res, &ret[0], &ret[1], &ret[2], &ret[3]);
}
void
gsk_matrix_transform_bounds (const graphene_matrix_t *m,
                             const graphene_rect_t   *r,
                             graphene_rect_t         *res)
{
  graphene_quad_t q;

  gsk_matrix_transform_rect (m, r, &q);
  graphene_quad_bounds (&q, res);
}

/* }}} */

/* vim:set foldmethod=marker: */
