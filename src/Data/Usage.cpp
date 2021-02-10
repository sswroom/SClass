#include "Stdafx.h"
#include "Data/Usage.h"

Data::Usage::Usage(Data::Usable *usable)
{
	this->usable = usable;
	if (this->usable)
	{
		this->usable->BeginUse();
	}
}

Data::Usage::~Usage()
{
	this->EndUse();
}

void Data::Usage::EndUse()
{
	if (this->usable)
	{
		this->usable->EndUse();
		this->usable = 0;
	}
}