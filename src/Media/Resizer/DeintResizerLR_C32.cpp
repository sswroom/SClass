#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/RGBLUTGen.h"
#include "Media/Resizer/DeintResizerLR_C32.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

#define PI 3.141592653589793

void Media::Resizer::DeintResizerLR_C32::DestoryVertO()
{
	if (oIndex)
	{
		MemFreeA(oIndex);
		oIndex = 0;
	}
	if (oWeight)
	{
		MemFreeA(oWeight);
		oWeight = 0;
	}
	if (this->oFilter)
	{
		this->action->DestroyVertFilter(this->oFilter);
		this->oFilter = 0;
	}
	osSize = 0;
	osStep = 0;
}

void Media::Resizer::DeintResizerLR_C32::DestoryVertE()
{
	if (eIndex)
	{
		MemFreeA(eIndex);
		eIndex = 0;
	}
	if (eWeight)
	{
		MemFreeA(eWeight);
		eWeight = 0;
	}
	if (this->eFilter)
	{
		this->action->DestroyVertFilter(this->eFilter);
		this->eFilter = 0;
	}
	esSize = 0;
	esStep = 0;
}

Media::Resizer::DeintResizerLR_C32::DeintResizerLR_C32(UOSInt hnTap, UOSInt vnTap, const Media::ColorProfile *destColor, Media::ColorManagerSess *colorSess, Media::AlphaType srcAlphaType, Double srcRefLuminance, Media::PixelFormat pf) : Media::Resizer::LanczosResizerLR_C32(hnTap, vnTap, destColor, colorSess, srcAlphaType, srcRefLuminance, pf), Media::IDeintResizer(srcAlphaType)
{
	this->osSize = 0;
	this->odSize = 0;
	this->osStep = 0;
	this->oIndex = 0;
	this->oWeight = 0;
	this->oTap = 0;
	this->oFilter = 0;

	this->esSize = 0;
	this->edSize = 0;
	this->esStep = 0;
	this->eIndex = 0;
	this->eWeight = 0;
	this->eTap = 0;
	this->eFilter = 0;
}

Media::Resizer::DeintResizerLR_C32::~DeintResizerLR_C32()
{
	DestoryVertO();
	DestoryVertE();
}

void Media::Resizer::DeintResizerLR_C32::DeintResize(Media::IDeintResizer::DeintType dType, UInt8 *src, UOSInt sbpl, Double swidth, Double sheight, UInt8 *dest, UOSInt dbpl, UOSInt dwidth, UOSInt dheight, Bool upsideDown)
{
	LRHPARAMETER prm;
	Double w;
	Double h;
	Int32 siWidth;
	Int32 siHeight;

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
		UpdateRGBTable();
		this->action->UpdateRGBTable(this->rgbTable);
	}

	OSInt sstep = (OSInt)sbpl;
	OSInt dstep;
	if (upsideDown)
	{
		dstep = -(OSInt)dbpl;
		dest += dbpl * (dheight - 1);
	}
	else
	{
		dstep = (OSInt)dbpl;
	}

	if (dType == Media::IDeintResizer::DT_TOP_FIELD)
	{
		if (swidth != UOSInt2Double(dwidth))
		{
			Sync::MutexUsage mutUsage(mut);
			if (this->hsSize != swidth || this->hdSize != dwidth)
			{
				DestoryHori();

				if (swidth > UOSInt2Double(dwidth))
				{
					setup_decimation_parameter_h(this->hnTap, swidth, (UInt32)siWidth, dwidth, &prm, 8, 0);
				}
				else
				{
					setup_interpolation_parameter_h(this->hnTap, swidth, (UInt32)siWidth, dwidth,&prm, 8, 0);
				}
				hsSize = swidth;
				hdSize = dwidth;
				hIndex = prm.index;
				hWeight = prm.weight;
				hTap = prm.tap;
				this->hFilter = action->CreateHoriFilter(prm.tap, prm.index, prm.weight, prm.length);
			}

			if (this->osSize != sheight || this->odSize != dheight || this->osStep != (OSInt)(dwidth << 3))
			{
				DestoryVertO();

				if (sheight > UOSInt2Double(dheight))
				{
					setup_decimation_parameter(this->vnTap, sheight, (UInt32)siHeight, dheight, &prm, (OSInt)dwidth << 3, 0.25);
				}
				else
				{
					setup_interpolation_parameter(this->vnTap, sheight, (UInt32)siHeight, dheight, &prm, (OSInt)dwidth << 3, 0.25);
				}
				osSize = sheight;
				odSize = dheight;
				osStep = (OSInt)dwidth << 3;
				oIndex = prm.index;
				oWeight = prm.weight;
				oTap = prm.tap;
				this->oFilter = action->CreateVertFilter(prm.tap, prm.index, prm.weight, prm.length);
			}
			
			action->DoHorizontalVerticalFilter(src, dest, dwidth, (UInt32)siHeight, dheight, hFilter, oFilter, sstep, dstep, this->Media::IImgResizer::srcAlphaType);
			mutUsage.EndUse();
		}
		else
		{
			Sync::MutexUsage mutUsage(mut);
			if (osSize != sheight || odSize != dheight || osStep != (OSInt)sbpl)
			{
				DestoryVertO();

				if (sheight > UOSInt2Double(dheight))
				{
					setup_decimation_parameter(this->vnTap, sheight, (UInt32)siHeight, dheight, &prm, sstep, 0.25);
				}
				else
				{
					setup_interpolation_parameter(this->vnTap, sheight, (UInt32)siHeight, dheight, &prm, sstep, 0.25);
				}
				osSize = sheight;
				odSize = dheight;
				osStep = (OSInt)sbpl;
				oIndex = prm.index;
				oWeight = prm.weight;
				oTap = prm.tap;
				oFilter = action->CreateVertFilter(prm.tap, prm.index, prm.weight, prm.length);
			}
			action->DoVerticalFilter(src, dest, (UInt32)siWidth, (UInt32)siHeight, dheight, oFilter, sstep, dstep, this->Media::IImgResizer::srcAlphaType);
			mutUsage.EndUse();
		}
	}
	else if (dType == Media::IDeintResizer::DT_BOTTOM_FIELD)
	{
		if (swidth != UOSInt2Double(dwidth))
		{
			Sync::MutexUsage mutUsage(mut);
			if (this->hsSize != swidth || this->hdSize != dwidth)
			{
				DestoryHori();

				if (swidth > UOSInt2Double(dwidth))
				{
					setup_decimation_parameter_h(this->hnTap, swidth, (UInt32)siWidth, dwidth, &prm, 8, 0);
				}
				else
				{
					setup_interpolation_parameter_h(this->hnTap, swidth, (UInt32)siWidth, dwidth,&prm, 8, 0);
				}
				hsSize = swidth;
				hdSize = dwidth;
				hIndex = prm.index;
				hWeight = prm.weight;
				hTap = prm.tap;
				hFilter = action->CreateHoriFilter(prm.tap, prm.index, prm.weight, prm.length);
			}

			if (this->esSize != sheight || this->edSize != dheight || this->esStep != (OSInt)(dwidth << 3))
			{
				DestoryVertE();

				if (sheight > UOSInt2Double(dheight))
				{
					setup_decimation_parameter(this->vnTap, sheight, (UInt32)siHeight, dheight, &prm, (OSInt)dwidth << 3, -0.25);
				}
				else
				{
					setup_interpolation_parameter(this->vnTap, sheight, (UInt32)siHeight, dheight, &prm, (OSInt)dwidth << 3, -0.25);
				}
				esSize = sheight;
				edSize = dheight;
				esStep = (OSInt)dwidth << 3;
				eIndex = prm.index;
				eWeight = prm.weight;
				eTap = prm.tap;
				eFilter = action->CreateVertFilter(prm.tap, prm.index, prm.weight, prm.length);
			}
			
			action->DoHorizontalVerticalFilter(src, dest, dwidth, (UInt32)siHeight, dheight, hFilter, eFilter, sstep, dstep, this->Media::IImgResizer::srcAlphaType);
			mutUsage.EndUse();
		}
		else
		{
			Sync::MutexUsage mutUsage(mut);
			if (esSize != sheight || edSize != dheight || esStep != (OSInt)sbpl)
			{
				DestoryVertE();

				if (sheight > UOSInt2Double(dheight))
				{
					setup_decimation_parameter(this->vnTap, sheight, (UInt32)siHeight, dheight, &prm, sstep, -0.25);
				}
				else
				{
					setup_interpolation_parameter(this->vnTap, sheight, (UInt32)siHeight, dheight, &prm, sstep, -0.25);
				}
				esSize = sheight;
				edSize = dheight;
				esStep = (OSInt)sbpl;
				eIndex = prm.index;
				eWeight = prm.weight;
				eTap = prm.tap;
				eFilter = action->CreateVertFilter(prm.tap, prm.index, prm.weight, prm.length);
			}
			action->DoVerticalFilter(src, dest, (UInt32)siWidth, (UInt32)siHeight, dheight, eFilter, sstep, dstep, this->Media::IImgResizer::srcAlphaType);
			mutUsage.EndUse();
		}
	}
	else
	{
		if (swidth != UOSInt2Double(dwidth) && sheight != UOSInt2Double(dheight))
		{
			Sync::MutexUsage mutUsage(mut);
			if (this->hsSize != swidth || this->hdSize != dwidth)
			{
				DestoryHori();

				if (swidth > UOSInt2Double(dwidth))
				{
					setup_decimation_parameter_h(this->hnTap, swidth, (UInt32)siWidth, dwidth, &prm, 8, 0);
				}
				else
				{
					setup_interpolation_parameter_h(this->hnTap, swidth, (UInt32)siWidth, dwidth,&prm, 8, 0);
				}
				hsSize = swidth;
				hdSize = dwidth;
				hIndex = prm.index;
				hWeight = prm.weight;
				hTap = prm.tap;
				hFilter = action->CreateHoriFilter(prm.tap, prm.index, prm.weight, prm.length);
			}

			if (this->vsSize != sheight || this->vdSize != dheight || this->vsStep != (OSInt)(dwidth << 3))
			{
				DestoryVert();

				if (sheight > UOSInt2Double(dheight))
				{
					setup_decimation_parameter(this->vnTap, sheight, (UInt32)siHeight, dheight, &prm, (OSInt)dwidth << 3, 0);
				}
				else
				{
					setup_interpolation_parameter(this->vnTap, sheight, (UInt32)siHeight, dheight, &prm, (OSInt)dwidth << 3, 0);
				}
				vsSize = sheight;
				vdSize = dheight;
				vsStep = (OSInt)dwidth << 3;
				vIndex = prm.index;
				vWeight = prm.weight;
				vTap = prm.tap;
				vFilter = action->CreateVertFilter(prm.tap, prm.index, prm.weight, prm.length);
			}
			
			action->DoHorizontalVerticalFilter(src, dest, dwidth, (UInt32)siHeight, dheight, hFilter, vFilter, sstep, dstep, this->Media::IImgResizer::srcAlphaType);
			mutUsage.EndUse();
		}
		else if (swidth != UOSInt2Double(dwidth))
		{
			Sync::MutexUsage mutUsage(mut);
			if (hsSize != swidth || hdSize != dwidth)
			{
				DestoryHori();

				if (swidth > UOSInt2Double(dwidth))
				{
					setup_decimation_parameter_h(this->hnTap, swidth, (UInt32)siWidth, dwidth, &prm, 8, 0);
				}
				else
				{
					setup_interpolation_parameter_h(this->hnTap, swidth, (UInt32)siWidth, dwidth, &prm, 8, 0);
				}
				hsSize = swidth;
				hdSize = dwidth;
				hIndex = prm.index;
				hWeight = prm.weight;
				hTap = prm.tap;
				hFilter = action->CreateHoriFilter(prm.tap, prm.index, prm.weight, prm.length);
			}

			action->DoHorizontalFilterCollapse(src, dest, dwidth, dheight, hFilter, sstep, dstep, this->Media::IImgResizer::srcAlphaType);
			mutUsage.EndUse();
		}
		else if (sheight != UOSInt2Double(dheight))
		{
			Sync::MutexUsage mutUsage(mut);
			if (vsSize != sheight || vdSize != dheight || vsStep != (OSInt)sbpl)
			{
				DestoryVert();

				if (sheight > UOSInt2Double(dheight))
				{
					setup_decimation_parameter(this->vnTap, sheight, (UInt32)siHeight, dheight, &prm, sstep, 0);
				}
				else
				{
					setup_interpolation_parameter(this->vnTap, sheight, (UInt32)siHeight, dheight, &prm, sstep, 0);
				}
				vsSize = sheight;
				vdSize = dheight;
				vsStep = (OSInt)sbpl;
				vIndex = prm.index;
				vWeight = prm.weight;
				vTap = prm.tap;
				vFilter = action->CreateVertFilter(prm.tap, prm.index, prm.weight, prm.length);
			}
			action->DoVerticalFilter(src, dest, (UInt32)siWidth, (UInt32)siHeight, dheight, vFilter, sstep, dstep, this->Media::IImgResizer::srcAlphaType);
			mutUsage.EndUse();
		}
		else
		{
			Sync::MutexUsage mutUsage(mut);
			action->DoCollapse(src, dest, (UInt32)siWidth, dheight, sstep, dstep, this->Media::IImgResizer::srcAlphaType);
			mutUsage.EndUse();

		}
	}
}

void Media::Resizer::DeintResizerLR_C32::SetDISrcRefLuminance(Double srcRefLuminance)
{
	this->SetSrcRefLuminance(srcRefLuminance);
}
