#include "tabToolBase.h"
#include "collapsingBar.h"

#include <QDebug>
#include <QFrame>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>

TabToolBase::TabToolBase(QString title) : QFrame()
{   
   _layout = new QVBoxLayout;
   _layout->setAlignment(Qt::AlignTop);
   _titleBar = new CollapsingBar(title);
   _titleBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
   setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
   _layout->addWidget(_titleBar);
   //_layout->setContentsMargins(1,1,1,1);
   _layout->setStretchFactor(_titleBar,0);
   setLayout(_layout);
   //setFrameShape(QFrame::NoFrame);
}

void TabToolBase::appendToTitle(QString string)
{
   const QString newTitle = _titleBar->title().append(string);
   _titleBar->barLabel()->setText(newTitle);
}