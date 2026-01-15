#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "IO/SerialPortUtil.h"

void IO::SerialPortUtil::FillPortSelector(NN<UI::ItemSelector> selector, UIntOS defPort)
{
	UIntOS defIndex = 0;
	Data::ArrayListNative<UIntOS> ports;
	Data::ArrayListNative<IO::SerialPort::SerialPortType> portTypeList;
	IO::SerialPort::GetAvailablePorts(ports, &portTypeList);
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UIntOS currPort;
	UIntOS currIndex;
	UIntOS i = 0;
	UIntOS j = ports.GetCount();
	while (i < j)
	{
		currPort = ports.GetItem(i);
		sptr = Text::StrUIntOS(Text::StrConcatC(sbuff, UTF8STRC("COM")), currPort);
		sptr = Text::StrConcatC(sptr, UTF8STRC(" ("));
		sptr = IO::SerialPort::GetPortTypeName(portTypeList.GetItem(i)).ConcatTo(sptr);
		sptr = Text::StrConcatC(sptr, UTF8STRC(")"));
		currIndex = selector->AddItem(CSTRP(sbuff, sptr), (void*)(IntOS)currPort);
		if (currPort == defPort)
		{
			defIndex = currIndex;
		}
		i++;
	}
	if (j > 0)
	{
		selector->SetSelectedIndex(defIndex);
	}
}

void IO::SerialPortUtil::FillBaudRateSelector(NN<UI::ItemSelector> selector)
{
	UIntOS defIndex;
	selector->AddItem(CSTR("460800"), (void*)460800);
	selector->AddItem(CSTR("256000"), (void*)256000);
	selector->AddItem(CSTR("230400"), (void*)230400);
	selector->AddItem(CSTR("128000"), (void*)128000);
	defIndex = selector->AddItem(CSTR("115200"), (void*)115200);
	selector->AddItem(CSTR("57600"), (void*)57600);
	selector->AddItem(CSTR("38400"), (void*)38400);
	selector->AddItem(CSTR("19200"), (void*)19200);
	selector->AddItem(CSTR("14400"), (void*)14400);
	selector->AddItem(CSTR("9600"), (void*)9600);
	selector->AddItem(CSTR("4800"), (void*)4800);
	selector->AddItem(CSTR("2400"), (void*)2400);
	selector->AddItem(CSTR("1800"), (void*)1800);
	selector->AddItem(CSTR("1200"), (void*)1200);
	selector->AddItem(CSTR("600"), (void*)600);
	selector->AddItem(CSTR("300"), (void*)300);
	selector->AddItem(CSTR("200"), (void*)200);
	selector->AddItem(CSTR("150"), (void*)150);
	selector->AddItem(CSTR("134"), (void*)134);
	selector->AddItem(CSTR("110"), (void*)110);
	selector->AddItem(CSTR("75"), (void*)75);
	selector->AddItem(CSTR("50"), (void*)50);
	selector->SetSelectedIndex(defIndex);
}

void IO::SerialPortUtil::FillParitySelector(NN<UI::ItemSelector> selector)
{
	UIntOS defIndex;
	defIndex = selector->AddItem(CSTR("No Parity"), (void*)(IntOS)IO::SerialPort::PARITY_NONE);
	selector->AddItem(CSTR("Even Parity"), (void*)(IntOS)IO::SerialPort::PARITY_EVEN);
	selector->AddItem(CSTR("Odd Parity"), (void*)(IntOS)IO::SerialPort::PARITY_ODD);
	selector->SetSelectedIndex(defIndex);
}

Optional<IO::SerialPort> IO::SerialPortUtil::OpenSerialPort(NN<UI::ItemSelector> portSelector, NN<UI::ItemSelector> baudRateSelector, NN<UI::ItemSelector> paritySelector, Bool flowControl)
{
	NN<IO::SerialPort> port;
	NEW_CLASSNN(port, IO::SerialPort(portSelector->GetSelectedItem().GetUIntOS(), (UInt32)baudRateSelector->GetSelectedItem().GetUIntOS(), (IO::SerialPort::ParityType)paritySelector->GetSelectedItem().GetIntOS(), flowControl));
	if (port->IsError())
	{
		port.Delete();
		return nullptr;
	}
	return port;
}
