#include "Stdafx.h"
#include "Media/Jasper/JasperReportElement.h"

Media::Jasper::JasperReportElement::JasperReportElement()
{
	this->x = 0;
	this->y = 0;
	this->width = 0;
	this->height = 0;
	this->uuid = 0;
}

Media::Jasper::JasperReportElement::~JasperReportElement()
{
	SDEL_STRING(this->uuid);
}

void Media::Jasper::JasperReportElement::SetX(UOSInt x)
{
	this->x = x;
}

void Media::Jasper::JasperReportElement::SetY(UOSInt y)
{
	this->y = y;
}

void Media::Jasper::JasperReportElement::SetWidth(UOSInt width)
{
	this->width = width;
}

void Media::Jasper::JasperReportElement::SetHeight(UOSInt height)
{
	this->height = height;
}

void Media::Jasper::JasperReportElement::SetUUID(Text::String *uuid)
{
	SDEL_STRING(this->uuid);
	this->uuid = SCOPY_STRING(uuid);
}

UOSInt Media::Jasper::JasperReportElement::GetX() const
{
	return this->x;
}

UOSInt Media::Jasper::JasperReportElement::GetY() const
{
	return this->y;
}

UOSInt Media::Jasper::JasperReportElement::GetWidth() const
{
	return this->width;
}

UOSInt Media::Jasper::JasperReportElement::GetHeight() const
{
	return this->height;
}

Text::String *Media::Jasper::JasperReportElement::GetUUID() const
{
	return this->uuid;
}
