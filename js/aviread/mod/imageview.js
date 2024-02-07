import * as media from "/js/@sswroom/sswr/media.js";
import * as parser from "/js/@sswroom/sswr/parser.js";
import * as web from "/js/@sswroom/sswr/web.js";

let currImg = null;
let content = document.getElementById("content");
async function fileHandler(file)
{
	let obj = await parser.parseFile(file);
	if (obj instanceof media.StaticImage)
	{
		content.innerHTML = "";
		if (window.innerWidth * obj.img.naturalHeight > window.innerHeight * obj.img.naturalWidth)
		{
			obj.img.width = Math.round(window.innerHeight * obj.img.naturalWidth / obj.img.naturalHeight);
			obj.img.height = window.innerHeight;
		}
		else
		{
			obj.img.width = window.innerWidth;
			obj.img.height = Math.round(window.innerWidth * obj.img.naturalHeight / obj.img.naturalWidth);
			console.log(window.innerWidth, obj.img.naturalHeight, obj.img.naturalWidth);
		}
		content.appendChild(obj.img);
		let p = document.createElement("span");
		p.innerHTML = web.propertiesToHTML(obj.getProperties());
		content.appendChild(p);
		currImg = obj;
	}
	else
	{
		console.log("Result obj unknown", obj);
	}
}

async function onJPGClicked()
{
	if (currImg)
	{
		let data = await currImg.exportJPG(0.75);
		web.openData(data, null, "Image.jpg");
	}
}

async function onWEBPClicked()
{
	if (currImg)
	{
		let data = await currImg.exportWEBP(0.75);
		web.openData(data, null, "Image.webp");
	}
}

async function onPNGClicked()
{
	if (currImg)
	{
		let data = await currImg.exportPNG();
		web.openData(data, null, "Image.png");
	}
}

web.handleFileDrop(content, fileHandler);
document.getElementById("btnJPG").addEventListener("click", onJPGClicked);
document.getElementById("btnWEBP").addEventListener("click", onWEBPClicked);
document.getElementById("btnPNG").addEventListener("click", onPNGClicked);
