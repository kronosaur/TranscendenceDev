//	TSEDocking.h
//
//	Defines classes and interfaces for docking
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CDockingPorts
	{
	public:
		CDockingPorts (void);
		inline CDockingPorts (const CDockingPorts &Src) { Copy(Src); }
		inline ~CDockingPorts (void) { CleanUp(); }

		CDockingPorts &operator= (const CDockingPorts &Src) { CleanUp(); Copy(Src); return *this; }

		void DebugPaint (CG32bitImage &Dest, int x, int y, int iOwnerRotation, int iScale) const;
		void DeleteAll (CSpaceObject *pOwner);
		void DockAtRandomPort (CSpaceObject *pOwner, CSpaceObject *pObj);
		bool DoesPortPaintInFront (CSpaceObject *pOwner, int iPort) const;
		int FindNearestEmptyPort (CSpaceObject *pOwner, CSpaceObject *pRequestingObj, CVector *retvDistance = NULL, int *retiEmptyPortCount = NULL);
		inline int GetPortCount (CSpaceObject *pOwner = NULL) const { return m_iPortCount; }
		inline CSpaceObject *GetPortObj (CSpaceObject *pOwner, int iPort) { ASSERT(m_pPort[iPort].pObj == NULL || m_pPort[iPort].iStatus != psEmpty); return m_pPort[iPort].pObj; }
		inline CVector GetPortPos (CSpaceObject *pOwner, int iPort, CSpaceObject *pShip, bool *retbPaintInFront = NULL, int *retiRotation = NULL) { return GetPortPos(pOwner, m_pPort[iPort], pShip, retbPaintInFront, retiRotation); }
		CVector GetPortPosAtRotation (int iOwnerRotation, int iScale, int iPort, bool *retbPaintInFront = NULL, int *retiRotation = NULL) const;
		int GetPortsInUseCount (CSpaceObject *pOwner);
		void InitPorts (CSpaceObject *pOwner, int iCount, Metric rRadius);
		void InitPorts (CSpaceObject *pOwner, const TArray<CVector> &Desc);
		void InitPortsFromXML (CSpaceObject *pOwner, CXMLElement *pElement, int iScale = -1);
		inline bool IsObjDocked (CSpaceObject *pObj) { return IsDocked(pObj); }
		inline bool IsObjDockedOrDocking (CSpaceObject *pObj) { return IsDockedOrDocking(pObj); }
		inline bool IsPortEmpty (CSpaceObject *pOwner, int iPort) const { return (m_pPort[iPort].iStatus == psEmpty); }
		void MoveAll (CSpaceObject *pOwner);
		void OnDestroyed (void);
		void OnNewSystem (CSystem *pNewSystem);
		void OnObjDestroyed (CSpaceObject *pOwner, CSpaceObject *pObj, bool *retbDestroyed = NULL);
		void ReadFromStream (CSpaceObject *pOwner, SLoadCtx &Ctx);
		void RepairAll (CSpaceObject *pOwner, int iRepairRate);
		bool RequestDock (CSpaceObject *pOwner, CSpaceObject *pObj, int iPort = -1);
		inline void SetMaxDockingDist (int iDist) { m_iMaxDist = iDist; }
		void Undock (CSpaceObject *pOwner, CSpaceObject *pObj, bool *retbWasDocked = NULL);
		void UpdateAll (SUpdateCtx &Ctx, CSpaceObject *pOwner);
		void WriteToStream (CSpaceObject *pOwner, IWriteStream *pStream);

	private:
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

		void CleanUp (void);
		void Copy (const CDockingPorts &Src);
		CVector GetPortPos (CSpaceObject *pOwner, const SDockingPort &Port, CSpaceObject *pShip, bool *retbPaintInFront = NULL, int *retiRotation = NULL) const;
		void InitXYPortPos (CSpaceObject *pOwner, int iScale = -1) const;
		void InitXYPortPos (int iRotation, int iScale) const;
		bool IsDocked (CSpaceObject *pObj);
		bool IsDockedOrDocking (CSpaceObject *pObj);
		bool ShipsNearPort (CSpaceObject *pOwner, CSpaceObject *pRequestingObj, const CVector &vPortPos);
		void UpdateDockingManeuvers (CSpaceObject *pOwner, SDockingPort &Port);

		int m_iPortCount;						//	Number of docking ports
		SDockingPort *m_pPort;					//	Array of docking ports
		int m_iMaxDist;							//	Max distance for docking (in light-seconds)

		mutable int m_iLastRotation;			//	Last owner rotation that we calculated port position for
	};

