#include "Stdafx.h"
#include "Data/MacCore.h"
#include "IO/PCIInfo.h"
#include "Text/MyString.h"

#undef UTF8Char
#undef UTF16Char
#undef UTF32Char
#include <IOKit/IOKitLib.h>
#define UTF8Char UTF8Ch
#define UTF16Char UTF16Ch
#define UTF32Char UTF32Ch

struct IO::PCIInfo::ClassData
{
	UInt16 vendorId;
	UInt16 productId;
	Text::CStringNN dispName;
};

IO::PCIInfo::PCIInfo(NN<ClassData> srcData)
{
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

UIntOS PCIInfo_AppendDevices(NN<Data::ArrayListNN<IO::PCIInfo>> pciList, const Char *clsName)
{
	IO::PCIInfo::ClassData clsData;
	NN<IO::PCIInfo> pci;
//	Text::StringBuilderUTF8 sb;
	UIntOS ret = 0;

	CFMutableDictionaryRef matchingDict;
	io_iterator_t iter;
	kern_return_t kr;
	io_service_t device;

	matchingDict = IOServiceMatching(clsName);
	if (matchingDict == NULL)
	{
		return 0;
	}

	kr = IOServiceGetMatchingServices(kIOMainPortDefault, matchingDict, &iter);
	if (kr != KERN_SUCCESS)
	{
		return 0;
	}

	while ((device = IOIteratorNext(iter)))
	{
		CFMutableDictionaryRef properties = NULL;
		io_name_t deviceName;
		kern_return_t result;
		result = IORegistryEntryGetName(device, deviceName);
		if (result == kIOReturnSuccess)
		{
			clsData.dispName = Text::CString::FromPtr((const UTF8Char*)deviceName);
			result = IORegistryEntryCreateCFProperties(device, &properties, kCFAllocatorDefault, kNilOptions);
			if (result == kIOReturnSuccess)
			{
				Data::MacDictionary prop(properties);
//				sb.ClearStr();
//				prop.ToString(sb);
//				printf("%s\r\n", sb.ToString());

				CFTypeRef vendorId = prop.Get(CFSTR("vendor-id"));
				CFTypeRef deviceId = prop.Get(CFSTR("device-id"));
				if (vendorId && deviceId)
				{
					clsData.vendorId = ReadUInt16(Data::MacData((CFDataRef)vendorId).Ptr());
					clsData.productId = ReadUInt16(Data::MacData((CFDataRef)deviceId).Ptr());
					NEW_CLASSNN(pci, IO::PCIInfo(clsData));
					pciList->Add(pci);
					ret++;
				}
//				prop.Release();
			}
		}
//		IOObjectRelease(device);
	}

	IOObjectRelease(iter);
	return ret;
}

UIntOS IO::PCIInfo::GetPCIList(NN<Data::ArrayListNN<PCIInfo>> pciList)
{
	UIntOS ret = 0;
	ret += PCIInfo_AppendDevices(pciList, "IOPCIDevice");
	return ret;
}

