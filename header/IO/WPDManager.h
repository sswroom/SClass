#ifndef _SM_IO_WPDMANAGER
#define _SM_IO_WPDMANAGER
#include "Data/ArrayListNN.hpp"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class WPDManager;

	class WPDeviceInfo
	{
	private:
		NN<WPDManager> mgr;
		UnsafeArray<const WChar> devId;
		UnsafeArray<const WChar> devName;
		UnsafeArray<const WChar> devDesc;
		UnsafeArray<const WChar> devManu;
	public:
		WPDeviceInfo(NN<WPDManager> mgr, UnsafeArray<const WChar> devId);
		~WPDeviceInfo();
	};

	class WPDManager
	{
	private:
		void *mgr;

	public:
		WPDManager();
		~WPDManager();

		IntOS GetDevices(NN<Data::ArrayListNN<WPDeviceInfo>> devList);
		Bool GetDevName(UnsafeArray<const WChar> devId, NN<Text::StringBuilderUTF8> sb);
		Bool GetDevDesc(UnsafeArray<const WChar> devId, NN<Text::StringBuilderUTF8> sb);
		Bool GetDevManu(UnsafeArray<const WChar> devId, NN<Text::StringBuilderUTF8> sb);
	};
};
#endif
