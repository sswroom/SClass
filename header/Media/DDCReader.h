#ifndef _SM_MEDIA_DDCREADER
#define _SM_MEDIA_DDCREADER
#include "Handles.h"
#include "Data/ArrayListNN.hpp"

namespace Media
{
	class DDCReader
	{
	private:
		Optional<MonitorHandle> hMon;
		UnsafeArrayOpt<UInt8> edid;
		UOSInt edidSize;

	public:
		DDCReader(Optional<MonitorHandle> hMon);
		DDCReader(UnsafeArray<const UTF8Char> monitorId);
		DDCReader(UnsafeArray<UInt8> edid, UOSInt edidSize);
		~DDCReader();

		UnsafeArrayOpt<UInt8> GetEDID(OutParam<UOSInt> size);

		static UOSInt CreateDDCReaders(NN<Data::ArrayListNN<DDCReader>> readerList);
	};
}
#endif
