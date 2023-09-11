#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/PowerInfo.h"

#undef UTF8Char
#undef UTF16Char
#undef UTF32Char
#include <stdlib.h>
#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/ps/IOPSKeys.h>
#define UTF8Char UTF8Ch
#define UTF16Char UTF16Ch
#define UTF32Char UTF32Ch

Bool IO::PowerInfo::GetPowerStatus(PowerStatus *power)
{
	MemClear(power, sizeof(PowerStatus));

	CFTypeRef sourceInfo = IOPSCopyPowerSourcesInfo();
	if (sourceInfo)
	{
		CFArrayRef powerSrcs = IOPSCopyPowerSourcesList(sourceInfo);
		if (powerSrcs)
		{
			CFIndex i = 0;
			CFIndex j = CFArrayGetCount(powerSrcs);
			while (i < j)
			{
				CFTypeRef ps = CFArrayGetValueAtIndex(powerSrcs, i);
				CFDictionaryRef desc = IOPSGetPowerSourceDescription(sourceInfo, ps);
				if (desc)
				{
					CFStringRef powerType = (CFStringRef)CFDictionaryGetValue(desc, kIOPSTypeKey);
					if (powerType)
					{
						printf("Power Type: %s\r\n", CFStringGetCStringPtr(powerType, kCFStringEncodingUTF8));
					}
					printf("PowerInfo found 2\r\n");
				}
				i++;
			}
			if (j == 0)
			{
				CFStringRef powerType = IOPSGetProvidingPowerSourceType(sourceInfo);
				if (powerType)
				{
					if (CFStringCompare(powerType, CFSTR(kIOPMACPowerKey), kCFCompareAnchored) == kCFCompareEqualTo)
					{
						power->acStatus = ACS_ON;
					}
					else
					{
						power->acStatus = ACS_OFF;
					}
				}
			}
			CFRelease(powerSrcs);
			CFRelease(sourceInfo);
			return true;
		}
		CFRelease(sourceInfo);
		return false;
	}
	else
	{
		return false;
	}
}
