import * as util from "./util.js";
import * as images from "./images.js";
import * as fontFaces from "./fontFaces.js";

// Default impl options
const defaultOptions = {
	// Default is to fail on error, no placeholder
	imagePlaceholder: undefined,
	// Default cache bust is false, it will use the cache
	cacheBust: false
};

/**
 * @param {HTMLElement} node - The DOM Node object to render
 * @param {{filter?:(node: Node)=>boolean,bgcolor?:string,width?:number,height?:number,style?:{[n:string]:string},quality?:number,imagePlaceholder?:string,cacheBust?:boolean}|null|undefined} options - Rendering options
 **/
export async function toSvg(node, options) {
	options = options || {};
	let implOptions = copyOptions(options);
	let clone = await cloneNode(node, options.filter, true);
	clone = await embedFonts(clone);
	clone = await inlineImages(clone, implOptions);
	if (options.bgcolor) clone.style.backgroundColor = options.bgcolor;
	if (options.width) clone.style.width = options.width + 'px';
	if (options.height) clone.style.height = options.height + 'px';
	if (options.style)
	{
		Object.keys(options.style).forEach(function (property) {
			clone.style[property] = options.style[property];
		});
	}
	return makeSvgDataUri(clone,
		options.width || util.width(node),
		options.height || util.height(node)
	);
};

/**
 * @param {HTMLElement} node - The DOM Node object to render
 * @param {{filter?:(node: Node)=>boolean,bgcolor?:string,width?:number,height?:number,style?:{[n:string]:string},quality?:number,imagePlaceholder?:string,cacheBust?:boolean}|null|undefined} options - Rendering options, @see {@link toSvg}
 * */
export function toPixelData(node, options) {
	return draw(node, options || {})
		.then(function (canvas) {
			let ctx = canvas.getContext('2d');
			if (ctx)
			{
				return ctx.getImageData(
						0,
						0,
						util.width(node),
						util.height(node)
					).data;
			}
			else
			{
				return null;
			}
		});
}

/**
 * @param {HTMLElement} node - The DOM Node object to render
 * @param {{filter?:(node: Node)=>boolean,bgcolor?:string,width?:number,height?:number,style?:{[n:string]:string},quality?:number,imagePlaceholder?:string,cacheBust?:boolean}|null|undefined} options - Rendering options, @see {@link toSvg}
 * */
export async function toPng(node, options) {
	let canvas = await draw(node, options || {});
	return canvas.toDataURL();
}

/**
 * @param {HTMLElement} node - The DOM Node object to render
 * @param {{filter?:(node: Node)=>boolean,bgcolor?:string,width?:number,height?:number,style?:{[n:string]:string},quality?:number,imagePlaceholder?:string,cacheBust?:boolean}|null|undefined} options - Rendering options, @see {@link toSvg}
 * */
export async function toJpeg(node, options) {
	options = options || {};
	let canvas = await draw(node, options);
	return canvas.toDataURL('image/jpeg', options.quality || 1.0);
}

/**
 * @param {HTMLElement} node - The DOM Node object to render
 * @param {{filter?:(node: Node)=>boolean,bgcolor?:string,width?:number,height?:number,style?:{[n:string]:string},quality?:number,imagePlaceholder?:string,cacheBust?:boolean}|null|undefined} options - Rendering options, @see {@link toSvg}
 * */
export async function toBlob(node, options) {
	let canvas = await draw(node, options || {});
	return await util.canvasToBlob(canvas);
}

/**
 * @param {{filter?:(node: Node)=>boolean,bgcolor?:string,width?:number,height?:number,style?:{[n:string]:string},quality?:number,imagePlaceholder?:string,cacheBust?:boolean}} options
 */
function copyOptions(options) {
	let implOptions = {};
	// Copy options to impl options for use in impl
	if(typeof(options.imagePlaceholder) === 'undefined') {
		implOptions.imagePlaceholder = defaultOptions.imagePlaceholder;
	} else {
		implOptions.imagePlaceholder = options.imagePlaceholder;
	}

	if(typeof(options.cacheBust) === 'undefined') {
		implOptions.cacheBust = defaultOptions.cacheBust;
	} else {
		implOptions.cacheBust = options.cacheBust;
	}
	return implOptions;
}

/**
 * @param {HTMLElement} domNode
 * @param {{filter?:(node: Node)=>boolean,bgcolor?:string,width?:number,height?:number,style?:{[n:string]:string},quality?:number,imagePlaceholder?:string,cacheBust?:boolean}|null|undefined} options
 */
async function draw(domNode, options) {
	let svgUrl = await toSvg(domNode, options);
	let image = await util.makeImage(svgUrl);
	await util.delay(100)();
	let canvas = newCanvas(domNode, options);
	let ctx = canvas.getContext('2d');
	if (ctx) ctx.drawImage(image, 0, 0);
	return canvas;
}

/**
 * @param {HTMLElement} domNode
 * @param {{filter?:(node: Node)=>boolean,bgcolor?:string,width?:number,height?:number,style?:{[n:string]:string},quality?:number,imagePlaceholder?:string,cacheBust?:boolean}|null|undefined} options
 */
function newCanvas(domNode, options) {
	let canvas = document.createElement('canvas');
	canvas.width = options.width || domNode.offsetWidth;
	canvas.height = options.height || domNode.offsetHeight;

	if (options.bgcolor) {
		var ctx = canvas.getContext('2d');
		if (ctx != null)
		{
			ctx.fillStyle = options.bgcolor;
			ctx.fillRect(0, 0, canvas.width, canvas.height);
		}
	}

	return canvas;
}


/**
 * @param {HTMLElement} node
 * @param {(node:Node)=>boolean|undefined} filter
 * @param {boolean | undefined} [root]
 */
async function cloneNode(node, filter, root) {
	if (!root && filter && !filter(node)) return node;
	if (node instanceof HTMLCanvasElement) return util.makeImage(node.toDataURL());
	let clone = node.cloneNode(false);
	clone = await cloneChildren(node, clone, filter);
	// @ts-ignore
	return await processClone(node, clone);
}

/**
 * @param {Node} original
 * @param {Node} clone
 * @param {(node: Node) => boolean | undefined} filter
 */
async function cloneChildren(original, clone, filter) {
	var children = original.childNodes;
	if (children.length === 0) return clone;
	for (let i = 0; i < children.length; i++)
	{
		let child = children[i];
		// @ts-ignore
		let childClone = await cloneNode(child, filter);
		if (childClone) clone.appendChild(childClone);
	}
	return clone;
}

/**
 * @param {HTMLElement} original
 * @param {HTMLElement} clone
 */
function processClone(original, clone) {
	if (!(clone instanceof Element)) return clone;

	return Promise.resolve()
		.then(cloneStyle)
		.then(clonePseudoElements)
		.then(copyUserInput)
		.then(fixSvg)
		.then(function () {
			return clone;
		});

	function cloneStyle() {
		copyStyle(window.getComputedStyle(original), clone.style);

		/**
		 * @param {CSSStyleDeclaration} source
		 * @param {CSSStyleDeclaration} target
		 */
		function copyStyle(source, target) {
			if (source.cssText) target.cssText = source.cssText;
			else copyProperties(source, target);

			/**
			 * @param {CSSStyleDeclaration} source
			 * @param {{ setProperty: (arg0: any, arg1: any, arg2: any) => void; }} target
			 */
			function copyProperties(source, target) {
				for (let i = 0; i < source.length; i++)
				{
					let name = source[i];
					target.setProperty(
						name,
						source.getPropertyValue(name),
						source.getPropertyPriority(name)
					);
				}
			}
		}
	}

	function clonePseudoElements() {
		[':before', ':after'].forEach(function (element) {
			clonePseudoElement(element);
		});

		/**
		 * @param {string} element
		 */
		function clonePseudoElement(element) {
			var style = window.getComputedStyle(original, element);
			var content = style.getPropertyValue('content');

			if (content === '' || content === 'none') return;

			var className = util.uid();
			clone.className = clone.className + ' ' + className;
			var styleElement = document.createElement('style');
			styleElement.appendChild(formatPseudoElementStyle(className, element, style));
			clone.appendChild(styleElement);

			/**
			 * @param {string} className
			 * @param {string} element
			 * @param {CSSStyleDeclaration} style
			 */
			function formatPseudoElementStyle(className, element, style) {
				let selector = '.' + className + ':' + element;
				let cssText = style.cssText ? formatCssText(style) : formatCssProperties(style);
				return document.createTextNode(selector + '{' + cssText + '}');

				/**
				 * @param {CSSStyleDeclaration} style
				 */
				function formatCssText(style) {
					let content = style.getPropertyValue('content');
					return style.cssText + ' content: ' + content + ';';
				}

				/**
				 * @param {CSSStyleDeclaration} style
				 */
				function formatCssProperties(style) {
					let css = [];
					for (let i = 0; i < style.length; i++)
					{
						let name = style[i];
						css.push(name + ': ' +
							style.getPropertyValue(name) +
							(style.getPropertyPriority(name) ? ' !important' : ''));
					}
					return css.join('; ') + ';';
				}
			}
		}
	}

	function copyUserInput() {
		if (original instanceof HTMLTextAreaElement) clone.innerHTML = original.value;
		if (original instanceof HTMLInputElement) clone.setAttribute("value", original.value);
	}

	function fixSvg() {
		if (!(clone instanceof SVGElement)) return;
		clone.setAttribute('xmlns', 'http://www.w3.org/2000/svg');

		if (!(clone instanceof SVGRectElement)) return;
		['width', 'height'].forEach(function (attribute) {
			var value = clone.getAttribute(attribute);
			if (!value) return;

			clone.style.setProperty(attribute, value);
		});
	}
}

/**
 * @param {HTMLElement} node
 */
async function embedFonts(node) {
	let cssText = await fontFaces.resolveAll();
	let styleNode = document.createElement('style');
	node.appendChild(styleNode);
	styleNode.appendChild(document.createTextNode(cssText));
	return node;
}

/**
 * @param {HTMLElement} node
 * @param {{ cacheBust: boolean; imagePlaceholder?: string; } | undefined} implOptions
 */
async function inlineImages(node, implOptions) {
	return await images.inlineAll(node, implOptions);
}

/**
 * @param {Element} node
 * @param {string | number} width
 * @param {string | number} height
 */
function makeSvgDataUri(node, width, height) {
	node.setAttribute('xmlns', 'http://www.w3.org/1999/xhtml');
	let xhtml = util.escapeXhtml(new XMLSerializer().serializeToString(node));
	let foreignObject = '<foreignObject x="0" y="0" width="100%" height="100%">' + xhtml + '</foreignObject>';
	let svg = '<svg xmlns="http://www.w3.org/2000/svg" width="' + width + '" height="' + height + '">' + foreignObject + '</svg>';
	return 'data:image/svg+xml;charset=utf-8,' + svg;
}
