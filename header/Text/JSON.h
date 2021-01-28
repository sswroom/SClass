#ifndef _SM_TEXT_JSON
#define _SM_TEXT_JSON
#include "Data/StringUTF8Map.h"
#include "Text/StringBuilderUTF.h"

namespace Text
{
	class JSONBase
	{
	public:
		typedef enum
		{
			JST_OBJECT,
			JST_ARRAY,
			JST_NUMBER,
			JST_STRINGUTF8,
			JST_BOOL,
			JST_NULL,
			JST_INT32,
			JST_INT64,
			JST_STRINGWO
		} JSType;
	private:
		OSInt useCnt;

	protected:
		JSONBase();
	protected:
		virtual ~JSONBase();

	public:
		virtual JSType GetJSType() = 0;
		virtual void ToJSONString(Text::StringBuilderUTF *sb) = 0;
		virtual Bool Equals(const UTF8Char *s) = 0;
		virtual Bool Identical(JSONBase *obj) = 0;
		void BeginUse();
		void EndUse();
		static JSONBase *ParseJSONStr(const UTF8Char *jsonStr);
		static JSONBase *ParseJSONStrLen(const UTF8Char *jsonStr, OSInt strLen);

	private:
		static const UTF8Char *ClearWS(const UTF8Char *jsonStr);
		static const UTF8Char *ParseJSString(const UTF8Char *jsonStr, Text::StringBuilderUTF *sb);
		static const UTF8Char *ParseJSNumber(const UTF8Char *jsonStr, Double *val);
		static JSONBase *ParseJSONStr2(const UTF8Char *jsonStr, const UTF8Char **jsonStrEnd);
	};

	class JSONNumber : public JSONBase
	{
	private:
		Double val;

	public:
		JSONNumber(Double val);
	private:
		virtual ~JSONNumber();

	public:
		virtual JSType GetJSType();
		virtual void ToJSONString(Text::StringBuilderUTF *sb);
		virtual Bool Equals(const UTF8Char *s);
		virtual Bool Identical(JSONBase *obj);
		Double GetValue();
	};

	class JSONInt32 : public JSONBase
	{
	private:
		Int32 val;

	public:
		JSONInt32(Int32 val);
	private:
		virtual ~JSONInt32();

	public:
		virtual JSType GetJSType();
		virtual void ToJSONString(Text::StringBuilderUTF *sb);
		virtual Bool Equals(const UTF8Char *s);
		virtual Bool Identical(JSONBase *obj);
		Int32 GetValue();
	};

	class JSONInt64 : public JSONBase
	{
	private:
		Int64 val;

	public:
		JSONInt64(Int64 val);
	private:
		virtual ~JSONInt64();

	public:
		virtual JSType GetJSType();
		virtual void ToJSONString(Text::StringBuilderUTF *sb);
		virtual Bool Equals(const UTF8Char *s);
		virtual Bool Identical(JSONBase *obj);
		Int64 GetValue();
	};

	class JSONStringUTF8 : public JSONBase
	{
	private:
		const UTF8Char *val;

	public:
		JSONStringUTF8(const UTF8Char *val);
	private:
		virtual ~JSONStringUTF8();

	public:
		virtual JSType GetJSType();
		virtual void ToJSONString(Text::StringBuilderUTF *sb);
		virtual Bool Equals(const UTF8Char *s);
		virtual Bool Identical(JSONBase *obj);
		const UTF8Char *GetValue();
	};

	class JSONStringW : public JSONBase
	{
	private:
		const WChar *val;

	public:
		JSONStringW(const WChar *val);
	private:
		virtual ~JSONStringW();

	public:
		virtual JSType GetJSType();
		virtual void ToJSONString(Text::StringBuilderUTF *sb);
		virtual Bool Equals(const UTF8Char *s);
		virtual Bool Identical(JSONBase *obj);
		const WChar *GetValue();
	};

	class JSONBool : public JSONBase
	{
	private:
		Bool val;

	public:
		JSONBool(Bool val);
	private:
		virtual ~JSONBool();

	public:
		virtual JSType GetJSType();
		virtual void ToJSONString(Text::StringBuilderUTF *sb);
		virtual Bool Equals(const UTF8Char *s);
		virtual Bool Identical(JSONBase *obj);
		Bool GetValue();
	};

	class JSONObject : public JSONBase
	{
	private:
		Data::StringUTF8Map<Text::JSONBase *> *objVals;
	public:
		JSONObject();
	private:
		virtual ~JSONObject();

	public:
		virtual JSType GetJSType();
		virtual void ToJSONString(Text::StringBuilderUTF *sb);
		virtual Bool Equals(const UTF8Char *s);
		virtual Bool Identical(JSONBase *obj);
		void SetObjectValue(const UTF8Char *name, JSONBase *val);
		JSONBase *GetObjectValue(const UTF8Char *name);
		void GetObjectNames(Data::ArrayList<const UTF8Char *> *names);
	};

	class JSONArray : public JSONBase
	{
	private:
		Data::ArrayList<Text::JSONBase *> *arrVals;
	public:
		JSONArray();
	private:
		virtual ~JSONArray();

	public:
		virtual JSType GetJSType();
		virtual void ToJSONString(Text::StringBuilderUTF *sb);
		virtual Bool Equals(const UTF8Char *s);
		virtual Bool Identical(JSONBase *obj);
		void SetArrayValue(UOSInt index, Text::JSONBase *val);
		void AddArrayValue(Text::JSONBase *val);
		JSONBase *GetArrayValue(UOSInt index);
		UOSInt GetArrayLength();
	};

	class JSONNull : public JSONBase
	{
	public:
		JSONNull();
	private:
		virtual ~JSONNull();

	public:
		virtual JSType GetJSType();
		virtual void ToJSONString(Text::StringBuilderUTF *sb);
		virtual Bool Equals(const UTF8Char *s);
		virtual Bool Identical(JSONBase *obj);
	};
}
#endif
