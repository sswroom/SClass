#include "Stdafx.h"
#include "IO/Device/QuectelGSMModemController.h"

IO::Device::QuectelGSMModemController::QuectelGSMModemController(NN<IO::ATCommandChannel> channel, Bool needRelease) : IO::GSMModemController(channel, needRelease)
{
}

IO::Device::QuectelGSMModemController::~QuectelGSMModemController()
{

}

UnsafeArrayOpt<UTF8Char> IO::Device::QuectelGSMModemController::GetICCID(UnsafeArray<UTF8Char> sbuff)
{
	return QuectelGetICCID(sbuff);
}

UOSInt IO::Device::QuectelGSMModemController::QueryCells(NN<Data::ArrayListNN<CellSignal>> cells)
{
	return 0;
}

UnsafeArrayOpt<UTF8Char> IO::Device::QuectelGSMModemController::QuectelGetICCID(UnsafeArray<UTF8Char> iccid)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (!this->SendStringCommand(sbuff, UTF8STRC("AT+QCCID"), 1000).SetTo(sptr))
	{
		return 0;
	}
	if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+QCCID: ")))
	{
		return Text::StrConcatC(iccid, &sbuff[8], (UOSInt)(sptr - &sbuff[8]));
	}
	return 0;
}

Bool IO::Device::QuectelGSMModemController::QuectelQueryServingCell(NN<ServingCell> cell)
{
	Text::StringBuilderUTF8 sb;
	if (!this->SendStringListCommand(sb, UTF8STRC("AT+QENG=\"servingcell\"")))
	{
		return false;
	}
	return false;
}

Bool IO::Device::QuectelGSMModemController::QuectelQueryNeighbourCells(NN<Data::ArrayListNN<NeighbourCell>> cells)
{
	Text::StringBuilderUTF8 sb;
	if (!this->SendStringListCommand(sb, UTF8STRC("AT+QENG=\"neighbourcell\"")))
	{
		return false;
	}
	return false;
}
