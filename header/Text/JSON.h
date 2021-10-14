#ifndef _SM_TEXT_JSON
#define _SM_TEXT_JSON
#include "Data/StringUTF8Map.h"
#include "Text/StringBuilderUTF.h"

namespace Text
{
	enum class JSONType
	{
		Object,
		Array,
		Number,
		StringUTF8,
		BOOL,
		Null,
		INT32,
		INT64,
		StringWO
	};

	class JSONBase
	{
	private:
		OSInt useCnt;

	protected:
		JSONBase();
	protected:
		virtual ~JSONBase();

	public:
		virtual JSONType GetType() = 0;
		virtual void ToJSONString(Text::StringBuilderUTF *sb) = 0;
		virtual Bool Equals(const UTF8Char *s) = 0;
		virtual Bool Identical(JSONBase *obj) = 0;
		void BeginUse();
		void EndUse();
		Bool IsStringUTF8();

		static JSONBase *ParseJSONStr(const UTF8Char *jsonStr);
		static JSONBase *ParseJSONStrLen(const UTF8Char *jsonStr, UOSInt strLen);

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
		virtual JSONType GetType();
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
		virtual JSONType GetType();
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
		virtual JSONType GetType();
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
		virtual JSONType GetType();
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
		virtual JSONType GetType();
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
		virtual JSONType GetType();
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
		virtual JSONType GetType();
		virtual void ToJSONString(Text::StringBuilderUTF *sb);
		virtual Bool Equals(const UTF8Char *s);
		virtual Bool Identical(JSONBase *obj);
		void SetObjectValue(const UTF8Char *name, JSONBase *val);
		JSONBase *GetObjectValue(const UTF8Char *name);
		void GetObjectNames(Data::ArrayList<const UTF8Char *> *names);
		const UTF8Char *GetObjectString(const UTF8Char *name);
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
		virtual JSONType GetType();
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
		virtual JSONType GetType();
		virtual void ToJSONString(Text::StringBuilderUTF *sb);
		virtual Bool Equals(const UTF8Char *s);
		virtual Bool Identical(JSONBase *obj);
	};

	const UTF8Char *JSONTypeGetName(JSONType t);
}
#endif
