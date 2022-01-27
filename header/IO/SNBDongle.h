#ifndef _SM_IO_SNBDONGLE
#define _SM_IO_SNBDONGLE
#include "Data/UInt64Map.h"
#include "IO/SNBProtocol.h"
#include "Sync/RWMutex.h"
#include "Text/CString.h"

namespace IO
{
	class SNBDongle
	{
	public:
		typedef enum
		{
			RT_UNKNOWN = 0,
			RT_TEMPERATURE = 1,
			RT_GAS_CH4 = 2,
			RT_DOOR_CONTACT = 3,
			RT_MOVING_SENSOR = 4,
			RT_AIR_CO = 0x21,
			RT_AIR_PM25 = 0x22,
			RT_AIR_PM10 = 0x23,
			RT_AIR_HCHO = 0x24,
			RT_AIR_VOC = 0x25,
			RT_AIR_CO2 = 0x26,
			RT_AIR_HUMIDITY = 0x27,
			RT_PARTICLE_0_3UM = 0x2C,
			RT_PARTICLE_0_5UM = 0x2D,
			RT_PARTICLE_1_0UM = 0x2E,
			RT_PARTICLE_2_5UM = 0x2F,
			RT_PARTICLE_5_0UM = 0x30,
			RT_PARTICLE_10UM = 0x31,
			RT_VOLTAGE = 0x80,
			RT_ONOFF = 0x81,
			RT_POWER = 0x82,
			RT_ALERT = 0x83
		} ReadingType;

		typedef enum
		{
			ST_UNKNOWN = 0xFE,
			ST_LUX = 0,
			ST_HUMIDITY_TEMP = 1,
			ST_AIR = 2,
			ST_DOOR_CONTACT = 3,
			ST_MOTION = 4,
			ST_VIBRATE = 5,
			ST_POSITION = 6,
			ST_PANIC_BUTTON = 7,
			ST_WAKEUP = 8,
			ST_DOOROPEN = 9,
			ST_WATERSENSOR = 0x0A,
			ST_SENSOR = 0xFA,
			ST_TEMPERATURE = 0xFB,
			ST_RETURN_NORMAL = 0xFC,
			ST_CUSTOM = 0xFE
		} SensorType;

		typedef enum
		{
			HT_UNKNOWN = 0,
			HT_DOOR = 1,
			HT_TEMPERATURE1 = 2,
			HT_TEMPERATURE2 = 3,
			HT_MOTION = 4,
			HT_TEMPERATURE2V2 = 5,
			HT_MOBILEPLUG = 6,
			HT_AIR_QUALITY = 7,
			HT_WATERLEAKAGE = 8,
			HT_LAST = HT_WATERLEAKAGE
		} HandleType;

		typedef enum
		{
			DT_UNKNOWN = 0,
			DT_REMOTE = 1,
			DT_SWITCH = 2,
			DT_CURTAIN = 3,
			DT_DIMMER = 4,
			DT_SENSOR = 5,
			DT_CONVERTER = 6,
			DT_ALARM = 7,
			DT_SERVER_DONGLE = 8,
			DT_LOCK_CONTROLLER = 9,
			DT_IP_CAMERA = 10,
			DT_PLUG = 11,
			DT_RF_UART = 12
		} DeviceType;

		class SNBHandler
		{
		public:
			virtual void DeviceAdded(UInt64 devId) = 0;
			virtual void DeviceSensor(UInt64 devId, SensorType sensorType, UOSInt nReading, ReadingType *readingTypes, Double *readingVals) = 0;
			virtual void DeviceUpdated(UInt64 devId, UInt16 shortAddr) = 0;
			virtual void DongleInfoUpdated() = 0;
		};

		typedef struct
		{
			UInt64 devId;
			UInt16 shortAddr;
			DeviceType devType;
			HandleType handType;
			UInt8 sensorCount;
			UInt8 versionMajor;
			UInt8 versionMinor;
			Int16 param;
			UInt8 sensorType;
		} DeviceInfo;
	public:
		IO::SNBProtocol *proto;
		IO::SNBProtocol::ProtocolHandler protoHdlr;
		void *protoObj;
		SNBHandler *hdlr;
		Data::UInt64Map<DeviceInfo*> *devMap;
		Sync::RWMutex *devMut;
		UInt64 dongleId;
		UInt32 dongleBaudRate;

		static void __stdcall OnProtocolRecv(void *userObj, UInt8 cmdType, UOSInt cmdSize, UInt8 *cmd);
		DeviceInfo *GetDevice(UInt64 devId);
	public:
		SNBDongle(IO::Stream *stm, SNBHandler *hdlr);
		~SNBDongle();

		void SetDevHandleType(UInt64 devId, HandleType handType);
		void SetDevShortAddr(UInt64 devId, UInt16 shortAddr);
		UInt64 GetDongleId();
		UInt32 GetBaudRate();

		void HandleProtocolReceived(IO::SNBProtocol::ProtocolHandler protoHdlr, void *userObj);
		void SendCheckDongle();
		void SendGetDongleInfo();
		void SendCheckDevices();
		void SendResetNetwork();
		void SendAddDevice(UInt8 timeout);
		void SendSetReportTime(UInt64 devId, Int32 interval); //1 = 10 seconds
		void SendGetReportTime(UInt64 devId);

		Bool SendDevTurnOn(UInt64 devId);
		Bool SendDevTurnOff(UInt64 devId);
		Bool SendDevGetStatus(UInt64 devId);

		static Text::CString GetHandleName(HandleType handType);
		static Text::CString GetReadingName(ReadingType readingType);
		static Text::CString SensorTypeGetName(SensorType sensorType);
	};
}

#endif
