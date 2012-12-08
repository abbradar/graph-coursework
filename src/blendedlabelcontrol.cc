#include "blendedlabelcontrol.h"

using namespace sdlobj;

BlendedLabelControl::BlendedLabelControl() = default;

void BlendedLabelControl::Repaint(sdlobj::Surface &surface) {
  Uint32 back_p = surface.ColorToPixel(back_color());
  SetColorKey(SDL_SRCCOLORKEY, back_p);
  surface = font().RenderUTF8_Blended(label().data(), font_color());
}
