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

export function zeroPad(val, ndigits)
{
	var s = "" + val;
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
	var j = s.length;
	if (j == 0)
		return false;
	var i = 0;
	var c;
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
	var out = "\"";
	var i = 0;
	var j = s.length;
	var c;
	var ccode;
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
	var out = "";
	var i = 0;
	var j = s.length;
	var c;
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
	var out = "";
	var i = 0;
	var j = s.length;
	var c;
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
	var i;
	var j;
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
	var ret = [];
	var i;
	for (i in arr)
	{
		ret.push(Number.parseFloat(arr[i]));
	}
	return ret;
}

export function toHex8(v)
{
	var s = (v & 255).toString(16);
	return "0".repeat(2 - s.length)+s;
}

export function toHex16(v)
{
	var s = (v & 0xffff).toString(16);
	return "0".repeat(4 - s.length)+s;
}

export function toHex32(v)
{
	var s = (v & 0xffffffff).toString(16);
	return "0".repeat(8 - s.length)+s;
}

export function getEncList()
{
	var ret = [];
	ret.push(new Base64Enc());
	ret.push(new UTF8TextBinEnc());
	ret.push(new UTF8LCaseTextBinEnc());
	ret.push(new UTF8UCaseTextBinEnc());
	ret.push(new CPPByteArrBinEnc());
	ret.push(new CPPTextBinEnc());
	ret.push(new HexTextBinEnc());
	ret.push(new UTF16LETextBinEnc());
	ret.push(new UTF16BETextBinEnc());
	return ret;
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

	encodeBin(buff)
	{
		var enc = new TextDecoder();
		return enc.decode(buff);
	}

	decodeBin(str)
	{
		var enc = new TextEncoder();
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
		var enc = new TextDecoder();
		return enc.decode(buff).toLowerCase();
	}

	decodeBin(str)
	{
		var enc = new TextEncoder();
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
		var enc = new TextDecoder();
		return enc.decode(buff).toUpperCase();
	}

	decodeBin(str)
	{
		var enc = new TextEncoder();
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
		var arr = new Uint8Array(buff);
		var ret = [];
		var i;
		for (i in arr)
		{
			ret.push(toHex8(arr[i]));
		}
		return ret.join(" ");
	}

	decodeBin(str)
	{
		var arr = [];
		var i = 0;
		var j = str.length;
		var v = 0;
		var found = 0;
		var c;
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
		var arr = new Uint8Array(buff);
		if (lbt == null)
			lbt = LineBreakType.None;
		var tmp1 = arr.length % 3;
		var tmp2 = Math.floor(arr.length / 3);
		if (lbt == LineBreakType.None || !charsPerLine)
		{
			var ret = [];
			var i = 0;
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
			var ret = [];
			var i = 0;
			var lineLeft = charsPerLine;
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
					else
					{
						if (lineLeft == 2)
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
					else
					{
						if (lineLeft == 3)
						{
							ret.push(lbt);
							ret.push("=");
						}
						else
						{
							ret.push("=");
						}
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
			return outSize;			
		}
	}

	decodeBin(str)
	{
		var ret = [];
		var i = 0;
		var j = str.length;
		var b = 0;
		var b2;
		var c;
		var code;
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
		var ret = [];
		var i = 0;
		var j = buff.length;
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
		var ret = [];
		var c;
		var i = 0;
		var j = str.length;
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
		var ret = [];
		var i = 0;
		var j = buff.length;
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
		var ret = [];
		var c;
		var i = 0;
		var j = str.length;
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
		var lines = [];
		var cols = [];
		var arr = new Uint8Array(buff);
		var i = 0;
		var j = arr.length;
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
		var ret = [];
		var i = -1;
		var j;
		var s;
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
		var arr = new Uint8Array(buff);
		var ret = [];
		var lineStart = true;
		var code;
		var b;
		var i = 0;
		var j = arr.length;
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
				code = (UInt32)(((b & 0x1) << 30) | ((arr[i + 1] & 0x3f) << 24) | ((arr[i + 2] & 0x3f) << 18) | ((arr[i + 3] & 0x3f) << 12) | ((arr[i + 4] & 0x3f) << 6) | (arr[i + 5] & 0x3f));
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
			ret.push('\"');
		}
		return ret.join("");
	}

	decodeBin(str)
	{
		var isQuote = false;
		var ret = [];
		var i = 0;
		var j = str.length;
		var c;
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
					return 0;
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
				else if (c == '\"')
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
			else
			{
				if (c < 0x80)
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
		}
		return new Uint8Array(ret);
	}
}
