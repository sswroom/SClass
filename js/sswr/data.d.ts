export enum Weekday
{
	Sunday,
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday
}

export function isArray(o: any): boolean;
export function isObject(o: any): boolean;
export function toObjectString(o: any, lev: number): string;
export function arrayBuffer2Base64(buff: ArrayBuffer): string;
export function arrayBufferEquals(buff1: ArrayBuffer, buff2: ArrayBuffer): boolean;
export function compare(a: any, b: any): number;
export function sort(arr: object[], compareFunc?: (val1: object, val2: object) => number, firstIndex?: number, lastIndex?: number): void;
export function mergeOptions(options: object | null, defOptions: object): object;
export function readUInt16(arr: Uint8Array, index: number): number;
export function readMUInt16(arr: Uint8Array, index: number): number;
export function readUInt24(arr: Uint8Array, index: number): number;
export function readMUInt24(arr: Uint8Array, index: number): number;
export function readUInt32(arr: Uint8Array, index: number): number;
export function readMUInt32(arr: Uint8Array, index: number): number;
export function rol32(v: number, n: number): number;
export function ror32(v: number, n: number): number;
export function shl32(v: number, n: number): number;
export function sar32(v: number, n: number): number;
export function shr32(v: number, n: number): number;
export function objectParseTS(o: object, items: string[]): void;

export class DateValue
{
	year: number;
	month: number;
	day: number;

	constructor();
}

export class TimeValue extends DateValue
{
	hour: number;
	minute: number;
	second: number;
	nanosec: number;
	tzQhr: number;
	
	constructor();
}

export class DateTimeUtil
{
	static monString: string[];
	static monthString: string[];
	static dateValueSetDate(t: DateValue, dateStrs: string[]): void;
	static timeValueSetTime(t: TimeValue, timeStrs: string[]): void;
	static date2TotalDays(year: number, month: number, day: number): number;
	static dateValue2TotalDays(d: DateValue): number;
	static timeValue2Secs(tval: TimeValue): bigint;
	static timeValue2Ticks(t: TimeValue): bigint;
	static ticks2TimeValue(ticks: number, tzQhr?: number): TimeValue;
	static secs2TimeValue(secs: bigint, tzQhr?: number): TimeValue;
	static totalDays2DateValue(totalDays: number, d: DateValue): void;
	static instant2TimeValue(secs: bigint, nanosec: number, tzQhr?: number): TimeValue;
	static ticks2Weekday(ticks: number, tzQhr: number): Weekday;
	static toString(tval: TimeValue, pattern: string): string;
	static string2TimeValue(dateStr: string, tzQhr?: number): TimeValue;
	static isYearLeap(year: number): boolean;
	static parseYearStr(year: string): number;
	static parseMonthStr(month: string): number;
	static getLocalTzQhr(): number;
}

export class Duration
{
	seconds: bigint;
	ns: number;
	constructor(seconds: number | bigint, nanosec: number);
	static fromTicks(ticks: number): Duration;
	static fromUs(us: bigint): Duration;
	getSeconds(): bigint;
	getNS(): number;
	getTotalMS(): number;
	getTotalSec(): number;
	notZero(): boolean;
	isZero(): boolean;
	toString(): string;
}

export class LocalDate
{
	static DATE_NULL: number;

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
	getWeekday(): Weekday;
	static today(): LocalDate;
	static fromStr(s: string): LocalDate | null;
}

export class TimeInstant
{
	sec: bigint;
	nanosec: number;

	constructor(sec: bigint | number, nanosec: number);
	static now(): TimeInstant;
	static fromVariTime(variTime: bigint | number): TimeInstant;
	static fromTicks(ticks: bigint | number): TimeInstant;
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
	toDotNetTicks(): bigint;
	toUnixTimestamp(): number;
	toEpochSec(): bigint;
	toEpochMS(): bigint;
	toEpochNS(): bigint;
}

export class Timestamp {
	inst: TimeInstant;
	tzQhr: number;

	constructor(inst: TimeInstant, tzQhr?: number);
	static fromTicks(ticks: number | number, tzQhr?: number): Timestamp;
	static fromStr(str: string, defTzQhr?: number): Timestamp | null;
	static now(): Timestamp;
	static utcNow(): Timestamp;
	static fromVariTime(variTime: any): Timestamp;
	static fromSecNS(unixTS: number, nanosec: number, tzQhr?: number): Timestamp;
	static fromDotNetTicks(ticks: bigint, tzQhr?: number): Timestamp;
	static fromEpochSec(epochSec: number | bigint, tzQhr?: number): Timestamp;
	static fromEpochMS(epochMS: number | bigint, tzQhr?: number): Timestamp;
	static fromEpochUS(epochUS: number | bigint, tzQhr?: number): Timestamp;
	static fromEpochNS(epochNS: number | bigint, tzQhr?: number): Timestamp;
	static fromTimeValue(tval: TimeValue): Timestamp;
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
	toDate(): LocalDate;
	toTicks(): number;
	toDotNetTicks(): number;
	toUnixTimestamp(): number;
	toEpochSec(): bigint;
	toEpochMS(): bigint;
	toEpochNS(): bigint;
	toString(pattern?: string): string;
	toStringISO8601(): string;
	toStringNoZone(): string;
	toUTCTime(): Timestamp;
	toLocalTime(): Timestamp;
	convertTimeZoneQHR(tzQhr: number): Timestamp;
	setTimeZoneQHR(tzQhr: number): Timestamp;
	getTimeZoneQHR(): number;
	sameDate(ts: Timestamp): boolean;
	toTimeValue(): TimeValue;
	roundToS(): Timestamp;
}

export class ByteReader
{
	view: DataView;

	constructor(arr: ArrayBuffer);
	getLength(): number;
	getArrayBuffer(ofst?: number, size?: number): ArrayBuffer;
	getU8Arr(ofst?: number, size?: number): Uint8Array;

	readUInt8(ofst: number): number;
	readUInt16(ofst: number, lsb: boolean): number;
	readUInt24(ofst: number, lsb: boolean): number;
	readUInt32(ofst: number, lsb: boolean): number;
	readInt8(ofst: number): number;
	readInt16(ofst: number, lsb: boolean): number;
	readInt24(ofst: number, lsb: boolean): number;
	readInt32(ofst: number, lsb: boolean): number;
	readFloat64(ofst: number, lsb: boolean): number;
	readUTF8(ofst: number, len: number): string;
	readUTF8Z(ofst: number, maxSize?: number): string;
	readUTF16(ofst: number, nChar: number, lsb: boolean): string;
	
	readUInt8Arr(ofst: number, cnt: number): number[];
	readUInt16Arr(ofst: number, lsb: boolean, cnt: number): number[];
	readUInt32Arr(ofst: number, lsb: boolean, cnt: number): number[];
	readInt8Arr(ofst: number, cnt: number): number[];
	readInt16Arr(ofst: number, lsb: boolean, cnt: number): number[];
	readInt32Arr(ofst: number, lsb: boolean, cnt: number): number[];
	readFloat64Arr(ofst: number, lsb: boolean, cnt: number): number[];

	isASCIIText(ofst: number, len: number): boolean;
}

export abstract class ParsedObject
{
	sourceName: string;
	objType: string;
	constructor(sourceName: string, objType: string);
}
