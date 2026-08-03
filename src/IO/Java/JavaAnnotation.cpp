#include "Stdafx.h"
#include "IO/Java/JavaAnnotation.h"

IO::Java::JavaAnnotation::JavaAnnotation(NN<JavaType> type, Optional<Data::ArrayListStringNN> names, Optional<Data::ArrayListNN<JavaElementValue>> values)
{
	this->type = type;
	this->names = names;
	this->values = values;
}

IO::Java::JavaAnnotation::~JavaAnnotation()
{
	NN<Data::ArrayListStringNN> names;
	if (this->names.SetTo(names))
	{
		names->FreeAll();
		names.Delete();
	}
	NN<Data::ArrayListNN<JavaElementValue>> values;
	if (this->values.SetTo(values))
	{
		values->DeleteAll();
		values.Delete();
	}
	this->type.Delete();
}

NN<IO::Java::JavaType> IO::Java::JavaAnnotation::GetAnnoType() const
{
	return this->type;
}

Optional<IO::Java::JavaElementValue> IO::Java::JavaAnnotation::GetValue(Text::CStringNN name) const
{
	NN<Data::ArrayListStringNN> names;
	NN<Data::ArrayListNN<JavaElementValue>> values;
	if (this->names.SetTo(names) && this->values.SetTo(values))
	{
		UIntOS i = 0;
		UIntOS j = names->GetCount();
		while (i < j)
		{
			if (names->GetItemNoCheck(i)->Equals(name))
			{
				return values->GetItemNoCheck(i);
			}
			i++;
		}
	}
	return nullptr;
}

void IO::Java::JavaAnnotation::ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName)
{
	sb->AppendUTF8Char('@');
	this->type->ToString(sb, importList, packageName);
	NN<Data::ArrayListStringNN> names;
	NN<Data::ArrayListNN<JavaElementValue>> values;
	if (this->names.SetTo(names) && this->values.SetTo(values))
	{
		sb->AppendUTF8Char('(');
		if (names->GetCount() != values->GetCount())
		{
			sb->AppendUTF8Char(')');
			return;
		}
		if (names->GetCount() == 1 && names->GetItemNoCheck(0)->Equals(UTF8STRC("value")))
		{
			values->GetItemNoCheck(0)->ToString(sb, importList, packageName);
			sb->AppendUTF8Char(')');
			return;
		}
		else
		{
			UIntOS i = 0;
			UIntOS j = names->GetCount();
			while (i < j)
			{
				if (i > 0)
				{
					sb->AppendUTF8Char(',');
				}
				sb->Append(names->GetItemNoCheck(i));
				sb->AppendUTF8Char('=');
				values->GetItemNoCheck(i)->ToString(sb, importList, packageName);
				i++;
			}
		}
		sb->AppendUTF8Char(')');
	}
}

IO::Java::JavaElementValue::ElementType IO::Java::JavaAnnotation::GetElementType() const
{
	return ElementType::Annotation;
}