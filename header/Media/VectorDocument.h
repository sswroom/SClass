#ifndef _SM_MEDIA_VECTORDOCUMENT
#define _SM_MEDIA_VECTORDOCUMENT
#include "Data/ArrayList.h"
#include "IO/ParsedObject.h"
#include "Math/Unit/Distance.h"
#include "Media/IPrintDocument.h"
#include "Media/VectorGraph.h"

namespace Media
{
	class VectorDocument : public IO::ParsedObject, public Data::List<Media::VectorGraph*>, public Media::IPrintHandler
	{
	private:
		OSInt currGraph;
		Data::ArrayList<Media::VectorGraph*> *items;
		Media::IPrintDocument *currDoc;
		Media::DrawEngine *refEng;
		Int32 srid;

		const UTF8Char *docName;
		const UTF8Char *author;
		const UTF8Char *subject;
		const UTF8Char *keywords;
		const UTF8Char *creator;
		const UTF8Char *producer;
		Int64 createTimeTicks;
		Int64 modTimeTicks;

	public:
		VectorDocument(Int32 srid, Media::DrawEngine *refEng);
		VectorDocument(Int32 srid, const UTF8Char *name, Media::DrawEngine *refEng);
		virtual ~VectorDocument();

		Media::VectorGraph *AddGraph(Double width, Double height, Math::Unit::Distance::DistanceUnit unit);
		const UTF8Char *GetDocName();
		void SetCreateTime(Int64 createTimeTicks);
		Int64 GetCreateTime();
		void SetModifyTime(Int64 modTimeTicks);
		Int64 GetModifyTime();
		void SetAuthor(const UTF8Char *author);
		const UTF8Char *GetAuthor();
		void SetSubject(const UTF8Char *subject);
		const UTF8Char *GetSubject();
		void SetKeywords(const UTF8Char *keywords);
		const UTF8Char *GetKeywords();
		void SetCreator(const UTF8Char *creator);
		const UTF8Char *GetCreator();
		void SetProducer(const UTF8Char *producer);
		const UTF8Char *GetProducer();

		virtual UOSInt GetCount();
		virtual Media::VectorGraph *GetItem(UOSInt Index);

		virtual Bool BeginPrint(IPrintDocument *doc);
		virtual Bool PrintPage(Media::DrawImage *printPage); //return has more pages 
		virtual Bool EndPrint(IPrintDocument *doc);

		virtual IO::ParsedObject::ParserType GetParserType();
	};
};
#endif
