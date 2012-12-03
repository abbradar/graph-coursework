#include "common/debug.h"
#include "movetransform.h"
#include "transformmenu.h"

using namespace std;

TransformMenu::TransformMenu(const std::shared_ptr<Context> &context) :
 ContextUser(context), action_(kNone) {
  std::vector<std::string> items;
  items.push_back("Move");
  items.push_back("Rotate");
  items.push_back("Attach");
  set_items(items);
}

void TransformMenu::PreRenderStep() {
  if (action_ != kNone) {
    std::shared_ptr<SceneObject> object = context()->traced_object.lock()->object.lock();
    if (object) {
      switch (action_) {
        case kMove: {
          auto i = make_shared<MoveTransform>(context(), object);
          context()->window->RegisterWorker(i);
          break;
        }
        default:
          Assert(false);
          break;
      }
    }
    context()->window->UnregisterWorker(this);
  }
}

bool TransformMenu::ProcessKeyDown(const SDL_KeyboardEvent &event) {
  switch (event.keysym.sym) {
    case SDLK_ESCAPE:
      context()->window->UnregisterWorker(this);
      break;
    default:
      break;
  }
  return true;
}

void TransformMenu::OnSelected() {
  switch (selected()) {
    case 0:
      action_ = kMove;
      break;
    case 1:
      action_ = kRotate;
      break;
    case 2:
      action_ = kAttach;
      break;
    default:
      Assert(false);
      break;
  }
}
