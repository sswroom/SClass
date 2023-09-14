#ifndef _SM_IO_ACTIVESTREAMREADER
#define _SM_IO_ACTIVESTREAMREADER
#include "IO/Stream.h"

#define ACTIVESTREAMREADER_BUFFCNT 2


namespace IO
{
	class ActiveStreamReader
	{
	public:
		enum class BottleNeckType
		{
			Unknown,
			Read,
			Write
		};

		typedef void (__stdcall *DataHdlr)(const UInt8 *buff, UOSInt buffSize, void *userData);
		typedef struct
		{
			UInt8 *buff;
			UOSInt buffSize;
		} ReadBuffer;
	private:
		NotNullPtr<IO::Stream> stm;
		DataHdlr hdlr;
		UOSInt buffSize;

		Bool running;
		Bool toStop;
		Int32 currIndex;

		OptOut<BottleNeckType> bnt;
		Bool reading;

		ReadBuffer buffs[ACTIVESTREAMREADER_BUFFCNT];
		Sync::Event fullEvt;
		Sync::Event emptyEvt;
		void *userData;

		static UInt32 __stdcall ReadThread(void *obj);
	public:
		ActiveStreamReader(DataHdlr hdlr, void *userData, NotNullPtr<IO::Stream> stm, UOSInt buffSize);
		~ActiveStreamReader();

		void ReadStream(OptOut<BottleNeckType> bnt);
	};
}
#endif
