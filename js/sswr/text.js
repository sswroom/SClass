export function zeroPad(val, ndigits)
{
	var s = "" + val;
	while (s.length < ndigits)
		s = "0"+s;
	return s;
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
