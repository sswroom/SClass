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
			Text::String *wind;
			Text::String *weather;
			Int32 maxTemp;
			Int32 minTemp;
			Int32 maxRH;
			Int32 minRH;
			ForecastIcon weatherIcon;
			PSR psr;
		};

		struct WeatherForecast
		{
			Text::String *generalSituation;
			Data::Timestamp updateTime;
			Int32 seaTemp;
			Text::String *seaTempPlace;
			Data::Timestamp seaTempTime;
			Data::ArrayList<DayForecast*> forecast;
		};

		struct LocalForecast
		{
			Text::String *generalSituation;
			Text::String *tcInfo;
			Text::String *fireDangerWarning;
			Text::String *forecastPeriod;
			Text::String *forecastDesc;
			Text::String *outlook;
			Data::Timestamp updateTime;
		};

		static const Int32 INVALID_READING = -99;

		typedef void (__stdcall *UpdateHandler)(WeatherSignal updatedSignal);
	private:
		UpdateHandler hdlr;
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		Text::EncodingFactory *encFact;
		Net::RSSReader *rss;
		WeatherSignal currSignal;

		static WeatherSignal String2Signal(Text::String *textMessage);
	public:
		static WeatherSignal GetSignalSummary(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact);
		static Bool GetCurrentTempRH(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Int32 *temperature, Int32 *rh);
		static Bool GetWeatherForecast(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Language lang, WeatherForecast *weatherForecast);
		static void FreeWeatherForecast(WeatherForecast *weatherForecast);
		static Bool GetLocalForecast(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Language lang, LocalForecast *localForecast);
		static void FreeLocalForecast(LocalForecast *localForecast);

		HKOWeather(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, UpdateHandler hdlr);
		virtual ~HKOWeather();

		virtual void ItemAdded(Net::RSSItem *item);
		virtual void ItemRemoved(Net::RSSItem *item);
		WeatherSignal GetCurrentSignal();

		static PSR PSRParse(Text::CString psr);
		static Text::CString ForecastIconGetName(ForecastIcon icon);
		static Text::CString PSRGetName(PSR psr);
	};
}
#endif
