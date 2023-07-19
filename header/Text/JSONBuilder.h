#ifndef _SM_TEXT_JSONBUILDER
#define _SM_TEXT_JSONBUILDER
#include "Data/ArrayList.h"
#include "Text/JSON.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class JSONBuilder
	{
	public:
		typedef enum
		{
			OT_OBJECT,
			OT_ARRAY
		} ObjectType;
	private:
		NotNullPtr<Text::StringBuilderUTF8> sb;
		Data::ArrayList<ObjectType> objTypes;
		ObjectType currType;
		Bool isFirst;

		void AppendStr(Text::CString val);
		void AppendStrUTF8(const UTF8Char *val);
		void AppendStrW(const WChar *val);
	public:
		JSONBuilder(NotNullPtr<Text::StringBuilderUTF8> sb, ObjectType rootType);
		~JSONBuilder();

		Bool ArrayAddInt32(Int32 val);
		Bool ArrayAddInt64(Int64 val);
		Bool ArrayAddFloat64(Double val);
		Bool ArrayAddBool(Bool val);
		Bool ArrayAddStr(Text::PString *val);
		Bool ArrayAddStrUTF8(const UTF8Char *val);
		Bool ArrayAddNull();
		Bool ArrayAdd(Text::JSONArray *arr);
		Bool ArrayBeginObject();
		Bool ArrayBeginArray();
		Bool ArrayEnd();

		Bool ObjectAddFloat64(Text::CString name, Double val);
		Bool ObjectAddInt32(Text::CString name, Int32 val);
		Bool ObjectAddInt64(Text::CString name, Int64 val);
		Bool ObjectAddUInt64(Text::CString name, UInt64 val);
		Bool ObjectAddBool(Text::CString name, Bool val);
		Bool ObjectAddStr(Text::CString name, Text::PString *val);
		Bool ObjectAddStr(Text::CString name, NotNullPtr<Text::String> val);
		Bool ObjectAddStr(Text::CString name, Text::CString val);
		Bool ObjectAddStrUTF8(Text::CString name, const UTF8Char *val);
		Bool ObjectAddStrW(Text::CString name, const WChar *val);
		Bool ObjectAddNull(Text::CString name);
		Bool ObjectAdd(Text::JSONObject *obj);
		Bool ObjectBeginArray(Text::CString name);
		Bool ObjectBeginObject(Text::CString name);
		Bool ObjectEnd();
	};
}
#endif
