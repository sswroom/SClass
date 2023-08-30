#include "Stdafx.h"
#include "Data/ArrayListUInt32.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/PCIInfo.h"
#include "Text/StringBuilderUTF8.h"
#include "Win32/WMIQuery.h"

struct IO::PCIInfo::ClassData
{
	UInt16 vendorId;
	UInt16 productId;
	Text::CString dispName;
};

IO::PCIInfo::PCIInfo(ClassData *info)
{
	ClassData *srcData = info;
	ClassData *clsData = MemAlloc(ClassData, 1);
	clsData->vendorId = srcData->vendorId;
	clsData->productId = srcData->productId;
	clsData->dispName.v = Text::StrCopyNewC(srcData->dispName.v, srcData->dispName.leng).Ptr();
	clsData->dispName.leng = srcData->dispName.leng;
	this->clsData = clsData;
}

IO::PCIInfo::~PCIInfo()
{
	Text::StrDelNew(this->clsData->dispName.v);
	MemFree(this->clsData);
}

UInt16 IO::PCIInfo::GetVendorId()
{
	return this->clsData->vendorId;
}

UInt16 IO::PCIInfo::GetProductId()
{
	return this->clsData->productId;
}

Text::CString IO::PCIInfo::GetDispName()
{
	return this->clsData->dispName;
}

UInt16 PCIInfo_ReadI16(Text::CStringNN fileName)
{
	UInt8 buff[33];
	UOSInt readSize;
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
	return (UInt16)(Text::StrToInt32((const UTF8Char*)buff) & 0xffff);
}

UOSInt IO::PCIInfo::GetPCIList(NotNullPtr<Data::ArrayList<PCIInfo*>> pciList)
{
	Text::StringBuilderUTF8 sb;
	Data::ArrayListUInt32 existList;
	IO::PCIInfo *pci;
	ClassData clsData;
	UInt32 id;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt ret = 0;
	Win32::WMIQuery qry(L"ROOT\\CIMV2");
	DB::DBReader *r = qry.QueryTableData(CSTR_NULL, CSTR("CIM_LogicalDevice"), 0, 0, 0, CSTR_NULL, 0);
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
				r->GetStr(devIdCol, sb);
				if (sb.StartsWith(UTF8STRC("PCI\\VEN_")))
				{
					sb.v[12] = 0;
					sb.v[21] = 0;
					clsData.vendorId = (UInt16)(Text::StrHex2Int16C(sb.ToString() + 8) & 0xffff);
					clsData.productId = (UInt16)(Text::StrHex2Int16C(sb.ToString() + 17) & 0xffff);
					id = (UInt32)(clsData.vendorId << 16) | clsData.productId;
					if (existList.SortedIndexOf(id) < 0)
					{
						existList.SortedInsert(id);
						sb.ClearStr();
						r->GetStr(descCol, sb);
						clsData.dispName = sb.ToCString();
						NEW_CLASS(pci, IO::PCIInfo(&clsData));
						pciList->Add(pci);
						ret++;
					}
				}
			}
		}
		qry.CloseReader(r);
	}
	else //wine
	{
		UTF8Char *sptr;
		UTF8Char *sptr2;
		UTF8Char *sptr2End;
		IO::Path::FindFileSession *sess;
		IO::Path::PathType pt;
		clsData.dispName = CSTR("PCI Device");
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Z:\\sys\\bus\\pci\\devices\\"));
		sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
		sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
		if (sess)
		{
			while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
			{
				if (sptr[0] != '.')
				{
					sptr2End = Text::StrConcatC(sptr2, UTF8STRC("\\vendor"));
					clsData.vendorId = PCIInfo_ReadI16(CSTRP(sbuff, sptr2End));
					sptr2End = Text::StrConcatC(sptr2, UTF8STRC("\\device"));
					clsData.productId = PCIInfo_ReadI16(CSTRP(sbuff, sptr2End));
					if (clsData.vendorId != 0)
					{
						NEW_CLASS(pci, IO::PCIInfo(&clsData));
						pciList->Add(pci);
						ret++;
					}
				}
			}
			IO::Path::FindFileClose(sess);
		}
	}
	return ret;
}
