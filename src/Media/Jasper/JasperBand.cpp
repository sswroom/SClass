#include "Stdafx.h"
#include "Media/Jasper/JasperBand.h"

Media::Jasper::JasperBand::JasperBand()
{
	this->height = INVALID_INDEX;
	this->splitType = nullptr;
}

Media::Jasper::JasperBand::~JasperBand()
{
	OPTSTR_DEL(this->splitType);
	this->elements.DeleteAll();
}

void Media::Jasper::JasperBand::SetSplitType(Optional<Text::String> splitType)
{
	OPTSTR_DEL(this->splitType);
	this->splitType = Text::String::CopyOrNull(splitType);
}

void Media::Jasper::JasperBand::SetHeight(UOSInt height)
{
	this->height = height;
}

Optional<Text::String> Media::Jasper::JasperBand::GetSplitType() const
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

void Media::Jasper::JasperBand::AddElement(NN<JasperElement> element)
{
	this->elements.Add(element);
}

UOSInt Media::Jasper::JasperBand::GetCount() const
{
	return this->elements.GetCount();
}

Optional<Media::Jasper::JasperElement> Media::Jasper::JasperBand::GetElement(UOSInt index) const
{
	return this->elements.GetItem(index);
}
