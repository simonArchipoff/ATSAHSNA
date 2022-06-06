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
  uint sr = 10000;
  std::vector<double> v  = sweep(100,1000,1,sr);

  auto r = spectrogram(v,10,5000,10,sr);

  qSpectrogram foo;
  //foo.setSpectrogram(r.data);
  //foo.show();
#else
  MainWindow w;
  w.show();
#endif
  return a.exec();
}
