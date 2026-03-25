#include "Stdafx.h"
#include "Text/CSSCore.h"
#include "Text/StandardColor.h"
#include "Text/StringBuilderUTF8.h"

UInt32 Text::CSSCore::ParseColor(Text::CStringNN colorStr)
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
	if (colorStr.StartsWith(UTF8STRC("rgb(")) && colorStr.EndsWith(')'))
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
	if (colorStr.StartsWith(UTF8STRC("rgba(")) && colorStr.EndsWith(')'))
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
	if (colorStr.StartsWith(UTF8STRC("hsl(")) && colorStr.EndsWith(')'))
	{
		printf("CSSColor: hsl() color not supported: %s\r\n", colorStr.v.Ptr());
		return 0;
	}
	if (colorStr.StartsWith(UTF8STRC("hsla(")) && colorStr.EndsWith(')'))
	{
		printf("CSSColor: hsla() color not supported: %s\r\n", colorStr.v.Ptr());
		return 0;
	}
	if (colorStr.EqualsICase(UTF8STRC("transparent")))
	{
		return 0;
	}
	if (colorStr.EqualsICase(UTF8STRC("none")))
	{
		return 0;
	}
	if (colorStr.EqualsICase(UTF8STRC("Black")))
	{
		return StandardColorGetARGB32(StandardColor::Black);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkBlue")))
	{
		return StandardColorGetARGB32(StandardColor::DarkBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("ConsoleDarkGreen")))
	{
		return StandardColorGetARGB32(StandardColor::ConsoleDarkGreen);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkCyan")))
	{
		return StandardColorGetARGB32(StandardColor::DarkCyan);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkRed")))
	{
		return StandardColorGetARGB32(StandardColor::DarkRed);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkMagenta")))
	{
		return StandardColorGetARGB32(StandardColor::DarkMagenta);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkYellow")))
	{
		return StandardColorGetARGB32(StandardColor::DarkYellow);
	}
	if (colorStr.EqualsICase(UTF8STRC("Gray")))
	{
		return StandardColorGetARGB32(StandardColor::Gray);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkGray")))
	{
		return StandardColorGetARGB32(StandardColor::DarkGray);
	}
	if (colorStr.EqualsICase(UTF8STRC("Blue")))
	{
		return StandardColorGetARGB32(StandardColor::Blue);
	}
	if (colorStr.EqualsICase(UTF8STRC("Lime")))
	{
		return StandardColorGetARGB32(StandardColor::Lime);
	}
	if (colorStr.EqualsICase(UTF8STRC("Cyan")))
	{
		return StandardColorGetARGB32(StandardColor::Cyan);
	}
	if (colorStr.EqualsICase(UTF8STRC("Red")))
	{
		return StandardColorGetARGB32(StandardColor::Red);
	}
	if (colorStr.EqualsICase(UTF8STRC("Magenta")))
	{
		return StandardColorGetARGB32(StandardColor::Magenta);
	}
	if (colorStr.EqualsICase(UTF8STRC("Yellow")))
	{
		return StandardColorGetARGB32(StandardColor::Yellow);
	}
	if (colorStr.EqualsICase(UTF8STRC("White")))
	{
		return StandardColorGetARGB32(StandardColor::White);
	}
	if (colorStr.EqualsICase(UTF8STRC("IndianRed")))
	{
		return StandardColorGetARGB32(StandardColor::IndianRed);
	}
	if (colorStr.EqualsICase(UTF8STRC("LightCoral")))
	{
		return StandardColorGetARGB32(StandardColor::LightCoral);
	}
	if (colorStr.EqualsICase(UTF8STRC("Salmon")))
	{
		return StandardColorGetARGB32(StandardColor::Salmon);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkSalmon")))
	{
		return StandardColorGetARGB32(StandardColor::DarkSalmon);
	}
	if (colorStr.EqualsICase(UTF8STRC("LightSalmon")))
	{
		return StandardColorGetARGB32(StandardColor::LightSalmon);
	}
	if (colorStr.EqualsICase(UTF8STRC("Crimson")))
	{
		return StandardColorGetARGB32(StandardColor::Crimson);
	}
	if (colorStr.EqualsICase(UTF8STRC("FireBrick")))
	{
		return StandardColorGetARGB32(StandardColor::FireBrick);
	}
	if (colorStr.EqualsICase(UTF8STRC("Pink")))
	{
		return StandardColorGetARGB32(StandardColor::Pink);
	}
	if (colorStr.EqualsICase(UTF8STRC("LightPink")))
	{
		return StandardColorGetARGB32(StandardColor::LightPink);
	}
	if (colorStr.EqualsICase(UTF8STRC("HotPink")))
	{
		return StandardColorGetARGB32(StandardColor::HotPink);
	}
	if (colorStr.EqualsICase(UTF8STRC("DeepPink")))
	{
		return StandardColorGetARGB32(StandardColor::DeepPink);
	}
	if (colorStr.EqualsICase(UTF8STRC("MediumVioletRed")))
	{
		return StandardColorGetARGB32(StandardColor::MediumVioletRed);
	}
	if (colorStr.EqualsICase(UTF8STRC("PaleVioletRed")))
	{
		return StandardColorGetARGB32(StandardColor::PaleVioletRed);
	}
	if (colorStr.EqualsICase(UTF8STRC("Coral")))
	{
		return StandardColorGetARGB32(StandardColor::Coral);
	}
	if (colorStr.EqualsICase(UTF8STRC("Tomato")))
	{
		return StandardColorGetARGB32(StandardColor::Tomato);
	}
	if (colorStr.EqualsICase(UTF8STRC("OrangeRed")))
	{
		return StandardColorGetARGB32(StandardColor::OrangeRed);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkOrange")))
	{
		return StandardColorGetARGB32(StandardColor::DarkOrange);
	}
	if (colorStr.EqualsICase(UTF8STRC("Orange")))
	{
		return StandardColorGetARGB32(StandardColor::Orange);
	}
	if (colorStr.EqualsICase(UTF8STRC("Gold")))
	{
		return StandardColorGetARGB32(StandardColor::Gold);
	}
	if (colorStr.EqualsICase(UTF8STRC("LightYellow")))
	{
		return StandardColorGetARGB32(StandardColor::LightYellow);
	}
	if (colorStr.EqualsICase(UTF8STRC("LemonChiffon")))
	{
		return StandardColorGetARGB32(StandardColor::LemonChiffon);
	}
	if (colorStr.EqualsICase(UTF8STRC("LightGoldenrodYellow")))
	{
		return StandardColorGetARGB32(StandardColor::LightGoldenrodYellow);
	}
	if (colorStr.EqualsICase(UTF8STRC("PapayaWhip")))
	{
		return StandardColorGetARGB32(StandardColor::PapayaWhip);
	}
	if (colorStr.EqualsICase(UTF8STRC("Moccasin")))
	{
		return StandardColorGetARGB32(StandardColor::Moccasin);
	}
	if (colorStr.EqualsICase(UTF8STRC("PeachPuff")))
	{
		return StandardColorGetARGB32(StandardColor::PeachPuff);
	}
	if (colorStr.EqualsICase(UTF8STRC("PaleGoldenrod")))
	{
		return StandardColorGetARGB32(StandardColor::PaleGoldenrod);
	}
	if (colorStr.EqualsICase(UTF8STRC("Khaki")))
	{
		return StandardColorGetARGB32(StandardColor::Khaki);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkKhaki")))
	{
		return StandardColorGetARGB32(StandardColor::DarkKhaki);
	}
	if (colorStr.EqualsICase(UTF8STRC("Lavender")))
	{
		return StandardColorGetARGB32(StandardColor::Lavender);
	}
	if (colorStr.EqualsICase(UTF8STRC("Thistle")))
	{
		return StandardColorGetARGB32(StandardColor::Thistle);
	}
	if (colorStr.EqualsICase(UTF8STRC("Plum")))
	{
		return StandardColorGetARGB32(StandardColor::Plum);
	}
	if (colorStr.EqualsICase(UTF8STRC("Violet")))
	{
		return StandardColorGetARGB32(StandardColor::Violet);
	}
	if (colorStr.EqualsICase(UTF8STRC("Orchid")))
	{
		return StandardColorGetARGB32(StandardColor::Orchid);
	}
	if (colorStr.EqualsICase(UTF8STRC("Fuchsia")))
	{
		return StandardColorGetARGB32(StandardColor::Fuchsia);
	}
	if (colorStr.EqualsICase(UTF8STRC("MediumOrchid")))
	{
		return StandardColorGetARGB32(StandardColor::MediumOrchid);
	}
	if (colorStr.EqualsICase(UTF8STRC("MediumPurple")))
	{
		return StandardColorGetARGB32(StandardColor::MediumPurple);
	}
	if (colorStr.EqualsICase(UTF8STRC("RebeccaPurple")))
	{
		return StandardColorGetARGB32(StandardColor::RebeccaPurple);
	}
	if (colorStr.EqualsICase(UTF8STRC("BlueViolet")))
	{
		return StandardColorGetARGB32(StandardColor::BlueViolet);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkViolet")))
	{
		return StandardColorGetARGB32(StandardColor::DarkViolet);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkOrchid")))
	{
		return StandardColorGetARGB32(StandardColor::DarkOrchid);
	}
	if (colorStr.EqualsICase(UTF8STRC("Purple")))
	{
		return StandardColorGetARGB32(StandardColor::Purple);
	}
	if (colorStr.EqualsICase(UTF8STRC("Indigo")))
	{
		return StandardColorGetARGB32(StandardColor::Indigo);
	}
	if (colorStr.EqualsICase(UTF8STRC("SlateBlue")))
	{
		return StandardColorGetARGB32(StandardColor::SlateBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkSlateBlue")))
	{
		return StandardColorGetARGB32(StandardColor::DarkSlateBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("MediumSlateBlue")))
	{
		return StandardColorGetARGB32(StandardColor::MediumSlateBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("GreenYellow")))
	{
		return StandardColorGetARGB32(StandardColor::GreenYellow);
	}
	if (colorStr.EqualsICase(UTF8STRC("Chartreuse")))
	{
		return StandardColorGetARGB32(StandardColor::Chartreuse);
	}
	if (colorStr.EqualsICase(UTF8STRC("LawnGreen")))
	{
		return StandardColorGetARGB32(StandardColor::LawnGreen);
	}
	if (colorStr.EqualsICase(UTF8STRC("LimeGreen")))
	{
		return StandardColorGetARGB32(StandardColor::LimeGreen);
	}
	if (colorStr.EqualsICase(UTF8STRC("PaleGreen")))
	{
		return StandardColorGetARGB32(StandardColor::PaleGreen);
	}
	if (colorStr.EqualsICase(UTF8STRC("LightGreen")))
	{
		return StandardColorGetARGB32(StandardColor::LightGreen);
	}
	if (colorStr.EqualsICase(UTF8STRC("MediumSpringGreen")))
	{
		return StandardColorGetARGB32(StandardColor::MediumSpringGreen);
	}
	if (colorStr.EqualsICase(UTF8STRC("SpringGreen")))
	{
		return StandardColorGetARGB32(StandardColor::SpringGreen);
	}
	if (colorStr.EqualsICase(UTF8STRC("MediumSeaGreen")))
	{
		return StandardColorGetARGB32(StandardColor::MediumSeaGreen);
	}
	if (colorStr.EqualsICase(UTF8STRC("SeaGreen")))
	{
		return StandardColorGetARGB32(StandardColor::SeaGreen);
	}
	if (colorStr.EqualsICase(UTF8STRC("ForestGreen")))
	{
		return StandardColorGetARGB32(StandardColor::ForestGreen);
	}
	if (colorStr.EqualsICase(UTF8STRC("Green")))
	{
		return StandardColorGetARGB32(StandardColor::Green);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkGreen")))
	{
		return StandardColorGetARGB32(StandardColor::DarkGreen);
	}
	if (colorStr.EqualsICase(UTF8STRC("YellowGreen")))
	{
		return StandardColorGetARGB32(StandardColor::YellowGreen);
	}
	if (colorStr.EqualsICase(UTF8STRC("OliveDrab")))
	{
		return StandardColorGetARGB32(StandardColor::OliveDrab);
	}
	if (colorStr.EqualsICase(UTF8STRC("Olive")))
	{
		return StandardColorGetARGB32(StandardColor::Olive);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkOliveGreen")))
	{
		return StandardColorGetARGB32(StandardColor::DarkOliveGreen);
	}
	if (colorStr.EqualsICase(UTF8STRC("MediumAquamarine")))
	{
		return StandardColorGetARGB32(StandardColor::MediumAquamarine);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkSeaGreen")))
	{
		return StandardColorGetARGB32(StandardColor::DarkSeaGreen);
	}
	if (colorStr.EqualsICase(UTF8STRC("LightSeaGreen")))
	{
		return StandardColorGetARGB32(StandardColor::LightSeaGreen);
	}
	if (colorStr.EqualsICase(UTF8STRC("Teal")))
	{
		return StandardColorGetARGB32(StandardColor::Teal);
	}
	if (colorStr.EqualsICase(UTF8STRC("Aqua")))
	{
		return StandardColorGetARGB32(StandardColor::Aqua);
	}
	if (colorStr.EqualsICase(UTF8STRC("LightCyan")))
	{
		return StandardColorGetARGB32(StandardColor::LightCyan);
	}
	if (colorStr.EqualsICase(UTF8STRC("PaleTurquoise")))
	{
		return StandardColorGetARGB32(StandardColor::PaleTurquoise);
	}
	if (colorStr.EqualsICase(UTF8STRC("Aquamarine")))
	{
		return StandardColorGetARGB32(StandardColor::Aquamarine);
	}
	if (colorStr.EqualsICase(UTF8STRC("Turquoise")))
	{
		return StandardColorGetARGB32(StandardColor::Turquoise);
	}
	if (colorStr.EqualsICase(UTF8STRC("MediumTurquoise")))
	{
		return StandardColorGetARGB32(StandardColor::MediumTurquoise);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkTurquoise")))
	{
		return StandardColorGetARGB32(StandardColor::DarkTurquoise);
	}
	if (colorStr.EqualsICase(UTF8STRC("CadetBlue")))
	{
		return StandardColorGetARGB32(StandardColor::CadetBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("SteelBlue")))
	{
		return StandardColorGetARGB32(StandardColor::SteelBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("LightSteelBlue")))
	{
		return StandardColorGetARGB32(StandardColor::LightSteelBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("PowderBlue")))
	{
		return StandardColorGetARGB32(StandardColor::PowderBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("LightBlue")))
	{
		return StandardColorGetARGB32(StandardColor::LightBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("SkyBlue")))
	{
		return StandardColorGetARGB32(StandardColor::SkyBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("LightSkyBlue")))
	{
		return StandardColorGetARGB32(StandardColor::LightSkyBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("DeepSkyBlue")))
	{
		return StandardColorGetARGB32(StandardColor::DeepSkyBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("DodgerBlue")))
	{
		return StandardColorGetARGB32(StandardColor::DodgerBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("CornflowerBlue")))
	{
		return StandardColorGetARGB32(StandardColor::CornflowerBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("RoyalBlue")))
	{
		return StandardColorGetARGB32(StandardColor::RoyalBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("MediumBlue")))
	{
		return StandardColorGetARGB32(StandardColor::MediumBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("Navy")))
	{
		return StandardColorGetARGB32(StandardColor::Navy);
	}
	if (colorStr.EqualsICase(UTF8STRC("MidnightBlue")))
	{
		return StandardColorGetARGB32(StandardColor::MidnightBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("Cornsilk")))
	{
		return StandardColorGetARGB32(StandardColor::Cornsilk);
	}
	if (colorStr.EqualsICase(UTF8STRC("BlanchedAlmond")))
	{
		return StandardColorGetARGB32(StandardColor::BlanchedAlmond);
	}
	if (colorStr.EqualsICase(UTF8STRC("Bisque")))
	{
		return StandardColorGetARGB32(StandardColor::Bisque);
	}
	if (colorStr.EqualsICase(UTF8STRC("NavajoWhite")))
	{
		return StandardColorGetARGB32(StandardColor::NavajoWhite);
	}
	if (colorStr.EqualsICase(UTF8STRC("Wheat")))
	{
		return StandardColorGetARGB32(StandardColor::Wheat);
	}
	if (colorStr.EqualsICase(UTF8STRC("BurlyWood")))
	{
		return StandardColorGetARGB32(StandardColor::BurlyWood);
	}
	if (colorStr.EqualsICase(UTF8STRC("Tan")))
	{
		return StandardColorGetARGB32(StandardColor::Tan);
	}
	if (colorStr.EqualsICase(UTF8STRC("RosyBrown")))
	{
		return StandardColorGetARGB32(StandardColor::RosyBrown);
	}
	if (colorStr.EqualsICase(UTF8STRC("SandyBrown")))
	{
		return StandardColorGetARGB32(StandardColor::SandyBrown);
	}
	if (colorStr.EqualsICase(UTF8STRC("Goldenrod")))
	{
		return StandardColorGetARGB32(StandardColor::Goldenrod);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkGoldenrod")))
	{
		return StandardColorGetARGB32(StandardColor::DarkGoldenrod);
	}
	if (colorStr.EqualsICase(UTF8STRC("Peru")))
	{
		return StandardColorGetARGB32(StandardColor::Peru);
	}
	if (colorStr.EqualsICase(UTF8STRC("Chocolate")))
	{
		return StandardColorGetARGB32(StandardColor::Chocolate);
	}
	if (colorStr.EqualsICase(UTF8STRC("SaddleBrown")))
	{
		return StandardColorGetARGB32(StandardColor::SaddleBrown);
	}
	if (colorStr.EqualsICase(UTF8STRC("Sienna")))
	{
		return StandardColorGetARGB32(StandardColor::Sienna);
	}
	if (colorStr.EqualsICase(UTF8STRC("Brown")))
	{
		return StandardColorGetARGB32(StandardColor::Brown);
	}
	if (colorStr.EqualsICase(UTF8STRC("Maroon")))
	{
		return StandardColorGetARGB32(StandardColor::Maroon);
	}
	if (colorStr.EqualsICase(UTF8STRC("Snow")))
	{
		return StandardColorGetARGB32(StandardColor::Snow);
	}
	if (colorStr.EqualsICase(UTF8STRC("HoneyDew")))
	{
		return StandardColorGetARGB32(StandardColor::HoneyDew);
	}
	if (colorStr.EqualsICase(UTF8STRC("MintCream")))
	{
		return StandardColorGetARGB32(StandardColor::MintCream);
	}
	if (colorStr.EqualsICase(UTF8STRC("Azure")))
	{
		return StandardColorGetARGB32(StandardColor::Azure);
	}
	if (colorStr.EqualsICase(UTF8STRC("AliceBlue")))
	{
		return StandardColorGetARGB32(StandardColor::AliceBlue);
	}
	if (colorStr.EqualsICase(UTF8STRC("GhostWhite")))
	{
		return StandardColorGetARGB32(StandardColor::GhostWhite);
	}
	if (colorStr.EqualsICase(UTF8STRC("WhiteSmoke")))
	{
		return StandardColorGetARGB32(StandardColor::WhiteSmoke);
	}
	if (colorStr.EqualsICase(UTF8STRC("SeaShell")))
	{
		return StandardColorGetARGB32(StandardColor::SeaShell);
	}
	if (colorStr.EqualsICase(UTF8STRC("Beige")))
	{
		return StandardColorGetARGB32(StandardColor::Beige);
	}
	if (colorStr.EqualsICase(UTF8STRC("OldLace")))
	{
		return StandardColorGetARGB32(StandardColor::OldLace);
	}
	if (colorStr.EqualsICase(UTF8STRC("FloralWhite")))
	{
		return StandardColorGetARGB32(StandardColor::FloralWhite);
	}
	if (colorStr.EqualsICase(UTF8STRC("Ivory")))
	{
		return StandardColorGetARGB32(StandardColor::Ivory);
	}
	if (colorStr.EqualsICase(UTF8STRC("AntiqueWhite")))
	{
		return StandardColorGetARGB32(StandardColor::AntiqueWhite);
	}
	if (colorStr.EqualsICase(UTF8STRC("Linen")))
	{
		return StandardColorGetARGB32(StandardColor::Linen);
	}
	if (colorStr.EqualsICase(UTF8STRC("LavenderBlush")))
	{
		return StandardColorGetARGB32(StandardColor::LavenderBlush);
	}
	if (colorStr.EqualsICase(UTF8STRC("MistyRose")))
	{
		return StandardColorGetARGB32(StandardColor::MistyRose);
	}
	if (colorStr.EqualsICase(UTF8STRC("Gainsboro")))
	{
		return StandardColorGetARGB32(StandardColor::Gainsboro);
	}
	if (colorStr.EqualsICase(UTF8STRC("LightGray")))
	{
		return StandardColorGetARGB32(StandardColor::LightGray);
	}
	if (colorStr.EqualsICase(UTF8STRC("Silver")))
	{
		return StandardColorGetARGB32(StandardColor::Silver);
	}
	if (colorStr.EqualsICase(UTF8STRC("DimGray")))
	{
		return StandardColorGetARGB32(StandardColor::DimGray);
	}
	if (colorStr.EqualsICase(UTF8STRC("LightSlateGray")))
	{
		return StandardColorGetARGB32(StandardColor::LightSlateGray);
	}
	if (colorStr.EqualsICase(UTF8STRC("SlateGray")))
	{
		return StandardColorGetARGB32(StandardColor::SlateGray);
	}
	if (colorStr.EqualsICase(UTF8STRC("DarkSlateGray")))
	{
		return StandardColorGetARGB32(StandardColor::DarkSlateGray);
	}
	else
	{
		printf("CSSColor: Unknown color name: %s\r\n", colorStr.v.Ptr());
		return 0;
	}
}

UInt32 Text::CSSCore::ParseColor(Text::CStringNN colorStr, Double opacity)
{
	UInt32 v = ParseColor(colorStr);
	if (opacity < 0)
	{
		opacity = 0;
	}
	else if (opacity > 1)
	{
		opacity = 1;
	}
	return ((UInt32)Double2Int32(opacity * 255) << 24) | (v & 0x00ffffff);
}

Double Text::CSSCore::FontSizeToPx(Text::CStringNN fontSizeStr, Double parentFontSizePx)
{
	if (Math::IsNAN(parentFontSizePx) || parentFontSizePx <= 0)
	{
		parentFontSizePx = 16;
	}
	if (fontSizeStr.EqualsICase(UTF8STRC("xx-small")))
	{
		return 9;
	}
	else if (fontSizeStr.EqualsICase(UTF8STRC("x-small")))
	{
		return 10.5;
	}
	else if (fontSizeStr.EqualsICase(UTF8STRC("small")))
	{
		return 13.5;
	}
	else if (fontSizeStr.EqualsICase(UTF8STRC("medium")))
	{
		return 16;
	}
	else if (fontSizeStr.EqualsICase(UTF8STRC("large")))
	{
		return 18;
	}
	else if (fontSizeStr.EqualsICase(UTF8STRC("x-large")))
	{
		return 24;
	}
	else if (fontSizeStr.EqualsICase(UTF8STRC("xx-large")))
	{
		return 32;
	}
	else if (fontSizeStr.EqualsICase(UTF8STRC("xxx-large")))
	{
		return 48;
	}
	else if (fontSizeStr.EqualsICase(UTF8STRC("smaller")))
	{
		return parentFontSizePx * 0.8;
	}
	else if (fontSizeStr.EqualsICase(UTF8STRC("larger")))
	{
		return parentFontSizePx * 1.2;
	}
	else if (fontSizeStr.EndsWith(UTF8STRC("px")))
	{
		Double v;
		Text::StringBuilderUTF8 sb;
		sb.AppendC(fontSizeStr.v, fontSizeStr.leng - 2);
		if (sb.ToDouble(v))
		{
			return v;
		}
		else
		{
			printf("CSSCore: Invalid font size px: %s\r\n", fontSizeStr.v.Ptr());
			return parentFontSizePx;
		}
	}
	else if (fontSizeStr.EndsWith(UTF8STRC("rem")))
	{
		Double v;
		Text::StringBuilderUTF8 sb;
		sb.AppendC(fontSizeStr.v, fontSizeStr.leng - 3);
		if (sb.ToDouble(v))
		{
			return 16 * v;
		}
		else
		{
			printf("CSSCore: Invalid font size rem: %s\r\n", fontSizeStr.v.Ptr());
			return parentFontSizePx;
		}
	}
	else if (fontSizeStr.EndsWith(UTF8STRC("em")))
	{
		Double v;
		Text::StringBuilderUTF8 sb;
		sb.AppendC(fontSizeStr.v, fontSizeStr.leng - 2);
		if (sb.ToDouble(v))
		{
			return v * parentFontSizePx;
		}
		else
		{
			printf("CSSCore: Invalid font size em: %s\r\n", fontSizeStr.v.Ptr());
			return parentFontSizePx;
		}
	}
	else if (fontSizeStr.EndsWith(UTF8STRC("%")))
	{
		Double v;
		Text::StringBuilderUTF8 sb;
		sb.AppendC(fontSizeStr.v, fontSizeStr.leng - 1);
		if (sb.ToDouble(v))
		{
			return v * parentFontSizePx / 100.0;
		}
		else
		{
			printf("CSSCore: Invalid font size %%: %s\r\n", fontSizeStr.v.Ptr());
			return parentFontSizePx;
		}
	}
	else
	{
		Double v;
		if (fontSizeStr.ToDouble(v))
		{
			return v;
		}
		printf("CSSCore: Unknown font size unit: %s\r\n", fontSizeStr.v.Ptr());
		return parentFontSizePx;
	}
}
