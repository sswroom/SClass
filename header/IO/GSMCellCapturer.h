#ifndef _SM_IO_GSMCELLCAPTURER
#define _SM_IO_GSMCELLCAPTURER
#include "IO/GSMModemController.h"
#include "Sync/Thread.h"

namespace IO
{
	class GSMCellCapturer
	{
	public:
		typedef void (CALLBACKFUNC UpdateHandler)(NN<Data::ArrayListNN<IO::GSMModemController::CellSignal>> cells, AnyType userObj);
	private:
		NN<IO::ATCommandChannel> channel;
		Bool needRelease;
		Optional<Text::String> manuf;
		Optional<Text::String> model;
		Optional<IO::GSMModemController> modem;
		UpdateHandler hdlr;
		AnyType hdlrObj;
		Sync::Thread thread;

		static void __stdcall CheckThread(NN<Sync::Thread> thread);
	public:
		GSMCellCapturer(NN<IO::ATCommandChannel> channel, Bool needRelease);
		~GSMCellCapturer();

		void Reload();
		Bool Start();
		void Stop();
		void SetUpdateHandler(UpdateHandler hdlr, AnyType userObj);
		Bool IsError() const;
	};
}
#endif
