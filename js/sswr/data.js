import * as text from "./text.js";

export function isArray(o)
{
	return o != null && o.constructor === Array;
}

export function isObject(o)
{
	return o != null && (typeof o) == "object";
}

export function toObjectString(o, lev)
{
	if (lev && lev > 8)
	{
		return "";
	}
	let nextLev;
	if (lev)
	{
		nextLev = lev + 1;
	}
	else
	{
		nextLev = 1;
	}
	let t = typeof o;
	let out;
	let name;
	if (isArray(o))
	{
		out = new Array();
		out.push("[");
		for (name in o)
		{
			out.push(toObjectString(o[name], nextLev));
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
			out.push(toObjectString(o[name], nextLev));
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
	else if (t == "number")
	{
		return o.toString();
	}
	else
	{
		console.log(t);
		return o;
	}
}

export function arrayBuffer2Base64(buff)
{
	return btoa(String.fromCharCode.apply(null, new Uint8Array(buff)));
}

export function arrayBufferEquals(buff1, buff2)
{
	let arr1 = new Uint8Array(buff1);
	let arr2 = new Uint8Array(buff2);
	if (arr1.byteLength != arr2.byteLength)
		return false;
	let i = 0;
	let j = arr1.length;
	while (i < j)
	{
		if (arr1[i] != arr2[i])
			return false;
		i++;
	}
	return true;
}

export function compare(a, b)
{
	if (a == b)
		return 0;
	if (a == null)
		return -1;
	if (b == null)
		return 1;
	let ta = typeof a;
	if (ta == "string")
	{
		return a.localeCompare(b);
	}
	else if (ta == "number")
	{
		if (a > b)
			return 1;
		else if (a < b)
			return -1;
		else
			return 0;
	}
	else if (ta == "object")
	{
		if (a.compareTo)
		{
			return a.compareTo(b);
		}
		return a.toString().localeCompare(b.toString());
	}
	else
	{
		return a.toString().localeCompare(b.toString());
	}
}

export function sort(arr, compareFunc, firstIndex, lastIndex)
{
    let levi = new Array();
    let desni = new Array();

	let index = 0;
	let i = 0;
	let j = 0;
	let left = 0;
	let right = 0;
	let meja = 0;
	let left1 = 0;
	let right1 = 0;
	let temp1 = 0;
	let temp2 = 0;
	let temp = 0;
	if (compareFunc == null)
	{
		compareFunc = compare;
	}
	if (firstIndex == null || lastIndex == null)
	{
		firstIndex = 0;
		lastIndex = arr.length - 1;
	}
    left = firstIndex;
	right = lastIndex;

	while (left < right)
	{
		if (compareFunc(arr[left], arr[right]) > 0)
		{
			temp = arr[left];
			arr[left] = arr[right];
			arr[right] = temp;
		}
		left++; right--;
	}

	index = 0;
	levi[index] = firstIndex;
	desni[index] = lastIndex;

	while ( index >= 0 )
	{
		left = levi[index];
		right = desni[index];
		i = right - left;
		if ( i > 15 )
		{
			temp1 = arr[left];
			j = left + 1;
			while (j <= right)
			{
				temp2 = arr[j];
				if ( compareFunc(temp2, temp1) < 0)
				{
					meja = temp1;
					if ( compareFunc(arr[ (left + right) >> 1], meja) > 0)
					{
						meja = arr[ (left + right) >> 1 ];
					}
					left1 = left;
					right1 = right;
					while (true)
					{
						while ( compareFunc(arr[right1], meja) >= 0) right1--;
						while ( compareFunc(arr[left1], meja) < 0) left1++;
						if (left1 > right1)
							break;

						temp = arr[right1];
						arr[right1--] = arr[left1];
						arr[left1++] = temp;
					}
					desni[index] = --left1;
					levi[++index] = ++right1;
					desni[index] = right;

					index++;
					break;
				}
				else
				{
					temp1 = temp2;
				}
				j++;
			}
		}
		else if ( i != 0 )
		{
			temp1 = arr[left];
			i = left + 1;
			while (i <= right)
			{
				temp2 = arr[i];
				if ( compareFunc(temp1, temp2) > 0)
				{
					arr[i] = temp1;
					j = i - 1;
					while (j > left)
					{
						temp = arr[j-1];
						if ( compareFunc(temp, temp2) > 0)
							arr[j--] = temp;
						else
							break;
					}
					arr[j] = temp2;
				}
				else
				{
					temp1 = temp2;
				}
				i++;
			}
		}
		index--;
	}
}

export function mergeOptions(options, defOptions)
{
	if (options == null)
		options = {};
	let i;
	for (i in defOptions)
	{
		if (options[i] == null)
			options[i] = defOptions[i];
	}
	return options;
}

export function readUInt16(arr, index)
{
	return arr[index] | (arr[index + 1] << 8);
}

export function readMUInt16(arr, index)
{
	return arr[index + 1] | (arr[index + 0] << 8);
}

export function readUInt24(arr, index)
{
	return arr[index] | (arr[index + 1] << 8) | (arr[index + 2] << 16);
}

export function readMUInt24(arr, index)
{
	return arr[index + 2] | (arr[index + 1] << 8) | (arr[index + 0] << 16);
}

export function readUInt32(arr, index)
{
	return arr[index] | (arr[index + 1] << 8) | (arr[index + 2] << 16) | (arr[index + 3] << 24);
}

export function readMUInt32(arr, index)
{
	return arr[index + 3] | (arr[index + 2] << 8) | (arr[index + 1] << 16) | (arr[index + 0] << 24);
}

export function rol32(v, n)
{
	if (n == 0)
		return v;
	return ((v << n) | ((v >> 1) & 0x7fffffff) >> (31 - n));
}

export function ror32(v, n)
{
	if (n == 0)
		return v;
	return ((v << (32 - n)) | ((v >> 1) & 0x7fffffff) >> (n - 1));
}

export function shl32(v, n)
{
	return v << n;
}

export function sar32(v, n)
{
	return v >> n;
}

export function shr32(v, n)
{
	if (n == 0)
		return v;
	return ((v >> 1) & 0x7fffffff) >> (n - 1);
}

export function objectParseTS(o, items)
{
	let item;
	for (item in items)
	{
		if (o[items[item]] && typeof o[items[item]] == "string")
		{
			o[items[item]] = Timestamp.fromStr(o[items[item]]);
		}
	}
}

export class DateValue
{
	constructor()
	{
		this.year = 1970;
		this.month = 1;
		this.day = 1;
	}
}

export class TimeValue extends DateValue
{
	constructor()
	{
		super();
		this.hour = 0;
		this.minute = 0;
		this.second = 0;
		this.nanosec = 0;
		this.tzQhr = 0;
	}
}

export class DateTimeUtil
{
	static monString = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];
	static monthString = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"];
	static dateValueSetDate(t, dateStrs)
	{
		let vals0 = dateStrs[0] - 0;
		let vals1 = dateStrs[1] - 0;
		let vals2 = dateStrs[2] - 0;
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
		else if (vals1 > 12)
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

	static timeValueSetTime(t, timeStrs)
	{
		let strs;

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

	static date2TotalDays(year, month, day)
	{
		let totalDays;
		let leapDays;
		let yearDiff;
		let yearDiff100;
		let yearDiff400;

		let currYear = year;
		let currMonth = month;
		let currDay = day;

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
			if (DateTimeUtil.isYearLeap(year))
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
		return totalDays;
	}

	static dateValue2TotalDays(d)
	{
		return DateTimeUtil.date2TotalDays(d.year, d.month, d.day);
	}

	static timeValue2Secs(tval)
	{
		return BigInt(DateTimeUtil.dateValue2TotalDays(tval)) * 86400n + BigInt(tval.second + tval.minute * 60 + tval.hour * 3600 - tval.tzQhr * 900);
	}

	static timeValue2Ticks(t)
	{
		return DateTimeUtil.timeValue2Secs(t) * 1000n + Math.floor(t.nanosec / 1000000);
	}

	static ticks2TimeValue(ticks, tzQhr)
	{
		return DateTimeUtil.instant2TimeValue(ticks / 1000, (ticks % 1000) * 1000000, tzQhr);
	}

	static secs2TimeValue(secs, tzQhr)
	{
		return DateTimeUtil.instant2TimeValue(secs, 0, tzQhr);
	}

	static totalDays2DateValue(totalDays, t)
	{
		totalDays = BigInt(totalDays);
		if (totalDays < 0)
		{
			t.year = 1970;
			while (totalDays < 0)
			{
				t.year--;
				if (DateTimeUtil.isYearLeap(t.year))
				{
					totalDays += 366n;
				}
				else
				{
					totalDays += 365n;
				}
			}
		}
		else if (totalDays < 10957)
		{
			t.year = 1970;
			while (true)
			{
				if (DateTimeUtil.isYearLeap(t.year))
				{
					if (totalDays < 366)
					{
						break;
					}
					else
					{
						t.year++;
						totalDays -= 366n;
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
						totalDays -= 365n;
					}
				}
			}
		}
		else
		{
			totalDays -= 10957n;
			t.year = (2000 + (Math.floor(Number(totalDays) / 1461) * 4));
			totalDays = totalDays % 1461n;
			if (totalDays >= 366n)
			{
				totalDays--;
				t.year = (t.year + Math.floor(Number(totalDays) / 365));
				totalDays = totalDays % 365n;
			}
		}

		if (DateTimeUtil.isYearLeap(t.year))
		{
			if (totalDays < 121)
			{
				if (totalDays < 60)
				{
					if (totalDays < 31)
					{
						t.month = 1;
						t.day = (Number(totalDays) + 1);
					}
					else
					{
						t.month = 2;
						t.day = (Number(totalDays) - 31 + 1);
					}
				}
				else
					if (totalDays < 91)
					{
						t.month = 3;
						t.day = (Number(totalDays) - 60 + 1);
					}
					else
					{
						t.month = 4;
						t.day = (Number(totalDays) - 91 + 1);
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
							t.day = (Number(totalDays) - 121 + 1);
						}
						else
						{
							t.month = 6;
							t.day = (Number(totalDays) - 152 + 1);
						}
					}
					else
						if (totalDays < 213)
						{
							t.month = 7;
							t.day = (Number(totalDays) - 182 + 1);
						}
						else
						{
							t.month = 8;
							t.day = (Number(totalDays) - 213 + 1);
						}
				}
				else
				{
					if (totalDays < 305)
					{
						if (totalDays < 274)
						{
							t.month = 9;
							t.day = (Number(totalDays) - 244 + 1);
						}
						else
						{
							t.month = 10;
							t.day = (Number(totalDays) - 274 + 1);
						}
					}
					else
						if (totalDays < 335)
						{
							t.month = 11;
							t.day = (Number(totalDays) - 305 + 1);
						}
						else
						{
							t.month = 12;
							t.day = (Number(totalDays) - 335 + 1);
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
						t.day = (Number(totalDays) + 1);
					}
					else
					{
						t.month = 2;
						t.day = (Number(totalDays) - 31 + 1);
					}
				}
				else
					if (totalDays < 90)
					{
						t.month = 3;
						t.day = (Number(totalDays) - 59 + 1);
					}
					else
					{
						t.month = 4;
						t.day = (Number(totalDays) - 90 + 1);
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
							t.day = (Number(totalDays) - 120 + 1);
						}
						else
						{
							t.month = 6;
							t.day = (Number(totalDays) - 151 + 1);
						}
					}
					else
						if (totalDays < 212)
						{
							t.month = 7;
							t.day = (Number(totalDays) - 181 + 1);
						}
						else
						{
							t.month = 8;
							t.day = (Number(totalDays) - 212 + 1);
						}
				}
				else
				{
					if (totalDays < 304)
					{
						if (totalDays < 273)
						{
							t.month = 9;
							t.day = (Number(totalDays) - 243 + 1);
						}
						else
						{
							t.month = 10;
							t.day = (Number(totalDays) - 273 + 1);
						}
					}
					else
						if (totalDays < 334)
						{
							t.month = 11;
							t.day = (Number(totalDays) - 304 + 1);
						}
						else
						{
							t.month = 12;
							t.day = (Number(totalDays) - 334 + 1);
						}
				}
			}
		}
	}

	static instant2TimeValue(secs, nanosec, tzQhr)
	{
		if (tzQhr == null)
			tzQhr = DateTimeUtil.getLocalTzQhr();
		secs = BigInt(secs) + BigInt(tzQhr * 900);
		let totalDays = secs / 86400n;
		let minutes;
		if (secs < 0)
		{
			secs -= totalDays * 86400n;
			while (secs < 0)
			{
				totalDays -= 1;
				secs += 86400n;
			}
			minutes = Number(secs % 86400n);
		}
		else
		{
			minutes = Number(secs % 86400n);
		}

		let t = new TimeValue();
		t.second = (minutes % 60);
		minutes = Math.floor(minutes / 60);
		t.minute = (minutes % 60);
		t.hour = Math.floor(minutes / 60);
		t.nanosec = nanosec;
		t.tzQhr = tzQhr;
		DateTimeUtil.totalDays2DateValue(totalDays, t);
		return t;
	}

	static toString(tval, pattern)
	{
		let output = new Array();
		let i = 0;
		while (i < pattern.length)
		{
			switch (pattern.charAt(i))
			{
			case 'y':
			{
				let thisVal = tval.year;
				let digiCnt = 1;
				i++;
				while (i < pattern.length && pattern.charAt(i) == 'y')
				{
					digiCnt++;
					i++;
				}
				let s = ""+thisVal;
				if (s.length >= digiCnt)
				{
					output.push(s.substr(s.length - digiCnt));
				}
				else
				{
					while (s.length < digiCnt)
					{
						s = "0"+s;
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
					output.push(text.zeroPad(Math.floor(tval.nanosec / 10000000), 2));
					i += 2;
				}
				else if (i + 3 >= pattern.length || pattern.charAt(i + 3) != 'f')
				{
					output.push(text.zeroPad(Math.floor(tval.nanosec / 1000000), 3));
					i += 3;
				}
				else if (i + 4 >= pattern.length || pattern.charAt(i + 4) != 'f')
				{
					output.push(text.zeroPad(Math.floor(tval.nanosec / 100000), 4));
					i += 4;
				}
				else if (i + 5 >= pattern.length || pattern.charAt(i + 5) != 'f')
				{
					output.push(text.zeroPad(Math.floor(tval.nanosec / 10000), 5));
					i += 5;
				}
				else if (i + 6 >= pattern.length || pattern.charAt(i + 6) != 'f')
				{
					output.push(text.zeroPad(Math.floor(tval.nanosec / 1000), 6));
					i += 6;
				}
				else if (i + 7 >= pattern.length || pattern.charAt(i + 7) != 'f')
				{
					output.push(text.zeroPad(Math.floor(tval.nanosec / 100), 7));
					i += 7;
				}
				else if (i + 8 >= pattern.length || pattern.charAt(i + 8) != 'f')
				{
					output.push(text.zeroPad(Math.floor(tval.nanosec / 10), 8));
					i += 8;
				}
				else
				{
					output.push(""+text.zeroPad(tval.nanosec, 9));
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
				let digiCnt;
				let thisMS;
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
					output.push(text.zeroPad(thisMS, digiCnt));
				break;
			}
			case 'h':
			{
				let thisH = tval.hour % 12;
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
					output.push(text.zeroPad(thisH, 2));
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
					output.push(text.zeroPad(tval.hour, 2));
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
					output.push(text.zeroPad(tval.month, 2));
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
				let hr = tval.tzQhr >> 2;
				let min = (tval.tzQhr & 3) * 15;
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
						output.push("+" + text.zeroPad(hr, 2));
					}
					else
					{
						output.push("-" + text.zeroPad(-hr, 2));
					}
					i += 2;
				}
				else if (i + 3 >= pattern.length || pattern.charAt(i + 3) != 'z')
				{
					if (hr >= 0)
					{
						output.push("+" + text.zeroPad(hr, 2) + text.zeroPad(min, 2));
					}
					else
					{
						output.push("-" + text.zeroPad(-hr, 2) + text.zeroPad(min, 2));
					}
					i += 3;
				}
				else
				{
					if (hr >= 0)
					{
						output.push("+" + text.zeroPad(hr, 2) + ':' + text.zeroPad(min, 2));
					}
					else
					{
						output.push("-" + text.zeroPad(-hr, 2) + ':' + text.zeroPad(min, 2));
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

	static string2TimeValue(dateStr, tzQhr)
	{
		if (dateStr.length < 4)
			return null;
		if (tzQhr == null)
			tzQhr = DateTimeUtil.getLocalTzQhr();
		let tval = new TimeValue();
		tval.nanosec = 0;
		tval.tzQhr = tzQhr;
		let strs2;
		let strs;
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
			let dateSucc = true;
			if ((strs = strs2[0].split('-')).length == 3)
			{
				DateTimeUtil.dateValueSetDate(tval, strs);
			}
			else if ((strs = strs2[0].split('/')).length == 3)
			{
				DateTimeUtil.dateValueSetDate(tval, strs);
			}
			else if ((strs = strs2[0].split(':')).length == 3)
			{
				DateTimeUtil.dateValueSetDate(tval, strs);
			}
			else
			{
				tval = DateTimeUtil.ticks2TimeValue(new Date().getTime(), tzQhr);
				dateSucc = false;
			}
			let i = strs2[1].indexOf('-');
			if (i == -1)
			{
				i = strs2[1].indexOf('+');
			}
			if (i != -1)
			{
				let c = strs2[1].charAt(i);
				let tz = strs2[1].substr(i + 1);
				if (tz.length == 5)
				{
					let min = strs2[1].substr(i + 4) - 0;
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
				DateTimeUtil.timeValueSetTime(tval, strs);
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
				DateTimeUtil.dateValueSetDate(tval, strs);
				tval.hour = 0;
				tval.minute = 0;
				tval.second = 0;
				tval.nanosec = 0;
			}
			else if ((strs = strs2[0].split('/')).length == 3)
			{
				DateTimeUtil.dateValueSetDate(tval, strs);
				tval.hour = 0;
				tval.minute = 0;
				tval.second = 0;
				tval.nanosec = 0;
			}
			else if ((strs = strs2[0].split(':')).length == 3)
			{
				tval = DateTimeUtil.ticks2TimeValue(new Date().getTime(), tzQhr);
				DateTimeUtil.timeValueSetTime(tval, strs);
			}
			else
			{
				return null;
			}
		}
		else if (strs2.length == 4 || (strs2.length == 5 && (strs2[4].charAt(0) == '-' || strs2[4].charAt(0) == '+' || strs2[4] == "GMT")))
		{
			let len1 = strs2[0].length;
			let len2 = strs2[1].length;
			let len3 = strs2[2].length;
			let len4 = strs2[3].length;
			let timeStr = strs2[3];
			if (len1 == 3 && len2 <= 2 && len3 == 4)
			{
				tval.year = DateTimeUtil.parseYearStr(strs2[2]);
				tval.month = DateTimeUtil.parseMonthStr(strs2[0]);
				tval.day = strs2[1] - 0;
			}
			else if (len1 <= 2 && len2 == 3 && len3 == 4)
			{
				tval.year = DateTimeUtil.parseYearStr(strs2[2]);
				tval.month = DateTimeUtil.parseMonthStr(strs2[1]);
				tval.day = strs2[0] - 0;
			}
			else if (len1 == 3 && len2 <= 2 && len4 == 4)
			{
				tval.year = DateTimeUtil.parseYearStr(strs2[3]);
				tval.month = DateTimeUtil.parseMonthStr(strs2[0]);
				tval.day = strs2[1] - 0;
				timeStr = strs2[2];
			}
			else
			{
				return null;
			}
			if ((strs = timeStr.split(':')).length == 3)
			{
				DateTimeUtil.timeValueSetTime(tval, strs);
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
					let min = strs2[4].substr(3) - 0;
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

			let j = 0;
			let i;
			while (j < strs2.length)
			{
				if ((strs = strs2[j].split(':')).length == 3)
				{
					DateTimeUtil.timeValueSetTime(tval, strs);
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
								DateTimeUtil.dateValueSetDate(tval, strs);
							}
						}
						else if ((strs = strs2[j].split('-')).length == 3)
						{
							DateTimeUtil.dateValueSetDate(tval, strs);
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
								i = DateTimeUtil.parseMonthStr(strs2[j]);
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

	static isYearLeap(year)
	{
		return ((year & 3) == 0) && ((year % 100) != 0 || (year % 400) == 0);
	}

	static parseYearStr(year)
	{
		let y = year - 0;
		if (y > 0)
			return y;
		return y + 1;
	}

	static parseMonthStr(month)
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

	static getLocalTzQhr()
	{
		return new Date().getTimezoneOffset() / -15;
	}
}

export class Duration
{
	constructor(seconds, nanosec)
	{
		this.seconds = BigInt(seconds);
		this.nanosec = nanosec;
	}

	static fromTicks(ticks)
	{
		if (ticks < 0)
		{
			let ns = Number(BigInt(ticks) % 1000n);
			let seconds = BigInt(ticks) / 1000n;
			if (ns != 0)
			{
				ns = (-ns) * 1000000;
				seconds--;
			}
			return new Duration(seconds, ns);
		}
		else
		{
			return new Duration(BigInt(ticks) / 1000n, Number(BigInt(ticks) % 1000n) * 1000000);
		}
	}

	static fromUs(us)
	{
		if (us < 0)
		{
			let ns = Number(BigInt(us) % 1000000n);
			let seconds = BigInt(us) / 1000000n;
			if (ns == 0)
				return new Duration(seconds, 0);
			else
				return new Duration(seconds - 1n, (-ns) * 1000);
		}
		else
		{
			return new Duration(BigInt(us) / 1000000n, Number(BigInt(us) % 1000000n) * 1000);
		}
	}

	getSeconds()
	{
		return this.seconds;
	}

	getNS()
	{
		return this.ns;
	}

	getTotalMS()
	{
		return Number(this.seconds) * 1000 + this.ns / 1000000n;
	}

	getTotalSec()
	{
		return Number(this.seconds) + (this.ns * 0.000000001);
	}

	notZero()
	{
		return this.seconds != 0 || this.ns != 0;
	}

	isZero()
	{
		return this.seconds == 0 && this.ns == 0;
	}

	toString()
	{
		let txt = new Array();
		let secs = this.seconds;
		let ns = this.nanosec;
		if (secs < 0)
		{
			txt.push('-');
			if (ns > 0)
			{
				secs = -secs - 1n;
				ns = 1000000000 - ns;
			}
			else
			{
				secs = -secs;
			}
		}
		txt.push((secs / 3600n).toString());
		secs = secs % 3600n;
		txt.push(':');
		if (secs < 600)
		{
			txt.push('0');
		}
		txt.push((secs / 60n).toString());
		secs = secs % 60n;
		txt.push(':');
		if (secs < 10)
		{
			txt.push('0');
		}
		txt.push(secs.toString());
		if (ns != 0)
		{
			txt.push('.');
			let s = ""+ns;
			while (s.length < 9) s = "0"+s;
			while (s.endsWith("0")) s = s.substring(0, s.length - 1);
			txt.push(s);
		}
		return txt.join("");
	}
}

export class LocalDate
{
	static DATE_NULL = -1234567;

	constructor(year, month, day)
	{
		if (year == null)
		{
			this.dateVal = LocalDate.DATE_NULL;
		}
		else
		{
			let t = typeof year;
			if (t == "number")
			{
				if (month != null && day != null)
				{
					this.dateVal = DateTimeUtil.date2TotalDays(year, month, day);
				}
				else
				{
					this.dateVal = year;
				}
			}
			else if (t == "string" && text.isInteger(year))
			{
				if (month != null && day != null)
				{
					this.dateVal = DateTimeUtil.date2TotalDays(year - 0, month - 0, day - 0);
				}
				else
				{
					this.dateVal = year;
				}
			}
			else if (t == "DateValue" || t == "TimeValue")
			{
				this.dateVal = DateTimeUtil.dateValue2TotalDays(year);
			}
			else
			{
				let tval = DateTimeUtil.string2TimeValue(year, 0);
				if (tval == null)
				{
					this.dateVal = LocalDate.DATE_NULL;
				}
				else
				{
					this.dateVal = DateTimeUtil.dateValue2TotalDays(tval);	
				}
			}
		}
	}

	setValue(year, month, day)
	{
		this.dateVal = DateTimeUtil.date2TotalDays(year, month, day);
	}

	getDateValue()
	{
		let d = new DateValue();
		DateTimeUtil.totalDays2DateValue(this.dateVal, d);
		return d;
	}

	getTotalDays()
	{
		return this.dateVal;
	}

	setYear(year)
	{
		let d = new DateValue();
		DateTimeUtil.totalDays2DateValue(this.dateVal, d);
		this.dateVal = DateTimeUtil.date2TotalDays(year, d.month, d.day);
	}

	setMonth(month)
	{
		let d = new DateValue();
		DateTimeUtil.totalDays2DateValue(this.dateVal, d);
		this.dateVal = DateTimeUtil.date2TotalDays(d.year, month, d.day);
	}

	setDay(day)
	{
		let d = new DateValue();
		DateTimeUtil.totalDays2DateValue(this.dateVal, d);
		this.dateVal = DateTimeUtil.date2TotalDays(d.year, d.month, day);
	}

	isYearLeap()
	{
		let d = new DateValue();
		DateTimeUtil.totalDays2DateValue(this.dateVal, d);
		return DateTimeUtil.isYearLeap(d.year);
	}

	toTicks()
	{
		return this.dateVal * 86400000;
	}

	toString(pattern)
	{
		if (pattern == null)
			pattern = "yyyy-MM-dd";
		let t = new TimeValue();
		DateTimeUtil.totalDays2DateValue(this.dateVal, t);
		return DateTimeUtil.toString(t, pattern);
	}

	compareTo(obj)
	{
		if (this.dateVal > obj.dateVal)
		return 1;
	else if (this.dateVal < obj.dateVal)
		return -1;
	else
		return 0;
	}

	isNull()
	{
		return this.dateVal == LocalDate.DATE_NULL;
	}

	static today()
	{
		let d = new Date();
		return new LocalDate(d.getFullYear(), d.getMonth() + 1, d.getDate());
	}

	static fromStr(s)
	{
		let timeVal = DateTimeUtil.string2TimeValue(s, 0);
		if (timeVal)
		{
			return new LocalDate(timeVal.year, timeVal.month, timeVal.day);
		}
		return null;
	}
}

export class TimeInstant
{
	constructor(sec, nanosec)
	{
		this.sec = BigInt(sec)
		this.nanosec = nanosec;
	}

	static now()
	{
		if (window.performance)
		{
			let t1 = performance.now();
			let t2 = performance.timeOrigin;
			let secs = Math.floor((t1 + t2) / 1000);
			let ns = (t1 / 1000) - Math.floor(t1 / 1000) + (t2 / 1000) - Math.floor(t2 / 1000);
			if (ns >= 1)
				ns -= 1;
			return new TimeInstant(secs, Math.round(ns * 1000000000));
		}
		else
		{
			return TimeInstant.fromTicks(Date.now());
		}
	}

	static fromVariTime(variTime)
	{
		let days = Math.floor(variTime);
		let ds = (variTime - days);
		let s = Math.floor(ds * 86400);
		return new TimeInstant((days - 25569n) * 86400000n + Math.floor(ds * 86400000n), ((ds * 86400n - s) * 1000000000n));
	}

	static fromTicks(ticks)
	{
		let ms = (ticks % 1000);
		if (ms < 0)
		{
			return new TimeInstant(Math.floor(ticks / 1000) - 1, (ms + 1000) * 1000000);
		}
		else
		{
			return new TimeInstant(Math.floor(ticks / 1000), ms * 1000000);
		}
	}

	addDay(val)
	{
		return new TimeInstant(this.sec + val * 86400n, this.nanosec);
	}

	addHour(val)
	{
		return new TimeInstant(this.sec + val * 3600n, this.nanosec);
	}

	addMinute(val)
	{
		return new TimeInstant(this.sec + val * 60n, this.nanosec);
	}

	addSecond(val)
	{
		return new TimeInstant(this.sec + val, this.nanosec);
	}

	addMS(val)
	{
		let newSec = this.sec + Math.floor(val / 1000);
		val = (val % 1000) * 1000000 + this.nanosec;
		while (val > 1000000000)
		{
			newSec++;
			val -= 1000000000;
		}
		return new TimeInstant(newSec, val);
	}

	addNS(val)
	{
		let newSec = this.sec + Math.floor(val / 1000000000);
		val = val % 1000000000 + this.nanosec;
		while (val > 1000000000)
		{
			newSec++;
			val -= 1000000000;
		}
		return new TimeInstant(newSec, val);
	}

	getMS()
	{
		return Math.floor(this.nanosec / 1000000);
	}

	clearTime()
	{
		return new TimeInstant(this.sec - this.sec % 86400, 0);
	}

	roundToS()
	{
		if (this.nanosec >= 500000000)
		{
			return new TimeInstant(this.sec + 1, 0);
		}
		else
		{
			return new TimeInstant(this.sec, 0);
		}
	}

	getMSPassedDate()
	{
		return Number(this.sec % 86400) * 1000 + Math.floor(this.nanosec / 1000000);
	}

	diffMS(ts)
	{
		return Number(this.sec - ts.sec) * 1000 + Math.floor((this.nanosec / 1000000) - (ts.nanosec / 1000000));
	}

	diffSec(ts)
	{
		return Number(this.sec - ts.sec);
	}

	diffSecDbl(ts)
	{
		return Number(this.sec - ts.sec) + (this.nanosec - ts.nanosec) / 1000000000.0;
	}

	diff(ts)
	{
		let secs = this.sec - ts.sec;
		let ns1 = this.nanosec;
		let ns2 = ts.nanosec;
		if (ns1 >= ns2)
			return new Duration(secs, ns1 - ns2);
		else
			return new Duration(secs - 1n, 1000000000 + ns1 - ns2);
	}

	toTicks()
	{
		return Number(this.sec) * 1000 + Math.floor(this.nanosec / 1000000);
	}

	toDotNetTicks()
	{
		return this.sec * 10000000n + 621355968000000000n + BigInt(Math.floor(this.nanosec / 100));
	}

	toUnixTimestamp()
	{
		return Number(this.sec);
	}

	toEpochSec()
	{
		return this.sec;
	}

	toEpochMS()
	{
		return this.sec * 1000 + BigInt(Math.floor(this.nanosec / 1000000));
	}

	toEpochNS()
	{
		return this.sec * 1000000000 + BigInt(this.nanosec);
	}
}

export class Timestamp
{
	constructor(inst, tzQhr)
	{
		this.inst = inst;
		this.tzQhr = tzQhr;
		if (this.tzQhr == null)
			this.tzQhr = DateTimeUtil.getLocalTzQhr();
	}

	static fromTicks(ticks, tzQhr)
	{
		return new Timestamp(TimeInstant.fromTicks(ticks), tzQhr);
	}

	static fromStr(str, defTzQhr)
	{
		let tval = DateTimeUtil.string2TimeValue(str, defTzQhr);
		if (tval == null)
		{
			return null;
		}
		else
		{
			return new Timestamp(new TimeInstant(DateTimeUtil.timeValue2Secs(tval), tval.nanosec), tval.tzQhr);
		}
	}

	static now()
	{
		return new Timestamp(TimeInstant.now(), DateTimeUtil.getLocalTzQhr());
	}

	static utcNow()
	{
		return new Timestamp(TimeInstant.now(), 0);
	}

	static fromVariTime(variTime)
	{
		return new Timestamp(TimeInstant.fromVariTime(variTime), DateTimeUtil.getLocalTzQhr());
	}

	static fromSecNS(unixTS, nanosec, tzQhr)
	{
		return new Timestamp(new TimeInstant(unixTS, nanosec), tzQhr);
	}

	static fromDotNetTicks(ticks, tzQhr)
	{
		return new Timestamp(TimeInstant.fromDotNetTicks(ticks), tzQhr);
	}

	static fromEpochSec(epochSec, tzQhr)
	{
		return new Timestamp(new TimeInstant(epochSec, 0), tzQhr);
	}

	static fromEpochMS(epochMS, tzQhr)
	{
		return Timestamp.fromTicks(epochMS, tzQhr);
	}

	static fromEpochUS(epochUS, tzQhr)
	{
		if (epochUS < 0)
		{
			return new Timestamp(new TimeInstant(epochUS / 1000000n - 1, Number(epochUS % 1000000 + 1000000) * 1000), tzQhr);
		}
		else
		{
			return new Timestamp(new TimeInstant(epochUS / 1000000n, Number(epochUS % 1000000) * 1000), tzQhr);
		}
	}

	static fromEpochNS(epochNS, tzQhr)
	{
		if (epochNS < 0)
		{
			return new Timestamp(new TimeInstant(epochNS / 1000000000n - 1, Number(epochNS % 1000000000 + 1000000000)), tzQhr);
		}
		else
		{
			return new Timestamp(new TimeInstant(epochNS / 1000000000n, Number(epochNS % 1000000000)), tzQhr);
		}
	}

	static fromTimeValue(tval)
	{
		return new Timestamp(new TimeInstant(DateTimeUtil.timeValue2Secs(tval), tval.nanosec), tval.tzQhr);
	}

	static fromYMDHMS(ymdhms, tzQhr)
	{
		let tval = DateTimeUtil.timeValueFromYMDHMS(ymdhms);
		if (tval != null)
		{
			return new Timestamp(new TimeInstant(DateTimeUtil.timeValue2Secs(tval), 0), tzQhr);
		}
		else
		{
			return null;
		}
	}

	addMonth(val)
	{
		let tval = this.toTimeValue();
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
		return new Timestamp(new TimeInstant(DateTimeUtil.timeValue2Secs(tval), this.inst.nanosec), this.tzQhr);
	}

	addYear(val)
	{
		let tval = this.toTimeValue();
		tval.year = (tval.year + val);
		return new Timestamp(new TimeInstant(DateTimeUtil.timeValue2Secs(tval), this.inst.nanosec), this.tzQhr);
	}

	addDay(val)
	{
		return new Timestamp(this.inst.addDay(val), this.tzQhr);
	}

	addHour(val)
	{
		return new Timestamp(this.inst.addHour(val), this.tzQhr);
	}

	addMinute(val)
	{
		return new Timestamp(this.inst.addMinute(val), this.tzQhr);
	}

	addSecond(val)
	{
		let sec = Math.floor(val);
		let ns = Math.floor((val - sec) * 1000000000);
		return new Timestamp(this.inst.addSecond(sec).addNS(ns), this.tzQhr);
	}

	addMS(val)
	{
		return new Timestamp(this.inst.addMS(val), this.tzQhr);
	}

	addNS(val)
	{
		return new Timestamp(this.inst.addNS(val), this.tzQhr);
	}

	getMS()
	{
		return this.inst.GetMS();
	}

	clearTimeUTC()
	{
		return new Timestamp(this.inst.clearTime(), this.tzQhr);
	}

	clearTimeLocal()
	{
		return new Timestamp(this.inst.addMinute(this.tzQhr * 15).clearTime().addMinute(this.tzQhr * -15), this.tzQhr);
	}

	clearMonthAndDay()
	{
		let tval = this.toTimeValue();
		tval.month = 1;
		tval.day = 1;
		tval.hour = 0;
		tval.minute = 0;
		tval.second = 0;
		tval.nanosec = 0;
		return Timestamp.fromTimeValue(tval);
	}

	clearDayOfMonth()
	{
		let tval = this.toTimeValue();
		tval.day = 1;
		tval.hour = 0;
		tval.minute = 0;
		tval.second = 0;
		tval.nanosec = 0;
		return Timestamp.fromTimeValue(tval);
	}

	getMSPassedUTCDate()
	{
		return this.inst.getMSPassedDate();
	}

	getMSPassedLocalDate()
	{
		return this.inst.addSecond(this.tzQhr * 900).getMSPassedDate();
	}

	diffSec(ts)
	{
		return this.inst.diffSec(ts.inst);
	}

	diffMS(ts)
	{
		return this.inst.diffMS(ts.inst);
	}

	diffSecDbl(ts)
	{
		return this.inst.diffSecDbl(ts.inst);
	}

	diff(ts)
	{
		return this.inst.diff(ts.inst);
	}

	toTicks()
	{
		return this.inst.toTicks();
	}

	toDotNetTicks()
	{
		return this.inst.toDotNetTicks();
	}

	toUnixTimestamp()
	{
		return this.inst.toUnixTimestamp();
	}

	toEpochSec()
	{
		return this.inst.toEpochSec();
	}

	toEpochMS()
	{
		return this.inst.toEpochMS();
	}

	toEpochNS()
	{
		return this.inst.toEpochNS();
	}

	toString(pattern)
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
		let tval = DateTimeUtil.instant2TimeValue(this.inst.sec, this.inst.nanosec, this.tzQhr);
		return DateTimeUtil.toString(tval, pattern);
	}

	toStringISO8601()
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

	toStringNoZone()
	{
		if (this.inst.nanosec == 0)
		{
			if (((this.inst.sec + BigInt(this.tzQhr * 900)) % 86400n) == 0)
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

	toUTCTime()
	{
		return new Timestamp(this.inst, 0);
	}

	toLocalTime()
	{
		return new Timestamp(this.inst, DateTimeUtil.getLocalTzQhr());
	}

	convertTimeZoneQHR(tzQhr)
	{
		return new Timestamp(this.inst, tzQhr);
	}

	setTimeZoneQHR(tzQhr)
	{
		if (this.tzQhr != tzQhr)
		{
			return new Timestamp(this.inst.addSecond((this.tzQhr - tzQhr) * (15 * 60)), tzQhr);
		}
		else
		{
			return this;
		}
	}

	getTimeZoneQHR()
	{
		return this.tzQhr;
	}

	sameDate(ts)
	{
		return this.inst.sameDate(ts.inst);
	}

	toTimeValue()
	{
		return DateTimeUtil.instant2TimeValue(this.inst.sec, this.inst.nanosec, this.tzQhr);
	}

	roundToS()
	{
		return new Timestamp(this.inst.roundToS(), this.tzQhr);
	}
}

export class ByteReader
{
	constructor(arr)
	{
		this.view = new DataView(arr);
	}

	getLength()
	{
		return this.view.byteLength;
	}

	getArrayBuffer(ofst, size)
	{
		if (ofst == null)
			ofst = 0;
		if (size == null)
			size = this.view.byteLength - ofst;
		return this.view.buffer.slice(this.view.byteOffset + ofst, this.view.byteOffset + ofst + size);
	}

	getU8Arr(ofst, size)
	{
		return new Uint8Array(this.getArrayBuffer(ofst, size));
	}

	readUInt8(ofst)
	{
		return this.view.getUint8(ofst);
	}

	readUInt16(ofst, lsb)
	{
		return this.view.getUint16(ofst, lsb);
	}

	readUInt24(ofst, lsb)
	{
		if (lsb)
			return this.view.getUint8(ofst) + (this.view.getUint16(ofst + 1, true) << 8);
		else
			return (this.view.getUint16(ofst, false) << 8) | this.view.getUint8(ofst + 2);
	}

	readUInt32(ofst, lsb)
	{
		return this.view.getUint32(ofst, lsb);
	}

	readInt8(ofst)
	{
		return this.view.getInt8(ofst);
	}

	readInt16(ofst, lsb)
	{
		return this.view.getInt16(ofst, lsb);
	}

	readInt24(ofst, lsb)
	{
		if (lsb)
			return this.view.getUint8(ofst) + (this.view.getInt16(ofst + 1, true) << 8);
		else
			return (this.view.getInt16(ofst, false) << 8) | this.view.getUint8(ofst + 2);
	}

	readInt32(ofst, lsb)
	{
		return this.view.getInt32(ofst, lsb);
	}

	readFloat64(ofst, lsb)
	{
		return this.view.getFloat64(ofst, lsb);
	}

	readUTF8(ofst, len)
	{
		let dec = new TextDecoder();
		return dec.decode(this.getArrayBuffer(ofst, len));
	}

	readUTF8Z(ofst, maxSize)
	{
		let end = ofst;
		let maxEnd;
		if (maxSize)
		{
			maxEnd = ofst + maxSize;
			if (maxEnd > this.view.byteLength)
				maxEnd = this.view.byteLength;
		}
		else
		{
			maxEnd = this.view.byteLength;
		}
		while (end < maxEnd && this.view.getUint8(end) != 0)
			end++;
		let dec = new TextDecoder();
		return dec.decode(this.view.buffer.slice(this.view.byteOffset + ofst, this.view.byteOffset + end));
	}

	readUTF16(ofst, nChar, lsb)
	{
		let ret = [];
		while (nChar-- > 0)
		{
			ret.push(String.fromCharCode(this.readUInt16(ofst, lsb)));
			ofst += 2;
		}
		return ret.join("");
	}

	readUInt8Arr(ofst, cnt)
	{
		let ret = [];
		while (cnt-- > 0)
		{
			ret.push(this.readUInt8(ofst));
			ofst++;
		}
		return ret;
	}

	readUInt16Arr(ofst, lsb, cnt)
	{
		let ret = [];
		while (cnt-- > 0)
		{
			ret.push(this.readUInt16(ofst, lsb));
			ofst += 2;
		}
		return ret;
	}

	readUInt32Arr(ofst, lsb, cnt)
	{
		let ret = [];
		while (cnt-- > 0)
		{
			ret.push(this.readUInt32(ofst, lsb));
			ofst += 4;
		}
		return ret;
	}

	readInt8Arr(ofst, cnt)
	{
		let ret = [];
		while (cnt-- > 0)
		{
			ret.push(this.readInt8(ofst));
			ofst++;
		}
		return ret;
	}

	readInt16Arr(ofst, lsb, cnt)
	{
		let ret = [];
		while (cnt-- > 0)
		{
			ret.push(this.readInt16(ofst, lsb));
			ofst += 2;
		}
		return ret;
	}

	readInt32Arr(ofst, lsb, cnt)
	{
		let ret = [];
		while (cnt-- > 0)
		{
			ret.push(this.readInt32(ofst, lsb));
			ofst += 4;
		}
		return ret;
	}

	readFloat64Arr(ofst, lsb, cnt)
	{
		let ret = [];
		while (cnt-- > 0)
		{
			ret.push(this.readFloat64(ofst, lsb));
			ofst += 8;
		}
		return ret;
	}

	isASCIIText(ofst, len)
	{
		while (len-- > 0)
		{
			let b = this.readUInt8(ofst);
			if ((b >= 0x20 && b < 0x7F) || b == 13 || b == 10)
			{
	
			}
			else
			{
				return false;
			}
			ofst++;
		}
		return true;
	}
}

export class ParsedObject
{
	constructor(sourceName, objType)
	{
		this.sourceName = sourceName;
		this.objType = objType;
	}
}
