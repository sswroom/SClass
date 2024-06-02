#ifndef _SM_MEDIA_VECTORDOCUMENT
#define _SM_MEDIA_VECTORDOCUMENT
#include "Data/ArrayList.h"
#include "IO/ParsedObject.h"
#include "Math/Unit/Distance.h"
#include "Media/IPrintDocument.h"
#include "Media/VectorGraph.h"

namespace Media
{
	class VectorDocument : public IO::ParsedObject, public Data::ReadingList<Media::VectorGraph*>, public Media::IPrintHandler
	{
	private:
		UOSInt currGraph;
		Data::ArrayList<Media::VectorGraph*> *items;
		Optional<Media::IPrintDocument> currDoc;
		NN<Media::DrawEngine> refEng;
		UInt32 srid;

		Text::String *docName;
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
		Text::String *GetDocName() const;
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

		virtual UOSInt GetCount() const;
		virtual Media::VectorGraph *GetItem(UOSInt Index) const;

		virtual Bool BeginPrint(NN<IPrintDocument> doc);
		virtual Bool PrintPage(NN<Media::DrawImage> printPage); //return has more pages 
		virtual Bool EndPrint(NN<IPrintDocument> doc);

		virtual IO::ParserType GetParserType() const;
	};
}
#endif
