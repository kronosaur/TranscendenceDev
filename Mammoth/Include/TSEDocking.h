//	TSEDocking.h
//
//	Defines classes and interfaces for docking
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CDockingPorts
	{
	public:
		CDockingPorts (void) { }
		CDockingPorts (const CDockingPorts &Src) { Copy(Src); }
		~CDockingPorts (void) { CleanUp(); }

		CDockingPorts &operator= (const CDockingPorts &Src) { CleanUp(); Copy(Src); return *this; }

		void DebugPaint (CG32bitImage &Dest, int x, int y, int iOwnerRotation, int iScale) const;
		void DeleteAll (CSpaceObject *pOwner);
		bool DockAtPort (CSpaceObject *pOwner, CSpaceObject *pObj, int iPort);
		void DockAtRandomPort (CSpaceObject *pOwner, CSpaceObject *pObj);
		bool DoesPortPaintInFront (CSpaceObject *pOwner, int iPort) const;
		int FindNearestEmptyPort (CSpaceObject *pOwner, CSpaceObject *pRequestingObj, CVector *retvDistance = NULL, int *retiEmptyPortCount = NULL);
		int GetPortCount (const CSpaceObject *pOwner = NULL) const { return m_iPortCount; }
		CSpaceObject *GetPortObj (const CSpaceObject *pOwner, int iPort) const { ASSERT(m_pPort[iPort].pObj == NULL || m_pPort[iPort].iStatus != psEmpty); return m_pPort[iPort].pObj; }
		CSpaceObject *GetObjAtPort (const CSpaceObject &Owner, int iPort) const { if (m_pPort[iPort].iStatus == psInUse) return m_pPort[iPort].pObj; else return NULL; }
		CVector GetPortPos (const CSpaceObject *pOwner, int iPort, CSpaceObject *pShip, bool *retbPaintInFront = NULL, int *retiRotation = NULL) const { return GetPortPos(pOwner, m_pPort[iPort], pShip, retbPaintInFront, retiRotation); }
		int GetPortsInUseCount (const CSpaceObject *pOwner) const;
		void InitPorts (CSpaceObject *pOwner, int iCount, Metric rRadius);
		void InitPorts (CSpaceObject *pOwner, const TArray<CVector> &Desc);
		void InitPortsFromXML (CSpaceObject *pOwner, CXMLElement *pElement, int iScale = -1);
		bool IsObjDocked (const CSpaceObject *pObj, int *retiPort = NULL) const { return IsDocked(pObj, retiPort); }
		bool IsObjDockedOrDocking (const CSpaceObject *pObj) const { return IsDockedOrDocking(pObj); }
		bool IsPortEmpty (const CSpaceObject *pOwner, int iPort) const { return (m_pPort[iPort].iStatus == psEmpty); }
		void MoveAll (CSpaceObject *pOwner);
		void OnDestroyed (void);
		void OnDockObjDestroyed (CSpaceObject *pOwner, const SDestroyCtx &Ctx);
		void OnNewSystem (CSystem *pNewSystem);
		void OnObjDestroyed (CSpaceObject *pOwner, CSpaceObject *pObj, bool *retbDestroyed = NULL);
		void ReadFromStream (CSpaceObject *pOwner, SLoadCtx &Ctx);
		bool RequestDock (CSpaceObject *pOwner, CSpaceObject *pObj, int iPort = -1);
		void SetMaxDockingDist (int iDist) { m_iMaxDist = iDist; }
		void Undock (CSpaceObject *pOwner, CSpaceObject *pObj, bool *retbWasDocked = NULL);
		void UpdateAll (SUpdateCtx &Ctx, CSpaceObject *pOwner);
		void UpdatePortsFromXML (CSpaceObject *pOwner, CXMLElement *pElement, int iScale = -1);
		void WriteToStream (CSpaceObject *pOwner, IWriteStream *pStream);

	private:
		static constexpr int DEFAULT_DOCK_DISTANCE_LS =			12;
		static constexpr Metric DEFAULT_DOCK_DISTANCE2 =		(LIGHT_SECOND * LIGHT_SECOND * DEFAULT_DOCK_DISTANCE_LS * DEFAULT_DOCK_DISTANCE_LS);

		enum DockingPortStatus
			{
			psEmpty =						0,
			psDocking =						1,
			psInUse =						2,
			};

		enum DockingPortLayer
			{
			plStandard =					0,	//	Depends on position

			plBringToFront =				1,	//	Ship is always in front of station
			plSendToBack =					2,	//	Ship is always behind station
			};

		struct SDockingPort
			{
			SDockingPort (void) :
					iStatus(psEmpty),
					iLayer(plStandard),
					pObj(NULL),
					iRotation(0)
				{ }

			DockingPortStatus iStatus;			//	Status of port
			DockingPortLayer iLayer;			//	Port layer relative to station
			CSpaceObject *pObj;					//	Object docked at this port
			C3DObjectPos Pos;					//	Position of dock relative to object
			CVector vPos;						//	Calculated position of dock (relative coords)
			int iRotation;						//	Rotation of ship at dock
			};

		int CalcMaxRadiusPixels (void) const;
		void CleanUp (void);
		void Copy (const CDockingPorts &Src);
		CVector GetPortPos (const CSpaceObject *pOwner, const SDockingPort &Port, CSpaceObject *pShip, bool *retbPaintInFront = NULL, int *retiRotation = NULL) const;
		CVector GetPortPosAtRotation (int iOwnerRotation, int iScale, int iPort, bool *retbPaintInFront = NULL, int *retiRotation = NULL) const;
		int GetScale (int iScale = -1) const { return (m_iScale > 0 ? m_iScale : iScale); }
		void InitXYPortPos (const CSpaceObject *pOwner, int iScale = -1) const;
		void InitXYPortPos (int iRotation, int iScale) const;
		bool IsDocked (const CSpaceObject *pObj, int *retiPort = NULL) const;
		bool IsDockedOrDocking (const CSpaceObject *pObj) const;
		bool ShipsNearPort (CSpaceObject *pOwner, CSpaceObject *pRequestingObj, const CVector &vPortPos);
		void UpdateDockingManeuvers (CSpaceObject *pOwner, SDockingPort &Port);

		int m_iPortCount = 0;					//	Number of docking ports
		SDockingPort *m_pPort = NULL;			//	Array of docking ports
		int m_iMaxDist = DEFAULT_DOCK_DISTANCE_LS;	//	Max distance for docking (in light-seconds)
		int m_iScale = -1;						//	If we override the scale value
		Metric m_rMaxPlayerDist = 0.0;			//	Max distance of player to check for nearest port.

		mutable int m_iLastRotation = -1;		//	Last owner rotation that we calculated port position for
	};

