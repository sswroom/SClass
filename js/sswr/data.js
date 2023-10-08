var data = {};

function zpadStr(val, ndigits)
{
	var s = "" + val;
	while (s.length < ndigits)
		s = "0"+s;
	return s;
}

data.isArray = function(o)
{
	return o != null && o.constructor === Array;
}

data.isObject = function(o)
{
	return o != null && (typeof o) == "object";
}

data.toObjectString = function(o, lev)
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
}

data.arrayBuffer2Base64 = function(buff)
{
	return btoa(String.fromCharCode.apply(null, new Uint8Array(buff)));
}

data.DateTimeUtil = new Object();
data.DateTimeUtil.monString = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];
data.DateTimeUtil.monthString = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"];
data.DateTimeUtil.timeValueSetDate = function(t, dateStrs)
{
	var vals0 = dateStrs[0] - 0;
	var vals1 = dateStrs[1] - 0;
	var vals2 = dateStrs[2] - 0;
	if (vals0 > 100)
	{
		t.year = vals0;
		t.month = vals1;
		t.day = vals2;
	}
	else if (vals2 > 100)
	{
		t.year = vals2;
		if (vals0 > 12)
		{
			t.month = vals1;
			t.day = vals0;
		}
		else
		{
			t.month = vals0;
			t.day = vals1;
		}
	}
	else
	{
		if (vals1 > 12)
		{
			t.year = ((Math.floor(t.year / 100) * 100) + vals2);
			t.month = vals0;
			t.day = vals1;
		}
		else
		{
			t.year = ((Math.floor(t.year / 100) * 100) + vals0);
			t.month = vals1;
			t.day = vals2;
		}
	}
}

data.DateTimeUtil.timeValueSetTime = function(t, timeStrs)
{
	var strs;

	t.hour = timeStrs[0] - 0;
	t.minute = timeStrs[1] - 0;
	strs = timeStrs[2].split('.');
	if (strs.length == 1)
	{
		t.second = strs[0] - 0;
		t.nanosec = 0;
	}
	else
	{
		t.second = strs[0] - 0;
		switch (strs[1].length)
		{
		case 0:
			t.nanosec = 0;
			break;
		case 1:
			t.nanosec = (strs[1] - 0) * 100000000;
			break;
		case 2:
			t.nanosec = (strs[1] - 0) * 10000000;
			break;
		case 3:
			t.nanosec = (strs[1] - 0) * 1000000;
			break;
		case 4:
			t.nanosec = (strs[1] - 0) * 100000;
			break;
		case 5:
			t.nanosec = (strs[1] - 0) * 10000;
			break;
		case 6:
			t.nanosec = (strs[1] - 0) * 1000;
			break;
		case 7:
			t.nanosec = (strs[1] - 0) * 100;
			break;
		case 8:
			t.nanosec = (strs[1] - 0) * 10;
			break;
		default:
			t.nanosec = strs[1].substr(0, 9) - 0;
			break;
		}
	}
}

data.DateTimeUtil.timeValue2Secs = function(tval)
{
	var totalDays;
	var leapDays;
	var yearDiff;
	var yearDiff100;
	var yearDiff400;

	var currYear = tval.year;
	var currMonth = tval.month;
	var currDay = tval.day;

	if (currYear <= 2000)
	{
		yearDiff = 2000 - currYear;
	}
	else
	{
		yearDiff = currYear - 2000 - 1;
	}
	yearDiff100 = Math.floor(yearDiff / 100);
	yearDiff400 = yearDiff100 >> 2;
	yearDiff >>= 2;
	leapDays = yearDiff - yearDiff100 + yearDiff400;

	if (currYear <= 2000)
	{
		totalDays = 10957 - (2000 - currYear) * 365 - leapDays;
	}
	else
	{
		totalDays = 10958 + (currYear - 2000) * 365 + leapDays;
	}

	switch (currMonth)
	{
	case 12:
		totalDays += 30;
	case 11:
		totalDays += 31;
	case 10:
		totalDays += 30;
	case 9:
		totalDays += 31;
	case 8:
		totalDays += 31;
	case 7:
		totalDays += 30;
	case 6:
		totalDays += 31;
	case 5:
		totalDays += 30;
	case 4:
		totalDays += 31;
	case 3:
		if (data.DateTimeUtil.isYearLeap(tval.year))
			totalDays += 29;
		else
			totalDays += 28;
	case 2:
		totalDays += 31;
	case 1:
		break;
	default:
		break;
	}
	totalDays += currDay - 1;

	return totalDays * 86400 + (tval.second + tval.minute * 60 + tval.hour * 3600 - tval.tzQhr * 900);
}

data.DateTimeUtil.timeValue2Ticks = function(t)
{
	return data.DateTimeUtil.timeValue2Secs(t) * 1000 + Math.floor(t.nanosec / 1000000);
}

data.DateTimeUtil.ticks2TimeValue = function(ticks, tzQhr)
{
	var t = Secs2TimeValue(ticks / 1000, tzQhr);
	t.nanosec = (ticks % 1000) * 1000000;
	return t;
}

data.DateTimeUtil.secs2TimeValue = function(secs, tzQhr)
{
	return data.DateTimeUtil.instant2TimeValue(secs, 0, tzQhr);
}
data.DateTimeUtil.instant2TimeValue = function(secs, nanosec, tzQhr)
{
	secs = secs + tzQhr * 900;
	var totalDays = Math.floor(secs / 86400);
	var minutes;
	if (secs < 0)
	{
		secs -= totalDays * 86400;
		while (secs < 0)
		{
			totalDays -= 1;
			secs += 86400;
		}
		minutes = (secs % 86400);
	}
	else
	{
		minutes = (secs % 86400);
	}

	var t = new Object();
	t.second = (minutes % 60);
	minutes = Math.floor(minutes / 60);
	t.minute = (minutes % 60);
	t.hour = Math.floor(minutes / 60);
	t.nanosec = nanosec;
	t.tzQhr = tzQhr;

	if (totalDays < 0)
	{
		t.year = 1970;
		while (totalDays < 0)
		{
			t.year--;
			if (data.DateTimeUtil.isYearLeap(t.year))
			{
				totalDays += 366;
			}
			else
			{
				totalDays += 365;
			}
		}
	}
	else
	{
		if (totalDays < 10957)
		{
			t.year = 1970;
			while (true)
			{
				if (data.DateTimeUtil.isYearLeap(t.year))
				{
					if (totalDays < 366)
					{
						break;
					}
					else
					{
						t.year++;
						totalDays -= 366;
					}
				}
				else
				{
					if (totalDays < 365)
					{
						break;
					}
					else
					{
						t.year++;
						totalDays -= 365;
					}
				}
			}
		}
		else
		{
			totalDays -= 10957;
			t.year = (2000 + (Math.floor(totalDays / 1461) * 4));
			totalDays = totalDays % 1461;
			if (totalDays >= 366)
			{
				totalDays--;
				t.year = (t.year + Math.floor(totalDays / 365));
				totalDays = totalDays % 365;
			}
		}
	}

	if (data.DateTimeUtil.isYearLeap(t.year))
	{
		if (totalDays < 121)
		{
			if (totalDays < 60)
			{
				if (totalDays < 31)
				{
					t.month = 1;
					t.day = (totalDays + 1);
				}
				else
				{
					t.month = 2;
					t.day = (totalDays - 31 + 1);
				}
			}
			else
			{
				if (totalDays < 91)
				{
					t.month = 3;
					t.day = (totalDays - 60 + 1);
				}
				else
				{
					t.month = 4;
					t.day = (totalDays - 91 + 1);
				}
			}
		}
		else
		{
			if (totalDays < 244)
			{
				if (totalDays < 182)
				{
					if (totalDays < 152)
					{
						t.month = 5;
						t.day = (totalDays - 121 + 1);
					}
					else
					{
						t.month = 6;
						t.day = (totalDays - 152 + 1);
					}
				}
				else
				{
					if (totalDays < 213)
					{
						t.month = 7;
						t.day = (totalDays - 182 + 1);
					}
					else
					{
						t.month = 8;
						t.day = (totalDays - 213 + 1);
					}
				}
			}
			else
			{
				if (totalDays < 305)
				{
					if (totalDays < 274)
					{
						t.month = 9;
						t.day = (totalDays - 244 + 1);
					}
					else
					{
						t.month = 10;
						t.day = (totalDays - 274 + 1);
					}
				}
				else
				{
					if (totalDays < 335)
					{
						t.month = 11;
						t.day = (totalDays - 305 + 1);
					}
					else
					{
						t.month = 12;
						t.day = (totalDays - 335 + 1);
					}
				}
			}
		}
	}
	else
	{
		if (totalDays < 120)
		{
			if (totalDays < 59)
			{
				if (totalDays < 31)
				{
					t.month = 1;
					t.day = (totalDays + 1);
				}
				else
				{
					t.month = 2;
					t.day = (totalDays - 31 + 1);
				}
			}
			else
			{
				if (totalDays < 90)
				{
					t.month = 3;
					t.day = (totalDays - 59 + 1);
				}
				else
				{
					t.month = 4;
					t.day = (totalDays - 90 + 1);
				}
			}
		}
		else
		{
			if (totalDays < 243)
			{
				if (totalDays < 181)
				{
					if (totalDays < 151)
					{
						t.month = 5;
						t.day = (totalDays - 120 + 1);
					}
					else
					{
						t.month = 6;
						t.day = (totalDays - 151 + 1);
					}
				}
				else
				{
					if (totalDays < 212)
					{
						t.month = 7;
						t.day = (totalDays - 181 + 1);
					}
					else
					{
						t.month = 8;
						t.day = (totalDays - 212 + 1);
					}
				}
			}
			else
			{
				if (totalDays < 304)
				{
					if (totalDays < 273)
					{
						t.month = 9;
						t.day = (totalDays - 243 + 1);
					}
					else
					{
						t.month = 10;
						t.day = (totalDays - 273 + 1);
					}
				}
				else
				{
					if (totalDays < 334)
					{
						t.month = 11;
						t.day = (totalDays - 304 + 1);
					}
					else
					{
						t.month = 12;
						t.day = (totalDays - 334 + 1);
					}
				}
			}
		}
	}
	return t;
}

data.DateTimeUtil.toString = function(tval, pattern)
{
	var output = new Array();
	var i = 0;
	while (i < pattern.length)
	{
		switch (pattern.charAt(i))
		{
		case 'y':
		{
			var thisVal = tval.year;
			var digiCnt = 1;
			i++;
			while (i < pattern.length && pattern.charAt(i) == 'y')
			{
				digiCnt++;
				i++;
			}
			var s = ""+thisVal;
			if (s.length >= digiCnt)
			{
				output.push(s.substr(s.length - digiCnt));
			}
			else
			{
				while (s.length < digiCnt)
				{
					s = "0"+digiCnt;
				}
				output.push(s);
			}
			break;
		}
		case 'm':
		{
			i++;
			if (i >= pattern.length || pattern.charAt(i) != 'm')
			{
				output.push(""+tval.minute);
			}
			else
			{
				if (tval.minute < 10)
				{
					output.push("0"+tval.minute);
				}
				else
				{
					output.push(""+tval.minute);
				}
				i++;
//				while (*pattern == 'm')
//					pattern++;
			}
			break;
		}
		case 's':
		{
			i++;
			if (i >= pattern.length || pattern.charAt(i) != 's')
			{
				output.push(""+tval.second);
			}
			else
			{
				if (tval.second < 10)
				{
					output.push("0"+tval.second);
				}
				else
				{
					output.push(""+tval.second);
				}
				i++;
//				while (*pattern == 's')
//					pattern++;
			}
			break;
		}
		case 'd':
		{
			i++;
			if (i >= pattern.length || pattern.charAt(i) != 'd')
			{
				output.push(""+tval.day);
			}
			else
			{
				if (tval.day < 10)
				{
					output.push("0"+tval.day);
				}
				else
				{
					output.push(""+tval.day);
				}
				i++;
//				while (*pattern == 'd')
//					pattern++;
			}
			break;
		}
		case 'f':
		{
			if (i + 1 >= pattern.length || pattern.charAt(i + 1) != 'f')
			{
				output.push(""+Math.floor(tval.nanosec / 100000000));
				i += 1;
			}
			else if (i + 2 >= pattern.length || pattern.charAt(i + 2) != 'f')
			{
				output.push(zpadStr(Math.floor(tval.nanosec / 10000000), 2));
				i += 2;
			}
			else if (i + 3 >= pattern.length || pattern.charAt(i + 3) != 'f')
			{
				output.push(zpadStr(Math.floor(tval.nanosec / 1000000), 3));
				i += 3;
			}
			else if (i + 4 >= pattern.length || pattern.charAt(i + 4) != 'f')
			{
				output.push(zpadStr(Math.floor(tval.nanosec / 100000), 4));
				i += 4;
			}
			else if (i + 5 >= pattern.length || pattern.charAt(i + 5) != 'f')
			{
				output.push(zpadStr(Math.floor(tval.nanosec / 10000), 5));
				i += 5;
			}
			else if (i + 6 >= pattern.length || pattern.charAt(i + 6) != 'f')
			{
				output.push(zpadStr(Math.floor(tval.nanosec / 1000), 6));
				i += 6;
			}
			else if (i + 7 >= pattern.length || pattern.charAt(i + 7) != 'f')
			{
				output.push(zpadStr(Math.floor(tval.nanosec / 100), 7));
				i += 7;
			}
			else if (i + 8 >= pattern.length || pattern.charAt(i + 8) != 'f')
			{
				output.push(zpadStr(Math.floor(tval.nanosec / 10), 8));
				i += 8;
			}
			else
			{
				output.push(""+zpadStr(tval.nanosec, 9));
				i += 9;
				while (i < pattern.length && pattern.charAt(i) == 'f')
				{
					output.push('0');
					i++;
				}
			}
			break;
		}
		case 'F':
		{
			var digiCnt;
			var thisMS;
			if (i + 1 >= pattern.length || pattern.charAt(i + 1) != 'F')
			{
				digiCnt = 1;
				thisMS = Math.floor(tval.nanosec / 100000000);
				i += 1;
			}
			else if (pattern[2] != 'F')
			{
				digiCnt = 2;
				thisMS = Math.floor(tval.nanosec / 10000000);
				i += 2;
			}
			else
			{
				digiCnt = 3;
				thisMS = Math.floor(tval.nanosec / 1000000);
				i += 3;
			}

			while ((thisMS % 10) == 0)
			{
				thisMS = Math.floor(thisMS / 10);
				if (--digiCnt <= 0)
					break;
			}
			if (digiCnt > 0)
				output.push(zpadStr(thisMS, digiCnt));
			break;
		}
		case 'h':
		{
			var thisH = tval.hour % 12;
			if (thisH == 0)
			{
				thisH = 12;
			}
			i++;
			if (i >= pattern.length || pattern.charAt(i) != 'h')
			{
				output.push(""+thisH);
			}
			else
			{
				output.push(zpadStr(thisH, 2));
				i++;

//				while (*pattern == 'h')
//					pattern++;
			}
			break;
		}
		case 'H':
		{
			i++;
			if (i >= pattern.length || pattern.charAt(i) != 'H')
			{
				output.push(""+tval.hour);
			}
			else
			{
				output.push(zpadStr(tval.hour, 2));
				i++;
//				while (*pattern == 'H')
//					pattern++;
			}
			break;
		}
		case 'M':
		{
			if (i + 1 >= pattern.length || pattern.charAt(i + 1) != 'M')
			{
				output.push(""+tval.month);
				i += 1;
			}
			else if (i + 2 >= pattern.length || pattern.charAt(i + 2) != 'M')
			{
				output.push(zpadStr(tval.month, 2));
				i += 2;
			}
			else if (pattern[3] != 'M')
			{
				output.push(data.DateTimeUtil.monString[tval.month - 1]);
				i += 3;
			}
			else
			{
				output.push(data.DateTimeUtil.monthString[tval.month - 1]);
				i += 4;
				while (i < pattern.length && pattern.charAt(i) == 'M')
					i++;
			}
			break;
		}
		case 't':
		{
			if (i + 1 >= pattern.length || pattern.charAt(i + 1) != 't')
			{
				if (tval.hour >= 12)
				{
					output.push('P');
				}
				else
				{
					output.push('A');
				}
				i += 1;
			}
			else
			{
				if (tval.hour >= 12)
				{
					output.push("PM");
				}
				else
				{
					output.push("AM");
				}
				i += 2;
				while (i < pattern.length && pattern.charAt(i) == 't')
					i++;
			}
			break;
		}
		case 'z':
		{
			var hr = tval.tzQhr >> 2;
			var min = (tval.tzQhr & 3) * 15;
			if (i + 1 >= pattern.length || pattern.charAt(i + 1) != 'z')
			{
				if (hr >= 0)
				{
					output.push("+" + hr);
				}
				else
				{
					output.push("-" + (-hr));
				}
				i++;
			}
			else if (i + 2 >= pattern.length || pattern.charAt(i + 2) != 'z')
			{
				if (hr >= 0)
				{
					output.push("+" + zpadStr(hr, 2));
				}
				else
				{
					output.push("-" + zpadStr(-hr, 2));
				}
				i += 2;
			}
			else if (i + 3 >= pattern.length || pattern.charAt(i + 3) != 'z')
			{
				if (hr >= 0)
				{
					output.push("+" + zpadStr(hr, 2) + zpadStr(min, 2));
				}
				else
				{
					output.push("-" + zpadStr(-hr, 2) + zpadStr(min, 2));
				}
				i += 3;
			}
			else
			{
				if (hr >= 0)
				{
					output.push("+" + zpadStr(hr, 2) + ':' + zpadStr(min, 2));
				}
				else
				{
					output.push("-" + zpadStr(-hr, 2) + ':' + zpadStr(min, 2));
				}
				i += 4;
				while (i < pattern.length && pattern.charAt(i) == 'z')
					i++;
			}
			break;
		}
		case '\\':
		{
			if (i + 1 >= pattern.length)
				output.push('\\');
			else
			{
				output.push(pattern.charAt(i + 1));
				i += 2;
			}
			break;
		}
		default:
			output.push(pattern.charAt(i));
			i++;
			break;
		}
	}
	return output.join("");
}

data.DateTimeUtil.string2TimeValue = function(dateStr, tzQhr)
{
	if (dateStr.length < 4)
		return null;
	if (tzQhr == null)
		tzQhr = data.DateTimeUtil.getLocalTzQhr();
	var tval = new Object();
	tval.nanosec = 0;
	tval.tzQhr = tzQhr;
	var strs2;
	var strs;
	var succ = true;
	if (dateStr.charAt(3) == ',' && dateStr.indexOf(',', 4) == -1)
	{
		dateStr = dateStr.substr(4);
		while (dateStr.charAt(0) == ' ')
			dateStr = dateStr.substr(1);
	}
	strs2 = dateStr.split(" ");
	if (strs2.length == 1)
	{
		strs2 = dateStr.split("T");
	}
	if (strs2.length == 2)
	{
		var dateSucc = true;
		if ((strs = strs2[0].split('-')).length == 3)
		{
			data.DateTimeUtil.timeValueSetDate(tval, strs);
		}
		else if ((strs = strs2[0].split('/')).length == 3)
		{
			data.DateTimeUtil.timeValueSetDate(tval, strs);
		}
		else if ((strs = strs2[0].split(':')).length == 3)
		{
			data.DateTimeUtil.timeValueSetDate(tval, strs);
		}
		else
		{
			tval = data.DateTimeUtil.ticks2TimeValue(new Date().getTime(), tzQhr);
			dateSucc = false;
		}
		var i = strs2[1].indexOf('-');
		if (i == -1)
		{
			i = strs2[1].indexOf('+');
		}
		if (i != -1)
		{
			var c = strs2[1].charAt(i);
			var tz = strs2[1].substr(i + 1);
			if (tz.length == 5)
			{
				var min = strs2[1].substr(i + 4) - 0;
				if (tz.charAt(2) == ':')
				{
					tz = tz.substr(0, 2);
				}
				else
				{
					tz = tz.substr(0, 3);
				}
				min = min + (tz - 0) * 60;
				if (c == '-')
				{
					tval.tzQhr = -min / 15;
				}
				else
				{
					tval.tzQhr = min / 15;
				}
			}
			else if (tzlen == 2)
			{
				if (c == '-')
				{
					tval.tzQhr = -(tz - 0) * 4;
				}
				else
				{
					tval.tzQhr = (tz - 0) * 4;
				}
			}
			strs2[1] = strs2.substr(0, i);
		}
		if ((strs = strs2[1].split(':')).length == 3)
		{
			if (strs[2].endsWith('Z'))
			{
				tval.tzQhr = 0;
				strs[2] = strs[2].substr(0, strs[2].length - 1);
			}
			data.DateTimeUtil.timeValueSetTime(tval, strs);
		}
		else
		{
			tval.hour = 0;
			tval.minute = 0;
			tval.second = 0;
			tval.nanosec = 0;
			if (!dateSucc)
			{
				return null;
			}
		}
	}
	else if (strs2.length == 1)
	{
		if ((strs = strs2[0].split('-')).length == 3)
		{
			data.DateTimeUtil.timeValueSetDate(tval, strs);
			tval.hour = 0;
			tval.minute = 0;
			tval.second = 0;
			tval.nanosec = 0;
		}
		else if ((strs = strs2[0].split('/')).length == 3)
		{
			data.DateTimeUtil.timeValueSetDate(tval, strs);
			tval.hour = 0;
			tval.minute = 0;
			tval.second = 0;
			tval.nanosec = 0;
		}
		else if ((strs = strs2[0].split(':')).length == 3)
		{
			tval = data.DateTimeUtil.ticks2TimeValue(new Date().getTime(), tzQhr);
			data.DateTimeUtil.timeValueSetTime(tval, strs);
		}
		else
		{
			return null;
		}
	}
	else if (strs2.length == 4 || (strs2.length == 5 && (strs2[4].charAt(0) == '-' || strs2[4].charAt(0) == '+' || strs2[4] == "GMT")))
	{
		var len1 = strs2[0].length;
		var len2 = strs2[1].length;
		var len3 = strs2[2].length;
		var len4 = strs2[3].length;
		var timeStr = strs2[3];
		if (len1 == 3 && len2 <= 2 && len3 == 4)
		{
			tval.year = strs2[2] - 0;
			tval.month = data.DateTimeUtil.parseMonthStr(strs2[0]);
			tval.day = strs2[1] - 0;
		}
		else if (len1 <= 2 && len2 == 3 && len3 == 4)
		{
			tval.year = strs2[2] - 0;
			tval.month = data.DateTimeUtil.parseMonthStr(strs2[1]);
			tval.day = strs2[0] - 0;
		}
		else if (len1 == 3 && len2 <= 2 && len4 == 4)
		{
			tval.year = strs2[3] - 0;
			tval.month = data.DateTimeUtil.parseMonthStr(strs2[0]);
			tval.day = strs2[1] - 0;
			timeStr = strs2[2];
		}
		else
		{
			return null;
		}
		if ((strs = timeStr.split(':')).length == 3)
		{
			data.DateTimeUtil.timeValueSetTime(tval, strs);
		}
		else
		{
			return null;
		}
		
		if (strs2.length == 5)
		{
			if (strs2[4] == "GMT")
			{
				tval.tzQhr = 0;
			}
			else if (strs2[4].length == 5)
			{
				var min = strs2[4].substr(3) - 0;
				if (strs2[4].charAt(2) == ':')
				{
					strs2[4] = strs2[4].substr(0, 2);
				}
				else
				{
					strs2[4] = strs2[4].substr(0, 3);
				}
				min = min + (strs2[4].substr(1) - 0) * 60;
				if (strs2[4].charAt(0) == '-')
				{
					tval.tzQhr = (-min / 15);
				}
				else
				{
					tval.tzQhr = (min / 15);
				}
			}
		}
	}
	else
	{
		tval.year = 1970;
		tval.month = 1;
		tval.day = 1;
		tval.hour = 0;
		tval.minute = 0;
		tval.second = 0;

		var j = 0;
		var i;
		while (j < strs2.length)
		{
			if ((strs = strs2[j].split(':')).length == 3)
			{
				data.DateTimeUtil.timeValueSetTime(tval, strs);
			}
			else
			{
				if (strs2[j].charAt(0) == '-')
				{
					if ((strs = strs2[j].split(':')).length == 2)
					{
						tval.tzQhr = -(((strs[0].substr(1) - 0) * 4) + (strs[1] - 0) / 15);
					}
					else if (strs2[j].leng == 5)
					{
						tval.tzQhr = (strs2[j].substr(3) - 0) / 15;
						strs2[j] = strs2[j].substr(0, 3);
						tval.tzQhr = -(tval.tzQhr + (strs2[j].substr(1) - 0) * 4);
					}
				}
				else if (strs2[j].charAt(0) == '+')
				{
					if ((strs = strs2[j].split(':')).length == 2)
					{
						tval.tzQhr = ((strs[0].substr(1) - 0) * 4) + (strs[1] - 0) / 15;
					}
					else if (strs2[j].leng == 5)
					{
						tval.tzQhr = (strs2[j].substr(3) - 0) / 15;
						strs2[j] = strs2[j].substr(0, 3);
						tval.tzQhr = (tval.tzQhr + (strs2[j].substr(1) - 0) * 4);
					}
				}
				else
				{
					i = strs2[j].indexOf('/');
					if (i != -1 && i > 0)
					{
						if ((strs = strs2[j].split('/')).length == 3)
						{
							data.DateTimeUtil.timeValueSetDate(tval, strs);
						}
					}
					else if ((strs = strs2[j].split('-')).length == 3)
					{
						data.DateTimeUtil.timeValueSetDate(tval, strs);
					}
					else if (strs2[j] == "HKT")
					{
						tval.tzQhr = 32;
					}
					else
					{
						i = strs2[j] - 0;
						if (isNaN(i))
						{
							i = data.DateTimeUtil.parseMonthStr(strs2[j]);
							if (i > 0)
							{
								tval.month = i;
							}
						}
						else if (i > 100)
						{
							tval.year = i;
						}
						else
						{
							tval.day = i;
						}
					}
				}
			}
			j++;
		}
	}
	return tval;
}

data.DateTimeUtil.isYearLeap = function(year)
{
	return ((year & 3) == 0) && ((year % 100) != 0 || (year % 400) == 0);
}

data.DateTimeUtil.parseMonthStr = function(month)
{
	if (month.length < 3)
		return 0;
	month = month.toUpperCase();
	if (month.startsWith("JAN"))
	{
		return 1;
	}
	else if (month.startsWith("FEB"))
	{
		return 2;
	}
	else if (month.startsWith("MAR"))
	{
		return 3;
	}
	else if (month.startsWith("APR"))
	{
		return 4;
	}
	else if (month.startsWith("MAY"))
	{
		return 5;
	}
	else if (month.startsWith("JUN"))
	{
		return 6;
	}
	else if (month.startsWith("JUL"))
	{
		return 7;
	}
	else if (month.startsWith("AUG"))
	{
		return 8;
	}
	else if (month.startsWith("SEP"))
	{
		return 9;
	}
	else if (month.startsWith("OCT"))
	{
		return 10;
	}
	else if (month.startsWith("NOV"))
	{
		return 11;
	}
	else if (month.startsWith("DEC"))
	{
		return 12;
	}
	return 0;
}

data.DateTimeUtil.getLocalTzQhr = function()
{
	return new Date().getTimezoneOffset() / -15;
}

data.Duration = function(seconds, nanosec)
{
	this.seconds = seconds;
	this.nanosec = nanosec;
}

data.Duration.fromTicks = function(ticks)
{
	if (ticks < 0)
	{
		var ns = ticks % 1000;
		var seconds = Math.floor(ticks / 1000);
		if (ns != 0)
		{
			ns = (-ns) * 1000000;
			seconds--;
		}
		return new Duration(seconds, ns);
	}
	else
	{
		return new Duration(Math.floor(ticks / 1000), (ticks % 1000) * 1000000);
	}
}

data.Duration.fromUs = function(us)
{
	if (us < 0)
	{
		var ns = us % 1000000;
		var seconds = Math.floor(us / 1000000);
		if (ns != 0)
			return new Duration(seconds, 0);
		else
			return new Duration(seconds - 1, (-ns) * 1000);
	}
	else
	{
		return new Duration(Math.floor(us / 1000000), (us % 1000000) * 1000);
	}
}

data.Duration.prototype.getSeconds = function()
{
	return this.seconds;
}

data.Duration.prototype.getNS = function()
{
	return this.ns;
}

data.Duration.prototype.getTotalMS = function()
{
	return this.seconds * 1000 + Math.floor(this.ns / 1000000);
}

data.Duration.prototype.getTotalSec = function()
{
	return this.seconds + (this.ns * 0.000000001);
}

data.Duration.prototype.notZero = function()
{
	return this.seconds != 0 || this.ns != 0;
}

data.Duration.prototype.isZero = function()
{
	return this.seconds == 0 && this.ns == 0;
}

data.TimeInstant = function(sec, nanosec)
{
	this.sec = sec;
	this.nanosec = nanosec;
}

data.TimeInstant.now = function()
{
	if (window.performance)
	{
		var t1 = performance.now();
		var t2 = performance.timeOrigin;
		var secs = Math.floor((t1 + t2) / 1000);
		var ns = (t1 / 1000) - Math.floor(t1 / 1000) + (t2 / 1000) - Math.floor(t2 / 1000);
		if (ns >= 1)
			ns -= 1;
		return new data.TimeInstant(secs, Math.round(ns * 1000000000));
	}
	else
	{
		return data.TimeInstant.fromTicks(Date.now());
	}
}

data.TimeInstant.fromVariTime = function(variTime)
{
	var days = Math.floor(variTime);
	var ds = (variTime - days);
	var s = Math.floor(ds * 86400);
	return new data.TimeInstant((days - 25569) * 86400000 + Math.floor(ds * 86400000), ((ds * 86400 - s) * 1000000000));
}

data.TimeInstant.fromTicks = function(ticks)
{
	var ms = (ticks % 1000);
	if (ms < 0)
	{
		return new data.TimeInstant(Math.floor(ticks / 1000) - 1, (ms + 1000) * 1000000);
	}
	else
	{
		return new data.TimeInstant(Math.floor(ticks / 1000), ms * 1000000);
	}
}

data.TimeInstant.prototype.addDay = function(val)
{
	return new data.TimeInstant(this.sec + val * 86400, this.nanosec);
}

data.TimeInstant.prototype.addHour = function(val)
{
	return new data.TimeInstant(this.sec + val * 3600, this.nanosec);
}

data.TimeInstant.prototype.addMinute = function(val)
{
	return new data.TimeInstant(this.sec + val * 60, this.nanosec);
}

data.TimeInstant.prototype.addSecond = function(val)
{
	return new data.TimeInstant(this.sec + val, this.nanosec);
}

data.TimeInstant.prototype.addMS = function(val)
{
	var newSec = this.sec + Math.floor(val / 1000);
	val = (val % 1000) * 1000000 + this.nanosec;
	while (val > 1000000000)
	{
		newSec++;
		val -= 1000000000;
	}
	return new data.TimeInstant(newSec, val);
}

data.TimeInstant.prototype.addNS = function(val)
{
	var newSec = this.sec + Math.floor(val / 1000000000);
	val = val % 1000000000 + this.nanosec;
	while (val > 1000000000)
	{
		newSec++;
		val -= 1000000000;
	}
	return new data.TimeInstant(newSec, val);
}

data.TimeInstant.prototype.getMS = function()
{
	return Math.floor(this.nanosec / 1000000);
}

data.TimeInstant.prototype.clearTime = function()
{
	return new data.TimeInstant(this.sec - this.sec % 86400, 0);
}

data.TimeInstant.prototype.roundToS = function()
{
	if (this.nanosec >= 500000000)
	{
		return new data.TimeInstant(this.sec + 1, 0);
	}
	else
	{
		return new data.TimeInstant(this.sec, 0);
	}
}

data.TimeInstant.prototype.getMSPassedDate = function()
{
	return (this.sec % 86400) * 1000 + Math.floor(this.nanosec / 1000000);
}

data.TimeInstant.prototype.diffMS = function(ts)
{
	return (this.sec - ts.sec) * 1000 + Math.floor((this.nanosec / 1000000) - (ts.nanosec / 1000000));
}

data.TimeInstant.prototype.diffSec = function(ts)
{
	return this.sec - ts.sec;
}

data.TimeInstant.prototype.diffSecDbl = function(ts)
{
	return (this.sec - ts.sec) + (this.nanosec - ts.nanosec) / 1000000000.0;
}

data.TimeInstant.prototype.diff = function(ts)
{
	var secs = this.sec - ts.sec;
	var ns1 = this.nanosec;
	var ns2 = ts.nanosec;
	if (ns1 >= ns2)
		return new Duration(secs, ns1 - ns2);
	else
		return new Duration(secs - 1, 1000000000 + ns1 - ns2);
}

data.TimeInstant.prototype.toTicks = function()
{
	return this.sec * 1000 + Math.floor(this.nanosec / 1000000);
}

data.TimeInstant.prototype.toDotNetTicks = function()
{
	return this.sec * 10000000 + 621355968000000000 + Math.floor(this.nanosec / 100);
}

data.TimeInstant.prototype.toUnixTimestamp = function()
{
	return this.sec;
}

data.TimeInstant.prototype.toEpochSec = function()
{
	return this.sec;
}

data.TimeInstant.prototype.toEpochMS = function()
{
	return this.sec * 1000 + Math.floor(this.nanosec / 1000000);
}

data.TimeInstant.prototype.toEpochNS = function()
{
	return this.sec * 1000000000 + this.nanosec;
}

data.Timestamp = function(inst, tzQhr)
{
	this.inst = inst;
	this.tzQhr = tzQhr;
}

data.Timestamp.fromTicks = function(ticks, tzQhr)
{
	return new data.Timestamp(data.TimeInstant.fromTicks(ticks), tzQhr);
}

data.Timestamp.fromStr = function(str, defTzQhr)
{
	var tval = data.DateTimeUtil.string2TimeValue(str, defTzQhr);
	if (tval == 0)
	{
		return new data.Timestamp(new data.TimeInstant(0, 0), defTzQhr);
	}
	else
	{
		return new data.Timestamp(new data.TimeInstant(data.DateTimeUtil.timeValue2Secs(tval), tval.nanosec), tval.tzQhr);
	}
}

data.Timestamp.now = function()
{
	return new data.Timestamp(data.TimeInstant.now(), data.DateTimeUtil.getLocalTzQhr());
}

data.Timestamp.utcNow = function()
{
	return new data.Timestamp(data.TimeInstant.now(), 0);
}

data.Timestamp.fromVariTime = function(variTime)
{
	return new data.Timestamp(data.TimeInstant.fromVariTime(variTime), data.DateTimeUtil.getLocalTzQhr());
}

data.Timestamp.fromSecNS = function(unixTS, nanosec, tzQhr)
{
	return new data.Timestamp(new data.TimeInstant(unixTS, nanosec), tzQhr);
}

data.Timestamp.fromDotNetTicks = function(ticks, tzQhr)
{
	return new data.Timestamp(data.TimeInstant.fromDotNetTicks(ticks), tzQhr);
}

data.Timestamp.fromEpochSec = function(epochSec, tzQhr)
{
	return new data.Timestamp(new data.TimeInstant(epochSec, 0), tzQhr);
}

data.Timestamp.fromEpochMS = function(epochMS, tzQhr)
{
	return new data.Timestamp(epochMS, tzQhr);
}

data.Timestamp.fromEpochUS = function(epochUS, tzQhr)
{
	if (epochUS < 0)
	{
		return new data.Timestamp(new data.TimeInstant(Math.floor(epochUS / 1000000) - 1, (epochUS % 1000000 + 1000000) * 1000), tzQhr);
	}
	else
	{
		return new data.Timestamp(new data.TimeInstant(Math.floor(epochUS / 1000000), (epochUS % 1000000) * 1000), tzQhr);
	}
}

data.Timestamp.fromEpochNS = function(epochNS, tzQhr)
{
	if (epochNS < 0)
	{
		return new data.Timestamp(new data.TimeInstant(Math.floor(epochNS / 1000000000) - 1, (epochNS % 1000000000 + 1000000000)), tzQhr);
	}
	else
	{
		return new data.Timestamp(new data.TimeInstant(Math.floor(epochNS / 1000000000), (epochNS % 1000000000)), tzQhr);
	}
}

data.Timestamp.fromTimeValue = function(tval)
{
	return new data.Timestamp(new data.TimeInstant(data.DateTimeUtil.timeValue2Secs(tval), tval.nanosec), tval.tzQhr);
}

data.Timestamp.fromYMDHMS = function(ymdhms, tzQhr)
{
	var tval = data.DateTimeUtil.timeValueFromYMDHMS(ymdhms);
	if (tval != null)
	{
		return new data.Timestamp(new data.TimeInstant(data.DateTimeUtil.timeValue2Secs(tval), 0), tzQhr);
	}
	else
	{
		return null;
	}
}

data.Timestamp.prototype.addMonth = function(val)
{
	var tval = this.toTimeValue();
	val += tval.month;
	while (val < 1)
	{
		val += 12;
		tval.year--;
	}
	while (val > 12)
	{
		val -= 12;
		tval.year++;
	}
	tval.month = val;
	return new data.Timestamp(new data.TimeInstant(data.DateTimeUtil.timeValue2Secs(tval), this.inst.nanosec), this.tzQhr);
}

data.Timestamp.prototype.addYear = function(val)
{
	var tval = this.toTimeValue();
	tval.year = (tval.year + val);
	return new data.Timestamp(new data.TimeInstant(data.DateTimeUtil.timeValue2Secs(tval), this.inst.nanosec), this.tzQhr);
}

data.Timestamp.prototype.addDay = function(val)
{
	return new data.Timestamp(this.inst.addDay(val), this.tzQhr);
}

data.Timestamp.prototype.addHour = function(val)
{
	return new data.Timestamp(this.inst.addHour(val), this.tzQhr);
}

data.Timestamp.prototype.addMinute = function(val)
{
	return new data.Timestamp(this.inst.addMinute(val), this.tzQhr);
}

data.Timestamp.prototype.addSecond = function(val)
{
	var sec = Math.floor(val);
	var ns = Math.floor((val - sec) * 1000000000);
	return new data.Timestamp(this.inst.addSecond(sec).addNS(ns), this.tzQhr);
}

data.Timestamp.prototype.addMS = function(val)
{
	return new data.Timestamp(this.inst.addMS(val), this.tzQhr);
}

data.Timestamp.prototype.addNS = function(val)
{
	return new data.Timestamp(this.inst.addNS(val), this.tzQhr);
}

data.Timestamp.prototype.getMS = function()
{
	return this.inst.GetMS();
}

data.Timestamp.prototype.clearTimeUTC = function()
{
	return new data.Timestamp(this.inst.clearTime(), this.tzQhr);
}

data.Timestamp.prototype.clearTimeLocal = function()
{
	return new data.Timestamp(this.inst.addMinute(this.tzQhr * 15).clearTime().addMinute(this.tzQhr * -15), this.tzQhr);
}

data.Timestamp.prototype.clearMonthAndDay = function()
{
	var tval = this.toTimeValue();
	tval.month = 1;
	tval.day = 1;
	tval.hour = 0;
	tval.minute = 0;
	tval.second = 0;
	tval.nanosec = 0;
	return data.Timestamp.fromTimeValue(tval);
}

data.Timestamp.prototype.clearDayOfMonth = function()
{
	var tval = this.toTimeValue();
	tval.day = 1;
	tval.hour = 0;
	tval.minute = 0;
	tval.second = 0;
	tval.nanosec = 0;
	return data.Timestamp.fromTimeValue(tval);
}

data.Timestamp.prototype.getMSPassedUTCDate = function()
{
	return this.inst.getMSPassedDate();
}

data.Timestamp.prototype.getMSPassedLocalDate = function()
{
	return this.inst.addSecond(this.tzQhr * 900).getMSPassedDate();
}

data.Timestamp.prototype.diffSec = function(ts)
{
	return this.inst.diffSec(ts.inst);
}

data.Timestamp.prototype.diffMS = function(ts)
{
	return this.inst.diffMS(ts.inst);
}

data.Timestamp.prototype.diffSecDbl = function(ts)
{
	return this.inst.diffSecDbl(ts.inst);
}

data.Timestamp.prototype.diff = function(ts)
{
	return this.inst.diff(ts.inst);
}

data.Timestamp.prototype.toTicks = function()
{
	return this.inst.toTicks();
}

data.Timestamp.prototype.toDotNetTicks = function()
{
	return this.inst.toDotNetTicks();
}

data.Timestamp.prototype.toUnixTimestamp = function()
{
	return this.inst.toUnixTimestamp();
}

data.Timestamp.prototype.toEpochSec = function()
{
	return this.inst.toEpochSec();
}

data.Timestamp.prototype.toEpochMS = function()
{
	return this.inst.toEpochMS();
}

data.Timestamp.prototype.toEpochNS = function()
{
	return this.inst.toEpochNS();
}

data.Timestamp.prototype.toString = function(pattern)
{
	if (pattern == null)
	{
		if (this.inst.nanosec == 0)
		{
			pattern = "yyyy-MM-dd HH:mm:ss zzzz";
		}
		else if (this.inst.nanosec % 1000000 == 0)
		{
			pattern = "yyyy-MM-dd HH:mm:ss.fff zzzz";
		}
		else if (this.inst.nanosec % 1000 == 0)
		{
			pattern = "yyyy-MM-dd HH:mm:ss.ffffff zzzz";
		}
		else
		{
			pattern = "yyyy-MM-dd HH:mm:ss.fffffffff zzzz";
		}
	}
	var tval = data.DateTimeUtil.instant2TimeValue(this.inst.sec, this.inst.nanosec, this.tzQhr);
	return data.DateTimeUtil.toString(tval, pattern);
}

data.Timestamp.prototype.toStringISO8601 = function()
{
	if (this.inst.nanosec == 0)
	{
		return this.toString("yyyy-MM-ddTHH:mm:sszzzz");
	}
	else if (this.inst.nanosec % 1000000 == 0)
	{
		return this.toString("yyyy-MM-ddTHH:mm:ss.fffzzzz");
	}
	else if (this.inst.nanosec % 1000 == 0)
	{
		return this.toString("yyyy-MM-ddTHH:mm:ss.ffffffzzzz");
	}
	else
	{
		return this.toString("yyyy-MM-ddTHH:mm:ss.fffffffffzzzz");
	}
}

data.Timestamp.prototype.toStringNoZone = function()
{
	if (this.inst.nanosec == 0)
	{
		if (((this.inst.sec + this.tzQhr * 900) % 86400) == 0)
		{
			return this.toString("yyyy-MM-dd");
		}
		else
		{
			return this.toString("yyyy-MM-dd HH:mm:ss");
		}
	}
	else if (this.inst.nanosec % 1000000 == 0)
	{
		return this.toString("yyyy-MM-dd HH:mm:ss.fff");
	}
	else if (this.inst.nanosec % 1000 == 0)
	{
		return this.toString("yyyy-MM-dd HH:mm:ss.ffffff");
	}
	else
	{
		return this.toString("yyyy-MM-dd HH:mm:ss.fffffffff");
	}
}

data.Timestamp.prototype.toUTCTime = function()
{
	return new data.Timestamp(this.inst, 0);
}

data.Timestamp.prototype.toLocalTime = function()
{
	return new data.Timestamp(this.inst, data.DateTimeUtil.getLocalTzQhr());
}

data.Timestamp.prototype.convertTimeZoneQHR = function(tzQhr)
{
	return new data.Timestamp(this.inst, tzQhr);
}

data.Timestamp.prototype.setTimeZoneQHR = function(tzQhr)
{
	if (this.tzQhr != tzQhr)
	{
		return new data.Timestamp(this.inst.addSecond((this.tzQhr - tzQhr) * (15 * 60)), tzQhr);
	}
	else
	{
		return this;
	}
}

data.Timestamp.prototype.getTimeZoneQHR = function()
{
	return this.tzQhr;
}

data.Timestamp.prototype.sameDate = function(ts)
{
	return this.inst.sameDate(ts.inst);
}

data.Timestamp.prototype.toTimeValue = function()
{
	return data.DateTimeUtil.instant2TimeValue(this.inst.sec, this.inst.nanosec, this.tzQhr);
}

data.Timestamp.prototype.roundToS = function()
{
	return new data.Timestamp(this.inst.roundToS(), this.tzQhr);
}

export default data;