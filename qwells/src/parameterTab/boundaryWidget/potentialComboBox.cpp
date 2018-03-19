#include "potentialComboBox.h"

#include <QComboBox>
#include <QString>
#include <QVariant>
#include <QStandardItemModel>
#include <QLineEdit>

#include <QDebug>

PotentialComboBox::PotentialComboBox(const int id) : QComboBox(), _potentialID(id)
{
//    addItem(" -- Choose -- "); // TODO uncomment below if this is activated
   addItem(QString(" Constant potential"));
   addItem(QString(" Impermeable"));
   connect(this, SIGNAL(activated(int)),this,SLOT(sendWithID(int)));
   
   setEditable(true);
   // Second : Put the lineEdit in read-only mode
   lineEdit()->setReadOnly(true);
   // Third  : Align the lineEdit to center
   lineEdit()->setAlignment(Qt::AlignCenter);  
   // Fourth : Align each item in the combo to the center
   for(int i = 0; i < count(); i++)
    setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
//    qobject_cast<QStandardItemModel *>(model())->item(0)->setSelectable( false );
   // TODO uncomment this line if first line is uncommented
}

void PotentialComboBox::sendWithID(const int itemID)
{
   emit activatedWithID(itemID, _potentialID);
}