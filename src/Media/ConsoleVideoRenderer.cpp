#include "Stdafx.h"
#include "Media/ConsoleVideoRenderer.h"

Media::ConsoleVideoRenderer::ConsoleVideoRenderer(Media::MonitorSurfaceMgr *surfaceMgr, Media::ColorManagerSess *colorSess) : Media::VideoRenderer(colorSess, surfaceMgr, 6, 2)
{
	this->surfaceMgr = surfaceMgr;
	if (this->surfaceMgr)
	{
		this->primarySurface = this->surfaceMgr->CreatePrimarySurface(this->surfaceMgr->GetMonitorHandle(0), 0, Media::RotateType::None);
		if (this->primarySurface)
		{
			this->UpdateDispInfo(this->primarySurface->info.dispSize, this->primarySurface->info.storeBPP, this->primarySurface->info.pf);
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

void Media::ConsoleVideoRenderer::SetRotateType(Media::RotateType rotateType)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->primarySurface)
	{
		Media::RotateType rtChange = Media::RotateTypeCalc(this->primarySurface->info.rotateType, rotateType);
		this->primarySurface->info.rotateType = rotateType;
		if (rtChange == Media::RotateType::CW_90 || rtChange == Media::RotateType::CW_270)
		{
			this->primarySurface->info.dispSize = this->primarySurface->info.dispSize.SwapXY();
			this->UpdateDispInfo(this->primarySurface->info.dispSize, this->primarySurface->info.storeBPP, this->primarySurface->info.pf);
			mutUsage.EndUse();
		}
	}	
}

void Media::ConsoleVideoRenderer::SetSurfaceBugMode(Bool surfaceBugMode)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->primarySurface)
	{
		this->primarySurface->SetSurfaceBugMode(surfaceBugMode);
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

void Media::ConsoleVideoRenderer::DrawFromSurface(Media::MonitorSurface *surface, Math::Coord2D<OSInt> destTL, Math::Size2D<UOSInt> buffSize, Bool clearScn)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->primarySurface)
	{
		this->primarySurface->DrawFromSurface(surface, destTL, buffSize, clearScn, true);
	}
}
