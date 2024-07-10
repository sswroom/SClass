import * as unit from "/js/@sswroom/sswr/unit.js";
import * as web from "/js/@sswroom/sswr/web.js";

function updateResult()
{
	let val = Number.parseFloat(web.getInputElement("fromValue").value);
	if (Number.isNaN(val))
	{
		web.getInputElement("toValue").value = "";
		return;
	}
	let unitType = web.getSelectElement("unitType");
	let u = units[unitType.value];
	val = u.convert(u.Unit[web.getSelectElement("fromUnit").value], u.Unit[web.getSelectElement("toUnit").value], val);
	web.getInputElement("toValue").value = ""+val;
}

function onUnitTypeChg()
{
	let unitType = web.getSelectElement("unitType");
	let u = units[unitType.value];
	let fromType = web.getSelectElement("fromUnit");
	let toType = web.getSelectElement("toUnit");
	let i;
	let info;
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

let units = unit.getList();
let unitType = web.getSelectElement("unitType");
let i;
let opt;
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
