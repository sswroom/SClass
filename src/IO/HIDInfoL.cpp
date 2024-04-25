#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/HIDInfo.h"
#include "IO/Path.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include <stdio.h>

struct IO::HIDInfo::ClassData
{
IO::HIDInfo::BusType busType;
UInt16 vendor;
UInt16 product;
NN<Text::String> devPath;
};


IO::HIDInfo::HIDInfo(ClassData *clsData)
{
	this->clsData = clsData;
}

IO::HIDInfo::~HIDInfo()
{
	this->clsData->devPath->Release();
	MemFree(this->clsData);
}

IO::HIDInfo::BusType IO::HIDInfo::GetBusType()
{
	return this->clsData->busType;
}

UInt16 IO::HIDInfo::GetVendorId()
{
	return this->clsData->vendor;
}

UInt16 IO::HIDInfo::GetProductId()
{
	return this->clsData->product;
}

Text::String *IO::HIDInfo::GetDevPath()
{
	return this->clsData->devPath.Ptr();
}

IO::Stream *IO::HIDInfo::OpenHID()
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(this->clsData->devPath, IO::FileMode::ReadWriteExisting, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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

OSInt IO::HIDInfo::GetHIDList(NN<Data::ArrayList<HIDInfo*>> hidList)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char *sptr3;
	IO::Path::FindFileSession *sess;
	IO::Path::FindFileSession *sess2;
	ClassData *clsData;
	IO::Path::PathType pt;
	OSInt ret = 0;
	Int32 busType;
	IO::HIDInfo *hid;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/bus/hid/devices/"));
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
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
				sptr2 = Text::StrConcatC(sptr2, UTF8STRC("/hidraw/"));
				sptr3 = Text::StrConcatC(sptr2, UTF8STRC("hidraw*"));
				sess2 = IO::Path::FindFile(CSTRP(sbuff, sptr3));
				if (sess2)
				{
					if ((sptr3 = IO::Path::FindNextFile(sptr2, sess2, 0, &pt, 0)) != 0)
					{
						sptr3 = Text::StrConcatC(Text::StrConcatC(sbuff2, UTF8STRC("/dev/")), sptr2, (UOSInt)(sptr3 - sptr2));
						clsData->devPath = Text::String::New(sbuff2, (UOSInt)(sptr3 - sbuff2));
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