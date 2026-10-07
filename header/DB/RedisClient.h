#ifndef _SM_DB_REDISCLIENT
#define _SM_DB_REDISCLIENT
#include "DB/RedisConn.h"
#include "IO/ConfigFile.h"
#include "Net/TCPClientFactory.h"

namespace DB
{
	class RedisClient : public IO::ConfigFile
	{
	private:
		DB::RedisConn redis;
		Optional<Text::String> lastVal;
		Data::ArrayListStringNN lastKeyList;
		Data::ArrayListStringNN cateList;
		Data::ArrayListStringNN keyList;

		Bool ConnInit(Text::CString password, Int32 db);
		void UpdateCateList();
	public:
		RedisClient(NN<Net::TCPClientFactory> clif, Text::CStringNN host, UInt16 port);
		RedisClient(NN<Net::TCPClientFactory> clif, Text::CStringNN host, UInt16 port, Text::CString password, Int32 db);
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