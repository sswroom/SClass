#ifndef _SM_SSWR_SMONITOR_SANALOGSENSOR
#define _SM_SSWR_SMONITOR_SANALOGSENSOR
#include "Text/CString.h"

namespace SSWR
{
	namespace SMonitor
	{
		class SAnalogSensor
		{
		public:
			typedef enum
			{
				RT_UNKNOWN,
				RT_TEMPERATURE, //Degree Celsius
				RT_RHUMIDITY, //%
				RT_DCVOLTAGE, //V
				RT_LATITUDE, //Degree
				RT_LONGITUDE, //Degree
				RT_FREQUENCY, //Hz
				RT_ONOFF, //1 = on, 0 = off
				RT_POWER, //1 = on, 0 = off
				RT_AIR_PM2_5, //ppm
				RT_AIR_PM10, //ppm
				RT_AIR_CO, //ppm
				RT_AIR_CO2, //ppm
				RT_AIR_HCHO, //ppm
				RT_AIR_VOC, //ppm
				RT_PARTICLE_0_3UM, //ppm
				RT_PARTICLE_0_5UM, //ppm
				RT_PARTICLE_1_0UM, //ppm
				RT_PARTICLE_2_5UM, //ppm
				RT_PARTICLE_5_0UM, //ppm
				RT_PARTICLE_10UM, //ppm
				RT_ALTITUDE, //meter
				RT_DISTANCE, //meter
				RT_AHUMIDITY, //Pa
				RT_SYSTEMCURRENT, //A
				RT_ENGINERPM,
				RT_COUNT,
				RT_ACTIVEPOWER, //W
				RT_APPARENTPOWER, //VA
				RT_REACTIVEPOWER, //VAr
				RT_POWERFACTOR,
				RT_PHASEANGLE, //degree
				RT_IMPORTACTIVEENERGY, //kWh
				RT_EXPORTACTIVEENERGY, //kWh
				RT_TOTALACTIVEENERGY, //kWh
				RT_IMPORTREACTIVEENERGY, //kVArh
				RT_EXPORTREACTIVEENERGY, //kVArh
				RT_TOTALREACTIVEENERGY, //kVArh
				RT_SYSTEMVOLTAGE, //V
				RT_ACFREQUENCY, //Hz
				RT_RSSI, //dBm
				RT_GAS_CH4 //ppm
			} ReadingType;

			typedef enum
			{
				ST_UNKNOWN,
				ST_CPU,
				ST_DHT22,
				ST_DS18B20,
				ST_GPS,
				ST_GPSGPIO,
				ST_GPU,
				ST_SNB,
				ST_BATTERY,
				ST_OBDII,
				ST_HTTPNUM,
				ST_SDM120M,
				ST_GSMMODEM,
				ST_AMGU4241,
				ST_TH10SB,
				ST_SNMP
			} SensorType;
		public:
			virtual ~SAnalogSensor() {};

			virtual Bool IsError() = 0;
			virtual UOSInt GetMinInterval() = 0;
			virtual UOSInt GetReadingCnt() = 0;
			virtual ReadingType GetReadingType(UOSInt index) = 0;
			virtual Bool GetReadings(Double *values) = 0;
			virtual SensorType GetSensorType() = 0;

			static Text::CString GetReadingTypeName(ReadingType readingType);
			static Text::CString GetSensorTypeName(SensorType sensorType);
		};
	}
}
#endif
