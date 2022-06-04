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
  std::vector<double> v(1024*1024);

  for(uint i = 0; i < v.size() ; i++)
    v[i]=sin(1000*i/(2*3.14))+1;
  auto r = v; //spectrogram(v);

  qSpectrogram foo;
  foo.setSpectrogram(r);
  foo.show();
#endif
  MainWindow w;
  w.show();

  return a.exec();
}
