#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/RedisClient.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"

//#define VERBOSE

Bool DB::RedisClient::ConnInit(Text::CString password, Int32 db)
{
	if (!this->redis.IsConnected())
	{
		return false;
	}
	Text::CStringNN passwordNN;
	if (password.SetTo(passwordNN))
	{
		NN<DB::RedisConn::ReplyInfo> reply;;
		if (!this->redis.SendAuth(passwordNN).SetTo(reply)) return false;
		if (reply->replyType == DB::RedisConn::ReplyType::Error)
		{
			DB::RedisConn::FreeReplyData(reply);
			return false;
		}
		DB::RedisConn::FreeReplyData(reply);
	}
	if (db != 0)
	{
		NN<DB::RedisConn::ReplyInfo> reply;
		if (!this->redis.SendSelect(db).SetTo(reply)) return false;
		if (reply->replyType == DB::RedisConn::ReplyType::Error)
		{
			DB::RedisConn::FreeReplyData(reply);
			return false;
		}
		DB::RedisConn::FreeReplyData(reply);
	}
	this->UpdateCateList();
	return true;
}

void DB::RedisClient::UpdateCateList()
{
	Data::ArrayListStringNN newCateList;
	Data::ArrayListStringNN newKeyList;
	NN<DB::RedisConn::ReplyInfo> reply;
	if (!this->redis.SendKeys(CSTR("*")).SetTo(reply))
		return;
	if (reply->replyType == DB::RedisConn::ReplyType::Array)
	{
		UIntOS i = 0;
		while (i < reply->arr.size)
		{
			NN<DB::RedisConn::ReplyData> item = reply->arr.items[i];
			if (item->type == DB::RedisConn::DataType::String)
			{
				NN<DB::RedisConn::ReplyInfo> typeReply;
				if (this->redis.SendType(item->str->ToCString()).SetTo(typeReply))
				{
					if (typeReply->replyType == DB::RedisConn::ReplyType::Status && typeReply->type == DB::RedisConn::DataType::String)
					{
						if (typeReply->str->Equals(UTF8STRC("hash")))
						{
							newCateList.Add(item->str->Clone());
						}
						else
						{
							newKeyList.Add(item->str->Clone());
						}
					}
					DB::RedisConn::FreeReplyData(typeReply);
				}
			}
			i++;
		}
	}
	DB::RedisConn::FreeReplyData(reply);
	this->cateList.FreeAll();
	this->keyList.FreeAll();
	Data::Sort::ArtificialQuickSort::Sort<NN<Text::String>>(newCateList, newCateList);
	Data::Sort::ArtificialQuickSort::Sort<NN<Text::String>>(newKeyList, newKeyList);
	this->cateList.AddAll(newCateList);
	this->keyList.AddAll(newKeyList);
}

DB::RedisClient::RedisClient(NN<Net::TCPClientFactory> clif, Text::CStringNN host, UInt16 port) : IO::ConfigFile(CSTR("RedisClient")), redis(clif, host, port)
{
	this->lastVal = nullptr;
	this->ConnInit(nullptr, 0);
}

DB::RedisClient::RedisClient(NN<Net::TCPClientFactory> clif, Text::CStringNN host, UInt16 port, Text::CString password, Int32 db) : IO::ConfigFile(CSTR("RedisClient")), redis(clif, host, port)
{
	this->lastVal = nullptr;
	this->ConnInit(password, db);
}

DB::RedisClient::~RedisClient()
{
	this->cateList.FreeAll();
	this->keyList.FreeAll();
	OPTSTR_DEL(this->lastVal);
	this->lastKeyList.FreeAll();
}

Bool DB::RedisClient::IsConnected() const
{
	return this->redis.IsConnected();
}

Optional<Text::String> DB::RedisClient::GetCateValue(NN<Text::String> category, NN<Text::String> name)
{
	return this->GetCateValue(category->ToCString(), name->ToCString());
}

Optional<Text::String> DB::RedisClient::GetCateValue(Text::CStringNN category, Text::CStringNN name)
{
	if (category.leng == 0)
	{
		NN<DB::RedisConn::ReplyInfo> typeReply;
		if (!this->redis.SendType(name).SetTo(typeReply))
			return nullptr;
		NN<DB::RedisConn::ReplyInfo> reply;
		Optional<Text::String> ret = nullptr;
		if (typeReply->type == DB::RedisConn::DataType::String && typeReply->str->Equals(UTF8STRC("string")))
		{
			if (!this->redis.SendGet(name).SetTo(reply))
			{
				DB::RedisConn::FreeReplyData(typeReply);
				return nullptr;
			}
			if (reply->replyType == DB::RedisConn::ReplyType::String && reply->type == DB::RedisConn::DataType::String)
			{
				if (Text::StringTool::IsTextUTF8(reply->str->ToByteArray()))
					ret = reply->str->Clone();
				else
				{
					Text::StringBuilderUTF8 sbRet;
					sbRet.Append(CSTR("0x"));
					sbRet.AppendHexBuff(reply->str->ToByteArray(), 0, Text::LineBreakType::None);
					ret = Text::String::New(sbRet.ToCString());
				}
			}
			else
			{
				printf("RedisClient: GetCateValue failed for category '%s' and name '%s', type=%d\n", category.v.Ptr(), name.v.Ptr(), (UInt32)reply->type);
			}
			DB::RedisConn::FreeReplyData(reply);
		}
		else if (typeReply->type == DB::RedisConn::DataType::String && typeReply->str->Equals(UTF8STRC("set")))
		{
			if (!this->redis.SendSMembers(name).SetTo(reply))
			{
				DB::RedisConn::FreeReplyData(typeReply);
				return nullptr;
			}
			if (reply->replyType == DB::RedisConn::ReplyType::Array && reply->type == DB::RedisConn::DataType::Array)
			{
				Text::StringBuilderUTF8 sbRet;
				UIntOS i = 0;
				UIntOS cnt = reply->arr.size;
				sbRet.AppendUTF8Char('[');
				while (i < cnt)
				{
					if (reply->arr.items[i].type == DB::RedisConn::DataType::String)
					{
						if (i > 0)
							sbRet.AppendUTF8Char(',');
						if (Text::StringTool::IsTextUTF8(reply->arr.items[i].str->ToByteArray()))
						{
							sbRet.AppendUTF8Char('\"');
							sbRet.Append(reply->arr.items[i].str);
							sbRet.AppendUTF8Char('\"');
						}
						else
						{
							sbRet.Append(CSTR("0x"));
							sbRet.AppendHexBuff(reply->arr.items[i].str->ToByteArray(), 0, Text::LineBreakType::None);
						}
					}
					i++;
				}
				sbRet.AppendUTF8Char(']');
				ret = Text::String::New(sbRet.ToCString());
			}
			else
			{
				printf("RedisClient: GetCateValue failed for category '%s' and name '%s', type=%d\n", category.v.Ptr(), name.v.Ptr(), (UInt32)reply->type);
			}
			DB::RedisConn::FreeReplyData(reply);
		}
		else if (typeReply->type == DB::RedisConn::DataType::String && typeReply->str->Equals(UTF8STRC("stream")))
		{
			if (!this->redis.SendXRange(name, 0, 0, 0).SetTo(reply))
			{
				DB::RedisConn::FreeReplyData(typeReply);
				return nullptr;
			}
			if (reply->replyType == DB::RedisConn::ReplyType::Array && reply->type == DB::RedisConn::DataType::Array)
			{
				Text::StringBuilderUTF8 sbRet;
				UIntOS i = 0;
				UIntOS cnt = reply->arr.size;
				sbRet.AppendUTF8Char('{');
				while (i < cnt)
				{
					if (reply->arr.items[i].type == DB::RedisConn::DataType::String)
					{
						if (i > 0)
							sbRet.AppendUTF8Char(',');
						if (Text::StringTool::IsTextUTF8(reply->arr.items[i].str->ToByteArray()))
						{
							sbRet.AppendUTF8Char('\"');
							sbRet.Append(reply->arr.items[i].str);
							sbRet.AppendUTF8Char('\"');
						}
						else
						{
							sbRet.Append(CSTR("0x"));
							sbRet.AppendHexBuff(reply->arr.items[i].str->ToByteArray(), 0, Text::LineBreakType::None);
						}
					}
					else if (reply->arr.items[i].type == DB::RedisConn::DataType::Array)
					{
						if (i > 0)
						{
							sbRet.Append(CSTR(",\r\n"));
						}
						UIntOS j = 0;
						UIntOS nestedCnt = reply->arr.items[i].arr.size;
						while (j < nestedCnt)
						{
							if (reply->arr.items[i].arr.items[j].type == DB::RedisConn::DataType::String)
							{
								if (j > 0)
									sbRet.AppendUTF8Char(',');
								if (Text::StringTool::IsTextUTF8(reply->arr.items[i].arr.items[j].str->ToByteArray()))
								{
									sbRet.AppendUTF8Char('\"');
									sbRet.Append(reply->arr.items[i].arr.items[j].str);
									sbRet.AppendUTF8Char('\"');
								}
								else
								{
									sbRet.Append(CSTR("0x"));
									sbRet.AppendHexBuff(reply->arr.items[i].arr.items[j].str->ToByteArray(), 0, Text::LineBreakType::None);
								}
							}
							else if (reply->arr.items[i].arr.items[j].type == DB::RedisConn::DataType::Array)
							{
								if (j > 0)
									sbRet.AppendUTF8Char(':');
								sbRet.AppendUTF8Char('{');
								UIntOS k = 0;
								UIntOS nestedNestedCnt = reply->arr.items[i].arr.items[j].arr.size;
								while (k < nestedNestedCnt)
								{
									if (reply->arr.items[i].arr.items[j].arr.items[k].type == DB::RedisConn::DataType::String)
									{
										if (k > 0)
											sbRet.AppendUTF8Char(',');
										if (Text::StringTool::IsTextUTF8(reply->arr.items[i].arr.items[j].arr.items[k].str->ToByteArray()))
										{
											sbRet.AppendUTF8Char('\"');
											sbRet.Append(reply->arr.items[i].arr.items[j].arr.items[k].str);
											sbRet.AppendUTF8Char('\"');
										}
										else
										{
											sbRet.Append(CSTR("0x"));
											sbRet.AppendHexBuff(reply->arr.items[i].arr.items[j].arr.items[k].str->ToByteArray(), 0, Text::LineBreakType::None);
										}
									}
									else
									{
										printf("RedisClient: GetCateValue unexpected nested nested item type for category '%s' and name '%s', type=%d\n", category.v.Ptr(), name.v.Ptr(), (UInt32)reply->arr.items[i].arr.items[j].arr.items[k].type);
									}
									k++;
								}
								sbRet.AppendUTF8Char('}');
							}
							else
							{
								printf("RedisClient: GetCateValue unexpected nested item type for category '%s' and name '%s', type=%d\n", category.v.Ptr(), name.v.Ptr(), (UInt32)reply->arr.items[i].arr.items[j].type);
							}
							j++;
						}
					}
					else
					{
						printf("RedisClient: GetCateValue unexpected item type for category '%s' and name '%s', type=%d\n", category.v.Ptr(), name.v.Ptr(), (UInt32)reply->arr.items[i].type);
					}
					i++;
				}
				sbRet.AppendUTF8Char('}');
				ret = Text::String::New(sbRet.ToCString());
			}
			else
			{
				printf("RedisClient: GetCateValue failed for category '%s' and name '%s', type=%d\n", category.v.Ptr(), name.v.Ptr(), (UInt32)reply->type);
			}
			DB::RedisConn::FreeReplyData(reply);
		}
		else if (typeReply->type == DB::RedisConn::DataType::String && typeReply->str->Equals(CSTR("none")))
		{
			ret = nullptr;
		}
		else
		{
			printf("RedisClient: GetCateValue type for category '%s' and name '%s', type=%d, str=%s\n", category.v.Ptr(), name.v.Ptr(), (UInt32)typeReply->type, typeReply->str->v.Ptr());
		}
		DB::RedisConn::FreeReplyData(typeReply);
		OPTSTR_DEL(this->lastVal);
		this->lastVal = ret;
		return ret;
	}
	NN<DB::RedisConn::ReplyInfo> reply;
	if (!this->redis.SendHGet(category, name).SetTo(reply))
		return nullptr;
	Optional<Text::String> ret = nullptr;
	if (reply->replyType == DB::RedisConn::ReplyType::String && reply->type == DB::RedisConn::DataType::String)
		ret = reply->str->Clone();
	DB::RedisConn::FreeReplyData(reply);
	OPTSTR_DEL(this->lastVal);
	this->lastVal = ret;
	return ret;
}

Bool DB::RedisClient::SetValue(NN<Text::String> category, NN<Text::String> name, Optional<Text::String> value)
{
	NN<Text::String> valueNN;
	if (value.SetTo(valueNN))
		return this->SetValue(category->ToCString(), name->ToCString(), valueNN->ToCString());
	return this->SetValue(category->ToCString(), name->ToCString(), nullptr);
}

Bool DB::RedisClient::SetValue(Text::CStringNN category, Text::CStringNN name, Text::CString value)
{
	if (category.leng == 0)
	{
		Text::CStringNN valueNN;
		Optional<DB::RedisConn::ReplyInfo> optreply;
		NN<DB::RedisConn::ReplyInfo> reply;
		if (value.SetTo(valueNN))
		{
			optreply = this->redis.SendSet(name, valueNN);
		}
		else
		{
			optreply = this->redis.SendDel(name);
		}
		if (!optreply.SetTo(reply))
		{
			return false;
		}
		Bool success = reply->replyType != DB::RedisConn::ReplyType::Error;
		DB::RedisConn::FreeReplyData(reply);
		if (!success)
		{
			return false;
		}
		return true;
	}
	Text::CStringNN valueNN;
	Optional<DB::RedisConn::ReplyInfo> optreply;
	NN<DB::RedisConn::ReplyInfo> reply;
	if (value.SetTo(valueNN))
	{
		optreply = this->redis.SendHSet(category, name, valueNN);
	}
	else
	{
		optreply = this->redis.SendHDel(category, name);
	}
	if (!optreply.SetTo(reply))
	{
		return false;
	}
	Bool success = reply->replyType != DB::RedisConn::ReplyType::Error;
	DB::RedisConn::FreeReplyData(reply);
	if (!success)
	{
		return false;
	}
	return success;
}

Bool DB::RedisClient::RemoveValue(Text::CString category, Text::CStringNN name)
{
	return this->SetValue(category.OrEmpty(), name, nullptr);
}

UIntOS DB::RedisClient::GetCateCount() const
{
	return this->cateList.GetCount();
}

UIntOS DB::RedisClient::GetCateList(NN<Data::ArrayListStringNN> cateList, Bool withEmpty)
{
	UIntOS i;
	UIntOS j;
	if (withEmpty)
	{
		cateList->Add(Text::String::NewEmpty());
	}
	i = 0;
	j = this->cateList.GetCount();
	while (i < j)
	{
		cateList->Add(this->cateList.GetItemNoCheck(i));
		i++;
	}
	if (withEmpty)
	{
		j++;
	}
	return j;
}

UIntOS DB::RedisClient::GetKeys(NN<Text::String> category, NN<Data::ArrayListStringNN> keyList)
{
	return this->GetKeys(category->ToCString(), keyList);
}

UIntOS DB::RedisClient::GetKeys(Text::CStringNN category, NN<Data::ArrayListStringNN> keyList)
{
	if (category.leng == 0)
	{
		UIntOS i = 0;
		UIntOS j = this->keyList.GetCount();
		while (i < j)
		{
			keyList->Add(this->keyList.GetItemNoCheck(i));
			i++;
		}
		return j;
	}
	NN<DB::RedisConn::ReplyInfo> reply;
	if (!this->redis.SendHKeys(category).SetTo(reply))
		return 0;
	this->lastKeyList.FreeAll();
	Data::ArrayListStringNN lastKeyList;
	if (reply->type == DB::RedisConn::DataType::Array)
	{
		UIntOS i = 0;
		while (i < reply->arr.size)
		{
			NN<DB::RedisConn::ReplyData> item = reply->arr.items[i];
			if (item->type == DB::RedisConn::DataType::String)
			{
				lastKeyList.Add(item->str->Clone());
			}
			i++;
		}
	}
	DB::RedisConn::FreeReplyData(reply);
	this->lastKeyList.AddAll(lastKeyList);
	keyList->AddAll(lastKeyList);
	return lastKeyList.GetCount();
}

UIntOS DB::RedisClient::GetCount(Text::CString category) const
{
	Text::CStringNN categoryNN;
	if (!category.SetTo(categoryNN) || category.leng == 0)
	{
		return this->keyList.GetCount();
	}
	NN<DB::RedisConn::ReplyInfo> reply;
	if (!this->redis.SendHLen(categoryNN).SetTo(reply))
		return 0;
	UIntOS count = reply->type == DB::RedisConn::DataType::Integer ? (UIntOS)reply->integer : 0;
	DB::RedisConn::FreeReplyData(reply);
	return count;
}

Optional<Text::String> DB::RedisClient::GetKey(Text::CString category, UIntOS index) const
{
	Text::CStringNN categoryNN;
	if (!category.SetTo(categoryNN) || category.leng == 0)
	{
		return this->keyList.GetItem(index);
	}
	NN<DB::RedisConn::ReplyInfo> reply;
	if (!this->redis.SendHKeys(categoryNN).SetTo(reply))
		return nullptr;
	Optional<Text::String> ret = nullptr;
	if (reply->type == DB::RedisConn::DataType::Array && index < reply->arr.size)
	{
		NN<DB::RedisConn::ReplyData> item = reply->arr.items[index];
		if (item->type == DB::RedisConn::DataType::String)
			ret = item->str->Clone();
	}
	DB::RedisConn::FreeReplyData(reply);
	DB::RedisClient* me = (DB::RedisClient*)this;
	OPTSTR_DEL(me->lastVal);
	me->lastVal = ret;
	return ret;
}

Bool DB::RedisClient::HasCategory(Text::CString category) const
{
	Text::CStringNN categoryNN;
	if (!category.SetTo(categoryNN))
		return true;
	if (categoryNN.leng == 0)
		return true;
	if (this->cateList.SortedIndexOfC(categoryNN) >= 0)
		return true;
	return false;
}