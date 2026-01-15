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

		typedef void (CALLBACKFUNC DataHdlr)(Data::ByteArrayR buff, AnyType userData);
		typedef struct
		{
			UnsafeArray<UInt8> buff;
			UIntOS buffSize;
		} ReadBuffer;
	private:
		DataHdlr hdlr;
		UIntOS buffSize;

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
		ActiveStreamReader(DataHdlr hdlr, AnyType userData, UIntOS buffSize);
		~ActiveStreamReader();

		void SetUserData(AnyType userData);
		void ReadStream(NN<IO::Stream> stm, OptOut<BottleNeckType> bnt);
		void ReadStreamData(NN<IO::StreamData> stmData, OptOut<BottleNeckType> bnt);
	};
}
#endif
