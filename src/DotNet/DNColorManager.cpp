#include "Stdafx.h"
#include "DotNet/DNColorManager.h"

DotNet::DNColorManager::DNColorManager(NN<Media::ColorManager> colorMgr)
{
	this->colorMgr = colorMgr;
	this->hdlrs = gcnew System::Collections::ArrayList();
}

DotNet::DNColorManager::~DNColorManager()
{
	this->hdlrs->Clear();
	this->hdlrs = 0;
}

void DotNet::DNColorManager::AddHandler(NN<DNColorHandler> hdlr)
{
	this->hdlrs->Add(hdlr);
}

void DotNet::DNColorManager::RemoveHandler(NN<DNColorHandler> hdlr)
{
	this->hdlrs->Remove(hdlr);
}

NN<Media::ColorManager> DotNet::DNColorManager::GetColorManager()
{
	return this->colorMgr;
}

void DotNet::DNColorManager::RGBUpdated()
{
	const Media::ColorHandler::RGBPARAM *rgb = this->colorMgr->GetRGBParam();
	System::Int32 i = this->hdlrs->Count;
	while (i-- > 0)
	{
		static_cast<DNColorHandler*>(this->hdlrs->Item[i])->RGBParamChanged(rgb);
	}
}

void DotNet::DNColorManager::YUVUpdated()
{
	NN<const Media::ColorHandler::YUVPARAM> yuv = this->colorMgr->GetYUVParam();
	System::Int32 i = this->hdlrs->Count;
	while (i-- > 0)
	{
		static_cast<DNColorHandler*>(this->hdlrs->Item[i])->YUVParamChanged(yuv);
	}
}
