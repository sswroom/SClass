#include "Stdafx.h"
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListNN.hpp"
#include "Data/ByteBuffer.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Parser/FileParser/IFCParser.h"

class IFCEntityComparator : public Data::Comparator<NN<Parser::FileParser::IFCParser::IFCEntity>>
{
public:
	virtual IntOS Compare(NN<Parser::FileParser::IFCParser::IFCEntity> a, NN<Parser::FileParser::IFCParser::IFCEntity> b) const
	{
		return (IntOS)a->id - (IntOS)b->id;
	}
};

void Parser::FileParser::IFCParser::SplitArgs(NN<Data::ArrayListObj<Text::CString>> args, Text::CStringNN s)
{
	UIntOS start = 0;
	Int32 depth = 0;
	Bool inString = false;
	UIntOS i = 0;
	while (i < s.leng)
	{
		UTF8Char c = s.v[i];
		if (c == '\'')
		{
			if (inString && i + 1 < s.leng && s.v[i + 1] == '\'')
			{
				i += 2;
				continue;
			}
			inString = !inString;
		}
		else if (!inString)
		{
			if (c == '(') depth++;
			else if (c == ')') depth--;
			else if (c == ',' && depth == 0)
			{
				args->Add(s.SubstringNoZ(start, i - start));
				start = i + 1;
			}
		}
		i++;
	}
	args->Add(s.Substring(start));
}

void Parser::FileParser::IFCParser::ParseEntities(NN<Data::ArrayListNN<Parser::FileParser::IFCParser::IFCEntity>> entities, Data::ByteArrayR data)
{
	UIntOS i = 0;
	Bool inString = false;
	while (i < data.GetSize())
	{
		UInt8 c = data[i];
		if (c == '\'')
		{
			if (inString && i + 1 < data.GetSize() && data[i + 1] == '\'')
			{
				i += 2;
				continue;
			}
			inString = !inString;
			i++;
			continue;
		}
		if (inString || c != '#')
		{
			i++;
			continue;
		}
		UIntOS numStart = ++i;
		while (i < data.GetSize() && data[i] >= '0' && data[i] <= '9') i++;
		if (i == numStart) continue;
		UInt32 id = Text::StrToUInt32B(&data[numStart], nullptr);
		while (i < data.GetSize() && (data[i] == ' ' || data[i] == '\r' || data[i] == '\n' || data[i] == '\t')) i++;
		if (i >= data.GetSize() || data[i] != '=') continue;
		i++;
		while (i < data.GetSize() && (data[i] == ' ' || data[i] == '\r' || data[i] == '\n' || data[i] == '\t')) i++;
		UIntOS typeStart = i;
		while (i < data.GetSize() && ((data[i] >= 'A' && data[i] <= 'Z') || (data[i] >= '0' && data[i] <= '9') || data[i] == '_')) i++;
		if (i == typeStart) continue;
		while (i < data.GetSize() && data[i] != '(') i++;
		if (i >= data.GetSize()) break;
		i++;
		UIntOS argStart = i;
		Int32 depth = 1;
		inString = false;
		while (i < data.GetSize() && depth > 0)
		{
			c = data[i];
			if (c == '\'')
			{
				if (inString && i + 1 < data.GetSize() && data[i + 1] == '\'')
				{
					i += 2;
					continue;
				}
				inString = !inString;
			}
			else if (!inString)
			{
				if (c == '(') depth++;
				else if (c == ')') depth--;
			}
			i++;
		}
		if (depth != 0) break;
		NN<Parser::FileParser::IFCParser::IFCEntity> ent;
		NEW_CLASSNN(ent, Parser::FileParser::IFCParser::IFCEntity());
		ent->id = id;
		UIntOS j = Text::StrIndexOfCharC(&data[typeStart], data.GetSize() - typeStart, '(');
		ent->type = Text::CStringNN(&data[typeStart], j);
		SplitArgs(ent->args, Text::CStringNN(&data[argStart], i - argStart - 1));
		entities->Add(ent);
	}
	Data::Sort::ArtificialQuickSort sort;
	IFCEntityComparator comparator;
	sort.Sort<NN<Parser::FileParser::IFCParser::IFCEntity>>(entities, comparator);
}

Optional<Parser::FileParser::IFCParser::IFCEntity> Parser::FileParser::IFCParser::Find(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 id)
{
	IntOS left = 0;
	IntOS right = (IntOS)entities->GetCount() - 1;
	while (left <= right)
	{
		IntOS mid = (left + right) >> 1;
		NN<IFCEntity> ent = entities->GetItemNoCheck((UIntOS)mid);
		if (ent->id < id)
		{
			left = mid + 1;
		}
		else if (ent->id > id)
		{
			right = mid - 1;
		}
		else
		{
			return ent;
		}
	}
	return nullptr;
}

UInt32 Parser::FileParser::IFCParser::Ref(Text::CString s)
{
	Text::CStringNN nns;
	UIntOS i;
	if (!s.SetTo(nns) || (i = nns.IndexOf('#')) == INVALID_INDEX) return 0;
	return Text::StrToUInt32B(nns.v + i + 1, nullptr);
}

Data::ArrayListNative<UInt32> Parser::FileParser::IFCParser::Refs(Text::CString s)
{
	Data::ArrayListNative<UInt32> refs;
	Text::CStringNN nns;
	if (!s.SetTo(nns)) return refs;
	UIntOS i = 0;
	UIntOS j = nns.leng;
	while (i < j)
	{
		if (nns.v[i] == '#')
		{
			UIntOS start = ++i;
			while (i < j && nns.v[i] >= '0' && nns.v[i] <= '9') i++;
			if (i > start) refs.Add(Text::StrToUInt32B(nns.v + start, nullptr));
		}
		else i++;
	}
	return refs;
}

Data::ArrayListNative<Double> Parser::FileParser::IFCParser::Numbers(Text::CString s)
{
	Data::ArrayListNative<Double> values;
	Text::CStringNN nns;
	if (!s.SetTo(nns)) return values;
	UnsafeArray<const UTF8Char> p = nns.v;
	UnsafeArray<const UTF8Char> pEnd = p + nns.leng;
	while (p < pEnd)
	{
		if ((*p >= '0' && *p <= '9') || *p == '-' || *p == '+')
		{
			UnsafeArray<const UTF8Char> end;
			Double v = Text::StrToDoubleB(p, end);
			if (end != p)
			{
				values.Add(v);
				p = end < pEnd ? end : pEnd;
				continue;
			}
		}
		p++;
	}
	return values;
}

Math::Vector3 Parser::FileParser::IFCParser::GetPoint(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 id)
{
	NN<Parser::FileParser::IFCParser::IFCEntity> ent;
	if (Find(entities, id).SetTo(ent) && ent->type.Equals(CSTR("IFCCARTESIANPOINT")) && !ent->args.IsEmpty())
	{
		Data::ArrayListNative<Double> nums = Numbers(ent->args.GetItem(0));
		if (nums.GetCount() >= 2) return {nums.GetItem(0), nums.GetItem(1), nums.GetCount() >= 3 ? nums.GetItem(2) : 0};
	}
	return {0, 0, 0};
}

Math::Vector3 Parser::FileParser::IFCParser::GetDirection(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 id, Math::Vector3 defVal)
{
	NN<IFCEntity> ent;
	if (Find(entities, id).SetTo(ent) && ent->type.Equals(CSTR("IFCDIRECTION")) && !ent->args.IsEmpty())
	{
		Data::ArrayListNative<Double> nums = Numbers(ent->args.GetItem(0));
		if (nums.GetCount() >= 2) return Math::Vector3(nums.GetItem(0), nums.GetItem(1), nums.GetCount() >= 3 ? nums.GetItem(2) : 0).Normalized();
	}
	return defVal;
}

Math::Transform Parser::FileParser::IFCParser::GetAxisPlacement(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 id)
{
	Math::Transform t = Math::Transform::Identity();
	NN<IFCEntity> ent;
	if (!Find(entities, id).SetTo(ent) || (!ent->type.Equals(CSTR("IFCAXIS2PLACEMENT3D")) && !ent->type.Equals(CSTR("IFCAXIS2PLACEMENT2D"))) || ent->args.IsEmpty()) return t;
	t.origin = GetPoint(entities, Ref(ent->args.GetItem(0)));
	if (ent->type.Equals(CSTR("IFCAXIS2PLACEMENT2D")))
	{
		if (ent->args.GetCount() > 1) t.axis.vec[0] = GetDirection(entities, Ref(ent->args.GetItem(1)), {1, 0, 0});
		t.axis.vec[1] = Math::Vector3(-t.axis.vec[0].GetY(), t.axis.vec[0].GetX(), 0);
		return t;
	}
	if (ent->args.GetCount() > 1) t.axis.vec[2] = GetDirection(entities, Ref(ent->args.GetItem(1)), {0, 0, 1});
	if (ent->args.GetCount() > 2) t.axis.vec[0] = GetDirection(entities, Ref(ent->args.GetItem(2)), {1, 0, 0});
	t.axis.vec[1] = (t.axis.vec[2].CrossProduct(t.axis.vec[0])).Normalized();
	t.axis.vec[0] = (t.axis.vec[1].CrossProduct(t.axis.vec[2])).Normalized();
	return t;
}

Math::Transform Parser::FileParser::IFCParser::GetObjectPlacement(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 id, UInt32 depth)
{
	if (id == 0 || depth > 32) return Math::Transform::Identity();
	NN<IFCEntity> ent;
	if (!Find(entities, id).SetTo(ent) || !ent->type.Equals(CSTR("IFCLOCALPLACEMENT")) || ent->args.GetCount() < 2) return Math::Transform::Identity();
	return GetObjectPlacement(entities, Ref(ent->args.GetItem(0)), depth + 1).Compose(GetAxisPlacement(entities, Ref(ent->args.GetItem(1))));
}

void Parser::FileParser::IFCParser::AddPolygon(NN<Math::Model3D> model, NN<Data::ArrayListObj<Math::Vector3>> points, const Math::Transform &t, UInt32 elementId)
{
	if (points->GetCount() < 3) return;
	Math::Vector3 p0 = t.TransformPoint(points->GetItem(0));
	UIntOS i = 1;
	while (i + 1 < points->GetCount())
	{
		Math::Vector3 p1 = t.TransformPoint(points->GetItem(i));
		Math::Vector3 p2 = t.TransformPoint(points->GetItem(i + 1));
		model->AddTriangle(p0, p1, p2, elementId);
		i++;
	}
}

Data::ArrayListObj<Math::Vector3> Parser::FileParser::IFCParser::GetProfilePoints(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 profileId)
{
	Data::ArrayListObj<Math::Vector3> points;
	NN<IFCEntity> profile;
	if (!Find(entities, profileId).SetTo(profile)) return points;
	if (profile->type.Equals(CSTR("IFCARBITRARYCLOSEDPROFILEDEF")) || profile->type.Equals(CSTR("IFCARBITRARYPROFILEDEFWITHVOIDS")))
	{
		if (profile->args.GetCount() < 3) return points;
		NN<IFCEntity> curve;
		if (!Find(entities, Ref(profile->args.GetItem(2))).SetTo(curve) || curve->args.IsEmpty()) return points;
		if (curve->type.Equals(CSTR("IFCPOLYLINE")))
		{
			Data::ArrayListNative<UInt32> refs = Refs(curve->args.GetItem(0));
			for (UInt32 i = 0; i < refs.GetCount(); i++) points.Add(GetPoint(entities, refs.GetItem(i)));
		}
		else if (curve->type.Equals(CSTR("IFCINDEXEDPOLYCURVE")))
		{
			NN<IFCEntity> coordList;
			if (Find(entities, Ref(curve->args.GetItem(0))).SetTo(coordList) && coordList->type.Equals(CSTR("IFCCARTESIANPOINTLIST2D")) && !coordList->args.IsEmpty())
			{
				Data::ArrayListNative<Double> nums = Numbers(coordList->args.GetItem(0));
				UIntOS i = 0;
				while (i + 1 < nums.GetCount())
				{
					points.Add({nums.GetItem(i), nums.GetItem(i + 1), 0});
					i += 2;
				}
			}
		}
	}
	else if (profile->type.Equals(CSTR("IFCRECTANGLEPROFILEDEF")) && profile->args.GetCount() >= 5)
	{
		Double x = Text::StrToDoubleB(profile->args.GetItem(3).OrEmpty().v, nullptr) * 0.5;
		Double y = Text::StrToDoubleB(profile->args.GetItem(4).OrEmpty().v, nullptr) * 0.5;
		points.Add({-x, -y, 0});
		points.Add({x, -y, 0});
		points.Add({x, y, 0});
		points.Add({-x, y, 0});
		if (Ref(profile->args.GetItem(2)) != 0)
		{
			Math::Transform t = GetAxisPlacement(entities, Ref(profile->args.GetItem(2)));
			for (UIntOS i = 0; i < points.GetCount(); i++) points.SetItem(i, t.TransformPoint(points.GetItem(i)));
		}
	}
	else if (profile->type.Equals(CSTR("IFCCIRCLEPROFILEDEF")) && profile->args.GetCount() >= 4)
	{
		Double r = Text::StrToDoubleB(profile->args.GetItem(3).OrEmpty().v, nullptr);
		UIntOS i = 0;
		while (i < 32)
		{
			Double a = (Double)i * 6.283185307179586 / 32.0;
			points.Add({Math_Cos(a) * r, Math_Sin(a) * r, 0});
			i++;
		}
		if (Ref(profile->args.GetItem(2)) != 0)
		{
			Math::Transform t = GetAxisPlacement(entities, Ref(profile->args.GetItem(2)));
			for (UIntOS i = 0; i < points.GetCount(); i++) points.SetItem(i, t.TransformPoint(points.GetItem(i)));
		}
	}
	if (points.GetCount() > 1 && Math_Abs(points.GetItem(0).GetX() - points.GetItem(points.GetCount() - 1).GetX()) < 1e-12 && Math_Abs(points.GetItem(0).GetY() - points.GetItem(points.GetCount() - 1).GetY()) < 1e-12 && Math_Abs(points.GetItem(0).GetZ() - points.GetItem(points.GetCount() - 1).GetZ()) < 1e-12) points.RemoveAt(points.GetCount() - 1);
	return points;
}

void Parser::FileParser::IFCParser::ParseExtrusion(NN<Math::Model3D> model, NN<Data::ArrayListNN<IFCEntity>> entities, NN<IFCEntity> solid, const Math::Transform &productT, UInt32 elementId)
{
	if (solid->args.GetCount() < 4) return;
	Data::ArrayListObj<Math::Vector3> profile = GetProfilePoints(entities, Ref(solid->args.GetItem(0)));
	if (profile.GetCount() < 3) return;
	Math::Transform t = productT.Compose(GetAxisPlacement(entities, Ref(solid->args.GetItem(1))));
	Math::Vector3 direction = GetDirection(entities, Ref(solid->args.GetItem(2)), {0, 0, 1});
	Double depth = Text::StrToDoubleB(solid->args.GetItem(3).OrEmpty().v, nullptr);
	Data::ArrayListObj<Math::Vector3> top(profile.GetCount());
	UIntOS i = 0;
	while (i < profile.GetCount())
	{
		top.SetItem(i, profile.GetItem(i) + direction * depth);
		i++;
	}
	AddPolygon(model, profile, t, elementId);
	AddPolygon(model, top, t, elementId);
	Data::ArrayListObj<Math::Vector3> side(4); 
	i = 0;
	while (i < profile.GetCount())
	{
		UIntOS next = (i + 1) % profile.GetCount();
		side.SetItem(0, profile.GetItem(i));
		side.SetItem(1, profile.GetItem(next));
		side.SetItem(2, top.GetItem(next));
		side.SetItem(3, top.GetItem(i));
		AddPolygon(model, side, t, elementId);
		i++;
	}
}

void Parser::FileParser::IFCParser::ParseFacetedBrep(NN<Math::Model3D> model, NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 brepId, const Math::Transform &t, UInt32 elementId)
{
	NN<Parser::FileParser::IFCParser::IFCEntity> brep;
	if (!Find(entities, brepId).SetTo(brep) || brep->args.IsEmpty()) return;
	NN<Parser::FileParser::IFCParser::IFCEntity> shell;
	if (!Find(entities, Ref(brep->args.GetItem(0))).SetTo(shell) || shell->args.IsEmpty()) return;
	Data::ArrayListNative<UInt32> faces = Refs(shell->args.GetItem(0));
	UIntOS i = 0;
	UIntOS j = faces.GetCount();
	while (i < j)
	{
		UInt32 faceId = faces.GetItem(i++);
		NN<Parser::FileParser::IFCParser::IFCEntity> face;
		if (!Find(entities, faceId).SetTo(face) || face->args.IsEmpty()) continue;
		Data::ArrayListNative<UInt32> bounds = Refs(face->args.GetItem(0));
		if (bounds.IsEmpty()) continue;
		NN<Parser::FileParser::IFCParser::IFCEntity> bound;
		if (!Find(entities, bounds.GetItem(0)).SetTo(bound) || bound->args.IsEmpty()) continue;
		NN<Parser::FileParser::IFCParser::IFCEntity> loop;
		if (!Find(entities, Ref(bound->args.GetItem(0))).SetTo(loop) || !loop->type.Equals(CSTR("IFCPOLYLOOP")) || loop->args.IsEmpty()) continue;
		Data::ArrayListObj<Math::Vector3> points;
		Data::ArrayListNative<UInt32> loopRefs = Refs(loop->args.GetItem(0));
		for (UInt32 i = 0; i < loopRefs.GetCount(); i++) points.Add(GetPoint(entities, loopRefs.GetItem(i)));
		AddPolygon(model, points, t, elementId);
	}
}

Data::ArrayListObj<Math::Vector3> Parser::FileParser::IFCParser::GetPointList3D(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 listId)
{
	Data::ArrayListObj<Math::Vector3> points;
	NN<IFCEntity> list;
	if (!Find(entities, listId).SetTo(list) || !list->type.Equals(CSTR("IFCCARTESIANPOINTLIST3D")) || list->args.IsEmpty()) return points;
	Data::ArrayListNative<Double> nums = Numbers(list->args.GetItem(0));
	UIntOS i = 0;
	while (i + 2 < nums.GetCount())
	{
		points.Add({nums.GetItem(i), nums.GetItem(i + 1), nums.GetItem(i + 2)});
		i += 3;
	}
	return points;
}

void Parser::FileParser::IFCParser::ParseTriangulatedFaceSet(NN<Math::Model3D> model, NN<Data::ArrayListNN<IFCEntity>> entities, NN<IFCEntity> faceSet, const Math::Transform &t, UInt32 elementId)
{
	if (faceSet->args.GetCount() < 4) return;
	Data::ArrayListObj<Math::Vector3> points = GetPointList3D(entities, Ref(faceSet->args.GetItem(0)));
	Data::ArrayListNative<Double> indices = Numbers(faceSet->args.GetItem(3));
	Data::ArrayListObj<Math::Vector3> triangle(3); 
	UIntOS i = 0;
	while (i + 2 < indices.GetCount())
	{
		UIntOS i1 = (UIntOS)indices.GetItem(i) - 1;
		UIntOS i2 = (UIntOS)indices.GetItem(i + 1) - 1;
		UIntOS i3 = (UIntOS)indices.GetItem(i + 2) - 1;
		if (i1 < points.GetCount() && i2 < points.GetCount() && i3 < points.GetCount())
		{
			triangle.SetItem(0, points.GetItem(i1));
			triangle.SetItem(1, points.GetItem(i2));
			triangle.SetItem(2, points.GetItem(i3));
			AddPolygon(model, triangle, t, elementId);
		}
		i += 3;
	}
}

void Parser::FileParser::IFCParser::ParsePolygonalFaceSet(NN<Math::Model3D> model, NN<Data::ArrayListNN<IFCEntity>> entities, NN<IFCEntity> faceSet, const Math::Transform &t, UInt32 elementId)
{
	if (faceSet->args.GetCount() < 3) return;
	Data::ArrayListObj<Math::Vector3> points = GetPointList3D(entities, Ref(faceSet->args.GetItem(0)));
	Data::ArrayListNative<UInt32> faceIds = Refs(faceSet->args.GetItem(2));
	for (UInt32 i = 0; i < faceIds.GetCount(); i++)
	{
		UInt32 faceId = faceIds.GetItem(i);
		NN<Parser::FileParser::IFCParser::IFCEntity> face;
		if (!Find(entities, faceId).SetTo(face) || face->args.IsEmpty()) continue;
		Data::ArrayListNative<Double> indices = Numbers(face->args.GetItem(0));
		Data::ArrayListObj<Math::Vector3> polygon;
		for (UIntOS j = 0; j < indices.GetCount(); j++)
		{
			Double index = indices.GetItem(j);
			UIntOS pointIndex = (UIntOS)index;
			if (pointIndex == 0 || pointIndex > points.GetCount())
			{
				polygon.Clear();
				break;
			}
			polygon.Add(points.GetItem(pointIndex - 1));
		}
		AddPolygon(model, polygon, t, elementId);
	}
}

Math::Transform Parser::FileParser::IFCParser::GetMappingTarget(NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 id)
{
	Math::Transform t = Math::Transform::Identity();
	NN<IFCEntity> op;
	if (!Find(entities, id).SetTo(op) || (!op->type.Equals(CSTR("IFCCARTESIANTRANSFORMATIONOPERATOR3D")) && !op->type.Equals(CSTR("IFCCARTESIANTRANSFORMATIONOPERATOR3DNONUNIFORM"))) || op->args.GetCount() < 3) return t;
	if (Ref(op->args.GetItem(0)) != 0) t.axis.vec[0] = GetDirection(entities, Ref(op->args.GetItem(0)), {1, 0, 0});
	if (Ref(op->args.GetItem(1)) != 0) t.axis.vec[1] = GetDirection(entities, Ref(op->args.GetItem(1)), {0, 1, 0});
	t.origin = GetPoint(entities, Ref(op->args.GetItem(2)));
	if (op->args.GetCount() > 4 && Ref(op->args.GetItem(4)) != 0) t.axis.vec[2] = GetDirection(entities, Ref(op->args.GetItem(4)), {0, 0, 1});
	else t.axis.vec[2] = (t.axis.vec[0].CrossProduct(t.axis.vec[1])).Normalized();
	Double scale = op->args.GetCount() > 3 && op->args.GetItem(3).OrEmpty().IndexOf('$') == INVALID_INDEX ? Text::StrToDoubleB(op->args.GetItem(3).OrEmpty().v, nullptr) : 1.0;
	Double scaleY = op->type.Equals(CSTR("IFCCARTESIANTRANSFORMATIONOPERATOR3DNONUNIFORM")) && op->args.GetCount() > 5 && op->args.GetItem(5).OrEmpty().IndexOf('$') == INVALID_INDEX ? Text::StrToDoubleB(op->args.GetItem(5).OrEmpty().v, nullptr) : scale;
	Double scaleZ = op->type.Equals(CSTR("IFCCARTESIANTRANSFORMATIONOPERATOR3DNONUNIFORM")) && op->args.GetCount() > 6 && op->args.GetItem(6).OrEmpty().IndexOf('$') == INVALID_INDEX ? Text::StrToDoubleB(op->args.GetItem(6).OrEmpty().v, nullptr) : scale;
	t.axis.vec[0] = t.axis.vec[0] * scale;
	t.axis.vec[1] = t.axis.vec[1] * scaleY;
	t.axis.vec[2] = t.axis.vec[2] * scaleZ;
	return t;
}

void Parser::FileParser::IFCParser::ParseItem(NN<Math::Model3D> model, NN<Data::ArrayListNN<IFCEntity>> entities, UInt32 itemId, const Math::Transform &t, UInt32 elementId, UInt32 depth)
{
	if (depth > 16) return;
	NN<Parser::FileParser::IFCParser::IFCEntity> item;
	if (!Find(entities, itemId).SetTo(item)) return;
	if (item->type.Equals(CSTR("IFCEXTRUDEDAREASOLID")))
	{
		ParseExtrusion(model, entities, item, t, elementId);
	}
	else if (item->type.Equals(CSTR("IFCFACETEDBREP")) || item->type.Equals(CSTR("IFCADVANCEDFACEDBREP")))
	{
		ParseFacetedBrep(model, entities, itemId, t, elementId);
	}
	else if (item->type.Equals(CSTR("IFCTRIANGULATEDFACESET")))
	{
		ParseTriangulatedFaceSet(model, entities, item, t, elementId);
	}
	else if (item->type.Equals(CSTR("IFCPOLYGONALFACESET")))
	{
		ParsePolygonalFaceSet(model, entities, item, t, elementId);
	}
	else if ((item->type.Equals(CSTR("IFCBOOLEANCLIPPINGRESULT")) || item->type.Equals(CSTR("IFCBOOLEANRESULT"))) && item->args.GetCount() > 1)
	{
		ParseItem(model, entities, Ref(item->args.GetItem(1)), t, elementId, depth + 1);
	}
	else if (item->type.Equals(CSTR("IFCMAPPEDITEM")) && item->args.GetCount() >= 2)
	{
		NN<Parser::FileParser::IFCParser::IFCEntity> repMap;
		if (!Find(entities, Ref(item->args.GetItem(0))).SetTo(repMap) || repMap->args.GetCount() < 2) return;
		NN<Parser::FileParser::IFCParser::IFCEntity> mappedRep;
		if (!Find(entities, Ref(repMap->args.GetItem(1))).SetTo(mappedRep) || mappedRep->args.GetCount() < 4) return;
		Math::Transform mapOrigin = GetAxisPlacement(entities, Ref(repMap->args.GetItem(0)));
		Math::Transform target = GetMappingTarget(entities, Ref(item->args.GetItem(1)));
		Math::Transform mappedT = t.Compose(target.Compose(mapOrigin.InversePlacement()));
		Data::ArrayListNative<UInt32> childIds = Refs(mappedRep->args.GetItem(3));
		UInt32 childId;
		UIntOS m = 0;
		UIntOS childCount = childIds.GetCount();
		while (m < childCount)
		{
			childId = childIds.GetItem(m);
			ParseItem(model, entities, childId, mappedT, elementId, depth + 1);
			m++;
		}
	}
}

void Parser::FileParser::IFCParser::ParseModel(NN<Math::Model3D> model, NN<Data::ArrayListNN<IFCEntity>> entities)
{
	NN<IFCEntity> ent;
	UIntOS i = 0;
	UIntOS j = entities->GetCount();
	while (i < j)
	{
		ent = entities->GetItemNoCheck(i++);
		if (ent->args.GetCount() < 7) continue;
		UInt32 placementId = Ref(ent->args.GetItem(5));
		UInt32 shapeId = Ref(ent->args.GetItem(6));
		if (shapeId == 0) continue;
		NN<IFCEntity> productShape;
		if (!Find(entities, shapeId).SetTo(productShape) || !productShape->type.Equals(CSTR("IFCPRODUCTDEFINITIONSHAPE"))) continue;
		Math::Transform productT = GetObjectPlacement(entities, placementId, 0);
		if (productShape->args.GetCount() < 3) continue;
		Data::ArrayListNative<UInt32> repIds = Refs(productShape->args.GetItem(2));
		UIntOS k = 0;
		UIntOS repCount = repIds.GetCount();
		while (k < repCount)
		{
			UInt32 repId = repIds.GetItem(k++);
			NN<IFCEntity> rep;
			if (!Find(entities, repId).SetTo(rep) || rep->args.GetCount() < 4) continue;
			Data::ArrayListNative<UInt32> itemIds = Refs(rep->args.GetItem(3));
			UIntOS l = 0;

			while (l < itemIds.GetCount())
			{
				UInt32 itemId = itemIds.GetItem(l++);
				ParseItem(model, entities, itemId, productT, ent->id, 0);
			}
		}
	}
}

Parser::FileParser::IFCParser::IFCParser()
{
}

Parser::FileParser::IFCParser::~IFCParser()
{
}

Int32 Parser::FileParser::IFCParser::GetName()
{
	return *(Int32*)"IFCP";
}

void Parser::FileParser::IFCParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::Model3D)
	{
		selector->AddFilter(CSTR("*.ifc"), CSTR("Industry Foundation Classes"));
	}
}

IO::ParserType Parser::FileParser::IFCParser::GetParserType()
{
	return IO::ParserType::Model3D;
}

Optional<IO::ParsedObject> Parser::FileParser::IFCParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (targetType != IO::ParserType::Unknown && targetType != IO::ParserType::Model3D)
		return nullptr;
	if (hdr.GetSize() < 12)
		return nullptr;
	UIntOS i = 0;
	if (hdr.GetSize() >= 3 && hdr[0] == 0xef && hdr[1] == 0xbb && hdr[2] == 0xbf)
		i = 3;
	while (i < hdr.GetSize() && (hdr[i] == ' ' || hdr[i] == '\r' || hdr[i] == '\n' || hdr[i] == '\t'))
		i++;
	if (hdr.GetSize() - i < 12 || !Text::StrEqualsC(&hdr[i], 12, UTF8STRC("ISO-10303-21")))
		return nullptr;

	UInt64 fileSize = fd->GetDataSize();
	if (fileSize < 16 || fileSize > 536870912)
		return nullptr;
	Data::ByteBuffer buff((UIntOS)fileSize);
	if (fd->GetRealData(0, (UIntOS)fileSize, buff) != (UIntOS)fileSize)
		return nullptr;
	Data::ArrayListNN<IFCEntity> entities;
	ParseEntities(entities, buff);
	if (entities.GetCount() == 0) return nullptr;
	NN<Math::Model3D> model;
	NEW_CLASSNN(model, Math::Model3D(fd->GetFullName()));
	ParseModel(model, entities);
	entities.DeleteAll();
	if (model->GetCount() == 0)
	{
		model.Delete();
		return nullptr;
	}
	return model;
}