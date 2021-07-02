#include "Stdafx.h"
#include "Data/ArrayListInt32.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/USBInfo.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include <wchar.h>

typedef struct
{
	UInt16 idVendor;
	UInt16 idProduct;
	UInt16 bcdDevice;
	const UTF8Char *dispName;
} ClassData;

IO::USBInfo::USBInfo(void *info)
{
	ClassData *srcData = (ClassData*)info;
	ClassData *clsData = MemAlloc(ClassData, 1);
	clsData->idVendor = srcData->idVendor;
	clsData->idProduct = srcData->idProduct;
	clsData->bcdDevice = srcData->bcdDevice;
	clsData->dispName = Text::StrCopyNew(srcData->dispName);
	this->clsData = clsData;
}

IO::USBInfo::~USBInfo()
{
	ClassData *clsData = (ClassData*)this->clsData;
	Text::StrDelNew(clsData->dispName);
	MemFree(clsData);
}

UInt16 IO::USBInfo::GetVendorId()
{
	ClassData *clsData = (ClassData*)this->clsData;
	return clsData->idVendor;
}

UInt16 IO::USBInfo::GetProductId()
{
	ClassData *clsData = (ClassData*)this->clsData;
	return clsData->idProduct;
}

UInt16 IO::USBInfo::GetRevision()
{
	ClassData *clsData = (ClassData*)this->clsData;
	return clsData->bcdDevice;
}

const UTF8Char *IO::USBInfo::GetDispName()
{
	ClassData *clsData = (ClassData*)this->clsData;
	return clsData->dispName;
}

UInt16 USBInfo_ReadI16(const UTF8Char *fileName)
{
	UInt8 buff[33];
	UOSInt readSize;
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	readSize = fs->Read(buff, 32);
	buff[readSize] = 0;
	while (readSize > 0)
	{
		if (buff[readSize - 1] == 13 || buff[readSize - 1] == 10)
		{
			readSize--;
			buff[readSize] = 0;
		}
		else
		{
			break;
		}
	}
	DEL_CLASS(fs);
	return (UInt16)(Text::StrHex2Int32C((const UTF8Char*)buff) & 0xffff);
}

OSInt IO::USBInfo::GetUSBList(Data::ArrayList<USBInfo*> *usbList)
{
	ClassData clsData;
	IO::USBInfo *usb;
	Text::StringBuilderUTF8 sb;
	OSInt ret = 0;
	UTF8Char sbuff[512];
	UInt8 cbuff[256];
	UOSInt readSize;
	UTF8Char *sptr;
	UTF8Char *sptr2;
	IO::Path::FindFileSession *sess;
	IO::FileStream *fs;
	IO::Path::PathType pt;
	sptr = Text::StrConcat(sbuff, (const UTF8Char*)"/sys/bus/usb/devices/");
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (sptr[0] != '.')
			{
				Text::StrConcat(sptr2, (const UTF8Char*)"/idVendor");
				clsData.idVendor = USBInfo_ReadI16(sbuff);
				Text::StrConcat(sptr2, (const UTF8Char*)"/idProduct");
				clsData.idProduct = USBInfo_ReadI16(sbuff);
				Text::StrConcat(sptr2, (const UTF8Char*)"/bcdDevice");
				clsData.bcdDevice = USBInfo_ReadI16(sbuff);
				if (clsData.idVendor != 0)
				{
					sb.ClearStr();
					Text::StrConcat(sptr2, (const UTF8Char*)"/manufacturer");
					NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
					if (!fs->IsError())
					{
						readSize = fs->Read(cbuff, 250);
						cbuff[readSize] = 0;
						while (readSize > 0)
						{
							if (cbuff[readSize - 1] == 13 || cbuff[readSize - 1] == 10)
							{
								readSize--;
								cbuff[readSize] = 0;
							}
							else
							{
								break;
							}
						}
						if (readSize > 0)
						{
							sb.Append((const UTF8Char*)cbuff);
						}
					}
					DEL_CLASS(fs);
					
					Text::StrConcat(sptr2, (const UTF8Char*)"/product");
					NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
					if (!fs->IsError())
					{
						readSize = fs->Read(cbuff, 250);
						cbuff[readSize] = 0;
						while (readSize > 0)
						{
							if (cbuff[readSize - 1] == 13 || cbuff[readSize - 1] == 10)
							{
								readSize--;
								cbuff[readSize] = 0;
							}
							else
							{
								break;
							}
						}
						if (readSize > 0)
						{
							if (sb.GetLength() > 0)
							{
								sb.AppendChar(' ', 1);
							}						
							sb.Append((const UTF8Char*)cbuff);
						}
					}
					DEL_CLASS(fs);
					
					if (sb.GetLength() > 0)
					{
						clsData.dispName = sb.ToString();
					}
					else
					{
						clsData.dispName = (const UTF8Char*)"USB Device";
					}
					NEW_CLASS(usb, IO::USBInfo(&clsData));
					usbList->Add(usb);
					ret++;
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
	return ret;
}
