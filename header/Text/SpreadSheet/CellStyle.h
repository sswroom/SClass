#ifndef _SM_TEXT_SPREADSHEET_CELLSTYLE
#define _SM_TEXT_SPREADSHEET_CELLSTYLE
#include "Text/SpreadSheet/WorkbookFont.h"
namespace Text
{
	namespace SpreadSheet
	{
		enum class HAlignment
		{
			Unknown,
			Left,
			Center,
			Right,
			Fill,
			Justify
		};

		enum class VAlignment
		{
			Unknown,
			Top,
			Center,
			Bottom,
			Justify
		};

		class CellStyle
		{
		public:
			typedef enum
			{
				BT_NONE = 0,
				BT_THIN,
				BT_MEDIUM,
				BT_DASHED,
				BT_DOTTED,
				BT_THICK,
				BT_DOUBLE,
				BT_HAIR,
				BT_MEDIUM_DASHED,
				BT_DASH_DOT,
				BT_MEDIUM_DASH_DOT,
				BT_DASH_DOT_DOT,
				BT_MEDIUM_DASH_DOT_DOT,
				BT_SLANTED_DASH_DOT
			} BorderType;

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

				Bool operator!=(BorderStyle style);
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
			const UTF8Char *dataFormat;
			Bool protection;

		public:
			CellStyle(UOSInt index);
			~CellStyle();

			CellStyle *Clone();
			void CopyFrom(CellStyle *style);
			Bool Equals(CellStyle *style);

			CellStyle *SetIndex(UOSInt index);
			CellStyle *SetID(const UTF8Char *id);
			CellStyle *SetHAlign(HAlignment halign);
			CellStyle *SetVAlign(VAlignment valign);
			CellStyle *SetWordWrap(Bool wordWrap);
			CellStyle *SetFillColor(UInt32 color, FillPattern pattern);
			CellStyle *SetFont(WorkbookFont *font);
			CellStyle *SetBorderLeft(BorderStyle *border);
			CellStyle *SetBorderRight(BorderStyle *border);
			CellStyle *SetBorderTop(BorderStyle *border);
			CellStyle *SetBorderBottom(BorderStyle *border);
			CellStyle *SetDataFormat(const UTF8Char *dataFormat);

			UOSInt GetIndex();
			const UTF8Char *GetID();
			HAlignment GetHAlign();
			VAlignment GetVAlign();
			Bool GetWordWrap();
			UInt32 GetFillColor();
			FillPattern GetFillPattern();
			WorkbookFont *GetFont();
			BorderStyle *GetBorderLeft();
			BorderStyle *GetBorderRight();
			BorderStyle *GetBorderTop();
			BorderStyle *GetBorderBottom();
			const UTF8Char *GetDataFormat();
		};
	}
}
#endif
