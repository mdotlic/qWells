#include "license.h"
#if (defined(__MINGW32__) || defined(__MINGW64__))
#include <windows.h>
#include <QTextStream>
#include <QFile>
#include <QDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QMessageBox>

#include <QCoreApplication>

License::License(bool & active)
{
   QFile file(QCoreApplication::applicationDirPath()+"\\.licQ.txt");
  
   /*
   QDialog * greskaDialog = new QDialog;
   greskaDialog->setWindowTitle(" kaze da ima fajl "+ file.fileName()+" i to jer "+QString::number(file.exists())+" prvo slovo je "+file.fileName().at(0));
   //QVBoxLayout * layout = new QVBoxLayout;
   greskaDialog->exec();
*/
   DWORD disk_serialINT;
   DWORD dummy1=0;
   DWORD dummy2=0;
   QString ss = file.fileName()[0]+":\\";
   std::string str = ss.toStdString();
   GetVolumeInformation((WCHAR *)ss.utf16(), NULL, dummy1, &disk_serialINT, NULL, NULL, NULL, dummy2);
   //std::string HDDserial = std::to_string(disk_serialINT);
   int key=15072804;
   int pass=0;
   if (!file.exists())
   {
    //  int pass=0;
      QDialog * dialog = new QDialog;
      dialog->setWindowTitle("Enter license number");
      QGridLayout * layout = new QGridLayout;
      QLineEdit * lic = new QLineEdit(dialog);
      lic->setEchoMode(QLineEdit::Password);
      layout->addWidget(lic,0,0,1,1);
      QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,dialog);
      connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
      connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));
      layout->addWidget(buttonBox,1,0,1,3,Qt::AlignCenter);
      dialog->setLayout(layout);
      if(dialog->exec() == QDialog::Accepted)
      {
         pass=lic->text().toInt()+disk_serialINT;
      }
      if ( file.open(QIODevice::ReadWrite) )
      {
         QTextStream stream( &file );
         stream << pass << endl;
      }
   }
   if(file.exists())
   {//fajl postoji
      //int pass=0;
      if ( file.open(QIODevice::ReadWrite) )
      {
         QTextStream stream( &file );
         stream >> pass;
      }     
      if(pass-disk_serialINT != key)      
      {//wrong password
         QMessageBox * newError = new QMessageBox(QMessageBox::Icon::Warning,
                                            QString("ERROR"),
                                            QString("Wrong license number!!!"));
         newError->exec();  
         active=false;
      }
      else
         active=true;
   }
}
#endif
