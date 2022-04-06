#ifndef _SM_NET_RSS
#define _SM_NET_RSS
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/XMLDOM.h"

namespace Net
{
	class RSSItem
	{
	public:
		Text::String *objectId;
		Text::String *title;
		Text::String *link;
		Text::String *description;
		Bool descHTML;
		Text::String *author;
		Text::String *category;
		Text::String *comments;
		Text::String *enclosure;
		Data::DateTime *pubDate;
		Text::String *source;
		Text::String *guid;
		Text::String *imgURL;
		Double lat;
		Double lon;

	public:
		RSSItem(Text::XMLNode *itemNode);
		~RSSItem();

		Bool IsError();
		Text::String *GetId();
	};

	class RSS : public Data::ReadingList<RSSItem*>
	{
	private:
		Bool isError;
		Text::String *title;
		Text::String *link;
		Text::String *description;
		Text::String *language;
		Text::String *copyright;
		Text::String *managingEditor;
		Text::String *webMaster;
		Data::DateTime *pubDate;
		Data::DateTime *lastBuildDate;
//		Text::String *category;
		Text::String *generator;
		Text::String *docs;
//		Text::String *cloud;
		Int32 ttl;
//		RSSImage *image;
		Data::ArrayList<RSSItem*> *items;

	public:
		RSS(Text::CString url, Text::String *userAgent, Net::SocketFactory *sockf, Net::SSLEngine *ssl);
		virtual ~RSS();

		Bool IsError();
		virtual UOSInt Add(RSSItem* val);
		virtual UOSInt GetCount() const;
		virtual RSSItem *GetItem(UOSInt Index) const;

		Text::String *GetTitle();
		Text::String *GetLink();
		Text::String *GetDescription();
		Text::String *GetLanguage();
		Text::String *GetCopyright();
		Text::String *GetManagingEditor();
		Text::String *GetWebMaster();
		Data::DateTime *GetPubDate();
		Data::DateTime *GetLastBuildDate();
		Text::String *GetGenerator();
		Text::String *GetDocs();

		static void GetYoutubeURL(Text::CString channelId, Text::StringBuilderUTF8 *outURL);
	};
}
#endif
