import * as data from "../data.js";
import * as text from "../text.js";
import * as util from "./util.js";
import * as inliner from "./inliner.js";

/**
 * @param {HTMLImageElement} element
 * @param {{ cacheBust: boolean; imagePlaceholder?: string; } | undefined} [options]
 * @returns {Promise<HTMLImageElement>}
 */
async function inline(element, options) {
	if (text.isDataURL(element.src)) return element;
	let blob = await data.fetchAsBlob(element.src, options);
	let dataUrl = await data.blob2DataURL(blob);
	return await new Promise(function (resolve, reject) {
		element.onload = () => {resolve(element);};
		element.onerror = reject;
		element.src = dataUrl;
	});
}

/**
 * @param {HTMLElement} node
 * @param {{ cacheBust: boolean; imagePlaceholder?: string; } | undefined} options
 */
export async function inlineAll(node, options) {
	if (!(node instanceof HTMLElement)) return node;
	let background = node.style.getPropertyValue('background');

	if (!background) return node;

	let inlined = await inliner.inlineAll(background, undefined, undefined, options)
	node.style.setProperty(
		'background',
		inlined,
		node.style.getPropertyPriority('background'));
	if (node instanceof HTMLImageElement)
		return await inline(node);
	else
	{
		let children = util.asArray(node.childNodes);
		let leng = children.length;
		for (let i = 0; i < leng; i++)
		{
			node.replaceChild(children[i], await inlineAll(children[i], options));
		}
		return node;
	}
}
