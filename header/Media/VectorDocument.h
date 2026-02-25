#ifndef _SM_MEDIA_VECTORDOCUMENT
#define _SM_MEDIA_VECTORDOCUMENT
#include "Data/ArrayListNN.hpp"
#include "IO/ParsedObject.h"
#include "Math/Unit/Distance.h"
#include "Media/ImageList.h"
#include "Media/PrintDocument.h"
#include "Media/VectorGraph.h"

namespace Media
{
	class VectorDocument : public IO::ParsedObject, public Data::ReadingList<Optional<Media::VectorGraph>>, public Media::PrintHandler
	{
	private:
		UIntOS currGraph;
		Data::ArrayListNN<Media::VectorGraph> items;
		Optional<Media::PrintDocument> currDoc;
		NN<Media::DrawEngine> refEng;
		UInt32 srid;

		Optional<Text::String> docName;
		UnsafeArrayOpt<const UTF8Char> author;
		UnsafeArrayOpt<const UTF8Char> subject;
		UnsafeArrayOpt<const UTF8Char> keywords;
		UnsafeArrayOpt<const UTF8Char> creator;
		UnsafeArrayOpt<const UTF8Char> producer;
		Int64 createTimeTicks;
		Int64 modTimeTicks;

	public:
		VectorDocument(UInt32 srid, NN<Media::DrawEngine> refEng);
		VectorDocument(UInt32 srid, Text::CStringNN name, NN<Media::DrawEngine> refEng);
		virtual ~VectorDocument();

		NN<Media::VectorGraph> AddGraph(Double width, Double height, Math::Unit::Distance::DistanceUnit unit);
		void SetDocName(Text::CString docName);
		Optional<Text::String> GetDocName() const;
		void SetCreateTime(Int64 createTimeTicks);
		Int64 GetCreateTime() const;
		void SetModifyTime(Int64 modTimeTicks);
		Int64 GetModifyTime() const;
		void SetAuthor(UnsafeArrayOpt<const UTF8Char> author);
		UnsafeArrayOpt<const UTF8Char> GetAuthor() const;
		void SetSubject(UnsafeArrayOpt<const UTF8Char> subject);
		UnsafeArrayOpt<const UTF8Char> GetSubject() const;
		void SetKeywords(UnsafeArrayOpt<const UTF8Char> keywords);
		UnsafeArrayOpt<const UTF8Char> GetKeywords() const;
		void SetCreator(UnsafeArrayOpt<const UTF8Char> creator);
		UnsafeArrayOpt<const UTF8Char> GetCreator() const;
		void SetProducer(UnsafeArrayOpt<const UTF8Char> producer);
		UnsafeArrayOpt<const UTF8Char> GetProducer() const;

		NN<Media::ImageList> CreateRaster() const;

		virtual UIntOS GetCount() const;
		virtual Optional<Media::VectorGraph> GetItem(UIntOS index) const;

		virtual Bool BeginPrint(NN<PrintDocument> doc);
		virtual Bool PrintPage(NN<Media::DrawImage> printPage); //return has more pages 
		virtual Bool EndPrint(NN<PrintDocument> doc);

		virtual IO::ParserType GetParserType() const;
	};
}
#endif
