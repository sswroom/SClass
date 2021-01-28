#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DShow/DShowFilter.h"
#include <windows.h>
#include <dshow.h>

Media::DShow::DShowFilter::DShowFilter(Media::DShow::DShowManager *mgr)
{
	this->mgr = mgr;
	this->filter = 0;
}

Media::DShow::DShowFilter::~DShowFilter()
{
	if (this->filter)
	{
		((IBaseFilter*)this->filter)->Release();
		this->filter = 0;
	}
}

void *Media::DShow::DShowFilter::GetFilter()
{
	return this->filter;
}
