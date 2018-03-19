#ifndef PRINTTAB_H
#define PRINTTAB_H

#include <QFrame>
#include "widgets/qcustomplot/qwellsPlot.h"
#include "engine/data.h"
#include <QDebug>

//class QWells;
//class Data;
//class QWellsPlot;
class QPushButton;

class PrintTab : public QFrame
{
   Q_OBJECT
public:
   PrintTab(Data * data);
   void refresh(QWellsPlot * splot);
   void removeGraphs(); 
   void setQGraphs();
   void setGraphs(QWellsPlot * splot);

   template<class Result>
   void refresh(QWellsPlot * splot, const Result * result)
   {
      setGraphs(splot);
      _splot->setAllData(_data->timeInSMHD(), result);
      _splot->xAxis->setRangeUpper(splot->xAxis->range().upper * 1.35);
      _splot->replot();
      setQGraphs();
   }
   QPushButton * exportQButton() { return _saveQButton; }
   QPushButton * exportSButton() { return _saveSButton; }
   QWellsPlot * splot() {return _splot;}

private slots:
   void exportQToPdf();
   void exportSToPdf();
   void mouseWheel();
   void selectionChanged();
   void setZoomFactor(const double, const int);
   void contextMenuRequestQ(QPoint pos);
   void contextMenuRequestS(QPoint pos);
   void titleDialog();
   void titleDialogS();
   void qminMaxDialog();
   void sminMaxDialog();
   void setQTicksSlot();
   void dataDialog();

private:
   QCustomPlot * _qplot;
   QWellsPlot * _splot;
   Data * _data;
   QPushButton * _saveQButton;
   QPushButton * _saveSButton;
   QCPPlotTitle * _qtitle;
   QCPPlotTitle * _stitle;
   QCPPlotTitle * _sdata;
   bool _hasSTitle;
   bool _hasSData;
};
#endif // PRINTTAB_H
