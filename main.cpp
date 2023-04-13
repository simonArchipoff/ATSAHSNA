#include <QCoreApplication>
#include <QDebug>
#include "constants.h"
#include "qcommandlineparser.h"

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QCommandLineParser>

#include <backendFaust.h>
#include <backendJack.h>

#include <signalHarmonics.h>
#include <signalResponse.h>
#include <signalSpectrogram.h>


struct OptionsParser : private QCommandLineParser{
public:
  OptionsParser(QStringList arguments){
    this->arguments = arguments;
    addHelpOption();
    addVersionOption();
    addPositionalArgument("analysis","analysis to perform <response|distortion|spectrogram>");// <faust_program_file|faust_code>) [options]");
    addPositionalArgument("backend", "backend to measure <jack|faust program [arg1=val1...]>");
    parse(arguments);
    const QStringList args = positionalArguments();
    auto command = args.isEmpty() ? QString() : args.first();
    auto backend = args.size() < 2 ? QString() : args[1];


    if(command == "response"){
        addOptions(response_options);
        computation = RESPONSE;
      } else if(command == "distortion") {
        addOptions(distortion_option);
        computation = HARMONICS;
      } else if(command == "spectrogram") {
        addOptions(spectrogram_option);
        computation = SPECTROGRAM;
      } else {
        showHelp();
      }


    if(backend == "jack"){
        addOptions(jack_options);
        this->backend = JACK;
      } else if(backend == "faust" && args.size() >= 3){
        addOptions(faust_options);
        this->backend = FAUST;
      } else {
        showHelp();
      }
    clearPositionalArguments();
    addPositionalArgument(command,"compute " + command);
    addPositionalArgument(backend,"use " + backend + " backend");
    if(backend == "faust"){
        addPositionalArgument("faust_program","");
      }
    process(arguments);
  }

  bool getParamBackend(ParamFaust * paramfaust){
    bool success=true;
    bool tmp;
    if(backend == FAUST){
        auto pos_arg = positionalArguments();
        assert(!pos_arg.empty());
        std::vector<std::pair<std::string,double>> params;
        for(int i = 3; i < pos_arg.size() ; i++){
            auto s = pos_arg[i].split('=');
            success |= s.size() == 2;
            auto a = s[0].toStdString();
            auto v = s[1].toDouble(&tmp);
            success |= tmp;
            params.push_back(std::pair{a,v});
          }
        paramfaust->file_or_code = arguments[3].toStdString();
        paramfaust->params = params;
        paramfaust->sample_rate = value(faust_samplerate).toInt(&tmp);
        success|=tmp;
        return success;
      }
    return false;
  }
  bool getParamBackend(ParamJack * paramjack){
    abort();
  }

  bool getParamResponse(ParamResponse * paramresponse){
    bool tmp,success=true;
    paramresponse->freqMin = value(freq_min).toDouble(&tmp);
    success |= tmp;
    paramresponse->freqMax = value(freq_max).toDouble(&tmp);
    success |= tmp;
    return success;
  }


/*
  bool getParamComputation(){
    assert(command != UNDEFCOMPUTATION);
    if(command == RESPONSE) {
      }else if(command == SPECTROGRAM){
      }else if(command == HARMONICS){
      }
    abort();
  }*/
  backend_type backend = UNDEFBACKEND;
  signal_gen_type gen_type = CHIRP;
  computation_type computation = UNDEFCOMPUTATION;

protected:
  QStringList arguments;

  QCommandLineOption faust_samplerate{"sample-rate","faust samplerate in FPS","samplerate","44100"};

  QList<QCommandLineOption> faust_options{faust_samplerate};


  QCommandLineOption jack_input  {{"inputs","i"},"comma separated list of jack ports or an integer","connect_inputs","1"};
  QCommandLineOption jack_output {{"outputs","o"},"comma separated list of jack ports or an integer","connect_outputs","1"};
  QCommandLineOption jack_latency{{"latency","l"},"latency in frames, automatically computed by default","latency","-1"};

  QList<QCommandLineOption> jack_options{jack_input,jack_output,jack_latency};


  QCommandLineOption freq_min {"freq-min","minimum frequency in Hz","freq-min","20"};
  QCommandLineOption freq_max {"freq-max","maximum frequency in Hz","freq-max","20000"};

  QCommandLineOption duration{{"duration","d"},"duration in s","duration","1"};
  QList<QCommandLineOption> response_options{freq_min, freq_max, duration};

  QCommandLineOption frequency{{"frequency","f"},"frequency for harmonic analysis","frequency","1000"};

  QList<QCommandLineOption> distortion_option{frequency,duration};

  QList<QCommandLineOption> spectrogram_option{response_options};

};


int main(int argc, char *argv[]){
  QCoreApplication app(argc, argv);
  QCoreApplication::setApplicationName(APPNAME);
  QCoreApplication::setApplicationVersion(APPVERS);
  OptionsParser o(QCoreApplication::arguments());
  ParamFaust f;
  o.getParamBackend(&f);

  Backend * b;
  if(o.backend == FAUST){
    b = make_faust_backend(f);
    }
  if(o.computation == RESPONSE) {
    ParamResponse p;
    auto r = compute<ImpulseResponse>(b, p);
    for(auto & o : r){
        FDFLOG foo(o.response);
        break;
      }
    }
  return 0;
}
