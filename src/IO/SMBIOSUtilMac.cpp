#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/SMBIOSUtil.h"
#include "Text/MyString.h"

#undef UTF8Char
#undef UTF16Char
#undef UTF32Char
#include <stdlib.h>
#include <IOKit/IOKitLib.h>
#define UTF8Char UTF8Ch
#define UTF16Char UTF16Ch
#define UTF32Char UTF32Ch

Optional<IO::SMBIOS> IO::SMBIOSUtil::GetSMBIOS()
{
	NN<IO::SMBIOS> smbios;
	CFMutableDictionaryRef	myMatchingDictionary;
	kern_return_t result;
	io_object_t foundService;
	
	myMatchingDictionary = IOServiceMatching("AppleSMBIOS");
	foundService = IOServiceGetMatchingService( kIOMainPortDefault, myMatchingDictionary );

	if (foundService == 0) {
		return nullptr;
	}
	
	CFMutableDictionaryRef properties = NULL;
	CFDataRef smbiosdata;
	
	result = IORegistryEntryCreateCFProperties( foundService,
											   &properties,
											   kCFAllocatorDefault,
											   kNilOptions );
	if (result != kIOReturnSuccess) {
		return nullptr;
	}
	
	result = CFDictionaryGetValueIfPresent( properties,
										   CFSTR("SMBIOS"),
										   (const void **)&smbiosdata );
	if (result != true) {
		return nullptr;
	}
	
	NEW_CLASSNN(smbios, IO::SMBIOS((const UInt8*)smbiosdata, (UIntOS)CFDataGetLength(smbiosdata), 0));
	return smbios;
}
