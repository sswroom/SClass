#include "Stdafx.h"
#include "Media/VideoSource.h"

Media::VideoSource::VideoSource()
{
	this->propNames = nullptr;
	this->propSizes = nullptr;
	this->propBuffs = nullptr;

}

Media::VideoSource::~VideoSource()
{
	NN<Data::ArrayListArr<UInt8>> propBuffs;
	if (this->propBuffs.SetTo(propBuffs))
	{
		UIntOS i = propBuffs->GetCount();
		while (i-- > 0)
		{
			MemFreeArr(propBuffs->GetItemNoCheck(i));
		}
	}
	this->propBuffs.Delete();
	this->propSizes.Delete();
	this->propNames.Delete();
}

Bool Media::VideoSource::CaptureImage(ImageCallback imgCb, AnyType userData)
{
	return false;
}

Bool Media::VideoSource::SetPreferFrameType(Media::FrameType ftype)
{
	return false;
}

UIntOS Media::VideoSource::GetFrameSize(UIntOS frameIndex)
{
	return 0;
}

UIntOS Media::VideoSource::ReadFrame(UIntOS frameIndex, UnsafeArray<UInt8> buff)
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
	UnsafeArray<UInt8> prop;
	NN<Data::ArrayListArr<UInt8>> propBuffs;
	NN<Data::ArrayListInt32> propNames;
	NN<Data::ArrayListUInt32> propSizes;
	if (!this->propBuffs.SetTo(propBuffs) || !this->propNames.SetTo(propNames) || !this->propSizes.SetTo(propSizes))
	{
		NEW_CLASSNN(propBuffs, Data::ArrayListArr<UInt8>());
		NEW_CLASSNN(propNames, Data::ArrayListInt32());
		NEW_CLASSNN(propSizes, Data::ArrayListUInt32());
		this->propBuffs = propBuffs;
		this->propNames = propNames;
		this->propSizes = propSizes;
	}
	IntOS i = propNames->SortedIndexOf(propName);
	prop = MemAllocArr(UInt8, propBuffSize);
	MemCopyNO(prop.Ptr(), propBuff.Ptr(), propBuffSize);
	if (i >= 0)
	{
		MemFreeArr(propBuffs->GetItemNoCheck((UIntOS)i));
		propBuffs->SetItem((UIntOS)i, prop);
		propSizes->SetItem((UIntOS)i, propBuffSize);
	}
	else
	{
		propNames->Insert((UIntOS)~i, propName);
		propBuffs->Insert((UIntOS)~i, prop);
		propSizes->Insert((UIntOS)~i, propBuffSize);
	}
}

UnsafeArrayOpt<UInt8> Media::VideoSource::GetProp(Int32 propName, OutParam<UInt32> size)
{
	NN<Data::ArrayListArr<UInt8>> propBuffs;
	NN<Data::ArrayListInt32> propNames;
	NN<Data::ArrayListUInt32> propSizes;
	if (!this->propBuffs.SetTo(propBuffs) || !this->propNames.SetTo(propNames) || !this->propSizes.SetTo(propSizes))
		return nullptr;
	IntOS i = propNames->SortedIndexOf(propName);
	if (i < 0)
		return nullptr;
	size.Set(propSizes->GetItem((UIntOS)i));
	return propBuffs->GetItem((UIntOS)i);
}

Media::MediaType Media::VideoSource::GetMediaType()
{
	return Media::MEDIA_TYPE_VIDEO;
}
