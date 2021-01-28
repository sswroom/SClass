#ifndef _SM_IO_ACTIVESTREAMREADER
#define _SM_IO_ACTIVESTREAMREADER
#include "IO/Stream.h"

namespace IO
{
	class ActiveStreamReader
	{
	public:
		typedef enum
		{
			BNT_UNKNOWN,
			BNT_READ,
			BNT_WRITE
		} BottleNeckType;

		typedef void (__stdcall *DataHdlr)(const UInt8 *buff, OSInt buffSize, void *userData);
		typedef struct
		{
			UInt8 *buff;
			OSInt buffSize;
		} ReadBuffer;
	private:
		IO::Stream *stm;
		DataHdlr hdlr;
		OSInt buffSize;

		Bool running;
		Bool toStop;
		Int32 currIndex;

		BottleNeckType *bnt;
		Bool reading;

		ReadBuffer *buffs;
		Sync::Event *fullEvt;
		Sync::Event *emptyEvt;
		void *userData;

		static UInt32 __stdcall ReadThread(void *obj);
	public:
		ActiveStreamReader(DataHdlr hdlr, void *userData, IO::Stream *stm, OSInt buffSize);
		~ActiveStreamReader();

		void ReadStream(BottleNeckType *bnt);
	};
};
#endif
