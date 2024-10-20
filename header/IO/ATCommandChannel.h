#ifndef _SM_IO_ATCOMMANDCHANNEL
#define _SM_IO_ATCOMMANDCHANNEL
#include "AnyType.h"
#include "Data/ArrayListStringNN.h"
#include "Data/Duration.h"
#include "IO/LogTool.h"
#include "IO/Stream.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace IO
{
	class ATCommandChannel
	{
	public:
		typedef void (CALLBACKFUNC EventHandler)(AnyType userObj, UInt8 eventType);
		typedef Bool (CALLBACKFUNC CommandHandler)(AnyType userObj, UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen);

	private:
		NN<IO::Stream> stm;
		Bool stmRelease;
		Sync::Mutex cmdMut;
		Sync::Event cmdEvt;
		EventHandler evtHdlr;
		AnyType evtHdlrObj;
		CommandHandler cmdHdlr;
		AnyType cmdHdlrObj;
		Optional<IO::ILogger> log;

		Data::ArrayListStringNN cmdResults;
		Sync::Mutex cmdResultMut;

		Bool threadRunning;
		Bool threadToStop;
	private:
		static UInt32 __stdcall CmdThread(AnyType userObj);
		void ClearResults();
	public:
		ATCommandChannel(NN<IO::Stream> stm, Bool needRelease);
		~ATCommandChannel();

		NN<IO::Stream> GetStream() const;

		UOSInt SendATCommand(NN<Data::ArrayListStringNN> retArr, UnsafeArray<const UTF8Char> atCmd, UOSInt atCmdLen, Data::Duration timeout);
		UOSInt SendATCommands(NN<Data::ArrayListStringNN> retArr, UnsafeArray<const UTF8Char> atCmd, UOSInt atCmdLen, UnsafeArray<const UTF8Char> atCmdSub, Data::Duration timeout);
		UOSInt SendDialCommand(NN<Data::ArrayListStringNN> retArr, UnsafeArray<const UTF8Char> atCmd, UOSInt atCmdLen, Data::Duration timeout);

		Bool UseCmd(NN<Sync::MutexUsage> mutUsage);
		UOSInt CmdSend(UnsafeArray<const UInt8> data, UOSInt dataSize);
		Optional<Text::String> CmdGetNextResult(Data::Duration timeout);

		void Close();

		void SetEventHandler(EventHandler evtHdlr, AnyType userObj);
		void SetCommandHandler(CommandHandler cmdHdlr, AnyType userObj);
		void SetLogger(IO::ILogger *log);
	};
}
#endif
