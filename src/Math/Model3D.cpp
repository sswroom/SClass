#include "Stdafx.h"
#include "Math/Model3D.h"

Math::Model3D::Model3D(NN<Text::String> sourceName) : IO::ParsedObject(sourceName)
{
	this->minPt = nullptr;
	this->maxPt = nullptr;
}

Math::Model3D::~Model3D()
{
	this->itemList.DeleteAll();
}

IO::ParserType Math::Model3D::GetParserType() const
{
	return IO::ParserType::Model3D;
}

void Math::Model3D::AddTriangle(Vector3 pt1, Vector3 pt2, Vector3 pt3, UInt32 elementId)
{
	NN<Triangle> tri;
	NEW_CLASSNN(tri, Triangle());
	tri->pt1 = pt1;
	tri->pt2 = pt2;
	tri->pt3 = pt3;
	tri->elementId = elementId;
	if (this->itemList.IsEmpty())
	{
		this->minPt = this->maxPt = pt1;
	}
	this->minPt = this->minPt.Min(pt1).Min(pt2).Min(pt3);
	this->maxPt = this->maxPt.Max(pt1).Max(pt2).Max(pt3);
	this->itemList.Add(tri);
}

UIntOS Math::Model3D::GetCount() const
{
	return this->itemList.GetCount();
}

NN<Math::Model3D::ModelItem> Math::Model3D::GetItemNoCheck(UIntOS index) const
{
	return this->itemList.GetItemNoCheck(index);
}

void Math::Model3D::GetBounds(OutParam<Vector3> minPt, OutParam<Vector3> maxPt) const
{
	minPt.Set(this->minPt);
	maxPt.Set(this->maxPt);
}