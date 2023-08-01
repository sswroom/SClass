#include "Stdafx.h"
#include "Text/MyString.h"
#include "IO/ParsedObject.h"

Text::CString IO::ParserTypeGetName(ParserType pt)
{
	switch (pt)
	{
	case ParserType::Stream:
		return CSTR("Stream");
	case ParserType::MediaFile:
		return CSTR("MediaFile");
	case ParserType::PackageFile:
		return CSTR("PackageFile");
	case ParserType::ImageList:
		return CSTR("ImageList");
	case ParserType::EXEFile:
		return CSTR("EXEFile");
	case ParserType::MapLayer:
		return CSTR("MapLayer");
	case ParserType::ReadingDB:
		return CSTR("ReadingDB");
	case ParserType::MapEnv:
		return CSTR("MapEnv");
	case ParserType::FileCheck:
		return CSTR("FileCheck");
	case ParserType::TextDocument:
		return CSTR("TextDocument");
	case ParserType::Workbook:
		return CSTR("Workbook");
	case ParserType::SectorData:
		return CSTR("SectorData");
	case ParserType::LogFile:
		return CSTR("LogFile");
	case ParserType::Playlist:
		return CSTR("Playlist");
	case ParserType::CoordinateSystem:
		return CSTR("CoordinateSystem");
	case ParserType::CodeProject:
		return CSTR("CodeProject");
	case ParserType::VectorDocument:
		return CSTR("VectorDocument");
	case ParserType::LUT:
		return CSTR("LUT");
	case ParserType::FontRenderer:
		return CSTR("FontRenderer");
	case ParserType::MIMEObject:
		return CSTR("MIMEObject");
	case ParserType::EthernetAnalyzer:
		return CSTR("EthernetAnalyzer");
	case ParserType::JavaClass:
		return CSTR("JavaClass");
	case ParserType::Smake:
		return CSTR("Smake");
	case ParserType::ASN1Data:
		return CSTR("ASN1Data");
	case ParserType::BTScanLog:
		return CSTR("BTScanLog");
	case ParserType::SystemInfoLog:
		return CSTR("SystemInfoLog");
	case ParserType::JasperReport:
		return CSTR("JasperReport");
	case ParserType::PDFDocument:
		return CSTR("PDFDocument");
	case ParserType::Unknown:
	default:
		return CSTR("Unknwon");
	}
}

IO::ParsedObject::ParsedObject(NotNullPtr<Text::String> sourceName)
{
	this->sourceName = sourceName->Clone();
}

IO::ParsedObject::ParsedObject(const Text::CString &sourceName)
{
	this->sourceName = Text::String::New(sourceName);
}

IO::ParsedObject::~ParsedObject()
{
	this->sourceName->Release();
}

UTF8Char *IO::ParsedObject::GetSourceName(UTF8Char *oriStr) const
{
	return this->sourceName->ConcatTo(oriStr);
}

void IO::ParsedObject::SetSourceName(NotNullPtr<Text::String> sourceName)
{
	this->sourceName->Release();
	this->sourceName = sourceName->Clone();
}

void IO::ParsedObject::SetSourceName(Text::CString sourceName)
{
	this->sourceName->Release();
	this->sourceName = Text::String::New(sourceName);
}
