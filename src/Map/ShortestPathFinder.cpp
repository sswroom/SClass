#include "Stdafx.h"
#include "Map/ShortestPathFinder.h"
#include "Math/Math.h"
#include "Text/MyString.h"

Int64 Map::ShortestPathFinder::CoordToId(Double x, Double y)
{
	if (cu == CU_DEGREE)
	{
		return (Math::Double2Int64(x * 1000000.0) << 32) | (UInt32)Math::Double2Int32(y * 1000000.0);
	}
	else
	{
		return (Math::Double2Int64(x * 10.0) << 32) | (UInt32)Math::Double2Int32(y * 10.0);
	}
}

Bool Map::ShortestPathFinder::SearchShortestPath(Data::ArrayList<Double> *pointList, void *sess, Int64 fromObjId, Double fromX, Double fromY, Int64 toObjId, Double toX, Double toY)
{
	UOSInt i;
	Math::Polyline *pl;
	UOSInt fromPointNo;
	UOSInt toPointNo;
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
		UOSInt nPoints;
		Int64 nextId;
		Int64 lastId;
		OSInt pathCnt;
		NodeInfo *node;
		NeighbourInfo *neighbour;
		Bool valid = true;
		pl = (Math::Polyline*)this->layer->GetVectorById(sess, toObjId);
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

		pl = (Math::Polyline*)this->layer->GetVectorById(sess, fromObjId);
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
				pl = (Math::Polyline*)this->layer->GetVectorById(sess, toObjId);
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
				pl = (Math::Polyline*)this->layer->GetVectorById(sess, toObjId);
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
						points = neighbour->pl->GetPointList(&nPoints);
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

Map::ShortestPathFinder::ShortestPathFinder(Map::IMapDrawLayer *layer, Bool toRelease, CoordinateUnit cu, OSInt nameCol, OSInt dirCol)
{
	this->layer = layer;
	this->toRelease = toRelease;
	this->cu = cu;
	this->nameCol = nameCol;
	this->dirCol = dirCol;
	NEW_CLASS(this->nodeMap, Data::Int64Map<NodeInfo*>());

	UTF8Char sbuff[256];
	Data::ArrayListInt64 idList;
	Math::Vector2D *vec;
	Math::Polyline *pl;
	NodeInfo *fromNode;
	NodeInfo *toNode;
	Int64 fromId;
	Int64 toId;
	NeighbourInfo *frNeighbour;
	NeighbourInfo *toNeighbour;
	Double dist;
	UOSInt nPoints;
	Double *points;
	Double xDiff;
	Double yDiff;
	void *sess;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	layer->GetAllObjectIds(&idList, &this->nameArr);
	sess = layer->BeginGetObject();
	i = 0;
	j = idList.GetCount();
	while (i < j)
	{
		vec = layer->GetVectorById(sess, idList.GetItem(i));
		if (vec)
		{
			if (vec->GetVectorType() == Math::Vector2D::VectorType::Polyline)
			{
				pl = (Math::Polyline *)vec;
				points = pl->GetPointList(&nPoints);
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
				fromNode = this->nodeMap->Get(fromId);
				toNode = this->nodeMap->Get(toId);
				if (fromNode == 0)
				{
					fromNode = MemAlloc(NodeInfo, 1);
					fromNode->x = points[0];
					fromNode->y = points[1];
					NEW_CLASS(fromNode->neighbours, Data::ArrayList<NeighbourInfo*>());
					this->nodeMap->Put(fromId, fromNode);
				}
				if (toNode == 0)
				{
					toNode = MemAlloc(NodeInfo, 1);
					toNode->x = points[nPoints * 2 - 2];
					toNode->y = points[nPoints * 2 - 1];
					NEW_CLASS(toNode->neighbours, Data::ArrayList<NeighbourInfo*>());
					this->nodeMap->Put(toId, toNode);
				}

				frNeighbour = MemAlloc(NeighbourInfo, 1);
				frNeighbour->x = points[nPoints * 2 - 2];
				frNeighbour->y = points[nPoints * 2 - 1];
				frNeighbour->isReversed = false;
				frNeighbour->pl = pl;
				frNeighbour->dist = dist;
				frNeighbour->name = 0;

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
					toNeighbour = MemAlloc(NeighbourInfo, 1);
					toNeighbour->x = points[0];
					toNeighbour->y = points[1];
					toNeighbour->isReversed = true;
					toNeighbour->pl = pl;
					toNeighbour->dist = dist;
					toNeighbour->name = 0;
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
				DEL_CLASS(vec);
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
		DEL_CLASS(this->layer);
	}
	OSInt i;
	OSInt j;
	NodeInfo *node;
	NeighbourInfo *neighbour;
	Data::ArrayList<NodeInfo*> *nodeList = this->nodeMap->GetValues();
	i = nodeList->GetCount();
	while (i-- > 0)
	{
		node = nodeList->GetItem(i);
		j = node->neighbours->GetCount();
		while (j-- > 0)
		{
			neighbour = node->neighbours->GetItem(j);
			if (!neighbour->isReversed)
			{
				DEL_CLASS(neighbour->pl);
			}
			SDEL_TEXT(neighbour->name);
			MemFree(neighbour);
		}
	}
	DEL_CLASS(this->nodeMap);
}

Math::Polyline *Map::ShortestPathFinder::GetPath(Double fromX, Double fromY, Double toX, Double toY, Bool sameName)
{
	Math::Polyline *retPl = 0;
	Data::ArrayList<Double> pointList;
	OSInt i;
	OSInt j;
	OSInt fromCnt;
	OSInt toCnt;
	UOSInt nPoints;
	Data::ArrayList<Map::IMapDrawLayer::ObjectInfo *> fromObjs;
	Data::ArrayList<Map::IMapDrawLayer::ObjectInfo *> toObjs;
	Map::IMapDrawLayer::ObjectInfo *fromObj;
	Map::IMapDrawLayer::ObjectInfo *toObj;
	void *sess;
	sess = this->layer->BeginGetObject();
	if (this->cu == Map::ShortestPathFinder::CU_METER)
	{
		this->layer->GetNearObjects(sess, &fromObjs, fromX, fromY, 30);
		this->layer->GetNearObjects(sess, &toObjs, toX, toY, 30);
	}
	else
	{
		this->layer->GetNearObjects(sess, &fromObjs, fromX, fromY, 0.00015);
		this->layer->GetNearObjects(sess, &toObjs, toX, toY, 0.00015);
	}
	fromCnt = fromObjs.GetCount();
	toCnt = toObjs.GetCount();
	i = 0;
	while (i < fromCnt)
	{
		fromObj = fromObjs.GetItem(i);
		j = 0;
		while (j < toCnt && j <= i)
		{
			toObj = toObjs.GetItem(j);
			pointList.Clear();
			if (this->SearchShortestPath(&pointList, sess, fromObj->objId, fromObj->objX, fromObj->objY, toObj->objId, toObj->objX, toObj->objY))
			{
				NEW_CLASS(retPl, Math::Polyline(0, pointList.GetArray(&nPoints), pointList.GetCount() >> 1));
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
			toObj = toObjs.GetItem(j);
			i = 0;
			while (i < fromCnt)
			{
				fromObj = fromObjs.GetItem(i);
				pointList.Clear();
				if (this->SearchShortestPath(&pointList, sess, fromObj->objId, fromObj->objX, fromObj->objY, toObj->objId, toObj->objX, toObj->objY))
				{
					NEW_CLASS(retPl, Math::Polyline(0, pointList.GetArray(&nPoints), pointList.GetCount() >> 1));
					break;
				}
				i++;
			}
			if (retPl)
				break;
			j++;
		}
	}
	this->layer->FreeObjects(&fromObjs);
	this->layer->FreeObjects(&toObjs);
	this->layer->EndGetObject(sess);
	if (retPl == 0)
	{
		return 0;
	}
	return retPl;
}
