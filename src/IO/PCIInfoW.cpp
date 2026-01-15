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
	Text::CStringNN dispName;
};

IO::PCIInfo::PCIInfo(NN<ClassData> info)
{
	NN<ClassData> srcData = info;
	NN<ClassData> clsData = MemAllocNN(ClassData);
	clsData->vendorId = srcData->vendorId;
	clsData->productId = srcData->productId;
	clsData->dispName.v = Text::StrCopyNewC(srcData->dispName.v, srcData->dispName.leng);
	clsData->dispName.leng = srcData->dispName.leng;
	this->clsData = clsData;
}

IO::PCIInfo::~PCIInfo()
{
	Text::StrDelNew(this->clsData->dispName.v);
	MemFreeNN(this->clsData);
}

UInt16 IO::PCIInfo::GetVendorId()
{
	return this->clsData->vendorId;
}

UInt16 IO::PCIInfo::GetProductId()
{
	return this->clsData->productId;
}

Text::CStringNN IO::PCIInfo::GetDispName()
{
	return this->clsData->dispName;
}

UInt16 PCIInfo_ReadI16(Text::CStringNN fileName)
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
	return (UInt16)(Text::StrToInt32((const UTF8Char*)buff) & 0xffff);
}

UIntOS IO::PCIInfo::GetPCIList(NN<Data::ArrayListNN<PCIInfo>> pciList)
{
	Text::StringBuilderUTF8 sb;
	Data::ArrayListUInt32 existList;
	NN<IO::PCIInfo> pci;
	ClassData clsData;
	UInt32 id;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UIntOS ret = 0;
	Win32::WMIQuery qry(L"ROOT\\CIMV2");
	NN<DB::DBReader> r;
	if (qry.QueryTableData(nullptr, CSTR("CIM_LogicalDevice"), nullptr, 0, 0, nullptr, nullptr).SetTo(r))
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
						NEW_CLASSNN(pci, IO::PCIInfo(clsData));
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
		UnsafeArray<UTF8Char> sptr;
		UnsafeArray<UTF8Char> sptr2;
		UnsafeArray<UTF8Char> sptr2End;
		NN<IO::Path::FindFileSession> sess;
		IO::Path::PathType pt;
		clsData.dispName = CSTR("PCI Device");
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Z:\\sys\\bus\\pci\\devices\\"));
		sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
		if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
		{
			while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).SetTo(sptr2))
			{
				if (sptr[0] != '.')
				{
					sptr2End = Text::StrConcatC(sptr2, UTF8STRC("\\vendor"));
					clsData.vendorId = PCIInfo_ReadI16(CSTRP(sbuff, sptr2End));
					sptr2End = Text::StrConcatC(sptr2, UTF8STRC("\\device"));
					clsData.productId = PCIInfo_ReadI16(CSTRP(sbuff, sptr2End));
					if (clsData.vendorId != 0)
					{
						NEW_CLASSNN(pci, IO::PCIInfo(clsData));
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
