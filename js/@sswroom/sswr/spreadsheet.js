import * as data from "./data.js";
import * as text from "./text.js";
import * as unit from "./unit.js";

export const FontFamily = {
	NA: 0,
	Roman: 1,
	Swiss: 2,
	Modern: 3,
	Script: 4,
	Decorative: 5
}

export const BorderType = {
	None: 0,
	Thin: 1,
	Medium: 2,
	Dashed: 3,
	Dotted: 4,
	Thick: 5,
	DOUBLE: 6,
	Hair: 7,
	MediumDashed: 8,
	DashDot: 9,
	MediumDashDot: 10,
	DashDotDot: 11,
	MediumDashDotDot: 12,
	SlantedDashDot: 13
}

export const FillPattern = {
	NoFill: 0,
	SolidForeground: 1,
	FineDot: 2,
	AltBars: 3,
	SparseDots: 4,
	ThickHorzBands: 5,
	ThickVertBands: 6,
	ThickBackwardDiag: 7,
	ThickForwardDiag: 8,
	BigSpots: 9,
	Bricks: 10,
	ThinHorzBands: 11,
	ThinVertBands: 12,
	ThinBackwardDiag: 13,
	ThinForwardDiag: 14,
	Squares: 15,
	Diamonds: 16,
	LessDots: 17,
	LeastDots: 18
}

export const CellDataType = {
	String: 0,
	Number: 1,
	DateTime: 2,
	MergedLeft: 3,
	MergedUp: 4
}

export const AnchorType = {
	Absolute: 0,
	OneCell: 1,
	TwoCell: 2
}

export const LegendPos = {
	Bottom: 0
}

export const BlankAs = {
	Default: 0,
	Gap: 1,
	Zero: 2
}

export const ChartType = {
	Unknown: 0,
	LineChart: 1
}

export const AxisType = {
	Date: 0,
	Category: 1,
	Numeric: 2,
	Series: 3
}

export const AxisPosition = {
	Left: 0,
	Top: 1,
	Right: 2,
	Bottom: 3
}

export const TickLabelPosition = {
	High: 0,
	Low: 1,
	NextTo: 2,
	None: 3
}

export const AxisCrosses = {
	AutoZero: 0,
	Max: 1,
	Min: 2
}

export const ColorType = {
	Preset: 0,
	Argb: 1
}

export const PresetColor = {
	AliceBlue: 0,
	AntiqueWhite: 1,
	Aqua: 2,
	Aquamarine: 3,
	Azure: 4,
	Beige: 5,
	Bisque: 6,
	Black: 7,
	BlanchedAlmond: 8,
	Blue: 9,
	BlueViolet: 10,
	Brown: 11,
	BurlyWood: 12,
	CadetBlue: 13,
	Chartreuse: 14,
	Chocolate: 15,
	Coral: 16,
	CornflowerBlue: 17,
	Cornsilk: 18,
	Crimson: 19,
	Cyan: 20,
	DeepPink: 21,
	DeepSkyBlue: 22,
	DimGray: 23,
	DarkBlue: 24,
	DarkCyan: 25,
	DarkGoldenrod: 26,
	DarkGray: 27,
	DarkGreen: 28,
	DarkKhaki: 29,
	DarkMagenta: 30,
	DarkOliveGreen: 31,
	DarkOrange: 32,
	DarkOrchid: 33,
	DarkRed: 34,
	DarkSalmon: 35,
	DarkSeaGreen: 36,
	DarkSlateBlue: 37,
	DarkSlateGray: 38,
	DarkTurquoise: 39,
	DarkViolet: 40,
	DodgerBlue: 41,
	Firebrick: 42,
	FloralWhite: 43,
	ForestGreen: 44,
	Fuchsia: 45,
	Gainsboro: 46,
	GhostWhite: 47,
	Gold: 48,
	Goldenrod: 49,
	Gray: 50,
	Green: 51,
	GreenYellow: 52,
	Honeydew: 53,
	HotPink: 54,
	IndianRed: 55,
	Indigo: 56,
	Ivory: 57,
	Khaki: 58,
	Lavender: 59,
	LavenderBlush: 60,
	LawnGreen: 61,
	LemonChiffon: 62,
	Lime: 63,
	LimeGreen: 64,
	Linen: 65,
	LightBlue: 66,
	LightCoral: 67,
	LightCyan: 68,
	LightGoldenrodYellow: 69,
	LightGray: 70,
	LightGreen: 71,
	LightPink: 72,
	LightSalmon: 73,
	LightSeaGreen: 74,
	LightSkyBlue: 75,
	LightSlateGray: 76,
	LightSteelBlue: 77,
	LightYellow: 78,
	Magenta: 79,
	Maroon: 80,
	MediumAquamarine: 81,
	MediumBlue: 82,
	MediumOrchid: 83,
	MediumPurple: 84,
	MediumSeaGreen: 85,
	MediumSlateBlue: 86,
	MediumSpringGreen: 87,
	MediumTurquoise: 88,
	MediumVioletRed: 89,
	MidnightBlue: 90,
	MintCream: 91,
	MistyRose: 92,
	Moccasin: 93,
	NavajoWhite: 94,
	Navy: 95,
	OldLace: 96,
	Olive: 97,
	OliveDrab: 98,
	Orange: 99,
	OrangeRed: 100,
	Orchid: 101,
	PaleGoldenrod: 102,
	PaleGreen: 103,
	PaleTurquoise: 104,
	PaleVioletRed: 105,
	PapayaWhip: 106,
	PeachPuff: 107,
	Peru: 108,
	Pink: 109,
	Plum: 110,
	PowderBlue: 111,
	Purple: 112,
	Red: 113,
	RosyBrown: 114,
	RoyalBlue: 115,
	SaddleBrown: 116,
	Salmon: 117,
	SandyBrown: 118,
	SeaGreen: 119,
	SeaShell: 120,
	Sienna: 121,
	Silver: 122,
	SkyBlue: 123,
	SlateBlue: 124,
	SlateGray: 125,
	Snow: 126,
	SpringGreen: 127,
	SteelBlue: 128,
	Tan: 129,
	Teal: 130,
	Thistle: 131,
	Tomato: 132,
	Turquoise: 133,
	Violet: 134,
	Wheat: 135,
	White: 136,
	WhiteSmoke: 137,
	Yellow: 138,
	YellowGreen: 139
}

export const FillType = {
	SolidFill: 0
}

export const MarkerStyle = {
	Circle: 0,
	Dash: 1,
	Diamond: 2,
	Dot: 3,
	None: 4,
	Picture: 5,
	Plus: 6,
	Square: 7,
	Star: 8,
	Triangle: 9,
	X: 10
}

export class WorkbookFont
{
	constructor()
	{
		/** @type {string|null} */
		this.name = null;
		this.size = 0;
		this.bold = false;
		this.italic = false;
		this.underline = false;
		this.color = 0;
		this.family = FontFamily.NA;
	}

	/**
	 * @param {string | null} name
	 */
	setName(name)
	{
		this.name = name;
		return this;
	}

	/**
	 * @param {number} size
	 */
	setSize(size)
	{
		this.size = size;
		return this;
	}

	/**
	 * @param {boolean} bold
	 */
	setBold(bold)
	{
		this.bold = bold;
	}

	/**
	 * @param {boolean} italic
	 */
	setItalic(italic)
	{
		this.italic = italic;
	}

	/**
	 * @param {boolean} underline
	 */
	setUnderline(underline)
	{
		this.underline = underline;
	}

	/**
	 * @param {number} color
	 */
	setColor(color)
	{
		this.color = color;
	}

	/**
	 * @param {number} family
	 */
	setFamily(family)
	{
		this.family = family;
	}

	getName()
	{
		return this.name;
	}

	getSize()
	{
		return this.size;
	}

	isBold()
	{
		return this.bold;
	}

	isItalic()
	{
		return this.italic;
	}

	isUnderline()
	{
		return this.underline;
	}

	getColor()
	{
		return this.color;
	}

	getFamily()
	{
		return this.family;
	}

	clone()
	{
		let font = new WorkbookFont();
		font.name = this.name;
		font.size = this.size;
		font.bold = this.bold;
		font.italic = this.italic;
		font.underline = this.underline;
		font.color = this.color;
		font.family = this.family;
		return font;
	}

	/**
	 * @param {WorkbookFont} font
	 */
	equals(font)
	{
		return this.name == font.name &&
			this.size == font.size &&
			this.bold == font.bold &&
			this.italic == font.italic &&
			this.underline == font.underline &&
			this.color == font.color;
	}
}

export class BorderStyle
{
	/**
	 * @param {number} borderColor
	 * @param {number} borderType
	 */
	constructor(borderColor, borderType)
	{
		this.borderColor = borderColor;
		this.borderType = borderType;
	}

	/**
	 * @param {BorderStyle} style
	 */
	set(style)
	{
		this.borderColor = style.borderColor;
		this.borderType = style.borderType;
	}

	clone()
	{
		return new BorderStyle(this.borderColor, this.borderType);
	}

	/**
	 * @param {BorderStyle} style
	 */
	equals(style)
	{
		if (style.borderType == BorderType.None && this.borderType == BorderType.None)
			return true;
		else if (style.borderType != this.borderType)
			return false;
		else if (style.borderColor != this.borderColor)
			return false;
		else
			return true;
	}
}

export class CellStyle
{
	/**
	 * @param {number} index
	 */
	constructor(index)
	{
		this.index = index;
		/** @type {string|null} */
		this.id = null;
		this.halign = text.HAlignment.Unknown;
		this.valign = text.VAlignment.Unknown;
		this.wordWrap = false;
		this.borderBottom = new BorderStyle(0, BorderType.None);
		this.borderLeft = new BorderStyle(0, BorderType.None);
		this.borderRight = new BorderStyle(0, BorderType.None);
		this.borderTop = new BorderStyle(0, BorderType.None);
		/** @type {WorkbookFont|null} */
		this.font = null;
		this.fillColor = 0xffffff;
		this.fillPattern = FillPattern.NoFill;
		/** @type {string|null} */
		this.dataFormat = null;
		this.protection = false;
	};

	clone()
	{
		let style = new CellStyle(this.index);
		style.id = this.id;
		style.halign = this.halign;
		style.valign = this.valign;
		style.wordWrap = this.wordWrap;
		style.borderBottom = this.borderBottom.clone();
		style.borderLeft = this.borderLeft.clone();
		style.borderRight = this.borderRight.clone();
		style.borderTop = this.borderTop.clone();
		style.font = this.font;
		style.fillColor = this.fillColor;
		style.fillPattern = this.fillPattern;
		style.dataFormat = this.dataFormat;
		style.protection = this.protection;
		return style;
	}

	/**
	 * @param {CellStyle} style
	 */
	copyFrom(style)
	{
		this.index = style.index;
		this.id = style.id;
		this.halign = style.halign;
		this.valign = style.valign;
		this.wordWrap = style.wordWrap;
		this.borderBottom = style.borderBottom.clone();
		this.borderLeft = style.borderLeft.clone();
		this.borderRight = style.borderRight.clone();
		this.borderTop = style.borderTop.clone();
		this.font = style.font;
		this.fillColor = style.fillColor;
		this.fillPattern = style.fillPattern;
		this.dataFormat = style.dataFormat;
		this.protection = style.protection;
	}

	/**
	 * @param {CellStyle} style
	 */
	equals(style)
	{
		if (style.halign != this.halign)
			return false;
		if (style.valign != this.valign)
			return false;
		if (style.wordWrap != this.wordWrap)
			return false;
	
		if (style.borderBottom != this.borderBottom)
			return false;
		if (style.borderLeft != this.borderLeft)
			return false;
		if (style.borderRight != this.borderRight)
			return false;
		if (style.borderTop != this.borderTop)
			return false;
	
		if (this.font == null)
		{
			if (style.font != null)
				return false;
		}
		else if (style.font == null)
		{
			return false;
		}
		else
		{
			if (style.font.equals(this.font))
				return false;
		}
		if (style.fillColor != this.fillColor)
			return false;
		if (style.fillPattern != this.fillPattern)
			return false;
		if (this.dataFormat != style.dataFormat)
			return false;
		if (style.protection != this.protection)
			return false;
		return true;
	}

	/**
	 * @param {number} index
	 */
	setIndex(index)
	{
		this.index = index;
	}

	/**
	 * @param {string | null} id
	 */
	setID(id)
	{
		if (id == null)
			return;
		this.id = id;
	}
	
	/**
	 * @param {text.HAlignment} halign
	 */
	setHAlign(halign)
	{
		this.halign = halign;
	}

	/**
	 * @param {text.VAlignment} valign
	 */
	setVAlign(valign)
	{
		this.valign = valign;
	}

	/**
	 * @param {boolean} wordWrap
	 */
	setWordWrap(wordWrap)
	{
		this.wordWrap = wordWrap;
	}

	/**
	 * @param {number} color
	 * @param {number} pattern
	 */
	setFillColor(color, pattern)
	{
		this.fillColor = color;
		this.fillPattern = pattern;
	}

	/**
	 * @param {WorkbookFont | null} font
	 */
	setFont(font)
	{
		this.font = font;
	}

	/**
	 * @param {BorderStyle} border
	 */
	setBorderLeft(border)
	{
		this.borderLeft.set(border);
	}

	/**
	 * @param {BorderStyle} border
	 */
	setBorderRight(border)
	{
		this.borderRight.set(border);
	}

	/**
	 * @param {BorderStyle} border
	 */
	setBorderTop(border)
	{
		this.borderTop = border;
	}

	/**
	 * @param {BorderStyle} border
	 */
	setBorderBottom(border)
	{
		this.borderBottom.set(border);
	}

	/**
	 * @param {string | null} dataFormat
	 */
	setDataFormat(dataFormat)
	{
		this.dataFormat = dataFormat;
	}

	getIndex()
	{
		return this.index;
	}

	getID()
	{
		return this.id;
	}

	getHAlign()
	{
		return this.halign;
	}

	getVAlign()
	{
		return this.valign;
	}

	getWordWrap()
	{
		return this.wordWrap;
	}

	getFillColor()
	{
		return this.fillColor;
	}

	getFillPattern()
	{
		return this.fillPattern;
	}

	getFont()
	{
		return this.font;
	}

	getBorderLeft()
	{
		return this.borderLeft;
	}

	getBorderRight()
	{
		return this.borderRight;
	}

	getBorderTop()
	{
		return this.borderTop;
	}

	getBorderBottom()
	{
		return this.borderBottom;
	}

	getDataFormat()
	{
		return this.dataFormat;
	}
}

/**
 * @param {number} r
 * @param {number} g
 * @param {number} b
 */
function rgbValue(r, g, b)
{
	return 0xFF000000 + ((r) << 16) + ((g) << 8) + (b);
}

export class OfficeColor
{
	/**
	 * @param {number} colorType
	 * @param {number} color
	 */
	constructor(colorType, color)
	{
		this.colorType = colorType;
		this.color = color;
	}

	getColorType()
	{
		return this.colorType;
	}

	getPresetColor()
	{
		return this.color;
	}

	getColorArgb()
	{
		switch (this.colorType)
		{
		case ColorType.Argb:
			return this.color;
		case ColorType.Preset:
			return OfficeColor.presetColorGetArgb(this.color);
		}
		return 0;
	}

	/**
	 * @param {number} color
	 */
	static newPreset(color)
	{
		return new OfficeColor(ColorType.Preset, color);	
	}

	/**
	 * @param {number} argb
	 */
	static newArgb(argb)
	{
		return new OfficeColor(ColorType.Argb, argb);
	}

	/**
	 * @param {number} color
	 */
	static presetColorGetArgb(color)
	{
		switch (color)
		{
		case PresetColor.AliceBlue:
			return rgbValue(240,248,255);
		case PresetColor.AntiqueWhite:
			return rgbValue(250,235,215);
		case PresetColor.Aqua:
			return rgbValue(0,255,255);
		case PresetColor.Aquamarine:
			return rgbValue(127,255,212);
		case PresetColor.Azure:
			return rgbValue(240,255,255);
		case PresetColor.Beige:
			return rgbValue(245,245,220);
		case PresetColor.Bisque:
			return rgbValue(255,228,196);
		case PresetColor.Black:
			return rgbValue(0,0,0);
		case PresetColor.BlanchedAlmond:
			return rgbValue(255,235,205);
		case PresetColor.Blue:
			return rgbValue(0,0,255);
		case PresetColor.BlueViolet:
			return rgbValue(138,43,226);
		case PresetColor.Brown:
			return rgbValue(165,42,42);
		case PresetColor.BurlyWood:
			return rgbValue(222,184,135);
		case PresetColor.CadetBlue:
			return rgbValue(95,158,160);
		case PresetColor.Chartreuse:
			return rgbValue(127,255,0);
		case PresetColor.Chocolate:
			return rgbValue(210,105,30);
		case PresetColor.Coral:
			return rgbValue(255,127,80);
		case PresetColor.CornflowerBlue:
			return rgbValue(100,149,237);
		case PresetColor.Cornsilk:
			return rgbValue(255,248,220);
		case PresetColor.Crimson:
			return rgbValue(220,20,60);
		case PresetColor.Cyan:
			return rgbValue(0,255,255);
		case PresetColor.DeepPink:
			return rgbValue(255,20,147);
		case PresetColor.DeepSkyBlue:
			return rgbValue(0,191,255);
		case PresetColor.DimGray:
			return rgbValue(105,105,105);
		case PresetColor.DarkBlue:
			return rgbValue(0,0,139);
		case PresetColor.DarkCyan:
			return rgbValue(0,139,139);
		case PresetColor.DarkGoldenrod:
			return rgbValue(184,134,11);
		case PresetColor.DarkGray:
			return rgbValue(169,169,169);
		case PresetColor.DarkGreen:
			return rgbValue(0,100,0);
		case PresetColor.DarkKhaki:
			return rgbValue(189,183,107);
		case PresetColor.DarkMagenta:
			return rgbValue(139,0,139);
		case PresetColor.DarkOliveGreen:
			return rgbValue(85,107,47);
		case PresetColor.DarkOrange:
			return rgbValue(255,140,0);
		case PresetColor.DarkOrchid:
			return rgbValue(153,50,204);
		case PresetColor.DarkRed:
			return rgbValue(153,50,204);
		case PresetColor.DarkSalmon:
			return rgbValue(233,150,122);
		case PresetColor.DarkSeaGreen:
			return rgbValue(143,188,139);
		case PresetColor.DarkSlateBlue:
			return rgbValue(72,61,139);
		case PresetColor.DarkSlateGray:
			return rgbValue(47,79,79);
		case PresetColor.DarkTurquoise:
			return rgbValue(0,206,209);
		case PresetColor.DarkViolet:
			return rgbValue(148,0,211);
		case PresetColor.DodgerBlue:
			return rgbValue(30,144,255);
		case PresetColor.Firebrick:
			return rgbValue(178,34,34);
		case PresetColor.FloralWhite:
			return rgbValue(255,250,240);
		case PresetColor.ForestGreen:
			return rgbValue(34,139,34);
		case PresetColor.Fuchsia:
			return rgbValue(255,0,255);
		case PresetColor.Gainsboro:
			return rgbValue(220,220,220);
		case PresetColor.GhostWhite:
			return rgbValue(248,248,255);
		case PresetColor.Gold:
			return rgbValue(255,215,0);
		case PresetColor.Goldenrod:
			return rgbValue(218,165,32);
		case PresetColor.Gray:
			return rgbValue(128,128,128);
		case PresetColor.Green:
			return rgbValue(0,128,0);
		case PresetColor.GreenYellow:
			return rgbValue(173,255,47);
		case PresetColor.Honeydew:
			return rgbValue(240,255,240);
		case PresetColor.HotPink:
			return rgbValue(255,105,180);
		case PresetColor.IndianRed:
			return rgbValue(205,92,92);
		case PresetColor.Indigo:
			return rgbValue(75,0,130);
		case PresetColor.Ivory:
			return rgbValue(255,255,240);
		case PresetColor.Khaki:
			return rgbValue(240,230,140);
		case PresetColor.Lavender:
			return rgbValue(230,230,250);
		case PresetColor.LavenderBlush:
			return rgbValue(255,240,245);
		case PresetColor.LawnGreen:
			return rgbValue(124,252,0);
		case PresetColor.LemonChiffon:
			return rgbValue(255,250,205);
		case PresetColor.Lime:
			return rgbValue(0,255,0);
		case PresetColor.LimeGreen:
			return rgbValue(50,205,50);
		case PresetColor.Linen:
			return rgbValue(250,240,230);
		case PresetColor.LightBlue:
			return rgbValue(173,216,230);
		case PresetColor.LightCoral:
			return rgbValue(240,128,128);
		case PresetColor.LightCyan:
			return rgbValue(224,255,255);
		case PresetColor.LightGoldenrodYellow:
			return rgbValue(250,250,120);
		case PresetColor.LightGray:
			return rgbValue(211,211,211);
		case PresetColor.LightGreen:
			return rgbValue(144,238,144);
		case PresetColor.LightPink:
			return rgbValue(255,182,193);
		case PresetColor.LightSalmon:
			return rgbValue(255,160,122);
		case PresetColor.LightSeaGreen:
			return rgbValue(32,178,170);
		case PresetColor.LightSkyBlue:
			return rgbValue(135,206,250);
		case PresetColor.LightSlateGray:
			return rgbValue(119,136,153);
		case PresetColor.LightSteelBlue:
			return rgbValue(176,196,222);
		case PresetColor.LightYellow:
			return rgbValue(255,255,224);
		case PresetColor.Magenta:
			return rgbValue(255,0,255);
		case PresetColor.Maroon:
			return rgbValue(128,0,0);
		case PresetColor.MediumAquamarine:
			return rgbValue(102,205,170);
		case PresetColor.MediumBlue:
			return rgbValue(0,0,205);
		case PresetColor.MediumOrchid:
			return rgbValue(186,85,211);
		case PresetColor.MediumPurple:
			return rgbValue(147,112,219);
		case PresetColor.MediumSeaGreen:
			return rgbValue(60,179,113);
		case PresetColor.MediumSlateBlue:
			return rgbValue(123,104,238);
		case PresetColor.MediumSpringGreen:
			return rgbValue(0,250,154);
		case PresetColor.MediumTurquoise:
			return rgbValue(72,209,204);
		case PresetColor.MediumVioletRed:
			return rgbValue(199,21,133);
		case PresetColor.MidnightBlue:
			return rgbValue(25,25,112);
		case PresetColor.MintCream:
			return rgbValue(245,255,250);
		case PresetColor.MistyRose:
			return rgbValue(255,228,225);
		case PresetColor.Moccasin:
			return rgbValue(255,228,181);
		case PresetColor.NavajoWhite:
			return rgbValue(255,222,173);
		case PresetColor.Navy:
			return rgbValue(0,0,128);
		case PresetColor.OldLace:
			return rgbValue(253,245,230);
		case PresetColor.Olive:
			return rgbValue(128,128,0);
		case PresetColor.OliveDrab:
			return rgbValue(107,142,35);
		case PresetColor.Orange:
			return rgbValue(255,165,0);
		case PresetColor.OrangeRed:
			return rgbValue(255,69,0);
		case PresetColor.Orchid:
			return rgbValue(218,112,214);
		case PresetColor.PaleGoldenrod:
			return rgbValue(238,232,170);
		case PresetColor.PaleGreen:
			return rgbValue(152,251,152);
		case PresetColor.PaleTurquoise:
			return rgbValue(175,238,238);
		case PresetColor.PaleVioletRed:
			return rgbValue(219,112,147);
		case PresetColor.PapayaWhip:
			return rgbValue(255,239,213);
		case PresetColor.PeachPuff:
			return rgbValue(255,218,185);
		case PresetColor.Peru:
			return rgbValue(205,133,63);
		case PresetColor.Pink:
			return rgbValue(255,192,203);
		case PresetColor.Plum:
			return rgbValue(221,160,221);
		case PresetColor.PowderBlue:
			return rgbValue(176,224,230);
		case PresetColor.Purple:
			return rgbValue(128,0,128);
		case PresetColor.Red:
			return rgbValue(255,0,0);
		case PresetColor.RosyBrown:
			return rgbValue(188,143,143);
		case PresetColor.RoyalBlue:
			return rgbValue(65,105,225);
		case PresetColor.SaddleBrown:
			return rgbValue(139,69,19);
		case PresetColor.Salmon:
			return rgbValue(250,128,114);
		case PresetColor.SandyBrown:
			return rgbValue(244,164,96);
		case PresetColor.SeaGreen:
			return rgbValue(46,139,87);
		case PresetColor.SeaShell:
			return rgbValue(255,245,238);
		case PresetColor.Sienna:
			return rgbValue(160,82,45);
		case PresetColor.Silver:
			return rgbValue(192,192,192);
		case PresetColor.SkyBlue:
			return rgbValue(135,206,235);
		case PresetColor.SlateBlue:
			return rgbValue(106,90,205);
		case PresetColor.SlateGray:
			return rgbValue(112,128,144);
		case PresetColor.Snow:
			return rgbValue(255,250,250);
		case PresetColor.SpringGreen:
			return rgbValue(0,255,127);
		case PresetColor.SteelBlue:
			return rgbValue(70,130,180);
		case PresetColor.Tan:
			return rgbValue(210,180,140);
		case PresetColor.Teal:
			return rgbValue(0,128,128);
		case PresetColor.Thistle:
			return rgbValue(216,191,216);
		case PresetColor.Tomato:
			return rgbValue(255,99,71);
		case PresetColor.Turquoise:
			return rgbValue(64,224,208);
		case PresetColor.Violet:
			return rgbValue(238,130,238);
		case PresetColor.Wheat:
			return rgbValue(245,222,179);
		case PresetColor.White:
			return rgbValue(255,255,255);
		case PresetColor.WhiteSmoke:
			return rgbValue(245,245,245);
		case PresetColor.Yellow:
			return rgbValue(255,255,0);
		case PresetColor.YellowGreen:
			return rgbValue(154,205,50);
		default:
			return 0;
		}
	}
}

export class OfficeFill
{
	/**
	 * @param {number} fillType
	 * @param {OfficeColor | null} color
	 */
	constructor(fillType, color)
	{
		this.fillType = fillType;
		this.color = color;
	}

	getFillType()
	{
		return this.fillType;
	}

	getColor()
	{
		return this.color;
	}

	/**
	 * @param {OfficeColor | null} color
	 */
	static newSolidFill(color)
	{
		return new OfficeFill(FillType.SolidFill, color);
	}
}

export class OfficeLineStyle
{
	/**
	 * @param {OfficeFill|null} fill
	 */
	constructor(fill)
	{
		this.fill = fill;
	}

	getFillStyle()
	{
		return this.fill;
	}
}

export class OfficeShapeProp
{
	/**
	 * @param {OfficeFill | null} fill
	 * @param {OfficeLineStyle | null} lineStyle
	 */
	constructor(fill, lineStyle)
	{
		this.fill = fill;
		this.lineStyle = lineStyle;
	};

	getFill()
	{
		return this.fill;
	}

	/**
	 * @param {OfficeFill | null} fill
	 */
	setFill(fill)
	{
		this.fill = fill;
	}

	getLineStyle()
	{
		return this.lineStyle;
	}

	/**
	 * @param {OfficeLineStyle | null} lineStyle
	 */
	setLineStyle(lineStyle)
	{
		this.lineStyle = lineStyle;
	}
}

export class OfficeChartAxis
{
	/**
	 * @param {number} axisType
	 * @param {number} axisPos
	 */
	constructor(axisType, axisPos)
	{
		this.axisType = axisType;
		this.axisPos = axisPos;
		/** @type {string|null} */
		this.title = null;
		/** @type {OfficeShapeProp|null} */
		this.shapeProp = null;
		/** @type {OfficeShapeProp|null} */
		this.majorGridProp = null;
		this.tickLblPos = TickLabelPosition.NextTo;
		this.crosses = AxisCrosses.AutoZero;
	}

	getAxisType()
	{
		return this.axisType;
	}

	getAxisPos()
	{
		return this.axisPos;
	}

	getTitle()
	{
		return this.title;
	}

	/**
	 * @param {string | null} title
	 */
	setTitle(title)
	{
		this.title = title;
	}
	
	getShapeProp()
	{
		return this.shapeProp;
	}

	/**
	 * @param {OfficeShapeProp | null} shapeProp
	 */
	setShapeProp(shapeProp)
	{
		this.shapeProp = shapeProp;
	}

	getMajorGridProp()
	{
		return this.majorGridProp;
	}

	/**
	 * @param {OfficeShapeProp | null} majorGridProp
	 */
	setMajorGridProp(majorGridProp)
	{
		this.majorGridProp = majorGridProp;
	}

	getTickLblPos()
	{
		return this.tickLblPos;
	}

	/**
	 * @param {number} tickLblPos
	 */
	setTickLblPos(tickLblPos)
	{
		this.tickLblPos = tickLblPos;
	}

	getCrosses()
	{
		return this.crosses;
	}

	/**
	 * @param {number} axisCrosses
	 */
	setCrosses(axisCrosses)
	{
		this.crosses = axisCrosses;
	}
}

export class WorkbookDataSource
{
	/**
	 * @param {Worksheet} sheet
	 * @param {number} firstRow
	 * @param {number} lastRow
	 * @param {number} firstCol
	 * @param {number} lastCol
	 */
	constructor(sheet, firstRow, lastRow, firstCol, lastCol)
	{
		this.sheet = sheet;
		this.firstRow = firstRow;
		this.lastRow = lastRow;
		this.firstCol = firstCol;
		this.lastCol = lastCol;
	}

	toCodeRange()
	{
		let sb = [];
		sb.push(this.sheet.getName());
		sb.push('!$');
		sb.push(Workbook.colCode(this.firstCol));
		sb.push('$');
		sb.push(""+(this.firstRow + 1));
		sb.push(':$');
		sb.push(Workbook.colCode(this.lastCol));
		sb.push('$');
		sb.push(""+(this.lastRow + 1));
		return sb.join("");
	}

	getSheet()
	{
		return this.sheet;
	}

	getFirstRow()
	{
		return this.firstRow;
	}

	getLastRow()
	{
		return this.lastRow;
	}

	getFirstCol()
	{
		return this.firstCol;
	}

	getLastCol()
	{
		return this.lastCol;
	}
}

export class OfficeChartSeries
{
	/**
	 * @param {WorkbookDataSource} categoryData
	 * @param {WorkbookDataSource} valueData
	 */
	constructor(categoryData, valueData)
	{
		this.categoryData = categoryData;
		this.valueData = valueData;
		/** @type {string|null} */
		this.title = null;
		this.smooth = false;
		/** @type {OfficeShapeProp|null} */
		this.shapeProp = null;
		this.markerSize = 0;
		this.markerStyle = MarkerStyle.None;
	}

	getCategoryData()
	{
		return this.categoryData;
	}

	getValueData()
	{
		return this.valueData;
	}

	getTitle()
	{
		return this.title;
	}

	/**
	 * @param {string | null} title
	 */
	setTitle(title)
	{
		this.title = title;
	}

	isSmooth()
	{
		return this.smooth;
	}

	/**
	 * @param {boolean} smooth
	 */
	setSmooth(smooth)
	{
		this.smooth = smooth;
	}

	getShapeProp()
	{
		return this.shapeProp;
	}

	/**
	 * @param {OfficeShapeProp | null} shapeProp
	 */
	setShapeProp(shapeProp)
	{
		this.shapeProp = shapeProp;
	}

	/**
	 * @param {OfficeLineStyle | null} lineStyle
	 */
	setLineStyle(lineStyle)
	{
		if (this.shapeProp)
		{
			this.shapeProp.setLineStyle(lineStyle);
		}
		else
		{
			this.shapeProp = new OfficeShapeProp(null, lineStyle);
		}
	}

	getMarkerSize()
	{
		return this.markerSize;
	}

	/**
	 * @param {number} markerSize
	 */
	setMarkerSize(markerSize)
	{
		this.markerSize = markerSize;
	}

	getMarkerStyle()
	{
		return this.markerStyle;
	}

	/**
	 * @param {number} markerStyle
	 */
	setMarkerStyle(markerStyle)
	{
		this.markerStyle = markerStyle;
	}
}

class OfficeChart
{
	static seriesColor = [
		PresetColor.DarkBlue,
		PresetColor.Aqua,
		PresetColor.Fuchsia,
		PresetColor.BlueViolet,
		PresetColor.Lavender,
		PresetColor.GreenYellow,
		PresetColor.Khaki,
		PresetColor.Honeydew,
		PresetColor.Magenta,
		PresetColor.Orchid,
		PresetColor.Thistle		
	];

	/**
	 * @param {unit.Distance.Unit} du
	 * @param {number} x
	 * @param {number} y
	 * @param {number} w
	 * @param {number} h
	 */
	constructor(du, x, y, w, h)
	{
		this.xInch = unit.Distance.convert(du, unit.Distance.Unit.INCH, x);
		this.yInch = unit.Distance.convert(du, unit.Distance.Unit.INCH, y);
		this.wInch = unit.Distance.convert(du, unit.Distance.Unit.INCH, w);
		this.hInch = unit.Distance.convert(du, unit.Distance.Unit.INCH, h);
		/** @type {string|null} */
		this.titleText = null;
		/** @type {OfficeShapeProp|null} */
		this.shapeProp = null;
		this.legend = false;
		this.legendPos = LegendPos.Bottom;
		this.legendOverlay = false;
		this.displayBlankAs = BlankAs.Default;
		this.chartType = ChartType.Unknown;
		/** @type {OfficeChartAxis|null} */
		this.categoryAxis = null;
		/** @type {OfficeChartAxis|null} */
		this.valueAxis = null;
		/** @type {OfficeChartAxis[]} */
		this.axes = [];
		/** @type {OfficeChartSeries[]} */
		this.series = [];
	}
	
	getXInch()
	{
		return this.xInch;
	}

	getYInch()
	{
		return this.yInch;
	}

	getWInch()
	{
		return this.wInch;
	}

	getHInch()
	{
		return this.hInch;
	}

	/**
	 * @param {string | null} titleText
	 */
	setTitleText(titleText)
	{
		this.titleText = titleText;
	}

	getTitleText()
	{
		return this.titleText;
	}

	getShapeProp()
	{
		return this.shapeProp;
	}

	/**
	 * @param {OfficeShapeProp | null} shapeProp
	 */
	setShapeProp(shapeProp)
	{
		this.shapeProp = shapeProp;
	}

	/**
	 * @param {number} pos
	 */
	addLegend(pos)
	{
		this.legend = true;
		this.legendPos = pos;
		this.legendOverlay = false;
	}

	hasLegend()
	{
		return this.legend;
	}

	getLegendPos()
	{
		return this.legendPos;
	}

	isLegendOverlay()
	{
		return this.legendOverlay;
	}

	/**
	 * @param {number} displayBlankAs
	 */
	setDisplayBlankAs(displayBlankAs)
	{
		this.displayBlankAs = displayBlankAs;
	}

	getDisplayBlankAs()
	{
		return this.displayBlankAs;
	}

	/**
	 * @param {number} chartType
	 * @param {OfficeChartAxis} categoryAxis
	 * @param {OfficeChartAxis} valueAxis
	 */
	initChart(chartType, categoryAxis, valueAxis)
	{
		this.chartType = chartType;
		this.categoryAxis = categoryAxis;
		this.valueAxis = valueAxis;
	}

	/**
	 * @param {string | null} leftAxisName
	 * @param {string | null} bottomAxisName
	 * @param {number} bottomAxisType
	 */
	initLineChart(leftAxisName, bottomAxisName, bottomAxisType)
	{
		let leftAxis = this.createAxis(AxisType.Numeric, AxisPosition.Left);
		if (leftAxisName && leftAxisName.length > 0) leftAxis.setTitle(leftAxisName);
		leftAxis.setCrosses(AxisCrosses.AutoZero);
		leftAxis.setMajorGridProp(new OfficeShapeProp(null, new OfficeLineStyle(OfficeFill.newSolidFill(OfficeColor.newPreset(PresetColor.LightGray)))));
		leftAxis.setShapeProp(new OfficeShapeProp(null, new OfficeLineStyle(OfficeFill.newSolidFill(OfficeColor.newPreset(PresetColor.Black)))));
		let bottomAxis = this.createAxis(bottomAxisType, AxisPosition.Bottom);
		if (bottomAxisName && bottomAxisName.length > 0) bottomAxis.setTitle(bottomAxisName);
		bottomAxis.setShapeProp(new OfficeShapeProp(null, new OfficeLineStyle(OfficeFill.newSolidFill(OfficeColor.newPreset(PresetColor.Black)))));
		bottomAxis.setTickLblPos(TickLabelPosition.Low);
	
		this.initChart(ChartType.LineChart, bottomAxis, leftAxis);
	}

	getChartType()
	{
		return this.chartType;
	}
	
	/**
	 * @param {number} axisType
	 * @param {number} axisPos
	 */
	createAxis(axisType, axisPos)
	{
		let axis = new OfficeChartAxis(axisType, axisPos);
		this.axes.push(axis);
		return axis;
	}

	getAxisCount()
	{
		return this.axes.length;
	}

	/**
	 * @param {number} index
	 */
	getAxis(index)
	{
		return this.axes[index];
	}

	/**
	 * @param {OfficeChartAxis} axis
	 */
	getAxisIndex(axis)
	{
		let i;
		for (i in this.axes)
		{
			if (this.axes[i] == axis)
				return Number(i);
		}
		return -1;
	}

	getCategoryAxis()
	{
		return this.categoryAxis;
	}

	getValueAxis()
	{
		return this.valueAxis;
	}

	/**
	 * @param {WorkbookDataSource} categoryData
	 * @param {WorkbookDataSource} valueData
	 * @param {string | null} name
	 * @param {boolean} showMarker
	 */
	addSeries(categoryData, valueData, name, showMarker)
	{
		let i = this.series.length;
		let series = new OfficeChartSeries(categoryData, valueData);
		if (name != null)
			series.setTitle(name);
		series.setSmooth(false);
		if (showMarker)
		{
			series.setMarkerSize(3);
			series.setMarkerStyle(MarkerStyle.Circle);
		}
		else
		{
			series.setMarkerStyle(MarkerStyle.None);
		}
		series.setLineStyle(new OfficeLineStyle(OfficeFill.newSolidFill(OfficeColor.newPreset(OfficeChart.seriesColor[i % OfficeChart.seriesColor.length]))));
		this.series.push(series);
	}

	getSeriesCount()
	{
		return this.series.length;
	}

	/**
	 * @param {number} index
	 */
	getSeriesNoCheck(index)
	{
		let o = this.series[index];
		if (o == null)
			throw new Error("Series is null");
		return o;
	}

	/**
	 * @param {number} index
	 */
	getSeries(index)
	{
		return this.series[index];
	}
}

export class Worksheet
{
	/**
	 * @param {number} row
	 */
	createRow(row)
	{
		if (row >= 1048576)
			return null;
		while (row >= this.rows.length)
		{
			this.rows.push(null);
		}
		let rowData = this.rows[row];
		if (rowData == null)
		{
			rowData = {style: null, height: -1, cells: []};
			this.rows[row] = rowData;
		}
		return rowData;		
	}

	/**
	 * @param {number} row
	 * @param {number} col
	 * @param {boolean} keepMerge
	 */
	getCellData(row, col, keepMerge)
	{
		let rowData;
		let cell;
		if (row >= this.rows.length + 65536)
			return null;
		if (col >= 65536)
			return null;
		if (col > this.maxCol)
		{
			this.maxCol = col;
		}
		while (true)
		{
			if ((rowData = this.createRow(row)) == null)
				return null;
			while (col >= rowData.cells.length)
			{
				rowData.cells.push(null);
			}
			if ((cell = rowData.cells[col]) == null)
			{
				cell = {cdt: CellDataType.String, cellValue: null, style: null, mergeHori: 0, mergeVert: 0, hidden: false, cellURL: null};
				rowData.cells[col] = cell;
			}
	
			if (keepMerge)
				break;
			if (cell.cdt == CellDataType.MergedLeft)
			{
				col--;
			}
			else if (cell.cdt == CellDataType.MergedUp)
			{
				row--;
			}
			else
			{
				break;
			}
		}
		return cell;
	}

	/**
	 * @param {{style: CellStyle|null,cells: ({cdt: number,cellValue: string|null,style: CellStyle|null,mergeHori: number,mergeVert: number,hidden: boolean,cellURL: string|null}|null)[],height: number}} row
	 * @param {Workbook} srcCtrl
	 * @param {Workbook} newCtrl
	 */
	cloneRow(row, srcCtrl, newCtrl)
	{
		let newRow;
		let cell;
		let i;
		let j;
		let style;
		if (row.style)
			style = newCtrl.getStyle(srcCtrl.getStyleIndex(row.style));
		else
			style = null;
		newRow = {style: style, cells: new Array(), height: -1};
		newRow.height = row.height;
		i = 0;
		j = row.cells.length;
		while (i < j)
		{
			if ((cell = row.cells[i]) == null)
			{
				newRow.cells.push(null);
			}
			else
			{
				newRow.cells.push(this.cloneCell(cell, srcCtrl, newCtrl));
			}
			i++;
		}
		return newRow;
	}

	/**
	 * @param {{cdt: number,cellValue: string|null,style: CellStyle|null,mergeHori: number,mergeVert: number,hidden: boolean,cellURL: string|null}} cell
	 * @param {Workbook} srcCtrl
	 * @param {Workbook} newCtrl
	 */
	cloneCell(cell, srcCtrl, newCtrl)
	{
		let newCell = {};
		newCell.cdt = cell.cdt;
		newCell.cellValue = cell.cellValue;
		if (cell.style)
			newCell.style = newCtrl.getStyle(srcCtrl.getStyleIndex(cell.style));
		else
			newCell.style = null;
		newCell.mergeHori = cell.mergeHori;
		newCell.mergeVert = cell.mergeVert;
		newCell.hidden = cell.hidden;
		newCell.cellURL = cell.cellURL;
		return newCell;
	}
	
	/**
	 * @param {string} name
	 */
	constructor(name)
	{
		this.name = name;
		/** @type {({style: CellStyle|null,cells: ({cdt: number,cellValue: string|null,style: CellStyle|null,mergeHori: number,mergeVert: number,hidden: boolean,cellURL: string|null}|null)[],height: number}|null)[]} */
		this.rows = [];
		/** @type {number[]} */
		this.colWidthsPt = [];
		/** @type {{anchorType: number,posXInch: number,posYInch: number,widthInch: number,heightInch: number,row1: number,col1: number,row2: number,col2: number,chart: OfficeChart|null}[]} */
		this.drawings = [];
		this.freezeHori = 0;
		this.freezeVert = 0;
		this.marginLeft = 2.0;
		this.marginRight = 2.0;
		this.marginTop = 2.5;
		this.marginBottom = 2.5;
		this.marginHeader = 1.3;
		this.marginFooter = 1.3;
		this.zoom = 100;
		this.options = 0x4b6;
		this.maxCol = 0;
		this.defColWidthPt = 48.0;
		this.defRowHeightPt = 13.5;
	}

	/**
	 * @param {Workbook} srcCtrl
	 * @param {Workbook} newCtrl
	 */
	clone(srcCtrl, newCtrl)
	{
		let i;
		let j;
		let row;
		let newWS = new Worksheet(this.name);
		newWS.freezeHori = this.freezeHori;
		newWS.freezeVert = this.freezeVert;
		newWS.marginLeft = this.marginLeft;
		newWS.marginRight = this.marginRight;
		newWS.marginTop = this.marginTop;
		newWS.marginBottom = this.marginBottom;
		newWS.marginHeader = this.marginHeader;
		newWS.marginFooter = this.marginFooter;
		newWS.options = this.options;
		newWS.zoom = this.zoom;
		i = 0;
		j = this.colWidthsPt.length;
		while (i < j)
		{
			newWS.colWidthsPt.push(this.colWidthsPt[i]);
			i++;
		}
		i = 0;
		j = this.rows.length;
		while (i < j)
		{
			if ((row = this.rows[i]) == null)
			{
				newWS.rows.push(null);
			}
			else
			{
				newWS.rows.push(this.cloneRow(row, srcCtrl, newCtrl));
			}
			i++;
		}
		return newWS;
	}

	/**
	 * @param {number} options
	 */
	setOptions(options)
	{
		this.options = options;
	}

	getOptions()
	{
		return this.options;
	}

	/**
	 * @param {number} freezeHori
	 */
	setFreezeHori(freezeHori)
	{
		this.freezeHori = freezeHori;
		this.options |= 0x108;
	}
	
	getFreezeHori()
	{
		return this.freezeHori;
	}

	/**
	 * @param {number} freezeVert
	 */
	setFreezeVert(freezeVert)
	{
		this.freezeVert = freezeVert;
		this.options |= 0x108;
	}

	getFreezeVert()
	{
		return this.freezeVert;
	}

	/**
	 * @param {number} marginLeft
	 */
	setMarginLeft(marginLeft)
	{
		this.marginLeft = marginLeft;
	}

	getMarginLeft()
	{
		return this.marginLeft;
	}

	/**
	 * @param {number} marginRight
	 */
	setMarginRight(marginRight)
	{
		this.marginRight = marginRight;
	}

	getMarginRight()
	{
		return this.marginRight;
	}

	/**
	 * @param {number} marginTop
	 */
	setMarginTop(marginTop)
	{
		this.marginTop = marginTop;
	}

	getMarginTop()
	{
		return this.marginTop;
	}

	/**
	 * @param {number} marginBottom
	 */
	setMarginBottom(marginBottom)
	{
		this.marginBottom = marginBottom;
	}

	getMarginBottom()
	{
		return this.marginBottom;
	}

	/**
	 * @param {number} marginHeader
	 */
	setMarginHeader(marginHeader)
	{
		this.marginHeader = marginHeader;
	}

	getMarginHeader()
	{
		return this.marginHeader;
	}

	/**
	 * @param {number} marginFooter
	 */
	setMarginFooter(marginFooter)
	{
		this.marginFooter = marginFooter;
	}

	getMarginFooter()
	{
		return this.marginFooter;
	}

	/**
	 * @param {number} zoom
	 */
	setZoom(zoom)
	{
		this.zoom = zoom;
	}

	getZoom()
	{
		return this.zoom;
	}

	isDefaultPageSetup()
	{
		return this.marginHeader == 1.3 && this.marginFooter == 1.3 && this.marginLeft == 2.0 && this.marginRight == 2.0 && this.marginTop == 2.5 && this.marginBottom == 2.5;
	}

	/**
	 * @param {number} defColWidthPt
	 */
	setDefColWidthPt(defColWidthPt)
	{
		this.defColWidthPt = defColWidthPt;
	}

	getDefColWidthPt()
	{
		return this.defColWidthPt;
	}

	/**
	 * @param {number} defRowHeightPt
	 */
	setDefRowHeightPt(defRowHeightPt)
	{
		this.defRowHeightPt = defRowHeightPt;
	}

	getDefRowHeightPt()
	{
		return this.defRowHeightPt;
	}

	getName()
	{
		return this.name;
	}

	/**
	 * @param {number} row
	 * @param {number} col
	 * @param {CellStyle|null} style
	 */
	setCellStyle(row, col, style)
	{
		let cell;
		if ((cell = this.getCellData(row, col, false)) == null)
			return false;
		cell.style = style;
		return true;
	}
	
	/**
	 * @param {number} row
	 * @param {number} col
	 * @param {Workbook} wb
	 * @param {text.HAlignment} hAlign
	 */
	setCellStyleHAlign(row, col, wb, hAlign)
	{
		let cell;
		if ((cell = this.getCellData(row, col, false)) == null)
			return false;
		let tmpStyle;
		if (cell.style == null)
		{
			if (hAlign == text.HAlignment.Unknown)
				return true;
			tmpStyle = new CellStyle(0);
			tmpStyle.setHAlign(hAlign);
			cell.style = wb.findOrCreateStyle(tmpStyle);
		}
		else
		{
			if (cell.style.getHAlign() == hAlign)
				return true;
			tmpStyle = cell.style.clone();
			tmpStyle.setHAlign(hAlign);
			cell.style = wb.findOrCreateStyle(tmpStyle);
		}
		return true;
	}
	
	/**
	 * @param {number} row
	 * @param {number} col
	 * @param {Workbook} wb
	 * @param {number} color
	 * @param {number} borderType
	 */
	setCellStyleBorderBottom(row, col, wb, color, borderType)
	{
		let cell;
		if ((cell = this.getCellData(row, col, true)) == null)
			return false;
		let tmpStyle;
		if (cell.style == null)
		{
			if (borderType == BorderType.None)
				return true;
			tmpStyle = new CellStyle(0);
			tmpStyle.setBorderBottom(new BorderStyle(color, borderType));
			cell.style = wb.findOrCreateStyle(tmpStyle);
		}
		else
		{
			
			if (cell.style.getBorderBottom().equals(new BorderStyle(color, borderType)))
				return true;
			tmpStyle = cell.style.clone();
			tmpStyle.setBorderBottom(new BorderStyle(color, borderType));
			cell.style = wb.findOrCreateStyle(tmpStyle);
		}
		return true;
	}

	/**
	 * @param {number} row
	 * @param {number} col
	 * @param {string | null} url
	 */
	setCellURL(row, col, url)
	{
		let cell;
		if ((cell = this.getCellData(row, col, false)) == null)
			return false;
		cell.cellURL = url;
		return true;
	}

	/**
	 * @param {number} row
	 * @param {number} col
	 * @param {string | null} val
	 * @param {CellStyle | null} style
	 */
	setCellString(row, col, val, style)
	{
		let cell;
		if ((cell = this.getCellData(row, col, false)) == null)
			return false;
		cell.cdt = CellDataType.String;
		cell.cellValue = val;
		if (style) cell.style = style;
		return true;
	}

	/**
	 * @param {number} row
	 * @param {number} col
	 * @param {data.Timestamp} val
	 * @param {CellStyle | null} style
	 */
	setCellTS(row, col, val, style)
	{
		let cell;
		if ((cell = this.getCellData(row, col, false)) == null)
			return false;
		cell.cdt = CellDataType.DateTime;
		cell.cellValue = val.toString("yyyy-MM-ddTHH:mm:ss.fffffffff");
		if (style) cell.style = style;
		return true;
	}

	/**
	 * @param {number} row
	 * @param {number} col
	 * @param {number} val
	 * @param {CellStyle | null} style
	 */
	setCellDouble(row, col, val, style)
	{
		let cell;
		if ((cell = this.getCellData(row, col, false)) == null)
			return false;
		cell.cdt = CellDataType.Number;
		cell.cellValue = ""+val;
		if (style) cell.style = style;
		return true;
	}

	/**
	 * @param {number} row
	 * @param {number} col
	 * @param {number} val
	 * @param {CellStyle | null} style
	 */
	setCellInt32(row, col, val, style)
	{
		let cell;
		if ((cell = this.getCellData(row, col, false)) == null)
			return false;
		cell.cdt = CellDataType.Number;
		cell.cellValue = ""+val;
		if (style) cell.style = style;
		return true;
	}

	/**
	 * @param {number} row
	 * @param {number} col
	 * @param {number} val
	 * @param {CellStyle | null} style
	 */
	setCellInt64(row, col, val, style)
	{
		let cell;
		if ((cell = this.getCellData(row, col, false)) == null)
			return false;
		cell.cdt = CellDataType.Number;
		cell.cellValue = ""+val;
		if (style) cell.style = style;
		return true;
	}

	/**
	 * @param {number} row
	 * @param {number} col
	 * @param {CellStyle | null} style
	 */
	setCellEmpty(row, col, style)
	{
		let cell;
		if ((cell = this.getCellData(row, col, false)) == null)
			return false;
		cell.cdt = CellDataType.Number;
		cell.cellValue = null;
		if (style) cell.style = style;
		return true;
	}

	/**
	 * @param {number} row
	 * @param {number} col
	 * @param {number} height
	 * @param {number} width
	 */
	mergeCells(row, col, height, width)
	{
		if (width == 0)
			return false;
		if (height == 0)
			return false;
		if (width == 0 && height == 0)
			return false;
	
		let cell;
		let i;
		let j;
		i = 0;
		while (i < height)
		{
			j = 0;
			while (j < width)
			{
				if ((cell = this.getCellData(row + i, col + j, true)) != null && (cell.cdt == CellDataType.MergedLeft || cell.cdt == CellDataType.MergedUp))
					return false;
				j++;
			}
			i++;
		}
	
		i = 0;
		while (i < height)
		{
			j = 0;
			while (j < width)
			{
				if ((cell = this.getCellData(row + i, col + j, true)) != null)
				{
					if (i == 0)
					{
						if (j == 0)
						{
							cell.mergeHori = width;
							cell.mergeVert = height;
						}
						else
						{
							cell.cdt = CellDataType.MergedLeft;
						}
					}
					else
					{
						cell.cdt = CellDataType.MergedUp;
					}
				}
				j++;
			}
			i++;
		}
		return true;
	}

	/**
	 * @param {number} row
	 * @param {number} col
	 */
	setCellMergeLeft(row, col)
	{
		if (col == 0)
			return false;
	
		let cell;
		let width = 1;
		let height = 1;
		if ((cell = this.getCellData(row, col, true)) != null)
			cell.cdt = CellDataType.MergedLeft;
		col--;
		width++;
		while (true)
		{
			if ((cell = this.getCellData(row, col, true)) == null)
			{
				return false;
			}
			else if (cell.cdt == CellDataType.MergedUp)
			{
				row--;
				height++;
			}
			else if (cell.cdt == CellDataType.MergedLeft)
			{
				col--;
				width++;
			}
			else
			{
				if (cell.mergeHori < width)
				{
					cell.mergeHori = width;
				}
				if (cell.mergeVert < height)
				{
					cell.mergeVert = height;
				}
				break;
			}
		}
		return true;
	}

	/**
	 * @param {number} row
	 * @param {number} col
	 */
	setCellMergeUp(row, col)
	{
		if (row == 0)
			return false;
	
		let cell;
		let width = 1;
		let height = 1;
		if ((cell = this.getCellData(row, col, true)) != null)
			cell.cdt = CellDataType.MergedUp;
		row--;
		height++;
		while (true)
		{
			if ((cell = this.getCellData(row, col, true)) == null)
			{
				return false;
			}
			else if (cell.cdt == CellDataType.MergedUp)
			{
				row--;
				height++;
			}
			else if (cell.cdt == CellDataType.MergedLeft)
			{
				col--;
				width++;
			}
			else
			{
				if (cell.mergeHori < width)
				{
					cell.mergeHori = width;
				}
				if (cell.mergeVert < height)
				{
					cell.mergeVert = height;
				}
				break;
			}
		}
		return true;
	}

	/**
	 * @param {number} row
	 * @param {boolean} hidden
	 */
	setRowHidden(row, hidden)
	{
		let cell;
		if (row >= 65536 || (cell = this.getCellData(row, 0, true)) == null)
			return false;
		cell.hidden = hidden;
		return true;
	}

	/**
	 * @param {number} row
	 * @param {number} height
	 */
	setRowHeight(row, height)
	{
		let rowData;
		if ((rowData = this.createRow(row)) != null)
		{
			rowData.height = height;
			return true;
		}
		else
		{
			return false;
		}
	}

	getCount()
	{
		return this.rows.length;
	}

	/**
	 * @param {number} row
	 */
	getItem(row)
	{
		return this.rows[row];
	}

	/**
	 * @param {number} col
	 */
	removeCol(col)
	{
		let i;
		let row;
	
		this.colWidthsPt.splice(col, 1);
		i = this.rows.length;
		while (i-- > 0)
		{
			if ((row = this.rows[i]) != null)
			{
				row.cells.splice(col, 1);
			}
		}
	}
	
	/**
	 * @param {number} col
	 */
	insertCol(col)
	{
		let i;
		let row;
	
		if (this.colWidthsPt.length > col)
		{
			this.colWidthsPt.splice(col, 0, 0);
		}
		i = this.rows.length;
		while (i-- > 0)
		{
			if ((row = this.rows[i]) != null)
			{
				if (row.cells.length > col)
				{
					row.cells.splice(col, 0, null);
				}
			}
		}
	}

	getMaxCol()
	{
		this.maxCol;
	}

	/**
	 * @param {number} col
	 * @param {number} width
	 * @param {unit.Distance.Unit} du
	 */
	setColWidth(col, width, du)
	{
		while (col >= this.colWidthsPt.length)
		{
			this.colWidthsPt.push(-1);
		}
		if (du == unit.Distance.Unit.POINT)
		{
			this.colWidthsPt[col] = width;
		}
		else
		{
			this.colWidthsPt[col] = unit.Distance.convert(du, unit.Distance.Unit.POINT, width);
		}
	}
	
	getColWidthCount()
	{
		return this.colWidthsPt.length;
	}

	/**
	 * @param {number} col
	 */
	getColWidthPt(col)
	{
		if (col >= this.colWidthsPt.length)
			return -1;
		return this.colWidthsPt[col];
	}
	
	/**
	 * @param {number} col
	 * @param {unit.Distance.Unit} du
	 */
	getColWidth(col, du)
	{
		if (col >= this.colWidthsPt.length)
			return -1;
		if (du == unit.Distance.Unit.POINT)
			return this.colWidthsPt[col];
		return unit.Distance.convert(unit.Distance.Unit.POINT, du, this.colWidthsPt[col]);
	}

	/**
	 * @param {number} row
	 * @param {number} col
	 */
	getCellDataRead(row, col)
	{
		let rowData;
		let cell;
		if (row >= this.rows.length + 65536)
			return null;
		if (col >= 65536)
			return null;
		if (col > this.maxCol)
		{
			return null;
		}
		while (true)
		{
			if ((rowData = this.rows[row]) == null)
				return null;
			if ((cell = rowData.cells[col]) == null)
			{
				return null;
			}
			if (cell.cdt == CellDataType.MergedLeft)
			{
				col--;
			}
			else if (cell.cdt == CellDataType.MergedUp)
			{
				row--;
			}
			else
			{
				break;
			}
		}
		return cell;		
	}

	/**
	 * @param {{cdt: number,cellValue: string|null,style: CellStyle|null,mergeHori: number,mergeVert: number,hidden: boolean,cellURL: string|null} | null} cell
	 */
	getCellString(cell)
	{
		let cellValue;
		if (cell == null || (cellValue = cell.cellValue) == null)
		{
			return false;
		}
		if (cell.cdt == CellDataType.Number)
		{
			let v = Number(cellValue);
			let iv;
			if (Number.isNaN(v))
			{
				return cellValue;
			}
			else
			{
				return ""+v;
/*				if (iv == v)
				{
					sb.AppendI32(iv);
				}
				else
				{
					sb.AppendDouble(v);
				}*/
	/*			Text.String *fmt;
				if (cell.style && (fmt = cell.style.GetDataFormat()) != 0)
				{
					printf("Style: %s\r\n", fmt.v);
				}
				else
				{
					printf("Style: null\r\n");
				}*/
			}
		}
		else
		{
			return cellValue;
		}
	}

	getDrawingCount()
	{
		return this.drawings.length;
	}

	/**
	 * @param {number} index
	 */
	getDrawing(index)
	{
		return this.drawings[index];	
	}

	/**
	 * @param {number} index
	 */
	getDrawingNoCheck(index)
	{
		let o = this.drawings[index];
		if (o == null)
			throw new Error("Drawing is null");
		return o;
	}

	/**
	 * @param {unit.Distance.Unit} du
	 * @param {number} x
	 * @param {number} y
	 * @param {number} w
	 * @param {number} h
	 */
	createDrawing(du, x, y, w, h)
	{
		let inch = unit.Distance.Unit.INCH;
		let drawing = {
			anchorType: AnchorType.Absolute,
			posXInch: unit.Distance.convert(du, inch, x),
			posYInch: unit.Distance.convert(du, inch, y),
			widthInch: unit.Distance.convert(du, inch, w),
			heightInch: unit.Distance.convert(du, inch, h),
			col1: 0,
			row1: 0,
			col2: 0,
			row2: 0,
			/** @type {OfficeChart|null} */
			chart: null
		};
		this.drawings.push(drawing);
		return drawing;
	}

	/**
	 * @param {unit.Distance.Unit} du
	 * @param {number} x
	 * @param {number} y
	 * @param {number} w
	 * @param {number} h
	 * @param {string | null} title
	 */
	createChart(du, x, y, w, h, title)
	{
		let drawing = this.createDrawing(du, x, y, w, h);
		let chart = new OfficeChart(du, x, y, w, h);
		drawing.chart = chart;
		if (title && title.length > 0)
		{
			chart.setTitleText(title);
		}
		chart.setShapeProp(new OfficeShapeProp(
			OfficeFill.newSolidFill(OfficeColor.newPreset(PresetColor.White)),
			new OfficeLineStyle(OfficeFill.newSolidFill(null))));
		return chart;
	}
}


export class Workbook extends data.ParsedObject
{
	static defPalette = [
		0xff000000, 0xffffffff, 0xffff0000, 0xff00ff00, 0xff0000ff, 0xffffff00, 0xffff00ff, 0xff00ffff,
		0xff800000, 0xff008000, 0xff000080, 0xff808000, 0xff800080, 0xff008080, 0xffc0c0c0, 0xff808080,
		0xff9999ff, 0xff993366, 0xffffffcc, 0xffccffff, 0xff660066, 0xffff8080, 0xff0066cc, 0xffccccff,
		0xff000080, 0xffff00ff, 0xffffff00, 0xff00ffff, 0xff800080, 0xff800000, 0xff008080, 0xff0000ff,
		0xff00ccff, 0xffccffff, 0xffccffcc, 0xffffff99, 0xff99ccff, 0xffff99cc, 0xffcc99ff, 0xffffcc99,
		0xff3366ff, 0xff33cccc, 0xff99cc00, 0xffffcc00, 0xffff9900, 0xffff6600, 0xff666699, 0xff969696,
		0xff003366, 0xff339966, 0xff003300, 0xff333300, 0xff993300, 0xff993366, 0xff333399, 0xff333333
	];

	constructor()
	{
		super("Untitled", "Workbook");
		/**
		 * @type {string|null}
		 */
		this.author = null;
		/**
		 * @type {string|null}
		 */
		this.lastAuthor = null;
		/**
		 * @type {string|null}
		 */
		this.company = null;
		/**
		 * @type {data.Timestamp|null}
		 */
		this.createTime = null;
		/**
		 * @type {data.Timestamp|null}
		 */
		this.modifyTime = null;
		this.version = 0;
		this.windowTopX = 0;
		this.windowTopY = 0;
		this.windowWidth = 0;
		this.windowHeight = 0;
		this.activeSheet = 0;
		/** @type {number[]} */
		this.palette = new Array(56);
		/** @type {Worksheet[]} */
		this.sheets = [];
		/** @type {CellStyle[]} */
		this.styles = [];
		/** @type {WorkbookFont[]} */
		this.fonts = [];
	
		let i;
		for (i in Workbook.defPalette)
		{
			this.palette[i] = Workbook.defPalette[i];
		}
	
		this.newCellStyle(null, text.HAlignment.Unknown, text.VAlignment.Bottom, "general");
		this.newFont("Arial", 10.0, false).setFamily(FontFamily.Swiss);
		this.newFont("Arial", 10.0, false);
		this.newFont("Arial", 10.0, false);
		this.newFont("Arial", 10.0, false);
	
	}

	clone()
	{
		let i;
		let j;
		let newWB = new Workbook();
		newWB.author = this.author;
		newWB.lastAuthor = this.lastAuthor;
		newWB.company = this.company;
		newWB.createTime = this.createTime;
		newWB.modifyTime = this.modifyTime;
		newWB.version = this.version;
		newWB.windowTopX = this.windowTopX;
		newWB.windowTopY = this.windowTopY;
		newWB.windowWidth = this.windowWidth;
		newWB.windowHeight = this.windowHeight;
		newWB.activeSheet = this.activeSheet;
		for (i in this.palette)
		{
			newWB.palette[i] = this.palette[i];
		}

		for (i in this.styles)
		{
			newWB.styles.push(this.styles[i].clone());
		}
		for (i in this.sheets)
		{
			newWB.sheets.push(this.sheets[i].clone(this, newWB));
		}
		i = 0;
		j = this.fonts.length;
		while (i < j)
		{
			newWB.fonts.push(this.fonts[i].clone());
			i++;
		}
		return newWB;
	}

	addDefaultStyles()
	{
		let style;
		while (this.styles.length < 21)
		{
			style = new CellStyle(this.styles.length);
			this.styles.push(style);
		}
	}

	/**
	 * @param {string | null} author
	 */
	setAuthor(author)
	{
		this.author = author;
	}

	/**
	 * @param {string | null} lastAuthor
	 */
	setLastAuthor(lastAuthor)
	{
		this.lastAuthor = lastAuthor;	
	}

	/**
	 * @param {string | null} company
	 */
	setCompany(company)
	{
		this.company = company;
	}

	/**
	 * @param {data.Timestamp | null} createTime
	 */
	setCreateTime(createTime)
	{
		this.createTime = createTime;
	}

	/**
	 * @param {data.Timestamp | null} modifyTime
	 */
	setModifyTime(modifyTime)
	{
		this.modifyTime = modifyTime;
	}

	/**
	 * @param {number} version
	 */
	setVersion(version)
	{
		this.version = version;
	}

	getAuthor()
	{
		return this.author;
	}

	getLastAuthor()
	{
		return this.lastAuthor;
	}

	getCompany()
	{
		return this.company;
	}

	getCreateTime()
	{
		return this.createTime;
	}

	getModifyTime()
	{
		return this.modifyTime;
	}

	getVersion()
	{
		return this.version;
	}

	hasInfo()
	{
		if (this.author != null)
			return true;
		if (this.lastAuthor != null)
			return true;
		if (this.company != null)
			return true;
		if (this.createTime != null)
			return true;
		if (this.version != 0)
			return true;
		return false;
	}

	/**
	 * @param {number} windowTopX
	 */
	setWindowTopX(windowTopX)
	{
		this.windowTopX = windowTopX;
	}

	/**
	 * @param {number} windowTopY
	 */
	setWindowTopY(windowTopY)
	{
		this.windowTopY = windowTopY;
	}

	/**
	 * @param {number} windowWidth
	 */
	setWindowWidth(windowWidth)
	{
		this.windowWidth = windowWidth;
	}

	/**
	 * @param {number} windowHeight
	 */
	setWindowHeight(windowHeight)
	{
		this.windowHeight = windowHeight;
	}

	/**
	 * @param {number} index
	 */
	setActiveSheet(index)
	{
		this.activeSheet = index;
	}

	getWindowTopX()
	{
		return this.windowTopX;
	}

	getWindowTopY()
	{
		return this.windowTopY;
	}

	getWindowWidth()
	{
		return this.windowWidth;
	}

	getWindowHeight()
	{
		return this.windowHeight;
	}

	getActiveSheet()
	{
		return this.activeSheet;
	}

	hasWindowInfo()
	{
		if (this.windowTopX != 0 || this.windowTopY != 0 || this.windowWidth != 0 || this.windowHeight != 0 || this.activeSheet != 0)
			return true;
		return false;
	}

	hasCellStyle()
	{
		if (this.styles.length > 0)
			return true;
		return false;
	}

	newCellStyleDef()
	{
		let style = new CellStyle(this.styles.length);
		this.styles.push(style);
		return style;
	}

	/**
	 * @param {WorkbookFont | null} font
	 * @param {text.HAlignment} halign
	 * @param {text.VAlignment} valign
	 * @param {string | null} dataFormat
	 */
	newCellStyle(font, halign, valign, dataFormat)
	{
		let style = new CellStyle(this.styles.length);
		style.setFont(font);
		style.setHAlign(halign);
		style.setVAlign(valign);
		style.setDataFormat(dataFormat);
		this.styles.push(style);
		return style;
	}

	getStyleCount()
	{
		return this.styles.length;
	}

	/**
	 * @param {CellStyle} style
	 */
	getStyleIndex(style)
	{
		let i = this.styles.length;
		while (i-- > 0)
		{
			if (this.styles[i] == style)
				return i;
		}
		return -1;
	}

	/**
	 * @param {number} index
	 */
	getStyle(index)
	{
		return this.styles[index];
	}

	/**
	 * @param {CellStyle} tmpStyle
	 */
	findOrCreateStyle(tmpStyle)
	{
		let style;
		let i = this.styles.length;
		while (i-- > 0)
		{
			if ((style = this.styles[i]) != null)
			{
				if (style.equals(tmpStyle))
				{
					return style;
				}
			}
		}
		style = tmpStyle.clone();
		style.setIndex(this.styles.length);
		this.styles.push(style);
		return style;
	}

	getDefaultStyle()
	{
		return this.styles[0];
	}

	getPalette()
	{
		return this.palette;
	}

	/**
	 * @param {number[]} palette
	 */
	setPalette(palette)
	{
		let i;
		for (i in palette)
		{
			this.palette[i] = palette[i];
		}
	}

	/**
	 * @param {string | null} name
	 */
	addWorksheet(name)
	{
		if (name == null)
			name = "Sheet"+ this.sheets.length;
		let ws = new Worksheet(name);
		this.sheets.push(ws);
		return ws;
	}
	
	/**
	 * @param {number} index
	 * @param {string} name
	 */
	insertWorksheet(index, name)
	{
		let ws = new Worksheet(name);
		this.sheets.splice(index, 0, ws);
		return ws;
	}

	getCount()
	{
		return this.sheets.length;
	}

	/**
	 * @param {number} index
	 */
	getItem(index)
	{
		return this.sheets[index];
	}

	/**
	 * @param {number} index
	 */
	removeAt(index)
	{
		return this.sheets.splice(index, 1);
	}

	/**
	 * @param {string} name
	 */
	getWorksheetByName(name)
	{
		let i;
		for (i in this.sheets)
		{
			if (this.sheets[i].getName() == name)
				return this.sheets[i];
		}
		return null;
	}

	getFontCount()
	{
		return this.fonts.length;
	}

	/**
	 * @param {number} index
	 */
	getFontNoCheck(index)
	{
		let fnt = this.fonts[index];
		if (fnt == null)
			throw new Error("Font is null");
		return fnt;
	}

	/**
	 * @param {number} index
	 */
	getFont(index)
	{
		return this.fonts[index];
	}

	/**
	 * @param {WorkbookFont} font
	 */
	getFontIndex(font)
	{
		let i;
		for (i in this.fonts)
		{
			if (this.fonts[i] == font)
			{
				return i;
			}
		}
		return -1;
	}

	/**
	 * @param {string | null} name
	 * @param {number} size
	 * @param {boolean} bold
	 */
	newFont(name, size, bold)
	{
		let font = new WorkbookFont();
		this.fonts.push(font);
		font.setName(name);
		font.setSize(size);
		font.setBold(bold);
		return font;
	}

	static getDefPalette()
	{
		return Workbook.defPalette;
	}

	/**
	 * @param {number} col
	 */
	static colCode(col)
	{
		if (col < 26)
		{
			return String.fromCharCode(65 + col);
		}
		col -= 26;
		if (col < 26 * 26)
		{
			return String.fromCharCode(65 + (col / 26), 65 + (col % 26));
		}
		let sbuff = new Array(3);
		col -= 26 * 26;
		sbuff[2] = (65 + (col % 26));
		col = col / 26;
		sbuff[1] = (65 + (col % 26));
		sbuff[0] = (65 + (col / 26));
		return String.fromCharCode(sbuff[0], sbuff[1], sbuff[2]);		
	}
}

export class XLSUtil
{
	/**
	 * @param {data.Timestamp} ts
	 */
	static date2Number(ts)
	{
		ts = ts.setTimeZoneQHR(0);
		let secs = ts.inst.sec;
		let days = Number(secs / 86400n) + 25569;
		secs -= BigInt(days - 25569) * 86400n;
		while (secs < 0)
		{
			secs += 86400n;
			days -= 1;
		}
		return days + Number(secs) / 86400.0 + (ts.inst.nanosec + 50000) / 86400000000000.0;
	}

	/**
	 * @param {number} v
	 */
	static number2Timestamp(v)
	{
		let days = Math.floor(v);
		let tz = data.DateTimeUtil.getLocalTzQhr();
		let ds = (v - days);
		let s = Math.floor(ds * 86400);
		return new data.Timestamp(new data.TimeInstant(BigInt(days - 25569) * 86400n + BigInt(s), Math.round((ds * 86400 - s) * 1000000000)), tz);
	}

	/**
	 * @param {number} col
	 * @param {number} row
	 */
	static getCellID(col, row)
	{
		let s;
		if (col >= 26)
		{
			s = String.fromCharCode(0x41 + Math.floor(col / 26) - 1, 0x41 + (col % 26));
		}
		else
		{
			s = String.fromCharCode(0x41 + col);
		}
		return s + (row + 1);
	}
}
