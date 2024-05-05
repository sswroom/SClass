#ifndef _SM_MEDIA_DDCREADER
#define _SM_MEDIA_DDCREADER
#include "Data/ArrayListNN.h"

namespace Media
{
	class DDCReader
	{
	private:
		void *hMon;
		UInt8 *edid;
		UOSInt edidSize;

	public:
		DDCReader(void *hMon);
		DDCReader(const UTF8Char *monitorId);
		DDCReader(UInt8 *edid, UOSInt edidSize);
		~DDCReader();

		UInt8 *GetEDID(OutParam<UOSInt> size);

		static UOSInt CreateDDCReaders(NN<Data::ArrayListNN<DDCReader>> readerList);
	};
}
#endif
