#ifndef __Remote_hpp
#define __Remote_hpp

enum RemoteButton {
  REMOTE_NO_BUTTON = 0,
  REMOTE_START_STOP,
  REMOTE_SPEED_UP,
  REMOTE_SPEED_DOWN,
  REMOTE_AIR_IN,
  REMOTE_AIR_OUT
};

namespace Remote {

void init(void);
RemoteButton getRemoteButton(void);

} // namespace Remote

#endif