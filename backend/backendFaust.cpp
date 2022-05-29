#include "backendFaust.h"

#include <list>
#include <QString>
#include <QTranslator>

BackendFaust::~BackendFaust(){
    delete dspInstance;
    deleteDSPFactory(factory);
}

bool BackendFaust::setCode(std::string dspCode){
  const char* argv[] = {"--double-precision-floats"};
  const int argc =sizeof(argv)/sizeof(*argv);
  // compiling in memory (createDSPFactoryFromFile could be used alternatively)
  factory = createDSPFactoryFromString("UI", dspCode, argc, argv ,"", errorString,0);
  dspInstance = factory ? factory->createDSPInstance():nullptr;
  return dspInstance;
}

uint BackendFaust::getSampleRate() const{
  return dspInstance->getSampleRate();
}

bool BackendFaust::isReady() const {
    return dspInstance;
}

std::string BackendFaust::getErrorMessage(){
    return errorString;
}

uint BackendFaust::numberInput()const {
  return dspInstance->getNumInputs();
}
uint BackendFaust::numberOutput() const{
  return dspInstance->getNumOutputs();
}

void BackendFaust::notify(){
  treatRequest();
}

void BackendFaust::treatRequest(){
  request r;
  bool b = requests.try_dequeue(r);
  struct V {
    V(BackendFaust * b):t(b){};
    void operator()(RequestMeasure &r){
      assert(r.input.size() >0);
      uint inputSize = r.input[0].size();
      vector<double*> inputs{t->numberInput()};
      for(uint i = 0; i < inputs.size(); i++){
          inputs[i] = r.input[i].data();
        }
      vector<double*> outputs{t->numberOutput()};
      vector<vector<double>> out{t->numberOutput()};

      for(uint i = 0; i < outputs.size(); i++){
          out[i].resize(inputSize);
          outputs[i] = out[i].data();
        }
      for(uint i = 0; i < outputs.size(); i++){
          for(uint j = 0; j < inputSize; j++){
              outputs[i][j] = ~0;
            }
        }

      uint block = 4*1024;
      uint nb_block = inputSize / block;

      for(uint i = 0; i < nb_block; i++){
          t->dspInstance->compute(block,inputs.data(), outputs.data());
          for(uint j = 0; j < outputs.size(); j++){
              outputs[j] = outputs[j] + block;
            }
          for(uint j = 0; j < inputs.size(); j++){
              inputs[j] = inputs[j] + block;
            }
        }

      t->dspInstance->compute(inputSize % block,inputs.data(), outputs.data());

      t->responses.enqueue(Output{out});
    };
    void operator()(RequestPartialOutput &){
      qDebug("RequestPartialOutput called on faust backend");
    };
    void operator()(CancelMeasure &){
      qDebug("CancelMeasure called on faust backend");
    };
    BackendFaust *t;
  };
  if(b){
      std::visit(V(this),r);
    }
}

std::list<GUI*> GUI::fGuiList;

BackendFaustQT::BackendFaustQT(QWidget * parent):BackendFaust(){
    ui = new QTGUI(parent);
}

bool BackendFaustQT::setCode(QString dspCode, uint sampleRate){
  if(BackendFaust::setCode(dspCode.toStdString())){
      dspInstance->buildUserInterface(ui);
      dspInstance->init(sampleRate);
      return true;
    }
  return false;
}

BackendFaustQT::~BackendFaustQT(){
    delete ui;
}

QWidget * BackendFaustQT::getUI(){
    return ui;
}

std::variant<BackendFaustQT *, QString>
create_faust_qt(QString dspCode, int sampleRate, QWidget * parent){
    auto tmp = new BackendFaustQT(parent);
    tmp->setCode(dspCode, sampleRate);
    if(tmp->isReady())
        return dsp_or_error{tmp};
    QString s = QString::fromStdString(tmp->getErrorMessage());
    delete tmp;
    return dsp_or_error{s};
}
