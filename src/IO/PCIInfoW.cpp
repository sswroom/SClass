#include "Stdafx.h"
#include "Data/ArrayListUInt32.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/PCIInfo.h"
#include "Text/StringBuilderUTF8.h"
#include "Win32/WMIQuery.h"

typedef struct
{
	UInt16 vendorId;
	UInt16 productId;
	const UTF8Char *dispName;
} ClassData;

IO::PCIInfo::PCIInfo(void *info)
{
	ClassData *srcData = (ClassData*)info;
	ClassData *clsData = MemAlloc(ClassData, 1);
	clsData->vendorId = srcData->vendorId;
	clsData->productId = srcData->productId;
	clsData->dispName = Text::StrCopyNew(srcData->dispName);
	this->clsData = clsData;
}

IO::PCIInfo::~PCIInfo()
{
	ClassData *clsData = (ClassData*)this->clsData;
	Text::StrDelNew(clsData->dispName);
	MemFree(clsData);
}

UInt16 IO::PCIInfo::GetVendorId()
{
	ClassData *clsData = (ClassData*)this->clsData;
	return clsData->vendorId;
}

UInt16 IO::PCIInfo::GetProductId()
{
	ClassData *clsData = (ClassData*)this->clsData;
	return clsData->productId;
}

const UTF8Char *IO::PCIInfo::GetDispName()
{
	ClassData *clsData = (ClassData*)this->clsData;
	return clsData->dispName;
}

UInt16 PCIInfo_ReadI16(const UTF8Char *fileName)
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
	return (UInt16)(Text::StrToInt32((const UTF8Char*)buff) & 0xffff);
}

UOSInt IO::PCIInfo::GetPCIList(Data::ArrayList<PCIInfo*> *pciList)
{
	Text::StringBuilderUTF8 sb;
	Data::ArrayListUInt32 existList;
	IO::PCIInfo *pci;
	ClassData clsData;
	UInt32 id;
	UTF8Char sbuff[512];
	UOSInt ret = 0;
	Win32::WMIQuery qry(L"ROOT\\CIMV2");
	DB::DBReader *r = qry.GetTableData((const UTF8Char*)"CIM_LogicalDevice", 0, 0, 0);
	if (r)
	{
		UOSInt descCol = INVALID_INDEX;
		UOSInt devIdCol = INVALID_INDEX;
		UOSInt i = 0;
		UOSInt j = r->ColCount();
		while (i < j)
		{
			sbuff[0] = 0;
			r->GetName(i, sbuff);
			if (Text::StrEquals(sbuff, (const UTF8Char*)"Description"))
			{
				descCol = i;
			}
			else if (Text::StrEquals(sbuff, (const UTF8Char*)"DeviceID"))
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
				if (sb.StartsWith((const UTF8Char*)"PCI\\VEN_"))
				{
					sb.ToString()[12] = 0;
					sb.ToString()[21] = 0;
					clsData.vendorId = (UInt16)(Text::StrHex2Int32C(sb.ToString() + 8) & 0xffff);
					clsData.productId = (UInt16)(Text::StrHex2Int32C(sb.ToString() + 17) & 0xffff);
					id = (UInt32)(clsData.vendorId << 16) | clsData.productId;
					if (existList.SortedIndexOf(id) < 0)
					{
						existList.SortedInsert(id);
						sb.ClearStr();
						r->GetStr(descCol, &sb);
						clsData.dispName = sb.ToString();
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
		IO::Path::FindFileSession *sess;
		IO::Path::PathType pt;
		clsData.dispName = (const UTF8Char*)"PCI Device";
		sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Z:\\sys\\bus\\pci\\devices\\");
		Text::StrConcat(sptr, IO::Path::ALL_FILES);
		sess = IO::Path::FindFile(sbuff);
		if (sess)
		{
			while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
			{
				if (sptr[0] != '.')
				{
					Text::StrConcat(sptr2, (const UTF8Char*)"\\vendor");
					clsData.vendorId = PCIInfo_ReadI16(sbuff);
					Text::StrConcat(sptr2, (const UTF8Char*)"\\device");
					clsData.productId = PCIInfo_ReadI16(sbuff);
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
