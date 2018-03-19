#include "isoList.h"

#include <QKeyEvent>
#include <QDebug>

IsoList::IsoList()
{
   connect(this, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(listItemClick(QListWidgetItem *)));

}

void IsoList::keyPressEvent(QKeyEvent * event)
{
   if(event->key() == Qt::Key_Delete)
   {
      //qDebug()<<" kliknuo na delete "<<_clicked<<"\n";
      delete this->takeItem(_clicked);
      emit listChanged();
   }
   //qDebug()<<" key event \n";
}

void IsoList::listItemClick(QListWidgetItem * item)
{
   _clicked=this->row(item);
  // this->openPersistentEditor(item);
  /* for(int i=0;i<this->count();i++)
   {
      if(i!=_clicked)
         this->closePersistentEditor(this->item(i));
   }*/
   //qDebug()<<" kliknuo "<<item->text()<<"\n";
}

