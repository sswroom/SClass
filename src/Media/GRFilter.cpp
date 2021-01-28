#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/GRFilter.h"

extern "C"
{
	void GRFilter_ProcessLayer32H(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, OSInt level, OSInt hOfst, OSInt vOfst);
	void GRFilter_ProcessLayer32V(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, OSInt level, OSInt hOfst, OSInt vOfst);
	void GRFilter_ProcessLayer32HV(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, OSInt level, OSInt hOfst, OSInt vOfst);
	void GRFilter_ProcessLayer64H(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, OSInt level, OSInt hOfst, OSInt vOfst);
	void GRFilter_ProcessLayer64V(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, OSInt level, OSInt hOfst, OSInt vOfst);
	void GRFilter_ProcessLayer64HV(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, OSInt level, OSInt hOfst, OSInt vOfst);
}

Media::GRFilter::GRFilter()
{
	NEW_CLASS(this->layers, Data::ArrayList<LayerSetting*>());
}

Media::GRFilter::~GRFilter()
{
	LayerSetting *lyr;
	OSInt i;
	i = this->layers->GetCount();
	while (i-- > 0)
	{
		lyr = this->layers->GetItem(i);
		MemFree(lyr);
	}
	DEL_CLASS(this->layers);
}

OSInt Media::GRFilter::GetLayerCount()
{
	return this->layers->GetCount();
}

OSInt Media::GRFilter::AddLayer()
{
	LayerSetting *layer;
	layer = MemAlloc(LayerSetting, 1);
	layer->hOfst = 0;
	layer->vOfst = 0;
	layer->level = 0;
	layer->status = 0;
	return this->layers->Add(layer);
}

Bool Media::GRFilter::RemoveLayer(OSInt layer)
{
	LayerSetting *lyr = this->layers->RemoveAt(layer);
	if (lyr)
	{
		MemFree(lyr);
		return true;
	}
	else
	{
		return false;
	}
}

void Media::GRFilter::SetParameter(OSInt layer, OSInt hOfst, OSInt vOfst, OSInt level, Int32 status)
{
	LayerSetting *lyr = this->layers->GetItem(layer);
	if (lyr)
	{
		lyr->hOfst = hOfst;
		lyr->vOfst = vOfst;
		lyr->level = level;
		lyr->status = status;
	}
}

Bool Media::GRFilter::GetParameter(OSInt layer, OSInt *hOfst, OSInt *vOfst, OSInt *level, Int32 *status)
{
	LayerSetting *lyr = this->layers->GetItem(layer);
	if (lyr)
	{
		if (hOfst)
		{
			*hOfst = lyr->hOfst;
		}
		if (vOfst)
		{
			*vOfst = lyr->vOfst;
		}
		if (level)
		{
			*level = lyr->level;
		}
		if (status)
		{
			*status = lyr->status;
		}
		return true;
	}
	else
	{
		return false;
	}
}

void Media::GRFilter::ProcessImage32(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	OSInt j;
	LayerSetting *lyr;
	UInt8 *tmpPtr1 = srcPtr;
	UInt8 *tmpPtr2 = destPtr;
	i = height;
	while (i-- > 0)
	{
		MemCopyNANC(tmpPtr2, tmpPtr1, width << 2);
		tmpPtr1 += sbpl;
		tmpPtr2 += dbpl;
	}

	i = 0;
	j = this->layers->GetCount();
	while (i < j)
	{
		lyr = this->layers->GetItem(i);
		if (lyr->level != 0 && (lyr->status & 4) == 0)
		{
			if ((lyr->status & 3) == 0)
			{
				GRFilter_ProcessLayer32H(srcPtr, destPtr, width, height, sbpl, dbpl, lyr->level, lyr->hOfst, lyr->vOfst);
			}
			else if ((lyr->status & 3) == 1)
			{
				GRFilter_ProcessLayer32V(srcPtr, destPtr, width, height, sbpl, dbpl, lyr->level, lyr->hOfst, lyr->vOfst);
			}
			else if ((lyr->status & 3) == 2)
			{
				GRFilter_ProcessLayer32HV(srcPtr, destPtr, width, height, sbpl, dbpl, lyr->level, lyr->hOfst, lyr->vOfst);
			}
		}
		i++;
	}
}

void Media::GRFilter::ProcessImage64(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl)
{
	OSInt i;
	OSInt j;
	LayerSetting *lyr;
	UInt8 *tmpPtr1 = srcPtr;
	UInt8 *tmpPtr2 = destPtr;
	i = height;
	while (i-- > 0)
	{
		MemCopyNANC(tmpPtr2, tmpPtr1, width << 3);
		tmpPtr1 += sbpl;
		tmpPtr2 += dbpl;
	}

	i = 0;
	j = this->layers->GetCount();
	while (i < j)
	{
		lyr = this->layers->GetItem(i);
		if (lyr->level != 0 && (lyr->status & 4) == 0)
		{
			if ((lyr->status & 3) == 0)
			{
				GRFilter_ProcessLayer64H(srcPtr, destPtr, width, height, sbpl, dbpl, lyr->level, lyr->hOfst, lyr->vOfst);
			}
			else if ((lyr->status & 3) == 1)
			{
				GRFilter_ProcessLayer64V(srcPtr, destPtr, width, height, sbpl, dbpl, lyr->level, lyr->hOfst, lyr->vOfst);
			}
			else if ((lyr->status & 3) == 2)
			{
				GRFilter_ProcessLayer64HV(srcPtr, destPtr, width, height, sbpl, dbpl, lyr->level, lyr->hOfst, lyr->vOfst);
			}
		}
		i++;
	}
}
