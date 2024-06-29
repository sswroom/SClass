#ifndef _SM_IO_ADVANTECHASCIICHANNEL
#define _SM_IO_ADVANTECHASCIICHANNEL
#include "AnyType.h"
#include "IO/Stream.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace IO
{
	class AdvantechASCIIChannel
	{
	public:
		enum TypeCode
		{
		TC_15mV = 0x00,
		TC_50mV = 0x01,
		TC_100mV = 0x02,
		TC_300mV = 0x03,
		TC_1000mV = 0x04,
		TC_2500mV = 0x05,
		TC_20mA = 0x06,
		TC_4_20mA = 0x07,
		TC_10V = 0x08,
		TC_5V = 0x09,
		TC_1V = 0x0A,
		TC_500mV_4017 = 0x0B,
		TC_150mV = 0x0C,
		TC_20mA_4017 = 0x0D,
		TC_TYPE_J_THERMOCOUPLE = 0x0E, //0-760 C
		TC_TYPE_K_THERMOCOUPLE = 0x0F, //0-13700 C
		TC_TYPE_T_THERMOCOUPLE = 0x10, //-100-400 C
		TC_TYPE_E_THERMOCOUPLE = 0x11, //-0-1000 C
		TC_TYPE_R_THERMOCOUPLE = 0x12, //500-1750 C
		TC_TYPE_S_THERMOCOUPLE = 0x13, //500-1750 C
		TC_TYPE_B_THERMOCOUPLE = 0x14, //500-1800 C
		TC_TYPE_N_THERMOCOUPLE = 0x18, //-200-1300 C
		TC_PLATINUM_100_IEC__50_150 = 0x20, //-50-150 C
		TC_PLATINUM_100_IEC_0_100 = 0x21, //0-100 C
		TC_PLATINUM_100_IEC_0_200 = 0x22, //0-200 C
		TC_PLATINUM_100_IEC_0_400 = 0x23, //0-400 C
		TC_PLATINUM_100_IEC__200_200 = 0x24, //-200-200 C
		TC_PLATINUM_100_JIS__50_150 = 0x25, //-50-150 C
		TC_PLATINUM_100_JIS_0_100 = 0x26, //0-100 C
		TC_PLATINUM_100_JIS_0_200 = 0x27, //0-200 C
		TC_PLATINUM_100_JIS_0_400 = 0x28, //0-400 C
		TC_PLATINUM_100_JIS__200_200 = 0x29, //-200-200 C
		TC_PLATINUM_1000 = 0x2A, //-40-160 C
		TC_BALCO_500 = 0x2B, //-30-120 C
		TC_NI_604__80_100 = 0x2C, //-80-100 CC
		TC_NI_604_0_100 = 0x2D, //0-100 C
		TC_THERMISTOR_3K = 0x30, //0-100 C
		TC_THERMISTOR_10K = 0x31, //0-100 C
		TC_0_20mA_4021 = 0x30, //0-20mA (4021)
		TC_4_20mA_4021 = 0x31, //4-20mA (4021)
		TC_0_10V_4021 = 0x32, //0-10V (4021)
		TC_NI_508 = 0x33, //-50-200 C
		TC_DIGITAL = 0x40,
		TC_0_100mV = 0x42,
		TC_0_500mV = 0x43,
		TC_0_1000mV = 0x44,
		TC_0_2500mV = 0x45,
		TC_0_10V = 0x48,
		TC_0_5V = 0x49,
		TC_0_1000mV_4017 = 0x4A,
		TC_0_500mV_4017 = 0x4B,
		TC_0_150mV = 0x4C,
		TC_0_20mA = 0x4D,
		TC_COUNTER = 0x50,
		TC_FREQUENCY = 0x51
		};

		enum BaudRate
		{
		BR_1200 = 3,
		BR_2400 = 4,
		BR_4800 = 5,
		BR_9600 = 6,
		BR_19200 = 7,
		BR_38400 = 8,
		BR_57600 = 9,
		BR_115200 = 10
		};

		enum IntegrationTime
		{
		IT_50MS,
		IT_60MS,
		IT_UNKNOWN
		};

		enum DataFormat
		{
		DF_ENGINEERING_UNIT,
		DF_PERCENT_FSR,
		DF_TWOS_COMPLEMENT,
		DF_OHMS,
		DF_UNKNOWN
		};

		enum SlewRate
		{
		SR_IMMEDIATE,
		SR_0_0625V_sec,
		SR_0_125V_sec,
		SR_0_25V_sec,
		SR_0_5V_sec,
		SR_1V_sec,
		SR_2V_sec,
		SR_4V_sec,
		SR_8V_sec,
		SR_16V_sec,
		SR_32V_sec,
		SR_64V_sec,
		SR_UNKNWON
		};

		enum FreqGateTime
		{
		FGT_100MS,
		FGT_1S,
		FGT_UNKNOWN
		};

		enum GateMode
		{
		GM_LOW,
		GM_HIGH,
		GM_DISABLE
		};

		struct ADAMConfig
		{
			UInt8 addr;
			TypeCode typeCode;
			BaudRate baudRateCode;
			Bool checksumEnabled;
			Bool modbusProtocol;
			IntegrationTime intTime;
			DataFormat dataFormat;
			SlewRate slewRate;
		};
	private:
		NN<IO::Stream> stm;
		Bool stmRelease;
		Sync::Event cmdEvt;
		Sync::Mutex cmdResMut;
		UnsafeArrayOpt<UTF8Char> cmdResBuff;
		UnsafeArrayOpt<UTF8Char> cmdResEnd;

		Bool threadRunning;
		Bool threadToStop;
	private:
		static UInt32 __stdcall CmdThread(AnyType userObj);
	public:
		AdvantechASCIIChannel(NN<IO::Stream> stm, Bool needRelease);
		~AdvantechASCIIChannel();

		NN<IO::Stream> GetStream() const;
		
		UnsafeArrayOpt<UTF8Char> SendCommand(UnsafeArray<UTF8Char> replyBuff, UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen);

		void Close();

		UnsafeArrayOpt<UTF8Char> GetFirmwareVer(UnsafeArray<UTF8Char> firmwareBuff, UInt8 addr);
		UnsafeArrayOpt<UTF8Char> GetModuleName(UnsafeArray<UTF8Char> moduleBuff, UInt8 addr);
		Bool GetConfigStatus(UInt8 addr, NN<ADAMConfig> config);
		Bool StoreCurrInputs();

		Bool AnalogOGetResetStatus(UInt8 addr, OutParam<Bool> hasReset);

		Bool DigitalSetConfig(UInt8 addr, UInt8 newAddr, BaudRate baudRate, Bool checksum, Bool modbus);
		Bool DigitalSetAllOutput8Ch(UInt8 addr, UInt8 outputs);
		Bool DigitalSetAllOutput16Ch(UInt8 addr, UInt16 outputs);
		Bool DigitalGetResetStatus(UInt8 addr, OutParam<Bool> hasReset);

		Bool ADAM4050GetIOStatus(UInt8 addr, OutParam<UInt16> outputs, OutParam<UInt16> inputs);
		Bool ADAM4051GetIOStatus(UInt8 addr, OutParam<UInt16> outputs, OutParam<UInt16> inputs);
		Bool ADAM4052GetIOStatus(UInt8 addr, OutParam<UInt16> outputs, OutParam<UInt16> inputs);
		Bool ADAM4053GetIOStatus(UInt8 addr, OutParam<UInt16> outputs, OutParam<UInt16> inputs);
		Bool ADAM4055GetIOStatus(UInt8 addr, OutParam<UInt16> outputs, OutParam<UInt16> inputs);
		Bool ADAM4056GetIOStatus(UInt8 addr, OutParam<UInt16> outputs, OutParam<UInt16> inputs);
		Bool ADAM4060GetIOStatus(UInt8 addr, OutParam<UInt16> outputs, OutParam<UInt16> inputs);
		Bool ADAM4068GetIOStatus(UInt8 addr, OutParam<UInt16> outputs, OutParam<UInt16> inputs);

		Bool ADAM4050GetStoredIO(UInt8 addr, OutParam<Bool> firstRead, OutParam<UInt16> outputs, OutParam<UInt16> inputs);
		Bool ADAM4051GetStoredIO(UInt8 addr, OutParam<Bool> firstRead, OutParam<UInt16> outputs, OutParam<UInt16> inputs);
		Bool ADAM4052GetStoredIO(UInt8 addr, OutParam<Bool> firstRead, OutParam<UInt16> outputs, OutParam<UInt16> inputs);
		Bool ADAM4053GetStoredIO(UInt8 addr, OutParam<Bool> firstRead, OutParam<UInt16> outputs, OutParam<UInt16> inputs);
		Bool ADAM4060GetStoredIO(UInt8 addr, OutParam<Bool> firstRead, OutParam<UInt16> outputs, OutParam<UInt16> inputs);
		Bool ADAM4068GetStoredIO(UInt8 addr, OutParam<Bool> firstRead, OutParam<UInt16> outputs, OutParam<UInt16> inputs);

		Bool ADAM4080SetConfig(UInt8 addr, UInt8 newAddr, Bool frequency, BaudRate baudRate, Bool checksum, FreqGateTime freqGateTime);
		Bool ADAM4080GetInputMode(UInt8 addr, OutParam<UInt8> inputMode);
		Bool ADAM4080SetInputMode(UInt8 addr, UInt8 inputMode);
		Bool ADAM4080GetValue(UInt8 addr, UInt8 channel, OutParam<UInt32> value);
		Bool ADAM4080SetGateMode(UInt8 addr, GateMode gateMode);
		Bool ADAM4080GetGateMode(UInt8 addr, OutParam<GateMode> gateMode);
		Bool ADAM4080SetMaxCounter(UInt8 addr, UInt8 channel, UInt32 maxCounter);
		Bool ADAM4080GetMaxCounter(UInt8 addr, UInt8 channel, OutParam<UInt32> maxCounter);
		Bool ADAM4080StartCounter(UInt8 addr, UInt8 channel);
		Bool ADAM4080StopCounter(UInt8 addr, UInt8 channel);
		Bool ADAM4080CounterIsStarted(UInt8 addr, UInt8 channel, OutParam<Bool> started);
		Bool ADAM4080ClearCounter(UInt8 addr, UInt8 channel);
		Bool ADAM4080CounterHasOverflow(UInt8 addr, UInt8 channel, OutParam<Bool> overflow);

		static UInt32 BaudRateGetBps(BaudRate baudRate);
		static Text::CStringNN DataFormatGetName(DataFormat dataFormat);
		static Text::CStringNN IntegrationTimeGetName(IntegrationTime intTime);
		static Text::CStringNN TypeCodeGetName(TypeCode typeCode);
		static Text::CStringNN SlewRateGetName(SlewRate slewRate);
	};
}
#endif
