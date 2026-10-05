#ifndef _SM_DB_REDISCLIENT
#define _SM_DB_REDISCLIENT
#include "AnyType.h"
#include "IO/ConfigFile.h"

namespace DB
{
	class RedisClient : public IO::ConfigFile
	{
	private:
		AnyType context;
		Data::ArrayListStringNN cateList;
		Data::ArrayListStringNN keyList;

		Bool Connect(Text::CStringNN host, UInt16 port, Text::CString password, Int32 db);
		void UpdateCateList();

		AnyType SendAuth(Text::CString password) const;
		AnyType SendSelect(Int32 db) const;
		AnyType SendType(Text::CStringNN key) const;
		AnyType SendGet(Text::CStringNN key) const;
		AnyType SendSet(Text::CStringNN key, Text::CStringNN value) const;
		AnyType SendDel(Text::CStringNN key) const;
		AnyType SendKeys(Text::CStringNN pattern) const;
		AnyType SendHGet(Text::CStringNN key, Text::CStringNN field) const;
		AnyType SendHSet(Text::CStringNN key, Text::CStringNN field, Text::CStringNN value) const;
		AnyType SendHDel(Text::CStringNN key, Text::CStringNN field) const;
		AnyType SendHLen(Text::CStringNN key) const;
		AnyType SendHKeys(Text::CStringNN key) const;
		AnyType SendCommand(Int32 argc, UnsafeArray<UnsafeArray<const Char>> argv, UnsafeArray<const UIntOS> argvlen) const;

	public:
		RedisClient(Text::CStringNN host, UInt16 port);
		RedisClient(Text::CStringNN host, UInt16 port, Text::CString password, Int32 db);
		virtual ~RedisClient();

		Bool IsConnected() const;
		virtual Optional<Text::String> GetCateValue(NN<Text::String> category, NN<Text::String> name);
		virtual Optional<Text::String> GetCateValue(Text::CStringNN category, Text::CStringNN name);
		virtual Bool SetValue(NN<Text::String> category, NN<Text::String> name, Optional<Text::String> value);
		virtual Bool SetValue(Text::CStringNN category, Text::CStringNN name, Text::CString value);
		virtual Bool RemoveValue(Text::CString category, Text::CStringNN name);
		virtual UIntOS GetCateCount() const;
		virtual UIntOS GetCateList(NN<Data::ArrayListStringNN> cateList, Bool withEmpty);
		virtual UIntOS GetKeys(NN<Text::String> category, NN<Data::ArrayListStringNN> keyList);
		virtual UIntOS GetKeys(Text::CStringNN category, NN<Data::ArrayListStringNN> keyList);
		virtual UIntOS GetCount(Text::CString category) const;
		virtual Optional<Text::String> GetKey(Text::CString category, UIntOS index) const;
		virtual Bool HasCategory(Text::CString category) const;
	};
}
#endif