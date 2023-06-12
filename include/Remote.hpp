#ifndef __Remote_hpp
#define __Remote_hpp

enum RemoteButton {
    REMOTE_NO_BUTTON = 0,
    REMOTE_START_STOP = (1 << 4),
    REMOTE_SPEED_UP = (1 << 2),
    REMOTE_SPEED_DOWN = (1 << 3),
    REMOTE_AIR_IN = (1 << 1),
    REMOTE_AIR_OUT = (1 << 5),
};

namespace Remote {

    void init(void);
    RemoteButton getRemoteButton(void);

} // namespace Remote

#endif