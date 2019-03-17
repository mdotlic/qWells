#include "about.h"

#include "qWells.h"

#include <QDialog>
#include <QFrame>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>

About::About(QWells * qWells) : QDialog(qWells)
{
   setWindowTitle("About");
   
   QLabel * iconJCI = new QLabel;
   iconJCI->setPixmap(QIcon(":/resources/images/logoJCI.png").pixmap(65,93));
   
   QLabel * labelJCI = new QLabel("JCI");
   labelJCI->setStyleSheet("font: 18px bold;");
   
   QString textCerni = QString("Jaroslav ").append(QChar(0x010C)).append("erni Institute\nfor the Development of Water Resources");
   QLabel * labelCerni = new QLabel(textCerni);
   labelCerni->setWordWrap(true);
   //labelCerni->setAlignment(Qt::AlignCenter);
   labelCerni->setFixedWidth(235);
   
   // TEXT LAYOUT
   
   QFrame * aboutFrame = new QFrame;
   aboutFrame->setFrameShape(QFrame::StyledPanel);
   aboutFrame->setFrameShadow(QFrame::Sunken);
   QVBoxLayout * frameLayout = new QVBoxLayout;
   
   constructText();
   
   QLabel * aboutQWells = new QLabel("About qWells_1.3.29");
   aboutQWells->setStyleSheet("font:12px bold;");
   
   QLabel * textQWellsLabel = new QLabel(_textQWells);
   textQWellsLabel->setWordWrap(true);
   textQWellsLabel->setFixedWidth(300);

   QLabel * credits = new QLabel("Credits:");
   credits->setStyleSheet("font:12px bold;");
   
   QLabel * textCreditsLabel = new QLabel(_textCredits);
   textCreditsLabel->setWordWrap(true);
   textCreditsLabel->setFixedWidth(300);

   QLabel * copyrightLabel = new QLabel(_copyright);
   copyrightLabel->setWordWrap(true);
   copyrightLabel->setFixedWidth(300);
   
   //copyr->setStyleSheet("font:12px bold;");
   
   
   
   frameLayout->addWidget(aboutQWells);
   frameLayout->addWidget(textQWellsLabel);
   frameLayout->addWidget(credits);
   frameLayout->addWidget(textCreditsLabel);
   frameLayout->addStretch();
   frameLayout->addWidget(copyrightLabel);
   
   aboutFrame->setLayout(frameLayout);
   
   // MAIN LAYOUT
   
   QGridLayout * aboutLayout = new QGridLayout;
   aboutLayout->addWidget(iconJCI,0,0,3,1);
   //aboutLayout->addWidget(labelJCI,0,2);
   aboutLayout->addWidget(labelCerni,1,2);
   aboutLayout->setColumnStretch(1,1);
   
   aboutLayout->setRowStretch(2,2);
   aboutLayout->addWidget(aboutFrame,3,0,1,3);
   aboutLayout->setRowStretch(4,1);
   
   QPushButton * okButton = new QPushButton(QString("OK"));
   okButton->setMaximumWidth(80);
   aboutLayout->addWidget(okButton,5,0,1,3,Qt::AlignCenter);
   connect(okButton,SIGNAL(clicked()),this,SLOT(accept()));
   
   setLayout(aboutLayout);
   
   exec();
}

void About::constructText()
{
   _textQWells = QString("qWells is an easy to use sofware package for analyzing pumping test data. \nAll bugs and problems should be reported on the e-mail milan.dotlic@jcerni.co.rs."      
   );
   _textCredits=QString("Milan Dotlić, \nBoris Pokorni, \nMilenko Pušić, \nGoran Jevtić \nMilan Dimkić");
   _copyright = QString("Copyright "+QString(QChar(0x00A9))+" 2016 Jaroslav Černi Institute for the Development of Water Resources, Serbia.");
}
