import * as media from "/js/@sswroom/sswr/media.js";
import * as parser from "/js/@sswroom/sswr/parser.js";
import * as web from "/js/@sswroom/sswr/web.js";

let imgFile = web.getInputElement("imgFile");
/**
 * @param {File} file
 */
function fileHandler(file)
{
	openFile(file);
}

function onFileChange()
{
	openFile(imgFile.files[0]);
}

async function openFile(file)
{
	let img = await parser.parseFile(file);
	let code = "";
	const codeReader = new ZXing.BrowserQRCodeReader();
	if (img instanceof media.StaticImage)
	{
		let preview = web.getDivElement("preview");
		preview.innerHTML = "";
		preview.appendChild(img.img);
		try
		{
			let result = await codeReader.decodeFromImage(img.img);
			code = result.text;
		}
		catch (e)
		{
			console.error(e);
		}
	}
	web.getSpanElement("txtQRCode").innerText = code;
}

web.handleFileDrop(document.body, fileHandler);
imgFile.addEventListener("change", onFileChange)