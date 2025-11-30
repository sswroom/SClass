#include "Stdafx.h"
#include "Data/Usage.h"

Data::Usage::Usage(Optional<Data::Usable> usable)
{
	NN<Data::Usable> nnusable;
	this->usable = usable;
	if (this->usable.SetTo(nnusable))
	{
		nnusable->BeginUse();
	}
}

Data::Usage::~Usage()
{
	this->EndUse();
}

void Data::Usage::EndUse()
{
	NN<Data::Usable> nnusable;
	if (this->usable.SetTo(nnusable))
	{
		nnusable->EndUse();
		this->usable = 0;
	}
}