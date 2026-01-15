#include "Stdafx.h"
#include "IO/PrintMODBUSDevSim.h"

#include <stdio.h>

IO::PrintMODBUSDevSim::PrintMODBUSDevSim()
{
}

IO::PrintMODBUSDevSim::~PrintMODBUSDevSim()
{
}

Text::CStringNN IO::PrintMODBUSDevSim::GetName()
{
	return CSTR("Debug Print");
}

UIntOS IO::PrintMODBUSDevSim::GetValueCount()
{
	return 0;
}

Text::CString IO::PrintMODBUSDevSim::GetValueName(UIntOS index)
{
	return nullptr;
}

Bool IO::PrintMODBUSDevSim::GetValue(UIntOS index, NN<Text::StringBuilderUTF8> sb)
{
	return false;
}

Bool IO::PrintMODBUSDevSim::ToggleValue(UIntOS index)
{
	return false;
}

Bool IO::PrintMODBUSDevSim::ReadCoil(UInt16 coilAddr, OutParam<Bool> value)
{
	printf("MODBUS: Read Coil: addr = %d\r\n", coilAddr);
	return false;
}

Bool IO::PrintMODBUSDevSim::ReadDescreteInput(UInt16 inputAddr, OutParam<Bool> value)
{
	printf("MODBUS: Read Descrete Input: addr = %d\r\n", inputAddr);
	return false;
}

Bool IO::PrintMODBUSDevSim::ReadHoldingRegister(UInt16 regAddr, OutParam<UInt16> value)
{
	printf("MODBUS: Read Holding Register: addr = %d\r\n", regAddr);
	return false;
}

Bool IO::PrintMODBUSDevSim::ReadInputRegister(UInt16 regAddr, OutParam<UInt16> value)
{
	printf("MODBUS: Read Input Register: addr = %d\r\n", regAddr);
	return false;
}

Bool IO::PrintMODBUSDevSim::WriteCoil(UInt16 coilAddr, Bool isHigh)
{
	printf("MODBUS: Write Coil: addr = %d, val = %d\r\n", coilAddr, isHigh);
	return false;
}

Bool IO::PrintMODBUSDevSim::WriteHoldingRegister(UInt16 regAddr, UInt16 val)
{
	printf("MODBUS: Write Holding Register: addr = %d, val = %d\r\n", regAddr, val);
	return false;
}
