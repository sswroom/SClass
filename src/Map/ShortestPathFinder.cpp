#include "Stdafx.h"
#include "Map/ShortestPathFinder.h"
#include "Math/Math_C.h"
#include "Text/MyString.h"

Int64 Map::ShortestPathFinder::CoordToId(Double x, Double y)
{
	if (cu == CU_DEGREE)
	{
		return (Double2Int64(x * 1000000.0) << 32) | (UInt32)Double2Int32(y * 1000000.0);
	}
	else
	{
		return (Double2Int64(x * 10.0) << 32) | (UInt32)Double2Int32(y * 10.0);
	}
}

Bool Map::ShortestPathFinder::SearchShortestPath(NN<Data::ArrayListNative<Double>> pointList, NN<Map::GetObjectSess> sess, Int64 fromObjId, Double fromX, Double fromY, Int64 toObjId, Double toX, Double toY)
{
	UIntOS i;
	Math::Geometry::Polyline *pl;
	UIntOS fromPointNo;
	UIntOS toPointNo;
	Int64 toId1;
	Int64 toId2;
	UTF8Char sbuff1[256];
	UTF8Char sbuff2[256];
	sbuff1[0] = 0;
	sbuff2[0] = 0;
	this->layer->GetString(sbuff1, sizeof(sbuff1), this->nameArr, fromObjId, this->nameCol);
	this->layer->GetString(sbuff2, sizeof(sbuff2), this->nameArr, toObjId, this->nameCol);
	if (Text::StrEquals(sbuff1, sbuff2))
	{
		Double dist1;
		Double dist2;
		Double xDiff;
		Double yDiff;
		Double *points;
		UIntOS nPoints;
		Int64 nextId;
		Int64 lastId;
		IntOS pathCnt;
		NodeInfo *node;
		NeighbourInfo *neighbour;
		Bool valid = true;
		pl = (Math::Geometry::Polyline*)this->layer->GetNewVectorById(sess, toObjId);
		toPointNo = pl->GetPointNo(toX, toY, 0, 0, 0);
		points = pl->GetPointList(&nPoints);
		toId1 = this->CoordToId(points[0], points[1]);

		Bool bothDir = true;
		if (this->dirCol >= 0)
		{
			if (layer->GetString(sbuff2, sizeof(sbuff2), nameArr, toObjId, dirCol))
			{
				if (Text::StrToInt32(sbuff2) == 3)
				{
					bothDir = false;
				}
			}
		}
		if (bothDir)
		{
			toId2 = this->CoordToId(points[nPoints * 2 - 2], points[nPoints * 2 - 1]);
		}
		else
		{
			toId2 = toId1;
		}
		DEL_CLASS(pl);

		pl = (Math::Polyline*)this->layer->GetNewVectorById(sess, fromObjId);
		points = pl->GetPointList(&nPoints);
		fromPointNo = pl->GetPointNo(fromX, fromY, 0, 0, 0);

		bothDir = false;
		if (this->dirCol >= 0)
		{
			if (layer->GetString(sbuff2, sizeof(sbuff2), nameArr, fromObjId, dirCol))
			{
				if (Text::StrToInt32(sbuff2) == 3)
				{
					bothDir = false;
				}
			}
		}
		if (bothDir)
		{
			xDiff = points[0] - toX;
			yDiff = points[1] - toY;
			dist1 = xDiff * xDiff - yDiff * yDiff;
			xDiff = points[nPoints * 2 - 2] - toX;
			yDiff = points[nPoints * 2 - 1] - toY;
			dist2 = xDiff * xDiff - yDiff * yDiff;
			if (dist1 < dist2)
			{
				pointList->Add(fromX);
				pointList->Add(fromY);
				i = fromPointNo;
				while (i >= 0)
				{
					pointList->Add(points[i * 2]);
					pointList->Add(points[i * 2 + 1]);
					i--;
				}
				nextId = this->CoordToId(points[0], points[1]);
				lastId = this->CoordToId(points[nPoints * 2 - 2], points[nPoints * 2 - 1]);
			}
			else
			{
				pointList->Add(fromX);
				pointList->Add(fromY);
				i = fromPointNo + 1;
				while (i < nPoints)
				{
					pointList->Add(points[i * 2]);
					pointList->Add(points[i * 2 + 1]);
					i++;
				}
				nextId = this->CoordToId(points[nPoints * 2 - 2], points[nPoints * 2 - 1]);
				lastId = this->CoordToId(points[0], points[1]);
			}
		}
		else
		{
			nextId = this->CoordToId(points[nPoints * 2 - 2], points[nPoints * 2 - 1]);
			lastId = this->CoordToId(points[0], points[1]);
		}
		DEL_CLASS(pl);
		pathCnt = 0;
		while (true)
		{
			if (nextId == toId1)
			{
				pl = (Math::Polyline*)this->layer->GetNewVectorById(sess, toObjId);
				points = pl->GetPointList(&nPoints);
				i = 1;
				while (i <= toPointNo)
				{
					pointList->Add(points[i * 2]);
					pointList->Add(points[i * 2 + 1]);
					i++;
				}
				pointList->Add(toX);
				pointList->Add(toY);
				DEL_CLASS(pl);
				break;
			}
			else if (nextId == toId2)
			{
				pl = (Math::Polyline*)this->layer->GetNewVectorById(sess, toObjId);
				points = pl->GetPointList(&nPoints);
				i = nPoints - 1;
				while (i > toPointNo)
				{
					pointList->Add(points[i * 2]);
					pointList->Add(points[i * 2 + 1]);
					i--;
				}
				pointList->Add(toX);
				pointList->Add(toY);
				DEL_CLASS(pl);
				break;
			}
			else
			{
				Bool found = false;
				node = this->nodeMap->Get(nextId);
				i = node->neighbours->GetCount();
				while (i-- > 0)
				{
					neighbour = node->neighbours->GetItem(i);
					if (this->CoordToId(neighbour->x, neighbour->y) != lastId && neighbour->name && Text::StrEquals(sbuff1, neighbour->name))
					{
						lastId = nextId;
						nextId = this->CoordToId(neighbour->x, neighbour->y);
						points = neighbour->pl->GetPointList(nPoints);
						if (neighbour->isReversed)
						{
							i = nPoints - 1;
							while (i-- > 0)
							{
								pointList->Add(points[i * 2]);
								pointList->Add(points[i * 2 + 1]);
							}
						}
						else
						{
							i = 1;
							while (i < nPoints)
							{
								pointList->Add(points[i * 2]);
								pointList->Add(points[i * 2 + 1]);
								i++;
							}
						}
						pathCnt++;
						found = true;
						break;
					}
				}
				if (!found)
				{
					valid = false;
					break;
				}
				if (pathCnt > 20)
				{
					valid = false;
					break;
				}
			}
		}
		if (valid)
		{
			return true;
		}
		else
		{
			pointList->Clear();
			return false;
		}
	}
	else
	{
		return false;
	}
}

Map::ShortestPathFinder::ShortestPathFinder(NN<Map::MapDrawLayer> layer, Bool toRelease, CoordinateUnit cu, IntOS nameCol, IntOS dirCol)
{
	this->layer = layer;
	this->toRelease = toRelease;
	this->cu = cu;
	this->nameCol = nameCol;
	this->dirCol = dirCol;
	NEW_CLASSNN(this->nodeMap, Data::Int64FastMapNN<NodeInfo>());

	UTF8Char sbuff[256];
	Data::ArrayListInt64 idList;
	NN<Math::Geometry::Vector2D> vec;
	NN<Math::Geometry::Polyline> pl;
	NN<NodeInfo> fromNode;
	NN<NodeInfo> toNode;
	Int64 fromId;
	Int64 toId;
	NN<NeighbourInfo> frNeighbour;
	NN<NeighbourInfo> toNeighbour;
	Double dist;
	UIntOS nPoints;
	Double *points;
	Double xDiff;
	Double yDiff;
	NN<Map::GetObjectSess> sess;
	UIntOS i;
	UIntOS j;
	UIntOS k;
	layer->GetAllObjectIds(idList, this->nameArr);
	sess = layer->BeginGetObject();
	i = 0;
	j = idList.GetCount();
	while (i < j)
	{
		if (layer->GetNewVectorById(sess, idList.GetItem(i)).SetTo(vec))
		{
			if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polyline)
			{
				pl = NN<Math::Geometry::Polyline>::ConvertFrom(vec);
				points = pl->GetPointList(nPoints);
				dist = 0;
				k = 1;
				while (k < nPoints)
				{
					xDiff = points[k * 2 - 2] - points[k * 2];
					yDiff = points[k * 2 - 1] - points[k * 2 + 1];
					dist += Math_Sqrt(xDiff * xDiff + yDiff * yDiff);
					k++;
				}
				fromId = this->CoordToId(points[0], points[1]);
				toId = this->CoordToId(points[nPoints * 2 - 2], points[nPoints * 2 - 1]);
				if (!this->nodeMap->Get(fromId).SetTo(fromNode))
				{
					fromNode = MemAllocNN(NodeInfo);
					fromNode->x = points[0];
					fromNode->y = points[1];
					NEW_CLASSNN(fromNode->neighbours, Data::ArrayListNN<NeighbourInfo>());
					this->nodeMap->Put(fromId, fromNode);
				}
				if (!this->nodeMap->Get(toId).SetTo(toNode))
				{
					toNode = MemAllocNN(NodeInfo);
					toNode->x = points[nPoints * 2 - 2];
					toNode->y = points[nPoints * 2 - 1];
					NEW_CLASSNN(toNode->neighbours, Data::ArrayListNN<NeighbourInfo>());
					this->nodeMap->Put(toId, toNode);
				}

				frNeighbour = MemAllocNN(NeighbourInfo);
				frNeighbour->x = points[nPoints * 2 - 2];
				frNeighbour->y = points[nPoints * 2 - 1];
				frNeighbour->isReversed = false;
				frNeighbour->pl = pl;
				frNeighbour->dist = dist;
				frNeighbour->name = nullptr;

				Bool bothDir = true;
				if (this->dirCol >= 0)
				{
					if (layer->GetString(sbuff, sizeof(sbuff), nameArr, idList.GetItem(i), dirCol))
					{
						if (Text::StrToInt32(sbuff) == 3)
						{
							bothDir = false;
						}
					}
				}

				if (bothDir)
				{
					toNeighbour = MemAllocNN(NeighbourInfo);
					toNeighbour->x = points[0];
					toNeighbour->y = points[1];
					toNeighbour->isReversed = true;
					toNeighbour->pl = pl;
					toNeighbour->dist = dist;
					toNeighbour->name = nullptr;
				}

				if (layer->GetString(sbuff, sizeof(sbuff), nameArr, idList.GetItem(i), nameCol))
				{
					frNeighbour->name = Text::StrCopyNew(sbuff);
					if (bothDir)
					{
						toNeighbour->name = Text::StrCopyNew(sbuff);
					}
				}
				fromNode->neighbours->Add(frNeighbour);
				if (bothDir)
				{
					toNode->neighbours->Add(toNeighbour);
				}
			}
			else
			{
				vec.Delete();
			}
		}
		i++;
	}
	layer->EndGetObject(sess);
}

Map::ShortestPathFinder::~ShortestPathFinder()
{
	this->layer->ReleaseNameArr(this->nameArr);
	if (this->toRelease)
	{
		this->layer.Delete();
	}
	IntOS i;
	IntOS j;
	NN<NodeInfo> node;
	NN<NeighbourInfo>neighbour;
	i = this->nodeMap->GetCount();
	while (i-- > 0)
	{
		node = this->nodeMap->GetItemNoCheck(i);
		j = node->neighbours->GetCount();
		while (j-- > 0)
		{
			neighbour = node->neighbours->GetItemNoCheck(j);
			if (!neighbour->isReversed)
			{
				DEL_CLASS(neighbour->pl);
			}
			SDEL_TEXT(neighbour->name);
			MemFreeNN(neighbour);
		}
	}
	this->nodeMap.Delete();
}

Optional<Math::Geometry::LineString> Map::ShortestPathFinder::GetPath(Double fromX, Double fromY, Double toX, Double toY, Bool sameName)
{
	Math::Geometry::LineString *retPl = 0;
	Data::ArrayListNative<Double> pointList;
	IntOS i;
	IntOS j;
	IntOS fromCnt;
	IntOS toCnt;
	UIntOS nPoints;
	Data::ArrayListNN<Map::MapDrawLayer::ObjectInfo> fromObjs;
	Data::ArrayListNN<Map::MapDrawLayer::ObjectInfo> toObjs;
	NN<Map::MapDrawLayer::ObjectInfo> fromObj;
	NN<Map::MapDrawLayer::ObjectInfo> toObj;
	NN<Map::GetObjectSess> sess;
	sess = this->layer->BeginGetObject();
	if (this->cu == Map::ShortestPathFinder::CU_METER)
	{
		this->layer->GetNearObjects(sess, fromObjs, Math::Coord2DDbl(fromX, fromY), 30);
		this->layer->GetNearObjects(sess, toObjs, Math::Coord2DDbl(toX, toY), 30);
	}
	else
	{
		this->layer->GetNearObjects(sess, fromObjs, Math::Coord2DDbl(fromX, fromY), 0.00015);
		this->layer->GetNearObjects(sess, toObjs, Math::Coord2DDbl(toX, toY), 0.00015);
	}
	fromCnt = fromObjs.GetCount();
	toCnt = toObjs.GetCount();
	i = 0;
	while (i < fromCnt)
	{
		fromObj = fromObjs.GetItemNoCheck(i);
		j = 0;
		while (j < toCnt && j <= i)
		{
			toObj = toObjs.GetItemNoCheck(j);
			pointList.Clear();
			if (this->SearchShortestPath(pointList, sess, fromObj->objId, fromObj->objX, fromObj->objY, toObj->objId, toObj->objX, toObj->objY))
			{
				NEW_CLASS(retPl, Math::Geometry::LineString(0, UnsafeArray<const Math::Coord2DDbl>::ConvertFrom(pointList.GetArr(nPoints)), pointList.GetCount() >> 1, nullptr, nullptr));
				break;
			}
			j++;
		}
		if (retPl)
			break;
		i++;
	}
	if (retPl == 0)
	{
		j = fromCnt + 1;
		while (j < toCnt)
		{
			toObj = toObjs.GetItemNoCheck(j);
			i = 0;
			while (i < fromCnt)
			{
				fromObj = fromObjs.GetItemNoCheck(i);
				pointList.Clear();
				if (this->SearchShortestPath(pointList, sess, fromObj->objId, fromObj->objX, fromObj->objY, toObj->objId, toObj->objX, toObj->objY))
				{
					NEW_CLASS(retPl, Math::Geometry::LineString(0, UnsafeArray<const Math::Coord2DDbl>::ConvertFrom(pointList.GetArr(nPoints)), pointList.GetCount() >> 1, nullptr, nullptr));
					break;
				}
				i++;
			}
			if (retPl)
				break;
			j++;
		}
	}
	this->layer->FreeObjects(fromObjs);
	this->layer->FreeObjects(toObjs);
	this->layer->EndGetObject(sess);
	if (retPl == 0)
	{
		return nullptr;
	}
	return retPl;
}
