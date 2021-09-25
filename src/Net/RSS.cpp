#include "Stdafx.h"
#include "Net/RSS.h"
#include "Net/URL.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Net::RSSItem::RSSItem(Text::XMLNode *itemNode)
{
	this->objectId = 0;
	this->title = 0;
	this->link = 0;
	this->guid = 0;
	this->description = 0;
	this->descHTML = false;
	this->author = 0;
	this->category = 0;
	this->comments = 0;
	this->enclosure = 0;
	this->pubDate = 0;
	this->source = 0;
	this->imgURL = 0;
	this->lat = 0;
	this->lon = 0;


	Text::StringBuilderUTF8 *sb;
	NEW_CLASS(sb, Text::StringBuilderUTF8());
	Text::XMLAttrib *attr;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = itemNode->GetAttribCnt();
	while (i < j)
	{
		attr = itemNode->GetAttrib(i);
		if (Text::StrEqualsICase(attr->name, (const UTF8Char*)"rdf:about") && attr->value)
		{
			SDEL_TEXT(this->guid);
			this->guid = Text::StrCopyNew(attr->value);
		}
		i++;
	}

	i = 0;
	j = itemNode->GetChildCnt();
	while (i < j)
	{
		Text::XMLNode *node = itemNode->GetChild(i);
		if (node->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			if (Text::StrCompareICase(node->name, (const UTF8Char*)"title") == 0)
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_TEXT(this->title);
				this->title = Text::StrCopyNew(sb->ToString());
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"link") == 0)
			{
				if (node->GetChildCnt() == 0)
				{
					UOSInt k = node->GetAttribCnt();
					while (k-- > 0)
					{
						attr = node->GetAttrib(k);
						if (Text::StrEquals(attr->name, (const UTF8Char*)"href") && attr->value)
						{
							SDEL_TEXT(this->link);
							this->link = Text::StrCopyNew(attr->value);
							break;
						}
					}
				}
				else
				{
					sb->ClearStr();
					node->GetInnerText(sb);
					SDEL_TEXT(this->link);
					this->link = Text::StrCopyNew(sb->ToString());
				}
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"guid") == 0)
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_TEXT(this->guid);
				this->guid = Text::StrCopyNew(sb->ToString());
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"id") == 0)
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_TEXT(this->guid);
				this->guid = Text::StrCopyNew(sb->ToString());
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"description") == 0)
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_TEXT(this->description);
				this->description = Text::StrCopyNew(sb->ToString());
				this->descHTML = Text::StrEquals(itemNode->name, (const UTF8Char*)"item");
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"category") == 0)
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_TEXT(this->category);
				this->category = Text::StrCopyNew(sb->ToString());
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"author") == 0)
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_TEXT(this->author);
				this->author = Text::StrCopyNew(sb->ToString());
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"pubDate") == 0)
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_CLASS(this->pubDate);
				NEW_CLASS(this->pubDate, Data::DateTime(sb->ToString()));
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"dc:date") == 0)
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_CLASS(this->pubDate);
				NEW_CLASS(this->pubDate, Data::DateTime(sb->ToString()));
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"published") == 0)
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_CLASS(this->pubDate);
				NEW_CLASS(this->pubDate, Data::DateTime(sb->ToString()));
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"updated") == 0)
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_CLASS(this->pubDate);
				NEW_CLASS(this->pubDate, Data::DateTime(sb->ToString()));
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"objectId") == 0)
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_TEXT(this->objectId);
				this->objectId = Text::StrCopyNew(sb->ToString());
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"yt:videoId") == 0)
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_TEXT(this->objectId);
				this->objectId = Text::StrCopyNew(sb->ToString());
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"geo:lat") == 0)
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				this->lat = Text::StrToDouble(sb->ToString());
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"geo:long") == 0)
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				this->lon = Text::StrToDouble(sb->ToString());
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"content") == 0)
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_TEXT(this->description);
				this->description = Text::StrCopyNew(sb->ToString());
			}
			else if (Text::StrCompareICase(node->name, (const UTF8Char*)"media:group") == 0)
			{
				UOSInt k = node->GetChildCnt();
				Text::XMLNode *node2;
				while (k-- > 0)
				{
					node2 = node->GetChild(k);
					if (node2->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						if (Text::StrEquals(node2->name, (const UTF8Char*)"media:description"))
						{
							sb->ClearStr();
							node->GetInnerText(sb);
							SDEL_TEXT(this->description);
							this->description = Text::StrCopyNew(sb->ToString());
						}
						else if (Text::StrEquals(node2->name, (const UTF8Char*)"media:imgURL"))
						{
							sb->ClearStr();
							node->GetInnerText(sb);
							SDEL_TEXT(this->description);
							this->description = Text::StrCopyNew(sb->ToString());
						}
					}
				}
			}
		}
		i++;
	}
	DEL_CLASS(sb);
}

Net::RSSItem::~RSSItem()
{
	SDEL_TEXT(this->objectId);
	SDEL_TEXT(this->title);
	SDEL_TEXT(this->link);
	SDEL_TEXT(this->description);
	SDEL_TEXT(this->author);
	SDEL_TEXT(this->category);
	SDEL_TEXT(this->comments);
	SDEL_TEXT(this->enclosure);
	SDEL_CLASS(this->pubDate);
	SDEL_TEXT(this->source);
	SDEL_TEXT(this->guid);
}

Bool Net::RSSItem::IsError()
{
	return this->title == 0;
}

const UTF8Char *Net::RSSItem::GetId()
{
	if (this->guid)
	{
		return this->guid;
	}
	else
	{
		return this->title;
	}
}

Net::RSS::RSS(const UTF8Char *url, const UTF8Char *userAgent, Net::SocketFactory *sockf, Net::SSLEngine *ssl)
{
	this->isError = true;
	this->title = 0;
	this->link = 0;
	this->description = 0;
	this->language = 0;
	this->copyright = 0;
	this->managingEditor = 0;
	this->webMaster = 0;
	this->pubDate = 0;
	this->lastBuildDate = 0;
	this->generator = 0;
	this->docs = 0;
	this->ttl = 0;
	this->items = 0;

	Text::StringBuilderUTF8 *sb;
	IO::ParsedObject *pobj;
	IO::Stream *stm;
	pobj = Net::URL::OpenObject(url, userAgent, sockf, ssl);
	if (pobj == 0)
	{
		return;
	}
	if (pobj->GetParserType() != IO::ParsedObject::PT_STREAM)
	{
		DEL_CLASS(pobj);
		return;
	}
	stm = (IO::Stream*)pobj;
	if (stm == 0)
		return;

	Text::XMLDocument *doc;
	Text::EncodingFactory *encFact;
	NEW_CLASS(encFact, Text::EncodingFactory());
	NEW_CLASS(doc, Text::XMLDocument());
	doc->ParseStream(encFact, stm);
	DEL_CLASS(encFact);
	DEL_CLASS(stm);

	NEW_CLASS(sb, Text::StringBuilderUTF8());
	NEW_CLASS(items, Data::ArrayList<RSSItem*>());
	UOSInt i = doc->GetChildCnt();
	while (i-- > 0)
	{
		Text::XMLNode *node = doc->GetChild(i);
		if (node->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrCompareICase(node->name, (const UTF8Char*)"RSS") == 0)
		{
			UOSInt j = node->GetChildCnt();
			while (j-- > 0)
			{
				Text::XMLNode *node2 = node->GetChild(j);
				if (node2->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrCompareICase(node2->name, (const UTF8Char*)"Channel") == 0)
				{
					UOSInt k = 0;
					UOSInt l = node2->GetChildCnt();
					while (k < l)
					{
						Text::XMLNode *node3 = node2->GetChild(k);
						if (node3->GetNodeType() == Text::XMLNode::NT_ELEMENT)
						{
							if (Text::StrCompareICase(node3->name, (const UTF8Char*)"title") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								if (this->title)
									Text::StrDelNew(this->title);
								this->title = Text::StrCopyNew(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"description") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								if (this->description)
									Text::StrDelNew(this->description);
								this->description = Text::StrCopyNew(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"link") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								if (this->link)
									Text::StrDelNew(this->link);
								this->link = Text::StrCopyNew(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"ttl") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								this->ttl = Text::StrToInt32(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"language") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								if (this->language)
									Text::StrDelNew(this->language);
								this->language = Text::StrCopyNew(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"webMaster") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								if (this->webMaster)
									Text::StrDelNew(this->webMaster);
								this->webMaster = Text::StrCopyNew(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"copyright") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								if (this->copyright)
									Text::StrDelNew(this->copyright);
								this->copyright = Text::StrCopyNew(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"generator") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_TEXT(this->generator);
								this->generator = Text::StrCopyNew(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"pubDate") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_CLASS(this->pubDate);
								NEW_CLASS(this->pubDate, Data::DateTime());
								this->pubDate->SetValue(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"lastBuildDate") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_CLASS(this->lastBuildDate);
								NEW_CLASS(this->lastBuildDate, Data::DateTime());
								this->lastBuildDate->SetValue(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"item") == 0)
							{
								RSSItem *itm;
								NEW_CLASS(itm, Net::RSSItem(node3));
								if (itm->IsError())
								{
									DEL_CLASS(itm);
								}
								else
								{
									this->items->Add(itm);
								}
							}
						}
						k++;
					}
				}
			}
		}
		else if (node->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrCompareICase(node->name, (const UTF8Char*)"rdf:RDF") == 0)
		{
			UOSInt j = node->GetChildCnt();
			UOSInt i2 = 0;
			while (i2 < j)
			{
				Text::XMLNode *node2 = node->GetChild(i2);
				if (node2->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrCompareICase(node2->name, (const UTF8Char*)"Channel") == 0)
				{
					UOSInt k = 0;
					UOSInt l = node2->GetChildCnt();
					while (k < l)
					{
						Text::XMLNode *node3 = node2->GetChild(k);
						if (node3->GetNodeType() == Text::XMLNode::NT_ELEMENT)
						{
							if (Text::StrCompareICase(node3->name, (const UTF8Char*)"title") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								if (this->title)
									Text::StrDelNew(this->title);
								this->title = Text::StrCopyNew(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"description") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								if (this->description)
									Text::StrDelNew(this->description);
								this->description = Text::StrCopyNew(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"link") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								if (this->link)
									Text::StrDelNew(this->link);
								this->link = Text::StrCopyNew(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"ttl") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								this->ttl = Text::StrToInt32(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"dc:language") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								if (this->language)
									Text::StrDelNew(this->language);
								this->language = Text::StrCopyNew(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"webMaster") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								if (this->webMaster)
									Text::StrDelNew(this->webMaster);
								this->webMaster = Text::StrCopyNew(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"dc:rights") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								if (this->copyright)
									Text::StrDelNew(this->copyright);
								this->copyright = Text::StrCopyNew(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"generator") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_TEXT(this->generator);
								this->generator = Text::StrCopyNew(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"dc:date") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_CLASS(this->pubDate);
								NEW_CLASS(this->pubDate, Data::DateTime());
								this->pubDate->SetValue(sb->ToString());
							}
							else if (Text::StrCompareICase(node3->name, (const UTF8Char*)"lastBuildDate") == 0)
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_CLASS(this->lastBuildDate);
								NEW_CLASS(this->lastBuildDate, Data::DateTime());
								this->lastBuildDate->SetValue(sb->ToString());
							}
						}
						k++;
					}
				}
				else if (node2->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrCompareICase(node2->name, (const UTF8Char*)"item") == 0)
				{
					RSSItem *itm;
					NEW_CLASS(itm, Net::RSSItem(node2));
					if (itm->IsError())
					{
						DEL_CLASS(itm);
					}
					else
					{
						this->items->Add(itm);
					}
				}
				i2++;
			}
		}
		else if (node->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrCompareICase(node->name, (const UTF8Char*)"feed") == 0)
		{
			Text::XMLAttrib *attr;
			UOSInt j = node->GetChildCnt();
			UOSInt i2 = 0;
			while (i2 < j)
			{
				Text::XMLNode *node2 = node->GetChild(i2);
				if (node2->GetNodeType() == Text::XMLNode::NT_ELEMENT)
				{
					if (Text::StrCompareICase(node2->name, (const UTF8Char*)"title") == 0)
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						SDEL_TEXT(this->title);
						this->title = Text::StrCopyNew(sb->ToString());
					}
					else if (Text::StrCompareICase(node2->name, (const UTF8Char*)"description") == 0)
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						SDEL_TEXT(this->description);
						this->description = Text::StrCopyNew(sb->ToString());
					}
					else if (Text::StrCompareICase(node2->name, (const UTF8Char*)"link") == 0)
					{
						Int32 linkType = 0;
						UOSInt k = 0;
						UOSInt l = node2->GetAttribCnt();
						while (k < l)
						{
							attr = node2->GetAttrib(k);
							if (Text::StrEquals(attr->name, (const UTF8Char*)"rel") && attr->value)
							{
								if (Text::StrEquals(attr->value, (const UTF8Char*)"self"))
								{
									linkType = 1;
								}
								else if (Text::StrEquals(attr->value, (const UTF8Char*)"alternate"))
								{
									linkType = 2;
								}
							}
							else if (Text::StrEquals(attr->name, (const UTF8Char*)"href") && attr->value)
							{
								if (linkType == 2)
								{
									SDEL_TEXT(this->link);
									this->link = Text::StrCopyNew(attr->value);
								}
							}
							k++;
						}
					}
					else if (Text::StrCompareICase(node2->name, (const UTF8Char*)"ttl") == 0)
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						this->ttl = Text::StrToInt32(sb->ToString());
					}
					else if (Text::StrCompareICase(node2->name, (const UTF8Char*)"dc:language") == 0)
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						SDEL_TEXT(this->language);
						this->language = Text::StrCopyNew(sb->ToString());
					}
					else if (Text::StrCompareICase(node2->name, (const UTF8Char*)"author") == 0)
					{
						UOSInt k;
						UOSInt l;
						Text::XMLNode *node3;
						k = 0;
						l = node2->GetChildCnt();
						while (k < l)
						{
							node3 = node2->GetChild(k);
							if (node3->GetNodeType() == Text::XMLNode::NT_ELEMENT && Text::StrEquals(node3->name, (const UTF8Char*)"name"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_TEXT(this->webMaster);
								this->webMaster = Text::StrCopyNew(sb->ToString());
							}
							k++;
						}
					}
					else if (Text::StrCompareICase(node2->name, (const UTF8Char*)"dc:rights") == 0)
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						SDEL_TEXT(this->copyright);
						this->copyright = Text::StrCopyNew(sb->ToString());
					}
					else if (Text::StrCompareICase(node2->name, (const UTF8Char*)"generator") == 0)
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						SDEL_TEXT(this->generator);
						this->generator = Text::StrCopyNew(sb->ToString());
					}
					else if (Text::StrCompareICase(node2->name, (const UTF8Char*)"published") == 0)
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						SDEL_CLASS(this->pubDate);
						NEW_CLASS(this->pubDate, Data::DateTime());
						this->pubDate->SetValue(sb->ToString());
					}
					else if (Text::StrCompareICase(node2->name, (const UTF8Char*)"lastBuildDate") == 0)
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						SDEL_CLASS(this->lastBuildDate);
						NEW_CLASS(this->lastBuildDate, Data::DateTime());
						this->lastBuildDate->SetValue(sb->ToString());
					}
					else if (Text::StrCompareICase(node2->name, (const UTF8Char*)"entry") == 0)
					{
						RSSItem *itm;
						NEW_CLASS(itm, Net::RSSItem(node2));
						if (itm->IsError())
						{
							DEL_CLASS(itm);
						}
						else
						{
							this->items->Add(itm);
						}
					}
				}
				i2++;
			}
		}
	}
	DEL_CLASS(sb);
	DEL_CLASS(doc);
}

Net::RSS::~RSS()
{
	SDEL_TEXT(this->title);
	SDEL_TEXT(this->link);
	SDEL_TEXT(this->description);
	SDEL_TEXT(this->language);
	SDEL_TEXT(this->copyright);
	SDEL_TEXT(this->managingEditor);
	SDEL_TEXT(this->webMaster);
	SDEL_CLASS(this->pubDate);
	SDEL_CLASS(this->lastBuildDate);
	SDEL_TEXT(this->generator);
	SDEL_TEXT(this->docs);
	if (this->items)
	{
		UOSInt i = this->items->GetCount();
		RSSItem *item;
		while (i-- > 0)
		{
			item = this->items->GetItem(i);
			DEL_CLASS(item);
		}
		DEL_CLASS(this->items);
	}
}

Bool Net::RSS::IsError()
{
	return this->items == 0;
}

UOSInt Net::RSS::Add(Net::RSSItem* val)
{
	if (this->items == 0)
		return INVALID_INDEX;
	return this->items->Add(val);
}

UOSInt Net::RSS::GetCount()
{
	if (this->items == 0)
		return 0;
	return this->items->GetCount();
}

Net::RSSItem *Net::RSS::GetItem(UOSInt Index)
{
	if (this->items == 0)
		return 0;
	return this->items->GetItem(Index);
}

const UTF8Char *Net::RSS::GetTitle()
{
	return this->title;
}

const UTF8Char *Net::RSS::GetLink()
{
	return this->link;
}

const UTF8Char *Net::RSS::GetDescription()
{
	return this->description;
}

const UTF8Char *Net::RSS::GetLanguage()
{
	return this->language;
}

const UTF8Char *Net::RSS::GetCopyright()
{
	return this->copyright;
}

const UTF8Char *Net::RSS::GetManagingEditor()
{
	return this->managingEditor;
}

const UTF8Char *Net::RSS::GetWebMaster()
{
	return this->webMaster;
}

Data::DateTime *Net::RSS::GetPubDate()
{
	return this->pubDate;
}

Data::DateTime *Net::RSS::GetLastBuildDate()
{
	return this->lastBuildDate;
}

const UTF8Char *Net::RSS::GetGenerator()
{
	return this->generator;
}

const UTF8Char *Net::RSS::GetDocs()
{
	return this->docs;
}

void Net::RSS::GetYoutubeURL(const UTF8Char *channelId, Text::StringBuilderUTF *outURL)
{
	outURL->Append((const UTF8Char*)"https://www.youtube.com/feeds/videos.xml?channel_id=");
	outURL->Append(channelId);
}
