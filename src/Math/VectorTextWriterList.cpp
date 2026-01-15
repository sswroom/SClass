#include "Stdafx.h"
#include "Math/CesiumEntityWriter.h"
#include "Math/GeoJSONWriter.h"
#include "Math/VectorTextWriterList.h"
#include "Math/WKTWriter.h"

Math::VectorTextWriterList::VectorTextWriterList()
{
	Math::VectorTextWriter *writer;
	NEW_CLASS(writer, Math::WKTWriter());
	this->list.Add(writer);
	NEW_CLASS(writer, Math::GeoJSONWriter());
	this->list.Add(writer);
	NEW_CLASS(writer, Math::CesiumEntityWriter());
	this->list.Add(writer);
}

Math::VectorTextWriterList::~VectorTextWriterList()
{
	Math::VectorTextWriter *writer;
	UIntOS i = this->list.GetCount();
	while (i-- > 0)
	{
		writer = this->list.GetItem(i);
		DEL_CLASS(writer);
	}
}

UIntOS Math::VectorTextWriterList::GetCount() const
{
	return this->list.GetCount();
}

Math::VectorTextWriter* Math::VectorTextWriterList::GetItem(UIntOS index) const
{
	return this->list.GetItem(index);
}
