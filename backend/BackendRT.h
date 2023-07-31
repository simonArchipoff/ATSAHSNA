#pragma once


#include "concurrentqueue.h"
#include "backend.h"

class ControlModule {

    struct RTModule{
        void init(int sampleRate);
        void process(const vector<VD> & inputs, vector<VD> & outputs);
        void after_process();

        void recieveCommand();
        void sendResponse();
    };

    void sendCommand();
    void recieveResponse();
};

class BackendRT : public Backend{



};
