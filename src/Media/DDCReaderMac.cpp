#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Media/DDCReader.h"
#include "Text/MyString.h"
#undef UTF8Char
#undef UTF16Char
#undef UTF32Char
#include <IOKit/graphics/IOGraphicsLib.h>
#include <IOKit/IOKitLib.h>
#define UTF8Char UTF8Ch
#define UTF16Char UTF16Ch
#define UTF32Char UTF32Ch

#include <stdio.h>

UInt8 *DDCReader_GetMonitorEDID(void *hMon, UIntOS *edidSizeRet)
{
	return 0;
}

Media::DDCReader::DDCReader(void *hMon)
{
	this->edid = 0;
	this->edidSize = 0;
	this->hMon = hMon;
	this->edid = DDCReader_GetMonitorEDID(hMon, &edidSize);	
}

Media::DDCReader::DDCReader(const UTF8Char *monitorId)
{
	this->edid = 0;
	this->edidSize = 0;
	this->hMon = 0;
	this->edid = DDCReader_GetMonitorEDID(hMon, &edidSize);	
}

Media::DDCReader::DDCReader(UInt8 *edid, UIntOS edidSize)
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

UInt8 *Media::DDCReader::GetEDID(UIntOS *size)
{
	if (size)
	{
		*size = this->edidSize;
	}
	return this->edid;
}

UIntOS Media::DDCReader::CreateDDCReaders(Data::ArrayList<DDCReader*> *readerList)
{
	UIntOS ret = 0;
	io_object_t object;
	io_iterator_t iterator;
	kern_return_t kernResult;
	kernResult = IOServiceGetMatchingServices(kIOMainPortDefault, IOServiceMatching("IODisplayConnect"), &iterator);
	if (kernResult != KERN_SUCCESS || iterator == 0)
		kernResult = IOServiceGetMatchingServices(kIOMainPortDefault, IOServiceMatching("IODPDevice"), &iterator);
	if (kernResult != KERN_SUCCESS || iterator == 0)
		kernResult = IOServiceGetMatchingServices(kIOMainPortDefault, IOServiceMatching("AppleTCControllerSingleTransport"), &iterator);

	if (KERN_SUCCESS == kernResult)
	{
		if (iterator != 0) {
			object = IOIteratorNext(iterator);
			while (object != 0) {
				CFDictionaryRef dict = IODisplayCreateInfoDictionary(object, kIODisplayOnlyPreferredName);
				if (dict)
				{
					printf("Found\r\n");
					/*
					process data here
					*/
				}
				else
				{
					printf("Cannot get name\r\n");
				}
				object = IOIteratorNext(iterator);
			}
			IOObjectRelease(iterator);
		}
		else
		{
			printf("Iterator not Found\r\n");
		}
	}
	else
	{
		printf("Not Found\r\n");
	}
	return ret;
}
