#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Media/DDCReader.h"
#include "Text/MyString.h"

Media::DDCReader::DDCReader(void *hMon)
{
	this->edid = 0;
	this->edidSize = 0;
	this->hMon = hMon;
}

Media::DDCReader::DDCReader(const UTF8Char *monitorId)
{
	this->edid = 0;
	this->edidSize = 0;
	this->hMon = 0;
}

Media::DDCReader::DDCReader(UInt8 *edid, UOSInt edidSize)
{
	this->edid = MemAlloc(UInt8, edidSize);
	this->edidSize = edidSize;
	this->hMon = 0;
	MemCopyNO(this->edid, edid, edidSize);
}

Media::DDCReader::~DDCReader()
{
	if (this->edid)
	{
		MemFree(this->edid);
		this->edid = 0;
	}
}

UInt8 *Media::DDCReader::GetEDID(UOSInt *size)
{
	if (size)
	{
		*size = this->edidSize;
	}
	return this->edid;
}

UOSInt Media::DDCReader::CreateDDCReaders(Data::ArrayList<DDCReader*> *readerList)
{
	return 0;
}
