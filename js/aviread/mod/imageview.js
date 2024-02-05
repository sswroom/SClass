import * as web from "/js/@sswroom/sswr/web.js";

let content = document.getElementById("content");
function fileHandler(file)
{
	console.log(file);
}

web.handleFileDrop(content, fileHandler);