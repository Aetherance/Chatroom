#include <csignal>

class SignalBlocker {
  public:
    SignalBlocker() { signal(SIGINT, SIG_IGN); }
    ~SignalBlocker() { signal(SIGINT, SIG_DFL); }
};