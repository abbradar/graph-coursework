#include "objectmenu.h"

using namespace std;

ObjectMenu::ObjectMenu(const std::shared_ptr<Context> &context_) : ContextUser(context_) {
  std::vector<string> vec;
  vec.reserve(context_->models.models().size());
  for (const auto &i : context_->models.models()) {
    vec.push_back(i.first);
  }
  set_items(vec);
  context()->window->set_grab_input(false);
}

void ObjectMenu::OnSelected() {
  if (selected() >= 0) {
    auto &model = context()->models.models()[items()[selected()]];
    Position pos = context()->camera;
    pos.roll = 0;
    pos.pitch = 0;
    pos.yaw = 0;
    context()->scene.objects().push_back(make_shared<SceneObject>(model, pos));
    context()->window->set_grab_input(true);
    context()->window->UnregisterWorker(this);
  }
}

bool ObjectMenu::ProcessKeyDown(const SDL_KeyboardEvent &event) {
  switch (event.keysym.sym) {
    case SDLK_ESCAPE: {
      context()->window->UnregisterWorker(this);
      return true;
    }
    default:
      return false;
  }
}
