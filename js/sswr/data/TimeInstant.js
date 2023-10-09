
var TimeInstant = function(sec, nanosec)
{
	this.sec = sec;
	this.nanosec = nanosec;
};

TimeInstant.now = function()
{
	if (window.performance)
	{
		var t1 = performance.now();
		var t2 = performance.timeOrigin;
		var secs = Math.floor((t1 + t2) / 1000);
		var ns = (t1 / 1000) - Math.floor(t1 / 1000) + (t2 / 1000) - Math.floor(t2 / 1000);
		if (ns >= 1)
			ns -= 1;
		return new TimeInstant(secs, Math.round(ns * 1000000000));
	}
	else
	{
		return TimeInstant.fromTicks(Date.now());
	}
}

TimeInstant.fromVariTime = function(variTime)
{
	var days = Math.floor(variTime);
	var ds = (variTime - days);
	var s = Math.floor(ds * 86400);
	return new TimeInstant((days - 25569) * 86400000 + Math.floor(ds * 86400000), ((ds * 86400 - s) * 1000000000));
}

TimeInstant.fromTicks = function(ticks)
{
	var ms = (ticks % 1000);
	if (ms < 0)
	{
		return new TimeInstant(Math.floor(ticks / 1000) - 1, (ms + 1000) * 1000000);
	}
	else
	{
		return new TimeInstant(Math.floor(ticks / 1000), ms * 1000000);
	}
}

TimeInstant.prototype.addDay = function(val)
{
	return new TimeInstant(this.sec + val * 86400, this.nanosec);
}

TimeInstant.prototype.addHour = function(val)
{
	return new TimeInstant(this.sec + val * 3600, this.nanosec);
}

TimeInstant.prototype.addMinute = function(val)
{
	return new TimeInstant(this.sec + val * 60, this.nanosec);
}

TimeInstant.prototype.addSecond = function(val)
{
	return new TimeInstant(this.sec + val, this.nanosec);
}

TimeInstant.prototype.addMS = function(val)
{
	var newSec = this.sec + Math.floor(val / 1000);
	val = (val % 1000) * 1000000 + this.nanosec;
	while (val > 1000000000)
	{
		newSec++;
		val -= 1000000000;
	}
	return new TimeInstant(newSec, val);
}

TimeInstant.prototype.addNS = function(val)
{
	var newSec = this.sec + Math.floor(val / 1000000000);
	val = val % 1000000000 + this.nanosec;
	while (val > 1000000000)
	{
		newSec++;
		val -= 1000000000;
	}
	return new TimeInstant(newSec, val);
}

TimeInstant.prototype.getMS = function()
{
	return Math.floor(this.nanosec / 1000000);
}

TimeInstant.prototype.clearTime = function()
{
	return new TimeInstant(this.sec - this.sec % 86400, 0);
}

TimeInstant.prototype.roundToS = function()
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

TimeInstant.prototype.getMSPassedDate = function()
{
	return (this.sec % 86400) * 1000 + Math.floor(this.nanosec / 1000000);
}

TimeInstant.prototype.diffMS = function(ts)
{
	return (this.sec - ts.sec) * 1000 + Math.floor((this.nanosec / 1000000) - (ts.nanosec / 1000000));
}

TimeInstant.prototype.diffSec = function(ts)
{
	return this.sec - ts.sec;
}

TimeInstant.prototype.diffSecDbl = function(ts)
{
	return (this.sec - ts.sec) + (this.nanosec - ts.nanosec) / 1000000000.0;
}

TimeInstant.prototype.diff = function(ts)
{
	var secs = this.sec - ts.sec;
	var ns1 = this.nanosec;
	var ns2 = ts.nanosec;
	if (ns1 >= ns2)
		return new Duration(secs, ns1 - ns2);
	else
		return new Duration(secs - 1, 1000000000 + ns1 - ns2);
}

TimeInstant.prototype.toTicks = function()
{
	return this.sec * 1000 + Math.floor(this.nanosec / 1000000);
}

TimeInstant.prototype.toDotNetTicks = function()
{
	return this.sec * 10000000 + 621355968000000000 + Math.floor(this.nanosec / 100);
}

TimeInstant.prototype.toUnixTimestamp = function()
{
	return this.sec;
}

TimeInstant.prototype.toEpochSec = function()
{
	return this.sec;
}

TimeInstant.prototype.toEpochMS = function()
{
	return this.sec * 1000 + Math.floor(this.nanosec / 1000000);
}

TimeInstant.prototype.toEpochNS = function()
{
	return this.sec * 1000000000 + this.nanosec;
}
export default TimeInstant;