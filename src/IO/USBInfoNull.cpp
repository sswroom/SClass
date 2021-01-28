#include "Stdafx.h"
#include "Data/ArrayListInt32.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/USBInfo.h"
#include "Text/MyString.h"
#include "Text/StringBuilderW.h"

typedef struct
{
	UInt16 vendorId;
	UInt16 productId;
	const UTF8Char *dispName;
} ClassData;

IO::USBInfo::USBInfo(void *info)
{
	ClassData *srcData = (ClassData*)info;
	ClassData *clsData = MemAlloc(ClassData, 1);
	clsData->vendorId = srcData->vendorId;
	clsData->productId = srcData->productId;
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
	return clsData->vendorId;
}

UInt16 IO::USBInfo::GetProductId()
{
	ClassData *clsData = (ClassData*)this->clsData;
	return clsData->productId;
}

const UTF8Char *IO::USBInfo::GetDispName()
{
	ClassData *clsData = (ClassData*)this->clsData;
	return clsData->dispName;
}

OSInt IO::USBInfo::GetUSBList(Data::ArrayList<USBInfo*> *usbList)
{
	return 0;
}

