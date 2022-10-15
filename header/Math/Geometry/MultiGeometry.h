#ifndef _SM_MATH_GEOMETRY_MULTIGEOMETRY
#define _SM_MATH_GEOMETRY_MULTIGEOMETRY
#include "Data/ArrayList.h"
#include "Math/Coord2DDbl.h"
#include "Math/CoordinateSystem.h"
#include "Math/Geometry/Vector2D.h"

namespace Math
{
	namespace Geometry
	{
		template <class T> class MultiGeometry : public Math::Geometry::Vector2D
		{
		protected:
			Data::ArrayList<T*> geometries;
			Bool hasZ;
			Bool hasM;
		public:
			MultiGeometry(UInt32 srid, Bool hasZ, Bool hasM) : Math::Geometry::Vector2D(srid)
			{
				this->hasZ = hasZ;
				this->hasM = hasM;
			}

			virtual ~MultiGeometry()
			{
				UOSInt i = this->geometries.GetCount();
				T *o;
				while (i-- > 0)
				{
					o = this->geometries.GetItem(i);
					DEL_CLASS(o);
				}
			}

			virtual void AddGeometry(T *geometry)
			{
				this->geometries.Add(geometry);
			}

			UOSInt GetCount() const
			{
				return this->geometries.GetCount();
			}

			T *GetItem(UOSInt index) const
			{
				return this->geometries.GetItem(index);
			}

			virtual Math::Coord2DDbl GetCenter() const
			{
				Math::RectAreaDbl bounds;
				this->GetBounds(&bounds);
				return (bounds.tl + bounds.br) * 0.5;
			}

			virtual void GetBounds(Math::RectAreaDbl *bounds) const
			{
				UOSInt i = 1;
				UOSInt j = this->geometries.GetCount();
				if (j == 0)
				{
					bounds->tl = Math::Coord2DDbl(0, 0);
					bounds->br = Math::Coord2DDbl(0, 0);
				}
				else
				{
					Math::RectAreaDbl thisBounds;
					this->geometries.GetItem(0)->GetBounds(bounds);
					while (i < j)
					{
						this->geometries.GetItem(i)->GetBounds(&thisBounds);
						bounds->tl = bounds->tl.Min(thisBounds.tl);
						bounds->br = bounds->br.Max(thisBounds.br);
						i++;
					}
				}
			}

			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const
			{
				UOSInt j = this->geometries.GetCount();
				if (j == 0)
				{
					*nearPt = Math::Coord2DDbl(0, 0);
					return 1000000000;
				}
				Math::Coord2DDbl minPt;
				Double minDist = this->geometries.GetItem(0)->CalBoundarySqrDistance(pt, &minPt);
				Math::Coord2DDbl thisPt;
				Double thisDist;
				UOSInt i = 1;
				while (i < j)
				{
					thisDist = this->geometries.GetItem(i)->CalBoundarySqrDistance(pt, &thisPt);
					if (minDist > thisDist)
					{
						minDist = thisDist;
						minPt = thisPt;
					}
					i++;
				}
				*nearPt = minPt;
				return minDist;
			}

			virtual Double CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const
			{
				UOSInt j = this->geometries.GetCount();
				if (j == 0)
				{
					*nearPt = Math::Coord2DDbl(0, 0);
					return 1000000000;
				}
				Math::Coord2DDbl minPt;
				Double minDist = this->geometries.GetItem(0)->CalSqrDistance(pt, &minPt);
				Math::Coord2DDbl thisPt;
				Double thisDist;
				UOSInt i = 1;
				while (i < j)
				{
					thisDist = this->geometries.GetItem(i)->CalSqrDistance(pt, &thisPt);
					if (minDist > thisDist)
					{
						minDist = thisDist;
						minPt = thisPt;
					}
					i++;
				}
				*nearPt = minPt;
				return minDist;
			}

			virtual Bool JoinVector(Math::Geometry::Vector2D *vec)
			{
				if (this->GetVectorType() != vec->GetVectorType())
				{
					return false;
				}
				Math::Geometry::MultiGeometry<T> *obj = (Math::Geometry::MultiGeometry<T> *)vec;
				UOSInt i = 0;
				UOSInt j = obj->GetCount();
				while (i < j)
				{
					this->AddGeometry((T*)obj->GetItem(i)->Clone());
					i++;
				}
				return true;
			}

			virtual Bool HasZ() const
			{
				return this->hasZ;
			}

			virtual Bool HasM() const
			{
				return this->hasM;
			}

			virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys)
			{
				UOSInt i = this->GetCount();
				while (i-- > 0)
				{
					this->GetItem(i)->ConvCSys(srcCSys, destCSys);
				}
				this->srid = destCSys->GetSRID();
			}

			virtual Bool Equals(Vector2D *vec) const
			{
				if (this->GetVectorType() != vec->GetVectorType())
				{
					return false;
				}
				Math::Geometry::MultiGeometry<T> *obj = (Math::Geometry::MultiGeometry<T> *)vec;
				if (obj->GetCount() != this->GetCount())
					return false;
				UOSInt i = this->GetCount();
				while (i-- > 0)
				{
					if (!this->GetItem(i)->Equals(obj->GetItem(i)))
						return false;
				}
				return true;
			}

			virtual UOSInt GetCoordinates(Data::ArrayListA<Math::Coord2DDbl> *coordList) const
			{
				UOSInt ret = 0;
				UOSInt i = 0;
				UOSInt j = this->GetCount();
				while (i < j)
				{
					ret += this->GetItem(i)->GetCoordinates(coordList);
					i++;
				}
				return ret;
			}

			virtual void SwapXY()
			{
				UOSInt i = this->GetCount();
				while (i-- > 0)
				{
					this->GetItem(i)->SwapXY();
				}
			}

			virtual Bool InsideVector(Math::Coord2DDbl coord) const
			{
				UOSInt i = 0;
				UOSInt j = this->GetCount();
				while (i < j)
				{
					if (this->GetItem(i)->InsideVector(coord))
						return true;
					i++;
				}
				return false;
			}
		};
	}
}
#endif
