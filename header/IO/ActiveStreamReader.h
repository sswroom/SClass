#ifndef _SM_IO_ACTIVESTREAMREADER
#define _SM_IO_ACTIVESTREAMREADER
#include "AnyType.h"
#include "IO/Stream.h"
#include "IO/StreamData.h"

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

		typedef void (__stdcall *DataHdlr)(const UInt8 *buff, UOSInt buffSize, AnyType userData);
		typedef struct
		{
			UInt8 *buff;
			UOSInt buffSize;
		} ReadBuffer;
	private:
		DataHdlr hdlr;
		UOSInt buffSize;

		Bool started;
		Bool running;
		Bool toStop;
		Int32 currIndex;

		OptOut<BottleNeckType> bnt;
		Bool reading;

		ReadBuffer buffs[ACTIVESTREAMREADER_BUFFCNT];
		Sync::Event fullEvt;
		Sync::Event emptyEvt;
		AnyType userData;

		static UInt32 __stdcall ReadThread(AnyType obj);
	public:
		ActiveStreamReader(DataHdlr hdlr, AnyType userData, UOSInt buffSize);
		~ActiveStreamReader();

		void SetUserData(AnyType userData);
		void ReadStream(NN<IO::Stream> stm, OptOut<BottleNeckType> bnt);
		void ReadStreamData(NN<IO::StreamData> stmData, OptOut<BottleNeckType> bnt);
	};
}
#endif
