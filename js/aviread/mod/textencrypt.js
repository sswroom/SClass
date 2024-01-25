import * as text from "/js/@sswroom/sswr/text.js";

var list = text.getEncList();
var i;
var opt;
var sourceType = document.getElementById("sourceType");
var destType = document.getElementById("destType");
for (i in list)
{
	opt = document.createElement("option");
	opt.text = list[i].getName();
	opt.value = i;
	sourceType.options.add(opt);
	opt = document.createElement("option");
	opt.text = list[i].getName();
	opt.value = i;
	destType.options.add(opt);
}

function onConvClicked(target, ev)
{
	var srcEnc = list[sourceType.value];
	var destEnc = list[destType.value];
	var srcText = document.getElementById("sourceText").value;
	document.getElementById("destText").innerText = destEnc.encodeBin(srcEnc.decodeBin(srcText));
}

document.getElementById("btnConv").addEventListener("click", onConvClicked);
