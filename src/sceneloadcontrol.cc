#include <fstream>
#include <boost/format.hpp>
#include "common/exception.h"
#include "common/logging.h"
#include "scenesettings.h"
#include "sceneloadcontrol.h"

using namespace std;

SceneLoadControl::SceneLoadControl(const shared_ptr<Context> &context_) : ContextUser(context_) {
  set_caption("Load scene");
}

void SceneLoadControl::OnEntered() {
  ifstream in(label());
  if (in.good()) {
    context()->scene.objects().clear();
    try {
      settings::LoadScene(context()->scene, in, context()->models);
    } catch (Exception &e) {
      string str("Error loading scene: ");
      str.append(e.what());
      LogWarning(str.data());
      return;
    }
    LogNotice("Loaded new scene");
  } else {
    LogWarning("Error loading scene: invalid file");
  }
  context()->window->UnregisterWorker(this);
}

bool SceneLoadControl::ProcessKeyDown(const SDL_KeyboardEvent &event) {
  switch (event.keysym.sym) {
    case SDLK_ESCAPE: {
      context()->window->UnregisterWorker(this);
      return true;
    }
    default:
      return TextEditControl::ProcessKeyDown(event);
  }
}
