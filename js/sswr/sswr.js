import data from "./data.js";
import map from "./map.js";
import math from "./math.js";
import text from "./text.js";
import web from "./web.js";

window.sswr = new Object();

sswr.data = data;
sswr.map = map;
sswr.math = math;
sswr.text = text;
sswr.web = web;

export default sswr;
