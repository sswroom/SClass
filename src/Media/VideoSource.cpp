#include "Stdafx.h"
#include "Media/VideoSource.h"

Media::VideoSource::VideoSource()
{
	this->propNames = 0;
	this->propSizes = 0;
	this->propBuffs = 0;

}

Media::VideoSource::~VideoSource()
{
	if (this->propBuffs)
	{
		UOSInt i = this->propBuffs->GetCount();
		while (i-- > 0)
		{
			MemFree(this->propBuffs->GetItem(i));
		}
		DEL_CLASS(this->propBuffs);
		DEL_CLASS(this->propSizes);
		DEL_CLASS(this->propNames);
	}
}

Bool Media::VideoSource::CaptureImage(ImageCallback imgCb, AnyType userData)
{
	return false;
}

Bool Media::VideoSource::SetPreferFrameType(Media::FrameType ftype)
{
	return false;
}

UOSInt Media::VideoSource::GetFrameSize(UOSInt frameIndex)
{
	return 0;
}

UOSInt Media::VideoSource::ReadFrame(UOSInt frameIndex, UnsafeArray<UInt8> buff)
{
	return 0;
}

Bool Media::VideoSource::ReadFrameBegin()
{
	return true;
}

Bool Media::VideoSource::ReadFrameEnd()
{
	return true;
}

void Media::VideoSource::SetProp(Int32 propName, UnsafeArray<const UInt8> propBuff, UInt32 propBuffSize)
{
	UInt8 *prop;
	if (this->propBuffs == 0)
	{
		NEW_CLASS(this->propBuffs, Data::ArrayListObj<UInt8*>());
		NEW_CLASS(this->propNames, Data::ArrayListInt32());
		NEW_CLASS(this->propSizes, Data::ArrayListUInt32());
	}
	OSInt i = this->propNames->SortedIndexOf(propName);
	prop = MemAlloc(UInt8, propBuffSize);
	MemCopyNO(prop, propBuff.Ptr(), propBuffSize);
	if (i >= 0)
	{
		MemFree(this->propBuffs->GetItem((UOSInt)i));
		this->propBuffs->SetItem((UOSInt)i, prop);
		this->propSizes->SetItem((UOSInt)i, propBuffSize);
	}
	else
	{
		this->propNames->Insert((UOSInt)~i, propName);
		this->propBuffs->Insert((UOSInt)~i, prop);
		this->propSizes->Insert((UOSInt)~i, propBuffSize);
	}
}

UInt8 *Media::VideoSource::GetProp(Int32 propName, UInt32 *size)
{
	if (this->propBuffs == 0)
		return 0;
	OSInt i = this->propNames->SortedIndexOf(propName);
	if (i < 0)
		return 0;
	*size = this->propSizes->GetItem((UOSInt)i);
	return this->propBuffs->GetItem((UOSInt)i);
}

Media::MediaType Media::VideoSource::GetMediaType()
{
	return Media::MEDIA_TYPE_VIDEO;
}
