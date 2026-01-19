#include "Stdafx.h"
#include "Math/CesiumEntityWriter.h"
#include "Math/GeoJSONWriter.h"
#include "Math/VectorTextWriterList.h"
#include "Math/WKTWriter.h"

Math::VectorTextWriterList::VectorTextWriterList()
{
	NN<Math::VectorTextWriter> writer;
	NEW_CLASSNN(writer, Math::WKTWriter());
	this->list.Add(writer);
	NEW_CLASSNN(writer, Math::GeoJSONWriter());
	this->list.Add(writer);
	NEW_CLASSNN(writer, Math::CesiumEntityWriter());
	this->list.Add(writer);
}

Math::VectorTextWriterList::~VectorTextWriterList()
{
	this->list.DeleteAll();
}

UIntOS Math::VectorTextWriterList::GetCount() const
{
	return this->list.GetCount();
}

Optional<Math::VectorTextWriter> Math::VectorTextWriterList::GetItem(UIntOS index) const
{
	return this->list.GetItem(index);
}
