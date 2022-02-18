#include "Stdafx.h"
#include "Data/ArrayListInt32.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/USBInfo.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include <wchar.h>

struct IO::USBInfo::ClassData
{
	UInt16 idVendor;
	UInt16 idProduct;
	UInt16 bcdDevice;
	Text::CString dispName;
};

IO::USBInfo::USBInfo(ClassData *info)
{
	ClassData *clsData = MemAlloc(ClassData, 1);
	clsData->idVendor = info->idVendor;
	clsData->idProduct = info->idProduct;
	clsData->bcdDevice = info->bcdDevice;
	clsData->dispName.v = Text::StrCopyNewC(info->dispName.v, info->dispName.leng);
	clsData->dispName.leng = info->dispName.leng;
	this->clsData = clsData;
}

IO::USBInfo::~USBInfo()
{
	Text::StrDelNew(this->clsData->dispName.v);
	MemFree(this->clsData);
}

UInt16 IO::USBInfo::GetVendorId()
{
	return this->clsData->idVendor;
}

UInt16 IO::USBInfo::GetProductId()
{
	return this->clsData->idProduct;
}

UInt16 IO::USBInfo::GetRevision()
{
	return this->clsData->bcdDevice;
}

Text::CString IO::USBInfo::GetDispName()
{
	return this->clsData->dispName;
}

UInt16 USBInfo_ReadI16(Text::CString fileName)
{
	UInt8 buff[33];
	UOSInt readSize;
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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

UOSInt IO::USBInfo::GetUSBList(Data::ArrayList<USBInfo*> *usbList)
{
	ClassData clsData;
	IO::USBInfo *usb;
	Text::StringBuilderUTF8 sb;
	UOSInt ret = 0;
	UTF8Char sbuff[512];
	UInt8 cbuff[256];
	UOSInt readSize;
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char *sptr3;
	IO::Path::FindFileSession *sess;
	IO::FileStream *fs;
	IO::Path::PathType pt;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/bus/usb/devices/"));
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(sbuff, (UOSInt)(sptr2 - sbuff));
	if (sess)
	{
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (sptr[0] != '.')
			{
				sptr3 = Text::StrConcatC(sptr2, UTF8STRC("/idVendor"));
				clsData.idVendor = USBInfo_ReadI16(CSTRP(sbuff, sptr3));
				sptr3 = Text::StrConcatC(sptr2, UTF8STRC("/idProduct"));
				clsData.idProduct = USBInfo_ReadI16(CSTRP(sbuff, sptr3));
				sptr3 = Text::StrConcatC(sptr2, UTF8STRC("/bcdDevice"));
				clsData.bcdDevice = USBInfo_ReadI16(CSTRP(sbuff, sptr3));
				if (clsData.idVendor != 0)
				{
					sb.ClearStr();
					sptr3 = Text::StrConcatC(sptr2, UTF8STRC("/manufacturer"));
					NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr3), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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
							sb.AppendC((const UTF8Char*)cbuff, readSize);
						}
					}
					DEL_CLASS(fs);
					
					sptr3 = Text::StrConcatC(sptr2, UTF8STRC("/product"));
					NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr3), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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
								sb.AppendUTF8Char(' ');
							}						
							sb.AppendC((const UTF8Char*)cbuff, readSize);
						}
					}
					DEL_CLASS(fs);
					
					if (sb.GetLength() > 0)
					{
						clsData.dispName = sb.ToCString();
					}
					else
					{
						clsData.dispName = CSTR("USB Device");
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
