import DateTimeUtil from "./data/DateTimeUtil.js";
import Duration from "./data/Duration.js";
import TimeInstant from "./data/TimeInstant.js";
import Timestamp from "./data/Timestamp.js";

var data = {
	isArray: function(o)
	{
		return o != null && o.constructor === Array;
	},
	
	isObject: function(o)
	{
		return o != null && (typeof o) == "object";
	},
	
	toObjectString: function(o, lev)
	{
		if (lev && lev > 8)
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
		var out;
		var name;
		if (data.isArray(o))
		{
			out = new Array();
			out.push("[");
			for (name in o)
			{
				out.push(data.toObjectString(o[name], nextLev));
				out.push(",");
			}
			out.pop();
			out.push("]");
			return out.join("");
		}
		else if (t == "object")
		{
			out = new Array();
			out.push("{");
			for (name in o)
			{
				out.push(text.toJSText(name));
				out.push(":");
				out.push(data.toObjectString(o[name], nextLev));
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
			return text.toJSText(o);
		}
		else
		{
			console.log(t);
			return o;
		}
	},
	
	arrayBuffer2Base64: function(buff)
	{
		return btoa(String.fromCharCode.apply(null, new Uint8Array(buff)));
	},

	DateTimeUtil: DateTimeUtil,
	Duration: Duration,
	TimeInstant: TimeInstant,
	Timestamp: Timestamp
};

export default data;