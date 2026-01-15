#ifndef _SM_DATA_CLASS
#define _SM_DATA_CLASS
#include "AnyType.h"
#include "Data/ArrayListNN.hpp"
#include "Data/VariItem.h"
#include "Text/StringBuilderUTF8.h"

namespace Data
{
	class Class
	{
	public:
		typedef IntOS (*ByNameFunc)(Text::CStringNN name);
		struct FieldInfo
		{
			NN<Text::String> name;
			IntOS ofst;
			Data::VariItem::ItemType itemType;
			Bool notNull;
			Optional<Text::String> typeName;
			ByNameFunc byNameFunc;
		};
	private:
		const void *refObj;
		Data::ArrayListNN<FieldInfo> fields;

		static void __stdcall FreeFieldInfo(NN<FieldInfo> field);
	public:
		Class(const void *refObj);
		virtual ~Class();

		UIntOS AddField(Text::CStringNN name, IntOS ofst, Data::VariItem::ItemType itemType, Bool notNull); //return field size
		UIntOS AddFieldEnum(Text::CStringNN name, IntOS ofst, Text::CStringNN typeName, UIntOS fieldSize, ByNameFunc byNameFunc);
		Bool AddField(Text::CStringNN name, const UInt8 *val);
		Bool AddField(Text::CStringNN name, const Int8 *val);
		Bool AddField(Text::CStringNN name, const UInt16 *val);
		Bool AddField(Text::CStringNN name, const Int16 *val);
		Bool AddField(Text::CStringNN name, const UInt32 *val);
		Bool AddField(Text::CStringNN name, const Int32 *val);
		Bool AddField(Text::CStringNN name, const NInt32 *val);
		Bool AddField(Text::CStringNN name, const UInt64 *val);
		Bool AddField(Text::CStringNN name, const Int64 *val);
		Bool AddField(Text::CStringNN name, const Single *val);
		Bool AddField(Text::CStringNN name, const Double *val);
		Bool AddField(Text::CStringNN name, Text::String *const *val);
		Bool AddField(Text::CStringNN name, const Optional<Text::String> *val);
		Bool AddField(Text::CStringNN name, const NN<Text::String> *val);
		Bool AddField(Text::CStringNN name, const Data::Timestamp *val);
		Bool AddField(Text::CStringNN name, const Data::Date *val);
		Bool AddField(Text::CStringNN name, const Bool *val);
		Bool AddField(Text::CStringNN name, Data::ReadonlyArray<UInt8> *const *val);
		Bool AddField(Text::CStringNN name, const Optional<Math::Geometry::Vector2D> *val);
		Bool AddField(Text::CStringNN name, Data::UUID *const *val);

		UIntOS GetFieldCount();
		Optional<Text::String> GetFieldName(UIntOS index);
		Data::VariItem::ItemType GetFieldType(UIntOS index);
		Optional<FieldInfo> GetFieldInfo(UIntOS index);
		
		Optional<Data::VariItem> GetNewValue(UIntOS index, AnyType obj);
		Bool IsNotNull(UIntOS index);
		Bool GetValue(NN<Data::VariItem> itm, UIntOS index, AnyType obj);
		Bool SetField(AnyType obj, UIntOS index, NN<Data::VariItem> item);
		Bool SetFieldClearItem(AnyType obj, UIntOS index, NN<Data::VariItem> item);
		Bool Equals(AnyType obj1, AnyType obj2);

		void ToCppClassHeader(Text::StringBase<UTF8Char> *clsName, UIntOS tabLev, NN<Text::StringBuilderUTF8> sb);
		void ToCppClassSource(Text::StringBase<UTF8Char> *clsPrefix, Text::StringBase<UTF8Char> *clsName, UIntOS tabLev, NN<Text::StringBuilderUTF8> sb);
		void ToJavaClass(Text::StringBase<UTF8Char> *clsName, UIntOS tabLev, NN<Text::StringBuilderUTF8> sb);

		static Optional<Data::Class> ParseFromStr(Text::CStringNN str);
		static Optional<Data::Class> ParseFromCpp(Text::CStringNN str);
	};
}

#define CLASS_ADD(cls, val) (cls->AddField(CSTR(#val), &this->val))
#endif
