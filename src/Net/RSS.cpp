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
		if (attr->name->EqualsICase((const UTF8Char*)"rdf:about") && attr->value)
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
		if (node->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			if (node->name->EqualsICase((const UTF8Char*)"title"))
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_STRING(this->title);
				this->title = Text::String::New(sb->ToString(), sb->GetLength());
			}
			else if (node->name->EqualsICase((const UTF8Char*)"link"))
			{
				if (node->GetChildCnt() == 0)
				{
					UOSInt k = node->GetAttribCnt();
					while (k-- > 0)
					{
						attr = node->GetAttrib(k);
						if (attr->name->Equals((const UTF8Char*)"href") && attr->value)
						{
							SDEL_STRING(this->link);
							this->link = attr->value->Clone();
							break;
						}
					}
				}
				else
				{
					sb->ClearStr();
					node->GetInnerText(sb);
					SDEL_STRING(this->link);
					this->link = Text::String::New(sb->ToString(), sb->GetLength());
				}
			}
			else if (node->name->EqualsICase((const UTF8Char*)"guid"))
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_STRING(this->guid);
				this->guid = Text::String::New(sb->ToString(), sb->GetLength());
			}
			else if (node->name->EqualsICase((const UTF8Char*)"id"))
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_STRING(this->guid);
				this->guid = Text::String::New(sb->ToString(), sb->GetLength());
			}
			else if (node->name->EqualsICase((const UTF8Char*)"description"))
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_STRING(this->description);
				this->description = Text::String::New(sb->ToString(), sb->GetLength());
				this->descHTML = itemNode->name->Equals((const UTF8Char*)"item");
			}
			else if (node->name->EqualsICase((const UTF8Char*)"category"))
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_STRING(this->category);
				this->category = Text::String::New(sb->ToString(), sb->GetLength());
			}
			else if (node->name->EqualsICase((const UTF8Char*)"author"))
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_STRING(this->author);
				this->author = Text::String::New(sb->ToString(), sb->GetLength());
			}
			else if (node->name->EqualsICase((const UTF8Char*)"pubDate"))
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_CLASS(this->pubDate);
				NEW_CLASS(this->pubDate, Data::DateTime(sb->ToString()));
			}
			else if (node->name->EqualsICase((const UTF8Char*)"dc:date"))
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_CLASS(this->pubDate);
				NEW_CLASS(this->pubDate, Data::DateTime(sb->ToString()));
			}
			else if (node->name->EqualsICase((const UTF8Char*)"published"))
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_CLASS(this->pubDate);
				NEW_CLASS(this->pubDate, Data::DateTime(sb->ToString()));
			}
			else if (node->name->EqualsICase((const UTF8Char*)"updated"))
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_CLASS(this->pubDate);
				NEW_CLASS(this->pubDate, Data::DateTime(sb->ToString()));
			}
			else if (node->name->EqualsICase((const UTF8Char*)"objectId"))
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_STRING(this->objectId);
				this->objectId = Text::String::New(sb->ToString(), sb->GetLength());
			}
			else if (node->name->EqualsICase((const UTF8Char*)"yt:videoId"))
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_STRING(this->objectId);
				this->objectId = Text::String::New(sb->ToString(), sb->GetLength());
			}
			else if (node->name->EqualsICase((const UTF8Char*)"geo:lat"))
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				this->lat = Text::StrToDouble(sb->ToString());
			}
			else if (node->name->EqualsICase((const UTF8Char*)"geo:long"))
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				this->lon = Text::StrToDouble(sb->ToString());
			}
			else if (node->name->EqualsICase((const UTF8Char*)"content"))
			{
				sb->ClearStr();
				node->GetInnerText(sb);
				SDEL_STRING(this->description);
				this->description = Text::String::New(sb->ToString(), sb->GetLength());
			}
			else if (node->name->EqualsICase((const UTF8Char*)"media:group"))
			{
				UOSInt k = node->GetChildCnt();
				Text::XMLNode *node2;
				while (k-- > 0)
				{
					node2 = node->GetChild(k);
					if (node2->GetNodeType() == Text::XMLNode::NT_ELEMENT)
					{
						if (node2->name->Equals((const UTF8Char*)"media:description"))
						{
							sb->ClearStr();
							node->GetInnerText(sb);
							SDEL_STRING(this->description);
							this->description = Text::String::New(sb->ToString(), sb->GetLength());
						}
						else if (node2->name->Equals((const UTF8Char*)"media:imgURL"))
						{
							sb->ClearStr();
							node->GetInnerText(sb);
							SDEL_STRING(this->description);
							this->description = Text::String::New(sb->ToString(), sb->GetLength());
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

Net::RSS::RSS(const UTF8Char *url, Text::String *userAgent, Net::SocketFactory *sockf, Net::SSLEngine *ssl)
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
	pobj = Net::URL::OpenObject(url, STR_PTRC(userAgent), sockf, ssl);
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
		if (node->GetNodeType() == Text::XMLNode::NT_ELEMENT && node->name->EqualsICase((const UTF8Char*)"RSS"))
		{
			UOSInt j = node->GetChildCnt();
			while (j-- > 0)
			{
				Text::XMLNode *node2 = node->GetChild(j);
				if (node2->GetNodeType() == Text::XMLNode::NT_ELEMENT && node2->name->EqualsICase((const UTF8Char*)"Channel"))
				{
					UOSInt k = 0;
					UOSInt l = node2->GetChildCnt();
					while (k < l)
					{
						Text::XMLNode *node3 = node2->GetChild(k);
						if (node3->GetNodeType() == Text::XMLNode::NT_ELEMENT)
						{
							if (node3->name->EqualsICase((const UTF8Char*)"title"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_STRING(this->title);
								this->title = Text::String::New(sb->ToString(), sb->GetLength());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"description"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_STRING(this->description);
								this->description = Text::String::New(sb->ToString(), sb->GetLength());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"link"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_STRING(this->link);
								this->link = Text::String::New(sb->ToString(), sb->GetLength());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"ttl"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								this->ttl = Text::StrToInt32(sb->ToString());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"language"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_STRING(this->language);
								this->language = Text::String::New(sb->ToString(), sb->GetLength());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"webMaster"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_STRING(this->webMaster);
								this->webMaster = Text::String::New(sb->ToString(), sb->GetLength());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"copyright"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_STRING(this->copyright);
								this->copyright = Text::String::New(sb->ToString(), sb->GetLength());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"generator"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_STRING(this->generator);
								this->generator = Text::String::New(sb->ToString(), sb->GetLength());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"pubDate"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_CLASS(this->pubDate);
								NEW_CLASS(this->pubDate, Data::DateTime());
								this->pubDate->SetValue(sb->ToString());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"lastBuildDate"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_CLASS(this->lastBuildDate);
								NEW_CLASS(this->lastBuildDate, Data::DateTime());
								this->lastBuildDate->SetValue(sb->ToString());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"item"))
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
		else if (node->GetNodeType() == Text::XMLNode::NT_ELEMENT && node->name->EqualsICase((const UTF8Char*)"rdf:RDF"))
		{
			UOSInt j = node->GetChildCnt();
			UOSInt i2 = 0;
			while (i2 < j)
			{
				Text::XMLNode *node2 = node->GetChild(i2);
				if (node2->GetNodeType() == Text::XMLNode::NT_ELEMENT && node2->name->EqualsICase((const UTF8Char*)"Channel"))
				{
					UOSInt k = 0;
					UOSInt l = node2->GetChildCnt();
					while (k < l)
					{
						Text::XMLNode *node3 = node2->GetChild(k);
						if (node3->GetNodeType() == Text::XMLNode::NT_ELEMENT)
						{
							if (node3->name->EqualsICase((const UTF8Char*)"title"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_STRING(this->title);
								this->title = Text::String::New(sb->ToString(), sb->GetLength());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"description"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_STRING(this->description);
								this->description = Text::String::New(sb->ToString(), sb->GetLength());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"link"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_STRING(this->link);
								this->link = Text::String::New(sb->ToString(), sb->GetLength());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"ttl"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								this->ttl = Text::StrToInt32(sb->ToString());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"dc:language"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_STRING(this->language);
								this->language = Text::String::New(sb->ToString(), sb->GetLength());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"webMaster"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_STRING(this->webMaster);
								this->webMaster = Text::String::New(sb->ToString(), sb->GetLength());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"dc:rights"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_STRING(this->copyright);
								this->copyright = Text::String::New(sb->ToString(), sb->GetLength());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"generator"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_STRING(this->generator);
								this->generator = Text::String::New(sb->ToString(), sb->GetLength());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"dc:date"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_CLASS(this->pubDate);
								NEW_CLASS(this->pubDate, Data::DateTime());
								this->pubDate->SetValue(sb->ToString());
							}
							else if (node3->name->EqualsICase((const UTF8Char*)"lastBuildDate"))
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
				else if (node2->GetNodeType() == Text::XMLNode::NT_ELEMENT && node2->name->EqualsICase((const UTF8Char*)"item"))
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
		else if (node->GetNodeType() == Text::XMLNode::NT_ELEMENT && node->name->EqualsICase((const UTF8Char*)"feed"))
		{
			Text::XMLAttrib *attr;
			UOSInt j = node->GetChildCnt();
			UOSInt i2 = 0;
			while (i2 < j)
			{
				Text::XMLNode *node2 = node->GetChild(i2);
				if (node2->GetNodeType() == Text::XMLNode::NT_ELEMENT)
				{
					if (node2->name->EqualsICase((const UTF8Char*)"title"))
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						SDEL_STRING(this->title);
						this->title = Text::String::New(sb->ToString(), sb->GetLength());
					}
					else if (node2->name->EqualsICase((const UTF8Char*)"description"))
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						SDEL_STRING(this->description);
						this->description = Text::String::New(sb->ToString(), sb->GetLength());
					}
					else if (node2->name->EqualsICase((const UTF8Char*)"link"))
					{
						Int32 linkType = 0;
						UOSInt k = 0;
						UOSInt l = node2->GetAttribCnt();
						while (k < l)
						{
							attr = node2->GetAttrib(k);
							if (attr->name->Equals((const UTF8Char*)"rel") && attr->value)
							{
								if (attr->value->Equals((const UTF8Char*)"self"))
								{
									linkType = 1;
								}
								else if (attr->value->Equals((const UTF8Char*)"alternate"))
								{
									linkType = 2;
								}
							}
							else if (attr->name->Equals((const UTF8Char*)"href") && attr->value)
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
					else if (node2->name->EqualsICase((const UTF8Char*)"ttl"))
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						this->ttl = Text::StrToInt32(sb->ToString());
					}
					else if (node2->name->EqualsICase((const UTF8Char*)"dc:language"))
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						SDEL_STRING(this->language);
						this->language = Text::String::New(sb->ToString(), sb->GetLength());
					}
					else if (node2->name->EqualsICase((const UTF8Char*)"author"))
					{
						UOSInt k;
						UOSInt l;
						Text::XMLNode *node3;
						k = 0;
						l = node2->GetChildCnt();
						while (k < l)
						{
							node3 = node2->GetChild(k);
							if (node3->GetNodeType() == Text::XMLNode::NT_ELEMENT && node3->name->Equals((const UTF8Char*)"name"))
							{
								sb->ClearStr();
								node3->GetInnerText(sb);
								SDEL_STRING(this->webMaster);
								this->webMaster = Text::String::New(sb->ToString(), sb->GetLength());
							}
							k++;
						}
					}
					else if (node2->name->EqualsICase((const UTF8Char*)"dc:rights"))
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						SDEL_STRING(this->copyright);
						this->copyright = Text::String::New(sb->ToString(), sb->GetLength());
					}
					else if (node2->name->EqualsICase((const UTF8Char*)"generator"))
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						SDEL_STRING(this->generator);
						this->generator = Text::String::New(sb->ToString(), sb->GetLength());
					}
					else if (node2->name->EqualsICase((const UTF8Char*)"published"))
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						SDEL_CLASS(this->pubDate);
						NEW_CLASS(this->pubDate, Data::DateTime());
						this->pubDate->SetValue(sb->ToString());
					}
					else if (node2->name->EqualsICase((const UTF8Char*)"lastBuildDate"))
					{
						sb->ClearStr();
						node2->GetInnerText(sb);
						SDEL_CLASS(this->lastBuildDate);
						NEW_CLASS(this->lastBuildDate, Data::DateTime());
						this->lastBuildDate->SetValue(sb->ToString());
					}
					else if (node2->name->EqualsICase((const UTF8Char*)"entry"))
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

void Net::RSS::GetYoutubeURL(const UTF8Char *channelId, Text::StringBuilderUTF *outURL)
{
	outURL->AppendC(UTF8STRC("https://www.youtube.com/feeds/videos.xml?channel_id="));
	outURL->Append(channelId);
}
