#include "Stdafx.h"
#include "Text/CSSColor.h"
#include "Text/StandardColor.h"
#include "Text/StringBuilderUTF8.h"

UInt32 Text::CSSColor::Parse(Text::CStringNN colorStr)
{
	if (colorStr.StartsWith('#'))
	{
		UInt32 v;
		if (!Text::StrHex2UInt32V(&colorStr.v[1], v))
		{
			printf("CSSColor: Invalid hex color: %s\r\n", colorStr.v.Ptr());
			return 0;
		}
		if (colorStr.leng == 4)
		{
			UInt32 r = (v >> 8) & 0xf;
			UInt32 g = (v >> 4) & 0xf;
			UInt32 b = v & 0xf;
			r = (r << 4) | r;
			g = (g << 4) | g;
			b = (b << 4) | b;
			return 0xff000000 | (r << 16) | (g << 8) | b;
		}
		else if (colorStr.leng == 7)
		{
			return 0xff000000 | v;
		}
		else if (colorStr.leng == 5)
		{
			UInt32 r = (v >> 12) & 0xf;
			UInt32 g = (v >> 8) & 0xf;
			UInt32 b = (v >> 4) & 0xf;
			UInt32 a = v & 0xf;
			r = (r << 4) | r;
			g = (g << 4) | g;
			b = (b << 4) | b;
			a = (a << 4) | a;
			return (a << 24) | (r << 16) | (g << 8) | b;
		}
		else if (colorStr.leng == 9)
		{
			return (v >> 8) | ((v & 0xff) << 24);
		}
		else
		{
			printf("CSSColor: Invalid hex color type: %s\r\n", colorStr.v.Ptr());
			return 0;
		}
	}
	else if (colorStr.StartsWith(UTF8STRC("rgb(")) && colorStr.EndsWith(')'))
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(&colorStr.v[4], colorStr.leng - 5);
		Text::PString sarr[4];
		UInt32 r = 0;
		UInt32 g = 0;
		UInt32 b = 0;
		if (Text::StrSplitTrimP(sarr, 4, sb, ',') == 3 && sarr[0].ToUInt32(r) && sarr[1].ToUInt32(g) && sarr[2].ToUInt32(b))
		{
			return 0xff000000 | (r << 16) | (g << 8) | b;
		}
		else
		{
			printf("CSSColor: Invalid rgb() color: %s\r\n", colorStr.v.Ptr());
			return 0;
		}
	}
	else if (colorStr.StartsWith(UTF8STRC("rgba(")) && colorStr.EndsWith(')'))
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(&colorStr.v[5], colorStr.leng - 6);
		Text::PString sarr[5];
		UInt32 r = 0;
		UInt32 g = 0;
		UInt32 b = 0;
		Double a = 0;
		if (Text::StrSplitTrimP(sarr, 5, sb, ',') == 4 && sarr[0].ToUInt32(r) && sarr[1].ToUInt32(g) && sarr[2].ToUInt32(b) && sarr[3].ToDouble(a))
		{
			return ((UInt32)Double2Int32(a * 255) << 24) | (r << 16) | (g << 8) | b;
		}
		else
		{
			printf("CSSColor: Invalid rgba() color: %s\r\n", colorStr.v.Ptr());
			return 0;
		}
	}
	else if (colorStr.StartsWith(UTF8STRC("hsl(")) && colorStr.EndsWith(')'))
	{
		printf("CSSColor: hsl() color not supported: %s\r\n", colorStr.v.Ptr());
		return 0;
	}
	else if (colorStr.StartsWith(UTF8STRC("hsla(")) && colorStr.EndsWith(')'))
	{
		printf("CSSColor: hsla() color not supported: %s\r\n", colorStr.v.Ptr());
		return 0;
	}
	else if (colorStr.EqualsICase(UTF8STRC("transparent")))
	{
		return 0;
	}
	else if (colorStr.EqualsICase(UTF8STRC("Black")))
	{
		return StandardColorGetARGB32(StandardColor::Black);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkBlue")))
	{
		return StandardColorGetARGB32(StandardColor::DarkBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("ConsoleDarkGreen")))
	{
		return StandardColorGetARGB32(StandardColor::ConsoleDarkGreen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkCyan")))
	{
		return StandardColorGetARGB32(StandardColor::DarkCyan);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkRed")))
	{
		return StandardColorGetARGB32(StandardColor::DarkRed);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkMagenta")))
	{
		return StandardColorGetARGB32(StandardColor::DarkMagenta);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkYellow")))
	{
		return StandardColorGetARGB32(StandardColor::DarkYellow);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Gray")))
	{
		return StandardColorGetARGB32(StandardColor::Gray);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkGray")))
	{
		return StandardColorGetARGB32(StandardColor::DarkGray);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Blue")))
	{
		return StandardColorGetARGB32(StandardColor::Blue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Lime")))
	{
		return StandardColorGetARGB32(StandardColor::Lime);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Cyan")))
	{
		return StandardColorGetARGB32(StandardColor::Cyan);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Red")))
	{
		return StandardColorGetARGB32(StandardColor::Red);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Magenta")))
	{
		return StandardColorGetARGB32(StandardColor::Magenta);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Yellow")))
	{
		return StandardColorGetARGB32(StandardColor::Yellow);
	}
	else if (colorStr.EqualsICase(UTF8STRC("White")))
	{
		return StandardColorGetARGB32(StandardColor::White);
	}
	else if (colorStr.EqualsICase(UTF8STRC("IndianRed")))
	{
		return StandardColorGetARGB32(StandardColor::IndianRed);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LightCoral")))
	{
		return StandardColorGetARGB32(StandardColor::LightCoral);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Salmon")))
	{
		return StandardColorGetARGB32(StandardColor::Salmon);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkSalmon")))
	{
		return StandardColorGetARGB32(StandardColor::DarkSalmon);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LightSalmon")))
	{
		return StandardColorGetARGB32(StandardColor::LightSalmon);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Crimson")))
	{
		return StandardColorGetARGB32(StandardColor::Crimson);
	}
	else if (colorStr.EqualsICase(UTF8STRC("FireBrick")))
	{
		return StandardColorGetARGB32(StandardColor::FireBrick);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Pink")))
	{
		return StandardColorGetARGB32(StandardColor::Pink);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LightPink")))
	{
		return StandardColorGetARGB32(StandardColor::LightPink);
	}
	else if (colorStr.EqualsICase(UTF8STRC("HotPink")))
	{
		return StandardColorGetARGB32(StandardColor::HotPink);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DeepPink")))
	{
		return StandardColorGetARGB32(StandardColor::DeepPink);
	}
	else if (colorStr.EqualsICase(UTF8STRC("MediumVioletRed")))
	{
		return StandardColorGetARGB32(StandardColor::MediumVioletRed);
	}
	else if (colorStr.EqualsICase(UTF8STRC("PaleVioletRed")))
	{
		return StandardColorGetARGB32(StandardColor::PaleVioletRed);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Coral")))
	{
		return StandardColorGetARGB32(StandardColor::Coral);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Tomato")))
	{
		return StandardColorGetARGB32(StandardColor::Tomato);
	}
	else if (colorStr.EqualsICase(UTF8STRC("OrangeRed")))
	{
		return StandardColorGetARGB32(StandardColor::OrangeRed);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkOrange")))
	{
		return StandardColorGetARGB32(StandardColor::DarkOrange);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Orange")))
	{
		return StandardColorGetARGB32(StandardColor::Orange);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Gold")))
	{
		return StandardColorGetARGB32(StandardColor::Gold);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LightYellow")))
	{
		return StandardColorGetARGB32(StandardColor::LightYellow);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LemonChiffon")))
	{
		return StandardColorGetARGB32(StandardColor::LemonChiffon);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LightGoldenrodYellow")))
	{
		return StandardColorGetARGB32(StandardColor::LightGoldenrodYellow);
	}
	else if (colorStr.EqualsICase(UTF8STRC("PapayaWhip")))
	{
		return StandardColorGetARGB32(StandardColor::PapayaWhip);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Moccasin")))
	{
		return StandardColorGetARGB32(StandardColor::Moccasin);
	}
	else if (colorStr.EqualsICase(UTF8STRC("PeachPuff")))
	{
		return StandardColorGetARGB32(StandardColor::PeachPuff);
	}
	else if (colorStr.EqualsICase(UTF8STRC("PaleGoldenrod")))
	{
		return StandardColorGetARGB32(StandardColor::PaleGoldenrod);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Khaki")))
	{
		return StandardColorGetARGB32(StandardColor::Khaki);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkKhaki")))
	{
		return StandardColorGetARGB32(StandardColor::DarkKhaki);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Lavender")))
	{
		return StandardColorGetARGB32(StandardColor::Lavender);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Thistle")))
	{
		return StandardColorGetARGB32(StandardColor::Thistle);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Plum")))
	{
		return StandardColorGetARGB32(StandardColor::Plum);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Violet")))
	{
		return StandardColorGetARGB32(StandardColor::Violet);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Orchid")))
	{
		return StandardColorGetARGB32(StandardColor::Orchid);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Fuchsia")))
	{
		return StandardColorGetARGB32(StandardColor::Fuchsia);
	}
	else if (colorStr.EqualsICase(UTF8STRC("MediumOrchid")))
	{
		return StandardColorGetARGB32(StandardColor::MediumOrchid);
	}
	else if (colorStr.EqualsICase(UTF8STRC("MediumPurple")))
	{
		return StandardColorGetARGB32(StandardColor::MediumPurple);
	}
	else if (colorStr.EqualsICase(UTF8STRC("RebeccaPurple")))
	{
		return StandardColorGetARGB32(StandardColor::RebeccaPurple);
	}
	else if (colorStr.EqualsICase(UTF8STRC("BlueViolet")))
	{
		return StandardColorGetARGB32(StandardColor::BlueViolet);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkViolet")))
	{
		return StandardColorGetARGB32(StandardColor::DarkViolet);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkOrchid")))
	{
		return StandardColorGetARGB32(StandardColor::DarkOrchid);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Purple")))
	{
		return StandardColorGetARGB32(StandardColor::Purple);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Indigo")))
	{
		return StandardColorGetARGB32(StandardColor::Indigo);
	}
	else if (colorStr.EqualsICase(UTF8STRC("SlateBlue")))
	{
		return StandardColorGetARGB32(StandardColor::SlateBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkSlateBlue")))
	{
		return StandardColorGetARGB32(StandardColor::DarkSlateBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("MediumSlateBlue")))
	{
		return StandardColorGetARGB32(StandardColor::MediumSlateBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("GreenYellow")))
	{
		return StandardColorGetARGB32(StandardColor::GreenYellow);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Chartreuse")))
	{
		return StandardColorGetARGB32(StandardColor::Chartreuse);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LawnGreen")))
	{
		return StandardColorGetARGB32(StandardColor::LawnGreen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LimeGreen")))
	{
		return StandardColorGetARGB32(StandardColor::LimeGreen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("PaleGreen")))
	{
		return StandardColorGetARGB32(StandardColor::PaleGreen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LightGreen")))
	{
		return StandardColorGetARGB32(StandardColor::LightGreen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("MediumSpringGreen")))
	{
		return StandardColorGetARGB32(StandardColor::MediumSpringGreen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("SpringGreen")))
	{
		return StandardColorGetARGB32(StandardColor::SpringGreen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("MediumSeaGreen")))
	{
		return StandardColorGetARGB32(StandardColor::MediumSeaGreen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("SeaGreen")))
	{
		return StandardColorGetARGB32(StandardColor::SeaGreen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("ForestGreen")))
	{
		return StandardColorGetARGB32(StandardColor::ForestGreen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Green")))
	{
		return StandardColorGetARGB32(StandardColor::Green);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkGreen")))
	{
		return StandardColorGetARGB32(StandardColor::DarkGreen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("YellowGreen")))
	{
		return StandardColorGetARGB32(StandardColor::YellowGreen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("OliveDrab")))
	{
		return StandardColorGetARGB32(StandardColor::OliveDrab);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Olive")))
	{
		return StandardColorGetARGB32(StandardColor::Olive);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkOliveGreen")))
	{
		return StandardColorGetARGB32(StandardColor::DarkOliveGreen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("MediumAquamarine")))
	{
		return StandardColorGetARGB32(StandardColor::MediumAquamarine);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkSeaGreen")))
	{
		return StandardColorGetARGB32(StandardColor::DarkSeaGreen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LightSeaGreen")))
	{
		return StandardColorGetARGB32(StandardColor::LightSeaGreen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Teal")))
	{
		return StandardColorGetARGB32(StandardColor::Teal);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Aqua")))
	{
		return StandardColorGetARGB32(StandardColor::Aqua);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LightCyan")))
	{
		return StandardColorGetARGB32(StandardColor::LightCyan);
	}
	else if (colorStr.EqualsICase(UTF8STRC("PaleTurquoise")))
	{
		return StandardColorGetARGB32(StandardColor::PaleTurquoise);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Aquamarine")))
	{
		return StandardColorGetARGB32(StandardColor::Aquamarine);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Turquoise")))
	{
		return StandardColorGetARGB32(StandardColor::Turquoise);
	}
	else if (colorStr.EqualsICase(UTF8STRC("MediumTurquoise")))
	{
		return StandardColorGetARGB32(StandardColor::MediumTurquoise);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkTurquoise")))
	{
		return StandardColorGetARGB32(StandardColor::DarkTurquoise);
	}
	else if (colorStr.EqualsICase(UTF8STRC("CadetBlue")))
	{
		return StandardColorGetARGB32(StandardColor::CadetBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("SteelBlue")))
	{
		return StandardColorGetARGB32(StandardColor::SteelBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LightSteelBlue")))
	{
		return StandardColorGetARGB32(StandardColor::LightSteelBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("PowderBlue")))
	{
		return StandardColorGetARGB32(StandardColor::PowderBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LightBlue")))
	{
		return StandardColorGetARGB32(StandardColor::LightBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("SkyBlue")))
	{
		return StandardColorGetARGB32(StandardColor::SkyBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LightSkyBlue")))
	{
		return StandardColorGetARGB32(StandardColor::LightSkyBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DeepSkyBlue")))
	{
		return StandardColorGetARGB32(StandardColor::DeepSkyBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DodgerBlue")))
	{
		return StandardColorGetARGB32(StandardColor::DodgerBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("CornflowerBlue")))
	{
		return StandardColorGetARGB32(StandardColor::CornflowerBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("RoyalBlue")))
	{
		return StandardColorGetARGB32(StandardColor::RoyalBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("MediumBlue")))
	{
		return StandardColorGetARGB32(StandardColor::MediumBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Navy")))
	{
		return StandardColorGetARGB32(StandardColor::Navy);
	}
	else if (colorStr.EqualsICase(UTF8STRC("MidnightBlue")))
	{
		return StandardColorGetARGB32(StandardColor::MidnightBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Cornsilk")))
	{
		return StandardColorGetARGB32(StandardColor::Cornsilk);
	}
	else if (colorStr.EqualsICase(UTF8STRC("BlanchedAlmond")))
	{
		return StandardColorGetARGB32(StandardColor::BlanchedAlmond);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Bisque")))
	{
		return StandardColorGetARGB32(StandardColor::Bisque);
	}
	else if (colorStr.EqualsICase(UTF8STRC("NavajoWhite")))
	{
		return StandardColorGetARGB32(StandardColor::NavajoWhite);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Wheat")))
	{
		return StandardColorGetARGB32(StandardColor::Wheat);
	}
	else if (colorStr.EqualsICase(UTF8STRC("BurlyWood")))
	{
		return StandardColorGetARGB32(StandardColor::BurlyWood);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Tan")))
	{
		return StandardColorGetARGB32(StandardColor::Tan);
	}
	else if (colorStr.EqualsICase(UTF8STRC("RosyBrown")))
	{
		return StandardColorGetARGB32(StandardColor::RosyBrown);
	}
	else if (colorStr.EqualsICase(UTF8STRC("SandyBrown")))
	{
		return StandardColorGetARGB32(StandardColor::SandyBrown);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Goldenrod")))
	{
		return StandardColorGetARGB32(StandardColor::Goldenrod);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkGoldenrod")))
	{
		return StandardColorGetARGB32(StandardColor::DarkGoldenrod);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Peru")))
	{
		return StandardColorGetARGB32(StandardColor::Peru);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Chocolate")))
	{
		return StandardColorGetARGB32(StandardColor::Chocolate);
	}
	else if (colorStr.EqualsICase(UTF8STRC("SaddleBrown")))
	{
		return StandardColorGetARGB32(StandardColor::SaddleBrown);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Sienna")))
	{
		return StandardColorGetARGB32(StandardColor::Sienna);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Brown")))
	{
		return StandardColorGetARGB32(StandardColor::Brown);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Maroon")))
	{
		return StandardColorGetARGB32(StandardColor::Maroon);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Snow")))
	{
		return StandardColorGetARGB32(StandardColor::Snow);
	}
	else if (colorStr.EqualsICase(UTF8STRC("HoneyDew")))
	{
		return StandardColorGetARGB32(StandardColor::HoneyDew);
	}
	else if (colorStr.EqualsICase(UTF8STRC("MintCream")))
	{
		return StandardColorGetARGB32(StandardColor::MintCream);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Azure")))
	{
		return StandardColorGetARGB32(StandardColor::Azure);
	}
	else if (colorStr.EqualsICase(UTF8STRC("AliceBlue")))
	{
		return StandardColorGetARGB32(StandardColor::AliceBlue);
	}
	else if (colorStr.EqualsICase(UTF8STRC("GhostWhite")))
	{
		return StandardColorGetARGB32(StandardColor::GhostWhite);
	}
	else if (colorStr.EqualsICase(UTF8STRC("WhiteSmoke")))
	{
		return StandardColorGetARGB32(StandardColor::WhiteSmoke);
	}
	else if (colorStr.EqualsICase(UTF8STRC("SeaShell")))
	{
		return StandardColorGetARGB32(StandardColor::SeaShell);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Beige")))
	{
		return StandardColorGetARGB32(StandardColor::Beige);
	}
	else if (colorStr.EqualsICase(UTF8STRC("OldLace")))
	{
		return StandardColorGetARGB32(StandardColor::OldLace);
	}
	else if (colorStr.EqualsICase(UTF8STRC("FloralWhite")))
	{
		return StandardColorGetARGB32(StandardColor::FloralWhite);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Ivory")))
	{
		return StandardColorGetARGB32(StandardColor::Ivory);
	}
	else if (colorStr.EqualsICase(UTF8STRC("AntiqueWhite")))
	{
		return StandardColorGetARGB32(StandardColor::AntiqueWhite);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Linen")))
	{
		return StandardColorGetARGB32(StandardColor::Linen);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LavenderBlush")))
	{
		return StandardColorGetARGB32(StandardColor::LavenderBlush);
	}
	else if (colorStr.EqualsICase(UTF8STRC("MistyRose")))
	{
		return StandardColorGetARGB32(StandardColor::MistyRose);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Gainsboro")))
	{
		return StandardColorGetARGB32(StandardColor::Gainsboro);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LightGray")))
	{
		return StandardColorGetARGB32(StandardColor::LightGray);
	}
	else if (colorStr.EqualsICase(UTF8STRC("Silver")))
	{
		return StandardColorGetARGB32(StandardColor::Silver);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DimGray")))
	{
		return StandardColorGetARGB32(StandardColor::DimGray);
	}
	else if (colorStr.EqualsICase(UTF8STRC("LightSlateGray")))
	{
		return StandardColorGetARGB32(StandardColor::LightSlateGray);
	}
	else if (colorStr.EqualsICase(UTF8STRC("SlateGray")))
	{
		return StandardColorGetARGB32(StandardColor::SlateGray);
	}
	else if (colorStr.EqualsICase(UTF8STRC("DarkSlateGray")))
	{
		return StandardColorGetARGB32(StandardColor::DarkSlateGray);
	}
	else
	{
		printf("CSSColor: Unknown color name: %s\r\n", colorStr.v.Ptr());
		return 0;
	}
}