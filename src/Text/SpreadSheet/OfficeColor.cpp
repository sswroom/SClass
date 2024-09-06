#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/SpreadSheet/OfficeColor.h"

#define RGBValue(r, g, b) (0xFF000000 + ((r) << 16) + ((g) << 8) + (b))

Text::SpreadSheet::OfficeColor::OfficeColor(PresetColor color)
{
	this->colorType = ColorType::Preset;
	this->color.preset = color;
}

Text::SpreadSheet::OfficeColor::OfficeColor(UInt32 argb)
{
	this->colorType = ColorType::Argb;
	this->color.argb = argb;
}

Text::SpreadSheet::OfficeColor::~OfficeColor()
{
}

Text::SpreadSheet::ColorType Text::SpreadSheet::OfficeColor::GetColorType()
{
	return this->colorType;
}

Text::SpreadSheet::PresetColor Text::SpreadSheet::OfficeColor::GetPresetColor()
{
	return this->color.preset;
}

UInt32 Text::SpreadSheet::OfficeColor::GetColorArgb()
{
	switch (this->colorType)
	{
	case ColorType::Argb:
		return this->color.argb;
	case ColorType::Preset:
		return PresetColorGetArgb(this->color.preset);
	}
	return 0;
}

NN<Text::SpreadSheet::OfficeColor> Text::SpreadSheet::OfficeColor::NewPreset(PresetColor color)
{
	NN<OfficeColor> c;
	NEW_CLASSNN(c, OfficeColor(color));
	return c;
}

NN<Text::SpreadSheet::OfficeColor> Text::SpreadSheet::OfficeColor::NewArgb(UInt32 argb)
{
	NN<OfficeColor> c;
	NEW_CLASSNN(c, OfficeColor(argb));
	return c;
}

UInt32 Text::SpreadSheet::OfficeColor::PresetColorGetArgb(PresetColor color)
{
	switch (color)
	{
	case PresetColor::AliceBlue:
		return RGBValue(240,248,255);
	case PresetColor::AntiqueWhite:
		return RGBValue(250,235,215);
	case PresetColor::Aqua:
		return RGBValue(0,255,255);
	case PresetColor::Aquamarine:
		return RGBValue(127,255,212);
	case PresetColor::Azure:
		return RGBValue(240,255,255);
	case PresetColor::Beige:
		return RGBValue(245,245,220);
	case PresetColor::Bisque:
		return RGBValue(255,228,196);
	case PresetColor::Black:
		return RGBValue(0,0,0);
	case PresetColor::BlanchedAlmond:
		return RGBValue(255,235,205);
	case PresetColor::Blue:
		return RGBValue(0,0,255);
	case PresetColor::BlueViolet:
		return RGBValue(138,43,226);
	case PresetColor::Brown:
		return RGBValue(165,42,42);
	case PresetColor::BurlyWood:
		return RGBValue(222,184,135);
	case PresetColor::CadetBlue:
		return RGBValue(95,158,160);
	case PresetColor::Chartreuse:
		return RGBValue(127,255,0);
	case PresetColor::Chocolate:
		return RGBValue(210,105,30);
	case PresetColor::Coral:
		return RGBValue(255,127,80);
	case PresetColor::CornflowerBlue:
		return RGBValue(100,149,237);
	case PresetColor::Cornsilk:
		return RGBValue(255,248,220);
	case PresetColor::Crimson:
		return RGBValue(220,20,60);
	case PresetColor::Cyan:
		return RGBValue(0,255,255);
	case PresetColor::DeepPink:
		return RGBValue(255,20,147);
	case PresetColor::DeepSkyBlue:
		return RGBValue(0,191,255);
	case PresetColor::DimGray:
		return RGBValue(105,105,105);
	case PresetColor::DarkBlue:
		return RGBValue(0,0,139);
	case PresetColor::DarkCyan:
		return RGBValue(0,139,139);
	case PresetColor::DarkGoldenrod:
		return RGBValue(184,134,11);
	case PresetColor::DarkGray:
		return RGBValue(169,169,169);
	case PresetColor::DarkGreen:
		return RGBValue(0,100,0);
	case PresetColor::DarkKhaki:
		return RGBValue(189,183,107);
	case PresetColor::DarkMagenta:
		return RGBValue(139,0,139);
	case PresetColor::DarkOliveGreen:
		return RGBValue(85,107,47);
	case PresetColor::DarkOrange:
		return RGBValue(255,140,0);
	case PresetColor::DarkOrchid:
		return RGBValue(153,50,204);
	case PresetColor::DarkRed:
		return RGBValue(153,50,204);
	case PresetColor::DarkSalmon:
		return RGBValue(233,150,122);
	case PresetColor::DarkSeaGreen:
		return RGBValue(143,188,139);
	case PresetColor::DarkSlateBlue:
		return RGBValue(72,61,139);
	case PresetColor::DarkSlateGray:
		return RGBValue(47,79,79);
	case PresetColor::DarkTurquoise:
		return RGBValue(0,206,209);
	case PresetColor::DarkViolet:
		return RGBValue(148,0,211);
	case PresetColor::DodgerBlue:
		return RGBValue(30,144,255);
	case PresetColor::Firebrick:
		return RGBValue(178,34,34);
	case PresetColor::FloralWhite:
		return RGBValue(255,250,240);
	case PresetColor::ForestGreen:
		return RGBValue(34,139,34);
	case PresetColor::Fuchsia:
		return RGBValue(255,0,255);
	case PresetColor::Gainsboro:
		return RGBValue(220,220,220);
	case PresetColor::GhostWhite:
		return RGBValue(248,248,255);
	case PresetColor::Gold:
		return RGBValue(255,215,0);
	case PresetColor::Goldenrod:
		return RGBValue(218,165,32);
	case PresetColor::Gray:
		return RGBValue(128,128,128);
	case PresetColor::Green:
		return RGBValue(0,128,0);
	case PresetColor::GreenYellow:
		return RGBValue(173,255,47);
	case PresetColor::Honeydew:
		return RGBValue(240,255,240);
	case PresetColor::HotPink:
		return RGBValue(255,105,180);
	case PresetColor::IndianRed:
		return RGBValue(205,92,92);
	case PresetColor::Indigo:
		return RGBValue(75,0,130);
	case PresetColor::Ivory:
		return RGBValue(255,255,240);
	case PresetColor::Khaki:
		return RGBValue(240,230,140);
	case PresetColor::Lavender:
		return RGBValue(230,230,250);
	case PresetColor::LavenderBlush:
		return RGBValue(255,240,245);
	case PresetColor::LawnGreen:
		return RGBValue(124,252,0);
	case PresetColor::LemonChiffon:
		return RGBValue(255,250,205);
	case PresetColor::Lime:
		return RGBValue(0,255,0);
	case PresetColor::LimeGreen:
		return RGBValue(50,205,50);
	case PresetColor::Linen:
		return RGBValue(250,240,230);
	case PresetColor::LightBlue:
		return RGBValue(173,216,230);
	case PresetColor::LightCoral:
		return RGBValue(240,128,128);
	case PresetColor::LightCyan:
		return RGBValue(224,255,255);
	case PresetColor::LightGoldenrodYellow:
		return RGBValue(250,250,120);
	case PresetColor::LightGray:
		return RGBValue(211,211,211);
	case PresetColor::LightGreen:
		return RGBValue(144,238,144);
	case PresetColor::LightPink:
		return RGBValue(255,182,193);
	case PresetColor::LightSalmon:
		return RGBValue(255,160,122);
	case PresetColor::LightSeaGreen:
		return RGBValue(32,178,170);
	case PresetColor::LightSkyBlue:
		return RGBValue(135,206,250);
	case PresetColor::LightSlateGray:
		return RGBValue(119,136,153);
	case PresetColor::LightSteelBlue:
		return RGBValue(176,196,222);
	case PresetColor::LightYellow:
		return RGBValue(255,255,224);
	case PresetColor::Magenta:
		return RGBValue(255,0,255);
	case PresetColor::Maroon:
		return RGBValue(128,0,0);
	case PresetColor::MediumAquamarine:
		return RGBValue(102,205,170);
	case PresetColor::MediumBlue:
		return RGBValue(0,0,205);
	case PresetColor::MediumOrchid:
		return RGBValue(186,85,211);
	case PresetColor::MediumPurple:
		return RGBValue(147,112,219);
	case PresetColor::MediumSeaGreen:
		return RGBValue(60,179,113);
	case PresetColor::MediumSlateBlue:
		return RGBValue(123,104,238);
	case PresetColor::MediumSpringGreen:
		return RGBValue(0,250,154);
	case PresetColor::MediumTurquoise:
		return RGBValue(72,209,204);
	case PresetColor::MediumVioletRed:
		return RGBValue(199,21,133);
	case PresetColor::MidnightBlue:
		return RGBValue(25,25,112);
	case PresetColor::MintCream:
		return RGBValue(245,255,250);
	case PresetColor::MistyRose:
		return RGBValue(255,228,225);
	case PresetColor::Moccasin:
		return RGBValue(255,228,181);
	case PresetColor::NavajoWhite:
		return RGBValue(255,222,173);
	case PresetColor::Navy:
		return RGBValue(0,0,128);
	case PresetColor::OldLace:
		return RGBValue(253,245,230);
	case PresetColor::Olive:
		return RGBValue(128,128,0);
	case PresetColor::OliveDrab:
		return RGBValue(107,142,35);
	case PresetColor::Orange:
		return RGBValue(255,165,0);
	case PresetColor::OrangeRed:
		return RGBValue(255,69,0);
	case PresetColor::Orchid:
		return RGBValue(218,112,214);
	case PresetColor::PaleGoldenrod:
		return RGBValue(238,232,170);
	case PresetColor::PaleGreen:
		return RGBValue(152,251,152);
	case PresetColor::PaleTurquoise:
		return RGBValue(175,238,238);
	case PresetColor::PaleVioletRed:
		return RGBValue(219,112,147);
	case PresetColor::PapayaWhip:
		return RGBValue(255,239,213);
	case PresetColor::PeachPuff:
		return RGBValue(255,218,185);
	case PresetColor::Peru:
		return RGBValue(205,133,63);
	case PresetColor::Pink:
		return RGBValue(255,192,203);
	case PresetColor::Plum:
		return RGBValue(221,160,221);
	case PresetColor::PowderBlue:
		return RGBValue(176,224,230);
	case PresetColor::Purple:
		return RGBValue(128,0,128);
	case PresetColor::Red:
		return RGBValue(255,0,0);
	case PresetColor::RosyBrown:
		return RGBValue(188,143,143);
	case PresetColor::RoyalBlue:
		return RGBValue(65,105,225);
	case PresetColor::SaddleBrown:
		return RGBValue(139,69,19);
	case PresetColor::Salmon:
		return RGBValue(250,128,114);
	case PresetColor::SandyBrown:
		return RGBValue(244,164,96);
	case PresetColor::SeaGreen:
		return RGBValue(46,139,87);
	case PresetColor::SeaShell:
		return RGBValue(255,245,238);
	case PresetColor::Sienna:
		return RGBValue(160,82,45);
	case PresetColor::Silver:
		return RGBValue(192,192,192);
	case PresetColor::SkyBlue:
		return RGBValue(135,206,235);
	case PresetColor::SlateBlue:
		return RGBValue(106,90,205);
	case PresetColor::SlateGray:
		return RGBValue(112,128,144);
	case PresetColor::Snow:
		return RGBValue(255,250,250);
	case PresetColor::SpringGreen:
		return RGBValue(0,255,127);
	case PresetColor::SteelBlue:
		return RGBValue(70,130,180);
	case PresetColor::Tan:
		return RGBValue(210,180,140);
	case PresetColor::Teal:
		return RGBValue(0,128,128);
	case PresetColor::Thistle:
		return RGBValue(216,191,216);
	case PresetColor::Tomato:
		return RGBValue(255,99,71);
	case PresetColor::Turquoise:
		return RGBValue(64,224,208);
	case PresetColor::Violet:
		return RGBValue(238,130,238);
	case PresetColor::Wheat:
		return RGBValue(245,222,179);
	case PresetColor::White:
		return RGBValue(255,255,255);
	case PresetColor::WhiteSmoke:
		return RGBValue(245,245,245);
	case PresetColor::Yellow:
		return RGBValue(255,255,0);
	case PresetColor::YellowGreen:
		return RGBValue(154,205,50);
	default:
		return 0;
	}
}
