#ifndef _SM_IO_PARSEDOBJECT
#define _SM_IO_PARSEDOBJECT
#include "Text/CString.h"
#include "Text/String.h"

namespace IO
{
	enum class ParserType
	{
		Unknown,
		Stream,
//		Image,
		MediaFile,
		PackageFile,
		ImageList,
		EXEFile,
		MapLayer,
		ReadingDB,
		MapEnv,
		FileCheck,
		TextDocument,
		Workbook,
		SectorData,
		LogFile,
		Playlist,
		CoordinateSystem,
		CodeProject,
		VectorDocument,
		LUT,
		FontRenderer,
		MIMEObject,
		EthernetAnalyzer,
		JavaClass,
		Smake,
		ASN1Data,
		BTScanLog,
		SystemInfoLog,
		JasperReport,
		PDFDocument,

		LastType = PDFDocument
	};

	Text::CStringNN ParserTypeGetName(ParserType pt);

	class ParsedObject
	{
	protected:
		NN<Text::String> sourceName;

		ParsedObject(NN<Text::String> sourceName);
		ParsedObject(const Text::CStringNN &sourceName);
	public:
		virtual ~ParsedObject();

		virtual IO::ParserType GetParserType() const = 0;
		UTF8Char *GetSourceName(UTF8Char *oriStr) const;
		NN<Text::String> GetSourceNameObj() const { return this->sourceName; }
		void SetSourceName(NN<Text::String> sourceName);
		void SetSourceName(Text::CStringNN sourceName);
	};
}

#endif
