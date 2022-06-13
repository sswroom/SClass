#include "Stdafx.h"
#include "Math/CesiumEntityWriter.h"
#include "Math/VectorTextWriterList.h"
#include "Math/WKTWriter.h"

Math::VectorTextWriterList::VectorTextWriterList()
{
	Math::VectorTextWriter *writer;
	NEW_CLASS(writer, Math::WKTWriter());
	this->list.Add(writer);
	NEW_CLASS(writer, Math::CesiumEntityWriter());
	this->list.Add(writer);
}

Math::VectorTextWriterList::~VectorTextWriterList()
{
	Math::VectorTextWriter *writer;
	UOSInt i = this->list.GetCount();
	while (i-- > 0)
	{
		writer = this->list.GetItem(i);
		DEL_CLASS(writer);
	}
}

UOSInt Math::VectorTextWriterList::GetCount() const
{
	return this->list.GetCount();
}

Math::VectorTextWriter* Math::VectorTextWriterList::GetItem(UOSInt index) const
{
	return this->list.GetItem(index);
}
