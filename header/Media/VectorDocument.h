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
		Media::IPrintDocument *currDoc;
		NotNullPtr<Media::DrawEngine> refEng;
		UInt32 srid;

		Text::String *docName;
		const UTF8Char *author;
		const UTF8Char *subject;
		const UTF8Char *keywords;
		const UTF8Char *creator;
		const UTF8Char *producer;
		Int64 createTimeTicks;
		Int64 modTimeTicks;

	public:
		VectorDocument(UInt32 srid, NotNullPtr<Media::DrawEngine> refEng);
		VectorDocument(UInt32 srid, Text::CStringNN name, NotNullPtr<Media::DrawEngine> refEng);
		virtual ~VectorDocument();

		NotNullPtr<Media::VectorGraph> AddGraph(Double width, Double height, Math::Unit::Distance::DistanceUnit unit);
		void SetDocName(Text::CString docName);
		Text::String *GetDocName() const;
		void SetCreateTime(Int64 createTimeTicks);
		Int64 GetCreateTime() const;
		void SetModifyTime(Int64 modTimeTicks);
		Int64 GetModifyTime() const;
		void SetAuthor(const UTF8Char *author);
		const UTF8Char *GetAuthor() const;
		void SetSubject(const UTF8Char *subject);
		const UTF8Char *GetSubject() const;
		void SetKeywords(const UTF8Char *keywords);
		const UTF8Char *GetKeywords() const;
		void SetCreator(const UTF8Char *creator);
		const UTF8Char *GetCreator() const;
		void SetProducer(const UTF8Char *producer);
		const UTF8Char *GetProducer() const;

		virtual UOSInt GetCount() const;
		virtual Media::VectorGraph *GetItem(UOSInt Index) const;

		virtual Bool BeginPrint(IPrintDocument *doc);
		virtual Bool PrintPage(NotNullPtr<Media::DrawImage> printPage); //return has more pages 
		virtual Bool EndPrint(IPrintDocument *doc);

		virtual IO::ParserType GetParserType() const;
	};
}
#endif
