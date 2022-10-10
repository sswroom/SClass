#ifndef _SM_MEDIA_JASPER_JASPERREPORTELEMENT
#define _SM_MEDIA_JASPER_JASPERREPORTELEMENT
#include "Text/String.h"

namespace Media
{
	namespace Jasper
	{
		class JasperReportElement
		{
		private:
			UOSInt x;
			UOSInt y;
			UOSInt width;
			UOSInt height;
			Text::String *uuid;
		public:
			JasperReportElement();
			~JasperReportElement();

			void SetX(UOSInt x);
			void SetY(UOSInt y);
			void SetWidth(UOSInt width);
			void SetHeight(UOSInt height);
			void SetUUID(Text::String *uuid);
			UOSInt GetX() const;
			UOSInt GetY() const;
			UOSInt GetWidth() const;
			UOSInt GetHeight() const;
			Text::String *GetUUID() const;
		};
	}
}
#endif
