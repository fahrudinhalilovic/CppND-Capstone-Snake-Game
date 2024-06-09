#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "game_resources.h"

class Controller {
 public:
  void HandleInput(bool &running, GameResources& game_resources) const;
};

#endif