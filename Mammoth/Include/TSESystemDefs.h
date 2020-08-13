//	TSESystemDefs.h
//
//	Defines classes and interfaces for star systems
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

constexpr int MIN_PLANET_SIZE = 1000;			//	Size at which a world is considered planetary size

class COrbit
	{
	public:
		COrbit (void) { }
		COrbit (const CVector &vCenter, Metric rRadius, Metric rPos = 0.0);

		bool operator== (const COrbit &Src) const;

		ICCItemPtr AsItem (void) const;
		const Metric &GetEccentricity (void) const { return m_rEccentricity; }
		const CVector3D &GetFocus3D (void) const { return m_vFocus; }
		CVector GetFocus (void) const { return CVector(m_vFocus.GetX(), m_vFocus.GetY()); }
		const Metric &GetInclination (void) const { return m_rInclination; }
		const Metric &GetObjectAngle (void) const { return m_rPos; }
		CVector GetObjectPos (Metric *retrZ = NULL) const { return GetPoint(m_rPos, retrZ); }
		CVector3D GetObjectPos3D (void) const;
		CVector GetPoint (Metric rAngle, Metric *retrZ = NULL) const;
		CVector GetPointAndRadius (Metric rAngle, Metric *retrRadius) const;
		CVector GetPointCircular (Metric rAngle) const;
		const Metric &GetRotation (void) const { return m_rRotation; }
		const Metric &GetSemiMajorAxis (void) const { return m_rSemiMajorAxis; }
		bool IsNull (void) const { return (m_rSemiMajorAxis == 0.0); }
		void Paint (CMapViewportCtx &Ctx, CG32bitImage &Dest, CG32bitPixel rgbColor);
		void PaintHD (CMapViewportCtx &Ctx, CG32bitImage &Dest, CG32bitPixel rgbColor, CGDraw::EBlendModes iMode = CGDraw::blendNormal) const;
		void ReadFromStream (SLoadCtx &Ctx);
		void SetEccentricity (Metric rValue) { m_rEccentricity = rValue; }
		void SetFocus (const CVector &vCenter) { m_vFocus = CVector3D(vCenter.GetX(), vCenter.GetY(), 0.0); }
		void SetFocus (const CVector3D &vCenter) { m_vFocus = vCenter; }
		void SetInclination (Metric rValue) { m_rInclination = rValue; }
		void SetObjectAngle (Metric rValue) { m_rPos = rValue; }
		void SetRotation (Metric rValue) { m_rRotation = rValue; }
		void SetSemiMajorAxis (Metric rValue) { m_rSemiMajorAxis = rValue; }
		void WriteToStream (IWriteStream &Stream) const;

		static bool FromItem (const ICCItem &Item, COrbit &retOrbit);
		static Metric RandomAngle (void) { return mathDegreesToRadians(mathRandom(0,3599) / 10.0); }
		static Metric ZToParallax (Metric rZ);

	private:
		static constexpr Metric CAMERA_DIST = LIGHT_SECOND * 60.0;

		struct SSerialized
			{
			CVector vFocus;
			Metric rEccentricity = 0.0;
			Metric rSemiMajorAxis = 0.0;
			Metric rRotation = 0.0;
			Metric rPos = 0.0;
			Metric rInclination = 0.0;
			Metric rFocusZ = 0.0;
			};

		CG32bitPixel GetColorAtRadiusHD (CG32bitPixel rgbColor, Metric rRadius) const;

		CVector3D m_vFocus;				//	Focus of orbit
		Metric m_rEccentricity = 0.0;	//	Ellipse eccentricity
		Metric m_rSemiMajorAxis = 0.0;	//	Semi-major axis
		Metric m_rRotation = 0.0;		//	Angle of rotation (radians)
		Metric m_rInclination = 0.0;	//	Angle of inclination (radians)

		Metric m_rPos = 0.0;			//	Obj position in orbit (radians)
	};

class CLocationDef
	{
	public:
		CLocationDef (void) { }

		bool CanBeBlocked (void) const;
		const CString &GetAttributes (void) const { return m_sAttributes; }
		DWORD GetObjID (void) const { return m_dwObjID; }
		const COrbit &GetOrbit (void) const { return m_OrbitDesc; }
		bool HasAttribute (const CString &sAttrib) const { return ::HasModifier(m_sAttributes, sAttrib); }
		bool IsBlocked (void) const { return m_bBlocked; }
		bool IsEmpty (void) const { return (m_dwObjID == 0 && !m_bBlocked); }
		bool IsRequired (void) const { return m_bRequired; }
		void SetAttributes (const CString &sAttribs) { m_sAttributes = sAttribs; }
		void SetBlocked (bool bBlocked = true) { m_bBlocked = bBlocked; }
		void SetID (const CString &sID) { m_sID = sID; }
		void SetObjID (DWORD dwObjID) { m_dwObjID = dwObjID; }
		void SetOrbit (const COrbit &Orbit) { m_OrbitDesc = Orbit; }
		void SetRequired (bool bValue = true) { m_bRequired = bValue; }
		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (IWriteStream *pStream);

	private:
		CString m_sID;						//	May be blank
		COrbit m_OrbitDesc;
		CString m_sAttributes;

		DWORD m_dwObjID = 0;				//	Object created at this location (or 0)

		bool m_bRequired = false;			//	If TRUE, location cannot be blocked
		bool m_bBlocked = false;			//	If TRUE, this location is too close to another
	};

class CTerritoryDef
	{
	public:
		CTerritoryDef (void);

		void AddAttributes (const CString &sAttribs);
		void AddRegion (const COrbit &Orbit, Metric rMinRadius, Metric rMaxRadius);
		static ALERROR CreateFromXML (CXMLElement *pDesc, const COrbit &OrbitDesc, CTerritoryDef **retpTerritory);
		void DebugPaint (CG32bitImage &Dest, CMapViewportCtx &PaintCtx, const CG16bitFont &LabelFont) const;
		const CString &GetAttributes (void) const { return m_sAttributes; }
		const CString &GetCriteria (void) { return m_sCriteria; }
		bool HasAttribute (const CString &sAttrib);
		bool HasAttribute (const CVector &vPos, const CString &sAttrib);
		bool IsMarked (void) { return m_bMarked; }
		bool PointInTerritory (const CVector &vPos);
		void ReadFromStream (SLoadCtx &Ctx);
		void SetCriteria (const CString &sCriteria) { m_sCriteria = sCriteria; }
		void SetMarked (bool bMarked = true) { m_bMarked = bMarked; }
		void WriteToStream (IWriteStream *pStream);

	private:
		struct SRegion
			{
			COrbit OrbitDesc;
			Metric rMinRadius = 0.0;
			Metric rMaxRadius = 0.0;

			Metric rMinRadius2 = 0.0;		//	Computed
			Metric rMaxRadius2 = 0.0;		//	Computed
			};

		CString m_sID;
		TArray<SRegion> m_Regions;
		CString m_sCriteria;
		CString m_sAttributes;

		bool m_bMarked;						//	Temporary mark
	};
