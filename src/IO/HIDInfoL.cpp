#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/HIDInfo.h"
#include "IO/Path.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include <stdio.h>

typedef struct
{
IO::HIDInfo::BusType busType;
UInt16 vendor;
UInt16 product;
const UTF8Char *devPath;
} ClassData;


IO::HIDInfo::HIDInfo(void *clsData)
{
	this->clsData = clsData;
}

IO::HIDInfo::~HIDInfo()
{
	ClassData *data = (ClassData*)this->clsData;
	Text::StrDelNew(data->devPath);
	MemFree(data);
}

IO::HIDInfo::BusType IO::HIDInfo::GetBusType()
{
	ClassData *data = (ClassData*)this->clsData;
	return data->busType;
}

UInt16 IO::HIDInfo::GetVendorId()
{
	ClassData *data = (ClassData*)this->clsData;
	return data->vendor;
}

UInt16 IO::HIDInfo::GetProductId()
{
	ClassData *data = (ClassData*)this->clsData;
	return data->product;
}

const UTF8Char *IO::HIDInfo::GetDevPath()
{
	ClassData *data = (ClassData*)this->clsData;
	return data->devPath;
}

IO::Stream *IO::HIDInfo::OpenHID()
{
	ClassData *data = (ClassData*)this->clsData;
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(data->devPath, IO::FileMode::ReadWriteExisting, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return 0;
	}
	else
	{
		return fs;
	}
}

OSInt IO::HIDInfo::GetHIDList(Data::ArrayList<HIDInfo*> *hidList)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	IO::Path::FindFileSession *sess;
	IO::Path::FindFileSession *sess2;
	ClassData *clsData;
	IO::Path::PathType pt;
	OSInt ret = 0;
	Int32 busType;
	IO::HIDInfo *hid;
	sptr = Text::StrConcat(sbuff, (const UTF8Char*)"/sys/bus/hid/devices/");
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if ((sptr2 - sptr) == 19 && sptr[4] == ':' && sptr[9] == ':' && sptr[14] == '.' && sptr[19] == 0)
			{
				clsData = MemAlloc(ClassData, 1);
				sptr[4] = 0;
				busType = Text::StrHex2Int16C(sptr);
				sptr[4] = ':';
				sptr[9] = 0;
				clsData->vendor = Text::StrHex2UInt16C(&sptr[5]);
				sptr[9] = ':';
				sptr[14] = 0;
				clsData->product = Text::StrHex2UInt16C(&sptr[10]);
				sptr[14] = '.';
				switch (busType)
				{
				case 3:
					clsData->busType = IO::HIDInfo::BT_USB;
					break;
				case 24:
					clsData->busType = IO::HIDInfo::BT_I2C;
					break;
				default:
					clsData->busType = IO::HIDInfo::BT_UNKNOWN;
					break;
				}
				sptr2 = Text::StrConcat(sptr2, (const UTF8Char*)"/hidraw/");
				Text::StrConcat(sptr2, (const UTF8Char*)"hidraw*");
				sess2 = IO::Path::FindFile(sbuff);
				if (sess2)
				{
					if (IO::Path::FindNextFile(sptr2, sess2, 0, &pt, 0))
					{
						Text::StrConcat(Text::StrConcat(sbuff2, (const UTF8Char*)"/dev/"), sptr2);
						clsData->devPath = Text::StrCopyNew(sbuff2);
						NEW_CLASS(hid, IO::HIDInfo(clsData));
						hidList->Add(hid);
						ret++;
					}
					else
					{
						MemFree(clsData);
					}					
					IO::Path::FindFileClose(sess2);
				}
				else
				{
					MemFree(clsData);
				}
				
			}
			else
			{
			}			
		}
		IO::Path::FindFileClose(sess);
	}
	return ret;
}