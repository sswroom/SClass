#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/SMBIOSUtil.h"
#include "Text/MyString.h"

namespace
{
	#include <stdlib.h>
	#include <IOKit/IOKitLib.h>
}

IO::SMBIOS *IO::SMBIOSUtil::GetSMBIOS()
{
	IO::SMBIOS *smbios;
	mach_port_t myMasterPort;
	CFMutableDictionaryRef	myMatchingDictionary;
	kern_return_t result;
	io_object_t foundService;

	IOMasterPort(MACH_PORT_NULL, &myMasterPort);
	myMatchingDictionary = IOServiceMatching("AppleSMBIOS");
	foundService = IOServiceGetMatchingService( myMasterPort, myMatchingDictionary );

	if (foundService == 0) {
		return 0;
	}
	
	CFMutableDictionaryRef properties = NULL;
	CFDataRef smbiosdata;
	
	result = IORegistryEntryCreateCFProperties( foundService,
											   &properties,
											   kCFAllocatorDefault,
											   kNilOptions );
	if (result != kIOReturnSuccess) {
		return 0;
	}
	
	result = CFDictionaryGetValueIfPresent( properties,
										   CFSTR("SMBIOS"),
										   (const void **)&smbiosdata );
	if (result != true) {
		return 0;
	}
	
	NEW_CLASS(smbios, IO::SMBIOS((const UInt8*)smbiosdata, (UOSInt)CFDataGetLength(smbiosdata), 0));
	return smbios;
}
