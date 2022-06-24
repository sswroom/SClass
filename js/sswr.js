if (window.sswr == null) window.sswr = new Object();
window.sswr.data = new Object();
window.sswr.map = new Object();
window.sswr.text = new Object();
window.sswr.web = new Object();

sswr.data.isArray = function(o)
{
	return o != null && o.constructor === Array;
}

sswr.data.isObject = function(o)
{
	return o != null && (typeof o) == "object";
}

sswr.data.toObjectString = function(o, lev)
{
	if (lev && lev > 6)
	{
		return "";
	}
	var nextLev;
	if (lev)
	{
		nextLev = lev + 1;
	}
	else
	{
		nextLev = 1;
	}
	var t = typeof o;
	if (sswr.data.isArray(o))
	{
		var out = new Array();
		out.push("[");
		var name;
		for (name in o)
		{
			out.push(sswr.data.toObjectString(o[name], nextLev));
			out.push(",");
		}
		out.pop();
		out.push("]");
		return out.join("");
	}
	else if (t == "object")
	{
		var out = new Array();
		out.push("{");
		var name;
		for (name in o)
		{
			out.push(sswr.text.toJSText(name));
			out.push(":");
			out.push(sswr.data.toObjectString(o[name], nextLev));
			out.push(",");
		}
		out.pop();
		out.push("}");
		return out.join("");
	}
	else if (t == "function")
	{
		return "function()";
	}
	else if (t == "string")
	{
		return sswr.text.toJSText(o);
	}
	else
	{
		return o;
	}
}

sswr.map.DataFormat = new Object();
sswr.map.DataFormat.Cesium = "cesium";
sswr.map.DataFormat.WKT = "wkt";
sswr.map.DataFormat.GeoJSON = "geojson";

sswr.map.getLayers = function(svcUrl, onResultFunc)
{
	var xmlhttp = new XMLHttpRequest();
	xmlhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200)
		{
			onResultFunc(JSON.parse(this.responseText));
		}
	};
	xmlhttp.open("GET", svcUrl+'/getlayers', true);
	xmlhttp.send();
}

sswr.map.getLayerData = function(svcUrl, onResultFunc, layerName, dataFormat)
{
	var xmlhttp = new XMLHttpRequest();
	xmlhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200)
		{
			onResultFunc(JSON.parse(this.responseText));
		}
	};
	xmlhttp.open("GET", svcUrl+'/getlayerdata?name='+encodeURIComponent(layerName)+"&fmt="+encodeURIComponent(dataFormat), true);
	xmlhttp.send();
}

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
	var out = "\"";
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
	return out + "\"";
}

sswr.web.getRequestURLBase = function()
{
	var url = document.location.href;
	var i = url.indexOf("://");
	var j = url.indexOf("/", i + 3);
	if (j >= 0)
	{
		return url.substring(0, j);
	}
	else
	{
		return url;
	}
}
