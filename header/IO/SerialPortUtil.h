#ifndef _SM_IO_SERIALPORTUTIL
#define _SM_IO_SERIALPORTUTIL
#include "IO/SerialPort.h"
#include "UI/ItemSelector.h"

namespace IO
{
	class SerialPortUtil
	{
	public:
		static void FillPortSelector(NN<UI::ItemSelector> selector, UOSInt defPort);
		static void FillBaudRateSelector(NN<UI::ItemSelector> selector);
		static void FillParitySelector(NN<UI::ItemSelector> selector);
		static Optional<IO::SerialPort> OpenSerialPort(NN<UI::ItemSelector> portSelector, NN<UI::ItemSelector> baudRateSelector, NN<UI::ItemSelector> paritySelector, Bool flowControl);
	};
}
#endif
