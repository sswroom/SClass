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
#include "Exporter/HEIFExporter.h"
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
#include "Exporter/WebPExporter.h"
#include "Exporter/XLSXExporter.h"
#include "Exporter/ZIPExporter.h"

Exporter::ExporterList::ExporterList()
{
	NN<IO::FileExporter> exporter;

	NEW_CLASSNN(exporter, Exporter::MapCSVExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::KMLExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::GPXExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::CIPExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::MEVExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::SHPExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::PLTExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::SPKExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::OruxMapExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::DocHTMLExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::ZIPExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::PEMExporter());
	this->exporters.Add(exporter);

	NEW_CLASSNN(exporter, Exporter::DBCSVExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::DBHTMLExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::DBExcelXMLExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::DBPListExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::ExcelXMLExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::MDBExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::SQLiteExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::DBFExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::XLSXExporter());
	this->exporters.Add(exporter);

	NEW_CLASSNN(exporter, Exporter::TIFFExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::BMPExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::GIFExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::PNGExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::CURExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::ICOExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::WAVExporter());
	this->exporters.Add(exporter);

	NEW_CLASSNN(exporter, Exporter::MD5Exporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::MD4Exporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::SHA1Exporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::SFVExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::WebPExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::HEIFExporter());
	this->exporters.Add(exporter);

	NEW_CLASSNN(exporter, Exporter::GUIJPGExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::GUIGIFExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::GUIPNGExporter());
	this->exporters.Add(exporter);
	NEW_CLASSNN(exporter, Exporter::GUITIFExporter());
	this->exporters.Add(exporter);
}

Exporter::ExporterList::~ExporterList()
{
	this->exporters.DeleteAll();
}

void Exporter::ExporterList::SetCodePage(UInt32 codePage)
{
	UOSInt i = this->exporters.GetCount();
	NN<IO::FileExporter> exporter;
	while (i-- > 0)
	{
		exporter = this->exporters.GetItemNoCheck(i);
		exporter->SetCodePage(codePage);
	}
}

void Exporter::ExporterList::SetEncFactory(Optional<Text::EncodingFactory> encFact)
{
	UOSInt i = this->exporters.GetCount();
	NN<IO::FileExporter> exporter;
	while (i-- > 0)
	{
		exporter = this->exporters.GetItemNoCheck(i);
		exporter->SetEncFactory(encFact);
	}
}

UOSInt Exporter::ExporterList::GetSupportedExporters(NN<Data::ArrayListNN<IO::FileExporter>> exporters, NN<IO::ParsedObject> obj)
{
	UOSInt cnt = 0;
	UOSInt i = 0;
	UOSInt j = this->exporters.GetCount();
	NN<IO::FileExporter> exporter;
	while (i < j)
	{
		exporter = this->exporters.GetItemNoCheck(i);
		if (exporter->IsObjectSupported(obj) != IO::FileExporter::SupportType::NotSupported)
		{
			exporters->Add(exporter);
			cnt++;
		}
		i++;
	}
	return cnt;
}
