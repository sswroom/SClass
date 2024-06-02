#include "Stdafx.h"
#include "Data/ArrayListInt32.h"
#include "Data/MacCore.h"
#include "IO/USBInfo.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

#undef UTF8Char
#undef UTF16Char
#undef UTF32Char
#include <IOKit/IOKitLib.h>
#include <IOKit/usb/IOUSBLib.h>
#define UTF8Char UTF8Ch
#define UTF16Char UTF16Ch
#define UTF32Char UTF32Ch

struct IO::USBInfo::ClassData
{
	UInt16 idVendor;
	UInt16 idProduct;
	UInt16 bcdDevice;
	Text::CStringNN dispName;
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

UOSInt USBInfo_AppendDevices(NN<Data::ArrayList<IO::USBInfo*>> usbList, const Char *clsName)
{
	IO::USBInfo::ClassData clsData;
	IO::USBInfo *usb;
//	Text::StringBuilderUTF8 sb;
	UOSInt ret = 0;

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

				CFTypeRef idProduct = prop.Get(CFSTR("idProduct"));
				CFTypeRef idVendor = prop.Get(CFSTR("idVendor"));
				CFTypeRef bcdDevice = prop.Get(CFSTR("bcdDevice"));
				if (idProduct && idVendor && bcdDevice)
				{
					clsData.idVendor = (UInt16)Data::MacNumber((CFNumberRef)idVendor).GetInt32();
					clsData.idProduct = (UInt16)Data::MacNumber((CFNumberRef)idProduct).GetInt32();
					clsData.bcdDevice = (UInt16)Data::MacNumber((CFNumberRef)bcdDevice).GetInt32();
					NEW_CLASS(usb, IO::USBInfo(&clsData));
					usbList->Add(usb);
					ret++;
				}
				CFRelease(properties);
			}
		}
		IOObjectRelease(device);
	}

	IOObjectRelease(iter);
	return ret;
}

UOSInt IO::USBInfo::GetUSBList(NN<Data::ArrayList<USBInfo*>> usbList)
{
	UOSInt ret = 0;
	ret += USBInfo_AppendDevices(usbList, kIOUSBDeviceClassName);
	return ret;
}
