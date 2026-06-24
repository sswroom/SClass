#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Media/DDCReader.h"
#include "Text/MyString.h"

Media::DDCReader::DDCReader(Optional<MonitorHandle> hMon)
{
	this->edid = nullptr;
	this->edidSize = 0;
	this->hMon = hMon;
}

Media::DDCReader::DDCReader(UnsafeArray<const UTF8Char> monitorId)
{
	this->edid = nullptr;
	this->edidSize = 0;
	this->hMon = nullptr;
}

Media::DDCReader::DDCReader(UnsafeArray<UInt8> edid, UIntOS edidSize)
{
	UnsafeArray<UInt8> nnedid;
	this->edid = nnedid = MemAllocArr(UInt8, edidSize);
	this->edidSize = edidSize;
	this->hMon = nullptr;
	MemCopyNO(nnedid.Ptr(), edid.Ptr(), edidSize);
}

Media::DDCReader::~DDCReader()
{
	UnsafeArray<UInt8> edid;
	if (this->edid.SetTo(edid))
	{
		MemFreeArr(edid);
		this->edid = nullptr;
	}
}

UnsafeArrayOpt<UInt8> Media::DDCReader::GetEDID(OutParam<UIntOS> size)
{
	size.Set(this->edidSize);
	return this->edid;
}

UIntOS Media::DDCReader::CreateDDCReaders(NN<Data::ArrayListNN<DDCReader>> readerList)
{
	return 0;
}
