#ifndef _SM_IO_BTDEVLOG
#define _SM_IO_BTDEVLOG
#include "Data/ArrayListUInt32.h"
#include "Data/UInt64Map.h"
#include "IO/BTScanner.h"

namespace IO
{
	class BTDevLog
	{
	public:
		typedef struct
		{
			UInt8 mac[6];
			UInt64 macInt;
			const UTF8Char *name;
			Int32 txPower;
			Data::ArrayListUInt32 *keys;
		} DevEntry;		
	private:
		Data::UInt64Map<DevEntry*> *logs;

		static Bool IsDefaultName(const UTF8Char *name);
	public:
		BTDevLog();
		~BTDevLog();

		DevEntry *AddEntry(UInt64 macInt, const UTF8Char *name, Int32 txPower);
		void AppendList(Data::UInt64Map<IO::BTScanner::ScanRecord*> *devMap);
		void ClearList();
		Bool LoadFile(const UTF8Char *fileName);
		Bool StoreFile(const UTF8Char *fileName);

		Data::ArrayList<IO::BTDevLog::DevEntry*> *GetLogList();
	};
}
#endif
