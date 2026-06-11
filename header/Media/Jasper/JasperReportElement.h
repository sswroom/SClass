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
			UIntOS x;
			UIntOS y;
			UIntOS width;
			UIntOS height;
			Optional<Text::String> uuid;
		public:
			JasperReportElement();
			~JasperReportElement();

			void SetX(UIntOS x);
			void SetY(UIntOS y);
			void SetWidth(UIntOS width);
			void SetHeight(UIntOS height);
			void SetUUID(Optional<Text::String> uuid);
			UIntOS GetX() const;
			UIntOS GetY() const;
			UIntOS GetWidth() const;
			UIntOS GetHeight() const;
			Optional<Text::String> GetUUID() const;
		};
	}
}
#endif
