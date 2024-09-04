import * as data from "./data";

export enum EXIFMaker
{
	Standard,
	Panasonic,
	Canon,
	Olympus,
	Casio1,
	Casio2,
	FLIR,
	Nikon3,
	Sanyo,
	Apple
}

export enum EXIFType
{
	Unknown,
	Bytes,
	STRING,
	UINT16,
	UINT32,
	Rational,
	Other,
	INT16,
	SubExif,
	INT32,
	SRational,
	Double,
	UINT64,
	INT64
}

declare class GPSInfo
{
	lat: number;
	lon: number;
	altitude: number;
	gpsTime: data.Timestamp;
}

export function loadImageFromBlob(blob: Blob): Promise<HTMLImageElement>;

export class EXIFItem
{
	id: number;
	type: EXIFType;
	data: any;

	constructor(id: number, type: EXIFType, data: any);
	clone(): EXIFItem;
}

export class EXIFData
{
	exifMaker: EXIFMaker;
	exifMap: object;

	constructor(exifMaker: EXIFMaker);

	getEXIFMaker(): EXIFMaker;
	clone(): EXIFData;
	addBytes(id: number, data: number[]): void;
	addString(id: number, data: string): void;
	addUInt16(id: number, data: number[]): void;
	addUInt32(id: number, data: number[]): void;
	addInt16(id: number, data: number[]): void;
	addInt32(id: number, data: number[]): void;
	addRational(id: number, data: number[]): void; //UInt32[]
	addSRational(id: number, data: number[]): void; //Int32[]
	addOther(id: number, data: ArrayBuffer): void;
	addSubEXIF(id: number, data: EXIFData): void;
	addDouble(id: number, data: number[]): void;
	addUInt64(id: number, data: number[]): void;
	addInt64(id: number, data: number[]): void;
	remove(id: number): void;

	getExifIds(): number[];
	getExifType(id: number): EXIFType;
	getExifItem(id: number): EXIFItem | null;
	getExifUInt16(id: number): number[] | null;
	getExifUInt32(id: number): number[] | null;
	getExifSubexif(id: number): EXIFData | null;
	getExifOther(id: number): ArrayBuffer | null;

	getPhotoDate(): data.Timestamp;
	getPhotoMake(): string;
	getPhotoModel(): string;
	getPhotoLens(): string;
	getPhotoFNumber(): number;
	getPhotoExpTime(): number;
	getPhotoISO(): number;
	getPhotoFocalLength(): number;
	getPhotoLocation(): GPSInfo | null;

	getProperties(): object;
	toString(linePrefix?: string): string;
	toStringCanonCameraSettings(linePrefix: string | null | undefined, valBuff: number[]): string;
	toStringCanonFocalLength(linePrefix: string | null | undefined, valBuff: number[]): string;
	toStringCanonShotInfo(linePrefix: string | null | undefined, valBuff: number[]): string;
	toStringCanonLensType(lensType: number): string;
	parseMakerNote(buff: ArrayBuffer): EXIFData | null;

	static getEXIFName(exifMaker: EXIFMaker, id: number, subId: number): string;
	static parseIFD(reader: data.ByteReader, ofst: number, lsb: boolean, nextOfst: object | null, readBase: number | null, maker?: EXIFMaker): EXIFData | null;
}

export class StaticImage extends data.ParsedObject
{
	img: HTMLImageElement;
	exif: EXIFData;
	
	constructor(img: HTMLImageElement, sourceName: string, objType: string);
	setExif(exif: EXIFData): void;
	getWidth(): number;
	getHeight(): number;
	getProperties(): object;

	createCanvas(): HTMLCanvasElement;
	exportJPG(quality?: number): Promise<Blob | null>;
	exportWEBP(quality?: number): Promise<Blob | null>;
	exportPNG(): Promise<Blob | null>;
}
