#include "Stdafx.h"
#include "SSWR/SMonitor/SAnalogSensor.h"

Text::CStringNN SSWR::SMonitor::SAnalogSensor::GetReadingTypeName(ReadingType readingType)
{
	switch (readingType)
	{
	case RT_TEMPERATURE:
		return CSTR("Temperature");
	case RT_RHUMIDITY:
		return CSTR("Relative Humidity");
	case RT_DCVOLTAGE:
		return CSTR("DC Voltage");
	case RT_LATITUDE:
		return CSTR("Latitude");
	case RT_LONGITUDE:
		return CSTR("Longitude");
	case RT_FREQUENCY:
		return CSTR("Frequency");
	case RT_ONOFF:
		return CSTR("On/Off");
	case RT_POWER:
		return CSTR("Power On/Off");
	case RT_AIR_PM2_5:
		return CSTR("Air PM2.5");
	case RT_AIR_PM10:
		return CSTR("Air PM10");
	case RT_AIR_CO:
		return CSTR("Air CO");
	case RT_AIR_CO2:
		return CSTR("Air CO2");
	case RT_AIR_HCHO:
		return CSTR("Air HCHO");
	case RT_AIR_VOC:
		return CSTR("Air VOC");
	case RT_PARTICLE_0_3UM:
		return CSTR("Particle 0.3um");
	case RT_PARTICLE_0_5UM:
		return CSTR("Particle 0.5um");
	case RT_PARTICLE_1_0UM:
		return CSTR("Particle 1.0um");
	case RT_PARTICLE_2_5UM:
		return CSTR("Particle 2.5um");
	case RT_PARTICLE_5_0UM:
		return CSTR("Particle 5.0um");
	case RT_PARTICLE_10UM:
		return CSTR("Particle 10um");
	case RT_ALTITUDE:
		return CSTR("Altitude");
	case RT_DISTANCE:
		return CSTR("Distance");
	case RT_AHUMIDITY:
		return CSTR("Absolute Humidity");
	case RT_SYSTEMCURRENT:
		return CSTR("System Current");
	case RT_ENGINERPM:
		return CSTR("Engine RPM");
	case RT_COUNT:
		return CSTR("Count");
	case RT_ACTIVEPOWER:
		return CSTR("Active Power");
	case RT_APPARENTPOWER:
		return CSTR("Apparent Power");
	case RT_REACTIVEPOWER:
		return CSTR("Reactive Power");
	case RT_POWERFACTOR:
		return CSTR("Power Factor");
	case RT_PHASEANGLE:
		return CSTR("Phase Angle");
	case RT_IMPORTACTIVEENERGY:
		return CSTR("Import Active Energy");
	case RT_EXPORTACTIVEENERGY:
		return CSTR("Export Active Energy");
	case RT_TOTALACTIVEENERGY:
		return CSTR("Total Active Energy");
	case RT_IMPORTREACTIVEENERGY:
		return CSTR("Import Reactive Energy");
	case RT_EXPORTREACTIVEENERGY:
		return CSTR("Export Reactive Energy");
	case RT_TOTALREACTIVEENERGY:
		return CSTR("Total Reactive Energy");
	case RT_SYSTEMVOLTAGE: //V
		return CSTR("System Voltage");
	case RT_ACFREQUENCY: //Hz
		return CSTR("AC Frequency");
	case RT_RSSI: //dBm
		return CSTR("RSSI");
	case RT_GAS_CH4: //ppm
		return CSTR("Methane (CH4)");
	case RT_UNKNOWN:
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN SSWR::SMonitor::SAnalogSensor::GetSensorTypeName(SensorType sensorType)
{
	switch (sensorType)
	{
	case ST_CPU:
		return CSTR("CPU");
	case ST_DHT22:
		return CSTR("DHT22");
	case ST_DS18B20:
		return CSTR("DS18B20");
	case ST_GPS:
		return CSTR("GPS");
	case ST_GPSGPIO:
		return CSTR("GPS in GPIO");
	case ST_GPU:
		return CSTR("GPU");
	case ST_SNB:
		return CSTR("SNB");
	case ST_BATTERY:
		return CSTR("Battery");
	case ST_OBDII:
		return CSTR("OBD II");
	case ST_HTTPNUM:
		return CSTR("HTTP Number");
	case ST_SDM120M:
		return CSTR("SDM120M");
	case ST_GSMMODEM:
		return CSTR("GSM Modem");
	case ST_AMGU4241:
		return CSTR("Panasonic AMGU4241");
	case ST_TH10SB:
		return CSTR("TH10S-B");
	case ST_SNMP:
		return CSTR("SNMP");
	case ST_UNKNOWN:
	default:
		return CSTR("Unknown");
	}
}
