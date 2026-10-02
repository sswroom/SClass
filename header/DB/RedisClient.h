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
		NN<Text::String> keyPrefix;

		Bool Connect(Text::CStringNN host, UInt16 port, Text::CString password, Int32 db);
		Optional<Text::String> GetCategoryKey(Text::CString category) const;
		Optional<Text::String> GetCategoriesKey() const;
		Optional<Text::String> GetDefaultKey(Text::CStringNN name) const;
		Optional<Text::String> GetDefaultKeysKey() const;
		AnyType SendCommand(Int32 argc, UnsafeArray<UnsafeArray<const Char>> argv, UnsafeArray<const UIntOS> argvlen) const;

	public:
		RedisClient(Text::CStringNN host, UInt16 port);
		RedisClient(Text::CStringNN host, UInt16 port, Text::CString password, Int32 db, Text::CStringNN keyPrefix);
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