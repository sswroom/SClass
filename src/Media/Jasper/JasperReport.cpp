#include "Stdafx.h"
#include "Math/Unit/Distance.h"
#include "Media/PaperSize.h"
#include "Media/Jasper/JasperReport.h"

void Media::Jasper::JasperReport::InitValues()
{
	this->queryString = 0;
	this->title = 0;
	this->uuid = 0;
	this->reportName = 0;
	Media::PaperSize paperSize(Media::PaperSize::PT_A4);
	Double pWidth = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_PIXEL, paperSize.GetWidthMM());
	Double pHeight = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_PIXEL, paperSize.GetHeightMM());
	this->pageWidth = (UInt32)Double2Int32(pWidth);
	this->pageHeight = (UInt32)Double2Int32(pHeight);
	this->marginLeft = 0;
	this->marginTop = 0;
	this->marginRight = 0;
	this->marginBottom = 0;
}

Media::Jasper::JasperReport::JasperReport(NN<Text::String> sourceName) : IO::ParsedObject(sourceName)
{
	this->InitValues();
}

Media::Jasper::JasperReport::JasperReport(Text::CStringNN sourceName) : IO::ParsedObject(sourceName)
{
	this->InitValues();
}

Media::Jasper::JasperReport::~JasperReport()
{
	OPTSTR_DEL(this->queryString);
	OPTSTR_DEL(this->uuid);
	OPTSTR_DEL(this->reportName);
	this->title.Delete();

	UOSInt i = this->properties.GetCount();
	while (i-- > 0)
	{
		NN<JasperProperty> property = this->properties.GetItemNoCheck(i);
		property->name->Release();
		property->value->Release();
		MemFreeNN(property);
	}
	i = this->importList.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->importList.GetItem(i));
	}
	i = this->params.GetCount();
	while (i-- > 0)
	{
		NN<JasperParameter> param = this->params.GetItemNoCheck(i);
		param->name->Release();
		param->className->Release();
		OPTSTR_DEL(param->defValueExp);
		MemFreeNN(param);
	}
	this->detailList.DeleteAll();
}

IO::ParserType Media::Jasper::JasperReport::GetParserType() const
{
	return IO::ParserType::JasperReport;
}

void Media::Jasper::JasperReport::SetReportName(Optional<Text::String> reportName)
{
	OPTSTR_DEL(this->reportName);
	this->reportName = Text::String::CopyOrNull(reportName);
}

void Media::Jasper::JasperReport::SetPageWidth(UInt32 pageWidth)
{
	this->pageWidth = pageWidth;
}

void Media::Jasper::JasperReport::SetPageHeight(UInt32 pageHeight)
{
	this->pageHeight = pageHeight;
}

void Media::Jasper::JasperReport::SetMarginLeft(UInt32 marginLeft)
{
	this->marginLeft = marginLeft;
}

void Media::Jasper::JasperReport::SetMarginTop(UInt32 marginTop)
{
	this->marginTop = marginTop;
}

void Media::Jasper::JasperReport::SetMarginRight(UInt32 marginRight)
{
	this->marginRight = marginRight;
}

void Media::Jasper::JasperReport::SetMarginBottom(UInt32 marginBottom)
{
	this->marginBottom = marginBottom;
}

void Media::Jasper::JasperReport::SetUUID(Optional<Text::String> uuid)
{
	OPTSTR_DEL(this->uuid);
	this->uuid = Text::String::CopyOrNull(uuid);
}

Optional<Text::String> Media::Jasper::JasperReport::GetReportName() const
{
	return this->reportName;
}

UInt32 Media::Jasper::JasperReport::GetPageWidth() const
{
	return this->pageWidth;
}

UInt32 Media::Jasper::JasperReport::GetPageHeight() const
{
	return this->pageHeight;
}

UInt32 Media::Jasper::JasperReport::GetMarginLeft() const
{
	return this->marginLeft;
}

UInt32 Media::Jasper::JasperReport::GetMarginTop() const
{
	return this->marginTop;
}

UInt32 Media::Jasper::JasperReport::GetMarginRight() const
{
	return this->marginRight;
}

UInt32 Media::Jasper::JasperReport::GetMarginBottom() const
{
	return this->marginBottom;
}

Optional<Text::String> Media::Jasper::JasperReport::GetUUID() const
{
	return this->uuid;
}

void Media::Jasper::JasperReport::SetQueryString(Optional<Text::String> str)
{
	OPTSTR_DEL(this->queryString);
	this->queryString = Text::String::CopyOrNull(str);
}

void Media::Jasper::JasperReport::SetProperty(NN<Text::String> name, NN<Text::String> value)
{
	NN<JasperProperty> property = MemAllocNN(JasperProperty);
	property->name = name->Clone();
	property->value = value->Clone();
	this->properties.Add(property);
}

void Media::Jasper::JasperReport::AddImport(NN<Text::String> value)
{
	this->importList.Add(value->Clone());
}

void Media::Jasper::JasperReport::AddParameter(NN<Text::String> name, NN<Text::String> className, Text::CString defValueExp)
{
	NN<JasperParameter> param = MemAllocNN(JasperParameter);
	param->name = name->Clone();
	param->className = className->Clone();
	param->defValueExp = Text::String::NewOrNull(defValueExp);
	this->params.Add(param);
}

void Media::Jasper::JasperReport::SetTitle(NN<Media::Jasper::JasperBand> band)
{
	this->title.Delete();
	this->title = band;
}

void Media::Jasper::JasperReport::AddDetail(NN<Media::Jasper::JasperBand> band)
{
	this->detailList.Add(band);
}
