#include "Stdafx.h"
#include "Data/ArrayListUInt32.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/USBInfo.h"
#include "Text/StringBuilderUTF8.h"
#include "Win32/WMIQuery.h"

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

OSInt IO::USBInfo::GetUSBList(Data::ArrayList<USBInfo*> *usbList)
{
	Text::StringBuilderUTF8 sb;
	Data::ArrayListUInt32 existList;
	IO::USBInfo *usb;
	ClassData clsData;
	UInt32 id;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	OSInt ret = 0;
	Win32::WMIQuery qry(L"ROOT\\CIMV2");
	DB::DBReader *r = qry.GetTableData((const UTF8Char*)"CIM_LogicalDevice", 0, 0, 0, 0, 0);
	if (r)
	{
		UOSInt descCol = INVALID_INDEX;
		UOSInt devIdCol = INVALID_INDEX;
		UOSInt i = 0;
		UOSInt j = r->ColCount();
		while (i < j)
		{
			sbuff[0] = 0;
			sptr = r->GetName(i, sbuff);
			if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Description")))
			{
				descCol = i;
			}
			else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("DeviceID")))
			{
				devIdCol = i;
			}
			i++;
		}
		if (descCol != INVALID_INDEX && devIdCol != INVALID_INDEX)
		{
			while (r->ReadNext())
			{
				sb.ClearStr();
				r->GetStr(devIdCol, &sb);
				if (sb.StartsWith(UTF8STRC("USB\\VID_")))
				{
					sb.ToString()[12] = 0;
					sb.ToString()[21] = 0;
					clsData.idVendor = (UInt16)(Text::StrHex2Int32C(sb.ToString() + 8) & 0xffff);
					clsData.idProduct = (UInt16)(Text::StrHex2Int32C(sb.ToString() + 17) & 0xffff);
					clsData.bcdDevice = 0xffff;//(UInt16)(Text::StrHex2Int32C(sb.ToString() + 17) & 0xffff);
					id = (UInt32)(clsData.idVendor << 16) | clsData.idProduct;
					if (existList.SortedIndexOf(id) < 0)
					{
						existList.SortedInsert(id);
						sb.ClearStr();
						r->GetStr(descCol, &sb);
						clsData.dispName = sb.ToString();
						NEW_CLASS(usb, IO::USBInfo(&clsData));
						usbList->Add(usb);
						ret++;
					}
				}
			}
		}
		qry.CloseReader(r);
	}
	else //wine
	{
		UInt8 cbuff[256];
		UOSInt readSize;
		UTF8Char *sptr;
		UTF8Char *sptr2;
		IO::Path::FindFileSession *sess;
		IO::FileStream *fs;
		IO::Path::PathType pt;
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Z:\\sys\\bus\\usb\\devices\\"));
		sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
		sess = IO::Path::FindFile(sbuff, (UOSInt)(sptr2 - sbuff));
		if (sess)
		{
			while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
			{
				if (sptr[0] != '.')
				{
					Text::StrConcatC(sptr2, UTF8STRC("\\idVendor"));
					clsData.idVendor = USBInfo_ReadI16(sbuff);
					Text::StrConcatC(sptr2, UTF8STRC("\\idProduct"));
					clsData.idProduct = USBInfo_ReadI16(sbuff);
					Text::StrConcatC(sptr2, UTF8STRC("\\bcdDevice"));
					clsData.bcdDevice = USBInfo_ReadI16(sbuff);
					if (clsData.idVendor != 0)
					{
						sb.ClearStr();
						Text::StrConcatC(sptr2, UTF8STRC("\\manufacturer"));
						NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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
						
						Text::StrConcatC(sptr2, UTF8STRC("\\product"));
						NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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
								sb.AppendC((const UTF8Char*)cbuff, readSize);
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
	}
	
	return ret;
}
