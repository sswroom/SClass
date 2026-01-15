#include "Stdafx.h"
#include "Media/ConsoleVideoRenderer.h"

Media::ConsoleVideoRenderer::ConsoleVideoRenderer(NN<Media::MonitorSurfaceMgr> surfaceMgr, NN<Media::ColorManagerSess> colorSess) : Media::VideoRenderer(colorSess, surfaceMgr, 6, 2)
{
	this->surfaceMgr = surfaceMgr;
	this->primarySurface = this->surfaceMgr->CreatePrimarySurface(this->surfaceMgr->GetMonitorHandle(0), nullptr, Media::RotateType::None);
	NN<Media::MonitorSurface> primarySurface;
	if (this->primarySurface.SetTo(primarySurface))
	{
		this->UpdateDispInfo(primarySurface->info.dispSize, primarySurface->info.storeBPP, primarySurface->info.pf);
	}
}

Media::ConsoleVideoRenderer::~ConsoleVideoRenderer()
{
	this->primarySurface.Delete();
}

Bool Media::ConsoleVideoRenderer::IsError()
{
	return this->primarySurface.IsNull();
}

void Media::ConsoleVideoRenderer::SetRotateType(Media::RotateType rotateType)
{
	Sync::MutexUsage mutUsage(this->mut);
	NN<Media::MonitorSurface> primarySurface;
	if (this->primarySurface.SetTo(primarySurface))
	{
		Media::RotateType rtChange = Media::RotateTypeCalc(primarySurface->info.rotateType, rotateType);
		primarySurface->info.rotateType = rotateType;
		if (rtChange == Media::RotateType::CW_90 || rtChange == Media::RotateType::CW_270 || rtChange == Media::RotateType::HFLIP_CW_90 || rtChange == Media::RotateType::HFLIP_CW_270)
		{
			primarySurface->info.dispSize = primarySurface->info.dispSize.SwapXY();
			this->UpdateDispInfo(primarySurface->info.dispSize, primarySurface->info.storeBPP, primarySurface->info.pf);
			mutUsage.EndUse();
		}
	}	
}

Media::RotateType Media::ConsoleVideoRenderer::GetRotateType() const
{
	NN<Media::MonitorSurface> primarySurface;
	if (this->primarySurface.SetTo(primarySurface))
		return primarySurface->info.rotateType;
	return Media::RotateType::None;
}

void Media::ConsoleVideoRenderer::SetSurfaceBugMode(Bool surfaceBugMode)
{
	Sync::MutexUsage mutUsage(this->mut);
	NN<Media::MonitorSurface> primarySurface;
	if (this->primarySurface.SetTo(primarySurface))
	{
		primarySurface->SetSurfaceBugMode(surfaceBugMode);
	}	
}

Bool Media::ConsoleVideoRenderer::IsUpdatingSize()
{
	return false;
}

void Media::ConsoleVideoRenderer::LockUpdateSize(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->mut);
}

void Media::ConsoleVideoRenderer::DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<IntOS> destTL, Math::Size2D<UIntOS> buffSize, Bool clearScn)
{
	Sync::MutexUsage mutUsage(this->mut);
	NN<Media::MonitorSurface> primarySurface;
	if (this->primarySurface.SetTo(primarySurface))
	{
		primarySurface->DrawFromSurface(surface, destTL, buffSize, clearScn, true);
	}
}
