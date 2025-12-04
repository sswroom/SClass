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
	ServingCell scells[2];
	NN<CellSignal> cell;
	UOSInt ret = 0;
	UOSInt i = 0;
	UOSInt ncells;
	ncells = QuectelQueryServingCell(scells);
	while (i < ncells)
	{
		cell = MemAllocNN(CellSignal);
		Text::StrConcat(cell->mcc, scells[i].mcc);
		Text::StrConcat(cell->mnc, scells[i].mnc);
		cell->lac = scells[i].lac;
		cell->cellid = scells[i].cellid;
		cell->channelNum = scells[i].channelNum;
		cell->pcid = scells[i].pcid;
		cell->servingCell = true;
		cell->sysMode = scells[i].sysMode;
		cell->rssi = scells[i].rssi;
		cell->rscp = scells[i].rscp;
		cell->ecio = scells[i].ecio.IsNull()?NAN:(Double)scells[i].ecio.val;
		cell->rsrp = scells[i].rsrp;
		cell->sinr = scells[i].sinr;
		cell->rsrq = scells[i].rsrq.IsNull()?NAN:(Double)scells[i].rsrq.val;
		cell->rssi2 = nullptr;
		cell->ecio2 = NAN;
		cell->sinr2 = NAN;
		cells->Add(cell);
		i++;
	}
	ret += ncells;
	UTF8Char mcc[4];
	UTF8Char mnc[4];
	UInt16 lac;
	if (ncells > 0)
	{
		Text::StrConcat(mcc, scells[0].mcc);
		Text::StrConcat(mnc, scells[0].mnc);
		lac = scells[0].lac;
	}
	else
	{
		UTF8Char sbuff[32];
		UnsafeArray<UTF8Char> sptr;
		if (this->GSMGetCurrPLMN(sbuff).SetTo(sptr))
		{
			Text::StrConcatC(mcc, sbuff, 3);
			Text::StrConcat(mnc, &sbuff[3]);
			NetworkResult res;
			UInt32 ci;
			RegisterStatus stat;
			AccessTech act;
			if (!this->GSMGetRegisterNetwork(res, stat, lac, ci, act))
			{
				lac = 0;
			}
		}
		else
		{
			mcc[0] = 0;
			mnc[0] = 0;
			lac = 0;
		}
	}

	Data::ArrayListNN<NeighbourCell> neiCells;
	NN<NeighbourCell> ncell;
	if (this->QuectelQueryNeighbourCells(neiCells))
	{
		i = 0;
		ncells = neiCells.GetCount();
		while (i < ncells)
		{
			ncell = neiCells.GetItemNoCheck(i);
			cell = MemAllocNN(CellSignal);
			Text::StrConcat(cell->mcc, mcc);
			Text::StrConcat(cell->mnc, mnc);
			cell->lac = lac;
			cell->cellid = 0;
			cell->channelNum = ncell->channelNum;
			cell->pcid = ncell->pcid;
			cell->servingCell = false;
			cell->sysMode = ncell->sysMode;
			cell->rssi = ncell->rssi;
			cell->rscp = ncell->rscp;
			cell->ecio = ncell->ecio.IsNull()?NAN:(Double)ncell->ecio.val;
			cell->rsrp = ncell->rsrp;
			cell->sinr = ncell->sinr;
			cell->rsrq = ncell->rsrq.IsNull()?NAN:(Double)ncell->rsrq.val;
			cell->rssi2 = nullptr;
			cell->ecio2 = NAN;
			cell->sinr2 = NAN;
			cells->Add(cell);
			ret++;
			i++;
		}
		neiCells.MemFreeAll();
	}
	return ret;
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

UOSInt IO::Device::QuectelGSMModemController::QuectelQueryServingCell(UnsafeArray<ServingCell> cell)
{
	Text::StringBuilderUTF8 sb;
	if (!this->SendStringListCommand(sb, UTF8STRC("AT+QENG=\"servingcell\"")))
	{
		return 0;
	}
	if (!sb.StartsWith(CSTR("+QENG: \"servingcell\",")))
		return 0;
	Text::PString strs[2];
	Text::PString cols[25];
	strs[1] = sb;
	//UOSInt lineCnt = 
	Text::StrSplitLineP(strs, 2, strs[1]);
	UOSInt colCnt = Text::StrSplitP(cols, 25, strs[0], ',');
	State state;
	if (colCnt < 2)
		return 0;
	if (cols[1].Equals(CSTR("\"SEARCH\"")))
		state = State::Search;
	else if (cols[1].Equals(CSTR("\"LIMSRV\"")))
		state = State::LimSrv;
	else if (cols[1].Equals(CSTR("\"NOCONN\"")))
		state = State::NoConn;
	else if (cols[1].Equals(CSTR("\"CONNECT\"")))
		state = State::Connect;
	else
		return 0;
	
	if (colCnt == 2)
	{

	}
	else
	{
		UInt32 uv;
		if (cols[2].Equals(CSTR("\"NR5G-SA\"")) && colCnt == 17)
		{
			cell[0].state = state;
			cell[0].sysMode = SysMode::NR5G_SA;
			if (cols[3].Equals(CSTR("\"FDD\"")))
			{
				cell[0].netMode = NetworkMode::FDD;
			}
			else if (cols[3].Equals(CSTR("\"TDD\"")))
			{
				cell[0].netMode = NetworkMode::TDD;
			}
			else
			{
				cell[0].netMode = NetworkMode::Unknown;
			}
			cols[4].ConcatTo(cell[0].mcc);
			cols[5].ConcatTo(cell[0].mnc);
			if (!cols[8].Hex2UInt16(cell[0].lac)) cell[0].lac = 0;
			if (!cols[6].Hex2UInt64(cell[0].cellid)) cell[0].cellid = 0;
			cell[0].pcid = cols[7].ToUInt32();
			cell[0].channelNum = cols[9].ToUInt32();
			cell[0].freqBandInd = 0;
			cell[0].ulBandwidth = Bandwidth::Unknown;
			cell[0].dlBandwidth = Bandwidth::Unknown;
			cell[0].psc = 0;
			cell[0].rac = 0;
			cell[0].rscp = nullptr;
			cell[0].ecio = nullptr;
			cell[0].rsrp = cols[12].ToNInt32();
			cell[0].rsrq = cols[13].ToNInt32();
			cell[0].rssi = nullptr;
			cell[0].sinr = cols[14].ToInt32();
			cell[0].band = cols[10].ToUInt32();
			if (cols[11].ToUInt32(uv) && uv <= 14) cell[0].nrDLBandwidth = (NRBandwidth)uv; else cell[0].nrDLBandwidth = NRBandwidth::Unknown;
			cell[0].cqi = nullptr;
			cell[0].txPower = nullptr;
			cell[0].srxlev = cols[16].ToNInt32();
			cell[0].phych = PhysicalChannel::Unknown;
			cell[0].sf = SpreadingFactor::UNKNOWN;
			cell[0].slot = nullptr;
			cell[0].speechCode = nullptr;
			cell[0].comMod = false;
			if (cols[15].ToUInt32(uv) && uv <= 4) cell[0].scs = (SubCarrierSpace)uv; else cell[0].scs = SubCarrierSpace::Unknown;
			return 1;
		}
		//+QENG: "servingcell","NOCONN","LTE","FDD",454,06,1625C29,193,1250,3,3,3,2E19,-88,-12,-60,12,-
		else if (cols[2].Equals(CSTR("\"LTE\"")) && (colCnt == 20 || colCnt == 18))
		{
			cell[0].state = state;
			cell[0].sysMode = SysMode::LTE;
			if (cols[3].Equals(CSTR("\"FDD\"")))
			{
				cell[0].netMode = NetworkMode::FDD;
			}
			else if (cols[3].Equals(CSTR("\"TDD\"")))
			{
				cell[0].netMode = NetworkMode::TDD;
			}
			else
			{
				cell[0].netMode = NetworkMode::Unknown;
			}
			cols[4].ConcatTo(cell[0].mcc);
			cols[5].ConcatTo(cell[0].mnc);
			if (!cols[12].Hex2UInt16(cell[0].lac)) cell[0].lac = 0;
			if (!cols[6].Hex2UInt64(cell[0].cellid)) cell[0].cellid = 0;
			cell[0].pcid = cols[7].ToUInt32();
			cell[0].channelNum = cols[8].ToUInt32();
			cell[0].freqBandInd = cols[9].ToUInt32();
			if (cols[10].ToUInt32(uv) && uv <= 5) cell[0].ulBandwidth = (Bandwidth)uv; else cell[0].ulBandwidth = Bandwidth::Unknown;
			if (cols[11].ToUInt32(uv) && uv <= 5) cell[0].dlBandwidth = (Bandwidth)uv; else cell[0].dlBandwidth = Bandwidth::Unknown;
			cell[0].psc = 0;
			cell[0].rac = 0;
			cell[0].rscp = nullptr;
			cell[0].ecio = nullptr;
			cell[0].rsrp = cols[13].ToNInt32();
			cell[0].rsrq = cols[14].ToNInt32();
			cell[0].rssi = cols[15].ToNInt32();
			cell[0].sinr = cols[16].ToUInt32() * 0.2 - 20;
			cell[0].band = 0;
			cell[0].nrDLBandwidth = NRBandwidth::Unknown;
			if (colCnt == 20)
			{
				cell[0].cqi = cols[17].ToNInt32();
				cell[0].txPower = cols[18].ToNInt32();
				cell[0].srxlev = cols[19].ToNInt32();
			}
			else
			{
				cell[0].cqi = nullptr;
				cell[0].txPower = nullptr;
				cell[0].srxlev = cols[17].ToNInt32();
			}
			cell[0].phych = PhysicalChannel::Unknown;
			cell[0].sf = SpreadingFactor::UNKNOWN;
			cell[0].slot = nullptr;
			cell[0].speechCode = nullptr;
			cell[0].comMod = false;
			cell[0].scs = SubCarrierSpace::Unknown;
			return 1;
		}
		else if (cols[2].Equals(CSTR("\"WCDMA\"")) && colCnt == 17)
		{
			cell[0].state = state;
			cell[0].sysMode = SysMode::WCDMA;
			cell[0].netMode = NetworkMode::Unknown;
			cols[3].ConcatTo(cell[0].mcc);
			cols[4].ConcatTo(cell[0].mnc);
			if (!cols[5].Hex2UInt16(cell[0].lac)) cell[0].lac = 0;
			if (!cols[6].Hex2UInt64(cell[0].cellid)) cell[0].cellid = 0;
			cell[0].pcid = 0;
			cell[0].channelNum = cols[7].ToUInt32();
			cell[0].freqBandInd = 0;
			cell[0].ulBandwidth = Bandwidth::Unknown;
			cell[0].dlBandwidth = Bandwidth::Unknown;
			cell[0].psc = cols[8].ToUInt32();
			cell[0].rac = cols[9].ToUInt32();
			cell[0].rscp = cols[10].ToNInt32();
			cell[0].ecio = cols[11].ToNInt32();
			cell[0].rsrp = nullptr;
			cell[0].rsrq = nullptr;
			cell[0].rssi = nullptr;
			cell[0].sinr = NAN;
			cell[0].band = 0;
			cell[0].nrDLBandwidth = NRBandwidth::Unknown;
			cell[0].cqi = nullptr;
			cell[0].txPower = nullptr;
			cell[0].srxlev = nullptr;
			if (cols[12].ToUInt32(uv) && uv <= 1) cell[0].phych = (PhysicalChannel)uv; else cell[0].phych = PhysicalChannel::Unknown;
			if (cols[13].ToUInt32(uv) && uv <= 8) cell[0].sf = (SpreadingFactor)uv; else cell[0].sf = SpreadingFactor::UNKNOWN;
			cell[0].slot = cols[14].ToNInt32();
			cell[0].speechCode = cols[15].ToNInt32();
			cell[0].comMod = cols[16].ToUInt32() != 0;
			cell[0].scs = SubCarrierSpace::Unknown;
			return 1;
		}
	}
	return 0;
}

Bool IO::Device::QuectelGSMModemController::QuectelQueryNeighbourCells(NN<Data::ArrayListNN<NeighbourCell>> cells)
{
	Text::StringBuilderUTF8 sb;
	if (!this->SendStringListCommand(sb, UTF8STRC("AT+QENG=\"neighbourcell\"")))
	{
		return false;
	}

	Text::PString strs[2];
	Text::PString cols[25];
	UOSInt lineCnt;
	UOSInt colCnt;
	NN<NeighbourCell> cell;
	strs[1] = sb;
	while (true)
	{
		lineCnt = Text::StrSplitLineP(strs, 2, strs[1]);
		colCnt = Text::StrSplitP(cols, 25, strs[0], ',');
		if (colCnt >= 3 && cols[0].StartsWith(CSTR("+QENG: ")))
		{
/*
+QENG: "neighbourcell intra","LTE",1250,193,-9,-88,-62,0,-,-,-,-,-
+QENG: "neighbourcell intra","LTE",1250,396,-13,-92,-70,0,-,-,-,-,-
+QENG: "neighbourcell inter","LTE",1551,193,-11,-88,-68,0,-,-,-,-
+QENG: "neighbourcell inter","LTE",1551,396,-18,-98,-68,0,-,-,-,-
+QENG: "neighbourcell inter","LTE",1551,378,-13,-89,-68,0,-,-,-,-
+QENG: "neighbourcell inter","LTE",1551,178,-17,-100,-68,0,-,-,-,-
*/

			if (cols[0].Equals(CSTR("+QENG: \"neighbourcell intra\"")) && cols[1].Equals(CSTR("\"LTE\"")) && colCnt == 13)
			{
				cell = MemAllocNN(NeighbourCell);
				cell->mode = NeighbourcellMode::NeighbourcellIntra;
				cell->sysMode = SysMode::LTE;
				cell->channelNum = cols[2].ToUInt32();
				cell->pcid = cols[3].ToUInt32();
				cell->rsrq = cols[4].ToNInt32();
				cell->rsrp = cols[5].ToNInt32();
				cell->rssi = cols[6].ToNInt32();
				cell->sinr = cols[7].ToUInt32()*0.2 - 20;
				cell->srxlev = cols[8].ToNInt32();
				cell->cellReselPriority = cols[9].ToNInt32();
				cell->sNonIntraSearch = cols[10].ToNInt32();
				cell->threshServingLow = cols[11].ToNInt32();
				cell->sIntraSearch = cols[12].ToNInt32();
				cell->threshXHigh = nullptr;
				cell->threshXLow = nullptr;
				cell->psc = 0;
				cell->rscp = nullptr;
				cell->ecio = nullptr;
				cell->srxqual = nullptr;
				cell->set = nullptr;
				cell->rank = nullptr;
				cells->Add(cell);
			}
			else if (cols[0].Equals(CSTR("+QENG: \"neighbourcell inter\"")) && cols[1].Equals(CSTR("\"LTE\"")) && colCnt == 12)
			{
				cell = MemAllocNN(NeighbourCell);
				cell->mode = NeighbourcellMode::NeighbourcellInter;
				cell->sysMode = SysMode::LTE;
				cell->channelNum = cols[2].ToUInt32();
				cell->pcid = cols[3].ToUInt32();
				cell->rsrq = cols[4].ToNInt32();
				cell->rsrp = cols[5].ToNInt32();
				cell->rssi = cols[6].ToNInt32();
				cell->sinr = cols[7].ToUInt32()*0.2 - 20;
				cell->srxlev = cols[8].ToNInt32();
				cell->cellReselPriority = cols[9].ToNInt32();
				cell->sNonIntraSearch = nullptr;
				cell->threshServingLow = nullptr;
				cell->sIntraSearch = nullptr;
				cell->threshXLow = cols[10].ToNInt32();
				cell->threshXHigh = cols[11].ToNInt32();
				cell->psc = 0;
				cell->rscp = nullptr;
				cell->ecio = nullptr;
				cell->srxqual = nullptr;
				cell->set = nullptr;
				cell->rank = nullptr;
				cells->Add(cell);
			}
			else if (cols[0].Equals(CSTR("+QENG: \"neighbourcell\"")))
			{
				if (cols[1].Equals(CSTR("\"LTE\"")) && colCnt == 7)
				{
					cell = MemAllocNN(NeighbourCell);
					cell->mode = NeighbourcellMode::Neighbourcell;
					cell->sysMode = SysMode::LTE;
					cell->channelNum = cols[2].ToUInt32();
					cell->pcid = cols[3].ToUInt32();
					cell->rsrp = cols[4].ToNInt32();
					cell->rsrq = cols[5].ToNInt32();
					cell->rssi = nullptr;
					cell->sinr = NAN;
					cell->srxlev = cols[6].ToNInt32();
					cell->cellReselPriority = nullptr;
					cell->sNonIntraSearch = nullptr;
					cell->threshServingLow = nullptr;
					cell->sIntraSearch = nullptr;
					cell->threshXLow = nullptr;
					cell->threshXHigh = nullptr;
					cell->psc = 0;
					cell->rscp = nullptr;
					cell->ecio = nullptr;
					cell->srxqual = nullptr;
					cell->set = nullptr;
					cell->rank = nullptr;
					cells->Add(cell);
				}
				else if (cols[1].EndsWith(CSTR("WCDMA")))
				{
					if (colCnt == 10)
					{
						cell = MemAllocNN(NeighbourCell);
						cell->mode = NeighbourcellMode::Neighbourcell;
						cell->sysMode = SysMode::WCDMA;
						cell->channelNum = cols[2].ToUInt32();
						cell->pcid = 0;
						cell->rsrp = nullptr;
						cell->rsrq = nullptr;
						cell->rssi = nullptr;
						cell->sinr = NAN;
						cell->srxlev = cols[9].ToNInt32();
						cell->cellReselPriority = cols[3].ToNInt32();
						cell->sNonIntraSearch = nullptr;
						cell->threshServingLow = nullptr;
						cell->sIntraSearch = nullptr;
						cell->threshXLow = cols[5].ToNInt32();
						cell->threshXHigh = cols[4].ToNInt32();
						cell->psc = cols[6].ToUInt32();
						cell->rscp = cols[7].ToNInt32();
						cell->ecio = cols[8].ToNInt32();
						cell->srxqual = nullptr;
						cell->set = nullptr;
						cell->rank = nullptr;
						cells->Add(cell);
					}
				}
			}
		}
		if (lineCnt != 2)
			break;
	}
	
	return true;
}
