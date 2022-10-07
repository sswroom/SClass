#ifndef _SM_MEDIA_JASPER_JASPERREPORT
#define _SM_MEDIA_JASPER_JASPERREPORT
#include "Data/ArrayList.h"
#include "IO/ParsedObject.h"
#include "Media/Jasper/JasperBand.h"

namespace Media
{
	namespace Jasper
	{
		class JasperReport : public IO::ParsedObject
		{
		public:
			struct JasperProperty
			{
				Text::String *name;
				Text::String *value;
			};

			struct JasperParameter
			{
				Text::String *name;
				Text::String *className;
				Text::String *defValueExp;
			};

			struct JasperDetail
			{
				JasperBand *band;
			};
		private:
			Data::ArrayList<JasperProperty*> properties;
			Data::ArrayList<Text::String*> importList;
			Data::ArrayList<JasperParameter*> params;
			Text::String *queryString;
			JasperBand *title;

		public:
			JasperReport(Text::String *sourceName);
			JasperReport(Text::CString sourceName);
			virtual ~JasperReport();

			virtual IO::ParserType GetParserType() const;
		};
	}
}
#endif
