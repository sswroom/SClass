#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/Jasper/JasperElement.h"

Media::Jasper::JasperElement::JasperElement()
{
	this->reportElement = 0;
}

Media::Jasper::JasperElement::~JasperElement()
{
	SDEL_CLASS(this->reportElement);
}

void Media::Jasper::JasperElement::SetReportElement(JasperReportElement *reportElement)
{
	SDEL_CLASS(this->reportElement);
	this->reportElement = reportElement;
}

Media::Jasper::JasperReportElement *Media::Jasper::JasperElement::GetReportElement() const
{
	return this->reportElement;
}
