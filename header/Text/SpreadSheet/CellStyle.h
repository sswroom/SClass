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
			const UTF8Char *id;

			HAlignment halign;
			VAlignment valign;
			Bool wordWrap;

			BorderStyle borderBottom;
			BorderStyle borderLeft;
			BorderStyle borderRight;
			BorderStyle borderTop;

			WorkbookFont *font;
			UInt32 fillColor;
			FillPattern fillPattern;
			Text::String *dataFormat;
			Bool protection;

		public:
			CellStyle(UOSInt index);
			~CellStyle();

			CellStyle *Clone() const;
			void CopyFrom(CellStyle *style);
			Bool Equals(const CellStyle *style) const;

			CellStyle *SetIndex(UOSInt index);
			CellStyle *SetID(const UTF8Char *id);
			CellStyle *SetHAlign(HAlignment halign);
			CellStyle *SetVAlign(VAlignment valign);
			CellStyle *SetWordWrap(Bool wordWrap);
			CellStyle *SetFillColor(UInt32 color, FillPattern pattern);
			CellStyle *SetFont(WorkbookFont *font);
			CellStyle *SetBorderLeft(const BorderStyle &border);
			CellStyle *SetBorderRight(const BorderStyle &border);
			CellStyle *SetBorderTop(const BorderStyle &border);
			CellStyle *SetBorderBottom(const BorderStyle &border);
			CellStyle *SetDataFormat(Text::String *dataFormat);
			CellStyle *SetDataFormat(Text::CString dataFormat);

			UOSInt GetIndex() const;
			const UTF8Char *GetID() const;
			HAlignment GetHAlign() const;
			VAlignment GetVAlign() const;
			Bool GetWordWrap() const;
			UInt32 GetFillColor() const;
			FillPattern GetFillPattern() const;
			WorkbookFont *GetFont() const;
			BorderStyle GetBorderLeft() const;
			BorderStyle GetBorderRight() const;
			BorderStyle GetBorderTop() const;
			BorderStyle GetBorderBottom() const;
			Text::String *GetDataFormat() const;
		};
	}
}
#endif
