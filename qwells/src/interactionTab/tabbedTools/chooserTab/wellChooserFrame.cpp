#include "wellChooserFrame.h"

#include "plotChooser.h"
#include "engine/data.h"

#include <QFrame>
#include <QGridLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QIcon>
#include <QChar>
#include <QPair>
#include <QStandardItemModel>
#include <QImage>
#include <QPainter>
#include <QRect>

#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

WellChooserFrame::WellChooserFrame(PlotChooser * chooser, const int wellID, const QString & nameString, Data * data) : QFrame(), _data(data),_chooser(chooser), _wellID(wellID), _name(nameString)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellFrameChooser START | wellID = "<<wellID<<" name = "<<nameString<<"\n";
#endif
   _frameLayout = new QGridLayout;
      
   _checkBox = new QCheckBox;
   _nameLabel = new QLabel(_name);
   _lossCheckBox = new QCheckBox("with loss");
   _lossLineStyle = constructLineStyle();
   _lossLineColor = constructLineColor();
   _noLossCheckBox = new QCheckBox("without");
   _noLossLineStyle = constructLineStyle();
   _noLossLineColor = constructLineColor();
   _smCheckBox = new QCheckBox("show Sm");
   constructSmTickType();
    
   _frameLayout->addWidget(_checkBox,0,0,Qt::AlignLeft);
   _frameLayout->addWidget(_nameLabel,0,1,Qt::AlignLeft);
   _frameLayout->addWidget(_lossCheckBox,0,2,Qt::AlignLeft);
   _frameLayout->addWidget(_noLossCheckBox,1,2,Qt::AlignLeft);
   _frameLayout->addWidget(_lossLineStyle,0,3);
   _frameLayout->addWidget(_noLossLineStyle,1,3);
   _frameLayout->addWidget(_lossLineColor,0,4);
   _frameLayout->addWidget(_noLossLineColor,1,4);
   _frameLayout->addWidget(_smCheckBox,0,5);
   _frameLayout->addWidget(_smTickType,1,5);
   _frameLayout->setColumnStretch(6,1);

   _checkBox->setStyleSheet("font:bold;");
   QString fontString = QString("font:12px;");
   _lossLineStyle->setStyleSheet(fontString);
   _lossLineColor->setStyleSheet(fontString);
   _smTickType->setStyleSheet(fontString);
   _noLossLineStyle->setStyleSheet(fontString);
   _noLossLineColor->setStyleSheet(fontString);
   
   setLayout(_frameLayout);
   
   setConnections();
   _smTickType->hide();
   initialize();
   
//    const int frameHeight = 2 * _lossCheckBox->sizeHint().height() + 10;
//    const int frameHeight = 60;
//    qDebug()<<" frameHeight = "<<frameHeight<<"\n";
//    setFixedHeight(frameHeight);
   setFixedHeight(66); // FIXME height
   
#ifdef QDEBUG_ALL
   qDebug()<<"WellFrameChooser END\n";
#endif
}

void WellChooserFrame::checkNameLabelWidth(const int w)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellFrameChooser::checkNameLabelWidth START | wellName = "<<_name<<" : iniWidth = "<<_nameLabel->sizeHint().width()<<"\n";
#endif
   if (w > _nameLabel->sizeHint().width()) setNameLabelWidth(w);
   else _chooser->fixNameLabelWidths(std::max(_nameLabel->sizeHint().width(),_minWidth));
#ifdef QDEBUG_ALL
   qDebug()<<"WellFrameChooser::checkNameLabelWidth END | WellName = "<<_name<<" : setWidth = "<<_nameLabel->size().width()<<"\n"; 
#endif  
}

void WellChooserFrame::setNameLabelWidth(const int w) 
{
   _nameLabel->setFixedWidth(std::min(w,_maxWidth)); 
#ifdef QDEBUG_ALL
   qDebug()<<"WellFrameChooser::setNameLabelWidth START | WELLID = "<<_wellID<<" | SUGGESTED WIDTH "<<w<<" | NEW WIDTH = "<<_nameLabel->size().width()<<"\n";
#endif
}

void WellChooserFrame::setName(const QString & name)
{
   _nameLabel->setText(name);
   _name = name;
   checkNameLabelWidth(_chooser->maxNameWidth());
}

void WellChooserFrame::reduceIDbyOne()
{
   _wellID = _wellID - 1;
#ifdef QDEBUG_ALL
   qDebug()<<"WellFrameChooser::reduceIDbyOne | wellID = "<<_wellID+1<<" -> "<<_wellID<<"\n";
#endif
}

QComboBox * WellChooserFrame::constructLineStyle()
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellFrameChooser::constructLineStyle START\n";
#endif
   QComboBox * comboBox = new QComboBox;
   comboBox->setEditable(true);
   comboBox->lineEdit()->setReadOnly(true);
   comboBox->lineEdit()->setAlignment(Qt::AlignCenter);  
   comboBox->addItem(" - Line -");
   qobject_cast<QStandardItemModel *>(comboBox->model())->item(0)->setSelectable(false);
   comboBox->addItem("solid");
   comboBox->addItem("dash");
   comboBox->addItem("dot");
   comboBox->addItem("dash-dot");
   comboBox->addItem("dash-dot-dot");
   for(int i = 0; i < comboBox->count(); i++)
    comboBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
   return comboBox;
#ifdef QDEBUG_ALL
   qDebug()<<"WellFrameChooser::constructLineStyle END\n";
#endif
}

// QComboBox * WellChooserFrame::constructLineColor()
// {
// #ifdef QDEBUG_ALL
//    qDebug()<<"WellFrameChooser::constructLineColor START\n";
// #endif
//    QComboBox * comboBox = new QComboBox;
//    comboBox->setEditable(true);
//    comboBox->lineEdit()->setReadOnly(true);
//    comboBox->lineEdit()->setAlignment(Qt::AlignCenter);  
//    comboBox->addItem(" - Color -");
//    qobject_cast<QStandardItemModel *>(comboBox->model())->item(0)->setSelectable(false);
//    const int noOfColors = _chooser->colors().size();
//    for (int i=0;i<noOfColors;i++) comboBox->addItem(_chooser->colors()[i].first,"");
//    for(int i = 0; i < comboBox->count(); i++)
//     comboBox->setItemData(i, Qt::AlignRight, Qt::TextAlignmentRole);
//    return comboBox;
// #ifdef QDEBUG_ALL
//    qDebug()<<"WellFrameChooser::constructLineColor END\n";
// #endif
// }

QComboBox * WellChooserFrame::constructLineColor()
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellFrameChooser::constructLineColor START\n";
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
   qDebug()<<"WellFrameChooser::constructLineColor END\n";
#endif
}

// QComboBox * WellChooserFrame::constructLineColor()
// {
// #ifdef QDEBUG_ALL
//    qDebug()<<"WellFrameChooser::constructLineColor START\n";
// #endif
//    QComboBox * comboBox = new QComboBox;
//    comboBox->setEditable(true);
//    comboBox->lineEdit()->setReadOnly(true);
//    comboBox->lineEdit()->setAlignment(Qt::AlignCenter);  
//    comboBox->addItem(" - Color -");
//    qobject_cast<QStandardItemModel *>(comboBox->model())->item(0)->setSelectable(false);
//    const int noOfColors = _chooser->colors().size();
//    for (int i=1;i<noOfColors;i++) comboBox->addItem(_chooser->colors()[i].first);
//    QImage img(30,8,QImage::Format_RGB32);
//    QPainter p(&img);
//    QRect rect = img.rect().adjusted(-1,-1,1,1);
//    for(int i = 1; i < comboBox->count(); i++)
//    {
//       p.fillRect(rect,_chooser->colors()[i].second);
//       comboBox->setItemData(i, QPixmap::fromImage(img), Qt::DecorationRole);
//       comboBox->setItemData(i, Qt::AlignLeft, Qt::TextAlignmentRole);
//    }
//    return comboBox;
// #ifdef QDEBUG_ALL
//    qDebug()<<"WellFrameChooser::constructLineColor END\n";
// #endif
// }

void WellChooserFrame::constructSmTickType()
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellFrameChooser::constructSmTickType START\n";
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
   qDebug()<<"WellFrameChooser::constructSmTickType END\n";
#endif
}

void WellChooserFrame::setConnections()
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellFrameChooser::setConnections START\n";
#endif
   // QCheckBox:
   connect(_checkBox,SIGNAL(stateChanged(int)),
           this,SLOT(switchboard(int)));
   connect(_lossCheckBox,SIGNAL(stateChanged(int)),
           this,SLOT(switchboard(int)));
   connect(_noLossCheckBox,SIGNAL(stateChanged(int)),
           this,SLOT(switchboard(int)));
   connect(_smCheckBox,SIGNAL(stateChanged(int)),
           this,SLOT(switchboard(int)));
   // Line style, color and tick type:
   connect(_lossLineStyle,SIGNAL(activated(int)),
           this,SLOT(setLossLineStyle(int)));
   connect(_lossLineColor,SIGNAL(activated(int)),
           this,SLOT(setLossLineColor(int)));
   connect(_noLossLineStyle,SIGNAL(activated(int)),
           this,SLOT(setNoLossLineStyle(int)));
   connect(_noLossLineColor,SIGNAL(activated(int)),
           this,SLOT(setNoLossLineColor(int)));
   connect(_smTickType,SIGNAL(activated(int)),
           this,SLOT(setSmTickType(int)));      
#ifdef QDEBUG_ALL
   qDebug()<<"WellFrameChooser::setConnections END\n";
#endif   
}

void WellChooserFrame::initialize()
{
   if(_data->getModel().sizePlotChWell()>0)
   {
      _lossCheckBox->setChecked(_data->getModel().plotChWell(_wellID).withLossChecked);
      //emit _lossCheckBox->stateChanged(_lossCheckBox->checkState());
      _noLossCheckBox->setChecked(_data->getModel().plotChWell(_wellID).noLossChecked);
     // emit _noLossCheckBox->stateChanged(_noLossCheckBox->checkState());


      if(_data->getModel().plotChWell(_wellID).wellChecked)
      {
         _checkBox->setChecked(true);
         //emit _checkBox->stateChanged(_checkBox->checkState());
         _lossCheckBox->setChecked(_data->getModel().plotChWell(_wellID).withLossChecked);
         //emit _lossCheckBox->stateChanged(_lossCheckBox->checkState());
      }else
         _checkBox->setChecked(false);


      _lossLineStyle->setCurrentIndex(_data->getModel().plotChWell(_wellID).withLossLine);    
      emit _lossLineStyle->activated(_data->getModel().plotChWell(_wellID).withLossLine);
      _lossLineColor->setCurrentIndex(_data->getModel().plotChWell(_wellID).withLossColor);
      emit _lossLineColor->activated(_data->getModel().plotChWell(_wellID).withLossColor);
      _noLossLineStyle->setCurrentIndex(_data->getModel().plotChWell(_wellID).noLossLine);     
      emit _noLossLineStyle->activated(_data->getModel().plotChWell(_wellID).noLossLine);
      _noLossLineColor->setCurrentIndex(_data->getModel().plotChWell(_wellID).noLossColor);
      emit _noLossLineColor->activated(_data->getModel().plotChWell(_wellID).noLossColor);

      _smCheckBox->setChecked(_data->getModel().plotChWell(_wellID).smChecked);
      if(_data->getModel().plotChWell(_wellID).smChecked)
      {
         //constructSmTickType();
         //emit _smCheckBox->stateChanged(_smCheckBox->checkState());
         _smTickType->setCurrentIndex(_data->getModel().plotChWell(_wellID).smType);
         emit _smTickType->activated(_data->getModel().plotChWell(_wellID).smType);
      }
   }else
   {
      _checkBox->setChecked(false);
   }
}

void WellChooserFrame::switchboard(const int state)
{
   const bool checked = bool(state);
   QCheckBox * checkBox = static_cast<QCheckBox*>(sender());
   int id;
   if (checkBox == _checkBox) id = 0;
   else if (checkBox == _lossCheckBox) id = 1;
   else if (checkBox == _noLossCheckBox) id = 2;
   else if (checkBox == _smCheckBox) id = 3;
   else {qDebug()<<"WellChooserFrame::ERR01\n";exit(-1);}
   switch (id)
   {
      case 0: // _checkBox
         {
            if (checked) 
            {
               _chooser->calculateWell(_wellID);
               if (_lossCheckBox->isChecked()){ 
                  if(_lossLineColor->currentIndex()==0)
                     _lossLineColor->setCurrentIndex(1);
                  if(_lossLineStyle->currentIndex()==0)
                     _lossLineStyle->setCurrentIndex(1);
                  _chooser->addPlot(itemFlag, _wellID, 
                        _lossLineColor->currentIndex()-1, 
                        _lossLineStyle->currentIndex(),_name);
               }else 
                  _lossCheckBox->setChecked(true);
               if (_noLossCheckBox->isChecked())
               {
                  if(_noLossLineColor->currentIndex()==0)
                     _noLossLineColor->setCurrentIndex(1);
                  if(_noLossLineStyle->currentIndex()==0)
                     _noLossLineStyle->setCurrentIndex(1);
                  _chooser->addNoLossPlot(_wellID,
                        _noLossLineColor->currentIndex()-1, 
                        _noLossLineStyle->currentIndex(),_name);
               }
               if (_smCheckBox->isChecked())
               {
                  if(_lossLineColor->currentIndex()==0)
                     _lossLineColor->setCurrentIndex(1);
                  if(_smTickType->currentIndex()==0)
                     _lossLineColor->setCurrentIndex(1);
                  _chooser->addSmPlot(itemFlag,_wellID, 
                        _lossLineColor->currentIndex(),
                        _smTickType->currentIndex(), _name);
               }
            }else
            {
               if ((_lossCheckBox->isChecked())||(_noLossCheckBox->isChecked()))
                  _chooser->clearWell(_wellID);
               if (_lossCheckBox->isChecked()) 
               {
                  _chooser->removePlot(itemFlag,_wellID);
                  _lossCheckBox->setChecked(false);
               }
               if (_noLossCheckBox->isChecked()) 
               {
                  _chooser->removePlot(itemFlag,_wellID,false,true);
                  _noLossCheckBox->setChecked(false);
               }
               if (_smCheckBox->isChecked()) 
               {
                  _chooser->removePlot(itemFlag,_wellID,true,false);
                  _smCheckBox->setChecked(false);
               }
            }
            break;
         }
            case 1: // _lossCheckBox
            {
               if (checked) 
               {
                  if (_checkBox->isChecked())
                  {
                     if (!_noLossCheckBox->isChecked()) 
                        _chooser->calculateWell(_wellID);
                     if(_lossLineStyle->currentIndex()==0)
                        _lossLineStyle->setCurrentIndex(1);
                     if(_lossLineColor->currentIndex()==0)
                        _lossLineColor->setCurrentIndex(1);
                     _chooser->addPlot(itemFlag,_wellID, 
                           _lossLineColor->currentIndex()-1,
                           _lossLineStyle->currentIndex(),_name);
                  }
               }
               else
               {
                  if (_checkBox->isChecked())
                  {
                     _chooser->removePlot(itemFlag,_wellID);
                     if (!_noLossCheckBox->isChecked()) 
                     {
                        _checkBox->setChecked(false);
                        _chooser->clearWell(_wellID);
                     }
                  }
               }
               break;
            }
            case 2: // _noLossCheckBox
            {
               if (checked)
               {
                  if (_checkBox->isChecked())
                  {
                     if (!_lossCheckBox->isChecked()) 
                        _chooser->calculateWell(_wellID);
                     if(_noLossLineStyle->currentIndex()==0)
                        _noLossLineStyle->setCurrentIndex(1);
                     if(_noLossLineColor->currentIndex()==0)
                        _noLossLineColor->setCurrentIndex(1);
                     _chooser->addNoLossPlot(_wellID,
                           _noLossLineColor->currentIndex()-1,
                           _noLossLineStyle->currentIndex(),_name);
                  }
               }
               else
               {
                  if (_checkBox->isChecked())
                  {
                     _chooser->removePlot(itemFlag,_wellID,false,true);
                     if (!_lossCheckBox->isChecked()) 
                     {
                        _checkBox->setChecked(false);
                        _chooser->clearWell(_wellID);
                     }
                  }
               }
               break;
            }
            case 3: // _smCheckBox
            {
               if (checked)
               {
                  if (_checkBox->isChecked()) 
                  {
                     if(_lossLineColor->currentIndex()==0)
                        _lossLineColor->setCurrentIndex(1);
                     if(_smTickType->currentIndex()==0)
                        _smTickType->setCurrentIndex(1);
                     _chooser->addSmPlot(itemFlag,_wellID,
                           _lossLineColor->currentIndex()-1,
                           _smTickType->currentIndex(),_name);
                  }
                  _smTickType->show();
               }
               else
               {
                  if (_checkBox->isChecked()) 
                     _chooser->removePlot(itemFlag,_wellID,true,false);
                  _smTickType->hide();
               }
               break;
            }
            default:
            {qDebug()<<"WellChooserFrame::ERR02\n";exit(-1);}
         }
}

void WellChooserFrame::setLossLineStyle(const int i)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellFrameChooser::setLossLineStyle | LINE STYLE CHOSEN = "<<_lossLineStyle->itemText(_styleID1)<<"\n";
#endif   
   if ((_checkBox->isChecked()) && (_lossCheckBox->isChecked()))
   {
      if(_lossLineColor->currentIndex()==0)
         _lossLineColor->setCurrentIndex(1);
      _chooser->setLineProperties(itemFlag,_wellID,
            _lossLineColor->currentIndex()-1, i);
   }
}

void WellChooserFrame::setLossLineColor(const int i)
{
   if ((_checkBox->isChecked()) && (_lossCheckBox->isChecked()))
   {
      if(_lossLineStyle->currentIndex()==0)
         _lossLineStyle->setCurrentIndex(1);
      _chooser->setLineProperties(itemFlag,_wellID, i-1,_lossLineStyle->currentIndex());
      _chooser->setSmTickType(itemFlag,_wellID,_smTickType->currentIndex(), 
            i-1);
   }
}

void WellChooserFrame::setNoLossLineStyle(const int i)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellFrameChooser::setNoLossLineStyle | LINE STYLE CHOSEN = "<<_noLossLineStyle->itemText(_styleID2)<<"\n";
#endif   
   if ((_checkBox->isChecked()) && (_noLossCheckBox->isChecked()))
   {
      if(_noLossLineColor->currentIndex()==0)
         _noLossLineColor->setCurrentIndex(1);

      _chooser->setNoLossLineProperties(_wellID, 
          _noLossLineColor->currentIndex()-1, i);
   }
}

void WellChooserFrame::setNoLossLineColor(const int i)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellFrameChooser::setNoLossLineColor | (NO LOSS) COLOR CHOSEN = "<<_chooser->colors()[_colorID2].first<<" | RGB = "<<_chooser->colors()[_colorID2].second.toRgb()<<"\n";
#endif   
   if ((_checkBox->isChecked()) && (_noLossCheckBox->isChecked()))
   {
      if(_noLossLineStyle->currentIndex()==0)
         _noLossLineStyle->setCurrentIndex(1);
      _chooser->setNoLossLineProperties(_wellID, 
          i-1, _noLossLineStyle->currentIndex());
   }
}

void WellChooserFrame::setSmTickType(const int i)
{
   if (_checkBox->isChecked()) 
   {
      if(_lossLineColor->currentIndex()==0)
         _lossLineColor->setCurrentIndex(1);
      _chooser->setSmTickType(itemFlag,_wellID, i,
            _lossLineColor->currentIndex()-1);
   }
}
