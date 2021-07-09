#include "Stdafx.h"
#include "SSWR/SMonitor/SAnalogSensor.h"

const UTF8Char *SSWR::SMonitor::SAnalogSensor::GetReadingTypeName(ReadingType readingType)
{
	switch (readingType)
	{
	case RT_TEMPERATURE:
		return (const UTF8Char*)"Temperature";
	case RT_RHUMIDITY:
		return (const UTF8Char*)"Relative Humidity";
	case RT_DCVOLTAGE:
		return (const UTF8Char*)"DC Voltage";
	case RT_LATITUDE:
		return (const UTF8Char*)"Latitude";
	case RT_LONGITUDE:
		return (const UTF8Char*)"Longitude";
	case RT_FREQUENCY:
		return (const UTF8Char*)"Frequency";
	case RT_ONOFF:
		return (const UTF8Char*)"On/Off";
	case RT_POWER:
		return (const UTF8Char*)"Power On/Off";
	case RT_AIR_PM2_5:
		return (const UTF8Char*)"Air PM2.5";
	case RT_AIR_PM10:
		return (const UTF8Char*)"Air PM10";
	case RT_AIR_CO:
		return (const UTF8Char*)"Air CO";
	case RT_AIR_CO2:
		return (const UTF8Char*)"Air CO2";
	case RT_AIR_HCHO:
		return (const UTF8Char*)"Air HCHO";
	case RT_AIR_VOC:
		return (const UTF8Char*)"Air VOC";
	case RT_PARTICLE_0_3UM:
		return (const UTF8Char*)"Particle 0.3um";
	case RT_PARTICLE_0_5UM:
		return (const UTF8Char*)"Particle 0.5um";
	case RT_PARTICLE_1_0UM:
		return (const UTF8Char*)"Particle 1.0um";
	case RT_PARTICLE_2_5UM:
		return (const UTF8Char*)"Particle 2.5um";
	case RT_PARTICLE_5_0UM:
		return (const UTF8Char*)"Particle 5.0um";
	case RT_PARTICLE_10UM:
		return (const UTF8Char*)"Particle 10um";
	case RT_ALTITUDE:
		return (const UTF8Char*)"Altitude";
	case RT_DISTANCE:
		return (const UTF8Char*)"Distance";
	case RT_AHUMIDITY:
		return (const UTF8Char*)"Absolute Humidity";
	case RT_SYSTEMCURRENT:
		return (const UTF8Char*)"System Current";
	case RT_ENGINERPM:
		return (const UTF8Char*)"Engine RPM";
	case RT_COUNT:
		return (const UTF8Char*)"Count";
	case RT_ACTIVEPOWER:
		return (const UTF8Char*)"Active Power";
	case RT_APPARENTPOWER:
		return (const UTF8Char*)"Apparent Power";
	case RT_REACTIVEPOWER:
		return (const UTF8Char*)"Reactive Power";
	case RT_POWERFACTOR:
		return (const UTF8Char*)"Power Factor";
	case RT_PHASEANGLE:
		return (const UTF8Char*)"Phase Angle";
	case RT_IMPORTACTIVEENERGY:
		return (const UTF8Char*)"Import Active Energy";
	case RT_EXPORTACTIVEENERGY:
		return (const UTF8Char*)"Export Active Energy";
	case RT_TOTALACTIVEENERGY:
		return (const UTF8Char*)"Total Active Energy";
	case RT_IMPORTREACTIVEENERGY:
		return (const UTF8Char*)"Import Reactive Energy";
	case RT_EXPORTREACTIVEENERGY:
		return (const UTF8Char*)"Export Reactive Energy";
	case RT_TOTALREACTIVEENERGY:
		return (const UTF8Char*)"Total Reactive Energy";
	case RT_SYSTEMVOLTAGE: //V
		return (const UTF8Char*)"System Voltage";
	case RT_ACFREQUENCY: //Hz
		return (const UTF8Char*)"AC Frequency";
	case RT_RSSI: //dBm
		return (const UTF8Char*)"RSSI";
	case RT_GAS_CH4: //ppm
		return (const UTF8Char*)"Methane (CH4)";
	case RT_UNKNOWN:
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *SSWR::SMonitor::SAnalogSensor::GetSensorTypeName(SensorType sensorType)
{
	switch (sensorType)
	{
	case ST_CPU:
		return (const UTF8Char*)"CPU";
	case ST_DHT22:
		return (const UTF8Char*)"DHT22";
	case ST_DS18B20:
		return (const UTF8Char*)"DS18B20";
	case ST_GPS:
		return (const UTF8Char*)"GPS";
	case ST_GPSGPIO:
		return (const UTF8Char*)"GPS in GPIO";
	case ST_GPU:
		return (const UTF8Char*)"GPU";
	case ST_SNB:
		return (const UTF8Char*)"SNB";
	case ST_BATTERY:
		return (const UTF8Char*)"Battery";
	case ST_OBDII:
		return (const UTF8Char*)"OBD II";
	case ST_HTTPNUM:
		return (const UTF8Char*)"HTTP Number";
	case ST_SDM120M:
		return (const UTF8Char*)"SDM120M";
	case ST_GSMMODEM:
		return (const UTF8Char*)"GSM Modem";
	case ST_AMGU4241:
		return (const UTF8Char*)"Panasonic AMGU4241";
	case ST_TH10SB:
		return (const UTF8Char*)"TH10S-B";
	case ST_SNMP:
		return (const UTF8Char*)"SNMP";
	case ST_UNKNOWN:
	default:
		return (const UTF8Char*)"Unknown";
	}
}
