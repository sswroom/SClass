import * as data from "../data.js";
import * as text from "../text.js";
import * as util from "./util.js";

let URL_REGEX = /url\(['"]?([^'"]+?)['"]?\)/g;

/**
 * @param {string} string
 */
export function shouldProcess(string) {
	return string.search(URL_REGEX) !== -1;
}

/**
 * @param {string} string
 */
function readUrls(string) {
	var result = [];
	var match;
	while ((match = URL_REGEX.exec(string)) !== null) {
		result.push(match[1]);
	}
	return result.filter(function (url) {
		return !text.isDataURL(url);
	});
}

/**
 * @param {string} url
 */
function urlAsRegex(url) {
	return new RegExp('(url\\([\'"]?)(' + util.escape(url) + ')([\'"]?\\))', 'g');
}

/**
 * @param {string} string
 * @param {string} url
 * @param {string | null | undefined} baseUrl
 * @param {((url: string)=>Blob)|undefined} get
 * @param {{ cacheBust: boolean; imagePlaceholder?: string; } | undefined} options
 */
async function inline(string, url, baseUrl, get, options) {
	url = baseUrl ? util.resolveUrl(url, baseUrl) : url;
	let blob;
	if (get)
		blob = get(url);
	else
		blob = await data.fetchAsBlob(url, options);
	let dataUrl = await data.blob2DataURL(blob);
	return string.replace(urlAsRegex(url), '$1' + dataUrl + '$3');
}

/**
 * @param {string} string
 * @param {string | null | undefined} baseUrl
 * @param {((url: string)=>Blob)|undefined} get
 * @param {{ cacheBust: boolean; imagePlaceholder?: string; } | undefined} options
 */
export async function inlineAll(string, baseUrl, get, options) {
	if (!shouldProcess(string)) return string;
	let urls = readUrls(string);
	let done = string;
	urls.forEach(async function (url) {
		done = await inline(done, url, baseUrl, get, options);
	});
	return done;
}
