#ifndef _SM_NET_RSS
#define _SM_NET_RSS
#include "Text/XMLDOM.h"
#include "Net/SocketFactory.h"

namespace Net
{
	class RSSItem
	{
	public:
		const UTF8Char *objectId;
		const UTF8Char *title;
		const UTF8Char *link;
		const UTF8Char *description;
		Bool descHTML;
		const UTF8Char *author;
		const UTF8Char *category;
		const UTF8Char *comments;
		const UTF8Char *enclosure;
		Data::DateTime *pubDate;
		const UTF8Char *source;
		const UTF8Char *guid;
		const UTF8Char *imgURL;
		Double lat;
		Double lon;

	public:
		RSSItem(Text::XMLNode *itemNode);
		~RSSItem();

		Bool IsError();
		const UTF8Char *GetId();
	};

	class RSS : public Data::List<RSSItem*>
	{
	private:
		Bool isError;
		const UTF8Char *title;
		const UTF8Char *link;
		const UTF8Char *description;
		const UTF8Char *language;
		const UTF8Char *copyright;
		const UTF8Char *managingEditor;
		const UTF8Char *webMaster;
		Data::DateTime *pubDate;
		Data::DateTime *lastBuildDate;
//		const UTF8Char *category;
		const UTF8Char *generator;
		const UTF8Char *docs;
//		const UTF8Char *cloud;
		Int32 ttl;
//		RSSImage *image;
		Data::ArrayList<RSSItem*> *items;

	public:
		RSS(const UTF8Char *url, const UTF8Char *userAgent, Net::SocketFactory *sockf);
		virtual ~RSS();

		Bool IsError();
		virtual UOSInt Add(RSSItem* val);
		virtual UOSInt GetCount();
		virtual RSSItem *GetItem(UOSInt Index);

		const UTF8Char *GetTitle();
		const UTF8Char *GetLink();
		const UTF8Char *GetDescription();
		const UTF8Char *GetLanguage();
		const UTF8Char *GetCopyright();
		const UTF8Char *GetManagingEditor();
		const UTF8Char *GetWebMaster();
		Data::DateTime *GetPubDate();
		Data::DateTime *GetLastBuildDate();
		const UTF8Char *GetGenerator();
		const UTF8Char *GetDocs();

		static void GetYoutubeURL(const UTF8Char *channelId, Text::StringBuilderUTF *outURL);
	};
}
#endif
