#ifndef _SM_MEDIA_DDCREADER
#define _SM_MEDIA_DDCREADER
#include "AnyType.h"
#include "Data/ArrayListNN.h"

namespace Media
{
	class DDCReader
	{
	private:
		AnyType hMon;
		UnsafeArrayOpt<UInt8> edid;
		UOSInt edidSize;

	public:
		DDCReader(AnyType hMon);
		DDCReader(UnsafeArray<const UTF8Char> monitorId);
		DDCReader(UnsafeArray<UInt8> edid, UOSInt edidSize);
		~DDCReader();

		UnsafeArrayOpt<UInt8> GetEDID(OutParam<UOSInt> size);

		static UOSInt CreateDDCReaders(NN<Data::ArrayListNN<DDCReader>> readerList);
	};
}
#endif
