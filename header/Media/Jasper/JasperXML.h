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
			static Media::Jasper::JasperReport *ParseJasperReport(NN<Text::XMLReader> reader, Text::CStringNN fileName);
		private:
			static Optional<Media::Jasper::JasperBand> ParseBand(NN<Text::XMLReader> reader);
			static Optional<Media::Jasper::JasperElement> ParseElement(NN<Text::XMLReader> reader);
		};
	}
}
#endif
