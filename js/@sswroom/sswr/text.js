import * as data from "./data.js";
import * as net from "./net.js";

export const Base64Charset = {
	Normal: 1,
	URL: 2
};

export const LineBreakType = {
	None: "",
	CR: "\r",
	LF: "\n",
	CRLF: "\r\n"
};

export const HAlignment = {
	Unknown: 0,
	Left: 1,
	Center: 2,
	Right: 3,
	Justify: 4,
	Fill: 5
}

export const VAlignment = {
	Unknown: 0,
	Top: 1,
	Center: 2,
	Bottom: 3,
	Justify: 4
}

export function zeroPad(val, ndigits)
{
	let s = "" + val;
	while (s.length < ndigits)
		s = "0"+s;
	return s;
}

export function isInteger(s)
{
	if (s === null)
		return false;
	if (s.startsWith("-"))
		s = s.substring(1);
	let j = s.length;
	if (j == 0)
		return false;
	let i = 0;
	let c;
	while (i < j)
	{
		c = s.charCodeAt(i);
		if (c < 0x30 || c > 0x39)
			return false;
		i++;
	}
	return true;
}

export function isUInteger(s)
{
	if (s === null)
		return false;
	let j = s.length;
	if (j == 0)
		return false;
	let i = 0;
	let c;
	while (i < j)
	{
		c = s.charCodeAt(i);
		if (c < 0x30 || c > 0x39)
			return false;
		i++;
	}
	return true;
}

export function toJSText(s)
{
	let out = "\"";
	let i = 0;
	let j = s.length;
	let c;
	let ccode;
	while (i < j)
	{
		c = s.charAt(i);
		if (c == '\\')
		{
			out += "\\\\";
		}
		else if (c == "\"")
		{
			out += "\\\"";
		}
		else if (c == "\r")
		{
			out += "\\r";
		}
		else if (c == "\n")
		{
			out += "\\n";
		}
		else if (c == "\0")
		{
			out += "\\0";
		}
		else if ((ccode = s.charCodeAt(i)) < 32)
		{
			out += "\\u"+zeroPad(ccode.toString(16), 4);
		}
		else
		{
			out += c;
		}
		i++;
	}
	return out + "\"";
}

export function toXMLText(s)
{
	if (s == null) return "";
	let out = "";
	let i = 0;
	let j = s.length;
	let c;
	while (i < j)
	{
		c = s.charAt(i);
		switch (c)
		{
		case '&':
			out += '&amp;';
			break;
		case '<':
			out += '&lt;';
			break;
		case '>':
			out += '&gt;';
			break;
		case '\'':
			out += '&apos;';
			break;
		case '"':
			out += '&quot;';
			break;
		case '\r':
			out += '&#13;';
			break;
		case '\n':
			out += '&#10;';
			break;
		default:
			out += c;
			break;
		}
		i++;
	}
	return out + "";
}

export function toAttrText(s)
{
	if (s == null) return "";
	return "\""+toXMLText(s)+"\"";
}

export function toHTMLText(s)
{
	if (s == null) return "";
	let out = "";
	let i = 0;
	let j = s.length;
	let c;
	while (i < j)
	{
		c = s.charAt(i);
		switch (c)
		{
		case '&':
			out += '&#38;';
			break;
		case '<':
			out += '&lt;';
			break;
		case '>':
			out += '&gt;';
			break;
		case '\'':
			out += '&#39;';
			break;
		case '"':
			out += '&quot;';
			break;
		case '\r':
			break;
		case '\n':
			out += '<br/>';
			break;
		default:
			out += c;
			break;
		}
		i++;
	}
	return out + "";
}

export function bracketToHTML(s)
{
	let i;
	let j;
	while (true)
	{
		i = s.indexOf("[i]");
		if (i < 0)
			break;
		j = s.indexOf("[/i]", i + 3);
		if (j < 0)
			break;
		s = s.substring(0, i)+"<i>"+s.substring(i + 3, j)+"</i>"+s.substring(j + 4);
	}
	return s;
}

export function arrayToNumbers(arr)
{
	let ret = [];
	let i;
	for (i in arr)
	{
		ret.push(Number.parseFloat(arr[i]));
	}
	return ret;
}

export function toHex8(v)
{
	let s = (v & 255).toString(16);
	return "0".repeat(2 - s.length)+s;
}

export function toHex16(v)
{
	let s = (v & 0xffff).toString(16);
	return "0".repeat(4 - s.length)+s;
}

export function toHex32(v)
{
	return toHex16(v >> 16) + toHex16(v);
}

export function u8Arr2Hex(buff, byteSep, rowSep)
{
	let rows = [];
	let thisRow = [];
	let i = 1;
	let j = buff.length;
	if (j == 0)
		return "";
	if (byteSep == null)
		byteSep = "";
	thisRow.push(toHex8(buff[0]));
	while (i < j)
	{
		if ((i & 15) == 0)
		{
			rows.push(thisRow.join(byteSep));
			thisRow = [];
		}
		thisRow.push(toHex8(buff[i]));
		i++;
	}
	rows.push(thisRow.join(byteSep));
	if (rowSep == null)
		rowSep = byteSep;
	return rows.join(rowSep);
}

export function splitLines(txt)
{
	let lines = [];
	let pos = 0;
	let i;
	let j;
	while (true)
	{
		i = txt.indexOf("\r", pos);
		j = txt.indexOf("\n", pos);
		if (i == -1 && j == -1)
		{
			lines.push(txt.substring(pos));
			break;
		}
		else if (i == -1)
		{
			lines.push(txt.substring(pos, j));
			pos = j + 1;
		}
		else if (j == -1)
		{
			lines.push(txt.substring(pos, i));
			pos = i + 1;
		}
		else if (i < j)
		{
			lines.push(txt.substring(pos, i));
			if (j == i + 1)
				pos = j + 1;
			else
				pos = i + 1;
		}
		else
		{
			lines.push(txt.substring(pos, j));
			pos = j + 1;
		}
	}
	return lines;
}

export function isEmailAddress(s)
{
	let atPos = -1;
	let dotIndex = -1;
	let c;
	let i = 0;
	let j = s.length;
	while (i < j)
	{
		c = s.charAt(i);
		if (charIsAlphaNumeric(s, i) || c == '-')
		{

		}
		else if (c == '_')
		{
			if (atPos != -1)
			{
				return false;
			}
		}
		else if (c == '.')
		{
			if (atPos != -1)
			{
				dotIndex = i;
			}
		}
		else if (c == '@')
		{
			if (atPos != -1)
			{
				return false;
			}
			atPos = i;
			dotIndex = -1;
		}
		else
		{
			return false;
		}
		i++;
	}
	if (atPos == -1 || atPos == 0 || dotIndex == -1 || (j - dotIndex) < 2)
	{
		return false;
	}
	return true;
}

export function toUTF32Length(s)
{
	let i = 0;
	let j = s.length;
	let ret = 0;
	while (i < j)
	{
		if (i + 1 < j)
		{
			let c1 = s.charCodeAt(i);
			let c2 = s.charCodeAt(i + 1);
			if (c1 >= 0xd800 && c1 < 0xdc00 && c2 >= 0xdc00 && c2 < 0xe000)
			{
				i += 2;
			}
			else
			{
				i++;
			}
		}
		else
		{
			i++;
		}
		ret++;
	}
	return ret;
}

export function isHKID(s)
{
	let hkid;
	let chk;
	let ichk;
	if (s.endsWith(')'))
	{
		if (s.length == 10)
		{
			if (s.charAt(7) == '(')
			{
				hkid = s.substring(0, 7);
				chk = s.charAt(8);
			}
			else
			{
				return false;
			}
		}
		else if (s.length == 11)
		{
			if (s.charAt(8) == '(')
			{
				hkid = s.substring(0, 8);
				chk = s.charAt(9);
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (s.length == 8)
		{
			hkid = s.substring(0, 7);
			chk = s.charAt(7);
		}
		else if (s.length == 9)
		{
			hkid = s.substring(0, 8);
			chk = s.charAt(8);
		}
		else
		{
			return false;
		}
	}

	if (charIsDigit(chk, 0))
		ichk = Number.parseInt(chk);
	else if (chk == 'A')
		ichk = 10;
	else
	{
		return false;
	}

	let thisChk;
	if (hkid.length == 8)
	{
		if (!charIsUpperCase(hkid, 0) ||
			!charIsUpperCase(hkid, 1) ||
			!charIsDigit(hkid, 2) ||
			!charIsDigit(hkid, 3) ||
			!charIsDigit(hkid, 4) ||
			!charIsDigit(hkid, 5) ||
			!charIsDigit(hkid, 6) ||
			!charIsDigit(hkid, 7))
				return false;
		
		thisChk = 0;
		thisChk += (hkid.charCodeAt(0) - 0x41 + 10) * 9;
		thisChk += (hkid.charCodeAt(1) - 0x41 + 10) * 8;
		thisChk += (hkid.charCodeAt(2) - 0x30) * 7;
		thisChk += (hkid.charCodeAt(3) - 0x30) * 6;
		thisChk += (hkid.charCodeAt(4) - 0x30) * 5;
		thisChk += (hkid.charCodeAt(5) - 0x30) * 4;
		thisChk += (hkid.charCodeAt(6) - 0x30) * 3;
		thisChk += (hkid.charCodeAt(7) - 0x30) * 2;
		thisChk += ichk;
		if ((thisChk % 11) != 0)
			return false;
		return true;
	}
	else
	{
		if (!charIsUpperCase(hkid, 0) ||
			!charIsDigit(hkid, 1) ||
			!charIsDigit(hkid, 2) ||
			!charIsDigit(hkid, 3) ||
			!charIsDigit(hkid, 4) ||
			!charIsDigit(hkid, 5) ||
			!charIsDigit(hkid, 6))
				return false;

		thisChk = 36 * 9;
		thisChk += (hkid.charCodeAt(0) - 0x41 + 10) * 8;
		thisChk += (hkid.charCodeAt(1) - 0x30) * 7;
		thisChk += (hkid.charCodeAt(2) - 0x30) * 6;
		thisChk += (hkid.charCodeAt(3) - 0x30) * 5;
		thisChk += (hkid.charCodeAt(4) - 0x30) * 4;
		thisChk += (hkid.charCodeAt(5) - 0x30) * 3;
		thisChk += (hkid.charCodeAt(6) - 0x30) * 2;
		thisChk += ichk;
		if ((thisChk % 11) != 0)
			return false;
		return true;
	}
}

export function charIsAlphaNumeric(s, index)
{
	let c = s.charCodeAt(index);
	if (c >= 0x30 && c <= 0x39)
		return true;
	if (c >= 0x41 && c <= 0x5A)
		return true;
	if (c >= 0x61 && c <= 0x7A)
		return true;
	return false;
}

export function charIsDigit(s, index)
{
	let c = s.charCodeAt(index);
	if (c >= 0x30 && c <= 0x39)
		return true;
	return false;
}

export function charIsUpperCase(s, index)
{
	let c = s.charCodeAt(index);
	if (c >= 0x41 && c <= 0x5A)
		return true;
	return false;
}

export function charIsLowerCase(s, index)
{
	let c = s.charCodeAt(index);
	if (c >= 0x61 && c <= 0x7A)
		return true;
	return false;
}

/**
 * @param {string} s
 * @param {string} replaceFrom
 * @param {string} replaceTo
 */
export function replaceAll(s, replaceFrom, replaceTo)
{
	let i = 0;
	let j;
	while (true)
	{
		j = s.indexOf(replaceFrom, i);
		if (j < 0)
			return s;
		s = s.substring(0, j) + replaceTo + s.substring(j + replaceFrom.length);
		i = j + replaceTo.length;
	}
}

export function getEncList()
{
	let ret = [];
	ret.push(new Base64Enc());
	ret.push(new UTF8TextBinEnc());
	ret.push(new UTF8LCaseTextBinEnc());
	ret.push(new UTF8UCaseTextBinEnc());
	ret.push(new CPPByteArrBinEnc());
	ret.push(new CPPTextBinEnc());
	ret.push(new HexTextBinEnc());
	ret.push(new QuotedPrintableEnc());
	ret.push(new UTF16LETextBinEnc());
	ret.push(new UTF16BETextBinEnc());
	ret.push(new ASN1OIDBinEnc());
	ret.push(new FormEncoding());
	ret.push(new URIEncoding());
	ret.push(new ASCII85Enc());
	return ret;
}

/**
 * @param {string} s
 * @param {number|undefined} cs
 * @returns {string}
 */
export function b64Enc(s, cs)
{
	let b64 = new Base64Enc(cs, false);
	let enc = new TextEncoder();
	return b64.encodeBin(enc.encode(s));
}

/**
 * @param {string} b64Str
 * @returns {string}
 */
export function b64Dec(b64Str)
{
	let b64 = new Base64Enc();
	let dec = new TextDecoder();
	return dec.decode(b64.decodeBin(b64Str));
}

/**
 * @param {string} url
 */
export function isDataURL(url)
{
	return url.startsWith("data:");
}

/**
 * @param {string} s
 */
export function escapeXhtml(s)
{
	return s.replace(/#/g, '%23').replace(/\n/g, '%0A');
}

/**
 * @param {string} svg
 */
export function svgStringToDataURI(svg)
{
	return 'data:image/svg+xml;charset=utf-8,' + svg;
}

export class TextBinEnc
{
	constructor(name)
	{
		this.name = name;
	}

	getName()
	{
		return this.name;
	}
	
	getClassName()
	{
		return this.constructor.name;
	}
}

export class UTF8TextBinEnc extends TextBinEnc
{
	constructor()
	{
		super("UTF-8 Text");
	}

	/**
	 * @param {Uint8Array} buff
	 * @returns {string}
	 */
	encodeBin(buff)
	{
		let enc = new TextDecoder();
		return enc.decode(buff);
	}

	/**
	 * @param {string} str 
	 * @returns {Uint8Array}
	 */
	decodeBin(str)
	{
		let enc = new TextEncoder();
		return enc.encode(str);
	}
}

export class UTF8LCaseTextBinEnc extends TextBinEnc
{
	constructor()
	{
		super("UTF-8 LCase Text");
	}

	encodeBin(buff)
	{
		let enc = new TextDecoder();
		return enc.decode(buff).toLowerCase();
	}

	decodeBin(str)
	{
		let enc = new TextEncoder();
		return enc.encode(str);
	}
}

export class UTF8UCaseTextBinEnc extends TextBinEnc
{
	constructor()
	{
		super("UTF-8 UCase Text");
	}

	encodeBin(buff)
	{
		let enc = new TextDecoder();
		return enc.decode(buff).toUpperCase();
	}

	decodeBin(str)
	{
		let enc = new TextEncoder();
		return enc.encode(str);
	}
}

export class HexTextBinEnc extends TextBinEnc
{
	constructor()
	{
		super("Hex");
	}

	encodeBin(buff)
	{
		let arr = new Uint8Array(buff);
		let ret = [];
		let i;
		for (i in arr)
		{
			ret.push(toHex8(arr[i]));
		}
		return ret.join(" ");
	}

	decodeBin(str)
	{
		let arr = [];
		let i = 0;
		let j = str.length;
		let v = 0;
		let found = 0;
		let c;
		while (i < j)
		{
			c = str.charCodeAt(i);
			if (c >= 0x30 && c <= 0x39)
			{
				v = (v << 4) + (c - 0x30);
				found++;
			}
			else if (c >= 0x41 && c <= 0x46)
			{
				v = (v << 4) + (c - 0x37);
				found++;
			}
			else if (c >= 0x61 && c <= 0x66)
			{
				v = (v << 4) + (c - 0x57);
				found++;
			}
			if (found == 2)
			{
				arr.push(v);
				v = 0;
				found = 0;
			}

			i++;
		}
		return new Uint8Array(arr);
	}
}

const Base64Enc_decArr = [
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3e, 0xff, 0x3e, 0xff, 0x3f,
	0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
	0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0x3f,
	0xff, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
];
export class Base64Enc extends TextBinEnc
{
	static getEncArr(charset)
	{
		if (charset == Base64Charset.URL)
		{
			return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
		}
		else
		{
			return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		}
	
	}

	constructor(charset, noPadding)
	{
		super("Base64");
		this.encArr = Base64Enc.getEncArr(charset);
		this.noPadding = noPadding || null;
	}

	encodeBin(buff, lbt, charsPerLine)
	{
		let arr = new Uint8Array(buff);
		if (lbt == null)
			lbt = LineBreakType.None;
		let tmp1 = arr.length % 3;
		let tmp2 = Math.floor(arr.length / 3);
		if (lbt == LineBreakType.None || !charsPerLine)
		{
			let ret = [];
			let i = 0;
			while (tmp2-- > 0)
			{
				ret.push(this.encArr.charAt(arr[i + 0] >> 2));
				ret.push(this.encArr.charAt(((arr[i + 0] << 4) | (arr[i + 1] >> 4)) & 0x3f));
				ret.push(this.encArr.charAt(((arr[i + 1] << 2) | (arr[i + 2] >> 6)) & 0x3f));
				ret.push(this.encArr.charAt(arr[i + 2] & 0x3f));
				i += 3;
			}
			if (tmp1 == 1)
			{
				ret.push(this.encArr.charAt(arr[i + 0] >> 2));
				ret.push(this.encArr.charAt((arr[i + 0] << 4) & 0x3f));
				if (this.noPadding)
				{
				}
				else
				{
					ret.push("==");
				}
			}
			else if (tmp1 == 2)
			{
				ret.push(this.encArr.charAt(arr[i + 0] >> 2));
				ret.push(this.encArr.charAt(((arr[i + 0] << 4) | (arr[i + 1] >> 4)) & 0x3f));
				ret.push(this.encArr.charAt((arr[i + 1] << 2) & 0x3f));
				if (this.noPadding)
				{
				}
				else
				{
					ret.push("=");
				}
			}
			return ret.join("");
		}
		else
		{
			let ret = [];
			let i = 0;
			let lineLeft = charsPerLine;
			while (tmp2-- > 0)
			{
				if (lineLeft >= 4)
				{
					ret.push(this.encArr.charAt(arr[i + 0] >> 2));
					ret.push(this.encArr.charAt(((arr[i + 0] << 4) | (arr[i + 1] >> 4)) & 0x3f));
					ret.push(this.encArr.charAt(((arr[i + 1] << 2) | (arr[i + 2] >> 6)) & 0x3f));
					ret.push(this.encArr.charAt(arr[i + 2] & 0x3f));
					if (lineLeft == 0)
					{
						if (tmp2 > 0 || tmp1 != 0)
							ret.push(lbt);
						lineLeft = charsPerLine;
					}
				}
				else if (lineLeft == 3)
				{
					ret.push(this.encArr.charAt(arr[i + 0] >> 2));
					ret.push(this.encArr.charAt(((arr[i + 0] << 4) | (arr[i + 1] >> 4)) & 0x3f));
					ret.push(this.encArr.charAt(((arr[i + 1] << 2) | (arr[i + 2] >> 6)) & 0x3f));
					ret.push(lbt);
					ret.push(this.encArr.charAt(arr[i + 2] & 0x3f));
					lineLeft = charsPerLine - 1;
				}
				else if (lineLeft == 2)
				{
					ret.push(this.encArr.charAt(arr[i + 0] >> 2));
					ret.push(this.encArr.charAt(((arr[i + 0] << 4) | (arr[i + 1] >> 4)) & 0x3f));
					ret.push(lbt);
					ret.push(this.encArr.charAt(((arr[i + 1] << 2) | (arr[i + 2] >> 6)) & 0x3f));
					ret.push(this.encArr.charAt(arr[i + 2] & 0x3f));
					lineLeft = charsPerLine - 2;
				}
				else
				{
					ret.push(this.encArr.charAt(arr[i + 0] >> 2));
					ret.push(lbt);
					ret.push(this.encArr.charAt(((arr[i + 0] << 4) | (arr[i + 1] >> 4)) & 0x3f));
					ret.push(this.encArr.charAt(((arr[i + 1] << 2) | (arr[i + 2] >> 6)) & 0x3f));
					ret.push(this.encArr.charAt(arr[i + 2] & 0x3f));
					lineLeft = charsPerLine - 3;
				}
				i += 3;
			}
			if (tmp1 == 1)
			{
				if (lineLeft >= 2)
				{
					ret.push(this.encArr.charAt(arr[i + 0] >> 2));
					ret.push(this.encArr.charAt((arr[i + 0] << 4) & 0x3f));
					if (this.noPadding)
					{
					}
					else if (lineLeft == 2)
					{
						ret.push(lbt);
						ret.push("==");
					}
					else if (lineLeft == 3)
					{
						ret.push("=");
						ret.push(lbt);
						ret.push("=");
					}
					else
					{
						ret.push("==");
					}
				}
				else
				{
					ret.push(this.encArr.charAt(arr[i + 0] >> 2));
					ret.push(lbt);
					ret.push(this.encArr.charAt((arr[i + 0] << 4) & 0x3f));
					if (this.noPadding)
					{
					}
					else
					{
						ret.push("==");
					}
				}
			}
			else if (tmp1 == 2)
			{
				if (lineLeft >= 3)
				{
					ret.push(this.encArr.charAt(arr[i + 0] >> 2));
					ret.push(this.encArr.charAt(((arr[i + 0] << 4) | (arr[i + 1] >> 4)) & 0x3f));
					ret.push(this.encArr.charAt((arr[i + 1] << 2) & 0x3f));
					if (this.noPadding)
					{
					}
					else if (lineLeft == 3)
					{
						ret.push(lbt);
						ret.push("=");
					}
					else
					{
						ret.push("=");
					}
				}
				else
				{
					if (lineLeft == 2)
					{
						ret.push(this.encArr.charAt(arr[i + 0] >> 2));
						ret.push(this.encArr.charAt(((arr[i + 0] << 4) | (arr[i + 1] >> 4)) & 0x3f));
						ret.push(lbt);
						ret.push(this.encArr.charAt((arr[i + 1] << 2) & 0x3f));
					}
					else
					{
						ret.push(this.encArr.charAt(arr[i + 0] >> 2));
						ret.push(lbt);
						ret.push(this.encArr.charAt(((arr[i + 0] << 4) | (arr[i + 1] >> 4)) & 0x3f));
						ret.push(this.encArr.charAt((arr[i + 1] << 2) & 0x3f));
					}
					if (this.noPadding)
					{
					}
					else
					{
						ret.push("=");
					}
				}
			}
			return ret.join("");
		}
	}

	decodeBin(str)
	{
		let ret = [];
		let i = 0;
		let j = str.length;
		let b = 0;
		let b2 = 0;
		let c;
		let code;
		while (i < j)
		{
			c = str.charCodeAt(i);
			if (c < 0x80)
			{
				code = Base64Enc_decArr[c];
				if (code != 0xff)
				{
					switch (b)
					{
					case 0:
						b2 = (code << 2);
						b = 1;
						break;
					case 1:
						ret.push(b2 | (code >> 4));
						b2 = (code << 4);
						b = 2;
						break;
					case 2:
						ret.push(b2 | (code >> 2));
						b2 = (code << 6);
						b = 3;
						break;
					case 3:
						ret.push(b2 | code);
						b = 0;
						break;
					}
				}
			}
			i++;
		}
		return new Uint8Array(ret);
	}
}

export class UTF16LETextBinEnc extends TextBinEnc
{
	constructor()
	{
		super("UTF16LE Text");
	}

	encodeBin(buff)
	{
		let ret = [];
		let i = 0;
		let j = buff.length;
		if (j & 1)
			j--;
		while (i < j)
		{
			ret.push(String.fromCharCode(buff[i] + (buff[i + 1] << 8)));
			i += 2;
		}
		return ret.join("");
	}

	decodeBin(str)
	{
		let ret = [];
		let c;
		let i = 0;
		let j = str.length;
		while (i < j)
		{
			c = str.charCodeAt(i);
			ret.push(c & 255);
			ret.push(c >> 8);
			i++;
		}
		return new Uint8Array(ret);
	}
}

export class UTF16BETextBinEnc extends TextBinEnc
{
	constructor()
	{
		super("UTF16BE Text");
	}

	encodeBin(buff)
	{
		let ret = [];
		let i = 0;
		let j = buff.length;
		if (j & 1)
			j--;
		while (i < j)
		{
			ret.push(String.fromCharCode(buff[i + 1] + (buff[i] << 8)));
			i += 2;
		}
		return ret.join("");
	}

	decodeBin(str)
	{
		let ret = [];
		let c;
		let i = 0;
		let j = str.length;
		while (i < j)
		{
			c = str.charCodeAt(i);
			ret.push(c >> 8);
			ret.push(c & 255);
			i++;
		}
		return new Uint8Array(ret);
	}
}

export class CPPByteArrBinEnc extends TextBinEnc
{
	constructor()
	{
		super("CPP Byte Arr");
	}

	encodeBin(buff)
	{
		let lines = [];
		let cols = [];
		let arr = new Uint8Array(buff);
		let i = 0;
		let j = arr.length;
		while (i < j)
		{
			if (i > 0 && (i & 15) == 0)
			{
				lines.push(cols.join(", "));
				cols = [];
			}
			cols.push("0x"+toHex8(arr[i]));
			i++;
		}
		if (cols.length > 0)
		{
			lines.push(cols.join(", "));
		}
		return lines.join(",\r\n");
	}

	decodeBin(str)
	{
		let ret = [];
		let i = -1;
		let j;
		let s;
		while (true)
		{
			j = str.indexOf(",", i + 1);
			if (j < 0)
			{
				s = str.substring(i + 1).trim();
			}
			else
			{
				s = str.substring(i + 1, j).trim();
			}
			if (s.startsWith("0x"))
				ret.push(Number.parseInt(s.substring(2), 16));
			else
				ret.push(Number.parseInt(s));
			if (j < 0)
				break;
			else
				i = j;
		}
		return new Uint8Array(ret);
	}
}

export class CPPTextBinEnc extends TextBinEnc
{
	constructor()
	{
		super("CPP String");
	}

	encodeBin(buff)
	{
		let arr = new Uint8Array(buff);
		let ret = [];
		let lineStart = true;
		let code;
		let b;
		let i = 0;
		let j = arr.length;
		while (i < j)
		{
			b = arr[i];
			if (lineStart)
			{
				ret.push("\"");
				lineStart = false;
			}
			if (b == 0)
			{
				ret.push("\\0");
			}
			else if (b == 13)
			{
				ret.push("\\r");
			}
			else if (b == 10)
			{
				ret.push("\\n\"\r\n");
				lineStart = true;
			}
			else if (b == 0x5c)
			{
				ret.push("\\\\");
			}
			else if (b == 0x22)
			{
				ret.push("\\\"");
			}
			else if (b < 0x80)
			{
				ret.push(String.fromCharCode(b));
			}
			else if ((b & 0xe0) == 0xc0)
			{
				ret.push(String.fromCharCode((((b & 0x1f) << 6) | (arr[i + 1] & 0x3f))));
				i++;
			}
			else if ((b & 0xf0) == 0xe0)
			{
				ret.push(String.fromCharCode(((b & 0x0f) << 12) | ((arr[i + 1] & 0x3f) << 6) | (arr[i + 2] & 0x3f)));
				i += 2;
			}
			else if ((b & 0xf8) == 0xf0)
			{
				code = (((b & 0x7) << 18) | ((arr[i + 1] & 0x3f) << 12) | ((arr[i + 2] & 0x3f) << 6) | (arr[i + 3] & 0x3f));
				if (code >= 0x10000)
				{
					ret.push(String.fromCharCode(((code - 0x10000) >> 10) + 0xd800, (code & 0x3ff) + 0xdc00));
				}
				else
				{
					ret.push(String.fromCharCode(code));
				}
				i += 3;
			}
			else if ((b & 0xfc) == 0xf8)
			{
				code = (((b & 0x3) << 24) | ((arr[i + 1] & 0x3f) << 18) | ((arr[i + 2] & 0x3f) << 12) | ((arr[i + 3] & 0x3f) << 6) | (arr[i + 4] & 0x3f));
				if (code >= 0x10000)
				{
					ret.push(String.fromCharCode(((code - 0x10000) >> 10) + 0xd800, (code & 0x3ff) + 0xdc00));
				}
				else
				{
					ret.push(String.fromCharCode(code));
				}
				i += 4;
			}
			else if ((b & 0xfe) == 0xfc)
			{
				code = (((b & 0x1) << 30) | ((arr[i + 1] & 0x3f) << 24) | ((arr[i + 2] & 0x3f) << 18) | ((arr[i + 3] & 0x3f) << 12) | ((arr[i + 4] & 0x3f) << 6) | (arr[i + 5] & 0x3f));
				if (code >= 0x10000)
				{
					ret.push(String.fromCharCode(((code - 0x10000) >> 10) + 0xd800, (code & 0x3ff) + 0xdc00));
				}
				else
				{
					ret.push(String.fromCharCode(code));
				}
				i += 5;
			}
			i++;
		}
		if (!lineStart)
		{
			ret.push('"');
		}
		return ret.join("");
	}

	decodeBin(str)
	{
		let isQuote = false;
		let ret = [];
		let i = 0;
		let j = str.length;
		let c;
		while (true)
		{
			if (i >= j)
			{
				if (isQuote)
				{
					throw new Error("Unexpected end of string");
				}
				break;
			}
			c = str.charCodeAt(i++);
			if (!isQuote)
			{
				if (c == 0x22)
				{
					isQuote = true;
				}
				else if (c == 0x20 || c == '\t'.charCodeAt(0) || c == 13 || c == 10)
				{
				}
				else
				{
					throw new Error("Only allow white space characters when it is not quoted");
				}
			}
			else if (c == 0x5c)
			{
				if (i >= j)
					throw new Error("Unexpected end of string after \\");
				c = str.charAt(i++);
				if (c == 'r')
				{
					ret.push(13);
				}
				else if (c == 'n')
				{
					ret.push(10);
				}
				else if (c == 't')
				{
					ret.push(9);
				}
				else if (c == '\\')
				{
					ret.push(0x5c);
				}
				else if (c == '"')
				{
					ret.push(0x22);
				}
				else if (c == '0')
				{
					ret.push(0);
				}
				else
				{
					throw new Error("Unsupported escape sequence \\"+c);
				}
			}
			else if (c == 0x22)
			{
				isQuote = false;
			}
			else if (c < 0x80)
			{
				ret.push(c);
			}
			else if (c < 0x800)
			{
				ret.push(0xc0 | (c >> 6));
				ret.push(0x80 | (c & 0x3f));
			}
			else if (c < 0x10000)
			{
				ret.push(0xe0 | (c >> 12));
				ret.push(0x80 | ((c >> 6) & 0x3f));
				ret.push(0x80 | (c & 0x3f));
			}
			else if (c < 0x200000)
			{
				ret.push(0xf0 | (c >> 18));
				ret.push(0x80 | ((c >> 12) & 0x3f));
				ret.push(0x80 | ((c >> 6) & 0x3f));
				ret.push(0x80 | (c & 0x3f));
			}
			else if (c < 0x4000000)
			{
				ret.push(0xf8 | (c >> 24));
				ret.push(0x80 | ((c >> 18) & 0x3f));
				ret.push(0x80 | ((c >> 12) & 0x3f));
				ret.push(0x80 | ((c >> 6) & 0x3f));
				ret.push(0x80 | (c & 0x3f));
			}
			else
			{
				ret.push(0xfc | (c >> 30));
				ret.push(0x80 | ((c >> 24) & 0x3f));
				ret.push(0x80 | ((c >> 18) & 0x3f));
				ret.push(0x80 | ((c >> 12) & 0x3f));
				ret.push(0x80 | ((c >> 6) & 0x3f));
				ret.push(0x80 | (c & 0x3f));
			}
		}
		return new Uint8Array(ret);
	}
}

export class QuotedPrintableEnc extends TextBinEnc
{
	constructor()
	{
		super("QuotedPrintable");
	}

	encodeBin(buff)
	{
		let hexStr = "0123456789ABCDEF";
		let arr = new Uint8Array(buff);
		let ret = [];
		let lineCnt = 0;
		let b;
		let i = 0;
		let j = arr.length;
		while (i < j)
		{
			b = arr[i++];
			if (b == 13 || b == 10)
			{
				ret.push(b);
				lineCnt = 0;
			}
			else if (b >= 32 && b <= 126)
			{
				if (lineCnt < 75)
				{
					ret.push(b);
					lineCnt++;
				}
				else if (arr[i] == 13 || arr[i] == 10)
				{
					ret.push(b);
					lineCnt++;
				}
				else
				{
					ret.push('='.charCodeAt(0));
					ret.push(13);
					ret.push(10);
					ret.push(b);
					lineCnt = 1;
				}
			}
			else if (lineCnt < 73)
			{
				ret.push('='.charCodeAt(0));
				ret.push(hexStr.charCodeAt(b >> 4));
				ret.push(hexStr.charCodeAt(b & 15));
				lineCnt += 3;
			}
			else if (arr[i] == 13 || arr[i] == 10)
			{
				ret.push('='.charCodeAt(0));
				ret.push(hexStr.charCodeAt(b >> 4));
				ret.push(hexStr.charCodeAt(b & 15));
				lineCnt += 3;
			}
			else
			{
				ret.push('='.charCodeAt(0));
				ret.push(13);
				ret.push(10);
				ret.push('='.charCodeAt(0));
				ret.push(hexStr.charCodeAt(b >> 4));
				ret.push(hexStr.charCodeAt(b & 15));
				lineCnt = 3;
			}
		}
		
		return new TextDecoder().decode(new Uint8Array(ret));
	}

	decodeBin(str)
	{
		let arr = new TextEncoder().encode(str);
		let ret = [];
		let c;
		let i = 0;
		let j = arr.length;
		while (i < j)
		{
			c = arr[i++];
			if (c == '='.charCodeAt(0))
			{
				if (i + 2 <= j && arr[i + 0] == 13 && arr[i + 1] == 10)
				{
					i += 2;
				}
				else if (i + 1 <= j && (arr[i] == 13 || arr[i] == 10))
				{
					i += 1;
				}
				else if (i + 2 <= j)
				{
					ret.push(Number.parseInt(String.fromCharCode(arr[i], arr[i + 1]), 16));
					i += 2;
				}
			}
			else
			{
				ret.push(c);
			}
		}
		return new Uint8Array(ret);
	}
}

export class ASN1OIDBinEnc extends TextBinEnc
{
	constructor()
	{
		super("SNMP OID");
	}

	encodeBin(buff)
	{
		return net.oidToString(buff);
	}

	decodeBin(str)
	{
		return net.oidText2PDU(str);
	}
}

export class URIEncoding extends TextBinEnc
{
	constructor()
	{
		super("URI Encoding");
	}

	encodeBin(buff)
	{
		return encodeURI(new TextDecoder().decode(buff));
	}

	decodeBin(str)
	{
		return new TextEncoder().encode(decodeURI(str));
	}
}

export class FormEncoding extends TextBinEnc
{
	constructor()
	{
		super("Form Encoding");
	}

	encodeBin(buff)
	{
		return encodeURIComponent(new TextDecoder().decode(buff));
	}

	decodeBin(str)
	{
		return new TextEncoder().encode(decodeURIComponent(str));
	}
}

export class ASCII85Enc extends TextBinEnc
{
	constructor()
	{
		super("ASCII85");
	}

	encodeBin(buff)
	{
		let ret = [];
		let arr = new Uint8Array(buff);
		let i = 0;
		let j = arr.length;
		let v;
		while (i + 4 <= j)
		{
			v = data.readMUInt32(arr, i);
			i += 4;
			ret.push(String.fromCharCode(Math.floor(v / 52200625) + 33));
			v %= 52200625;
			ret.push(String.fromCharCode(Math.floor(v / 614125) + 33));
			v %= 614125;
			ret.push(String.fromCharCode(Math.floor(v / 7225) + 33));
			v %= 7225;
			ret.push(String.fromCharCode(Math.floor(v / 85) + 33));
			v %= 85;
			ret.push(String.fromCharCode(v + 33));
		}
		switch (j - i)
		{
		case 1:
			v = arr[i] << 24;
			ret.push(String.fromCharCode(Math.floor(v / 52200625) + 33));
			v %= 52200625;
			ret.push(String.fromCharCode(Math.floor(v / 614125) + 33));
			break;
		case 2:
			v = data.readMUInt16(arr, i) << 16;
			ret.push(String.fromCharCode(Math.floor(v / 52200625) + 33));
			v %= 52200625;
			ret.push(String.fromCharCode(Math.floor(v / 614125) + 33));
			v %= 614125;
			ret.push(String.fromCharCode(Math.floor(v / 7225) + 33));
			break;
		case 3:
			v = data.readMUInt24(arr, i) << 8;
			ret.push(String.fromCharCode(Math.floor(v / 52200625) + 33));
			v %= 52200625;
			ret.push(String.fromCharCode(Math.floor(v / 614125) + 33));
			v %= 614125;
			ret.push(String.fromCharCode(Math.floor(v / 7225) + 33));
			v %= 7225;
			ret.push(String.fromCharCode(Math.floor(v / 85) + 33));
			break;
		}
		return ret.join("");
	}

	decodeBin(str)
	{
		let i = 0;
		let j = str.length;
		let validCnt = 0;
		const mulVals = [52200625, 614125, 7225, 85, 1];
		let v = 0;
		let lastU = 0;
		let c;
		let ret = [];
		while (i < j)
		{
			c = str.charCodeAt(i++);
			if (c == 'z'.charCodeAt(0))
			{
				ret.push(0);
				ret.push(0);
				ret.push(0);
				ret.push(0);
				lastU = 0;
			}
			else if (c == '~'.charCodeAt(0))
			{
				if (i < j && str.charAt(i) == '>')
				{
					lastU = 0;
					break;
				}
			}
			else if (c == 'u'.charCodeAt(0))
			{
				lastU++;
				v += (c - 33) * mulVals[validCnt];
				validCnt++;
			}
			else if (c < 33 || c > 'u'.charCodeAt(0))
			{
			}
			else
			{
				lastU = 0;
				v += (c - 33) * mulVals[validCnt];
				validCnt++;
			}
			if (validCnt == 5)
			{
				ret.push((v >> 24) & 0xff);
				ret.push((v >> 16) & 0xff);
				ret.push((v >> 8) & 0xff);
				ret.push(v & 0xff);
				validCnt = 0;
				v = 0;
			}
		}
		if (validCnt > 0)
		{
			i = validCnt;
			while (i < 5)
			{
				v += ('u'.charCodeAt(0) - 33) * mulVals[i];
				i++;
			}
			validCnt--;
			i = 0;
			while (i < validCnt)
			{
				ret.push((v >> 24) & 0xff);
				v = v << 8;
				i++;
			}
		}
		else if (lastU > 0)
		{
			while (lastU-- > 0)
			{
				ret.pop();
			}
		}
		return new Uint8Array(ret);
	}
}
