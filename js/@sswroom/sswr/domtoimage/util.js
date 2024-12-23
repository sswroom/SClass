import * as web from "../web.js";
export const uid = genUid();

function mimes() {
	/*
		* Only WOFF and EOT mime types for fonts are 'real'
		* see http://www.iana.org/assignments/media-types/media-types.xhtml
		*/
	let WOFF = 'application/font-woff';
	let JPEG = 'image/jpeg';

	return {
		'woff': WOFF,
		'woff2': WOFF,
		'ttf': 'application/font-truetype',
		'eot': 'application/vnd.ms-fontobject',
		'png': 'image/png',
		'jpg': JPEG,
		'jpeg': JPEG,
		'gif': 'image/gif',
		'tiff': 'image/tiff',
		'svg': 'image/svg+xml'
	};
}

/**
 * @param {string} url
 */
export function parseExtension(url) {
	let match = /\.([^\.\/]*?)$/g.exec(url);
	if (match) return match[1];
	else return '';
}

/**
 * @param {string} url
 * @returns {string}
 */
export function mimeType(url) {
	let extension = parseExtension(url).toLowerCase();
	return mimes()[extension] || '';
}

/**
 * @param {string} url
 * @param {string} baseUrl
 */
export function resolveUrl(url, baseUrl) {
	let doc = document.implementation.createHTMLDocument();
	let base = doc.createElement('base');
	doc.head.appendChild(base);
	let a = doc.createElement('a');
	doc.body.appendChild(a);
	base.href = baseUrl;
	a.href = url;
	return a.href;
}

function genUid() {
	let index = 0;

	return function () {
		return 'u' + fourRandomChars() + index++;

		function fourRandomChars() {
			/* see http://stackoverflow.com/a/6248722/2519373 */
			return ('0000' + (Math.random() * Math.pow(36, 4) << 0).toString(36)).slice(-4);
		}
	};
}

/**
 * @param {string} uri
 * @returns {Promise<HTMLImageElement>}
 */
export function makeImage(uri) {
	return new Promise(function (resolve, reject) {
		let image = new Image();
		image.onload = function () {
			resolve(image);
		};
		image.onerror = reject;
		image.src = uri;
	});
}

/**
 * @param {string} string
 */
export function escape(string) {
	return string.replace(/([.*+?^${}()|\[\]\/\\])/g, '\\$1');
}

/**
 * @param {number | undefined} ms
 */
export function delay(ms) {
	return function (/** @type {any} */ arg) {
		return new Promise(function (resolve) {
			setTimeout(function () {
				resolve(arg);
			}, ms);
		});
	};
}

/**
 * @param {string | any[] | StyleSheetList | NodeListOf<ChildNode>} arrayLike
 */
export function asArray(arrayLike) {
	let array = [];
	let length = arrayLike.length;
	for (let i = 0; i < length; i++) array.push(arrayLike[i]);
	return array;
}

/**
 * @param {Element} node
 */
export function width(node) {
	let leftBorder = px(node, 'border-left-width');
	let rightBorder = px(node, 'border-right-width');
	return node.scrollWidth + leftBorder + rightBorder;
}

/**
 * @param {Element} node
 */
export function height(node) {
	let topBorder = px(node, 'border-top-width');
	let bottomBorder = px(node, 'border-bottom-width');
	return node.scrollHeight + topBorder + bottomBorder;
}

/**
 * @param {Element} node
 * @param {string} styleProperty
 */
function px(node, styleProperty) {
	var value = window.getComputedStyle(node).getPropertyValue(styleProperty);
	return parseFloat(value.replace('px', ''));
}
