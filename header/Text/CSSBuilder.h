#ifndef _SM_TEXT_CSSBUILDER
#define _SM_TEXT_CSSBUILDER
#include "Math/Unit/Distance.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class CSSBuilder
	{
	public:
		typedef enum
		{
			PM_COMPACT,
			PM_SPACE,
			PM_LINES
		} PrettyMode;

		typedef enum
		{
			FONT_WEIGHT_NORMAL = 400,
			FONT_WEIGHT_BOLD = 700,
			FONT_WEIGHT_BOLDER = 1001,
			FONT_WEIGHT_LIGHTER = -1,
			FONT_WEIGHT_INHERIT = -2,
			FONT_WEIGHT_INITIAL = -3
		} FontWeight;

	private:
		typedef enum
		{
			BS_ROOT,
			BS_ENTRY,
			BS_ENTRY_FIRST
		} BuildState;

		Text::StringBuilderUTF8 sb;
		PrettyMode pm;
		BuildState bstate;

	public:
		CSSBuilder(PrettyMode pm);
		~CSSBuilder();

		Bool NewStyle(Text::CString name, Text::CString className);
		Bool EndStyle();

		Bool AddColorRGBA(UInt32 argb);
		Bool AddColorRGB(UInt32 rgb);
		Bool AddBGColorRGBA(UInt32 argb);

		Bool AddFontFamily(const UTF8Char *family);
		Bool AddFontSize(Double size, Math::Unit::Distance::DistanceUnit du);
		Bool AddFontWeight(FontWeight weight);

		Bool AddMinHeight(Double h, Math::Unit::Distance::DistanceUnit du);

		const UTF8Char *ToString();

	private:
		void AppendNewLine();
		void AppendStyleName(Text::CString name);
		void AppendDistance(Double size, Math::Unit::Distance::DistanceUnit du);
		void AppendRGBAColor(UInt32 argb);
		void NextEntry();
	};
}
#endif
