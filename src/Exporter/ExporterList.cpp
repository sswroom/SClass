#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/BMPExporter.h"
#include "Exporter/CIPExporter.h"
#include "Exporter/CURExporter.h"
#include "Exporter/DBCSVExporter.h"
#include "Exporter/DBExcelXMLExporter.h"
#include "Exporter/DBFExporter.h"
#include "Exporter/DBHTMLExporter.h"
#include "Exporter/DBPListExporter.h"
#include "Exporter/DocHTMLExporter.h"
#include "Exporter/ExcelXMLExporter.h"
#include "Exporter/ExporterList.h"
#include "Exporter/GIFExporter.h"
#include "Exporter/GUIGIFExporter.h"
#include "Exporter/GUIJPGExporter.h"
#include "Exporter/GUIPNGExporter.h"
#include "Exporter/GUITIFExporter.h"
#include "Exporter/GPXExporter.h"
#include "Exporter/ICOExporter.h"
#include "Exporter/KMLExporter.h"
#include "Exporter/MapCSVExporter.h"
#include "Exporter/MD4Exporter.h"
#include "Exporter/MD5Exporter.h"
#include "Exporter/MDBExporter.h"
#include "Exporter/MEVExporter.h"
#include "Exporter/OruxMapExporter.h"
#include "Exporter/PEMExporter.h"
#include "Exporter/PLTExporter.h"
#include "Exporter/PNGExporter.h"
#include "Exporter/SFVExporter.h"
#include "Exporter/SHA1Exporter.h"
#include "Exporter/SHPExporter.h"
#include "Exporter/SPKExporter.h"
#include "Exporter/SQLiteExporter.h"
#include "Exporter/TIFFExporter.h"
#include "Exporter/WAVExporter.h"
#include "Exporter/ZIPExporter.h"

Exporter::ExporterList::ExporterList()
{
	IO::FileExporter *exporter;
	NEW_CLASS(this->exporters, Data::ArrayList<IO::FileExporter*>());

	NEW_CLASS(exporter, Exporter::MapCSVExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::KMLExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::GPXExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::CIPExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::MEVExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::SHPExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::PLTExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::SPKExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::OruxMapExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::DocHTMLExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::ZIPExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::PEMExporter());
	this->exporters->Add(exporter);

	NEW_CLASS(exporter, Exporter::DBCSVExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::DBHTMLExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::DBExcelXMLExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::DBPListExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::ExcelXMLExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::MDBExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::SQLiteExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::DBFExporter());
	this->exporters->Add(exporter);

	NEW_CLASS(exporter, Exporter::TIFFExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::BMPExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::GIFExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::PNGExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::CURExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::ICOExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::WAVExporter());
	this->exporters->Add(exporter);

	NEW_CLASS(exporter, Exporter::MD5Exporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::MD4Exporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::SHA1Exporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::SFVExporter());
	this->exporters->Add(exporter);

	NEW_CLASS(exporter, Exporter::GUIJPGExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::GUIGIFExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::GUIPNGExporter());
	this->exporters->Add(exporter);
	NEW_CLASS(exporter, Exporter::GUITIFExporter());
	this->exporters->Add(exporter);
}

Exporter::ExporterList::~ExporterList()
{
	UOSInt i = this->exporters->GetCount();
	IO::FileExporter *exporter;
	while (i-- > 0)
	{
		exporter = this->exporters->GetItem(i);
		DEL_CLASS(exporter);
	}
	DEL_CLASS(this->exporters);
}

void Exporter::ExporterList::SetCodePage(UInt32 codePage)
{
	UOSInt i = this->exporters->GetCount();
	IO::FileExporter *exporter;
	while (i-- > 0)
	{
		exporter = this->exporters->GetItem(i);
		exporter->SetCodePage(codePage);
	}
}

void Exporter::ExporterList::SetEncFactory(Text::EncodingFactory *encFact)
{
	UOSInt i = this->exporters->GetCount();
	IO::FileExporter *exporter;
	while (i-- > 0)
	{
		exporter = this->exporters->GetItem(i);
		exporter->SetEncFactory(encFact);
	}
}

UOSInt Exporter::ExporterList::GetSupportedExporters(Data::ArrayList<IO::FileExporter*> *exporters, IO::ParsedObject *obj)
{
	UOSInt cnt = 0;
	UOSInt i = 0;
	UOSInt j = this->exporters->GetCount();
	IO::FileExporter *exporter;
	while (i < j)
	{
		exporter = this->exporters->GetItem(i);
		if (exporter->IsObjectSupported(obj) != IO::FileExporter::SupportType::NotSupported)
		{
			exporters->Add(exporter);
			cnt++;
		}
		i++;
	}
	return cnt;
}
