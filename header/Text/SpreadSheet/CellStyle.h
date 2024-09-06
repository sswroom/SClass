#ifndef _SM_TEXT_SPREADSHEET_CELLSTYLE
#define _SM_TEXT_SPREADSHEET_CELLSTYLE
#include "Text/String.h"
#include "Text/TextCommon.h"
#include "Text/SpreadSheet/WorkbookFont.h"
namespace Text
{
	namespace SpreadSheet
	{
		enum class BorderType
		{
			None,
			Thin,
			Medium,
			Dashed,
			Dotted,
			Thick,
			DOUBLE,
			Hair,
			MediumDashed,
			DashDot,
			MediumDashDot,
			DashDotDot,
			MediumDashDotDot,
			SlantedDashDot
		};

		class CellStyle
		{
		public:
			typedef enum
			{
				FP_NO_FILL,
				FP_SOLID_FOREGROUND,
				FP_FINE_DOT,
				FP_ALT_BARS,
				FP_SPARSE_DOTS,
				FP_THICK_HORZ_BANDS,
				FP_THICK_VERT_BANDS,
				FP_THICK_BACKWARD_DIAG,
				FP_THICK_FORWARD_DIAG,
				FP_BIG_SPOTS,
				FP_BRICKS,
				FP_THIN_HORZ_BANDS,
				FP_THIN_VERT_BANDS,
				FP_THIN_BACKWARD_DIAG,
				FP_THIN_FORWARD_DIAG,
				FP_SQUARES,
				FP_DIAMONDS,
				FP_LESS_DOTS,
				FP_LEAST_DOTS
			} FillPattern;

			struct BorderStyle
			{
				BorderType borderType;
				UInt32 borderColor;

				BorderStyle() = default;
				BorderStyle(UInt32 borderColor, BorderType borderType);

				Bool operator!=(const BorderStyle &style) const;
				Bool operator==(const BorderStyle &style) const;
			};
		private:
			UOSInt index;
			UnsafeArrayOpt<const UTF8Char> id;

			HAlignment halign;
			VAlignment valign;
			Bool wordWrap;

			BorderStyle borderBottom;
			BorderStyle borderLeft;
			BorderStyle borderRight;
			BorderStyle borderTop;

			Optional<WorkbookFont> font;
			UInt32 fillColor;
			FillPattern fillPattern;
			Optional<Text::String> dataFormat;
			Bool protection;

		public:
			CellStyle(UOSInt index);
			~CellStyle();

			NN<CellStyle> Clone() const;
			void CopyFrom(NN<CellStyle> style);
			Bool Equals(NN<const CellStyle> style) const;

			NN<CellStyle> SetIndex(UOSInt index);
			NN<CellStyle> SetID(UnsafeArrayOpt<const UTF8Char> id);
			NN<CellStyle> SetHAlign(HAlignment halign);
			NN<CellStyle> SetVAlign(VAlignment valign);
			NN<CellStyle> SetWordWrap(Bool wordWrap);
			NN<CellStyle> SetFillColor(UInt32 color, FillPattern pattern);
			NN<CellStyle> SetFont(Optional<WorkbookFont> font);
			NN<CellStyle> SetBorderLeft(const BorderStyle &border);
			NN<CellStyle> SetBorderRight(const BorderStyle &border);
			NN<CellStyle> SetBorderTop(const BorderStyle &border);
			NN<CellStyle> SetBorderBottom(const BorderStyle &border);
			NN<CellStyle> SetDataFormat(Text::String *dataFormat);
			NN<CellStyle> SetDataFormat(Text::CString dataFormat);

			UOSInt GetIndex() const;
			UnsafeArrayOpt<const UTF8Char> GetID() const;
			HAlignment GetHAlign() const;
			VAlignment GetVAlign() const;
			Bool GetWordWrap() const;
			UInt32 GetFillColor() const;
			FillPattern GetFillPattern() const;
			Optional<WorkbookFont> GetFont() const;
			BorderStyle GetBorderLeft() const;
			BorderStyle GetBorderRight() const;
			BorderStyle GetBorderTop() const;
			BorderStyle GetBorderBottom() const;
			Optional<Text::String> GetDataFormat() const;
		};
	}
}
#endif
