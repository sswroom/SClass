#include "Stdafx.h"
#include "IO/Java/JavaAnnotation.h"
#include "IO/Java/JavaArrayValue.h"
#include "IO/Java/JavaBooleanValue.h"
#include "IO/Java/JavaClassValue.h"
#include "IO/Java/JavaElementValue.h"
#include "IO/Java/JavaEnumValue.h"
#include "IO/Java/JavaIntegerValue.h"
#include "IO/Java/JavaStringValue.h"

Optional<IO::Java::JavaElementValue> IO::Java::JavaElementValue::ParseElementValue(NN<JavaClass> cls, InOutParam<UnsafeArray<const UInt8>> annoPtr, UnsafeArray<const UInt8> annoEnd)
{
	UnsafeArray<const UInt8> anno = annoPtr.Get();
	if (annoEnd - anno < 3)
	{
		return nullptr;
	}

	UnsafeArray<UInt8> constPtr;
	if (anno[0] == '[')
	{
		UInt16 num_values = ReadMUInt16(&anno[1]);
		NN<JavaArrayValue> arr;
		NN<JavaElementValue> val;
		NEW_CLASSNN(arr, JavaArrayValue());
		UIntOS i;
		anno += 3;
		i = 0;
		while (i < num_values)
		{
			if (ParseElementValue(cls, anno, annoEnd).SetTo(val))
			{
				arr->AddValue(val);
			}
			i++;
		}
		annoPtr.Set(anno);
		return arr;
	}
	else if (anno[0] == 's') //String
	{
		if (cls->GetConst(ReadMUInt16(&anno[1])).SetTo(constPtr))
		{
			if (constPtr[0] == 1) //String
			{
				UInt16 strLen = ReadMUInt16(&constPtr[1]);
				NN<JavaStringValue> val;
				NEW_CLASSNN(val, JavaStringValue(Text::CStringNN(&constPtr[3], strLen)));
				annoPtr.Set(anno + 3);
				return val;
			}
		}
		annoPtr.Set(anno + 3);
		return nullptr;
	}
	else if (anno[0] == 'I') //int
	{
		if (cls->GetConst(ReadMUInt16(&anno[1])).SetTo(constPtr))
		{
			if (constPtr[0] == 3) //Integer
			{
				NN<JavaIntegerValue> val;
				NEW_CLASSNN(val, JavaIntegerValue(ReadMInt32(&constPtr[1])));
				annoPtr.Set(anno + 3);
				return val;
			}
		}
		annoPtr.Set(anno + 3);
		return nullptr;
	}
	else if (anno[0] == 'Z') //boolean
	{
		if (cls->GetConst(ReadMUInt16(&anno[1])).SetTo(constPtr))
		{
			if (constPtr[0] == 3) //Integer
			{
				NN<JavaBooleanValue> val;
				NEW_CLASSNN(val, JavaBooleanValue(ReadMInt32(&constPtr[1]) != 0));
				annoPtr.Set(anno + 3);
				return val;
			}
		}
		annoPtr.Set(anno + 3);
		return nullptr;
	}
	else if (anno[0] == 'c') //class
	{
		if (cls->GetConst(ReadMUInt16(&anno[1])).SetTo(constPtr))
		{
			if (constPtr[0] == 1)
			{
				NN<JavaType> type;
				NN<JavaClassValue> val;
				NEW_CLASSNN(type, JavaType(Text::CStringNN(&constPtr[3], ReadMUInt16(&constPtr[1]))));
				NEW_CLASSNN(val, JavaClassValue(type));
				anno += 5;
				annoPtr.Set(anno);
				return val;
			}
		}
		annoPtr.Set(anno + 3);
		return nullptr;
	}
/*	else if (anno[0] == 's' || anno[0] == 'B' || anno[0] == 'C' || anno[0] == 'D' || anno[0] == 'F' || anno[0] == 'I' || anno[0] == 'J' || anno[0] == 'S' || anno[0] == 'Z')
	{
		this->DetailConstVal(ReadMUInt16(&anno[1]), sb, false);
		anno += 3;
		annoPtr.Set(anno);
		return anno;
	}*/
	else if (anno[0] == 'e')
	{
		UInt16 type_name_index = ReadMUInt16(&anno[1]);
		UInt16 const_name_index = ReadMUInt16(&anno[3]);
		UnsafeArray<UInt8> typeNamePtr;
		UnsafeArray<UInt8> constNamePtr;
		if (cls->GetConst(type_name_index).SetTo(typeNamePtr) && cls->GetConst(const_name_index).SetTo(constNamePtr))
		{
			if (typeNamePtr[0] == 1 && constNamePtr[0] == 1)
			{
				NN<JavaType> type;
				NN<JavaEnumValue> val;
				NEW_CLASSNN(type, JavaType(Text::CStringNN(&typeNamePtr[3], ReadMUInt16(&typeNamePtr[1]))));
				NEW_CLASSNN(val, JavaEnumValue(type, Text::CStringNN(&constNamePtr[3], ReadMUInt16(&constNamePtr[1]))));
				anno += 5;
				annoPtr.Set(anno);
				return val;
			}
		}
		anno += 5;
		annoPtr.Set(anno);
		return nullptr;
	}
	else if (anno[0] == '@') //Annotation
	{
		anno += 1;
		Optional<JavaAnnotation> annoObj = JavaElementValue::ParseAnnotation(cls, anno, annoEnd);
		annoPtr.Set(anno);
		return annoObj;
	}
	else
	{
		printf("JavaElementValue.ParseElementValue: Unknown element_value tag: %c\r\n", anno[0]);
		annoPtr.Set(anno + 1);
		return nullptr;
	}
}

Optional<IO::Java::JavaAnnotation> IO::Java::JavaElementValue::ParseAnnotation(NN<JavaClass> cls, InOutParam<UnsafeArray<const UInt8>> annoPtr, UnsafeArray<const UInt8> annoEnd)
{
	UnsafeArray<const UInt8> anno = annoPtr.Get();
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UInt16 type_index = ReadMUInt16(&anno[0]);
	anno += 2;
	UInt16 num_element_value_pairs = ReadMUInt16(&anno[0]);
	anno += 2;
	if (num_element_value_pairs > 0)
	{
		NN<Data::ArrayListStringNN> names;
		NN<Data::ArrayListNN<JavaElementValue>> values;
		NEW_CLASSNN(names, Data::ArrayListStringNN());
		NEW_CLASSNN(values, Data::ArrayListNN<JavaElementValue>());
		UIntOS j = 0;
		while (j < num_element_value_pairs)
		{
			UInt16 element_name_index = ReadMUInt16(&anno[0]);
			anno += 2;
			NN<JavaElementValue> value;
			if (JavaElementValue::ParseElementValue(cls, anno, annoEnd).SetTo(value))
			{
				if (cls->GetConstUTF8(sbuff, element_name_index).SetTo(sptr))
				{
					names->Add(Text::String::NewP(sbuff, sptr));
					values->Add(value);
				}
				else
				{
					value.Delete();
				}
			}
			j++;
		}
		if (names->GetCount() > 0)
		{
			if (cls->GetConstUTF8(sbuff, type_index).SetTo(sptr))
			{
				NN<JavaType> type;
				NN<JavaAnnotation> ann;
				NEW_CLASSNN(type, JavaType(CSTRP(sbuff, sptr)));
				NEW_CLASSNN(ann, JavaAnnotation(type, names, values));
				annoPtr.Set(anno);
				return ann;
			}
			names->FreeAll();
			names.Delete();
			values->DeleteAll();
			values.Delete();
		}
		else
		{
			names->FreeAll();
			names.Delete();
			values->DeleteAll();
			values.Delete();
		}
	}
	else
	{
		if (cls->GetConstUTF8(sbuff, type_index).SetTo(sptr))
		{
			NN<JavaType> type;
			NN<JavaAnnotation> ann;
			NEW_CLASSNN(type, JavaType(CSTRP(sbuff, sptr)));
			NEW_CLASSNN(ann, JavaAnnotation(type, nullptr, nullptr));
			annoPtr.Set(anno);
			return ann;
		}
	}
	annoPtr.Set(anno);
	return nullptr;
}

UIntOS IO::Java::JavaElementValue::ParseAnnotations(NN<Data::ArrayListNN<JavaAnnotation>> annoList, NN<JavaClass> cls, UnsafeArray<const UInt8> annoPtr, UnsafeArray<const UInt8> annoEnd)
{
	UInt16 num_annotations = ReadMUInt16(&annoPtr[0]);
	annoPtr += 2;
	UIntOS initCnt = annoList->GetCount();
	UIntOS i = 0;
	while (i < num_annotations)
	{
		NN<JavaAnnotation> anno;
		if (JavaElementValue::ParseAnnotation(cls, annoPtr, annoEnd).SetTo(anno))
		{
			annoList->Add(anno);
		}
		i++;
	}
	return annoList->GetCount() - initCnt;
}
