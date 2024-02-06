export enum Base64Charset
{
	Normal,
	URL
}

export enum LineBreakType
{
	None,
	CR,
	LF,
	CRLF
};

export function zeroPad(val: string | number, ndigits: number): string;
export function isInteger(s: string): boolean;
export function toJSText(s: string): string;
export function toXMLText(s: string): string;
export function toAttrText(s: string): string;
export function toHTMLText(s: string): string;
export function bracketToHTML(s: string): string;
export function arrayToNumbers(arr: string[]): number[];
export function toHex8(v: number): string;
export function toHex16(v: number): string;
export function toHex32(v: number): string;
export function u8Arr2Hex(buff: Uint8Array, byteSep: string, rowSep: string): string;
export function getEncList(): TextBinEnc[];

export class TextBinEnc
{
	constructor(name: string);
	getName(): string;
	getClassName(): string;
	encodeBin(buff: ArrayBuffer): string;
	decodeBin(str: string): Uint8Array;
}

export class UTF8TextBinEnc extends TextBinEnc
{
	constructor();
	encodeBin(buff: ArrayBuffer): string;
	decodeBin(str: string): Uint8Array;
}

export class UTF8LCaseTextBinEnc extends TextBinEnc
{
	constructor();
	encodeBin(buff: ArrayBuffer): string;
	decodeBin(str: string): Uint8Array;
}

export class UTF8UCaseTextBinEnc extends TextBinEnc
{
	constructor();
	encodeBin(buff: ArrayBuffer): string;
	decodeBin(str: string): Uint8Array;
}

export class HexTextBinEnc extends TextBinEnc
{
	constructor();
	encodeBin(buff: ArrayBuffer): string;
	decodeBin(str: string): Uint8Array;
}

export class Base64Enc extends TextBinEnc
{
	static getEncArr(charset: Base64Charset): string;
	constructor(charset?: Base64Charset, noPadding?: boolean);
	encodeBin(buff: ArrayBuffer, lbt?: LineBreakType, charsPerLine?: number): string;
	decodeBin(str: string): Uint8Array;
}

export class UTF16LETextBinEnc extends TextBinEnc
{
	constructor();
	encodeBin(buff: ArrayBuffer): string;
	decodeBin(str: string): Uint8Array;
}

export class UTF16BETextBinEnc extends TextBinEnc
{
	constructor();
	encodeBin(buff: ArrayBuffer): string;
	decodeBin(str: string): Uint8Array;
}

export class CPPByteArrBinEnc extends TextBinEnc
{
	constructor();
	encodeBin(buff: ArrayBuffer): string;
	decodeBin(str: string): Uint8Array;
}

export class CPPTextBinEnc extends TextBinEnc
{
	constructor();
	encodeBin(buff: ArrayBuffer): string;
	decodeBin(str: string): Uint8Array;
}

export class QuotedPrintableEnc extends TextBinEnc
{
	constructor();
	encodeBin(buff: ArrayBuffer): string;
	decodeBin(str: string): Uint8Array;
}

export class ASN1OIDBinEnc extends TextBinEnc
{
	constructor();
	encodeBin(buff: ArrayBuffer): string;
	decodeBin(str: string): Uint8Array;
}

export class URIEncoding extends TextBinEnc
{
	constructor();
	encodeBin(buff: ArrayBuffer): string;
	decodeBin(str: string): Uint8Array;
}

export class FormEncoding extends TextBinEnc
{
	constructor();
	encodeBin(buff: ArrayBuffer): string;
	decodeBin(str: string): Uint8Array;
}

export class ASCII85Enc extends TextBinEnc
{
	constructor();
	encodeBin(buff: ArrayBuffer): string;
	decodeBin(str: string): Uint8Array;
}
