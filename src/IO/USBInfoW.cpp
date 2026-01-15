#include "Stdafx.h"
#include "Data/ArrayListUInt32.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/USBInfo.h"
#include "Text/StringBuilderUTF8.h"
#include "Win32/WMIQuery.h"

struct IO::USBInfo::ClassData
{
	UInt16 idVendor;
	UInt16 idProduct;
	UInt16 bcdDevice;
	Text::CStringNN dispName;
};

IO::USBInfo::USBInfo(NN<ClassData> info)
{
	NN<ClassData> clsData = MemAllocNN(ClassData);
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
	MemFreeNN(this->clsData);
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

Text::CStringNN IO::USBInfo::GetDispName()
{
	return this->clsData->dispName;
}

UInt16 USBInfo_ReadI16(Text::CStringNN fileName)
{
	UInt8 buff[33];
	UIntOS readSize;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	readSize = fs.Read(Data::ByteArray(buff, 32));
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
	return (UInt16)(Text::StrHex2Int32C((const UTF8Char*)buff) & 0xffff);
}

UIntOS IO::USBInfo::GetUSBList(NN<Data::ArrayListNN<USBInfo>> usbList)
{
	Text::StringBuilderUTF8 sb;
	Data::ArrayListUInt32 existList;
	NN<IO::USBInfo> usb;
	ClassData clsData;
	UInt32 id;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> sptr2End;
	UIntOS ret = 0;
	Win32::WMIQuery qry(L"ROOT\\CIMV2");
	NN<DB::DBReader> r;
	if (qry.QueryTableData(nullptr, CSTR("CIM_LogicalDevice"), 0, 0, 0, nullptr, 0).SetTo(r))
	{
		UIntOS descCol = INVALID_INDEX;
		UIntOS devIdCol = INVALID_INDEX;
		UIntOS i = 0;
		UIntOS j = r->ColCount();
		while (i < j)
		{
			sbuff[0] = 0;
			if (r->GetName(i, sbuff).SetTo(sptr))
			{
				if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("Description")))
				{
					descCol = i;
				}
				else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("DeviceID")))
				{
					devIdCol = i;
				}
			}
			i++;
		}
		if (descCol != INVALID_INDEX && devIdCol != INVALID_INDEX)
		{
			while (r->ReadNext())
			{
				sb.ClearStr();
				r->GetStr(devIdCol, sb);
				if (sb.StartsWith(UTF8STRC("USB\\VID_")))
				{
					sb.v[12] = 0;
					sb.v[21] = 0;
					clsData.idVendor = (UInt16)(Text::StrHex2Int16C(sb.ToString() + 8) & 0xffff);
					clsData.idProduct = (UInt16)(Text::StrHex2Int16C(sb.ToString() + 17) & 0xffff);
					clsData.bcdDevice = 0xffff;//(UInt16)(Text::StrHex2Int32C(sb.ToString() + 17) & 0xffff);
					id = (UInt32)(clsData.idVendor << 16) | clsData.idProduct;
					if (existList.SortedIndexOf(id) < 0)
					{
						existList.SortedInsert(id);
						sb.ClearStr();
						r->GetStr(descCol, sb);
						clsData.dispName = sb.ToCString();
						NEW_CLASSNN(usb, IO::USBInfo(clsData));
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
		UIntOS readSize;
		NN<IO::Path::FindFileSession> sess;
		IO::Path::PathType pt;
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Z:\\sys\\bus\\usb\\devices\\"));
		sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
		if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
		{
			while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).SetTo(sptr2))
			{
				if (sptr[0] != '.')
				{
					sptr2End = Text::StrConcatC(sptr2, UTF8STRC("\\idVendor"));
					clsData.idVendor = USBInfo_ReadI16(CSTRP(sbuff, sptr2End));
					sptr2End = Text::StrConcatC(sptr2, UTF8STRC("\\idProduct"));
					clsData.idProduct = USBInfo_ReadI16(CSTRP(sbuff, sptr2End));
					sptr2End = Text::StrConcatC(sptr2, UTF8STRC("\\bcdDevice"));
					clsData.bcdDevice = USBInfo_ReadI16(CSTRP(sbuff, sptr2End));
					if (clsData.idVendor != 0)
					{
						{
							sb.ClearStr();
							sptr2End = Text::StrConcatC(sptr2, UTF8STRC("\\manufacturer"));
							IO::FileStream fs(CSTRP(sbuff, sptr2End), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
							if (!fs.IsError())
							{
								readSize = fs.Read(BYTEARR(cbuff).WithSize(250));
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
						}
						
						{
							sptr2End = Text::StrConcatC(sptr2, UTF8STRC("\\product"));
							IO::FileStream fs(CSTRP(sbuff, sptr2End), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
							if (!fs.IsError())
							{
								readSize = fs.Read(Data::ByteArray(cbuff, 250));
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
						}
						
						if (sb.GetLength() > 0)
						{
							clsData.dispName = sb.ToCString();
						}
						else
						{
							clsData.dispName = CSTR("USB Device");
						}
						NEW_CLASSNN(usb, IO::USBInfo(clsData));
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
