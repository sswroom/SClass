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
		Optional<Text::String> objectId;
		Optional<Text::String> title;
		Optional<Text::String> link;
		Optional<Text::String> description;
		Bool descHTML;
		Optional<Text::String> author;
		Optional<Text::String> category;
		Optional<Text::String> comments;
		Optional<Text::String> enclosure;
		Data::Timestamp pubDate;
		Optional<Text::String> source;
		Optional<Text::String> guid;
		Optional<Text::String> imgURL;
		Double lat;
		Double lon;

	public:
		RSSItem(NN<Text::XMLNode> itemNode);
		RSSItem(NN<Text::XMLReader> itemNode);
		~RSSItem();

		Bool IsError();
		Optional<Text::String> GetId();
	};

	class RSS : public Data::ReadingListNN<RSSItem>
	{
	private:
		Bool isError;
		Optional<Text::String> title;
		Optional<Text::String> link;
		Optional<Text::String> description;
		Optional<Text::String> language;
		Optional<Text::String> copyright;
		Optional<Text::String> managingEditor;
		Optional<Text::String> webMaster;
		Data::Timestamp pubDate;
		Data::Timestamp lastBuildDate;
//		Optional<Text::String< category;
		Optional<Text::String> generator;
		Optional<Text::String> docs;
//		Optional<Text::String> cloud;
		Int32 ttl;
//		Optional<RSSImage> image;
		Data::ArrayListNN<RSSItem> items;

	public:
		RSS(Text::CStringNN url, Optional<Text::String> userAgent, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Data::Duration timeout, NN<IO::LogTool> log);
		virtual ~RSS();

		Bool IsError();
		virtual UOSInt Add(NN<RSSItem> val);
		virtual UOSInt GetCount() const;
		virtual NN<RSSItem> GetItemNoCheck(UOSInt Index) const;
		virtual Optional<RSSItem> GetItem(UOSInt Index) const;

		Optional<Text::String> GetTitle() const;
		Optional<Text::String> GetLink() const;
		Optional<Text::String> GetDescription() const;
		Optional<Text::String> GetLanguage() const;
		Optional<Text::String> GetCopyright() const;
		Optional<Text::String> GetManagingEditor() const;
		Optional<Text::String> GetWebMaster() const;
		Data::Timestamp GetPubDate() const;
		Data::Timestamp GetLastBuildDate() const;
		Optional<Text::String> GetGenerator() const;
		Optional<Text::String> GetDocs() const;
		Int32 GetTTL() const;

		static void GetYoutubeURL(Text::CStringNN channelId, NN<Text::StringBuilderUTF8> outURL);
	};
}
#endif
