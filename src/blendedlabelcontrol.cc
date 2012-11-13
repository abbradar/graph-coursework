#include "blendedlabelcontrol.h"

using namespace sdlobj;

BlendedLabelControl::BlendedLabelControl() = default;

void BlendedLabelControl::Repaint(sdlobj::Surface &surface) {
  Surface rendered = font().RenderUTF8_Blended(label().data(), font_color());
  surface.Blit(rendered);
}
