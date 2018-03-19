#ifndef QWELLSPLOT_H
#define QWELLSPLOT_H
#include "qcustomplot.h"
#include "helperFunctions.h"
#include <QString>

class PlotSettings;
class QStatusBar;

class QWellsPlot : public QCustomPlot
{
   Q_OBJECT
   public:
      QWellsPlot(){}
      QWellsPlot(size_t nWells, size_t nPiez, PlotSettings *, double *);

      void mouseDoubleClickEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      
      void changeGraphIds(const size_t iGraph);
      void setGraph(const QString & name);
     // void addPlot(const QVector<double> & x, 
     //    const QVector<double> & y, const QColor & color, const QString & name,
     //    const size_t iGraph, const size_t style, const size_t scatter = 0);
      void setStyle(const size_t iGraph, const size_t style);
      void setScatter(const size_t iGraph, const size_t style, 
            const QColor & color);
      void setLineStyle (const QColor & color, const size_t iGraph, 
            const size_t style);
      void removePlot (const size_t iGraph);

      void addWell();
      void addPiez();
      void deleteWell(const size_t i);
      void deletePiez(const size_t i);

      void setWellPlotName(const size_t wellId, const QString & name);
      void setPiezPlotName(const size_t piezId, const QString & name);

      void addWellPlot(const size_t wellId, const QColor & color, 
            const size_t style, const QString & name);
      void addWellSmPlot(const size_t wellId, const QColor & color, 
            const size_t scatterStyle, const QString & name);
      void addWellWithoutLossPlot(const size_t wellId, const QColor & color,
            const size_t style, const QString & name);
      void addPiezPlot(const size_t piezId, const QColor & color, 
            const size_t style, const QString & name);
      void addPiezSmPlot(const size_t piezId, const QColor & color, 
            const size_t scatterStyle, const QString & name);
      void removeWellPlot(const size_t wellId);
      void removeWellSmPlot(const size_t wellId);
      void removeWellWithoutLossPlot(const size_t wellId);
      void removePiezPlot(const size_t piezId);
      void removePiezSmPlot(const size_t piezId);
      void setDataOfPlot(const QVector<double> & x, 
            const QVector<double> & y, const size_t iGraph);
   //   template<class Result> 
   //      void setAllData(QVector<double> t, const Result * result);
      const std::vector<size_t> & wellPlot()const {return _wellPlot;}
      const std::vector<size_t> & wellSmPlot()const {return _wellSmPlot;}
      const std::vector<size_t> & wellWithoutLossPlot()const 
      {return _wellWithoutLossPlot;}
      const std::vector<size_t> & piezPlot()const {return _piezPlot;}
      const std::vector<size_t> & piezSmPlot()const {return _piezSmPlot;}

      size_t nwells() const {return _wellPlot.size();}
      size_t npiez() const {return _piezPlot.size();}

      size_t wellPlot(const size_t i)const {return _wellPlot[i];}
      size_t wellSmPlot(const size_t i)const {return _wellSmPlot[i];}
      size_t wellWithoutLossPlot(const size_t i)const 
      {
         return _wellWithoutLossPlot[i];
      }
      size_t piezPlot(const size_t i)const {return _piezPlot[i];}
      size_t piezSmPlot(const size_t i)const {return _piezSmPlot[i];}

      void setWellPlot(const std::vector<size_t> & v) { _wellPlot=v;}   
      void setWellSmPlot(const std::vector<size_t> & v) { _wellSmPlot=v;}
      void setWellWithoutLossPlot(const std::vector<size_t> & v)
      {
         _wellWithoutLossPlot=v;
      }
      void setPiezPlot(const std::vector<size_t> & v){_piezPlot=v;}
      void setPiezSmPlot(const std::vector<size_t> & v){_piezSmPlot=v;}

      template<class Result>
      void setAllData(const QVector<double> & t, const Result * result)
      {
         size_t nwells=_wellPlot.size();
         size_t npiez=_piezPlot.size();
         for(size_t iwell=0;iwell<nwells;iwell++)
         {
            if(_wellPlot[iwell]==std::numeric_limits<size_t>::max())
               continue;
            else
               setDataOfPlot(t,qvecOperation::qvecPlus(result->well(iwell),
                        result->wellLosses(iwell)), _wellPlot[iwell]);
         }
         for(size_t iwell=0;iwell<nwells;iwell++)
         {
            if(_wellWithoutLossPlot[iwell]==std::numeric_limits<size_t>::max())
               continue;
            else
               setDataOfPlot(t,result->well(iwell),_wellWithoutLossPlot[iwell]);
         }
         for(size_t iwell=0;iwell<nwells;iwell++)
         {
            if(_wellSmPlot[iwell]==std::numeric_limits<size_t>::max())
               continue;
            else
               setDataOfPlot(conversion::point2FirstToQvec(
                        result->data()->wells()[iwell].Sm(), 
                        result->data()->getModel().timeUnit()),
                     conversion::point2SecondToQvec(
                        result->data()->wells()[iwell].Sm()),
                     _wellSmPlot[iwell]);
         }
         for(size_t ipiez=0;ipiez<npiez;ipiez++)
         {
            if(_piezPlot[ipiez]==std::numeric_limits<size_t>::max())
               continue;
            else
               setDataOfPlot(t,result->piez(ipiez),_piezPlot[ipiez]);
         } 
         
         for(size_t ipiez=0;ipiez<npiez;ipiez++)
         {
            if(_piezSmPlot[ipiez]==std::numeric_limits<size_t>::max())
               continue;
            else
               setDataOfPlot(
                conversion::point2FirstToQvec(
                   result->data()->piezometers()[ipiez].Sm(),
                   result->data()->getModel().timeUnit()), 
                conversion::point2SecondToQvec(
                   result->data()->piezometers()[ipiez].Sm()),
                _piezSmPlot[ipiez]);
         }
      }

      template<class Result>
      void setChangedLosses(const QVector<double> & t, const Result * result, 
            const size_t wellId)   
      {
         setDataOfPlot(t,qvecOperation::qvecPlus(result->well(wellId),
                        result->wellLosses(wellId)), _wellPlot[wellId]);
      }

      int lineWidth()const
      {
         return _lineWidth;
      }

      void setLineWidth(const int l)
      {
         _lineWidth = l;
      }

      void setStaticLevel(bool i)
      {
         _useStaticLevel = i;
      }

      bool useStaticBoolean()
      {
         return _useStaticLevel;
      }
      
   private:
      std::vector<size_t> _wellPlot;
      std::vector<size_t> _wellSmPlot;
      std::vector<size_t> _wellWithoutLossPlot;
      std::vector<size_t> _piezPlot;
      std::vector<size_t> _piezSmPlot;
      PlotSettings * _plotSettings;
      bool _legendShow;
      int _lineWidth = 2;
      double * _staticLevel;
      bool _useStaticLevel = false;
      //element points to the Graph number
   public slots:
      void mouseWheel();
      void selectionChanged();
      void setTimeUnit(const size_t timeUnit);
      void setZoomFactorSlot(const double, const int);
      void showLegendSlot();      
      void hideLegendSlot();
      void setTicksSlot();
      void contextMenuRequest(QPoint pos);
      void minMaxDialog();
      void setRangeStaticLevel(QCPRange);
   private slots:
      void showCoord(QMouseEvent*);

};
#endif
