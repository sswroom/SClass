import * as util from "./util.js";
import * as inliner from "./inliner.js";

export function resolveAll() {
	return readAll()
		.then(function (webFonts) {
			return Promise.all(
				webFonts.map(function (webFont) {
					return webFont.resolve();
				})
			);
		})
		.then(function (cssStrings) {
			return cssStrings.join('\n');
		});
}

/**
 * @param {{ cacheBust: boolean; imagePlaceholder?: string; } | undefined} [implOptions]
 */
async function readAll(implOptions) {
	let styleSheets = [];
	for (let i = 0; i < document.styleSheets.length; i++) styleSheets.push(document.styleSheets[i]);
	let cssRules = [];
	styleSheets.forEach(function (sheet) {
		try {
			if (sheet.cssRules)
			{
				for (let i = 0; i < sheet.cssRules.length; i++) cssRules.push(sheet.cssRules[i]);
			}
		} catch (e) {
			console.log('Error while reading CSS rules from ' + sheet.href, e.toString());
		}
	});
	/**  @type {CSSRule[]} */
	let rules = cssRules
	.filter(function (rule) {
		return rule.type === CSSRule.FONT_FACE_RULE;
	})
	.filter(function (rule) {
		return inliner.shouldProcess(rule.style.getPropertyValue('src'));
	});
	return rules.map((webFontRule) => {
		return {
			resolve: function resolve() {
				let baseUrl = (webFontRule.parentStyleSheet || {}).href;
				return inliner.inlineAll(webFontRule.cssText, baseUrl, undefined, implOptions);
			},
			src: function () {
				// @ts-ignore
				return webFontRule.style.getPropertyValue('src');
			}
		};
	});
}

