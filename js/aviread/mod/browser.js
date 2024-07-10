import * as web from "/js/@sswroom/sswr/web.js";

let info = await web.getBrowserInfo();
document.getElementById("txtContent").innerHTML = web.buildTable(info)+"<hr/>"+web.buildTable(navigator);