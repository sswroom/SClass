#ifndef _SM_IO_WPDMANAGER
#define _SM_IO_WPDMANAGER
#include "Data/ArrayList.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class WPDManager;

	class WPDeviceInfo
	{
	private:
		NN<WPDManager> mgr;
		const WChar *devId;
		const WChar *devName;
		const WChar *devDesc;
		const WChar *devManu;
	public:
		WPDeviceInfo(NN<WPDManager> mgr, const WChar *devId);
		~WPDeviceInfo();
	};

	class WPDManager
	{
	private:
		void *mgr;

	public:
		WPDManager();
		~WPDManager();

		OSInt GetDevices(NN<Data::ArrayListNN<WPDeviceInfo>> devList);
		Bool GetDevName(const WChar *devId, NN<Text::StringBuilderUTF8> sb);
		Bool GetDevDesc(const WChar *devId, NN<Text::StringBuilderUTF8> sb);
		Bool GetDevManu(const WChar *devId, NN<Text::StringBuilderUTF8> sb);
	};
};
#endif
