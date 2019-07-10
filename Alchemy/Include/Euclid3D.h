//	Euclid3D.h
//
//	3D drawing and animation
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	3D Vector Class ------------------------------------------------------------

class CVector3D
	{
	public:
		CVector3D (void) : x(0.0), y(0.0), z(0.0) { }
		CVector3D (Metric ix, Metric iy, Metric iz) : x(ix), y(iy), z(iz) { }

		bool operator == (const CVector3D &vA) const { return (x == vA.x && y == vA.y && z == vA.z); }

		const Metric &GetX (void) const { return x; }
		const Metric &GetY (void) const { return y; }
		const Metric &GetZ (void) const { return z; }
		bool IsNull (void) const { return (x == 0.0 && y == 0.0 && z == 0.0); }
		Metric Length (void) const { return sqrt(x * x + y * y + z * z); }
		Metric Length2 (void) const { return (x * x + y * y + z * z); }
		Metric Longest (void) const
			{
			Metric ax = (x < 0.0 ? -x : x);
			Metric ay = (y < 0.0 ? -y : y);
			Metric az = (z < 0.0 ? -z : z);
			return (ax > ay ? (ax > az ? ax : az) : (ay > az ? ay : az));
			}
		CVector Perpendicular (void) const { return CVector(-y, x); }
		void SetX (Metric NewX) { x = NewX; }
		void SetY (Metric NewY) { y = NewY; }
		void SetZ (Metric NewZ) { z = NewZ; }
		CVector3D Unit (void) const 
			{
			Metric rLength = Length();
			if (rLength == 0.0)
				return CVector3D();
			else
				return CVector3D(x / rLength, y / rLength, z / rLength);
			}
		CVector3D Unit (Metric *retrLength) const
			{
			*retrLength = Length();
			if (*retrLength == 0.0)
				return CVector3D();
			else
				return CVector3D(x / *retrLength, y / *retrLength, z / *retrLength);
			}

	private:
		Metric x;
		Metric y;
		Metric z;
	};

//	Vector-Vector operations
inline CVector3D operator+ (const CVector3D &op1, const CVector3D &op2) { return CVector3D(op1.GetX() + op2.GetX(), op1.GetY() + op2.GetY(), op1.GetZ() + op2.GetZ()); }
inline CVector3D operator- (const CVector3D &op1, const CVector3D &op2) { return CVector3D(op1.GetX() - op2.GetX(), op1.GetY() - op2.GetY(), op1.GetZ() - op2.GetZ()); }
inline CVector3D operator* (const CVector3D &op1, const CVector3D &op2) { return CVector3D(op1.GetX() * op2.GetX(), op1.GetY() * op2.GetY(), op1.GetZ() * op2.GetZ()); }
inline CVector3D operator- (const CVector3D &op) { return CVector3D(-op.GetX(), -op.GetY(), -op.GetZ()); }

//	Vector-scalar operations
inline CVector3D operator* (const CVector3D &op1, const Metric op2) { return CVector3D(op1.GetX() * op2, op1.GetY() * op2, op1.GetZ() * op2); }
inline CVector3D operator* (const Metric op2, const CVector3D &op1) { return CVector3D(op1.GetX() * op2, op1.GetY() * op2, op1.GetZ() * op2); }
inline CVector3D operator/ (const CVector3D &op1, const Metric op2) { return CVector3D(op1.GetX() / op2, op1.GetY() / op2, op1.GetZ() / op2); }

//	Transforms -----------------------------------------------------------------

class CXForm3D
	{
	public:
		CXForm3D (XFormType type = xformIdentity);
		CXForm3D (XFormType type, Metric rX, Metric rY, Metric rZ);
		CXForm3D (XFormType type, const CVector3D &vVector);
		CXForm3D (XFormType type, Metric rRadians);

		void SetAt (int iRow, int iCol, Metric rValue) { m_Xform[iRow][iCol] = rValue; }
		CVector3D Transform (const CVector3D &vVector) const;
		CVector3D Transform (const CVector3D &vVector, Metric *retrW) const;

	private:
		Metric m_Xform[4][4];

	friend const CXForm3D operator* (const CXForm3D &op1, const CXForm3D &op2);
	};

const CXForm3D operator* (const CXForm3D &op1, const CXForm3D &op2);

//  3D Projection --------------------------------------------------------------

class CProjection3D
	{
	public:
		CProjection3D (Metric rFoV, Metric rAspectRatio = 1.0, Metric rNearPlane = 1.0, Metric rFarPlane = 10.0) :
				m_rFoV(rFoV),
				m_rAspectRatio(rAspectRatio),
				m_rNearPlane(rNearPlane),
				m_rFarPlane(rFarPlane)
			{
			m_Xform.SetAt(0, 0, cotan(0.5 * rFoV) / rAspectRatio);
			m_Xform.SetAt(1, 1, cotan(0.5 * rFoV));
			m_Xform.SetAt(2, 2, rFarPlane / (rNearPlane - rFarPlane));
			m_Xform.SetAt(2, 3, -rFarPlane * rNearPlane / (rNearPlane - rFarPlane));
			m_Xform.SetAt(3, 2, 1.0);
			m_Xform.SetAt(3, 3, 0.0);
			}

		operator const CXForm3D & () const { return m_Xform; }

		const CXForm3D &GetXform (void) const { return m_Xform; }
		CVector Transform (const CXForm3D &Xform, const CVector3D &vPos) const { Metric rW; CVector3D vA = Xform.Transform(vPos, &rW); return CVector(vA.GetX() / rW, vA.GetY() / rW); }
		CVector Transform (const CVector3D &vPos) const { return Transform(m_Xform, vPos); }
		TArray<CVector> Transform (const CXForm3D &Xform, const TArray<CVector3D> &Points) const;

	private:
		static Metric cotan (Metric rValue) { return cos(rValue) / sin(rValue); }

		Metric m_rFoV;
		Metric m_rAspectRatio;
		Metric m_rNearPlane;
		Metric m_rFarPlane;

		CXForm3D m_Xform;
	};

//	Basic Shapes ---------------------------------------------------------------

class CLine3D
	{
	public:
		CLine3D (void) { }
		CLine3D (const CVector3D &vStart, const CVector3D &vEnd) :
				m_vStart(vStart),
				m_vEnd(vEnd)
			{ }

		const CVector3D &GetStart (void) const { return m_vStart; }
		const CVector3D &GetEnd (void) const { return m_vEnd; }

	private:
		CVector3D m_vStart;
		CVector3D m_vEnd;
	};

//	Spherical Coordinates ------------------------------------------------------

class CSphericalCoordinates
	{
	public:
		static CVector3D FromCartessian (const CVector3D &vPos);
		static CVector3D ToCartessian (const CVector3D &vPos);
	};

//	3D Algorithms --------------------------------------------------------------

class CGeometry3D
	{
	public:
		struct SIndexedLine
			{
			int iStart;
			int iEnd;
			};

		static void CalcConvexHull (const TArray<CVector3D> &Points, TArray<SIndexedLine> &Edges);
	};
