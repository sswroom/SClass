import * as map from "./map.js";

export const Language = {
	English: "en",
	TChinese: "tc",
	SChinese: "sc"
}

export function getLayers()
{
	return [
		{"name": "Daily total rainfall", "type":map.WebMapType.WFS,		"url":"https://portal.csdi.gov.hk/server/services/common/hko_rcd_1634958957456_52030/MapServer/WFSServer"},
		{"name": "Daily total rainfall", "type":map.WebMapType.WMS,		"url":"https://portal.csdi.gov.hk/server/services/common/hko_rcd_1634958957456_52030/MapServer/WMSServer"},
		{"name": "Daily total rainfall", "type":map.WebMapType.ArcGIS,	"url":"https://portal.csdi.gov.hk/server/rest/services/common/hko_rcd_1634958957456_52030/FeatureServer"},
	];
}

export async function getLocalWeatherForecast(lang)
{
	var resp = await fetch("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=flw&lang="+lang);
	if (resp.ok)
	{
		return await resp.json();
	}
	return null;
}

export async function get9DayWeatherForecast(lang)
{
	var resp = await fetch("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=fnd&lang="+lang);
	if (resp.ok)
	{
		return await resp.json();
	}
	return null;
}

export async function getCurrentWeatherReport(lang)
{
	var resp = await fetch("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=rhrread&lang="+lang);
	if (resp.ok)
	{
		return await resp.json();
	}
	return null;
}

export async function getWeatherWarningSummary(lang)
{
	var resp = await fetch("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=warnsum&lang="+lang);
	if (resp.ok)
	{
		return await resp.json();
	}
	return null;
}

export async function getWeatherWarningInfo(lang)
{
	var resp = await fetch("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=warningInfo&lang="+lang);
	if (resp.ok)
	{
		return await resp.json();
	}
	return null;
}

export async function getSpecialWeatherTips(lang)
{
	var resp = await fetch("https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=swt&lang="+lang);
	if (resp.ok)
	{
		return await resp.json();
	}
	return null;
}

export async function getQuickEarthquakeMessages(lang)
{
	var resp = await fetch("https://data.weather.gov.hk/weatherAPI/opendata/earthquake.php?dataType=qem&lang="+lang);
	if (resp.ok)
	{
		return await resp.json();
	}
	return null;
}

export async function getLocallyFeltEarthTremorReport(lang)
{
	var resp = await fetch("https://data.weather.gov.hk/weatherAPI/opendata/earthquake.php?dataType=feltearthquake&lang="+lang);
	if (resp.ok)
	{
		return await resp.json();
	}
	return null;
}

export async function getLunarDate(date)
{
	var resp = await fetch("https://data.weather.gov.hk/weatherAPI/opendata/lunardate.php?date="+date.toString("yyyy-MM-dd"));
	if (resp.ok)
	{
		return await resp.json();
	}
	return null;
}

export async function getHourlyRainfall(lang)
{
	var resp = await fetch("https://data.weather.gov.hk/weatherAPI/opendata/hourlyRainfall.php?lang="+lang);
	if (resp.ok)
	{
		return await resp.json();
	}
	return null;
}
