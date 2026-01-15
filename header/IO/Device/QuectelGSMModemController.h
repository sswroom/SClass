#ifndef _SM_IO_DEVICE_QUECTELGSMMODEMCONTROLLER
#define _SM_IO_DEVICE_QUECTELGSMMODEMCONTROLLER
#include "IO/GSMModemController.h"

namespace IO
{
	namespace Device
	{
		class QuectelGSMModemController : public GSMModemController
		{
		public:
			enum class State
			{
				Unknown,
				Search,
				LimSrv,
				NoConn,
				Connect
			};

			enum class Bandwidth
			{
				BW_1_4MHz = 0,
				BW_3HMz = 1,
				BW_5MHz = 2,
				BW_10MHz = 3,
				BW_15MHz = 4,
				BW_20MHz = 5,
				Unknown = 255
			};

			enum class NRBandwidth
			{
				BW_5MHz = 0,
				BW_10MHz = 1,
				BW_15MHz = 2,
				BW_20MHz = 3,
				BW_25MHz = 4,
				BW_30MHz = 5,
				BW_40MHz = 6,
				BW_50MHz = 7,
				BW_60MHz = 8,
				BW_70MHz = 9,
				BW_80MHz = 10,
				BW_90MHz = 11,
				BW_100MHz = 12,
				BW_200MHz = 13,
				BW_400MHz = 14,
				Unknown = 255
			};

			enum class NetworkMode
			{
				TDD,
				FDD,
				Unknown
			};

			enum class PhysicalChannel
			{
				DPCH = 0,
				FDPCH = 1,
				Unknown
			};

			enum class SpreadingFactor
			{
				SF_4 = 0,
				SF_8 = 1,
				SF_16 = 2,
				SF_32 = 3,
				SF_64 = 4,
				SF_128 = 5,
				SF_256 = 6,
				SF_512 = 7,
				UNKNOWN = 8
			};

			enum class SubCarrierSpace
			{
				SCS_15kHz,
				SCS_30kHz,
				SCS_60kHz,
				SCS_120kHz,
				SCS_240kHz,
				Unknown
			};

			enum class NeighbourcellMode
			{
				NeighbourcellIntra,
				NeighbourcellInter,
				Neighbourcell
			};

			struct ServingCell
			{
				State state;
				SysMode sysMode;
				NetworkMode netMode;
				UTF8Char mcc[4];
				UTF8Char mnc[4];
				UInt16 lac;
				UInt64 cellid;
				UInt32 pcid;
				UInt32 channelNum;
				UInt32 freqBandInd;
				Bandwidth ulBandwidth;
				Bandwidth dlBandwidth;
				UInt32 psc;
				UInt32 rac;
				NInt32 rscp;
				NInt32 ecio;
				NInt32 rsrp;
				NInt32 rsrq;
				NInt32 rssi;
				Double sinr;
				UInt32 band;
				NRBandwidth nrDLBandwidth;
				NInt32 cqi;
				NInt32 txPower;
				NInt32 srxlev;
				PhysicalChannel phych;
				SpreadingFactor sf;
				NInt32 slot;
				NInt32 speechCode;
				Bool comMod;
				SubCarrierSpace scs;
			};

			struct NeighbourCell
			{
				NeighbourcellMode mode;
				SysMode sysMode;
				UInt32 channelNum;
				UInt32 pcid;
				NInt32 rsrp;
				NInt32 rsrq;
				NInt32 rssi;
				Double sinr;
				NInt32 srxlev;
				NInt32 cellReselPriority;
				NInt32 sNonIntraSearch;
				NInt32 threshServingLow;
				NInt32 sIntraSearch;
				NInt32 threshXHigh;
				NInt32 threshXLow;
				UInt32 psc;
				NInt32 rscp;
				NInt32 ecio;
				NInt32 srxqual;
				NInt32 set;
				NInt32 rank;
			};
		public:
			QuectelGSMModemController(NN<IO::ATCommandChannel> channel, Bool needRelease);
			virtual ~QuectelGSMModemController();

			virtual UnsafeArrayOpt<UTF8Char> GetICCID(UnsafeArray<UTF8Char> sbuff);
			virtual UIntOS QueryCells(NN<Data::ArrayListNN<CellSignal>> cells);

			// (U)SIM Related Commands
			UnsafeArrayOpt<UTF8Char> QuectelGetICCID(UnsafeArray<UTF8Char> iccid); //AT+QCCID

			// Network Service Command
			UIntOS QuectelQueryServingCell(UnsafeArray<ServingCell> cell); // AT+QENG="servingcell"
			Bool QuectelQueryNeighbourCells(NN<Data::ArrayListNN<NeighbourCell>> cells); // AT+QENG="neighbourcell"
		};
	}
}
#endif
