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

Text::SpreadSheet::OfficeColor *Text::SpreadSheet::OfficeColor::NewPreset(PresetColor color)
{
	return NEW_CLASS_D(OfficeColor(color));
}

Text::SpreadSheet::OfficeColor *Text::SpreadSheet::OfficeColor::NewArgb(UInt32 argb)
{
	return NEW_CLASS_D(OfficeColor(argb));
}

UInt32 Text::SpreadSheet::OfficeColor::PresetColorGetArgb(PresetColor color)
{
	switch (color)
	{
	case PresetColor::AliceBlue:
		return RGBValue(240,248,255);
	case PresetColor::AntiqueWhite:
	case PresetColor::Aqua:
	case PresetColor::Aquamarine:
	case PresetColor::Azure:
	case PresetColor::Beige:
	case PresetColor::Bisque:
	case PresetColor::Black:
	case PresetColor::BlanchedAlmond:
	case PresetColor::Blue:
	case PresetColor::BlueViolet:
	case PresetColor::Brown:
	case PresetColor::BurlyWood:
	case PresetColor::CadetBlue:
	case PresetColor::Chartreuse:
	case PresetColor::Chocolate:
	case PresetColor::Coral:
	case PresetColor::CornflowerBlue:
	case PresetColor::Cornsilk:
	case PresetColor::Crimson:
	case PresetColor::Cyan:
	case PresetColor::DeepPink:
	case PresetColor::DeepSkyBlue:
	case PresetColor::DimGray:
	case PresetColor::DarkBlue:
	case PresetColor::DarkCyan:
	case PresetColor::DarkGoldenrod:
	case PresetColor::DarkGray:
	case PresetColor::DarkGreen:
	case PresetColor::DarkKhaki:
	case PresetColor::DarkMagenta:
	case PresetColor::DarkOliveGreen:
	case PresetColor::DarkOrange:
	case PresetColor::DarkOrchid:
	case PresetColor::DarkRed:
	case PresetColor::DarkSalmon:
	case PresetColor::DarkSeaGreen:
	case PresetColor::DarkSlateBlue:
	case PresetColor::DarkSlateGray:
	case PresetColor::DarkTurquoise:
	case PresetColor::DarkViolet:
	case PresetColor::DodgerBlue:
	case PresetColor::Firebrick:
	case PresetColor::FloralWhite:
	case PresetColor::ForestGreen:
	case PresetColor::Fuchsia:
	case PresetColor::Gainsboro:
	case PresetColor::GhostWhite:
	case PresetColor::Gold:
	case PresetColor::Goldenrod:
	case PresetColor::Gray:
	case PresetColor::Green:
	case PresetColor::GreenYellow:
	case PresetColor::Honeydew:
	case PresetColor::HotPink:
	case PresetColor::IndianRed:
	case PresetColor::Indigo:
	case PresetColor::Ivory:
	case PresetColor::Khaki:
	case PresetColor::Lavender:
	case PresetColor::LavenderBlush:
	case PresetColor::LawnGreen:
	case PresetColor::LemonChiffon:
	case PresetColor::Lime:
	case PresetColor::LimeGreen:
	case PresetColor::Linen:
	case PresetColor::LightBlue:
	case PresetColor::LightCoral:
	case PresetColor::LightCyan:
	case PresetColor::LightGoldenrodYellow:
	case PresetColor::LightGray:
	case PresetColor::LightGreen:
	case PresetColor::LightPink:
	case PresetColor::LightSalmon:
	case PresetColor::LightSeaGreen:
	case PresetColor::LightSkyBlue:
	case PresetColor::LightSlateGray:
	case PresetColor::LightSteelBlue:
	case PresetColor::LightYellow:
	case PresetColor::Magenta:
	case PresetColor::Maroon:
	case PresetColor::MediumAquamarine:
	case PresetColor::MediumBlue:
	case PresetColor::MediumOrchid:
	case PresetColor::MediumPurple:
	case PresetColor::MediumSeaGreen:
	case PresetColor::MediumSlateBlue:
	case PresetColor::MediumSpringGreen:
	case PresetColor::MediumTurquoise:
	case PresetColor::MediumVioletRed:
	case PresetColor::MidnightBlue:
	case PresetColor::MintCream:
	case PresetColor::MistyRose:
	case PresetColor::Moccasin:
	case PresetColor::NavajoWhite:
	case PresetColor::Navy:
	case PresetColor::OldLace:
	case PresetColor::Olive:
	case PresetColor::OliveDrab:
	case PresetColor::Orange:
	case PresetColor::OrangeRed:
	case PresetColor::Orchid:
	case PresetColor::PaleGoldenrod:
	case PresetColor::PaleGreen:
	case PresetColor::PaleTurquoise:
	case PresetColor::PaleVioletRed:
	case PresetColor::PapayaWhip:
	case PresetColor::PeachPuff:
	case PresetColor::Peru:
	case PresetColor::Pink:
	case PresetColor::Plum:
	case PresetColor::PowderBlue:
	case PresetColor::Purple:
	case PresetColor::Red:
	case PresetColor::RosyBrown:
	case PresetColor::RoyalBlue:
	case PresetColor::SaddleBrown:
	case PresetColor::Salmon:
	case PresetColor::SandyBrown:
	case PresetColor::SeaGreen:
	case PresetColor::SeaShell:
	case PresetColor::Sienna:
	case PresetColor::Silver:
	case PresetColor::SkyBlue:
	case PresetColor::SlateBlue:
	case PresetColor::SlateGray:
	case PresetColor::Snow:
	case PresetColor::SpringGreen:
	case PresetColor::SteelBlue:
	case PresetColor::Tan:
	case PresetColor::Teal:
	case PresetColor::Thistle:
	case PresetColor::Tomato:
	case PresetColor::Turquoise:
	case PresetColor::Violet:
	case PresetColor::Wheat:
	case PresetColor::White:
	case PresetColor::WhiteSmoke:
	case PresetColor::Yellow:
//	case PresetColor::YellowGreen:
	default:
		return 0;
	}
}
