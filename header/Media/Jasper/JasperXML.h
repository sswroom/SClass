#ifndef _SM_MEDIA_JASPER_JASPERXML
#define _SM_MEDIA_JASPER_JASPERXML
#include "Media/Jasper/JasperReport.h"
#include "Text/XMLReader.h"

namespace Media
{
	namespace Jasper
	{
		class JasperXML
		{
		public:
			static Media::Jasper::JasperReport *ParseJasperReport(NotNullPtr<Text::XMLReader> reader, Text::CString fileName);
		private:
			static Media::Jasper::JasperBand *ParseBand(NotNullPtr<Text::XMLReader> reader);
			static Media::Jasper::JasperElement *ParseElement(NotNullPtr<Text::XMLReader> reader);
		};
	}
}
#endif
