//require wiaguid.lib
#ifndef _SM_MEDIA_WIAMANAGER
#define _SM_MEDIA_WIAMANAGER
#include "Data/ArrayListStrUTF8.h"

namespace Media
{
	class WIADevice;

	class WIAManager
	{
	private:
		void *pWiaDevMgr;
		Data::ArrayListStrUTF8 *devNames;
		Data::ArrayListStrUTF8 *devIds;
	public:
		WIAManager();
		~WIAManager();

		UOSInt GetDeviceCount();
		const UTF8Char *GetDeviceName(UOSInt index);
		Optional<WIADevice> CreateDevice(UOSInt index);
	};

	class WIADevice
	{
	private:
		void *pWiaItem;
	public:
		WIADevice(void *pWiaItem);
		~WIADevice();
	};
}
#endif