#ifndef _SM_TEXT_JSON
#define _SM_TEXT_JSON
#include "Data/ArrayList.h"
#include "Data/FastStringMap.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	enum class JSONType
	{
		Object,
		Array,
		Number,
		String,
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
		virtual void ToJSONString(Text::StringBuilderUTF8 *sb) = 0;
		virtual Bool Equals(Text::CString s) = 0;
		virtual Bool Identical(JSONBase *obj) = 0;
		void BeginUse();
		void EndUse();
		Bool IsString();

		JSONBase *GetValue(const UTF8Char *path, UOSInt pathLen);
		Text::String *GetString(const UTF8Char *path, UOSInt pathLen);
		Int32 GetAsInt32();
		Int64 GetAsInt64();
		Double GetAsDouble();

		static JSONBase *ParseJSONStr(Text::CString jsonStr);
		static JSONBase *ParseJSONBytes(const UInt8 *jsonBytes, UOSInt len);

	private:
		static const UTF8Char *ClearWS(const UTF8Char *jsonStr);
		static const UTF8Char *ParseJSString(const UTF8Char *jsonStr, Text::StringBuilderUTF8 *sb);
		static const UTF8Char *ParseJSNumber(const UTF8Char *jsonStr, Double *val);
		static JSONBase *ParseJSONStr2(const UTF8Char *jsonStr, const UTF8Char *jsonStrEnd, const UTF8Char **jsonStrEndOut, Text::StringBuilderUTF8 *sbEnv);
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
		virtual void ToJSONString(Text::StringBuilderUTF8 *sb);
		virtual Bool Equals(Text::CString s);
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
		virtual void ToJSONString(Text::StringBuilderUTF8 *sb);
		virtual Bool Equals(Text::CString s);
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
		virtual void ToJSONString(Text::StringBuilderUTF8 *sb);
		virtual Bool Equals(Text::CString s);
		virtual Bool Identical(JSONBase *obj);
		Int64 GetValue();
	};

	class JSONString : public JSONBase
	{
	private:
		Text::String *val;

	public:
		JSONString(Text::String *val);
		JSONString(Text::CString val);
	private:
		virtual ~JSONString();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(Text::StringBuilderUTF8 *sb);
		virtual Bool Equals(Text::CString s);
		virtual Bool Identical(JSONBase *obj);
		Text::String *GetValue();
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
		virtual void ToJSONString(Text::StringBuilderUTF8 *sb);
		virtual Bool Equals(Text::CString s);
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
		virtual void ToJSONString(Text::StringBuilderUTF8 *sb);
		virtual Bool Equals(Text::CString s);
		virtual Bool Identical(JSONBase *obj);
		Bool GetValue();
	};

	class JSONObject : public JSONBase
	{
	private:
		Data::FastStringMap<Text::JSONBase *> objVals;
	public:
		JSONObject();
	private:
		virtual ~JSONObject();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(Text::StringBuilderUTF8 *sb);
		virtual Bool Equals(Text::CString s);
		virtual Bool Identical(JSONBase *obj);
		void SetObjectValue(Text::CString name, JSONBase *val);
		void SetObjectInt32(Text::CString name, Int32 val);
		void SetObjectInt64(Text::CString name, Int64 val);
		void SetObjectDouble(Text::CString name, Double val);
		void SetObjectString(Text::CString name, Text::CString val);
		void SetObjectString(Text::CString name, Text::String *val);
		JSONBase *GetObjectValue(Text::CString name);
		void GetObjectNames(Data::ArrayList<Text::String *> *names);
		Text::String *GetObjectString(Text::CString name);
		Text::String *GetObjectNewString(Text::CString name);
		Double GetObjectDouble(Text::CString name);
		Int32 GetObjectInt32(Text::CString name);
		Int64 GetObjectInt64(Text::CString name);
		void RemoveObject(Text::CString name);
	};

	class JSONArray : public JSONBase
	{
	private:
		Data::ArrayList<Text::JSONBase *> arrVals;
	public:
		JSONArray();
	private:
		virtual ~JSONArray();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(Text::StringBuilderUTF8 *sb);
		virtual Bool Equals(Text::CString s);
		virtual Bool Identical(JSONBase *obj);
		void SetArrayValue(UOSInt index, Text::JSONBase *val);
		void AddArrayValue(Text::JSONBase *val);
		JSONBase *GetArrayValue(UOSInt index);
		Double GetArrayDouble(UOSInt index);
		UOSInt GetArrayLength();
		void RemoveArrayItem(UOSInt index);
	};

	class JSONNull : public JSONBase
	{
	public:
		JSONNull();
	private:
		virtual ~JSONNull();

	public:
		virtual JSONType GetType();
		virtual void ToJSONString(Text::StringBuilderUTF8 *sb);
		virtual Bool Equals(Text::CString s);
		virtual Bool Identical(JSONBase *obj);
	};

	Text::CString JSONTypeGetName(JSONType t);
}
#endif
