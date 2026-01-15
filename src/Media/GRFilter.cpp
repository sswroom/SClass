#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/GRFilter.h"

extern "C"
{
	void GRFilter_ProcessLayer32H(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
	void GRFilter_ProcessLayer32V(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
	void GRFilter_ProcessLayer32HV(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
	void GRFilter_ProcessLayer64H(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
	void GRFilter_ProcessLayer64V(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
	void GRFilter_ProcessLayer64HV(UInt8 *srcPtr, UInt8 *destPtr, UIntOS width, UIntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
}

Media::GRFilter::GRFilter()
{
}

Media::GRFilter::~GRFilter()
{
	this->layers.MemFreeAll();
}

UIntOS Media::GRFilter::GetLayerCount()
{
	return this->layers.GetCount();
}

UIntOS Media::GRFilter::AddLayer()
{
	NN<LayerSetting> layer;
	layer = MemAllocNN(LayerSetting);
	layer->hOfst = 0;
	layer->vOfst = 0;
	layer->level = 0;
	layer->status = 0;
	return this->layers.Add(layer);
}

Bool Media::GRFilter::RemoveLayer(UIntOS layer)
{
	NN<LayerSetting> lyr;
	if (this->layers.RemoveAt(layer).SetTo(lyr))
	{
		MemFreeNN(lyr);
		return true;
	}
	else
	{
		return false;
	}
}

void Media::GRFilter::SetParameter(UIntOS layer, IntOS hOfst, IntOS vOfst, IntOS level, Int32 status)
{
	NN<LayerSetting> lyr;
	if (this->layers.GetItem(layer).SetTo(lyr))
	{
		lyr->hOfst = hOfst;
		lyr->vOfst = vOfst;
		lyr->level = level;
		lyr->status = status;
	}
}

Bool Media::GRFilter::GetParameter(UIntOS layer, OptOut<IntOS> hOfst, OptOut<IntOS> vOfst, OptOut<IntOS> level, OptOut<Int32> status)
{
	NN<LayerSetting> lyr;
	if (this->layers.GetItem(layer).SetTo(lyr))
	{
		hOfst.Set(lyr->hOfst);
		vOfst.Set(lyr->vOfst);
		level.Set(lyr->level);
		status.Set(lyr->status);
		return true;
	}
	else
	{
		return false;
	}
}

void Media::GRFilter::ProcessImage32(UnsafeArray<UInt8> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS width, UIntOS height, IntOS sbpl, IntOS dbpl)
{
	UIntOS i;
	UIntOS j;
	NN<LayerSetting> lyr;
	UnsafeArray<UInt8> tmpPtr1 = srcPtr;
	UnsafeArray<UInt8> tmpPtr2 = destPtr;
	i = height;
	while (i-- > 0)
	{
		MemCopyNANC(tmpPtr2.Ptr(), tmpPtr1.Ptr(), width << 2);
		tmpPtr1 += sbpl;
		tmpPtr2 += dbpl;
	}

	i = 0;
	j = this->layers.GetCount();
	while (i < j)
	{
		lyr = this->layers.GetItemNoCheck(i);
		if (lyr->level != 0 && (lyr->status & 4) == 0)
		{
			if ((lyr->status & 3) == 0)
			{
				GRFilter_ProcessLayer32H(srcPtr.Ptr(), destPtr.Ptr(), width, height, sbpl, dbpl, lyr->level, lyr->hOfst, lyr->vOfst);
			}
			else if ((lyr->status & 3) == 1)
			{
				GRFilter_ProcessLayer32V(srcPtr.Ptr(), destPtr.Ptr(), width, height, sbpl, dbpl, lyr->level, lyr->hOfst, lyr->vOfst);
			}
			else if ((lyr->status & 3) == 2)
			{
				GRFilter_ProcessLayer32HV(srcPtr.Ptr(), destPtr.Ptr(), width, height, sbpl, dbpl, lyr->level, lyr->hOfst, lyr->vOfst);
			}
		}
		i++;
	}
}

void Media::GRFilter::ProcessImage64(UnsafeArray<UInt8> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS width, UIntOS height, IntOS sbpl, IntOS dbpl)
{
	UIntOS i;
	UIntOS j;
	NN<LayerSetting> lyr;
	UnsafeArray<UInt8> tmpPtr1 = srcPtr;
	UnsafeArray<UInt8> tmpPtr2 = destPtr;
	i = height;
	while (i-- > 0)
	{
		MemCopyNANC(tmpPtr2.Ptr(), tmpPtr1.Ptr(), width << 3);
		tmpPtr1 += sbpl;
		tmpPtr2 += dbpl;
	}

	i = 0;
	j = this->layers.GetCount();
	while (i < j)
	{
		lyr = this->layers.GetItemNoCheck(i);
		if (lyr->level != 0 && (lyr->status & 4) == 0)
		{
			if ((lyr->status & 3) == 0)
			{
				GRFilter_ProcessLayer64H(srcPtr.Ptr(), destPtr.Ptr(), width, height, sbpl, dbpl, lyr->level, lyr->hOfst, lyr->vOfst);
			}
			else if ((lyr->status & 3) == 1)
			{
				GRFilter_ProcessLayer64V(srcPtr.Ptr(), destPtr.Ptr(), width, height, sbpl, dbpl, lyr->level, lyr->hOfst, lyr->vOfst);
			}
			else if ((lyr->status & 3) == 2)
			{
				GRFilter_ProcessLayer64HV(srcPtr.Ptr(), destPtr.Ptr(), width, height, sbpl, dbpl, lyr->level, lyr->hOfst, lyr->vOfst);
			}
		}
		i++;
	}
}
