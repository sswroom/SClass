#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "IO/FileStream.h"
#include "Net/HKTrafficImage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLDOM.h"

void Net::HKTrafficImage::Init(NotNullPtr<Text::EncodingFactory> encFact, const UInt8 *buff, UOSInt buffSize)
{
	Text::XMLNode *node1;
	Text::XMLNode *node2;
	Text::XMLNode *node3;
	NotNullPtr<GroupInfo> grp;
	NotNullPtr<ImageInfo> img;
	Text::XMLDocument doc;
	if (doc.ParseBuff(encFact, buff, buffSize))
	{
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		Text::StringBuilderUTF8 sbKey;
		Text::StringBuilderUTF8 sbRegion;
		Text::StringBuilderUTF8 sbDesc;
		Text::StringBuilderUTF8 sbURL;
		Text::StringBuilderUTF8 sb;
		Double lat;
		Double lon;

		i = doc.GetChildCnt();
		while (i-- > 0)
		{
			node1 = doc.GetChild(i);
			if (node1->GetNodeType() == Text::XMLNode::NodeType::Element && node1->name->EqualsICase(UTF8STRC("image-list")))
			{
				j = 0;
				k = node1->GetChildCnt();
				while (j < k)
				{
					node2 = node1->GetChild(j);
					if (node2->GetNodeType() == Text::XMLNode::NodeType::Element && node2->name->EqualsICase(UTF8STRC("image")))
					{
						sbKey.ClearStr();
						sbRegion.ClearStr();
						sbDesc.ClearStr();
						sbURL.ClearStr();
						lat = 0;
						lon = 0;
						l = node2->GetChildCnt();
						while (l-- > 0)
						{
							node3 = node2->GetChild(l);
							if (node3->GetNodeType() == Text::XMLNode::NodeType::Element)
							{
								if (node3->name->EqualsICase(UTF8STRC("key")))
								{
									node3->GetInnerText(sbKey);
								}
								else if (node3->name->EqualsICase(UTF8STRC("region")))
								{
									node3->GetInnerText(sbRegion);
								}
								else if (node3->name->EqualsICase(UTF8STRC("description")))
								{
									node3->GetInnerText(sbDesc);
								}
								else if (node3->name->EqualsICase(UTF8STRC("latitude")))
								{
									sb.ClearStr();
									node3->GetInnerText(sb);
									lat = Text::StrToDouble(sb.ToString());
								}
								else if (node3->name->EqualsICase(UTF8STRC("longitude")))
								{
									sb.ClearStr();
									node3->GetInnerText(sb);
									lon = Text::StrToDouble(sb.ToString());
								}
								else if (node3->name->EqualsICase(UTF8STRC("url")))
								{
									node3->GetInnerText(sbURL);
								}
							}
						}

						if (lat != 0 && lon != 0 && sbKey.GetLength() > 0 && sbRegion.GetLength() > 0 && sbDesc.GetLength() > 0 && sbURL.GetLength() > 0)
						{
							if (!this->groupMap.GetC(sbRegion.ToCString()).SetTo(grp))
							{
								grp = MemAllocNN(GroupInfo);
								grp->groupName = Text::String::New(sbRegion.ToString(), sbRegion.GetLength());
								NEW_CLASS(grp->imageList, Data::ArrayListNN<ImageInfo>());
								this->groupMap.PutNN(grp->groupName, grp);
							}

							img = MemAllocNN(ImageInfo);
							img->key = Text::String::New(sbKey.ToString(), sbKey.GetLength());
							img->addr = Text::String::New(sbDesc.ToString(), sbDesc.GetLength());
							img->lat = lat;
							img->lon = lon;
							img->url = Text::String::New(sbURL.ToString(), sbURL.GetLength());
							grp->imageList->Add(img);
						}
					}
					j++;
				}
			}
		}
	}
}

Net::HKTrafficImage::HKTrafficImage(NotNullPtr<Text::EncodingFactory> encFact, const UInt8 *buff, UOSInt buffSize)
{
	this->Init(encFact, buff, buffSize);
}

Net::HKTrafficImage::HKTrafficImage(NotNullPtr<Text::EncodingFactory> encFact, Text::CStringNN fileName)
{
	UInt64 fileSize;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	fileSize = fs.GetLength();
	if (fileSize > 0)
	{
		Data::ByteBuffer buff(fileSize);
		if (fs.Read(buff) == fileSize)
		{
			this->Init(encFact, buff.Ptr(), (UOSInt)fileSize);
		}
	}
}

Net::HKTrafficImage::~HKTrafficImage()
{
	NotNullPtr<GroupInfo> grp;
	NotNullPtr<ImageInfo> img;
	UOSInt i;
	UOSInt j;
	i = this->groupMap.GetCount();
	while (i-- > 0)
	{
		grp = this->groupMap.GetItemNoCheck(i);
		j = grp->imageList->GetCount();
		while (j-- > 0)
		{
			img = grp->imageList->GetItemNoCheck(j);
			img->key->Release();
			img->addr->Release();
			img->url->Release();
			MemFreeNN(img);
		}
		DEL_CLASS(grp->imageList);
		grp->groupName->Release();
		MemFreeNN(grp);
	}
}

UOSInt Net::HKTrafficImage::GetGroups(NotNullPtr<Data::ArrayListNN<Net::HKTrafficImage::GroupInfo>> groups)
{
	return groups->AddAll(this->groupMap);
}
