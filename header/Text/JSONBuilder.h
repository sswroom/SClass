#ifndef _SM_TEXT_JSONBUILDER
#define _SM_TEXT_JSONBUILDER
#include "Data/ArrayList.h"
#include "Text/StringBuilderUTF.h"

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
		Text::StringBuilderUTF *sb;
		Data::ArrayList<ObjectType> *objTypes;
		ObjectType currType;
		Bool isFirst;

		void AppendStrUTF8(const UTF8Char *val);
		void AppendStrW(const WChar *val);
	public:
		JSONBuilder(Text::StringBuilderUTF *sb, ObjectType rootType);
		~JSONBuilder();

		Bool ArrayAddFloat64(Double val);
		Bool ArrayBeginObject();
		Bool ArrayEnd();

		Bool ObjectAddFloat64(const UTF8Char *name, Double val);
		Bool ObjectAddInt32(const UTF8Char *name, Int32 val);
		Bool ObjectAddInt64(const UTF8Char *name, Int64 val);
		Bool ObjectAddStrUTF8(const UTF8Char *name, const UTF8Char *val);
		Bool ObjectAddStrW(const UTF8Char *name, const WChar *val);
		Bool ObjectBeginArray(const UTF8Char *name);
		Bool ObjectBeginObject(const UTF8Char *name);
		Bool ObjectEnd();
	};
}
#endif
