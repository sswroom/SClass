export function oidText2PDU(txt)
{
	var sarr = txt.split(".");
	var ret = [];
	var i = 2;
	var j = sarr.length;
	var v;
	if (j == 1)
	{
		ret.push(sarr[0] * 40);
	}
	else
	{
		ret.push(sarr[0] * 40 + Number.parseInt(sarr[1]));
	}
	while (i < j)
	{
		v = sarr[i];

		if (v < 128)
		{
			ret.push(Number.parseInt(v));
		}
		else if (v < 0x4000)
		{
			ret.push(0x80 | (v >> 7));
			ret.push(v & 0x7f);
		}
		else if (v < 0x200000)
		{
			ret.push(0x80 | (v >> 14));
			ret.push(0x80 | ((v >> 7) & 0x7f));
			ret.push(v & 0x7f);
		}
		else if (v < 0x10000000)
		{
			ret.push(0x80 | (v >> 21));
			ret.push(0x80 | ((v >> 14) & 0x7f));
			ret.push(0x80 | ((v >> 7) & 0x7f));
			ret.push(v & 0x7f);
		}
		else
		{
			ret.push(0x80 | (v >> 28));
			ret.push(0x80 | ((v >> 21) & 0x7f));
			ret.push(0x80 | ((v >> 14) & 0x7f));
			ret.push(0x80 | ((v >> 7) & 0x7f));
			ret.push(v & 0x7f);
		}

		i++;
	}
	return new Uint8Array(ret);
}

export function oidToString(oid)
{
	let arr = new Uint8Array(oid);
	let i = 1;
	let j = arr.length;
	let v = 0;
	let ret = [];
	ret.push(Math.floor(arr[0] / 40));
	ret.push(arr[0] % 40);
	while (i < j)
	{
		v = (v << 7) | (arr[i] & 0x7f);
		if ((arr[i] & 0x80) == 0)
		{
			ret.push(v);
			v = 0;
		}
		i++;
	}
	return ret.join(".");
}

export function getIPv4Name(ip)
{
	return ((ip >> 24) & 0xff).toString() + "." + ((ip >> 16) & 0xff).toString() + "." + ((ip >> 8) & 0xff).toString() + "." + (ip & 0xff).toString()
}
