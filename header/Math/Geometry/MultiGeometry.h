#ifndef _SM_MATH_GEOMETRY_MULTIGEOMETRY
#define _SM_MATH_GEOMETRY_MULTIGEOMETRY
#include "Data/ArrayListNN.h"
#include "Math/Coord2DDbl.h"
#include "Math/CoordinateSystem.h"
#include "Math/Geometry/Vector2D.h"

#ifndef VERBOSE
//#define VERBOSE
#endif
#if defined(VERBOSE)
#include <stdio.h>
#endif

namespace Math
{
	namespace Geometry
	{
		template <class T> class MultiGeometry : public Math::Geometry::Vector2D
		{
		protected:
			Data::ArrayListNN<T> geometries;
		public:
			MultiGeometry(UInt32 srid) : Math::Geometry::Vector2D(srid)
			{
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

			virtual void AddGeometry(NotNullPtr<T> geometry)
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
				return this->GetBounds().GetCenter();
			}

			virtual Math::RectAreaDbl GetBounds() const
			{
				Math::RectAreaDbl bounds;
				UOSInt i = 1;
				UOSInt j = this->geometries.GetCount();
				if (j == 0)
				{
					bounds.tl = Math::Coord2DDbl(0, 0);
					bounds.br = Math::Coord2DDbl(0, 0);
				}
				else
				{
					bounds = this->geometries.GetItem(0)->GetBounds();
					while (i < j)
					{
						bounds = bounds.MergeArea(this->geometries.GetItem(i)->GetBounds());
						i++;
					}
				}
				return bounds;
			}

			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
			{
				UOSInt j = this->geometries.GetCount();
				if (j == 0)
				{
					nearPt.Set(Math::Coord2DDbl(0, 0));
					return 1000000000;
				}
				Math::Coord2DDbl minPt;
				Double minDist = this->geometries.GetItem(0)->CalBoundarySqrDistance(pt, minPt);
				Math::Coord2DDbl thisPt;
				Double thisDist;
				UOSInt i = 1;
				while (i < j)
				{
					thisDist = this->geometries.GetItem(i)->CalBoundarySqrDistance(pt, thisPt);
					if (minDist > thisDist)
					{
						minDist = thisDist;
						minPt = thisPt;
					}
					i++;
				}
				nearPt.Set(minPt);
				return minDist;
			}

			virtual Double CalSqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const
			{
				UOSInt j = this->geometries.GetCount();
				if (j == 0)
				{
					nearPt.Set(Math::Coord2DDbl(0, 0));
					return 1000000000;
				}
				Math::Coord2DDbl minPt;
				Double minDist = this->geometries.GetItem(0)->CalSqrDistance(pt, minPt);
				Math::Coord2DDbl thisPt;
				Double thisDist;
				UOSInt i = 1;
				while (i < j)
				{
					thisDist = this->geometries.GetItem(i)->CalSqrDistance(pt, thisPt);
					if (minDist > thisDist)
					{
						minDist = thisDist;
						minPt = thisPt;
					}
					i++;
				}
				nearPt.Set(minPt);
				return minDist;
			}

			virtual Bool JoinVector(NotNullPtr<const Math::Geometry::Vector2D> vec)
			{
				if (this->GetVectorType() != vec->GetVectorType())
				{
					return false;
				}
				Math::Geometry::MultiGeometry<T> *obj = (Math::Geometry::MultiGeometry<T> *)vec.Ptr();
				UOSInt i = 0;
				UOSInt j = obj->GetCount();
				while (i < j)
				{
					this->AddGeometry(NotNullPtr<T>::ConvertFrom(obj->GetItem(i)->Clone()));
					i++;
				}
				return true;
			}

			virtual Bool HasZ() const
			{
				if (this->geometries.GetCount() > 0)
					return this->geometries.GetItem(0)->HasZ();
				return false;
			}

			virtual Bool HasM() const
			{
				if (this->geometries.GetCount() > 0)
					return this->geometries.GetItem(0)->HasM();
				return false;
			}

			virtual void ConvCSys(NotNullPtr<const Math::CoordinateSystem> srcCSys, NotNullPtr<const Math::CoordinateSystem> destCSys)
			{
				UOSInt i = this->GetCount();
				while (i-- > 0)
				{
					this->GetItem(i)->ConvCSys(srcCSys, destCSys);
				}
				this->srid = destCSys->GetSRID();
			}

			virtual Bool Equals(NotNullPtr<const Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const
			{
				if (this->GetVectorType() != vec->GetVectorType())
				{
					if (!sameTypeOnly && this->geometries.GetCount() == 1)
					{
						return this->geometries.GetItem(0)->Equals(vec, sameTypeOnly, nearlyVal);
					}
#if defined(VERBOSE)
					printf("MultiGeometry: Vector type different\r\n");
#endif
					return false;
				}
				Math::Geometry::MultiGeometry<T> *obj = (Math::Geometry::MultiGeometry<T> *)vec.Ptr();
				if (obj->GetCount() != this->GetCount())
				{
#if defined(VERBOSE)
					printf("MultiGeometry: Vector count different: %d != %d\r\n", (UInt32)obj->GetCount(), (UInt32)this->GetCount());
#endif
					return false;
				}
				NotNullPtr<Math::Geometry::Vector2D> v;
				UOSInt i = this->GetCount();
				while (i-- > 0)
				{
					if (!v.Set(obj->GetItem(i)) || !this->GetItem(i)->Equals(v, sameTypeOnly, nearlyVal))
						return false;
				}
				return true;
			}

			virtual UOSInt GetCoordinates(NotNullPtr<Data::ArrayListA<Math::Coord2DDbl>> coordList) const
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

			virtual Bool InsideOrTouch(Math::Coord2DDbl coord) const
			{
				UOSInt i = 0;
				UOSInt j = this->GetCount();
				while (i < j)
				{
					if (this->GetItem(i)->InsideOrTouch(coord))
						return true;
					i++;
				}
				return false;
			}

			virtual void SwapXY()
			{
				UOSInt i = this->GetCount();
				while (i-- > 0)
				{
					this->GetItem(i)->SwapXY();
				}
			}

			virtual void MultiplyCoordinatesXY(Double v)
			{
				UOSInt i = this->GetCount();
				while (i-- > 0)
				{
					this->GetItem(i)->MultiplyCoordinatesXY(v);
				}
			}

			virtual void SetSRID(UInt32 srid)
			{
				this->srid = srid;
				UOSInt i = this->GetCount();
				while (i-- > 0)
				{
					this->GetItem(i)->SetSRID(srid);
				}
			}

			virtual UOSInt GetPointCount() const
			{
				UOSInt ret = 0;
				UOSInt j = this->GetCount();
				while (j-- > 0)
				{
					ret += this->GetItem(j)->GetPointCount();
				}
				return ret;
			}
		};
	}
}
#endif
