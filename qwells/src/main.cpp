#include <QApplication>
#include <QTranslator>
#include <QSplashScreen>
#include <QTimer>
#include "qWells.h"
#include "mainMenu.h"
#include "license.h"
#include <QDebug>
#include <QString>
//#include <windows.h>

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
#if (defined(__MINGW32__) || defined(__MINGW64__))
   bool active;
   License license(active);
   if(active)
   {
#endif
   QTranslator translator;
   app.installTranslator(&translator);
   QWells qWells;

   QPixmap pixmap(":/resources/images/qWellsSplash.png");
   QSplashScreen * splash = new QSplashScreen;
   splash->setPixmap(pixmap.scaledToHeight(600,Qt::SmoothTransformation));
   splash->show();

   QTimer::singleShot(2000,splash,SLOT(close()));


   qApp->processEvents();//This is used to accept a click on the screen so that user can cancel the screen
   //QSplashScreen splash(pixmap);
  //QTimer::singleShot(2500,qWells,SLOT(show()));

   if(argc==2)
   {
     QString fileName= QString::fromStdString(argv[1]);
     qWells.mainMenu()->loadFile(fileName);
     QString title = qWells.title();
     qWells.setWindowTitle(title.append(" - ").append(fileName));
     emit qWells.modelLoaded();
   }
   
   qWells.show();

   return app.exec();
#if (defined(__MINGW32__) || defined(__MINGW64__))
   }else
      return 1;
#endif
}


