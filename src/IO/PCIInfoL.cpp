#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/PCIInfo.h"
#include "Text/MyString.h"

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
	readSize = fs.Read(BYTEARR(buff).WithSize(32));
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
	ClassData clsData;
	NN<IO::PCIInfo> pci;
	UIntOS ret = 0;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> sptr3;
	NN<IO::Path::FindFileSession> sess;
	IO::Path::PathType pt;
	clsData.dispName = CSTR("PCI Device");
	sptr = Text::StrConcatC(sbuff, UTF8STRC("/sys/bus/pci/devices/"));
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).SetTo(sptr2))
		{
			if (sptr[0] != '.')
			{
				sptr3 = Text::StrConcatC(sptr2, UTF8STRC("/vendor"));
				clsData.vendorId = PCIInfo_ReadI16(CSTRP(sbuff, sptr3));
				sptr3 = Text::StrConcatC(sptr2, UTF8STRC("/device"));
				clsData.productId = PCIInfo_ReadI16(CSTRP(sbuff, sptr3));
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
	return ret;
}

