#define GSK_N_TEXTURES 2

#include "common.glsl"

#define VARIATION_MASK_MODE GSK_VARIATION

PASS(0) vec2 _pos;
PASS_FLAT(1) Rect _source_rect;
PASS_FLAT(2) Rect _mask_rect;
PASS(3) vec2 _source_coord;
PASS(4) vec2 _mask_coord;
PASS_FLAT(7) float _opacity;


#ifdef GSK_VERTEX_SHADER

IN(0) vec4 in_rect;
IN(1) vec4 in_source_rect;
IN(2) vec4 in_mask_rect;
IN(3) float in_opacity;

void
run (out vec2 pos)
{
  Rect r = rect_from_gsk (in_rect);
  
  pos = rect_get_position (r);

  _pos = pos;
  Rect source_rect = rect_from_gsk (in_source_rect);
  _source_rect = source_rect;
  _source_coord = rect_get_coord (source_rect, pos);
  Rect mask_rect = rect_from_gsk (in_mask_rect);
  _mask_rect = mask_rect;
  _mask_coord = rect_get_coord (mask_rect, pos);
  _opacity = in_opacity;
}

#endif



#ifdef GSK_FRAGMENT_SHADER

void
run (out vec4 color,
     out vec2 position)
{
  vec4 source = texture (GSK_TEXTURE0, _source_coord);
  source = output_color_alpha (source, rect_coverage (_source_rect, _pos));
  vec4 mask = texture (GSK_TEXTURE1, _mask_coord);
  mask = output_color_alpha (mask, rect_coverage (_mask_rect, _pos));

  float alpha = _opacity;
  switch (VARIATION_MASK_MODE)
  {
    case GSK_MASK_MODE_ALPHA:
      alpha *= mask.a;
      break;

    case GSK_MASK_MODE_INVERTED_ALPHA:
      alpha *= (1.0 - mask.a);
      break;

    case GSK_MASK_MODE_LUMINANCE:
      alpha *= luminance (mask.rgb);
      break;

    case GSK_MASK_MODE_INVERTED_LUMINANCE:
      alpha *= (mask.a - luminance (mask.rgb));
      break;

    default:
      break;
  }

  color = output_color_alpha (source, alpha);
  position = _pos;
}

#endif
