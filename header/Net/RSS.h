#ifndef _SM_NET_RSS
#define _SM_NET_RSS
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/XMLDOM.h"
#include "Text/XMLReader.h"

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
		Data::Timestamp pubDate;
		Text::String *source;
		Text::String *guid;
		Text::String *imgURL;
		Double lat;
		Double lon;

	public:
		RSSItem(Text::XMLNode *itemNode);
		RSSItem(NN<Text::XMLReader> itemNode);
		~RSSItem();

		Bool IsError();
		Text::String *GetId();
	};

	class RSS : public Data::ReadingListNN<RSSItem>
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
		Data::Timestamp pubDate;
		Data::Timestamp lastBuildDate;
//		Text::String *category;
		Text::String *generator;
		Text::String *docs;
//		Text::String *cloud;
		Int32 ttl;
//		RSSImage *image;
		Data::ArrayListNN<RSSItem> items;

	public:
		RSS(Text::CStringNN url, Optional<Text::String> userAgent, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Data::Duration timeout, NN<IO::LogTool> log);
		virtual ~RSS();

		Bool IsError();
		virtual UOSInt Add(NN<RSSItem> val);
		virtual UOSInt GetCount() const;
		virtual NN<RSSItem> GetItemNoCheck(UOSInt Index) const;
		virtual Optional<RSSItem> GetItem(UOSInt Index) const;

		Text::String *GetTitle() const;
		Text::String *GetLink() const;
		Text::String *GetDescription() const;
		Text::String *GetLanguage() const;
		Text::String *GetCopyright() const;
		Text::String *GetManagingEditor() const;
		Text::String *GetWebMaster() const;
		Data::Timestamp GetPubDate() const;
		Data::Timestamp GetLastBuildDate() const;
		Text::String *GetGenerator() const;
		Text::String *GetDocs() const;
		Int32 GetTTL() const;

		static void GetYoutubeURL(Text::CStringNN channelId, NN<Text::StringBuilderUTF8> outURL);
	};
}
#endif
