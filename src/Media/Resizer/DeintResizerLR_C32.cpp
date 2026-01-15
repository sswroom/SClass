#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/RGBLUTGen.h"
#include "Media/Resizer/DeintResizerLR_C32.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

#define PI 3.141592653589793

void Media::Resizer::DeintResizerLR_C32::DestoryVertO()
{
	UnsafeArray<IntOS> oIndex;
	UnsafeArray<Int64> oWeight;
	if (this->oIndex.SetTo(oIndex))
	{
		MemFreeAArr(oIndex);
		this->oIndex = nullptr;
	}
	if (this->oWeight.SetTo(oWeight))
	{
		MemFreeAArr(oWeight);
		this->oWeight = nullptr;
	}
	NN<LanczosResizerLR_C32Action::VertFilter> vFilter;
	if (this->oFilter.SetTo(vFilter))
	{
		this->action->DestroyVertFilter(vFilter);
		this->oFilter = nullptr;
	}
	this->osSize = 0;
	this->osStep = 0;
}

void Media::Resizer::DeintResizerLR_C32::DestoryVertE()
{
	UnsafeArray<IntOS> eIndex;
	UnsafeArray<Int64> eWeight;
	if (this->eIndex.SetTo(eIndex))
	{
		MemFreeAArr(eIndex);
		this->eIndex = nullptr;
	}
	if (this->eWeight.SetTo(eWeight))
	{
		MemFreeAArr(eWeight);
		this->eWeight = nullptr;
	}
	NN<LanczosResizerLR_C32Action::VertFilter> vFilter;
	if (this->eFilter.SetTo(vFilter))
	{
		this->action->DestroyVertFilter(vFilter);
		this->eFilter = nullptr;
	}
	this->esSize = 0;
	this->esStep = 0;
}

Media::Resizer::DeintResizerLR_C32::DeintResizerLR_C32(UIntOS hnTap, UIntOS vnTap, NN<const Media::ColorProfile> destColor, Optional<Media::ColorManagerSess> colorSess, Media::AlphaType srcAlphaType, Double srcRefLuminance, Media::PixelFormat pf) : Media::Resizer::LanczosResizerLR_C32(hnTap, vnTap, destColor, colorSess, srcAlphaType, srcRefLuminance, pf), Media::DeinterlacingResizer(srcAlphaType)
{
	this->osSize = 0;
	this->odSize = 0;
	this->osStep = 0;
	this->oIndex = nullptr;
	this->oWeight = nullptr;
	this->oTap = 0;
	this->oFilter = nullptr;

	this->esSize = 0;
	this->edSize = 0;
	this->esStep = 0;
	this->eIndex = nullptr;
	this->eWeight = nullptr;
	this->eTap = 0;
	this->eFilter = nullptr;
}

Media::Resizer::DeintResizerLR_C32::~DeintResizerLR_C32()
{
	DestoryVertO();
	DestoryVertE();
}

void Media::Resizer::DeintResizerLR_C32::DeintResize(Media::DeinterlacingResizer::DeintType dType, UnsafeArray<UInt8> src, UIntOS sbpl, Double swidth, Double sheight, UnsafeArray<UInt8> dest, UIntOS dbpl, UIntOS dwidth, UIntOS dheight, Bool upsideDown)
{
	LRHPARAMETER prm;
	Double w;
	Double h;
	Int32 siWidth;
	Int32 siHeight;
	NN<LanczosResizerLR_C32Action::HoriFilter> hFilter;
	NN<LanczosResizerLR_C32Action::VertFilter> vFilter;

	if (dwidth < 16 || dheight < 16)
		return;

	w = swidth;
	h = sheight;
	siWidth = (Int32)w;
	siHeight = (Int32)h;
	w -= siWidth;
	h -= siHeight;
	if (w > 0)
		siWidth++;
	if (h > 0)
		siHeight++;

	if (siWidth <= 0 || siHeight <= 0)
		return;

	if (this->rgbChanged)
	{
		this->rgbChanged = false;
		UnsafeArray<UInt8> rgbTable = UpdateRGBTable();
		this->action->UpdateRGBTable(rgbTable);
	}

	IntOS sstep = (IntOS)sbpl;
	IntOS dstep;
	if (upsideDown)
	{
		dstep = -(IntOS)dbpl;
		dest += dbpl * (dheight - 1);
	}
	else
	{
		dstep = (IntOS)dbpl;
	}

	if (dType == Media::DeinterlacingResizer::DT_TOP_FIELD)
	{
		if (swidth != UIntOS2Double(dwidth))
		{
			Sync::MutexUsage mutUsage(this->mut);
			if (this->hsSize != swidth || this->hdSize != dwidth || !this->hFilter.SetTo(hFilter))
			{
				DestoryHori();

				if (swidth > UIntOS2Double(dwidth))
				{
					SetupDecimationParameterH(this->hnTap, swidth, (UInt32)siWidth, dwidth, prm, 8, 0);
				}
				else
				{
					SetupInterpolationParameterH(this->hnTap, swidth, (UInt32)siWidth, dwidth, prm, 8, 0);
				}
				hsSize = swidth;
				hdSize = dwidth;
				hIndex = prm.index;
				hWeight = prm.weight;
				hTap = prm.tap;
				hFilter = action->CreateHoriFilter(prm.tap, prm.index, prm.weight, prm.length);
				this->hFilter = hFilter;
			}

			if (this->osSize != sheight || this->odSize != dheight || this->osStep != (IntOS)(dwidth << 3) || !this->oFilter.SetTo(vFilter))
			{
				DestoryVertO();

				if (sheight > UIntOS2Double(dheight))
				{
					SetupDecimationParameterV(this->vnTap, sheight, (UInt32)siHeight, dheight, prm, (IntOS)dwidth << 3, 0.25);
				}
				else
				{
					SetupInterpolationParameterV(this->vnTap, sheight, (UInt32)siHeight, dheight, prm, (IntOS)dwidth << 3, 0.25);
				}
				osSize = sheight;
				odSize = dheight;
				osStep = (IntOS)dwidth << 3;
				oIndex = prm.index;
				oWeight = prm.weight;
				oTap = prm.tap;
				vFilter = action->CreateVertFilter(prm.tap, prm.index, prm.weight, prm.length);
				this->oFilter = vFilter;
			}
			
			action->DoHorizontalVerticalFilter(src, dest, dwidth, (UInt32)siHeight, dheight, hFilter, vFilter, sstep, dstep, this->Media::ImageResizer::srcAlphaType);
			mutUsage.EndUse();
		}
		else
		{
			Sync::MutexUsage mutUsage(this->mut);
			if (osSize != sheight || odSize != dheight || osStep != (IntOS)sbpl || !this->oFilter.SetTo(vFilter))
			{
				DestoryVertO();

				if (sheight > UIntOS2Double(dheight))
				{
					SetupDecimationParameterV(this->vnTap, sheight, (UInt32)siHeight, dheight, prm, sstep, 0.25);
				}
				else
				{
					SetupInterpolationParameterV(this->vnTap, sheight, (UInt32)siHeight, dheight, prm, sstep, 0.25);
				}
				osSize = sheight;
				odSize = dheight;
				osStep = (IntOS)sbpl;
				oIndex = prm.index;
				oWeight = prm.weight;
				oTap = prm.tap;
				vFilter = action->CreateVertFilter(prm.tap, prm.index, prm.weight, prm.length);
				this->oFilter = vFilter;
			}
			action->DoVerticalFilter(src, dest, (UInt32)siWidth, (UInt32)siHeight, dheight, vFilter, sstep, dstep, this->Media::ImageResizer::srcAlphaType);
			mutUsage.EndUse();
		}
	}
	else if (dType == Media::DeinterlacingResizer::DT_BOTTOM_FIELD)
	{
		if (swidth != UIntOS2Double(dwidth))
		{
			Sync::MutexUsage mutUsage(this->mut);
			if (this->hsSize != swidth || this->hdSize != dwidth || !this->hFilter.SetTo(hFilter))
			{
				DestoryHori();

				if (swidth > UIntOS2Double(dwidth))
				{
					SetupDecimationParameterH(this->hnTap, swidth, (UInt32)siWidth, dwidth, prm, 8, 0);
				}
				else
				{
					SetupInterpolationParameterH(this->hnTap, swidth, (UInt32)siWidth, dwidth,prm, 8, 0);
				}
				hsSize = swidth;
				hdSize = dwidth;
				hIndex = prm.index;
				hWeight = prm.weight;
				hTap = prm.tap;
				hFilter = action->CreateHoriFilter(prm.tap, prm.index, prm.weight, prm.length);
				this->hFilter = hFilter;
			}

			if (this->esSize != sheight || this->edSize != dheight || this->esStep != (IntOS)(dwidth << 3) || !this->eFilter.SetTo(vFilter))
			{
				DestoryVertE();

				if (sheight > UIntOS2Double(dheight))
				{
					SetupDecimationParameterV(this->vnTap, sheight, (UInt32)siHeight, dheight, prm, (IntOS)dwidth << 3, -0.25);
				}
				else
				{
					SetupInterpolationParameterV(this->vnTap, sheight, (UInt32)siHeight, dheight, prm, (IntOS)dwidth << 3, -0.25);
				}
				esSize = sheight;
				edSize = dheight;
				esStep = (IntOS)dwidth << 3;
				eIndex = prm.index;
				eWeight = prm.weight;
				eTap = prm.tap;
				vFilter = action->CreateVertFilter(prm.tap, prm.index, prm.weight, prm.length);
				this->eFilter = vFilter;
			}
			
			action->DoHorizontalVerticalFilter(src, dest, dwidth, (UInt32)siHeight, dheight, hFilter, vFilter, sstep, dstep, this->Media::ImageResizer::srcAlphaType);
			mutUsage.EndUse();
		}
		else
		{
			Sync::MutexUsage mutUsage(this->mut);
			if (esSize != sheight || edSize != dheight || esStep != (IntOS)sbpl || !this->eFilter.SetTo(vFilter))
			{
				DestoryVertE();

				if (sheight > UIntOS2Double(dheight))
				{
					SetupDecimationParameterV(this->vnTap, sheight, (UInt32)siHeight, dheight, prm, sstep, -0.25);
				}
				else
				{
					SetupInterpolationParameterV(this->vnTap, sheight, (UInt32)siHeight, dheight, prm, sstep, -0.25);
				}
				esSize = sheight;
				edSize = dheight;
				esStep = (IntOS)sbpl;
				eIndex = prm.index;
				eWeight = prm.weight;
				eTap = prm.tap;
				vFilter = action->CreateVertFilter(prm.tap, prm.index, prm.weight, prm.length);
				this->eFilter = vFilter;
			}
			action->DoVerticalFilter(src, dest, (UInt32)siWidth, (UInt32)siHeight, dheight, vFilter, sstep, dstep, this->Media::ImageResizer::srcAlphaType);
			mutUsage.EndUse();
		}
	}
	else
	{
		if (swidth != UIntOS2Double(dwidth) && sheight != UIntOS2Double(dheight))
		{
			Sync::MutexUsage mutUsage(this->mut);
			if (this->hsSize != swidth || this->hdSize != dwidth || !this->hFilter.SetTo(hFilter))
			{
				DestoryHori();

				if (swidth > UIntOS2Double(dwidth))
				{
					SetupDecimationParameterH(this->hnTap, swidth, (UInt32)siWidth, dwidth, prm, 8, 0);
				}
				else
				{
					SetupInterpolationParameterH(this->hnTap, swidth, (UInt32)siWidth, dwidth, prm, 8, 0);
				}
				hsSize = swidth;
				hdSize = dwidth;
				hIndex = prm.index;
				hWeight = prm.weight;
				hTap = prm.tap;
				hFilter = action->CreateHoriFilter(prm.tap, prm.index, prm.weight, prm.length);
				this->hFilter = hFilter;
			}

			if (this->vsSize != sheight || this->vdSize != dheight || this->vsStep != (IntOS)(dwidth << 3) || !this->vFilter.SetTo(vFilter))
			{
				DestoryVert();

				if (sheight > UIntOS2Double(dheight))
				{
					SetupDecimationParameterV(this->vnTap, sheight, (UInt32)siHeight, dheight, prm, (IntOS)dwidth << 3, 0);
				}
				else
				{
					SetupInterpolationParameterV(this->vnTap, sheight, (UInt32)siHeight, dheight, prm, (IntOS)dwidth << 3, 0);
				}
				vsSize = sheight;
				vdSize = dheight;
				vsStep = (IntOS)dwidth << 3;
				vIndex = prm.index;
				vWeight = prm.weight;
				vTap = prm.tap;
				vFilter = action->CreateVertFilter(prm.tap, prm.index, prm.weight, prm.length);
				this->vFilter = vFilter;
			}
			
			action->DoHorizontalVerticalFilter(src, dest, dwidth, (UInt32)siHeight, dheight, hFilter, vFilter, sstep, dstep, this->Media::ImageResizer::srcAlphaType);
			mutUsage.EndUse();
		}
		else if (swidth != UIntOS2Double(dwidth))
		{
			Sync::MutexUsage mutUsage(this->mut);
			if (hsSize != swidth || hdSize != dwidth || !this->hFilter.SetTo(hFilter))
			{
				DestoryHori();

				if (swidth > UIntOS2Double(dwidth))
				{
					SetupDecimationParameterH(this->hnTap, swidth, (UInt32)siWidth, dwidth, prm, 8, 0);
				}
				else
				{
					SetupInterpolationParameterH(this->hnTap, swidth, (UInt32)siWidth, dwidth, prm, 8, 0);
				}
				hsSize = swidth;
				hdSize = dwidth;
				hIndex = prm.index;
				hWeight = prm.weight;
				hTap = prm.tap;
				hFilter = action->CreateHoriFilter(prm.tap, prm.index, prm.weight, prm.length);
				this->hFilter = hFilter;
			}

			action->DoHorizontalFilterCollapse(src, dest, dwidth, dheight, hFilter, sstep, dstep, this->Media::ImageResizer::srcAlphaType);
			mutUsage.EndUse();
		}
		else if (sheight != UIntOS2Double(dheight))
		{
			Sync::MutexUsage mutUsage(this->mut);
			if (vsSize != sheight || vdSize != dheight || vsStep != (IntOS)sbpl || !this->vFilter.SetTo(vFilter))
			{
				DestoryVert();

				if (sheight > UIntOS2Double(dheight))
				{
					SetupDecimationParameterV(this->vnTap, sheight, (UInt32)siHeight, dheight, prm, sstep, 0);
				}
				else
				{
					SetupInterpolationParameterV(this->vnTap, sheight, (UInt32)siHeight, dheight, prm, sstep, 0);
				}
				vsSize = sheight;
				vdSize = dheight;
				vsStep = (IntOS)sbpl;
				vIndex = prm.index;
				vWeight = prm.weight;
				vTap = prm.tap;
				vFilter = action->CreateVertFilter(prm.tap, prm.index, prm.weight, prm.length);
				this->vFilter = vFilter;
			}
			action->DoVerticalFilter(src, dest, (UInt32)siWidth, (UInt32)siHeight, dheight, vFilter, sstep, dstep, this->Media::ImageResizer::srcAlphaType);
			mutUsage.EndUse();
		}
		else
		{
			Sync::MutexUsage mutUsage(this->mut);
			action->DoCollapse(src, dest, (UInt32)siWidth, dheight, sstep, dstep, this->Media::ImageResizer::srcAlphaType);
			mutUsage.EndUse();

		}
	}
}

void Media::Resizer::DeintResizerLR_C32::SetDISrcRefLuminance(Double srcRefLuminance)
{
	this->SetSrcRefLuminance(srcRefLuminance);
}
