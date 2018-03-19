#include "imExFrame.h"

#include <QVBoxLayout>
#include <QPushButton>

ImportExportFrame::ImportExportFrame() : QFrame()
{
   QVBoxLayout * imExLayout = new QVBoxLayout;
   imExLayout->setAlignment(Qt::AlignLeft);
   imExLayout->setContentsMargins(0,0,0,0);
   _importButton = new QPushButton(tr("Import"));
   _exportButton = new QPushButton(tr("Export"));
   imExLayout->addWidget(_importButton);
   imExLayout->addWidget(_exportButton);
   imExLayout->addStretch();
   setLayout(imExLayout);
}