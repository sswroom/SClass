#ifndef _SM_MEDIA_DDCREADER
#define _SM_MEDIA_DDCREADER
#include "Data/ArrayList.h"

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

		UInt8 *GetEDID(UOSInt *size);

		static UOSInt CreateDDCReaders(Data::ArrayList<DDCReader*> *readerList);
	};
}
#endif
