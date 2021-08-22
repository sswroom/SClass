#include "Stdafx.h"
#include "Media/ConsoleVideoRenderer.h"

Media::ConsoleVideoRenderer::ConsoleVideoRenderer(Media::MonitorSurfaceMgr *surfaceMgr, Media::ColorManagerSess *colorSess) : Media::VideoRenderer(colorSess, 6, 2)
{
	this->surfaceMgr = surfaceMgr;
	NEW_CLASS(this->mut, Sync::Mutex());
	if (this->surfaceMgr)
	{
		this->primarySurface = this->surfaceMgr->CreatePrimarySurface(this->surfaceMgr->GetMonitorHandle(0), 0, Media::RT_NONE);
		if (this->primarySurface)
		{
			this->UpdateDispInfo(this->primarySurface->info->dispWidth, this->primarySurface->info->dispHeight, this->primarySurface->info->storeBPP, this->primarySurface->info->pf);
		}
	}
	else
	{
		this->primarySurface = 0;
	}
}

Media::ConsoleVideoRenderer::~ConsoleVideoRenderer()
{
	SDEL_CLASS(this->primarySurface);
	DEL_CLASS(this->mut);
}

Bool Media::ConsoleVideoRenderer::IsError()
{
	return this->primarySurface == 0;
}

void Media::ConsoleVideoRenderer::SetRotateType(Media::RotateType rotateType)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->primarySurface)
	{
		Media::RotateType rtChange = Media::RotateTypeCalc(this->primarySurface->info->rotateType, rotateType);
		UOSInt tmpV;
		this->primarySurface->info->rotateType = rotateType;
		if (rtChange == Media::RT_CW_90 || rtChange == Media::RT_CW_270)
		{
			tmpV = this->primarySurface->info->dispWidth;
			this->primarySurface->info->dispWidth = this->primarySurface->info->dispHeight;
			this->primarySurface->info->dispHeight = tmpV;
			this->UpdateDispInfo(this->primarySurface->info->dispWidth, this->primarySurface->info->dispHeight, this->primarySurface->info->storeBPP, this->primarySurface->info->pf);
			mutUsage.EndUse();
		}
	}	
}

Bool Media::ConsoleVideoRenderer::IsUpdatingSize()
{
	return false;
}

void Media::ConsoleVideoRenderer::LockUpdateSize(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->mut);
}

void Media::ConsoleVideoRenderer::DrawFromMem(UInt8 *memPtr, OSInt lineAdd, OSInt destX, OSInt destY, UOSInt buffWidth, UOSInt buffHeight, Bool clearScn)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->primarySurface)
	{
		this->primarySurface->DrawFromMem(memPtr, lineAdd, destX, destY, buffWidth, buffHeight, clearScn, true);
	}
}
