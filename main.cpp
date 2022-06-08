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
#if 0
  uint sr = 44100;
  std::vector<double> v  = sweep(150,150,2,sr);
  std::vector<double> v1 = sweep(100,100,2,sr);
  std::vector<double> v2 = sweep(10000,10000,2,sr);

  for(int i = 0; i < v.size() ; i++){
      v[i] = 0.5 * v[i] +  0.5*v1[i] + 1 *v2[i];
    }

  auto r = spectrogram(v,10,64,sr);

  qSpectrogram foo;

  foo.setSpectrogram(r);
  foo.show();
#else
  MainWindow w;
  w.show();
#endif
  return a.exec();
}
