#ifndef _SM_NET_HKOWEATHER
#define _SM_NET_HKOWEATHER
#include "Net/RSSReader.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/EncodingFactory.h"

namespace Net
{
	class HKOWeather : public Net::RSSHandler
	{
	public:
		enum class Language
		{
			En,
			TC,
			SC
		};

		enum class PSR
		{
			High,
			MediumHigh,
			Medium,
			MediumLow,
			Low
		};

		enum class WeatherWarning
		{
			None,
			WFIREY,
			WFIRER,
			WFROST,
			WHOT,
			WCOLD,
			WMSGNL,
			WRAINA,
			WRAINR,
			WRAINB,
			WFNTSA,
			WL,
			TC1,
			TC3,
			TC8NE,
			TC8SE,
			TC8NW,
			TC8SW,
			TC9,
			TC10,
			WTMW,
			WTS
		};

		enum class SignalAction
		{
			ISSUE,
			REISSUE,
			CANCEL,
			EXTEND,
			UPDATE
		};

		enum class ForecastIcon
		{
			Sunny = 50,
			SunnyPeriods = 51,
			SunnyIntervals = 52,
			SunnyPeriodsFewShowers = 53,
			SunnyIntervalsShowers = 54,
			Cloudy = 60,
			Overcast = 61,
			LightRain = 62,
			Rain = 63,
			HeavyRain = 64,
			Thunderstorms = 65,
			Night_1 = 70,
			Night_2 = 71,
			Night_3 = 72,
			Night_4 = 73,
			Night_5 = 74,
			Night_6 = 75,
			NightCloudy = 76,
			NightMainlyFine = 77,
			Windy = 80,
			Dry = 81,
			Humid = 82,
			Fog = 83,
			Mist = 84,
			Haze = 85,
			Hot = 90,
			Warm = 91,
			Cool = 92,
			Cold = 93
		};
	
		typedef enum
		{
			WS_NONE = 0,
			WS_TYPHOON_1 = 1,
			WS_TYPHOON_3 = 2,
			WS_TYPHOON_8NE = 3,
			WS_TYPHOON_8NW = 4,
			WS_TYPHOON_8SE = 5,
			WS_TYPHOON_8SW = 6,
			WS_TYPHOON_9 = 7,
			WS_TYPHOON_10 = 8,
			WS_TYPHOON_MASK = 15,
			WS_RAIN_YELLOW = 16,
			WS_RAIN_RED = 32,
			WS_RAIN_BLACK = 48,
			WS_RAIN_MASK = 0x70,
			WS_FIRE_YELLOW = 0x80,
			WS_FIRE_RED = 0x100,
			WS_FIRE_MASK = 0x180,
			WS_THUNDERSTORM = 0x200,
			WS_FLOODING = 0x400,
			WS_LANDSLIP = 0x800,
			WS_STRONGMONSOON = 0x1000,
			WS_FROST = 0x2000,
			WS_COLD = 0x4000,
			WS_VERYHOT = 0x8000,
			WS_TSUNAMI = 0x10000,

			WS_UNKNOWN = 0xffffffff
		} WeatherSignal;

		struct DayForecast
		{
			Data::DateTimeUtil::DateValue date;
			Data::DateTimeUtil::Weekday weekday;
			NN<Text::String> wind;
			NN<Text::String> weather;
			Int32 maxTemp;
			Int32 minTemp;
			Int32 maxRH;
			Int32 minRH;
			ForecastIcon weatherIcon;
			PSR psr;
		};

		struct WeatherForecast
		{
			Optional<Text::String> generalSituation;
			Data::Timestamp updateTime;
			Int32 seaTemp;
			Optional<Text::String> seaTempPlace;
			Data::Timestamp seaTempTime;
			Data::ArrayListNN<DayForecast> forecast;
		};

		struct LocalForecast
		{
			Optional<Text::String> generalSituation;
			Optional<Text::String> tcInfo;
			Optional<Text::String> fireDangerWarning;
			Optional<Text::String> forecastPeriod;
			Optional<Text::String> forecastDesc;
			Optional<Text::String> outlook;
			Data::Timestamp updateTime;
		};

		struct WarningSummary
		{
			WeatherWarning code;
			SignalAction actionCode;
			Data::Timestamp issueTime;
			Data::Timestamp updateTime;
			Data::Timestamp expireTime;
		};

		static const Int32 INVALID_READING = -99;

		typedef void (__stdcall *UpdateHandler)(WeatherSignal updatedSignal);
	private:
		UpdateHandler hdlr;
		NN<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		NN<Text::EncodingFactory> encFact;
		Net::RSSReader *rss;
		WeatherSignal currSignal;
		NN<IO::LogTool> log;

		static WeatherSignal String2Signal(Text::String *textMessage);
	public:
		static WeatherSignal GetSignalSummary(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, NN<Text::EncodingFactory> encFact);
		static Bool GetCurrentTempRH(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, OutParam<Int32> temperature, OutParam<Int32> rh, NN<IO::LogTool> log);
		static Bool GetWeatherForecast(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang, NN<WeatherForecast> weatherForecast);
		static void FreeWeatherForecast(NN<WeatherForecast> weatherForecast);
		static Bool GetLocalForecast(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang, NN<LocalForecast> localForecast);
		static void FreeLocalForecast(NN<LocalForecast> localForecast);
		static Bool GetWarningSummary(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, NN<Data::ArrayListNN<WarningSummary>> warnings);
		static void FreeWarningSummary(NN<Data::ArrayListNN<WarningSummary>> warnings);

		HKOWeather(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, NN<Text::EncodingFactory> encFact, UpdateHandler hdlr, NN<IO::LogTool> log);
		virtual ~HKOWeather();

		virtual void ItemAdded(NN<Net::RSSItem> item);
		virtual void ItemRemoved(NN<Net::RSSItem> item);
		WeatherSignal GetCurrentSignal();

		static PSR PSRParse(Text::CStringNN psr);
		static Text::CStringNN ForecastIconGetName(ForecastIcon icon);
		static Text::CStringNN PSRGetName(PSR psr);
		static WeatherWarning WeatherWarningParse(Text::CStringNN warning);
		static Text::CStringNN WeatherWarningGetCode(WeatherWarning warning);
		static Text::CStringNN WeatherWarningGetName(WeatherWarning warning);
		static SignalAction SignalActionParse(Text::CStringNN action);
		static Text::CStringNN SignalActionGetName(SignalAction action);
	};
}
#endif
