import * as unit from "/js/@sswroom/sswr/unit.js";

function updateResult()
{
	var val = Number.parseFloat(document.getElementById("fromValue").value);
	if (Number.isNaN(val))
	{
		document.getElementById("toValue").value = "";
		return;
	}
	var unitType = document.getElementById("unitType");
	var u = units[unitType.value];
	val = u.convert(u.Unit[document.getElementById("fromUnit").value], u.Unit[document.getElementById("toUnit").value], val);
	document.getElementById("toValue").value = val;
}

function onUnitTypeChg()
{
	var unitType = document.getElementById("unitType");
	var u = units[unitType.value];
	var fromType = document.getElementById("fromUnit");
	var toType = document.getElementById("toUnit");
	var i;
	var info;
	i = fromType.options.length;
	while (i-- > 0)
	{
		fromType.remove(i);
		toType.remove(i);
	}
	for (i in u.Unit)
	{
		info = u.getUnitInfo(u.Unit[i]);
		opt = document.createElement("option");
		opt.text = info.symbol+" ("+info.name+")";
		opt.value = i;
		fromType.add(opt);
		opt = document.createElement("option");
		opt.text = info.symbol+" ("+info.name+")";
		opt.value = i;
		toType.add(opt);
	}
	updateResult();
}

var units = unit.getList();
var unitType = document.getElementById("unitType");
var i;
var opt;
for (i in units)
{
	opt = document.createElement("option");
	opt.text = units[i].name;
	opt.value = i;
	unitType.add(opt);
}

onUnitTypeChg();
unitType.addEventListener("change", (e)=>{onUnitTypeChg();});
document.getElementById("fromUnit").addEventListener("change", (e)=>{updateResult();});
document.getElementById("toUnit").addEventListener("change", (e)=>{updateResult();});
document.getElementById("fromValue").addEventListener("change", (e)=>{updateResult();});
