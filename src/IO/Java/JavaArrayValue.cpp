#include "Stdafx.h"
#include "IO/Java/JavaArrayValue.h"

IO::Java::JavaArrayValue::JavaArrayValue()
{
}

IO::Java::JavaArrayValue::~JavaArrayValue()
{
	this->values.DeleteAll();
}

void IO::Java::JavaArrayValue::AddValue(NN<JavaElementValue> val)
{
	this->values.Add(val);
}

UIntOS IO::Java::JavaArrayValue::GetCount() const
{
	return this->values.GetCount();
}

Optional<IO::Java::JavaElementValue> IO::Java::JavaArrayValue::GetItem(UIntOS index) const
{
	return this->values.GetItem(index);
}

void IO::Java::JavaArrayValue::ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName)
{
	sb->AppendUTF8Char('{');
	UIntOS i = 0;
	UIntOS j = this->values.GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendUTF8Char(',');
		}
		this->values.GetItemNoCheck(i)->ToString(sb, importList, packageName);
		i++;
	}
	sb->AppendUTF8Char('}');
}

IO::Java::JavaElementValue::ElementType IO::Java::JavaArrayValue::GetElementType() const
{
	return ElementType::Array;
}
