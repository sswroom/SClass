#ifndef _SM_IO_GSMCELLCAPTURER
#define _SM_IO_GSMCELLCAPTURER
#include "IO/GSMModemController.h"

namespace IO
{
	class GSMCellCapturer
	{
	private:
		NN<IO::ATCommandChannel> channel;
		Bool needRelease;
		Optional<Text::String> manuf;
		Optional<Text::String> model;

	public:
		GSMCellCapturer(NN<IO::ATCommandChannel> channel, Bool needRelease);
		~GSMCellCapturer();

		void Reload();
		Bool IsError() const;
	};
}
#endif
