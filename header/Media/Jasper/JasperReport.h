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
			Data::ArrayList<JasperBand*> detailList;
			Text::String *uuid;
			Text::String *reportName;
			UInt32 pageWidth; //Pixels
			UInt32 pageHeight; //Pixels
			UInt32 marginLeft; //Pixels
			UInt32 marginTop; //Pixels
			UInt32 marginRight; //Pixels
			UInt32 marginBottom; //Pixels

			void InitValues();
		public:
			JasperReport(Text::String *sourceName);
			JasperReport(Text::CString sourceName);
			virtual ~JasperReport();

			virtual IO::ParserType GetParserType() const;

			void SetReportName(Text::String *reportName);
			void SetPageWidth(UInt32 pageWidth);
			void SetPageHeight(UInt32 pageHeight);
			void SetMarginLeft(UInt32 marginLeft);
			void SetMarginTop(UInt32 marginTop);
			void SetMarginRight(UInt32 marginRight);
			void SetMarginBottom(UInt32 marginBottom);
			void SetUUID(Text::String *uuid);

			Text::String *GetReportName() const;
			UInt32 GetPageWidth() const;
			UInt32 GetPageHeight() const;
			UInt32 GetMarginLeft() const;
			UInt32 GetMarginTop() const;
			UInt32 GetMarginRight() const;
			UInt32 GetMarginBottom() const;
			Text::String *GetUUID() const;

			void SetQueryString(Text::String *str);
			void SetProperty(Text::String *name, Text::String *value);
			void AddImport(Text::String *value);
			void AddParameter(Text::String *name, Text::String *className, Text::CString defValueExp);
			void SetTitle(Media::Jasper::JasperBand *band);
			void AddDetail(Media::Jasper::JasperBand *band);
		};
	}
}
#endif