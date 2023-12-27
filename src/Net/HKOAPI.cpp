#include "Stdafx.h"
#include "Net/HKOAPI.h"

Optional<Net::HKOAPI::LocalWeatherForecast> Net::HKOAPI::GetLocalWeatherForecast(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang)
{
	Text::CStringNN url;
	switch (lang)
	{
	case Language::En:
	default:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=flw&lang=en");
		break;
	case Language::SC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=flw&lang=sc");
		break;
	case Language::TC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=flw&lang=tc");
		break;
	}
	JSONREQ_RET(sockf, ssl, url, LocalWeatherForecast)
}

Optional<Net::HKOAPI::NineDayWeatherForecast> Net::HKOAPI::Get9DayWeatherForecast(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang)
{
	Text::CStringNN url;
	switch (lang)
	{
	case Language::En:
	default:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=fnd&lang=en");
		break;
	case Language::SC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=fnd&lang=sc");
		break;
	case Language::TC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=fnd&lang=tc");
		break;
	}
	JSONREQ_RET(sockf, ssl, url, NineDayWeatherForecast)
}

Optional<Net::HKOAPI::CurrentWeatherReport> Net::HKOAPI::GetCurrentWeatherReport(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang)
{
	Text::CStringNN url;
	switch (lang)
	{
	case Language::En:
	default:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=rhrread&lang=en");
		break;
	case Language::SC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=rhrread&lang=sc");
		break;
	case Language::TC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=rhrread&lang=tc");
		break;
	}
	JSONREQ_RET(sockf, ssl, url, CurrentWeatherReport)
}

Optional<Net::HKOAPI::WeatherWarningSummary> Net::HKOAPI::GetWeatherWarningSummary(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang)
{
	Text::CStringNN url;
	switch (lang)
	{
	case Language::En:
	default:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=warnsum&lang=en");
		break;
	case Language::SC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=warnsum&lang=sc");
		break;
	case Language::TC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=warnsum&lang=tc");
		break;
	}
	JSONREQ_RET(sockf, ssl, url, WeatherWarningSummary)
}

Optional<Net::HKOAPI::WeatherWarningInfo> Net::HKOAPI::GetWeatherWarningInfo(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang)
{
	Text::CStringNN url;
	switch (lang)
	{
	case Language::En:
	default:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=warningInfo&lang=en");
		break;
	case Language::SC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=warningInfo&lang=sc");
		break;
	case Language::TC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=warningInfo&lang=tc");
		break;
	}
	JSONREQ_RET(sockf, ssl, url, WeatherWarningInfo)
}

Optional<Net::HKOAPI::SpecialWeatherTips> Net::HKOAPI::GetSpecialWeatherTips(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang)
{
	Text::CStringNN url;
	switch (lang)
	{
	case Language::En:
	default:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=swt&lang=en");
		break;
	case Language::SC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=swt&lang=sc");
		break;
	case Language::TC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=swt&lang=tc");
		break;
	}
	JSONREQ_RET(sockf, ssl, url, SpecialWeatherTips)
}

Optional<Net::HKOAPI::QuickEarthquakeMessages> Net::HKOAPI::GetQuickEarthquakeMessages(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang)
{
	Text::CStringNN url;
	switch (lang)
	{
	case Language::En:
	default:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/earthquake.php?dataType=qem&lang=en");
		break;
	case Language::SC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/earthquake.php?dataType=qem&lang=sc");
		break;
	case Language::TC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/earthquake.php?dataType=qem&lang=tc");
		break;
	}
	JSONREQ_RET(sockf, ssl, url, QuickEarthquakeMessages)
}

Optional<Net::HKOAPI::LocallyFeltEarthTremorReport> Net::HKOAPI::GetLocallyFeltEarthTremorReport(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang)
{
	Text::CStringNN url;
	switch (lang)
	{
	case Language::En:
	default:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/earthquake.php?dataType=feltearthquake&lang=en");
		break;
	case Language::SC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/earthquake.php?dataType=feltearthquake&lang=sc");
		break;
	case Language::TC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/earthquake.php?dataType=feltearthquake&lang=tc");
		break;
	}
	JSONREQ_RET(sockf, ssl, url, LocallyFeltEarthTremorReport)
}

Optional<Net::HKOAPI::LunarDate> Net::HKOAPI::GetLunarDate(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Data::Date date)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("https://data.weather.gov.hk/weatherAPI/opendata/lunardate.php?date="));
	sb.AppendDate(date);
	JSONREQ_RET(sockf, ssl, sb.ToCString(), LunarDate)
}

Optional<Net::HKOAPI::HourlyRainfall> Net::HKOAPI::GetHourlyRainfall(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Language lang)
{
	Text::CStringNN url;
	switch (lang)
	{
	case Language::En:
	default:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/hourlyRainfall.php?lang=en");
		break;
	case Language::SC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/hourlyRainfall.php?lang=sc");
		break;
	case Language::TC:
		url = CSTR("https://data.weather.gov.hk/weatherAPI/opendata/hourlyRainfall.php?lang=tc");
		break;
	}
	JSONREQ_RET(sockf, ssl, url, HourlyRainfall)
}
