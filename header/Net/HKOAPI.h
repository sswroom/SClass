#ifndef _SM_NET_HKOAPI
#define _SM_NET_HKOAPI
#include "Net/JSONResponse.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"

namespace Net
{
	class HKOAPI
	{
	public:
		enum class Language
		{
			En,
			TC,
			SC
		};

		JSONRESP_BEGIN(UnitValue)
		JSONRESP_STR("unit",false,false)
		JSONRESP_DOUBLE("value",false,false)
		JSONRESP_SEC_GET(UnitValue)
		JSONRESP_GETSTR("unit",GetUnit)
		JSONRESP_GETDOUBLE("value",GetValue,0.0)
		JSONRESP_END

		JSONRESP_BEGIN(SoilTemp)
		JSONRESP_OBJ("depth",false,false,UnitValue)
		JSONRESP_STR("place",false,false)
		JSONRESP_STR("recordTime",false,false)
		JSONRESP_STR("unit",false,false)
		JSONRESP_DOUBLE("value",false,false)
		JSONRESP_SEC_GET(SoilTemp)
		JSONRESP_GETOBJ("depth",GetDepth,UnitValue)
		JSONRESP_GETSTR("place",GetPlace)
		JSONRESP_GETSTR("recordTime",GetRecordTime)
		JSONRESP_GETSTR("unit",GetUnit)
		JSONRESP_GETDOUBLE("value",GetValue,0.0)
		JSONRESP_END

		JSONRESP_BEGIN(SeaTemp)
		JSONRESP_STR("place",false,false)
		JSONRESP_STR("recordTime",false,false)
		JSONRESP_STR("unit",false,false)
		JSONRESP_DOUBLE("value",false,false)
		JSONRESP_SEC_GET(SeaTemp)
		JSONRESP_GETSTR("place",GetPlace)
		JSONRESP_GETSTR("recordTime",GetRecordTime)
		JSONRESP_GETSTR("unit",GetUnit)
		JSONRESP_GETDOUBLE("value",GetValue,0.0)
		JSONRESP_END

		JSONRESP_BEGIN(PlaceUnitValue)
		JSONRESP_STR("place",false,false)
		JSONRESP_STR("unit",false,false)
		JSONRESP_DOUBLE("value",false,false)
		JSONRESP_SEC_GET(PlaceUnitValue)
		JSONRESP_GETSTR("place",GetPlace)
		JSONRESP_GETSTR("unit",GetUnit)
		JSONRESP_GETDOUBLE("value",GetValue,0.0)
		JSONRESP_END

		JSONRESP_BEGIN(HumidityData)
		JSONRESP_ARRAY_OBJ("data",false,false,PlaceUnitValue)
		JSONRESP_STR("recordTime",false,false)
		JSONRESP_SEC_GET(HumidityData)
		JSONRESP_GETARRAY_OBJ("data",GetData,PlaceUnitValue)
		JSONRESP_GETSTR("recordTime",GetRecordTime)
		JSONRESP_END

		JSONRESP_BEGIN(Rainfall)
		JSONRESP_STR("main",false,false)
		JSONRESP_DOUBLE("max",false,false)
		JSONRESP_STR("place",false,false)
		JSONRESP_STR("unit",false,false)
		JSONRESP_SEC_GET(Rainfall)
		JSONRESP_GETSTR("main",GetMain)
		JSONRESP_GETDOUBLE("max",GetMax,0.0)
		JSONRESP_GETSTR("place",GetPlace)
		JSONRESP_GETSTR("unit",GetUnit)
		JSONRESP_END

		JSONRESP_BEGIN(RainfallData)
		JSONRESP_ARRAY_OBJ("data",false,false,Rainfall)
		JSONRESP_STR("endTime",false,false)
		JSONRESP_STR("startTime",false,false)
		JSONRESP_SEC_GET(RainfallData)
		JSONRESP_GETARRAY_OBJ("data",GetData,Rainfall)
		JSONRESP_GETSTR("endTime",GetEndTime)
		JSONRESP_GETSTR("startTime",GetStartTime)
		JSONRESP_END

		JSONRESP_BEGIN(TemperatureData)
		JSONRESP_ARRAY_OBJ("data",false,false,PlaceUnitValue)
		JSONRESP_STR("recordTime",false,false)
		JSONRESP_SEC_GET(TemperatureData)
		JSONRESP_GETARRAY_OBJ("data",GetData,PlaceUnitValue)
		JSONRESP_GETSTR("recordTime",GetRecordTime)
		JSONRESP_END

		JSONRESP_BEGIN(UVIndex)
		JSONRESP_STR("place",false,false)
		JSONRESP_STR("desc",false,false)
		JSONRESP_DOUBLE("value",false,false)
		JSONRESP_SEC_GET(UVIndex)
		JSONRESP_GETSTR("place",GetPlace)
		JSONRESP_GETSTR("desc",GetDesc)
		JSONRESP_GETDOUBLE("value",GetValue,0.0)
		JSONRESP_END

		JSONRESP_BEGIN(UVIndexData)
		JSONRESP_ARRAY_OBJ("data",false,false,UVIndex)
		JSONRESP_STR("recordDesc",false,false)
		JSONRESP_SEC_GET(UVIndexData)
		JSONRESP_GETARRAY_OBJ("data",GetData,UVIndex)
		JSONRESP_GETSTR("recordDesc",GetRecordDesc)
		JSONRESP_END

		JSONRESP_BEGIN(WeatherForecast)
		JSONRESP_DOUBLE("ForecastIcon",false,false)
		JSONRESP_STR("PSR",false,false)
		JSONRESP_STR("forecastDate",false,false)
		JSONRESP_OBJ("forecastMaxrh",false,false,UnitValue)
		JSONRESP_OBJ("forecastMaxtemp",false,false,UnitValue)
		JSONRESP_OBJ("forecastMinrh",false,false,UnitValue)
		JSONRESP_OBJ("forecastMintemp",false,false,UnitValue)
		JSONRESP_STR("forecastWeather",false,false)
		JSONRESP_STR("forecastWind",false,false)
		JSONRESP_STR("week",false,false)
		JSONRESP_SEC_GET(WeatherForecast)
		JSONRESP_GETDOUBLE("ForecastIcon",GetForecastIcon,0.0)
		JSONRESP_GETSTR("PSR",GetPSR)
		JSONRESP_GETSTR("forecastDate",GetForecastDate)
		JSONRESP_GETOBJ("forecastMaxrh",GetForecastMaxrh,UnitValue)
		JSONRESP_GETOBJ("forecastMaxtemp",GetForecastMaxtemp,UnitValue)
		JSONRESP_GETOBJ("forecastMinrh",GetForecastMinrh,UnitValue)
		JSONRESP_GETOBJ("forecastMintemp",GetForecastMintemp,UnitValue)
		JSONRESP_GETSTR("forecastWeather",GetForecastWeather)
		JSONRESP_GETSTR("forecastWind",GetForecastWind)
		JSONRESP_GETSTR("week",GetWeek)
		JSONRESP_END

		JSONRESP_BEGIN(LocalWeatherForecast)
		JSONRESP_STR("fireDangerWarning",false,false)
		JSONRESP_STR("forecastDesc",false,false)
		JSONRESP_STR("forecastPeriod",false,false)
		JSONRESP_STR("generalSituation",false,false)
		JSONRESP_STR("outlook",false,false)
		JSONRESP_STR("tcInfo",false,false)
		JSONRESP_STR("updateTime",false,false)
		JSONRESP_SEC_GET(LocalWeatherForecast)
		JSONRESP_GETSTR("fireDangerWarning",GetFireDangerWarning)
		JSONRESP_GETSTR("forecastDesc",GetForecastDesc)
		JSONRESP_GETSTR("forecastPeriod",GetForecastPeriod)
		JSONRESP_GETSTR("generalSituation",GetGeneralSituation)
		JSONRESP_GETSTR("outlook",GetOutlook)
		JSONRESP_GETSTR("tcInfo",GetTcInfo)
		JSONRESP_GETSTR("updateTime",GetUpdateTime)
		JSONRESP_END

		JSONRESP_BEGIN(NineDayWeatherForecast)
		JSONRESP_STR("generalSituation",false,false)
		JSONRESP_OBJ("seaTemp",false,false,SeaTemp)
		JSONRESP_ARRAY_OBJ("soilTemp",false,false,SoilTemp)
		JSONRESP_STR("updateTime",false,false)
		JSONRESP_ARRAY_OBJ("weatherForecast",false,false,WeatherForecast)
		JSONRESP_SEC_GET(NineDayWeatherForecast)
		JSONRESP_GETSTR("generalSituation",GetGeneralSituation)
		JSONRESP_GETOBJ("seaTemp",GetSeaTemp,SeaTemp)
		JSONRESP_GETARRAY_OBJ("soilTemp",GetSoilTemp,SoilTemp)
		JSONRESP_GETSTR("updateTime",GetUpdateTime)
		JSONRESP_GETARRAY_OBJ("weatherForecast",GetWeatherForecast,WeatherForecast)
		JSONRESP_END

		JSONRESP_BEGIN(CurrentWeatherReport)
		JSONRESP_OBJ("humidity",false,false,HumidityData)
		JSONRESP_ARRAY_DOUBLE("icon",false,false)
		JSONRESP_STR("iconUpdateTime",false,false)
		JSONRESP_STR("mintempFrom00To09",false,false)
		JSONRESP_OBJ("rainfall",false,false,RainfallData)
		JSONRESP_STR("rainfallFrom00To12",false,false)
		JSONRESP_STR("rainfallJanuaryToLastMonth",false,false)
		JSONRESP_STR("rainfallLastMonth",false,false)
		JSONRESP_STR("tcmessage",false,false)
		JSONRESP_OBJ("temperature",false,false,TemperatureData)
		JSONRESP_STR("updateTime",false,false)
		JSONRESP_OBJ("uvindex",false,false,UVIndexData)
		JSONRESP_STR("warningMessage",false,false)
		JSONRESP_SEC_GET(CurrentWeatherReport)
		JSONRESP_GETOBJ("humidity",GetHumidity,HumidityData)
		JSONRESP_GETARRAY_DOUBLE("icon",GetIcon)
		JSONRESP_GETSTR("iconUpdateTime",GetIconUpdateTime)
		JSONRESP_GETSTR("mintempFrom00To09",GetMintempFrom00To09)
		JSONRESP_GETOBJ("rainfall",GetRainfall,RainfallData)
		JSONRESP_GETSTR("rainfallFrom00To12",GetRainfallFrom00To12)
		JSONRESP_GETSTR("rainfallJanuaryToLastMonth",GetRainfallJanuaryToLastMonth)
		JSONRESP_GETSTR("rainfallLastMonth",GetRainfallLastMonth)
		JSONRESP_GETSTR("tcmessage",GetTcmessage)
		JSONRESP_GETOBJ("temperature",GetTemperature,TemperatureData)
		JSONRESP_GETSTR("updateTime",GetUpdateTime)
		JSONRESP_GETOBJ("uvindex",GetUvindex,UVIndexData)
		JSONRESP_GETSTR("warningMessage",GetWarningMessage)
		JSONRESP_END

		JSONRESP_BEGIN(WarningSummary)
		JSONRESP_STR("name",false,false)
		JSONRESP_STR("code",false,false)
		JSONRESP_STR("actionCode",false,false)
		JSONRESP_STR("type",true,false)
		JSONRESP_STR("issueTime",false,false)
		JSONRESP_STR("updateTime",false,false)
		JSONRESP_STR("expireTime",true,false)
		JSONRESP_SEC_GET(WarningSummary)
		JSONRESP_GETSTR("name",GetName)
		JSONRESP_GETSTR("code",GetCode)
		JSONRESP_GETSTR("actionCode",GetActionCode)
		JSONRESP_GETSTROPT("type",GetType)
		JSONRESP_GETSTR("issueTime",GetIssueTime)
		JSONRESP_GETSTR("updateTime",GetUpdateTime)
		JSONRESP_GETSTROPT("expireTime",GetExpireTime)
		JSONRESP_END

		JSONRESP_BEGIN(WeatherWarningSummary)
		JSONRESP_OBJ("WFIRE",true,false,WarningSummary)
		JSONRESP_OBJ("WFROST",true,false,WarningSummary)
		JSONRESP_OBJ("WHOT",true,false,WarningSummary)
		JSONRESP_OBJ("WCOLD",true,false,WarningSummary)
		JSONRESP_OBJ("WMSGNL",true,false,WarningSummary)
		JSONRESP_OBJ("WRAIN",true,false,WarningSummary)
		JSONRESP_OBJ("WFNTSA",true,false,WarningSummary)
		JSONRESP_OBJ("WL",true,false,WarningSummary)
		JSONRESP_OBJ("WTCSGNL",true,false,WarningSummary)
		JSONRESP_OBJ("WTMW",true,false,WarningSummary)
		JSONRESP_OBJ("WTS",true,false,WarningSummary)
		JSONRESP_SEC_GET(WeatherWarningSummary)
		JSONRESP_GETOBJ("WFIRE",GetWFIRE,WarningSummary)
		JSONRESP_GETOBJ("WFROST",GetWFROST,WarningSummary)
		JSONRESP_GETOBJ("WHOT",GetWHOT,WarningSummary)
		JSONRESP_GETOBJ("WCOLD",GetWCOLD,WarningSummary)
		JSONRESP_GETOBJ("WMSGNL",GetWMSGNL,WarningSummary)
		JSONRESP_GETOBJ("WRAIN",GetWRAIN,WarningSummary)
		JSONRESP_GETOBJ("WFNTSA",GetWFNTSA,WarningSummary)
		JSONRESP_GETOBJ("WL",GetWL,WarningSummary)
		JSONRESP_GETOBJ("WTCSGNL",GetWTCSGNL,WarningSummary)
		JSONRESP_GETOBJ("WTMW",GetWTMW,WarningSummary)
		JSONRESP_GETOBJ("WTS",GetWTS,WarningSummary)
		JSONRESP_END

		JSONRESP_BEGIN(WeatherWarningInfo)
		JSONRESP_SEC_GET(WeatherWarningInfo)
		JSONRESP_END

		JSONRESP_BEGIN(WeatherTips)
		JSONRESP_STR("desc",true,false)
		JSONRESP_STR("updateTime",true,false)
		JSONRESP_SEC_GET(WeatherTips)
		JSONRESP_GETSTROPT("desc",GetDesc)
		JSONRESP_GETSTR("updateTime",GetUpdateTime)
		JSONRESP_END

		JSONRESP_BEGIN(SpecialWeatherTips)
		JSONRESP_ARRAY_OBJ("swt",false,false,WeatherTips)
		JSONRESP_SEC_GET(SpecialWeatherTips)
		JSONRESP_GETARRAY_OBJ("swt",GetData,WeatherTips)
		JSONRESP_END

		JSONRESP_BEGIN(QuickEarthquakeMessages)
		JSONRESP_DOUBLE("lat",false,false)
		JSONRESP_DOUBLE("lon",false,false)
		JSONRESP_DOUBLE("mag",false,false)
		JSONRESP_STR("ptime",false,false)
		JSONRESP_STR("region",false,false)
		JSONRESP_STR("updateTime",false,false)
		JSONRESP_SEC_GET(QuickEarthquakeMessages)
		JSONRESP_GETDOUBLE("lat",GetLat,0.0)
		JSONRESP_GETDOUBLE("lon",GetLon,0.0)
		JSONRESP_GETDOUBLE("mag",GetMag,0.0)
		JSONRESP_GETSTR("ptime",GetPtime)
		JSONRESP_GETSTR("region",GetRegion)
		JSONRESP_GETSTR("updateTime",GetUpdateTime)
		JSONRESP_END

		JSONRESP_BEGIN(LocallyFeltEarthTremorReport)
		JSONRESP_STR("details",false,false)
		JSONRESP_DOUBLE("intensity",false,false)
		JSONRESP_DOUBLE("lat",false,false)
		JSONRESP_DOUBLE("lon",false,false)
		JSONRESP_DOUBLE("mag",false,false)
		JSONRESP_STR("ptime",false,false)
		JSONRESP_STR("region",false,false)
		JSONRESP_STR("updateTime",false,false)
		JSONRESP_SEC_GET(LocallyFeltEarthTremorReport)
		JSONRESP_GETSTR("details",GetDetails)
		JSONRESP_GETDOUBLE("intensity",GetIntensity,0.0)
		JSONRESP_GETDOUBLE("lat",GetLat,0.0)
		JSONRESP_GETDOUBLE("lon",GetLon,0.0)
		JSONRESP_GETDOUBLE("mag",GetMag,0.0)
		JSONRESP_GETSTR("ptime",GetPtime)
		JSONRESP_GETSTR("region",GetRegion)
		JSONRESP_GETSTR("updateTime",GetUpdateTime)
		JSONRESP_END

		JSONRESP_BEGIN(LunarDate)
		JSONRESP_STR("LunarDate",false,false)
		JSONRESP_STR("LunarYear",false,false)
		JSONRESP_SEC_GET(LunarDate)
		JSONRESP_GETSTR("LunarDate",GetLunarDate)
		JSONRESP_GETSTR("LunarYear",GetLunarYear)
		JSONRESP_END

		JSONRESP_BEGIN(AutomaticWeatherStationRainfall)
		JSONRESP_STR("automaticWeatherStation",false,false)
		JSONRESP_STR("automaticWeatherStationID",false,false)
		JSONRESP_STR("unit",false,false)
		JSONRESP_STR("value",false,false)
		JSONRESP_SEC_GET(AutomaticWeatherStationRainfall)
		JSONRESP_GETSTR("automaticWeatherStation",GetAutomaticWeatherStation)
		JSONRESP_GETSTR("automaticWeatherStationID",GetAutomaticWeatherStationID)
		JSONRESP_GETSTR("unit",GetUnit)
		JSONRESP_GETSTR("value",GetValue)
		JSONRESP_END

		JSONRESP_BEGIN(HourlyRainfall)
		JSONRESP_ARRAY_OBJ("hourlyRainfall",false,false,AutomaticWeatherStationRainfall)
		JSONRESP_STR("obsTime",false,false)
		JSONRESP_SEC_GET(HourlyRainfall)
		JSONRESP_GETARRAY_OBJ("hourlyRainfall",GetHourlyRainfall,AutomaticWeatherStationRainfall)
		JSONRESP_GETSTR("obsTime",GetObsTime)
		JSONRESP_END

	public:
		static Optional<LocalWeatherForecast> GetLocalWeatherForecast(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang);
		static Optional<NineDayWeatherForecast> Get9DayWeatherForecast(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang);
		static Optional<CurrentWeatherReport> GetCurrentWeatherReport(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang);
		static Optional<WeatherWarningSummary> GetWeatherWarningSummary(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang);
		///////////////
		static Optional<WeatherWarningInfo> GetWeatherWarningInfo(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang);
		static Optional<SpecialWeatherTips> GetSpecialWeatherTips(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang);
		static Optional<QuickEarthquakeMessages> GetQuickEarthquakeMessages(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang);
		///////////////
		static Optional<LocallyFeltEarthTremorReport> GetLocallyFeltEarthTremorReport(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang);
		static Optional<LunarDate> GetLunarDate(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Data::Date date);
		static Optional<HourlyRainfall> GetHourlyRainfall(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang);
	};
}
#endif
