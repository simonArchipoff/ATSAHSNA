 #include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDebug>
#include "view/qspectrogram.h"
#include "backend/spectrogram.h"


int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  /*
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ATSAHSNA_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
*/
#if 1
  uint sr = 44100;
  std::vector<double> v  = sweep(16000,16000,2,sr);
  std::vector<double> v1 = sweep(50,10000,2,sr);
  std::vector<double> v2 = sweep(4410,4410,0.1,sr);
  for(int i = 0; i < v2.size() ; i++) {
      v[i+sr] += sin(M_PI * i/v2.size()) * v2[i] ;
    }
  v[sr/2] = 100;

  auto r = spectrogram(v,10,128,sr);

  qSpectrogram foo;

  foo.setSpectrogram(r);
  foo.show();
#else
  MainWindow w;
  w.show();
#endif
  return a.exec();
}
