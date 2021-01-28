//require gdiplus.lib msimg32.lib
#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/GDIEngineC.h"
#include "Media/ABlend/AlphaBlend8_C8.h"

Media::GDIEngineC::GDIEngineC() : Media::GDIEngine()
{
	DEL_CLASS(this->iab);
	NEW_CLASS(this->iab, Media::ABlend::AlphaBlend8_C8(0, true));
}

Media::GDIEngineC::~GDIEngineC()
{
}
