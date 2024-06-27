#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Geometry/CurvePolygon.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Text/JSONBuilder.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

void Text::JSONBuilder::AppendStr(Text::CStringNN val)
{
	this->AppendStrUTF8(val.v);
}

void Text::JSONBuilder::AppendStrUTF8(UnsafeArray<const UTF8Char> val)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UTF8Char c;
	sptr = sbuff;
	*sptr++ = '\"';
	while ((c = *val++) != 0)
	{
		switch (c)
		{
		case '\\':
			sptr[0] = '\\';
			sptr[1] = '\\';
			sptr += 2;
			break;
		case '\"':
			sptr[0] = '\\';
			sptr[1] = '\"';
			sptr += 2;
			break;
		case '\r':
			sptr[0] = '\\';
			sptr[1] = 'r';
			sptr += 2;
			break;
		case '\n':
			sptr[0] = '\\';
			sptr[1] = 'n';
			sptr += 2;
			break;
		case '\0':
			sptr[0] = '\\';
			sptr[1] = '0';
			sptr += 2;
			break;
		default:
			if (c < 32)
			{
				sptr[0] = '\\';
				sptr[1] = 'u';
				sptr = Text::StrHexVal16(sptr + 2, c);
			}
			else
			{
				*sptr++ = c;
			}
			break;
		}
		if (sptr - sbuff >= 254)
		{
			this->sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sptr = sbuff;
		}
	}
	*sptr++ = '\"';
	this->sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
}

void Text::JSONBuilder::AppendStrW(const WChar *val)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UTF32Char c;
	sptr = sbuff;
	*sptr++ = '\"';
	while (true)
	{
		val = Text::StrReadChar(val, c);
		if (c == 0)
		{
			break;
		}
		else if (c == '\"')
		{
			*sptr++ = '\\';
			*sptr++ = '\\';
		}
		else if (c == '\"')
		{
			*sptr++ = '\\';
			*sptr++ = '\"';
		}
		else if (c == '\r')
		{
			*sptr++ = '\\';
			*sptr++ = 'r';
		}
		else if (c == '\n')
		{
			*sptr++ = '\\';
			*sptr++ = 'n';
		}
		else
		{
			sptr = Text::StrWriteChar(sptr, c);
		}
		if (sptr - sbuff >= 250)
		{
			this->sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sptr = sbuff;
		}
	}
	*sptr++ = '\"';
	this->sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
}

void Text::JSONBuilder::AppendDouble(Double val)
{
	if (Math::IsNAN(val))
	{
		this->sb.Append(CSTR("null"));
	}
	else
	{
		this->sb.AppendDouble(val);
	}
}

void Text::JSONBuilder::AppendTSStr(Data::Timestamp ts)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (ts.IsNull())
	{
		this->sb.AppendC(UTF8STRC("null"));
	}
	else
	{
		sptr = ts.ToString(sbuff);
		this->AppendStr(CSTRP(sbuff, sptr));
	}
}

void Text::JSONBuilder::AppendDateStr(Data::Date dat)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (dat.IsNull())
	{
		this->sb.AppendC(UTF8STRC("null"));
	}
	else
	{
		sptr = dat.ToString(sbuff);
		this->AppendStr(CSTRP(sbuff, sptr));
	}
}

void Text::JSONBuilder::AppendCoord2D(Math::Coord2DDbl coord)
{
	this->sb.AppendUTF8Char('[');
	this->AppendDouble(coord.x);
	this->sb.AppendUTF8Char(',');
	this->AppendDouble(coord.y);
	this->sb.AppendUTF8Char(']');
}

void Text::JSONBuilder::AppendCoord2DArray(const Math::Coord2DDbl *coordList, UOSInt nPoints)
{
	this->sb.AppendUTF8Char('[');
	if (nPoints > 0)
	{
		this->AppendCoord2D(coordList[0]);
		UOSInt i = 1;
		while (i < nPoints)
		{
			this->sb.AppendUTF8Char(',');
			this->AppendCoord2D(coordList[i]);
			i++;
		}
	}
	this->sb.AppendUTF8Char(']');
}

void Text::JSONBuilder::AppendCoordPL(NN<Math::Geometry::Polyline> pl)
{
	Data::ArrayIterator<NN<Math::Geometry::LineString>> it = pl->Iterator();
	this->sb.AppendUTF8Char('[');
	NN<Math::Geometry::LineString> ls;
	if (it.HasNext())
	{
		ls = it.Next();
		UOSInt nPoints;
		const Math::Coord2DDbl *ptList = ls->GetPointListRead(nPoints);
		this->AppendCoord2DArray(ptList, nPoints);
		while (it.HasNext())
		{
			ls = it.Next();
			UOSInt nPoints;
			const Math::Coord2DDbl *ptList = ls->GetPointListRead(nPoints);
			this->sb.AppendUTF8Char(',');
			this->AppendCoord2DArray(ptList, nPoints);
		}
	}
	this->sb.AppendUTF8Char(']');
}

void Text::JSONBuilder::AppendCoordPG(NN<Math::Geometry::Polygon> pg)
{
	Data::ArrayIterator<NN<Math::Geometry::LinearRing>> it = pg->Iterator();
	this->sb.AppendUTF8Char('[');
	NN<Math::Geometry::LinearRing> lr;
	if (it.HasNext())
	{
		lr = it.Next();
		UOSInt nPoints;
		const Math::Coord2DDbl *ptList = lr->GetPointListRead(nPoints);
		this->AppendCoord2DArray(ptList, nPoints);
		while (it.HasNext())
		{
			lr = it.Next();
			UOSInt nPoints;
			const Math::Coord2DDbl *ptList = lr->GetPointListRead(nPoints);
			this->sb.AppendUTF8Char(',');
			this->AppendCoord2DArray(ptList, nPoints);
		}
	}
	this->sb.AppendUTF8Char(']');
}

void Text::JSONBuilder::AppendGeometry(NN<Math::Geometry::Vector2D> vec)
{
	Math::Geometry::Vector2D::VectorType vecType = vec->GetVectorType();
	if (vecType == Math::Geometry::Vector2D::VectorType::Point)
	{
		NN<Math::Geometry::Point> pt = NN<Math::Geometry::Point>::ConvertFrom(vec);
		this->sb.AppendC(UTF8STRC("{\"type\":\"Point\",\"coordinates\":"));
		this->AppendCoord2D(pt->GetCenter());
		this->sb.AppendUTF8Char('}');
	}
	else if (vecType == Math::Geometry::Vector2D::VectorType::LineString)
	{
		NN<Math::Geometry::LineString> ls = NN<Math::Geometry::LineString>::ConvertFrom(vec);
		this->sb.AppendC(UTF8STRC("{\"type\":\"LineString\",\"coordinates\":"));
		UOSInt nPoints;
		const Math::Coord2DDbl *ptList = ls->GetPointListRead(nPoints);
		this->AppendCoord2DArray(ptList, nPoints);
		this->sb.AppendUTF8Char('}');
	}
	else if (vecType == Math::Geometry::Vector2D::VectorType::Polyline)
	{
		NN<Math::Geometry::Polyline> pl = NN<Math::Geometry::Polyline>::ConvertFrom(vec);
		this->sb.AppendC(UTF8STRC("{\"type\":\"MultiLineString\",\"coordinates\":"));
		this->AppendCoordPL(pl);
		this->sb.AppendUTF8Char('}');
	}
	else if (vecType == Math::Geometry::Vector2D::VectorType::Polygon)
	{
		NN<Math::Geometry::Polygon> pg = NN<Math::Geometry::Polygon>::ConvertFrom(vec);
		this->sb.AppendC(UTF8STRC("{\"type\":\"Polygon\",\"coordinates\":"));
		this->AppendCoordPG(pg);
		this->sb.AppendUTF8Char('}');
	}
	else if (vecType == Math::Geometry::Vector2D::VectorType::MultiPolygon)
	{
		NN<Math::Geometry::MultiPolygon> mpg = NN<Math::Geometry::MultiPolygon>::ConvertFrom(vec);
		this->sb.AppendC(UTF8STRC("{\"type\":\"MultiPolygon\",\"coordinates\":"));
		Data::ArrayIterator<NN<Math::Geometry::Polygon>> it = mpg->Iterator();
		this->sb.AppendUTF8Char('[');
		NN<Math::Geometry::Polygon> pg;
		if (it.HasNext())
		{
			pg = it.Next();
			this->AppendCoordPG(pg);
			while (it.HasNext())
			{
				pg = it.Next();
				this->sb.AppendUTF8Char(',');
				this->AppendCoordPG(pg);
			}
		}
		this->sb.AppendUTF8Char(']');
		this->sb.AppendUTF8Char('}');
	}
	else if (vecType == Math::Geometry::Vector2D::VectorType::CurvePolygon)
	{
		NN<Math::Geometry::CurvePolygon> cpg = NN<Math::Geometry::CurvePolygon>::ConvertFrom(vec);
		NN<Math::Geometry::Polygon> pg = NN<Math::Geometry::Polygon>::ConvertFrom(cpg->CurveToLine());
		this->sb.AppendC(UTF8STRC("{\"type\":\"Polygon\",\"coordinates\":"));
		this->AppendCoordPG(pg);
		this->sb.AppendUTF8Char('}');
		pg.Delete();
	}
	else
	{
		this->sb.AppendC(UTF8STRC("null"));
		printf("JSONBuilder: Unsupport Geometry Type: %s\r\n", Math::Geometry::Vector2D::VectorTypeGetName(vecType).v.Ptr());
	}
}

Text::JSONBuilder::JSONBuilder(ObjectType rootType)
{
	this->currType = rootType;
	this->isFirst = true;
	if (rootType == OT_ARRAY)
	{
		sb.AppendUTF8Char('[');
	}
	else
	{
		sb.AppendUTF8Char('{');
	}
}

Text::JSONBuilder::~JSONBuilder()
{
	this->currType = OT_END;
}

Bool Text::JSONBuilder::ArrayAddInt32(Int32 val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->sb.AppendI32(val);
	return true;
}

Bool Text::JSONBuilder::ArrayAddNInt32(NInt32 val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	if (val.IsNull())
		this->sb.Append(CSTR("null"));
	else
		this->sb.AppendI32(val.IntVal());
	return true;
}

Bool Text::JSONBuilder::ArrayAddInt64(Int64 val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->sb.AppendI64(val);
	return true;
}

Bool Text::JSONBuilder::ArrayAddFloat64(Double val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendDouble(val);
	return true;
}

Bool Text::JSONBuilder::ArrayAddBool(Bool val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->sb.Append(val?CSTR("true"):CSTR("false"));
	return true;
}

Bool Text::JSONBuilder::ArrayAddStr(Text::PString *val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	if (val == 0)
	{
		this->sb.AppendC(UTF8STRC("null"));
	}
	else
	{
		this->AppendStrUTF8(val->v);
	}
	return true;
}

Bool Text::JSONBuilder::ArrayAddStr(Text::CString val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	Text::CStringNN nnval;
	if (!val.SetTo(nnval))
	{
		this->sb.AppendC(UTF8STRC("null"));
	}
	else
	{
		this->AppendStrUTF8(nnval.v);
	}
	return true;
}

Bool Text::JSONBuilder::ArrayAddStr(NN<Text::String> val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStrUTF8(val->v);
	return true;
}

Bool Text::JSONBuilder::ArrayAddStrOpt(Optional<Text::String> val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	NN<Text::String> nnval;
	if (!val.SetTo(nnval))
	{
		this->sb.AppendC(UTF8STRC("null"));
	}
	else
	{
		this->AppendStrUTF8(nnval->v);
	}
	return true;

}

Bool Text::JSONBuilder::ArrayAddStrUTF8(UnsafeArrayOpt<const UTF8Char> val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	UnsafeArray<const UTF8Char> nnval;
	if (!val.SetTo(nnval))
	{
		this->sb.AppendC(UTF8STRC("null"));
	}
	else
	{
		this->AppendStrUTF8(nnval);
	}
	return true;
}

Bool Text::JSONBuilder::ArrayAddNull()
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->sb.AppendC(UTF8STRC("null"));
	return true;
}

Bool Text::JSONBuilder::ArrayAddCoord2D(Math::Coord2DDbl coord)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->sb.AppendUTF8Char('[');
	this->AppendDouble(coord.x);
	this->sb.AppendUTF8Char(',');
	this->AppendDouble(coord.y);
	this->sb.AppendUTF8Char(']');
	return true;
}

Bool Text::JSONBuilder::ArrayAddVector3(Math::Vector3 vec3)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->sb.AppendUTF8Char('[');
	this->AppendDouble(vec3.GetX());
	this->sb.AppendUTF8Char(',');
	this->AppendDouble(vec3.GetY());
	this->sb.AppendUTF8Char(',');
	this->AppendDouble(vec3.GetZ());
	this->sb.AppendUTF8Char(']');
	return true;
}

Bool Text::JSONBuilder::ArrayAdd(NN<Text::JSONArray> arr)
{
	if (this->currType != OT_ARRAY)
		return false;
	UOSInt i = 0;
	UOSInt j = arr->GetArrayLength();
	NN<Text::JSONBase> json;
	while (i < j)
	{
		if (arr->GetArrayValue(i).SetTo(json))
		{
			switch (json->GetType())
			{
			case Text::JSONType::Object:
				this->ArrayBeginObject();
				this->ObjectAdd(NN<Text::JSONObject>::ConvertFrom(json));
				this->ObjectEnd();
				break;
			case Text::JSONType::Array:
				this->ArrayBeginArray();
				this->ArrayAdd(NN<Text::JSONArray>::ConvertFrom(json));
				this->ArrayEnd();
				break;
			case Text::JSONType::Number:
				this->ArrayAddFloat64(((Text::JSONNumber*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::String:
				this->ArrayAddStr(((Text::JSONString*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::BOOL:
				this->ArrayAddBool(((Text::JSONBool*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::Null:
				this->ArrayAddNull();
				break;
			case Text::JSONType::INT32:
				this->ArrayAddInt32(((Text::JSONInt32*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::INT64:
				this->ArrayAddInt64(((Text::JSONInt64*)json.Ptr())->GetValue());
				break;
			}
		}
		else
		{
			this->ArrayAddNull();
		}
		i++;
	}
	return true;
}

Bool Text::JSONBuilder::ArrayBeginObject()
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->objTypes.Add(OT_ARRAY);
	this->currType = OT_OBJECT;
	this->isFirst = true;
	this->sb.AppendUTF8Char('{');
	return true;
}

Bool Text::JSONBuilder::ArrayBeginArray()
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->objTypes.Add(OT_ARRAY);
	this->currType = OT_ARRAY;
	this->isFirst = true;
	this->sb.AppendUTF8Char('[');
	return true;
}

Bool Text::JSONBuilder::ArrayEnd()
{
	if (this->currType != OT_ARRAY)
		return false;
	UOSInt i = this->objTypes.GetCount();
	if (i <= 0)
		return false;
	this->currType = this->objTypes.RemoveAt(i - 1);
	this->isFirst = false;
	this->sb.AppendUTF8Char(']');
	return true;
}

Bool Text::JSONBuilder::ObjectAddFloat64(Text::CStringNN name, Double val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":"));
	this->AppendDouble(val);
	return true;
}

Bool Text::JSONBuilder::ObjectAddInt32(Text::CStringNN name, Int32 val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":"));
	this->sb.AppendI32(val);
	return true;
}

Bool Text::JSONBuilder::ObjectAddNInt32(Text::CStringNN name, NInt32 val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":"));
	if (val.IsNull())
		this->sb.Append(CSTR("null"));
	else
		this->sb.AppendI32(val.IntVal());
	return true;
}

Bool Text::JSONBuilder::ObjectAddInt64(Text::CStringNN name, Int64 val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":"));
	this->sb.AppendI64(val);
	return true;
}

Bool Text::JSONBuilder::ObjectAddUInt64(Text::CStringNN name, UInt64 val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":"));
	this->sb.AppendU64(val);
	return true;
}

Bool Text::JSONBuilder::ObjectAddBool(Text::CStringNN name, Bool val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":"));
	this->sb.Append(val?CSTR("true"):CSTR("false"));
	return true;
}

Bool Text::JSONBuilder::ObjectAddStr(Text::CStringNN name, Text::PString *val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":"));
	if (val == 0)
	{
		this->sb.AppendC(UTF8STRC("null"));
	}
	else
	{
		this->AppendStr(val->ToCString());
	}
	return true;
}

Bool Text::JSONBuilder::ObjectAddStr(Text::CStringNN name, NN<const Text::String> val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":"));
	this->AppendStr(val->ToCString());
	return true;
}

Bool Text::JSONBuilder::ObjectAddStr(Text::CStringNN name, Text::CString val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":"));
	if (val.v == 0)
	{
		this->sb.AppendC(UTF8STRC("null"));
	}
	else
	{
		this->AppendStr(val.OrEmpty());
	}
	return true;
}

Bool Text::JSONBuilder::ObjectAddStrOpt(Text::CStringNN name, Optional<Text::String> val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":"));
	NN<Text::String> s;
	if (!val.SetTo(s))
	{
		this->sb.AppendC(UTF8STRC("null"));
	}
	else
	{
		this->AppendStr(s->ToCString());
	}
	return true;
}

Bool Text::JSONBuilder::ObjectAddStrUTF8(Text::CStringNN name, UnsafeArrayOpt<const UTF8Char> val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":"));
	UnsafeArray<const UTF8Char> nnval;
	if (!val.SetTo(nnval))
	{
		this->sb.AppendC(UTF8STRC("null"));
	}
	else
	{
		this->AppendStrUTF8(nnval);
	}
	return true;
}

Bool Text::JSONBuilder::ObjectAddStrW(Text::CStringNN name, const WChar *val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":"));
	if (val == 0)
	{
		this->sb.AppendC(UTF8STRC("null"));
	}
	else
	{
		this->AppendStrW(val);
	}
	return true;
}

Bool Text::JSONBuilder::ObjectAddTSStr(Text::CStringNN name, Data::Timestamp ts)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendUTF8Char(':');
	this->AppendTSStr(ts);
	return true;
}

Bool Text::JSONBuilder::ObjectAddDateStr(Text::CStringNN name, Data::Date dat)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendUTF8Char(':');
	this->AppendDateStr(dat);
	return true;
}

Bool Text::JSONBuilder::ObjectAddNull(Text::CStringNN name)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":null"));
	return true;
}

Bool Text::JSONBuilder::ObjectAddArrayInt32(Text::CStringNN name, Data::ArrayList<Int32> *i32Arr)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	if (i32Arr == 0)
	{
		this->sb.AppendC(UTF8STRC(":null"));
	}
	else
	{
		UOSInt i = 0;
		UOSInt j = i32Arr->GetCount();
		this->sb.AppendC(UTF8STRC(":["));
		while (i < j)
		{
			if (i > 0)
				sb.AppendUTF8Char(',');
			sb.AppendI32(i32Arr->GetItem(i));
			i++;
		}
		this->sb.AppendUTF8Char(']');
	}
	return true;
}

Bool Text::JSONBuilder::ObjectAddCoord2D(Text::CStringNN name, Math::Coord2DDbl coord)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendUTF8Char(':');
	this->AppendCoord2D(coord);
	return true;
}

Bool Text::JSONBuilder::ObjectAddArrayCoord2D(Text::CStringNN name, Data::ArrayListA<Math::Coord2DDbl> *coordArr)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	if (coordArr == 0)
	{
		this->sb.AppendC(UTF8STRC(":null"));
	}
	else
	{
		Math::Coord2DDbl coord;
		UOSInt i = 0;
		UOSInt j = coordArr->GetCount();
		this->sb.AppendC(UTF8STRC(":["));
		while (i < j)
		{
			coord = coordArr->GetItem(i);
			if (i > 0)
				sb.AppendUTF8Char(',');
			this->AppendCoord2D(coord);
			i++;
		}
		this->sb.AppendUTF8Char(']');
	}
	return true;
}

Bool Text::JSONBuilder::ObjectAddVector3(Text::CStringNN name, Math::Vector3 vec3)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":["));
	this->AppendDouble(vec3.GetX());
	this->sb.AppendUTF8Char(',');
	this->AppendDouble(vec3.GetY());
	this->sb.AppendUTF8Char(',');
	this->AppendDouble(vec3.GetZ());
	this->sb.AppendUTF8Char(']');
	return true;
}

Bool Text::JSONBuilder::ObjectAddGeometry(Text::CStringNN name, NN<Math::Geometry::Vector2D> vec)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendUTF8Char(':');
	this->AppendGeometry(vec);
	return true;
}

Bool Text::JSONBuilder::ObjectAddGeometryOpt(Text::CStringNN name, Optional<Math::Geometry::Vector2D> vec)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendUTF8Char(':');
	NN<Math::Geometry::Vector2D> nnvec;
	if (vec.SetTo(nnvec))
	{
		this->AppendGeometry(nnvec);
	}
	else
	{
		this->sb.AppendC(UTF8STRC("null"));
	}
	return true;
}

Bool Text::JSONBuilder::ObjectAddVarItem(Text::CStringNN name, NN<Data::VariItem> item)
{
	if (this->currType != OT_OBJECT)
		return false;
	switch (item->GetItemType())
	{
	default:
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::UUID:
		return ObjectAddNull(name);
	case Data::VariItem::ItemType::Timestamp:
		return ObjectAddTSStr(name, item->GetAsTimestamp());
	case Data::VariItem::ItemType::F32:
	case Data::VariItem::ItemType::F64:
		return ObjectAddFloat64(name, item->GetAsF64());
	case Data::VariItem::ItemType::I8:
	case Data::VariItem::ItemType::U8:
	case Data::VariItem::ItemType::I16:
	case Data::VariItem::ItemType::U16:
	case Data::VariItem::ItemType::I32:
		return ObjectAddInt32(name, item->GetAsI32());
	case Data::VariItem::ItemType::U32:
		return ObjectAddUInt64(name, item->GetAsU32());
	case Data::VariItem::ItemType::NI32:
		return ObjectAddNInt32(name, item->GetAsNI32());
	case Data::VariItem::ItemType::I64:
		return ObjectAddInt64(name, item->GetAsI64());
	case Data::VariItem::ItemType::U64:
		return ObjectAddUInt64(name, item->GetAsU64());
	case Data::VariItem::ItemType::BOOL:
		return ObjectAddUInt64(name, item->GetAsBool());
	case Data::VariItem::ItemType::CStr:
	{
		Data::VariItem::ItemValue v = item->GetItemValue();
		return ObjectAddStr(name, Text::CString(v.cstr.v, v.cstr.leng));
	}
	case Data::VariItem::ItemType::Date:
		return ObjectAddDateStr(name, item->GetAsDate());
	case Data::VariItem::ItemType::Str:
		return ObjectAddStr(name, item->GetItemValue().str);
	case Data::VariItem::ItemType::Vector:
		return ObjectAddGeometry(name, item->GetItemValue().vector);
	}
}

Bool Text::JSONBuilder::ObjectAdd(NN<Text::JSONObject> obj)
{
	if (this->currType != OT_OBJECT)
		return false;
	Data::ArrayListNN<Text::String> names;
	obj->GetObjectNames(names);
	NN<Text::JSONBase> json;
	NN<Text::String> name;
	Data::ArrayIterator<NN<Text::String>> it = names.Iterator();
	while (it.HasNext())
	{
		name = it.Next();
		if (obj->GetObjectValue(name->ToCString()).SetTo(json))
		{
			switch (json->GetType())
			{
			case Text::JSONType::Object:
				this->ObjectBeginObject(name->ToCString());
				this->ObjectAdd(NN<Text::JSONObject>::ConvertFrom(json));
				this->ObjectEnd();
				break;
			case Text::JSONType::Array:
				this->ObjectBeginArray(name->ToCString());
				this->ArrayAdd(NN<Text::JSONArray>::ConvertFrom(json));
				this->ArrayEnd();
				break;
			case Text::JSONType::Number:
				this->ObjectAddFloat64(name->ToCString(), ((Text::JSONNumber*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::String:
				this->ObjectAddStr(name->ToCString(), ((Text::JSONString*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::BOOL:
				this->ObjectAddBool(name->ToCString(), ((Text::JSONBool*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::Null:
				this->ObjectAddNull(name->ToCString());
				break;
			case Text::JSONType::INT32:
				this->ObjectAddInt32(name->ToCString(), ((Text::JSONInt32*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::INT64:
				this->ObjectAddInt64(name->ToCString(), ((Text::JSONInt64*)json.Ptr())->GetValue());
				break;
			}
		}
	}
	return true;
}

Bool Text::JSONBuilder::ObjectBeginArray(Text::CStringNN name)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":["));
	this->objTypes.Add(OT_OBJECT);
	this->currType = OT_ARRAY;
	this->isFirst = true;
	return true;
}

Bool Text::JSONBuilder::ObjectBeginObject(Text::CStringNN name)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb.AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb.AppendC(UTF8STRC(":{"));
	this->objTypes.Add(OT_OBJECT);
	this->currType = OT_OBJECT;
	this->isFirst = true;
	return true;
}

Bool Text::JSONBuilder::ObjectEnd()
{
	if (this->currType != OT_OBJECT)
		return false;
	UOSInt i = this->objTypes.GetCount();
	if (i <= 0)
		return false;
	this->currType = this->objTypes.RemoveAt(i - 1);
	this->isFirst = false;
	this->sb.AppendUTF8Char('}');
	return true;
}

Text::CStringNN Text::JSONBuilder::Build()
{
	UOSInt i;
	if (this->currType == OT_ARRAY)
	{
		this->sb.AppendUTF8Char(']');
	}
	else if (this->currType == OT_OBJECT)
	{
		this->sb.AppendUTF8Char('}');
	}
	i = this->objTypes.GetCount();
	while (i-- > 0)
	{
		if (this->objTypes.GetItem(i) == OT_OBJECT)
		{
			this->sb.AppendUTF8Char('}');
		}
		else
		{
			this->sb.AppendUTF8Char(']');
		}
	}
	this->objTypes.Clear();
	return this->sb.ToCString();
}
