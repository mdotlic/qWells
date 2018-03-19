#ifndef PLOTSETTINGS_H

#include <QObject>

class QWells;
class QDialog;

class PlotSettings : public QObject
{
   Q_OBJECT
   public:
      PlotSettings(QWells*);
      QWells * qWells();
   public slots:
      void startDialog();
   signals:
      void setZoomFactor(double, int);
   private:
      QWells * _qWells;
      QDialog * _dialog;
      double _zoomFactor = 0.95;
      int _lineWidth = 2;

};
#endif
