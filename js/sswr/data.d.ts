export function isArray(o: any): boolean;
export function isObject(o: any): boolean;
export function toObjectString(o: any, lev: number): string;
export function arrayBuffer2Base64(buff: any): string;
export function compare(a: any, b: any): number;

declare class DateValue
{
	year: number;
	month: number;
	day: number;

	constructor();
};

declare class TimeValue extends DateValue
{
	hour: number;
	minute: number;
	second: number;
	nanosec: number;
	tzQhr: number;
	
	constructor();
};

declare class DateTimeUtil
{
	static monString: string[];
	static monthString: string[];
	static dateValueSetDate(t: DateValue, dateStrs: string[]): void;
	static timeValueSetTime(t: TimeValue, timeStrs: string[]): void;
	static date2TotalDays(year: number, month: number, day: number): number;
	static dateValue2TotalDays(d: DateValue): number;
	static timeValue2Secs(tval: TimeValue): number;
	static timeValue2Ticks(t: TimeValue): number;
	static ticks2TimeValue(ticks: number, tzQhr?: number): TimeValue;
	static secs2TimeValue(secs: number, tzQhr?: number): TimeValue;
	static totalDays2DateValue(totalDays: number, d: DateValue): void;
	static instant2TimeValue(secs: number, nanosec: number, tzQhr?: number): TimeValue;
	static toString(tval: TimeValue, pattern: string): string;
	static string2TimeValue(dateStr: string, tzQhr?: number): TimeValue;
	static isYearLeap(year: number): boolean;
	static parseYearStr(year: string): number;
	static parseMonthStr(month: string): number;
	static getLocalTzQhr(): number;
};

declare class Duration
{
	seconds: number;
	ns: number;
	constructor(seconds: number, nanosec: number);
	static fromTicks(ticks: number): Duration;
	static fromUs(us: number): Duration;
	getSeconds(): number;
	getNS(): number;
	getTotalMS(): number;
	getTotalSec(): number;
	notZero(): boolean;
	isZero(): boolean;
	toString(): string;
}

declare class LocalDate
{
	static DATE_NULL: number = -1234567;

	dateVal: number;
	constructor();
	constructor(totalDays: number);
	constructor(dateVal: DateValue);
	constructor(year: number, month: number, day: number);
	constructor(dateStr: string);
	setValue(year: number, month: number, day: number): void;
	getDateValue(): DateValue;
	getTotalDays(): number;
	setYear(year: number): void;
	setMonth(month: number): void;
	setDay(day: number): void;
	isYearLeap(): boolean;
	toTicks(): number;
	toString(pattern: string | null): string;
	compareTo(obj: Date): number;
	isNull(): boolean;
}

declare class TimeInstant
{
	sec: number;
	nanosec: number;

	constructor(sec: number, nanosec: number);
	static now(): TimeInstant;
	static fromVariTime(variTime: number): TimeInstant;
	static fromTicks(ticks: number): TimeInstant;
	addDay(val: number): TimeInstant;
	addHour(val: number): TimeInstant;
	addMinute(val: number): TimeInstant;
	addSecond(val: number): TimeInstant;
	addMS(val: number): TimeInstant;
	addNS(val: number): TimeInstant;
	getMS(): number;
	clearTime(): TimeInstant;
	roundToS(): TimeInstant;
	getMSPassedDate(): number;
	diffMS(ts: TimeInstant): number;
	diffSec(ts: TimeInstant): number;
	diffSecDbl(ts: TimeInstant): number;
	diff(ts: TimeInstant): Duration;
	toTicks(): number;
	toDotNetTicks(): number;
	toUnixTimestamp(): number;
	toEpochSec(): number;
	toEpochMS(): number;
	toEpochNS(): number;
}

declare class Timestamp {
	constructor(inst: TimeInstant, tzQhr?: number);
	static fromTicks(ticks: number, tzQhr?: number): Timestamp;
	static fromStr(str: string, defTzQhr?: number): Timestamp;
	static now(): Timestamp;
	static utcNow(): Timestamp;
	static fromVariTime(variTime: any): Timestamp;
	static fromSecNS(unixTS: number, nanosec: number, tzQhr?: number): Timestamp;
	static fromDotNetTicks(ticks: number, tzQhr?: number): Timestamp;
	static fromEpochSec(epochSec: number, tzQhr?: number): Timestamp;
	static fromEpochMS(epochMS: number, tzQhr?: number): Timestamp;
	static fromEpochUS(epochUS: number, tzQhr?: number): Timestamp;
	static fromEpochNS(epochNS: number, tzQhr?: number): Timestamp;
	static fromTimeValue(tval: any): Timestamp;
	static fromYMDHMS(ymdhms: number, tzQhr?: number): Timestamp;
	addMonth(val: number): Timestamp;
	addYear(val: number): Timestamp;
	addDay(val: number): Timestamp;
	addHour(val: number): Timestamp;
	addMinute(val: number): Timestamp;
	addSecond(val: number): Timestamp;
	addMS(val: number): Timestamp;
	addNS(val: number): Timestamp;
	getMS(): number;
	clearTimeUTC(): Timestamp;
	clearTimeLocal(): Timestamp;
	clearMonthAndDay(): Timestamp;
	clearDayOfMonth(): Timestamp;
	getMSPassedUTCDate(): number;
	getMSPassedLocalDate(): number;
	diffSec(ts: Timestamp): number;
	diffMS(ts: Timestamp): number;
	diffSecDbl(ts: Timestamp): number;
	diff(ts: Timestamp): number;
	toTicks(): number;
	toDotNetTicks(): number;
	toUnixTimestamp(): number;
	toEpochSec(): number;
	toEpochMS(): number;
	toEpochNS(): number;
	toString(pattern: string | null): string;
	toStringISO8601(): string;
	toStringNoZone(): string;
	toUTCTime(): Timestamp;
	toLocalTime(): Timestamp;
	convertTimeZoneQHR(tzQhr: number): Timestamp;
	setTimeZoneQHR(tzQhr: number): Timestamp;
	getTimeZoneQHR(): number;
	sameDate(ts: Timestamp): boolean;
	toTimeValue(): any;
	roundToS(): Timestamp;
}
