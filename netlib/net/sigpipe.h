#include<signal.h>

class IgnoreSigPipe
{
public:
    IgnoreSigPipe() {
        ::signal(SIGPIPE,SIG_IGN);
    }    
};