import * as cert from "/js/@sswroom/sswr/cert.js";
import * as parser from "/js/@sswroom/sswr/parser.js";
import * as text from "/js/@sswroom/sswr/text.js";
import * as web from "/js/@sswroom/sswr/web.js";

let currTab = 0;
let content = document.getElementById("content");
let currData;

async function fileHandler(file)
{
	let obj = await parser.parseFile(file);
	if (obj instanceof cert.ASN1Data)
	{
		currData = obj;
		updateDisp();
	}
	else
	{
		console.log("Result obj unknown", obj);
	}
}

function updateTabContent()
{
	content.innerHTML = "";
	switch (currTab)
	{
	case 0:
		content.innerHTML = "<textarea id=\"txtDesc\" style=\"width: 100%; height: 100%\"></textarea>";
		break;
	case 1:
		content.innerHTML = "<textarea id=\"txtASN1\" style=\"width: 100%; height: 100%\"></textarea>";
		break;
	}	
	updateDisp();
}

function updateDisp()
{
	if (currData == null)
		return;
	switch (currTab)
	{
	case 0:
		web.getInputElement("txtDesc").value = currData.toString();
		break;
	case 1:
		web.getInputElement("txtASN1").value = currData.toASN1String();
		break;
	}
}

function onDescTab()
{
	if (currTab != 0)
	{
		currTab = 0;
		updateTabContent();
	}
}

function onASN1Tab()
{
	if (currTab != 1)
	{
		currTab = 1;
		updateTabContent();
	}
}

function onVerifyTab()
{
	if (currTab != 2)
	{
		currTab = 2;
		updateTabContent();
	}
}

function onEncryptTab()
{
	if (currTab != 3)
	{
		currTab = 3;
		updateTabContent();
	}
}

document.getElementById("btnDesc").addEventListener("click", onDescTab);
document.getElementById("btnASN1").addEventListener("click", onASN1Tab);
document.getElementById("btnVerify").addEventListener("click", onVerifyTab);
document.getElementById("btnEncrypt").addEventListener("click", onEncryptTab);

web.handleFileDrop(content, fileHandler);
updateTabContent();