#ifndef _SM_MEDIA_JASPER_JASPERREPORT
#define _SM_MEDIA_JASPER_JASPERREPORT
#include "Data/ArrayListNN.hpp"
#include "Data/ArrayListStringNN.h"
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
				NN<Text::String> name;
				NN<Text::String> value;
			};

			struct JasperParameter
			{
				NN<Text::String> name;
				NN<Text::String> className;
				Optional<Text::String> defValueExp;
			};

			struct JasperDetail
			{
				JasperBand *band;
			};
		private:
			Data::ArrayListNN<JasperProperty> properties;
			Data::ArrayListStringNN importList;
			Data::ArrayListNN<JasperParameter> params;
			Optional<Text::String> queryString;
			Optional<JasperBand> title;
			Data::ArrayListNN<JasperBand> detailList;
			Optional<Text::String> uuid;
			Optional<Text::String> reportName;
			UInt32 pageWidth; //Pixels
			UInt32 pageHeight; //Pixels
			UInt32 marginLeft; //Pixels
			UInt32 marginTop; //Pixels
			UInt32 marginRight; //Pixels
			UInt32 marginBottom; //Pixels

			void InitValues();
		public:
			JasperReport(NN<Text::String> sourceName);
			JasperReport(Text::CStringNN sourceName);
			virtual ~JasperReport();

			virtual IO::ParserType GetParserType() const;

			void SetReportName(Optional<Text::String> reportName);
			void SetPageWidth(UInt32 pageWidth);
			void SetPageHeight(UInt32 pageHeight);
			void SetMarginLeft(UInt32 marginLeft);
			void SetMarginTop(UInt32 marginTop);
			void SetMarginRight(UInt32 marginRight);
			void SetMarginBottom(UInt32 marginBottom);
			void SetUUID(Optional<Text::String> uuid);

			Optional<Text::String> GetReportName() const;
			UInt32 GetPageWidth() const;
			UInt32 GetPageHeight() const;
			UInt32 GetMarginLeft() const;
			UInt32 GetMarginTop() const;
			UInt32 GetMarginRight() const;
			UInt32 GetMarginBottom() const;
			Optional<Text::String> GetUUID() const;

			void SetQueryString(Optional<Text::String> str);
			void SetProperty(NN<Text::String> name, NN<Text::String> value);
			void AddImport(NN<Text::String> value);
			void AddParameter(NN<Text::String> name, NN<Text::String> className, Text::CString defValueExp);
			void SetTitle(NN<Media::Jasper::JasperBand> band);
			void AddDetail(NN<Media::Jasper::JasperBand> band);
		};
	}
}
#endif
