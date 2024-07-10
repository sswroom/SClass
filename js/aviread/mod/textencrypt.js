import * as text from "/js/@sswroom/sswr/text.js";
import * as web from "/js/@sswroom/sswr/web.js";

let list = text.getEncList();
let i;
let opt;
let sourceType = web.getSelectElement("sourceType");
let destType = web.getSelectElement("destType");
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
	let srcText = web.getInputElement("sourceText").value;
	document.getElementById("destText").innerText = destEnc.encodeBin(srcEnc.decodeBin(srcText));
}

document.getElementById("btnConv").addEventListener("click", onConvClicked);
