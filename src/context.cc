#include "context.h"

Context::Context(Window *window_) : window(window_) {}

TransformedObject::TransformedObject(const std::weak_ptr<SceneObject> &object_)
  : object(object_) {}


ContextUser::ContextUser(const std::shared_ptr<Context> &context) : context_(context) {}
