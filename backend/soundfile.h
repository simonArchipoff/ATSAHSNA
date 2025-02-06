#pragma once

#include <backend.h>
#include <set>
class SndfileHandle;

class soundFile : public Backend{
public:
    soundFile();
    void setFile(std::string s);
    /*void setFile(QString s){
        setFile(s.toStdString());
    }*/
    uint numberInput()   const override;
    uint numberOutput()  const override;
    uint getSampleRate() const override;
    bool isReady()       const override;

    uint numberChannels() const;
    uint numberFrames() const ;
    std::string error();

    void setInput(uint);
    void setWindow(int start, int number);

    variant<const vector<ResultHarmonics>, ErrorBackend> getResultHarmonics();
    variant<const vector<ResultResponse>,  ErrorBackend> getResultResponse();

protected:
    struct Data{
        std::vector<VD> inputs;
        std::vector<VD> outputs;
    };
    Data getFrames();
private:
    int start_frame = 0,
        nb_frames=-1;

    std::set<uint> channels_input,channels_output;
    SndfileHandle * fileHandle = nullptr;
};
