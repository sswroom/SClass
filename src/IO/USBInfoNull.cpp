#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/USBInfo.h"
#include "Text/MyString.h"
#include "Text/StringBuilderW.h"

struct IO::USBInfo::ClassData
{
	UInt16 vendorId;
	UInt16 productId;
	Text::CString dispName;
};

IO::USBInfo::USBInfo(ClassData *info)
{
	ClassData *clsData = MemAlloc(ClassData, 1);
	clsData->vendorId = info->vendorId;
	clsData->productId = info->productId;
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
	return this->clsData->vendorId;
}

UInt16 IO::USBInfo::GetProductId()
{
	return this->clsData->productId;
}

Text::CString IO::USBInfo::GetDispName()
{
	return this->clsData->dispName;
}

UOSInt IO::USBInfo::GetUSBList(Data::ArrayList<USBInfo*> *usbList)
{
	return 0;
}

