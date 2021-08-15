#include "Stdafx.h"
#include "Media/ConsoleVideoRenderer.h"

Media::ConsoleVideoRenderer::ConsoleVideoRenderer(Media::MonitorSurfaceMgr *surfaceMgr, Media::ColorManagerSess *colorSess) : Media::VideoRenderer(colorSess, 6, 2)
{
	this->surfaceMgr = surfaceMgr;
	if (this->surfaceMgr)
	{
		this->primarySurface = this->surfaceMgr->CreatePrimarySurface(this->surfaceMgr->GetMonitorHandle(0), 0);
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
}

Bool Media::ConsoleVideoRenderer::IsError()
{
	return this->primarySurface == 0;
}

Bool Media::ConsoleVideoRenderer::IsUpdatingSize()
{
	return false;
}

void Media::ConsoleVideoRenderer::LockUpdateSize(Sync::MutexUsage *mutUsage)
{

}

void Media::ConsoleVideoRenderer::DrawFromMem(UInt8 *memPtr, OSInt lineAdd, OSInt destX, OSInt destY, UOSInt buffWidth, UOSInt buffHeight, Bool clearScn)
{
	if (this->primarySurface)
	{
		this->primarySurface->DrawFromMem(memPtr, lineAdd, destX, destY, buffWidth, buffHeight, clearScn, true);
	}
}
