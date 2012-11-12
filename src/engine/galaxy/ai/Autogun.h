#ifndef AUTOGUN_H
#define AUTOGUN_H

#include "engine/galaxy/CGalaxySpriteObject.h"

namespace galaxy
{

class AutoGun : public CGalaxySpriteObject
{
  
public:
  AutoGun(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);
  
  void process();
  
  void getTouchedBy(CSpriteObject &theObject);
  
private:
  
  void waiting();
  void flying();
  
  void (AutoGun::*processState)();
  
  const int FLY_SPEED = 160;
  const int WAIT_TIME = 150;
  
  int time;
  
  VectorD2<int> origin;
};

};

#endif // AUTOGUN_H