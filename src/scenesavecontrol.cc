#include <fstream>
#include <boost/format.hpp>
#include "common/exception.h"
#include "common/logging.h"
#include "scenesettings.h"
#include "scenesavecontrol.h"

using namespace std;

SceneSaveControl::SceneSaveControl(const shared_ptr<Context> &context_) : ContextUser(context_) {
  set_caption("Save scene");
}

void SceneSaveControl::OnEntered() {
  ofstream out(label());
  if (out.good()) {
    settings::SaveScene(context()->scene, out);
    LogNotice("Saved scene");
  } else {
    LogWarning("Error saving scene: invalid file");
  }
  context()->window->UnregisterWorker(this);
}

bool SceneSaveControl::ProcessKeyDown(const SDL_KeyboardEvent &event) {
  switch (event.keysym.sym) {
    case SDLK_ESCAPE: {
      context()->window->UnregisterWorker(this);
      return true;
    }
    default:
      return TextEditControl::ProcessKeyDown(event);
  }
}

