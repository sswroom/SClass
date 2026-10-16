#include "Stdafx.h"
#include "Model/Model3D.h"

Model::Model3D::Model3D(NN<Text::String> sourceName) : IO::ParsedObject(sourceName)
{
	this->minX = this->minY = this->minZ = 0;
	this->maxX = this->maxY = this->maxZ = 0;
}

Model::Model3D::~Model3D()
{
}

IO::ParserType Model::Model3D::GetParserType() const
{
	return IO::ParserType::Model3D;
}

void Model::Model3D::AddTriangle(Double x1, Double y1, Double z1, Double x2, Double y2, Double z2, Double x3, Double y3, Double z3, UInt32 elementId)
{
	Triangle tri;
	tri.x1 = x1;
	tri.y1 = y1;
	tri.z1 = z1;
	tri.x2 = x2;
	tri.y2 = y2;
	tri.z2 = z2;
	tri.x3 = x3;
	tri.y3 = y3;
	tri.z3 = z3;
	tri.elementId = elementId;
	if (this->triangles.empty())
	{
		this->minX = this->maxX = x1;
		this->minY = this->maxY = y1;
		this->minZ = this->maxZ = z1;
	}
	Double xs[3] = {x1, x2, x3};
	Double ys[3] = {y1, y2, y3};
	Double zs[3] = {z1, z2, z3};
	UIntOS i = 0;
	while (i < 3)
	{
		if (xs[i] < this->minX) this->minX = xs[i];
		if (ys[i] < this->minY) this->minY = ys[i];
		if (zs[i] < this->minZ) this->minZ = zs[i];
		if (xs[i] > this->maxX) this->maxX = xs[i];
		if (ys[i] > this->maxY) this->maxY = ys[i];
		if (zs[i] > this->maxZ) this->maxZ = zs[i];
		i++;
	}
	this->triangles.push_back(tri);
}

UIntOS Model::Model3D::GetTriangleCount() const
{
	return (UIntOS)this->triangles.size();
}

const Model::Model3D::Triangle &Model::Model3D::GetTriangle(UIntOS index) const
{
	return this->triangles[(size_t)index];
}

void Model::Model3D::GetBounds(OutParam<Double> minX, OutParam<Double> minY, OutParam<Double> minZ, OutParam<Double> maxX, OutParam<Double> maxY, OutParam<Double> maxZ) const
{
	minX.Set(this->minX);
	minY.Set(this->minY);
	minZ.Set(this->minZ);
	maxX.Set(this->maxX);
	maxY.Set(this->maxY);
	maxZ.Set(this->maxZ);
}