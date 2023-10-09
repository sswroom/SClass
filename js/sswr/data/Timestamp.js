import DateTimeUtil from "./DateTimeUtil";
import TimeInstant from "./TimeInstant";

var Timestamp = function(inst, tzQhr)
{
	this.inst = inst;
	this.tzQhr = tzQhr;
}

Timestamp.fromTicks = function(ticks, tzQhr)
{
	return new Timestamp(TimeInstant.fromTicks(ticks), tzQhr);
}

Timestamp.fromStr = function(str, defTzQhr)
{
	var tval = DateTimeUtil.string2TimeValue(str, defTzQhr);
	if (tval == 0)
	{
		return new Timestamp(new TimeInstant(0, 0), defTzQhr);
	}
	else
	{
		return new Timestamp(new TimeInstant(DateTimeUtil.timeValue2Secs(tval), tval.nanosec), tval.tzQhr);
	}
}

Timestamp.now = function()
{
	return new Timestamp(TimeInstant.now(), DateTimeUtil.getLocalTzQhr());
}

Timestamp.utcNow = function()
{
	return new Timestamp(TimeInstant.now(), 0);
}

Timestamp.fromVariTime = function(variTime)
{
	return new Timestamp(TimeInstant.fromVariTime(variTime), DateTimeUtil.getLocalTzQhr());
}

Timestamp.fromSecNS = function(unixTS, nanosec, tzQhr)
{
	return new Timestamp(new TimeInstant(unixTS, nanosec), tzQhr);
}

Timestamp.fromDotNetTicks = function(ticks, tzQhr)
{
	return new Timestamp(TimeInstant.fromDotNetTicks(ticks), tzQhr);
}

Timestamp.fromEpochSec = function(epochSec, tzQhr)
{
	return new Timestamp(new TimeInstant(epochSec, 0), tzQhr);
}

Timestamp.fromEpochMS = function(epochMS, tzQhr)
{
	return new Timestamp(epochMS, tzQhr);
}

Timestamp.fromEpochUS = function(epochUS, tzQhr)
{
	if (epochUS < 0)
	{
		return new Timestamp(new TimeInstant(Math.floor(epochUS / 1000000) - 1, (epochUS % 1000000 + 1000000) * 1000), tzQhr);
	}
	else
	{
		return new Timestamp(new TimeInstant(Math.floor(epochUS / 1000000), (epochUS % 1000000) * 1000), tzQhr);
	}
}

Timestamp.fromEpochNS = function(epochNS, tzQhr)
{
	if (epochNS < 0)
	{
		return new Timestamp(new TimeInstant(Math.floor(epochNS / 1000000000) - 1, (epochNS % 1000000000 + 1000000000)), tzQhr);
	}
	else
	{
		return new Timestamp(new TimeInstant(Math.floor(epochNS / 1000000000), (epochNS % 1000000000)), tzQhr);
	}
}

Timestamp.fromTimeValue = function(tval)
{
	return new Timestamp(new TimeInstant(DateTimeUtil.timeValue2Secs(tval), tval.nanosec), tval.tzQhr);
}

Timestamp.fromYMDHMS = function(ymdhms, tzQhr)
{
	var tval = DateTimeUtil.timeValueFromYMDHMS(ymdhms);
	if (tval != null)
	{
		return new Timestamp(new TimeInstant(DateTimeUtil.timeValue2Secs(tval), 0), tzQhr);
	}
	else
	{
		return null;
	}
}

Timestamp.prototype.addMonth = function(val)
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
	return new Timestamp(new TimeInstant(DateTimeUtil.timeValue2Secs(tval), this.inst.nanosec), this.tzQhr);
}

Timestamp.prototype.addYear = function(val)
{
	var tval = this.toTimeValue();
	tval.year = (tval.year + val);
	return new Timestamp(new TimeInstant(DateTimeUtil.timeValue2Secs(tval), this.inst.nanosec), this.tzQhr);
}

Timestamp.prototype.addDay = function(val)
{
	return new Timestamp(this.inst.addDay(val), this.tzQhr);
}

Timestamp.prototype.addHour = function(val)
{
	return new Timestamp(this.inst.addHour(val), this.tzQhr);
}

Timestamp.prototype.addMinute = function(val)
{
	return new Timestamp(this.inst.addMinute(val), this.tzQhr);
}

Timestamp.prototype.addSecond = function(val)
{
	var sec = Math.floor(val);
	var ns = Math.floor((val - sec) * 1000000000);
	return new Timestamp(this.inst.addSecond(sec).addNS(ns), this.tzQhr);
}

Timestamp.prototype.addMS = function(val)
{
	return new Timestamp(this.inst.addMS(val), this.tzQhr);
}

Timestamp.prototype.addNS = function(val)
{
	return new Timestamp(this.inst.addNS(val), this.tzQhr);
}

Timestamp.prototype.getMS = function()
{
	return this.inst.GetMS();
}

Timestamp.prototype.clearTimeUTC = function()
{
	return new Timestamp(this.inst.clearTime(), this.tzQhr);
}

Timestamp.prototype.clearTimeLocal = function()
{
	return new Timestamp(this.inst.addMinute(this.tzQhr * 15).clearTime().addMinute(this.tzQhr * -15), this.tzQhr);
}

Timestamp.prototype.clearMonthAndDay = function()
{
	var tval = this.toTimeValue();
	tval.month = 1;
	tval.day = 1;
	tval.hour = 0;
	tval.minute = 0;
	tval.second = 0;
	tval.nanosec = 0;
	return Timestamp.fromTimeValue(tval);
}

Timestamp.prototype.clearDayOfMonth = function()
{
	var tval = this.toTimeValue();
	tval.day = 1;
	tval.hour = 0;
	tval.minute = 0;
	tval.second = 0;
	tval.nanosec = 0;
	return Timestamp.fromTimeValue(tval);
}

Timestamp.prototype.getMSPassedUTCDate = function()
{
	return this.inst.getMSPassedDate();
}

Timestamp.prototype.getMSPassedLocalDate = function()
{
	return this.inst.addSecond(this.tzQhr * 900).getMSPassedDate();
}

Timestamp.prototype.diffSec = function(ts)
{
	return this.inst.diffSec(ts.inst);
}

Timestamp.prototype.diffMS = function(ts)
{
	return this.inst.diffMS(ts.inst);
}

Timestamp.prototype.diffSecDbl = function(ts)
{
	return this.inst.diffSecDbl(ts.inst);
}

Timestamp.prototype.diff = function(ts)
{
	return this.inst.diff(ts.inst);
}

Timestamp.prototype.toTicks = function()
{
	return this.inst.toTicks();
}

Timestamp.prototype.toDotNetTicks = function()
{
	return this.inst.toDotNetTicks();
}

Timestamp.prototype.toUnixTimestamp = function()
{
	return this.inst.toUnixTimestamp();
}

Timestamp.prototype.toEpochSec = function()
{
	return this.inst.toEpochSec();
}

Timestamp.prototype.toEpochMS = function()
{
	return this.inst.toEpochMS();
}

Timestamp.prototype.toEpochNS = function()
{
	return this.inst.toEpochNS();
}

Timestamp.prototype.toString = function(pattern)
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
	var tval = DateTimeUtil.instant2TimeValue(this.inst.sec, this.inst.nanosec, this.tzQhr);
	return DateTimeUtil.toString(tval, pattern);
}

Timestamp.prototype.toStringISO8601 = function()
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

Timestamp.prototype.toStringNoZone = function()
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

Timestamp.prototype.toUTCTime = function()
{
	return new Timestamp(this.inst, 0);
}

Timestamp.prototype.toLocalTime = function()
{
	return new Timestamp(this.inst, DateTimeUtil.getLocalTzQhr());
}

Timestamp.prototype.convertTimeZoneQHR = function(tzQhr)
{
	return new Timestamp(this.inst, tzQhr);
}

Timestamp.prototype.setTimeZoneQHR = function(tzQhr)
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

Timestamp.prototype.getTimeZoneQHR = function()
{
	return this.tzQhr;
}

Timestamp.prototype.sameDate = function(ts)
{
	return this.inst.sameDate(ts.inst);
}

Timestamp.prototype.toTimeValue = function()
{
	return DateTimeUtil.instant2TimeValue(this.inst.sec, this.inst.nanosec, this.tzQhr);
}

Timestamp.prototype.roundToS = function()
{
	return new Timestamp(this.inst.roundToS(), this.tzQhr);
}
export default Timestamp;
