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


	Text::StringBuilderUTF8 sb;
	Text::XMLAttrib *attr;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = itemNode->GetAttribCnt();
	while (i < j)
	{
		attr = itemNode->GetAttrib(i);
		if (attr->name->EqualsICase(UTF8STRC("rdf:about")) && attr->value)
		{
			SDEL_STRING(this->guid);
			this->guid = attr->value->Clone();
		}
		i++;
	}

	i = 0;
	j = itemNode->GetChildCnt();
	while (i < j)
	{
		Text::XMLNode *node = itemNode->GetChild(i);
		if (node->GetNodeType() == Text::XMLNode::NodeType::Element)
		{
			if (node->name->EqualsICase(UTF8STRC("title")))
			{
				sb.ClearStr();
				node->GetInnerText(&sb);
				SDEL_STRING(this->title);
				this->title = Text::String::New(sb.ToCString());
			}
			else if (node->name->EqualsICase(UTF8STRC("link")))
			{
				if (node->GetChildCnt() == 0)
				{
					UOSInt k = node->GetAttribCnt();
					while (k-- > 0)
					{
						attr = node->GetAttrib(k);
						if (attr->name->Equals(UTF8STRC("href")) && attr->value)
						{
							SDEL_STRING(this->link);
							this->link = attr->value->Clone();
							break;
						}
					}
				}
				else
				{
					sb.ClearStr();
					node->GetInnerText(&sb);
					SDEL_STRING(this->link);
					this->link = Text::String::New(sb.ToCString());
				}
			}
			else if (node->name->EqualsICase(UTF8STRC("guid")))
			{
				sb.ClearStr();
				node->GetInnerText(&sb);
				SDEL_STRING(this->guid);
				this->guid = Text::String::New(sb.ToCString());
			}
			else if (node->name->EqualsICase(UTF8STRC("id")))
			{
				sb.ClearStr();
				node->GetInnerText(&sb);
				SDEL_STRING(this->guid);
				this->guid = Text::String::New(sb.ToCString());
			}
			else if (node->name->EqualsICase(UTF8STRC("description")))
			{
				sb.ClearStr();
				node->GetInnerText(&sb);
				SDEL_STRING(this->description);
				this->description = Text::String::New(sb.ToCString());
				this->descHTML = itemNode->name->Equals(UTF8STRC("item"));
			}
			else if (node->name->EqualsICase(UTF8STRC("category")))
			{
				sb.ClearStr();
				node->GetInnerText(&sb);
				SDEL_STRING(this->category);
				this->category = Text::String::New(sb.ToCString());
			}
			else if (node->name->EqualsICase(UTF8STRC("author")))
			{
				sb.ClearStr();
				node->GetInnerText(&sb);
				SDEL_STRING(this->author);
				this->author = Text::String::New(sb.ToCString());
			}
			else if (node->name->EqualsICase(UTF8STRC("pubDate")))
			{
				sb.ClearStr();
				node->GetInnerText(&sb);
				SDEL_CLASS(this->pubDate);
				NEW_CLASS(this->pubDate, Data::DateTime(sb.ToCString()));
			}
			else if (node->name->EqualsICase(UTF8STRC("dc:date")))
			{
				sb.ClearStr();
				node->GetInnerText(&sb);
				SDEL_CLASS(this->pubDate);
				NEW_CLASS(this->pubDate, Data::DateTime(sb.ToCString()));
			}
			else if (node->name->EqualsICase(UTF8STRC("published")))
			{
				sb.ClearStr();
				node->GetInnerText(&sb);
				SDEL_CLASS(this->pubDate);
				NEW_CLASS(this->pubDate, Data::DateTime(sb.ToCString()));
			}
			else if (node->name->EqualsICase(UTF8STRC("updated")))
			{
				sb.ClearStr();
				node->GetInnerText(&sb);
				SDEL_CLASS(this->pubDate);
				NEW_CLASS(this->pubDate, Data::DateTime(sb.ToCString()));
			}
			else if (node->name->EqualsICase(UTF8STRC("objectId")))
			{
				sb.ClearStr();
				node->GetInnerText(&sb);
				SDEL_STRING(this->objectId);
				this->objectId = Text::String::New(sb.ToCString());
			}
			else if (node->name->EqualsICase(UTF8STRC("yt:videoId")))
			{
				sb.ClearStr();
				node->GetInnerText(&sb);
				SDEL_STRING(this->objectId);
				this->objectId = Text::String::New(sb.ToCString());
			}
			else if (node->name->EqualsICase(UTF8STRC("geo:lat")))
			{
				sb.ClearStr();
				node->GetInnerText(&sb);
				this->lat = sb.ToDouble();
			}
			else if (node->name->EqualsICase(UTF8STRC("geo:long")))
			{
				sb.ClearStr();
				node->GetInnerText(&sb);
				this->lon = sb.ToDouble();
			}
			else if (node->name->EqualsICase(UTF8STRC("content")))
			{
				sb.ClearStr();
				node->GetInnerText(&sb);
				SDEL_STRING(this->description);
				this->description = Text::String::New(sb.ToCString());
			}
			else if (node->name->EqualsICase(UTF8STRC("media:group")))
			{
				UOSInt k = node->GetChildCnt();
				Text::XMLNode *node2;
				while (k-- > 0)
				{
					node2 = node->GetChild(k);
					if (node2->GetNodeType() == Text::XMLNode::NodeType::Element)
					{
						if (node2->name->Equals(UTF8STRC("media:description")))
						{
							sb.ClearStr();
							node->GetInnerText(&sb);
							SDEL_STRING(this->description);
							this->description = Text::String::New(sb.ToCString());
						}
						else if (node2->name->Equals(UTF8STRC("media:imgURL")))
						{
							sb.ClearStr();
							node->GetInnerText(&sb);
							SDEL_STRING(this->description);
							this->description = Text::String::New(sb.ToCString());
						}
					}
				}
			}
		}
		i++;
	}
}

Net::RSSItem::~RSSItem()
{
	SDEL_STRING(this->objectId);
	SDEL_STRING(this->title);
	SDEL_STRING(this->link);
	SDEL_STRING(this->description);
	SDEL_STRING(this->author);
	SDEL_STRING(this->category);
	SDEL_STRING(this->comments);
	SDEL_STRING(this->enclosure);
	SDEL_CLASS(this->pubDate);
	SDEL_STRING(this->source);
	SDEL_STRING(this->guid);
}

Bool Net::RSSItem::IsError()
{
	return this->title == 0;
}

Text::String *Net::RSSItem::GetId()
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

Net::RSS::RSS(Text::CString url, Text::String *userAgent, Net::SocketFactory *sockf, Net::SSLEngine *ssl)
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

	IO::ParsedObject *pobj;
	IO::Stream *stm;
	pobj = Net::URL::OpenObject(url, STR_CSTR(userAgent), sockf, ssl);
	if (pobj == 0)
	{
		return;
	}
	if (pobj->GetParserType() != IO::ParserType::Stream)
	{
		DEL_CLASS(pobj);
		return;
	}
	stm = (IO::Stream*)pobj;
	if (stm == 0)
		return;

	Text::XMLDocument doc;
	Text::EncodingFactory *encFact;
	NEW_CLASS(encFact, Text::EncodingFactory());
	doc.ParseStream(encFact, stm);
	DEL_CLASS(encFact);
	DEL_CLASS(stm);

	Text::StringBuilderUTF8 sb;
	UOSInt i = doc.GetChildCnt();
	while (i-- > 0)
	{
		Text::XMLNode *node = doc.GetChild(i);
		if (node->GetNodeType() == Text::XMLNode::NodeType::Element && node->name->EqualsICase(UTF8STRC("RSS")))
		{
			UOSInt j = node->GetChildCnt();
			while (j-- > 0)
			{
				Text::XMLNode *node2 = node->GetChild(j);
				if (node2->GetNodeType() == Text::XMLNode::NodeType::Element && node2->name->EqualsICase(UTF8STRC("Channel")))
				{
					UOSInt k = 0;
					UOSInt l = node2->GetChildCnt();
					while (k < l)
					{
						Text::XMLNode *node3 = node2->GetChild(k);
						if (node3->GetNodeType() == Text::XMLNode::NodeType::Element)
						{
							if (node3->name->EqualsICase(UTF8STRC("title")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_STRING(this->title);
								this->title = Text::String::New(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("description")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_STRING(this->description);
								this->description = Text::String::New(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("link")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_STRING(this->link);
								this->link = Text::String::New(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("ttl")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								this->ttl = Text::StrToInt32(sb.ToString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("language")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_STRING(this->language);
								this->language = Text::String::New(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("webMaster")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_STRING(this->webMaster);
								this->webMaster = Text::String::New(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("copyright")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_STRING(this->copyright);
								this->copyright = Text::String::New(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("generator")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_STRING(this->generator);
								this->generator = Text::String::New(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("pubDate")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_CLASS(this->pubDate);
								NEW_CLASS(this->pubDate, Data::DateTime());
								this->pubDate->SetValue(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("lastBuildDate")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_CLASS(this->lastBuildDate);
								NEW_CLASS(this->lastBuildDate, Data::DateTime());
								this->lastBuildDate->SetValue(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("item")))
							{
								RSSItem *itm;
								NEW_CLASS(itm, Net::RSSItem(node3));
								if (itm->IsError())
								{
									DEL_CLASS(itm);
								}
								else
								{
									this->items.Add(itm);
								}
							}
						}
						k++;
					}
				}
			}
		}
		else if (node->GetNodeType() == Text::XMLNode::NodeType::Element && node->name->EqualsICase(UTF8STRC("rdf:RDF")))
		{
			UOSInt j = node->GetChildCnt();
			UOSInt i2 = 0;
			while (i2 < j)
			{
				Text::XMLNode *node2 = node->GetChild(i2);
				if (node2->GetNodeType() == Text::XMLNode::NodeType::Element && node2->name->EqualsICase(UTF8STRC("Channel")))
				{
					UOSInt k = 0;
					UOSInt l = node2->GetChildCnt();
					while (k < l)
					{
						Text::XMLNode *node3 = node2->GetChild(k);
						if (node3->GetNodeType() == Text::XMLNode::NodeType::Element)
						{
							if (node3->name->EqualsICase(UTF8STRC("title")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_STRING(this->title);
								this->title = Text::String::New(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("description")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_STRING(this->description);
								this->description = Text::String::New(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("link")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_STRING(this->link);
								this->link = Text::String::New(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("ttl")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								this->ttl = Text::StrToInt32(sb.ToString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("dc:language")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_STRING(this->language);
								this->language = Text::String::New(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("webMaster")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_STRING(this->webMaster);
								this->webMaster = Text::String::New(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("dc:rights")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_STRING(this->copyright);
								this->copyright = Text::String::New(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("generator")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_STRING(this->generator);
								this->generator = Text::String::New(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("dc:date")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_CLASS(this->pubDate);
								NEW_CLASS(this->pubDate, Data::DateTime());
								this->pubDate->SetValue(sb.ToCString());
							}
							else if (node3->name->EqualsICase(UTF8STRC("lastBuildDate")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_CLASS(this->lastBuildDate);
								NEW_CLASS(this->lastBuildDate, Data::DateTime());
								this->lastBuildDate->SetValue(sb.ToCString());
							}
						}
						k++;
					}
				}
				else if (node2->GetNodeType() == Text::XMLNode::NodeType::Element && node2->name->EqualsICase(UTF8STRC("item")))
				{
					RSSItem *itm;
					NEW_CLASS(itm, Net::RSSItem(node2));
					if (itm->IsError())
					{
						DEL_CLASS(itm);
					}
					else
					{
						this->items.Add(itm);
					}
				}
				i2++;
			}
		}
		else if (node->GetNodeType() == Text::XMLNode::NodeType::Element && node->name->EqualsICase(UTF8STRC("feed")))
		{
			Text::XMLAttrib *attr;
			UOSInt j = node->GetChildCnt();
			UOSInt i2 = 0;
			while (i2 < j)
			{
				Text::XMLNode *node2 = node->GetChild(i2);
				if (node2->GetNodeType() == Text::XMLNode::NodeType::Element)
				{
					if (node2->name->EqualsICase(UTF8STRC("title")))
					{
						sb.ClearStr();
						node2->GetInnerText(&sb);
						SDEL_STRING(this->title);
						this->title = Text::String::New(sb.ToCString());
					}
					else if (node2->name->EqualsICase(UTF8STRC("description")))
					{
						sb.ClearStr();
						node2->GetInnerText(&sb);
						SDEL_STRING(this->description);
						this->description = Text::String::New(sb.ToCString());
					}
					else if (node2->name->EqualsICase(UTF8STRC("link")))
					{
						Int32 linkType = 0;
						UOSInt k = 0;
						UOSInt l = node2->GetAttribCnt();
						while (k < l)
						{
							attr = node2->GetAttrib(k);
							if (attr->name->Equals(UTF8STRC("rel")) && attr->value)
							{
								if (attr->value->Equals(UTF8STRC("self")))
								{
									linkType = 1;
								}
								else if (attr->value->Equals(UTF8STRC("alternate")))
								{
									linkType = 2;
								}
							}
							else if (attr->name->Equals(UTF8STRC("href")) && attr->value)
							{
								if (linkType == 2)
								{
									SDEL_STRING(this->link);
									this->link = attr->value->Clone();
								}
							}
							k++;
						}
					}
					else if (node2->name->EqualsICase(UTF8STRC("ttl")))
					{
						sb.ClearStr();
						node2->GetInnerText(&sb);
						this->ttl = Text::StrToInt32(sb.ToString());
					}
					else if (node2->name->EqualsICase(UTF8STRC("dc:language")))
					{
						sb.ClearStr();
						node2->GetInnerText(&sb);
						SDEL_STRING(this->language);
						this->language = Text::String::New(sb.ToCString());
					}
					else if (node2->name->EqualsICase(UTF8STRC("author")))
					{
						UOSInt k;
						UOSInt l;
						Text::XMLNode *node3;
						k = 0;
						l = node2->GetChildCnt();
						while (k < l)
						{
							node3 = node2->GetChild(k);
							if (node3->GetNodeType() == Text::XMLNode::NodeType::Element && node3->name->Equals(UTF8STRC("name")))
							{
								sb.ClearStr();
								node3->GetInnerText(&sb);
								SDEL_STRING(this->webMaster);
								this->webMaster = Text::String::New(sb.ToCString());
							}
							k++;
						}
					}
					else if (node2->name->EqualsICase(UTF8STRC("dc:rights")))
					{
						sb.ClearStr();
						node2->GetInnerText(&sb);
						SDEL_STRING(this->copyright);
						this->copyright = Text::String::New(sb.ToCString());
					}
					else if (node2->name->EqualsICase(UTF8STRC("generator")))
					{
						sb.ClearStr();
						node2->GetInnerText(&sb);
						SDEL_STRING(this->generator);
						this->generator = Text::String::New(sb.ToCString());
					}
					else if (node2->name->EqualsICase(UTF8STRC("published")))
					{
						sb.ClearStr();
						node2->GetInnerText(&sb);
						SDEL_CLASS(this->pubDate);
						NEW_CLASS(this->pubDate, Data::DateTime());
						this->pubDate->SetValue(sb.ToCString());
					}
					else if (node2->name->EqualsICase(UTF8STRC("lastBuildDate")))
					{
						sb.ClearStr();
						node2->GetInnerText(&sb);
						SDEL_CLASS(this->lastBuildDate);
						NEW_CLASS(this->lastBuildDate, Data::DateTime());
						this->lastBuildDate->SetValue(sb.ToCString());
					}
					else if (node2->name->EqualsICase(UTF8STRC("entry")))
					{
						RSSItem *itm;
						NEW_CLASS(itm, Net::RSSItem(node2));
						if (itm->IsError())
						{
							DEL_CLASS(itm);
						}
						else
						{
							this->items.Add(itm);
						}
					}
				}
				i2++;
			}
		}
	}
}

Net::RSS::~RSS()
{
	SDEL_STRING(this->title);
	SDEL_STRING(this->link);
	SDEL_STRING(this->description);
	SDEL_STRING(this->language);
	SDEL_STRING(this->copyright);
	SDEL_STRING(this->managingEditor);
	SDEL_STRING(this->webMaster);
	SDEL_CLASS(this->pubDate);
	SDEL_CLASS(this->lastBuildDate);
	SDEL_STRING(this->generator);
	SDEL_STRING(this->docs);
	UOSInt i = this->items.GetCount();
	RSSItem *item;
	while (i-- > 0)
	{
		item = this->items.GetItem(i);
		DEL_CLASS(item);
	}
}

Bool Net::RSS::IsError()
{
	return this->items.GetCount() == 0;
}

UOSInt Net::RSS::Add(Net::RSSItem* val)
{
	return this->items.Add(val);
}

UOSInt Net::RSS::GetCount() const
{
	return this->items.GetCount();
}

Net::RSSItem *Net::RSS::GetItem(UOSInt Index) const
{
	return this->items.GetItem(Index);
}

Text::String *Net::RSS::GetTitle()
{
	return this->title;
}

Text::String *Net::RSS::GetLink()
{
	return this->link;
}

Text::String *Net::RSS::GetDescription()
{
	return this->description;
}

Text::String *Net::RSS::GetLanguage()
{
	return this->language;
}

Text::String *Net::RSS::GetCopyright()
{
	return this->copyright;
}

Text::String *Net::RSS::GetManagingEditor()
{
	return this->managingEditor;
}

Text::String *Net::RSS::GetWebMaster()
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

Text::String *Net::RSS::GetGenerator()
{
	return this->generator;
}

Text::String *Net::RSS::GetDocs()
{
	return this->docs;
}

void Net::RSS::GetYoutubeURL(Text::CString channelId, Text::StringBuilderUTF8 *outURL)
{
	outURL->AppendC(UTF8STRC("https://www.youtube.com/feeds/videos.xml?channel_id="));
	outURL->Append(channelId);
}
