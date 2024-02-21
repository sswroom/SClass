import * as cert from "/js/@sswroom/sswr/cert.js";
import * as parser from "/js/@sswroom/sswr/parser.js";
import * as web from "/js/@sswroom/sswr/web.js";

let currTab = 0;
let content = document.getElementById("content");

async function fileHandler(file)
{
	let obj = await parser.parseFile(file);
	if (obj instanceof cert.ASN1Data)
	{
		console.log(obj);
		console.log(obj.toASN1String());
		console.log(obj.toString());
	}
	else
	{
		console.log("Result obj unknown", obj);
	}
}

function updateTabContent()
{
	
}

function updateDisp()
{

}

web.handleFileDrop(content, fileHandler);
