#include "piezoChooserFrame.h"

#include "plotChooser.h"
#include "engine/data.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QChar>
#include <QPair>
#include <QStandardItemModel>

#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

PiezoChooserFrame::PiezoChooserFrame(PlotChooser * chooser, const int piezoID, const QString & nameString, Data * data) : QFrame(), _data(data),
   _chooser(chooser), _piezoID(piezoID), _name(nameString)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoFrameChooser START | piezoID = "<<piezoID<<" name = "<<nameString<<"\n";
#endif
   _frameLayout = new QHBoxLayout;
      
   _checkBox = new QCheckBox;
   _nameLabel = new QLabel(_name);
   _lineStyle = constructLineStyle();
   _lineColor = constructLineColor();
   _smCheckBox = new QCheckBox("show Sm");
   constructSmTickType();
    
   _frameLayout->addWidget(_checkBox);
   _frameLayout->addWidget(_nameLabel);
   _frameLayout->addWidget(_lineStyle);
   _frameLayout->addWidget(_lineColor);
   _frameLayout->addWidget(_smCheckBox);
   _frameLayout->addWidget(_smTickType);
   _frameLayout->addStretch();

   _checkBox->setStyleSheet("font:bold;");
   const QString fontString = QString("font:12px;");
   _lineStyle->setStyleSheet(fontString);
   _lineColor->setStyleSheet(fontString);
   _smTickType->setStyleSheet(fontString);
   
   setLayout(_frameLayout);
   
   setConnections();
   initialize();
   _smTickType->hide();
   
   setFixedHeight(40); // FIXME
   
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoFrameChooser END\n";
#endif
}

void PiezoChooserFrame::checkNameLabelWidth(const int w)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoFrameChooser::checkNameLabelWidth START | piezoName = "<<_name<<" : iniWidth = "<<_nameLabel->sizeHint().width()<<"\n";
#endif
   if (w > _nameLabel->sizeHint().width()) setNameLabelWidth(w);
   else _chooser->fixNameLabelWidths(std::max(_nameLabel->sizeHint().width(),_minWidth));
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoFrameChooser::checkNameLabelWidth END | PiezoName = "<<_name<<" : setWidth = "<<_nameLabel->size().width()<<"\n"; 
#endif    
}

void PiezoChooserFrame::setNameLabelWidth(const int w) 
{
   _nameLabel->setFixedWidth(std::min(w,_maxWidth));
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoFrameChooser::setNameLabelWidth START | PIEZOID = "<<_piezoID<<" | SUGGESTED WIDTH "<<w<<" | NEW WIDTH = "<<_nameLabel->size().width()<<"\n";
#endif 
}

void PiezoChooserFrame::setName(const QString & name)
{
   _nameLabel->setText(name);
   _name = name;
   checkNameLabelWidth(_chooser->maxNameWidth());
}

void PiezoChooserFrame::reduceIDbyOne()
{
   _piezoID = _piezoID - 1;
}

QComboBox * PiezoChooserFrame::constructLineStyle()
{
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoFrameChooser::constructLineStyle START\n";
#endif
   QComboBox * comboBox = new QComboBox;
   comboBox->setEditable(true);
   comboBox->lineEdit()->setReadOnly(true);
   comboBox->lineEdit()->setAlignment(Qt::AlignCenter);  
   comboBox->addItem(" - Line -");
   qobject_cast<QStandardItemModel *>(comboBox->model())->item(0)->setSelectable(false);
   comboBox->addItem("solid"); // WARNING ORDER OF ITEMS TAKEN FROM QCUSTOMPLOT!
   comboBox->addItem("dash");
   comboBox->addItem("dot");
   comboBox->addItem("dash-dot");
   comboBox->addItem("dash-dot-dot");
   for(int i = 0; i < comboBox->count(); i++)
    comboBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
   return comboBox;
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoFrameChooser::constructLineStyle END\n";
#endif
}

QComboBox * PiezoChooserFrame::constructLineColor()
{
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoFrameChooser::constructLineColor START\n";
#endif
   QComboBox * comboBox = new QComboBox;
   comboBox->setEditable(true);
   comboBox->lineEdit()->setReadOnly(true);
   comboBox->lineEdit()->setAlignment(Qt::AlignCenter);  
   comboBox->addItem(" - Color -");
   qobject_cast<QStandardItemModel *>(comboBox->model())->item(0)->setSelectable(false);
   const int noOfColors = _chooser->colors().size();
   for (int i=0;i<noOfColors;i++) comboBox->addItem(_chooser->colors()[i].second.first,_chooser->colors()[i].first);
   for(int i = 0; i < comboBox->count(); i++)
    comboBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
   return comboBox;
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoFrameChooser::constructLineColor END\n";
#endif
}

// QComboBox * PiezoChooserFrame::constructLineColor()
// {
// #ifdef QDEBUG_ALL
//    qDebug()<<"PiezoFrameChooser::constructLineColor START\n";
// #endif
//    QComboBox * comboBox = new QComboBox;
//    comboBox->setEditable(true);
//    comboBox->lineEdit()->setReadOnly(true);
//    comboBox->lineEdit()->setAlignment(Qt::AlignCenter);  
//    comboBox->addItem(" - Color -");
//    qobject_cast<QStandardItemModel *>(comboBox->model())->item(0)->setSelectable(false);
//    const int noOfColors = _chooser->colors().size();
//    for (int i=0;i<noOfColors;i++) comboBox->addItem(_chooser->colors()[i].first,"");
// //    for(int i = 0; i < comboBox->count(); i++)
// //     comboBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
//    return comboBox;
// #ifdef QDEBUG_ALL
//    qDebug()<<"PiezoFrameChooser::constructLineColor END\n";
// #endif
// }

void PiezoChooserFrame::constructSmTickType()
{
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoFrameChooser::constructSmTickType START\n";
#endif
   _smTickType = new QComboBox;
   _smTickType->setEditable(true);
   _smTickType->lineEdit()->setReadOnly(true);
   _smTickType->lineEdit()->setAlignment(Qt::AlignCenter);  
   _smTickType->addItem(" - Tick -");
   qobject_cast<QStandardItemModel *>(_smTickType->model())->item(0)->setSelectable(false);
   _smTickType->addItem(QString(QChar(0x2022)));
//    _smTickType->addItem(QString(QChar(0x274C)));
   _smTickType->addItem("x");
   _smTickType->addItem(QString(QChar(0x002B)));
   _smTickType->addItem(QString(QChar(0x25CB)));
   _smTickType->addItem(QString(QChar(0x25CF)));
   _smTickType->addItem(QString(QChar(0x25A1)));
   _smTickType->addItem(QString(QChar(0x25C7)));
   _smTickType->addItem(QString(QChar(0x2734)));
   _smTickType->addItem(QString(QChar(0x25B3)));
   _smTickType->addItem(QString(QChar(0x25BD)));
   //QString str = QChar(0x4FF0);
   //_smTickType->addItem(str);
   for(int i = 0; i < _smTickType->count(); i++)
    _smTickType->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoFrameChooser::constructSmTickType END\n";
#endif
}

void PiezoChooserFrame::setConnections()
{
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoFrameChooser::setConnections START\n";
#endif
   // QCheckBox:
   connect(_checkBox,SIGNAL(stateChanged(int)),
           this,SLOT(addRemovePlot(int)));
   connect(_smCheckBox,SIGNAL(stateChanged(int)),
           this,SLOT(addRemovePlot(int)));
   // Line style, color and tick type:
   connect(_lineStyle,SIGNAL(activated(int)),
           this,SLOT(setLineStyle(int)));
   connect(_lineColor,SIGNAL(activated(int)),
           this,SLOT(setLineColor(int)));
   connect(_smTickType,SIGNAL(activated(int)),
           this,SLOT(setSmTickType(int)));      
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoFrameChooser::setConnections END\n";
#endif   
}

void PiezoChooserFrame::initialize()
{
   if(_data->getModel().sizePlotChPiez()>0)
   {
      _checkBox->setChecked(_data->getModel().plotChPiez(
               _piezoID).piezChecked);
      _lineStyle->setCurrentIndex(_data->getModel().plotChPiez(_piezoID).piezLine);    
      emit _lineStyle->activated(_data->getModel().plotChPiez(_piezoID).piezLine);
      _lineColor->setCurrentIndex(_data->getModel().plotChPiez(_piezoID).piezColor);
      emit _lineColor->activated(_data->getModel().plotChPiez(_piezoID).piezColor);
      _smCheckBox->setChecked(_data->getModel().plotChPiez(_piezoID).smChecked);
      if(_data->getModel().plotChPiez(_piezoID).smChecked)
      {
         _smTickType->setCurrentIndex(_data->getModel().plotChPiez(_piezoID).smType);
         emit _smTickType->activated(_data->getModel().plotChPiez(_piezoID).smType);
      }
   }else
      _checkBox->setChecked(false);
}

void PiezoChooserFrame::addRemovePlot(const int i)
{
   const bool show = bool(i);
   QCheckBox * checkBox = static_cast<QCheckBox*>(sender());
   int id;
   if (checkBox == _checkBox) id = 0;
   else if (checkBox == _smCheckBox) id = 1;
   else {qDebug()<<"PiezoChooserFrame::ERR01\n";exit(-1);}
   if (id == 0)
   {
      if (show) 
      {
         if(_lineColor->currentIndex()==0)
            _lineColor->setCurrentIndex(1);
         if(_lineStyle->currentIndex()==0)
            _lineStyle->setCurrentIndex(1);
         _chooser->addPlot(itemFlag,_piezoID, _lineColor->currentIndex()-1,
               _lineStyle->currentIndex(),_name);
         if (_smCheckBox->isChecked())
         {
            if(_smTickType->currentIndex()==0)
               _smTickType->setCurrentIndex(1);
            _chooser->addSmPlot(itemFlag,_piezoID,_lineColor->currentIndex()-1,
                  _smTickType->currentIndex(),_name);
         }
      }
      else 
      {
         _chooser->removePlot(itemFlag,_piezoID);
         if (_smCheckBox->isChecked())
         {
            _chooser->removePlot(itemFlag,_piezoID,true,false);
            _smCheckBox->setChecked(false);
         }
      }
   }
   else // _smCheckBox
   {
      if (show) 
      {
         if (_checkBox->isChecked()){ 
            if(_lineColor->currentIndex()==0)
               _lineColor->setCurrentIndex(1);
            if(_smTickType->currentIndex()==0)
               _smTickType->setCurrentIndex(1);
            _chooser->addSmPlot(itemFlag,_piezoID,_lineColor->currentIndex()-1,
                  _smTickType->currentIndex(),_name);
         }
         _smTickType->show();
      }
      else 
      {
         if (_checkBox->isChecked()) 
            _chooser->removePlot(itemFlag,_piezoID,true,false);
         _smTickType->hide();
      }
   }
}

void PiezoChooserFrame::setLineStyle(const int i)
{
   if (_checkBox->isChecked())
   {
      if(_lineColor->currentIndex()==0)
         _lineColor->setCurrentIndex(1);
      _chooser->setLineProperties(itemFlag,_piezoID,
            _lineColor->currentIndex()-1, i);
   }
}

void PiezoChooserFrame::setLineColor(const int i)
{
   if (_checkBox->isChecked())
   {
      if(_lineStyle->currentIndex()==0)
         _lineStyle->setCurrentIndex(1);
      _chooser->setLineProperties(itemFlag,_piezoID, i-1,
            _lineStyle->currentIndex());
      _chooser->setSmTickType(itemFlag,_piezoID, _smTickType->currentIndex(),
            i-1);
   }
}

void PiezoChooserFrame::showSm(const int i)
{
   if (bool(i)) _smTickType->show();
   else _smTickType->hide();
}

void PiezoChooserFrame::setSmTickType(const int i)
{
   if (_checkBox->isChecked()) 
   {
      if(_lineColor->currentIndex()==0)
         _lineColor->setCurrentIndex(1);
      _chooser->setSmTickType(itemFlag,_piezoID, i,
         _lineColor->currentIndex()-1);
   }
}
