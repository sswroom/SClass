#include "Stdafx.h"
#include "Media/Jasper/JasperBand.h"

Media::Jasper::JasperBand::JasperBand()
{
	this->height = INVALID_INDEX;
	this->splitType = 0;
}

Media::Jasper::JasperBand::~JasperBand()
{
	SDEL_STRING(this->splitType);
	JasperElement *ele;
	UOSInt i = this->elements.GetCount();
	while (i-- > 0)
	{
		ele = this->elements.GetItem(i);
		DEL_CLASS(ele);
	}
}

void Media::Jasper::JasperBand::SetSplitType(Text::String *splitType)
{
	SDEL_STRING(this->splitType);
	this->splitType = SCOPY_STRING(splitType);
}

void Media::Jasper::JasperBand::SetHeight(UOSInt height)
{
	this->height = height;
}

Text::String *Media::Jasper::JasperBand::GetSplitType() const
{
	return this->splitType;
}

Bool Media::Jasper::JasperBand::HasHeight() const
{
	return this->height == INVALID_INDEX;
}

UOSInt Media::Jasper::JasperBand::GetHeight() const
{
	return this->height;
}

void Media::Jasper::JasperBand::AddElement(JasperElement *element)
{
	this->elements.Add(element);
}

UOSInt Media::Jasper::JasperBand::GetCount() const
{
	return this->elements.GetCount();
}

Media::Jasper::JasperElement *Media::Jasper::JasperBand::GetElement(UOSInt index) const
{
	return this->elements.GetItem(index);
}
