#ifndef TABLESIZE_H
#define TABLESIZE_H

#include "qWells.h"
#include "settings/tableSettings.h"

#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

class TableSize 
{
public:
   TableSize(TableSettings * settings) : _settings(settings) 
   { 
      resetAll(); 
   }
   
   void setVHeaderWidth(const size_t w, const size_t rows)
   {
      if (rows < 10) 
      {
         if (_vHeaderWidthIsSet[0]) return;
         _vHeaderWidth[0] = w;
         _vHeaderWidthIsSet[0] = true;
#ifdef QDEBUG_ALL
   qDebug()<<"TableSize | vHeaderWidth (rows < 10) = "<<w<<"\n";
#endif
      }
      else if (rows < 100)
      {
         if (_vHeaderWidthIsSet[1]) return;
         _vHeaderWidth[1] = w;
         _vHeaderWidthIsSet[1] = true;
#ifdef QDEBUG_ALL
   qDebug()<<"TableSize | vHeaderWidth (10 <= rows < 100) = "<<w<<"\n";
#endif
      }
      else if (rows < 1000)
      {
         if (_vHeaderWidthIsSet[2]) return;
         _vHeaderWidth[2] = w;
         _vHeaderWidthIsSet[2] = true;
#ifdef QDEBUG_ALL
   qDebug()<<"TableSize | vHeaderWidth (100 <= rows < 1000) = "<<w<<"\n";
#endif
      }
      else
      {
         if (_vHeaderWidthIsSet[3]) return;
         _vHeaderWidth[3] = w;
         _vHeaderWidthIsSet[3] = true;
#ifdef QDEBUG_ALL
   qDebug()<<"TableSize | vHeaderWidth (1000 <= rows) = "<<w<<"\n";
#endif
      }
      if (callSettings()) _settings->setSizes(_cellHeight,_hHeaderHeight);
   }
   
   void setHHeaderHeight(const size_t h)
   {
      _hHeaderHeight = h;
      _hHeaderHeightIsSet = true;
      if (callSettings()) _settings->setSizes(_cellHeight,_hHeaderHeight);
#ifdef QDEBUG_ALL
   qDebug()<<"TableSize | hHeaderHeight = "<<h<<"\n";
#endif
   }
   
   void setCellWidth(const size_t cw)
   {
      _cellWidth = cw;
      _cellWidthIsSet = true;
      if (callSettings()) _settings->setSizes(_cellHeight,_hHeaderHeight);
#ifdef QDEBUG_ALL
   qDebug()<<"TableSize | _cellWidth = "<<cw<<"\n";
#endif
   }
   
   void setCellHeight(const size_t ch)
   {
      _cellHeight = ch;
      _cellHeightIsSet = true;
      if (callSettings()) _settings->setSizes(_cellHeight,_hHeaderHeight);
#ifdef QDEBUG_ALL
   qDebug()<<"TableSize | _cellHeight = "<<ch<<"\n";
#endif
   }
   
   void setScrollBarWidth(const size_t sbw)
   {
      _scrollWidth = sbw;
   }
   
   size_t getRowsForScroll(const int height)
   {
      return int((height - _hHeaderHeight - 2)/_cellHeight);
   }
   
   size_t getVHeader(const size_t rows)
   {
      if (rows < 10) return _vHeaderWidth[0];
      else if (rows < 100) return _vHeaderWidth[1];
      else if (rows < 1000) return _vHeaderWidth[2];
      else return _vHeaderWidth[3];
   }
   
   bool isVHeaderSet(const size_t i)
   {
      return _vHeaderWidthIsSet[i];
   }
   
   size_t getHHeader()
   {
      return _hHeaderHeight;
   }
   
   size_t getCellWidth()
   {
      return _cellWidth;
   }
   
   size_t getCellHeight()
   {
      return _cellHeight;
   }
   
   size_t getScrollWidth()
   {
      return _scrollWidth;
   }
   
   size_t getTableHeight(const int rows)
   {
      const int nr = std::min(rows,_settings->getCurrentRowsShown());
//       return _hHeaderHeight + nr*_cellHeight + 2;
      return _hHeaderHeight + nr*_cellHeight + 4;
   }

   bool callSettings()
   {
      return (_hHeaderHeightIsSet && _cellWidthIsSet && _cellHeightIsSet);
   }
   
   bool allSet(const int rows)
   {
      int i=rows;
      int k=0;
      while (int(i/10) > 0)
      {
         i = int(i/10);
         k = k + 1;
      }
      bool isSet = true;
      for (int i=0;i<=k;i++) isSet = isSet && _vHeaderWidthIsSet[i];
      isSet = (isSet && _hHeaderHeightIsSet && _cellWidthIsSet && 
               _cellHeightIsSet);
      return isSet;
   }
      
   void resetAll()
   {
      for (size_t i=0;i<=3;i++) _vHeaderWidthIsSet[i] = false;
      _hHeaderHeightIsSet = false;
      _cellWidthIsSet = false;
      _cellHeightIsSet = false;
   }
   
   TableSettings * settings() { return _settings; }

private:
   TableSettings * _settings;
   size_t _vHeaderWidth[4];
   bool _vHeaderWidthIsSet[4];
   size_t _hHeaderHeight;
   bool _hHeaderHeightIsSet;
   size_t _cellWidth, _cellHeight;
   bool _cellWidthIsSet, _cellHeightIsSet;
//    const size_t _scrollWidth = 15;
   size_t _scrollWidth;
};
#endif // TABLESIZE_H