#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/RedisClient.h"
#include "Text/StringBuilderUTF8.h"
#include <hiredis/hiredis.h>

//#define VERBOSE

Bool DB::RedisClient::Connect(Text::CStringNN host, UInt16 port, Text::CString password, Int32 db)
{
	if (!this->context.IsNull())
	{
		redisFree((redisContext*)this->context.p);
		this->context = 0;
	}
	redisContext *ctx = redisConnect((const Char*)host.v.Ptr(), port);
	if (ctx == 0 || ctx->err)
	{
		if (ctx)
			redisFree(ctx);
		return false;
	}
	this->context = ctx;
	Text::CStringNN passwordNN;
	if (password.SetTo(passwordNN))
	{
		redisReply *reply = (redisReply*)this->SendAuth(passwordNN).p;
		if (reply == 0 || reply->type == REDIS_REPLY_ERROR)
		{
			if (reply)
				freeReplyObject(reply);
			redisFree(ctx);
			this->context = 0;
			return false;
		}
		freeReplyObject(reply);
	}
	if (db != 0)
	{
		redisReply *reply = (redisReply*)this->SendSelect(db).p;
		if (reply == 0 || reply->type == REDIS_REPLY_ERROR)
		{
			if (reply)
				freeReplyObject(reply);
			redisFree(ctx);
			this->context = 0;
			return false;
		}
		freeReplyObject(reply);
	}
	this->UpdateCateList();
	return true;
}

void DB::RedisClient::UpdateCateList()
{
	Data::ArrayListStringNN newCateList;
	Data::ArrayListStringNN newKeyList;
	redisReply *reply = (redisReply*)this->SendKeys(CSTR("*")).p;
	if (!reply)
		return;
	if (reply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t i = 0; i < reply->elements; i++)
		{
			redisReply *item = reply->element[i];
			if (item->type == REDIS_REPLY_STRING)
			{
				redisReply *typeReply = (redisReply*)this->SendType(Text::CStringNN((const UTF8Char*)item->str, item->len)).p;
				if (typeReply)
				{
					if (typeReply->type == REDIS_REPLY_STATUS)
					{
						if (Text::StrEqualsC((const UTF8Char*)typeReply->str, typeReply->len, UTF8STRC("hash")))
						{
							newCateList.Add(Text::String::New((const UTF8Char*)item->str, item->len));
						}
						else
						{
							newKeyList.Add(Text::String::New((const UTF8Char*)item->str, item->len));
						}
					}
					freeReplyObject(typeReply);
				}
			}
		}
	}
	freeReplyObject(reply);
	this->cateList.FreeAll();
	this->keyList.FreeAll();
	Data::Sort::ArtificialQuickSort::Sort<NN<Text::String>>(newCateList, newCateList);
	Data::Sort::ArtificialQuickSort::Sort<NN<Text::String>>(newKeyList, newKeyList);
	this->cateList.AddAll(newCateList);
	this->keyList.AddAll(newKeyList);
}

AnyType DB::RedisClient::SendAuth(Text::CString password) const
{
	UnsafeArray<const Char> argv[2] = {"AUTH", (const Char*)password.v.Ptr()};
	UIntOS argvlen[2] = {4, password.leng};
	return this->SendCommand(2, argv, argvlen);
}

AnyType DB::RedisClient::SendSelect(Int32 db) const
{
	UTF8Char dbStr[32];
	UnsafeArray<UTF8Char> dbEnd = Text::StrInt32(dbStr, db);
	UnsafeArray<const Char> argv[2] = {"SELECT", (const Char*)dbStr};
	UIntOS argvlen[2] = {6, (UIntOS)(dbEnd.Ptr() - dbStr)};
	return this->SendCommand(2, argv, argvlen);
}

AnyType DB::RedisClient::SendType(Text::CStringNN key) const
{
	UnsafeArray<const Char> argv[2] = {"TYPE", (const Char*)key.v.Ptr()};
	UIntOS argvlen[2] = {4, key.leng};
	return this->SendCommand(2, argv, argvlen);
}

AnyType DB::RedisClient::SendGet(Text::CStringNN key) const
{
	UnsafeArray<const Char> argv[2] = {"GET", (const Char*)key.v.Ptr()};
	UIntOS argvlen[2] = {3, key.leng};
	return this->SendCommand(2, argv, argvlen);
}

AnyType DB::RedisClient::SendSet(Text::CStringNN key, Text::CStringNN value) const
{
	UnsafeArray<const Char> argv[3] = {"SET", (const Char*)key.v.Ptr(), (const Char*)value.v.Ptr()};
	UIntOS argvlen[3] = {3, key.leng, value.leng};
	return this->SendCommand(3, argv, argvlen);
}

AnyType DB::RedisClient::SendDel(Text::CStringNN key) const
{
	UnsafeArray<const Char> argv[2] = {"DEL", (const Char*)key.v.Ptr()};
	UIntOS argvlen[2] = {3, key.leng};
	return this->SendCommand(2, argv, argvlen);
}

AnyType DB::RedisClient::SendKeys(Text::CStringNN pattern) const
{
	UnsafeArray<const Char> argv[2] = {"KEYS", (const Char*)pattern.v.Ptr()};
	UIntOS argvlen[2] = {4, pattern.leng};
	return this->SendCommand(2, argv, argvlen);
}

AnyType DB::RedisClient::SendHGet(Text::CStringNN key, Text::CStringNN field) const
{
	UnsafeArray<const Char> argv[3] = {"HGET", (const Char*)key.v.Ptr(), (const Char*)field.v.Ptr()};
	UIntOS argvlen[3] = {4, key.leng, field.leng};
	return this->SendCommand(3, argv, argvlen);
}

AnyType DB::RedisClient::SendHSet(Text::CStringNN key, Text::CStringNN field, Text::CStringNN value) const
{
	UnsafeArray<const Char> argv[4] = {"HSET", (const Char*)key.v.Ptr(), (const Char*)field.v.Ptr(), (const Char*)value.v.Ptr()};
	UIntOS argvlen[4] = {4, key.leng, field.leng, value.leng};
	return this->SendCommand(4, argv, argvlen);
}

AnyType DB::RedisClient::SendHDel(Text::CStringNN key, Text::CStringNN field) const
{
	UnsafeArray<const Char> argv[3] = {"HDEL", (const Char*)key.v.Ptr(), (const Char*)field.v.Ptr()};
	UIntOS argvlen[3] = {4, key.leng, field.leng};
	return this->SendCommand(3, argv, argvlen);
}

AnyType DB::RedisClient::SendHLen(Text::CStringNN key) const
{
	UnsafeArray<const Char> argv[2] = {"HLEN", (const Char*)key.v.Ptr()};
	UIntOS argvlen[2] = {4, key.leng};
	return this->SendCommand(2, argv, argvlen);
}

AnyType DB::RedisClient::SendHKeys(Text::CStringNN key) const
{
	UnsafeArray<const Char> argv[2] = {"HKEYS", (const Char*)key.v.Ptr()};
	UIntOS argvlen[2] = {5, key.leng};
	return this->SendCommand(2, argv, argvlen);
}

AnyType DB::RedisClient::SendCommand(Int32 argc, UnsafeArray<UnsafeArray<const Char>> argv, UnsafeArray<const UIntOS> argvlen) const
{
	if (this->context.IsNull() || ((redisContext*)this->context.p)->err)
		return 0;
#ifdef VERBOSE
	printf("RedisClient: Sending command");
	Int32 i = 0;
	while (i < argc)
	{
		printf(" %s", argv[i].Ptr());
		i++;
	}
	printf("\r\n");
#endif
	return redisCommandArgv((redisContext*)this->context.p, argc, (const Char**)argv.Ptr(), (const size_t*)argvlen.Ptr());
}

DB::RedisClient::RedisClient(Text::CStringNN host, UInt16 port) : IO::ConfigFile(CSTR("RedisClient"))
{
	this->context = 0;
	this->Connect(host, port, nullptr, 0);
}

DB::RedisClient::RedisClient(Text::CStringNN host, UInt16 port, Text::CString password, Int32 db) : IO::ConfigFile(CSTR("RedisClient"))
{
	this->context = 0;
	this->Connect(host, port, password, db);
}

DB::RedisClient::~RedisClient()
{
	if (!this->context.IsNull())
		redisFree((redisContext*)this->context.p);
	this->cateList.FreeAll();
	this->keyList.FreeAll();
}

Bool DB::RedisClient::IsConnected() const
{
	return !this->context.IsNull() && ((redisContext*)this->context.p)->err == 0;
}

Optional<Text::String> DB::RedisClient::GetCateValue(NN<Text::String> category, NN<Text::String> name)
{
	return this->GetCateValue(category->ToCString(), name->ToCString());
}

Optional<Text::String> DB::RedisClient::GetCateValue(Text::CStringNN category, Text::CStringNN name)
{
	if (category.leng == 0)
	{
		redisReply *typeReply = (redisReply*)this->SendType(name).p;
		if (!typeReply)
			return nullptr;
		redisReply *reply = 0;
		Optional<Text::String> ret = nullptr;
		if (Text::StrEqualsC((const UTF8Char*)typeReply->str, typeReply->len, UTF8STRC("string")))
		{
			reply = (redisReply*)this->SendGet(name).p;
			if (!reply)
			{
				freeReplyObject(typeReply);
				return nullptr;
			}
			if (reply->type == REDIS_REPLY_STRING)
			{
				ret = Text::String::New((const UTF8Char*)reply->str, reply->len);
			}
			else
			{
				printf("RedisClient: GetCateValue failed for category '%s' and name '%s', type=%d\n", category.v.Ptr(), name.v.Ptr(), reply->type);
			}
			freeReplyObject(reply);
		}
		else
		{
			printf("RedisClient: GetCateValue type for category '%s' and name '%s', type=%d, str=%s\n", category.v.Ptr(), name.v.Ptr(), typeReply->type, typeReply->str);
		}
		freeReplyObject(typeReply);
		return ret;
	}
	redisReply *reply = (redisReply*)this->SendHGet(category, name).p;
	if (!reply)
		return nullptr;
	Optional<Text::String> ret = nullptr;
	if (reply->type == REDIS_REPLY_STRING)
		ret = Text::String::New((const UTF8Char*)reply->str, reply->len);
	freeReplyObject(reply);
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
		redisReply *reply;
		if (value.SetTo(valueNN))
		{
			reply = (redisReply*)this->SendSet(name, valueNN).p;
		}
		else
		{
			reply = (redisReply*)this->SendDel(name).p;
		}
		if (!reply)
		{
			return false;
		}
		Bool success = reply->type != REDIS_REPLY_ERROR;
		freeReplyObject(reply);
		if (!success)
		{
			return false;
		}
		return true;
	}
	Text::CStringNN valueNN;
	redisReply *reply;
	if (value.SetTo(valueNN))
	{
		reply = (redisReply*)this->SendHSet(category, name, valueNN).p;
	}
	else
	{
		reply = (redisReply*)this->SendHDel(category, name).p;
	}
	if (!reply)
	{
		return false;
	}
	Bool success = reply->type != REDIS_REPLY_ERROR;
	freeReplyObject(reply);
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
		cateList->Add(this->cateList.GetItemNoCheck(i)->Clone());
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
			keyList->Add(this->keyList.GetItemNoCheck(i)->Clone());
			i++;
		}
		return j;
	}
	redisReply *reply = (redisReply*)this->SendHKeys(category).p;
	if (!reply)
		return 0;
	UIntOS count = 0;
	if (reply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t i = 0; i < reply->elements; i++)
		{
			redisReply *item = reply->element[i];
			if (item->type == REDIS_REPLY_STRING)
			{
				keyList->Add(Text::String::New((const UTF8Char*)item->str, item->len));
				count++;
			}
		}
	}
	freeReplyObject(reply);
	return count;
}

UIntOS DB::RedisClient::GetCount(Text::CString category) const
{
	Text::CStringNN categoryNN;
	if (!category.SetTo(categoryNN) || category.leng == 0)
	{
		return this->keyList.GetCount();
	}
	redisReply *reply = (redisReply*)this->SendHLen(categoryNN).p;
	if (!reply)
		return 0;
	UIntOS count = reply->type == REDIS_REPLY_INTEGER ? (UIntOS)reply->integer : 0;
	freeReplyObject(reply);
	return count;
}

Optional<Text::String> DB::RedisClient::GetKey(Text::CString category, UIntOS index) const
{
	Text::CStringNN categoryNN;
	if (!category.SetTo(categoryNN) || category.leng == 0)
	{
		return this->keyList.GetItem(index);
	}
	redisReply *reply = (redisReply*)this->SendHKeys(categoryNN).p;
	if (!reply)
		return nullptr;
	Optional<Text::String> ret = nullptr;
	if (reply->type == REDIS_REPLY_ARRAY && index < reply->elements)
	{
		redisReply *item = reply->element[index];
		if (item->type == REDIS_REPLY_STRING)
			ret = Text::String::New((const UTF8Char*)item->str, item->len);
	}
	freeReplyObject(reply);
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