#include "Stdafx.h"
#include "DotNet/DNColorManager.h"

DotNet::DNColorManager::DNColorManager(Media::ColorManager *colorMgr)
{
	this->colorMgr = colorMgr;
	this->hdlrs = new System::Collections::ArrayList();
}

DotNet::DNColorManager::~DNColorManager()
{
	this->hdlrs->Clear();
	this->hdlrs = 0;
}

void DotNet::DNColorManager::AddHandler(DNColorHandler *hdlr)
{
	this->hdlrs->Add(hdlr);
}

void DotNet::DNColorManager::RemoveHandler(DNColorHandler *hdlr)
{
	this->hdlrs->Remove(hdlr);
}

Media::ColorManager *DotNet::DNColorManager::GetColorMgr()
{
	return this->colorMgr;
}

void DotNet::DNColorManager::RGBUpdated()
{
	const Media::IColorHandler::RGBPARAM *rgb = this->colorMgr->GetRGBParam();
	System::Int32 i = this->hdlrs->Count;
	while (i-- > 0)
	{
		static_cast<DNColorHandler*>(this->hdlrs->Item[i])->RGBParamChanged(rgb);
	}
}

void DotNet::DNColorManager::YUVUpdated()
{
	NN<const Media::IColorHandler::YUVPARAM> yuv = this->colorMgr->GetYUVParam();
	System::Int32 i = this->hdlrs->Count;
	while (i-- > 0)
	{
		static_cast<DNColorHandler*>(this->hdlrs->Item[i])->YUVParamChanged(yuv);
	}
}
