import sswr from "./sswr.js";
sswr.text = new Object();

sswr.text.toJSText = function(s)
{
	var out = "\"";
	var i = 0;
	var j = s.length;
	var c;
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
		else
		{
			out += c;
		}
		i++;
	}
	return out + "\"";
}

sswr.text.toHTMLText = function(s)
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

export default sswr;