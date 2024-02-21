import * as web from "/js/@sswroom/sswr/web.js";

let currTab = 0;
let content = document.getElementById("content");

async function fileHandler(file)
{
	let obj = await parser.parseFile(file);
	if (false)//obj instanceof media.StaticImage)
	{
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
