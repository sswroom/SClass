#ifndef _SM_TEXT_SPREADSHEET_OFFICECOLOR
#define _SM_TEXT_SPREADSHEET_OFFICECOLOR

namespace Text
{
	namespace SpreadSheet
	{
		enum class ColorType
		{
			Preset,
			Argb
		};

// https://c-rex.net/projects/samples/ooxml/e1/Part4/OOXML_P4_DOCX_ST_PresetColorVal_topic_ID0ELA5NB.html#topic_ID0ELA5NB
		enum class PresetColor
		{
			AliceBlue,
			AntiqueWhite,
			Aqua,
			Aquamarine,
			Azure,
			Beige,
			Bisque,
			Black,
			BlanchedAlmond,
			Blue,
			BlueViolet,
			Brown,
			BurlyWood,
			CadetBlue,
			Chartreuse,
			Chocolate,
			Coral,
			CornflowerBlue,
			Cornsilk,
			Crimson,
			Cyan,
			DeepPink,
			DeepSkyBlue,
			DimGray,
			DarkBlue,
			DarkCyan,
			DarkGoldenrod,
			DarkGray,
			DarkGreen,
			DarkKhaki,
			DarkMagenta,
			DarkOliveGreen,
			DarkOrange,
			DarkOrchid,
			DarkRed,
			DarkSalmon,
			DarkSeaGreen,
			DarkSlateBlue,
			DarkSlateGray,
			DarkTurquoise,
			DarkViolet,
			DodgerBlue,
			Firebrick,
			FloralWhite,
			ForestGreen,
			Fuchsia,
			Gainsboro,
			GhostWhite,
			Gold,
			Goldenrod,
			Gray,
			Green,
			GreenYellow,
			Honeydew,
			HotPink,
			IndianRed,
			Indigo,
			Ivory,
			Khaki,
			Lavender,
			LavenderBlush,
			LawnGreen,
			LemonChiffon,
			Lime,
			LimeGreen,
			Linen,
			LightBlue,
			LightCoral,
			LightCyan,
			LightGoldenrodYellow,
			LightGray,
			LightGreen,
			LightPink,
			LightSalmon,
			LightSeaGreen,
			LightSkyBlue,
			LightSlateGray,
			LightSteelBlue,
			LightYellow,
			Magenta,
			Maroon,
			MediumAquamarine,
			MediumBlue,
			MediumOrchid,
			MediumPurple,
			MediumSeaGreen,
			MediumSlateBlue,
			MediumSpringGreen,
			MediumTurquoise,
			MediumVioletRed,
			MidnightBlue,
			MintCream,
			MistyRose,
			Moccasin,
			NavajoWhite,
			Navy,
			OldLace,
			Olive,
			OliveDrab,
			Orange,
			OrangeRed,
			Orchid,
			PaleGoldenrod,
			PaleGreen,
			PaleTurquoise,
			PaleVioletRed,
			PapayaWhip,
			PeachPuff,
			Peru,
			Pink,
			Plum,
			PowderBlue,
			Purple,
			Red,
			RosyBrown,
			RoyalBlue,
			SaddleBrown,
			Salmon,
			SandyBrown,
			SeaGreen,
			SeaShell,
			Sienna,
			Silver,
			SkyBlue,
			SlateBlue,
			SlateGray,
			Snow,
			SpringGreen,
			SteelBlue,
			Tan,
			Teal,
			Thistle,
			Tomato,
			Turquoise,
			Violet,
			Wheat,
			White,
			WhiteSmoke,
			Yellow,
			YellowGreen
		};

		class OfficeColor
		{
		private:
			ColorType colorType;
			union 
			{
				PresetColor preset;
				UInt32 argb;
			} color;

			OfficeColor(PresetColor color);
			OfficeColor(UInt32 argb);
		public:
			~OfficeColor();

			ColorType GetColorType();
			PresetColor GetPresetColor();
			UInt32 GetColorArgb();

			static OfficeColor *NewPreset(PresetColor color);
			static OfficeColor *NewArgb(UInt32 argb);

			static UInt32 PresetColorGetArgb(PresetColor color);
		};
	}
}
#endif
