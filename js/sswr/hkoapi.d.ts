import * as data from "./data";
import * as map from "./map";

export enum Language {
	English,
	TChinese,
	SChinese
};

declare class WebLayerInfo
{
	name: string;
	url: string;
	type: map.WebMapType;
};

declare class UnitValue
{
	unit: string;
	value: number;
};

declare class SoilTemp
{
	depth: UnitValue;
	place: string;
	recordTime: string;
	unit: string;
	value: number;
};

declare class SeaTemp
{
	place: string;
	recordTime: string;
	unit: string;
	value: number;
};

declare class PlaceUnitValue
{
	place: string;
	unit: string;
	value: number;
};

declare class HumidityData
{
	data: PlaceUnitValue[];
	recordTime: string;
};

declare class Rainfall
{
	main: string;
	max: number;
	place: string;
	unit: string;
};

declare class RainfallData
{
	data: Rainfall[];
	endTime: string;
	startTime: string;
};

declare class TemperatureData
{
	data: PlaceUnitValue[];
	recordTime: string;
};

declare class UVIndex
{
	place: string;
	desc: string;
	value: number;
};

declare class UVIndexData
{
	data: UVIndex[];
	recordDesc: string;
};

declare class WeatherForecast
{
	ForecastIcon: number;
	PSR: string;
	forecastDate: string;
	forecastMaxrh: UnitValue;
	forecastMaxtemp: UnitValue;
	forecastMinrh: UnitValue;
	forecastMintemp: UnitValue;
	forecastWeather: string;
	forecastWind: string;
	week: string;
};

declare class LocalWeatherForecast
{
	fireDangerWarning: string;
	forecastDesc: string;
	forecastPeriod: string;
	generalSituation: string;
	outlook: string;
	tcInfo: string;
	updateTime: string;
};

declare class NineDayWeatherForecast
{
	generalSituation: string;
	seaTemp: SeaTemp;
	soilTemp: SoilTemp;
	updateTime: string;
	weatherForecast: WeatherForecast[];
};

declare class CurrentWeatherReport
{
	humidity: HumidityData;
	icon: number[];
	iconUpdateTime: string;
	mintempFrom00To09: string;
	rainfall: RainfallData;
	rainfallFrom00To12: string;
	rainfallJanuaryToLastMonth: string;
	rainfallLastMonth: string;
	tcmessage: string;
	temperature: TemperatureData;
	updateTime: string;
	uvindex: UVIndexData;
	warningMessage: string;
};

declare class WarningSummary
{
	name: string;
	code: string;
	actionCode: string;
	type?: string;
	issueTime: string;
	updateTime: string;
	expireTime?: string;
};

declare class WeatherWarningSummary
{
	WFIRE: WarningSummary;
	WFROST: WarningSummary;
	WHOT: WarningSummary;
	WCOLD: WarningSummary;
	WMSGNL: WarningSummary;
	WRAIN: WarningSummary;
	WFNTSA: WarningSummary;
	WL: WarningSummary;
	WTCSGNL: WarningSummary;
	WTMW: WarningSummary;
	WTS: WarningSummary;
};

declare class WeatherWarningInfo
{

};

declare class WeatherTips
{
	desc?: string;
	updateTime?: string;
};

declare class SpecialWeatherTips
{
	swt: WeatherTips[];
};

declare class QuickEarthquakeMessages
{
	lat: number;
	lon: number;
	mag: number;
	ptime: string;
	region: string;
	updateTime: string;
};

declare class LocallyFeltEarthTremorReport
{
	details: string;
	intensity: number;
	lat: number;
	lon: number;
	mag: number;
	ptime: string;
	region: string;
	updateTime: string;
};

declare class LunarDate
{
	LunarDate: string;
	LunarYear: string;
};

declare class AutomaticWeatherStationRainfall
{
	automaticWeatherStation: string;
	automaticWeatherStationID: string;
	unit: string;
	value: string;
};

declare class HourlyRainfall
{
	hourlyRainfall: AutomaticWeatherStationRainfall[];
	obsTime: string;
};

export function getLayers() : WebLayerInfo[];
export function getLocalWeatherForecast(lang: Language) : Promise<LocalWeatherForecast | null>;
export function get9DayWeatherForecast(lang: Language): Promise<NineDayWeatherForecast | null>;
export function getCurrentWeatherReport(lang: Language): Promise<CurrentWeatherReport | null>;
export function getWeatherWarningSummary(lang: Language): Promise<WeatherWarningSummary | null>;
export function getWeatherWarningInfo(lang: Language): Promise<WeatherWarningInfo | null>;
export function getSpecialWeatherTips(lang: Language): Promise<SpecialWeatherTips | null>;
export function getQuickEarthquakeMessages(lang: Language): Promise<QuickEarthquakeMessages | null>;
export function getLocallyFeltEarthTremorReport(lang: Language): Promise<LocallyFeltEarthTremorReport | null>;
export function getLunarDate(date: data.LocalDate): Promise<LunarDate | null>;
export function getHourlyRainfall(lang: Language): Promise<HourlyRainfall | null>;
