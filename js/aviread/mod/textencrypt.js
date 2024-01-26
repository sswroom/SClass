import * as text from "/js/@sswroom/sswr/text.js";

let list = text.getEncList();
let i;
let opt;
let sourceType = document.getElementById("sourceType");
let destType = document.getElementById("destType");
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
	let srcEnc = list[sourceType.value];
	let destEnc = list[destType.value];
	let srcText = document.getElementById("sourceText").value;
	document.getElementById("destText").innerText = destEnc.encodeBin(srcEnc.decodeBin(srcText));
}

document.getElementById("btnConv").addEventListener("click", onConvClicked);
