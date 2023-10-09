
var Duration = function(seconds, nanosec)
{
	this.seconds = seconds;
	this.nanosec = nanosec;
};

Duration.fromTicks = function(ticks)
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

Duration.fromUs = function(us)
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

Duration.prototype.getSeconds = function()
{
	return this.seconds;
}

Duration.prototype.getNS = function()
{
	return this.ns;
}

Duration.prototype.getTotalMS = function()
{
	return this.seconds * 1000 + Math.floor(this.ns / 1000000);
}

Duration.prototype.getTotalSec = function()
{
	return this.seconds + (this.ns * 0.000000001);
}

Duration.prototype.notZero = function()
{
	return this.seconds != 0 || this.ns != 0;
}

Duration.prototype.isZero = function()
{
	return this.seconds == 0 && this.ns == 0;
}
export default Duration;