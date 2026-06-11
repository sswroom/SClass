#include "Stdafx.h"
#include "Media/Jasper/JasperReportElement.h"

Media::Jasper::JasperReportElement::JasperReportElement()
{
	this->x = 0;
	this->y = 0;
	this->width = 0;
	this->height = 0;
	this->uuid = nullptr;
}

Media::Jasper::JasperReportElement::~JasperReportElement()
{
	OPTSTR_DEL(this->uuid);
}

void Media::Jasper::JasperReportElement::SetX(UIntOS x)
{
	this->x = x;
}

void Media::Jasper::JasperReportElement::SetY(UIntOS y)
{
	this->y = y;
}

void Media::Jasper::JasperReportElement::SetWidth(UIntOS width)
{
	this->width = width;
}

void Media::Jasper::JasperReportElement::SetHeight(UIntOS height)
{
	this->height = height;
}

void Media::Jasper::JasperReportElement::SetUUID(Optional<Text::String> uuid)
{
	OPTSTR_DEL(this->uuid);
	this->uuid = Text::String::CopyOrNull(uuid);
}

UIntOS Media::Jasper::JasperReportElement::GetX() const
{
	return this->x;
}

UIntOS Media::Jasper::JasperReportElement::GetY() const
{
	return this->y;
}

UIntOS Media::Jasper::JasperReportElement::GetWidth() const
{
	return this->width;
}

UIntOS Media::Jasper::JasperReportElement::GetHeight() const
{
	return this->height;
}

Optional<Text::String> Media::Jasper::JasperReportElement::GetUUID() const
{
	return this->uuid;
}
