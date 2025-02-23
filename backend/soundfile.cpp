#include <sndfile.hh>
#include "soundfile.h"


soundFile::soundFile() {}

void soundFile::setFile(std::string s){
    if(fileHandle){
        delete fileHandle;
    }
    fileHandle = new SndfileHandle(s);
    if(fileHandle->error()){
        delete fileHandle;
        fileHandle = nullptr;
    }
    for(uint i = 0; i < numberChannels(); i++){
        channels_output.insert(i);
    }
}

uint soundFile::numberInput() const{
    return channels_input.size();
}

uint soundFile::numberOutput() const{
    return channels_output.size();
}

uint soundFile::getSampleRate() const{
    assert(fileHandle);
    return fileHandle->samplerate();
}

bool soundFile::isReady() const{
    assert(fileHandle);
    return fileHandle;
}

uint soundFile::numberChannels() const{
    return fileHandle->channels();
}

uint soundFile::numberFrames() const{
    return fileHandle->frames();
}

void soundFile::setInput(int i){
    assert(i < numberChannels());
    channels_output.erase(i);
    channels_input.insert(i);
}

void soundFile::unsetInput(int i){
    assert(i < numberChannels());
    channels_output.insert(i);
    channels_input.erase(i);
}

std::string soundFile::error(){
    std::string s(fileHandle->strError());
    return s;
}



void soundFile::setWindow(int start, int number){
    assert(fileHandle && start + number <= fileHandle->frames());
    start_frame = start;
    nb_frames = number;
}

variant<const vector<ResultHarmonics>, ErrorBackend> soundFile::getResultHarmonics(){
    auto f = getFrames();
    std::vector<ResultHarmonics> res;
    int i = 0;
    for(auto o : f.outputs){
        auto r = computeTHD(paramHarmonics,  o, getSampleRate());
        res.push_back(r);
        r.name = std::to_string(i++);
    }
    return std::variant<const std::vector<ResultHarmonics>,ErrorBackend>(res);
}

variant<const vector<ResultResponse>, ErrorBackend> soundFile::getResultResponse(){

    auto data = getFrames();
    std::vector<ResultResponse> res;

    int i = 0;
    for(const auto & o : data.outputs){
        if(data.inputs.empty()){
            auto tmp = computeResponse(paramResponse,o,getSampleRate());
            tmp.name =  "file_" + std::to_string(i++) ;
            res.push_back(tmp);
        }else{
            for(const auto & in : data.inputs){
                auto tmp = computeResponse(paramResponse,in,o,getSampleRate());
                tmp.name =  "file_" + std::to_string(i++) ;
                res.push_back(tmp);
            }
        }
//        auto tmp =  computeResponse(paramResponse,in, o, getSampleRate());

    }
    return std::variant<const std::vector<ResultResponse>,ErrorBackend>(res);
}

soundFile::Data soundFile::getFrames(){
    Data d;
    d.inputs.resize(channels_input.size());
    d.outputs.resize(channels_output.size());
    int f ;
    if(nb_frames == -1){
        assert(start_frame < fileHandle->frames());
        f = fileHandle->frames() - start_frame;
    } else {
        assert(start_frame + nb_frames <= fileHandle->frames());
        f = nb_frames;
    }

    for(auto &i:d.inputs){
        i.resize(f);
    }
    for(auto &i:d.outputs){
        i.resize(f);
    }
    fileHandle->seek(start_frame, SF_SEEK_SET);

    vector<double> tmp;
    tmp.resize(f*numberChannels());
    auto res = fileHandle->readf(tmp.data(),tmp.size());
    assert(res == f * numberChannels());

    const vector<int> vect_input(channels_input.cbegin(),channels_input.cend());
    const vector<int> vect_output(channels_output.cbegin(),channels_output.cend());

    for(int i = 0; i < f; i++){
        for(auto idx = 0; idx < vect_input.size(); idx++){
            d.inputs[idx][i] = tmp[i+vect_input[idx]];
        }
        for(auto idx = 0 ; idx < vect_output.size(); idx++){
            d.outputs[idx][i] = tmp[i + vect_output[idx]];
        }
    }

    return d;
}


/*

void lireFichierAudio(const std::string& chemin) {
    // Ouvrir le fichier audio avec l'interface C++
    SndfileHandle fichier(chemin);

    // Vérifier si le fichier a bien été ouvert
    if (fichier.error()) {
        std::cerr << "Erreur de chargement du fichier audio : " << fichier.strError() << std::endl;
        return;
    }

    std::cout << "Fichier chargé avec succès !" << std::endl;
    std::cout << "Taux d'échantillonnage : " << fichier.samplerate() << " Hz" << std::endl;
    std::cout << "Nombre de canaux : " << fichier.channels() << std::endl;
    std::cout << "Nombre d'échantillons : " << fichier.frames() << std::endl;

    // Allouer un buffer pour stocker les échantillons audio
    std::vector<float> buffer(fichier.frames() * fichier.channels());

    // Lire les données audio dans le buffer
    sf_count_t read_count = fichier.readf(buffer.data(), fichier.frames());
    if (read_count < fichier.frames()) {
        std::cerr << "Erreur lors de la lecture des échantillons." << std::endl;
        return;
    }

    // Afficher les premières valeurs des échantillons (par exemple, les 10 premiers)
    std::cout << "Premiers échantillons audio (valeurs des 10 premiers échantillons) :" << std::endl;
    for (int i = 0; i < 10 && i < read_count; ++i) {
        std::cout << buffer[i] << " ";
    }
    std::cout << std::endl;
}
*/
