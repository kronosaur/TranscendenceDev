//	CDockingPorts.cpp
//
//	CDockingPorts class

#include "PreComp.h"

#define FINAL_DOCKING							(8.0 * g_KlicksPerPixel)
#define FINAL_DOCKING2							(FINAL_DOCKING * FINAL_DOCKING)

#define FINAL_APPROACH							(32.0 * g_KlicksPerPixel)
#define FINAL_APPROACH2							(FINAL_APPROACH * FINAL_APPROACH)

#define DOCKING_THRESHOLD						(4.0 * g_KlicksPerPixel)
#define DOCKING_THRESHOLD2						(DOCKING_THRESHOLD * DOCKING_THRESHOLD)

#define DOCKING_PORTS_TAG						CONSTLIT("DockingPorts")

#define BRING_TO_FRONT_ATTRIB					CONSTLIT("bringToFront")
#define DOCKING_PORTS_ATTRIB					CONSTLIT("dockingPorts")
#define MAX_DIST_ATTRIB							CONSTLIT("maxDist")
#define PORT_ANGLE_ATTRIB						CONSTLIT("portAngle")
#define PORT_COUNT_ATTRIB						CONSTLIT("portCount")
#define PORT_RADIUS_ATTRIB						CONSTLIT("portRadius")
#define POS_Z_ATTRIB							CONSTLIT("posZ")
#define ROTATION_ATTRIB							CONSTLIT("rotation")
#define SEND_TO_BACK_ATTRIB						CONSTLIT("sendToBack")
#define X_ATTRIB								CONSTLIT("x")
#define Y_ATTRIB								CONSTLIT("y")

#define MIN_PORT_DISTANCE						(8.0 * g_KlicksPerPixel)
#define MIN_PORT_DISTANCE2						(MIN_PORT_DISTANCE * MIN_PORT_DISTANCE)

const int DEFAULT_PORT_POS_RADIUS =				64;
const int DEFAULT_DOCK_DISTANCE_LS =			12;
const Metric DEFAULT_DOCK_DISTANCE2 =			(LIGHT_SECOND * LIGHT_SECOND * DEFAULT_DOCK_DISTANCE_LS * DEFAULT_DOCK_DISTANCE_LS);
const Metric GATE_DIST =						KLICKS_PER_PIXEL * 64.0;
const Metric GATE_DIST2 =						GATE_DIST * GATE_DIST;

CDockingPorts::CDockingPorts (void) : 
		m_iPortCount(0),
		m_pPort(NULL),
		m_iMaxDist(DEFAULT_DOCK_DISTANCE_LS),
		m_iLastRotation(-1)

//	CDockingPorts constructor

	{
	}

void CDockingPorts::CleanUp (void)

//	CleanUp
//
//	Free resources

	{
	if (m_pPort)
		delete [] m_pPort;

	m_iPortCount = 0;
	m_pPort = NULL;
	m_iMaxDist = DEFAULT_DOCK_DISTANCE_LS;
	m_iLastRotation = -1;
	}

void CDockingPorts::Copy (const CDockingPorts &Src)

//	Copy
//
//	Copy. We assume we are clean (no resources allocated).

	{
	int i;

	m_iPortCount = Src.m_iPortCount;
	if (Src.m_pPort)
		{
		m_pPort = new SDockingPort [m_iPortCount];
		for (i = 0; i < m_iPortCount; i++)
			m_pPort[i] = Src.m_pPort[i];
		}
	else
		m_pPort = NULL;

	m_iMaxDist = Src.m_iMaxDist;
	m_iLastRotation = Src.m_iLastRotation;
	}

void CDockingPorts::DebugPaint (CG32bitImage &Dest, int x, int y, int iOwnerRotation, int iScale) const

//	DebugPaint
//
//	Paint the docking ports

	{
	int i;

	for (i = 0; i < GetPortCount(); i++)
		{
		int iRotation;
		bool bInFront;
		CVector vPos = GetPortPosAtRotation(iOwnerRotation, iScale, i, &bInFront, &iRotation);

		//	Colors

		CG32bitPixel rgbArrowColor = (bInFront ? CG32bitPixel(0x00, 0x40, 0x80) : CG32bitPixel(0x80, 0x40, 0x00));
		CG32bitPixel rgbCenterColor = (bInFront ? CG32bitPixel(0x00, 0x7f, 0xff) : CG32bitPixel(0xff, 0x7f, 0x00));

		//	Get the position

		int xPos = x + (int)(vPos.GetX() / g_KlicksPerPixel);
		int yPos = y - (int)(vPos.GetY() / g_KlicksPerPixel);

		//	Paint arrow

		CPaintHelper::PaintArrow(Dest, xPos, yPos, iRotation, rgbArrowColor);

		//	Paint center crosshairs

		Dest.DrawDot(xPos, yPos, rgbCenterColor, markerMediumCross);
		}
	}

void CDockingPorts::DeleteAll (CSpaceObject *pOwner)

//	DeleteAll
//
//	Delete all ports

	{
	for (int i = 0; i < m_iPortCount; i++)
		if (m_pPort[i].iStatus == psDocking)
			{
			m_pPort[i].pObj->OnDockingStop();
			}
		else if (m_pPort[i].iStatus == psInUse)
			{
			m_pPort[i].pObj->OnDockingPortDestroyed();
			}

	CleanUp();
	}

bool CDockingPorts::DockAtPort (CSpaceObject *pOwner, CSpaceObject *pObj, int iPort)

//	DockAtPort
//
//	Docks at the given port and returns TRUE if successful.

	{
	//	Make sure this is a valid port to dock at.

	if (iPort < 0 || iPort >= m_iPortCount || m_pPort[iPort].iStatus != psEmpty)
		return false;

	//	Dock

	m_pPort[iPort].pObj = pObj;
	m_pPort[iPort].iStatus = psInUse;

	int iRotation;
	pObj->Place(GetPortPos(pOwner, m_pPort[iPort], pObj, NULL, &iRotation));
	pObj->OnDocked(pOwner);

	//	Set the ship's rotation. We do this because this is only called
	//	during set up

	CShip *pShip = pObj->AsShip();
	if (pShip)
		pShip->SetRotation(iRotation);

	//	Success!

	return true;
	}

void CDockingPorts::DockAtRandomPort (CSpaceObject *pOwner, CSpaceObject *pObj)

//	DockAtRandomPort
//
//	Dock the ship at a random port. This is used during setup

	{
	int iStart = mathRandom(0, m_iPortCount - 1);
	int iDockingPort = -1;

	for (int i = 0; i < m_iPortCount; i++)
		{
		int iPort = (iStart + i) % m_iPortCount;
		if (m_pPort[iPort].iStatus == psEmpty)
			{
			iDockingPort = iPort;
			break;
			}
		}

	//	Set ship at the docking port

	if (iDockingPort != -1)
		{
		DockAtPort(pOwner, pObj, iDockingPort);
		}
	else
		{
		kernelDebugLogPattern("Warning: Unable to find docking port at %s", pOwner->GetNounPhrase());
		}
	}

int CDockingPorts::FindNearestEmptyPort (CSpaceObject *pOwner, CSpaceObject *pRequestingObj, CVector *retvDistance, int *retiEmptyPortCount)

//	FindNearestEmptyPort
//
//	FindNearestEmptyPort 

	{
	int i;

	int iEmptyPortCount = 0;

	int iBestPort = -1;
	Metric rBestDist2 = g_InfiniteDistance;
	CVector vBestDistance;

	int iBestClearPort = -1;
	Metric rBestClearDist2 = g_InfiniteDistance;
	CVector vBestClearDistance;

	CVector vPos = pRequestingObj->GetPos();

	//	See if we can find a clear port

	for (i = 0; i < m_iPortCount; i++)
		if (m_pPort[i].iStatus == psEmpty)
			{
			CVector vPortPos = GetPortPos(pOwner, m_pPort[i], pRequestingObj);
			CVector vDistance = vPos - vPortPos;
			Metric rDistance2 = vDistance.Length2();

			//	Keep track of both the nearest clear port and the nearest
			//	port that is not clear.

			if (rDistance2 < rBestClearDist2 
					&& !ShipsNearPort(pOwner, pRequestingObj, vPortPos))
				{
				rBestClearDist2 = rDistance2;
				vBestClearDistance = vDistance;
				iBestClearPort = i;
				}
			else if (rDistance2 < rBestDist2)
				{
				rBestDist2 = rDistance2;
				vBestDistance = vDistance;
				iBestPort = i;
				}

			iEmptyPortCount++;
			}

	//	Return total number of empty ports

	if (retiEmptyPortCount)
		*retiEmptyPortCount = iEmptyPortCount;

	//	If we've got a clear port, then return that

	if (iBestClearPort != -1)
		{
		if (retvDistance)
			*retvDistance = vBestClearDistance;

		return iBestClearPort;
		}

	//	Otherwise, return the nearest non-clear port

	else
		{
		if (retvDistance)
			*retvDistance = vBestDistance;

		return iBestPort;
		}
	}

bool CDockingPorts::DoesPortPaintInFront (CSpaceObject *pOwner, int iPort) const

//	DoesPortPaintInFront
//
//	Retuns TRUE if we paint this port in front of the owning object.

	{
	const SDockingPort &Port = m_pPort[iPort];

	if (pOwner == NULL)
		return (Port.iLayer != plSendToBack);

	else if (pOwner->GetRotation() == 0)
		{
		switch (Port.iLayer)
			{
			case plBringToFront:
				return true;

			case plSendToBack:
				return false;

			default:
				return (Port.vPos.GetY() < 0.0);
			}
		}
	else
		{
		CVector vPortPos = Port.vPos.Rotate(pOwner->GetRotation());
		return (vPortPos.GetY() < 0.0);
		}
	}

CVector CDockingPorts::GetPortPos (CSpaceObject *pOwner, const SDockingPort &Port, CSpaceObject *pShip, bool *retbPaintInFront, int *retiRotation) const

//	GetPortPos
//
//	Get the absolute position of the port (or relative, if pOwner == NULL)

	{
	if (pOwner == NULL)
		{
		if (retbPaintInFront)
			*retbPaintInFront = (Port.iLayer != plSendToBack);

		if (retiRotation)
			*retiRotation = Port.iRotation;

		return Port.vPos;
		}
	else
		{
		//	Recalc the port positions if we've rotated.

		if (m_iLastRotation != pOwner->GetRotation())
			InitXYPortPos(pOwner);

		//	Paint layer

		if (retbPaintInFront)
			{
			switch (Port.iLayer)
				{
				case plBringToFront:
					*retbPaintInFront = true;
					break;

				case plSendToBack:
					*retbPaintInFront = false;
					break;

				default:
					*retbPaintInFront = (Port.vPos.GetY() < 0.0);
				}
			}

		//	Ship rotation

		int iShipRotation = AngleMod(Port.iRotation + m_iLastRotation);
		if (retiRotation)
			*retiRotation = iShipRotation;

		//	Return the absolute position

		return (pOwner->GetPos() + Port.vPos + (pShip ? pShip->GetDockingPortOffset(iShipRotation) : NullVector));
		}
	}

CVector CDockingPorts::GetPortPosAtRotation (int iOwnerRotation, int iScale, int iPort, bool *retbPaintInFront, int *retiRotation) const

//	GetPortPosAtRotation
//
//	Returns port position.

	{
	if (iPort < 0 || iPort >= m_iPortCount)
		return CVector();

	const SDockingPort &Port = m_pPort[iPort];

	//	Recalc the port positions if we've rotated.

	if (m_iLastRotation != iOwnerRotation)
		InitXYPortPos(iOwnerRotation, iScale);

	//	Paint layer

	if (retbPaintInFront)
		{
		switch (Port.iLayer)
			{
			case plBringToFront:
				*retbPaintInFront = true;
				break;

			case plSendToBack:
				*retbPaintInFront = false;
				break;

			default:
				*retbPaintInFront = (Port.vPos.GetY() < 0.0);
			}
		}

	//	Ship rotation

	if (retiRotation)
		*retiRotation = AngleMod(Port.iRotation + m_iLastRotation);

	//	Return the absolute position

	return Port.vPos;
	}

int CDockingPorts::GetPortsInUseCount (CSpaceObject *pOwner)

//	GetPortsInUseCount
//
//	GetPortsInUseCount 

	{
	int iCount = 0;

	for (int i = 0; i < m_iPortCount; i++)
		if (m_pPort[i].iStatus != psEmpty)
			iCount++;

	return iCount;
	}

void CDockingPorts::InitPorts (CSpaceObject *pOwner, int iCount, Metric rRadius)

//	InitPorts
//
//	InitPorts 

	{
	ASSERT(m_pPort == NULL);

	if (iCount > 0)
		{
		m_iPortCount = iCount;
		m_pPort = new SDockingPort[iCount];

		int iAngle = 360 / iCount;
		for (int i = 0; i < iCount; i++)
			{
			int iPortAngle = i * iAngle;
			m_pPort[i].Pos = C3DObjectPos(iPortAngle, mathRound(rRadius / g_KlicksPerPixel));
			m_pPort[i].vPos = PolarToVector(iPortAngle, rRadius);
			m_pPort[i].iRotation = AngleMod(iPortAngle + 180);
			}
		}

	m_iMaxDist = DEFAULT_DOCK_DISTANCE_LS;
	m_iLastRotation = (pOwner ? pOwner->GetRotation() : 0);
	}

void CDockingPorts::InitPorts (CSpaceObject *pOwner, const TArray<CVector> &Desc)

//	InitPorts
//
//	Initialize from array

	{
	ASSERT(m_pPort == NULL);

	if (Desc.GetCount() > 0)
		{
		m_iPortCount = Desc.GetCount();
		m_pPort = new SDockingPort[m_iPortCount];

		for (int i = 0; i < m_iPortCount; i++)
			{
			m_pPort[i].vPos = Desc[i];
			m_pPort[i].iRotation = (VectorToPolar(Desc[i]) + 180) % 360;
			}
		}

	m_iMaxDist = DEFAULT_DOCK_DISTANCE_LS;
	}

void CDockingPorts::InitPortsFromXML (CSpaceObject *pOwner, CXMLElement *pElement, int iScale)

//	InitPortsFromXML
//
//	Initialize ports from an XML element. NOTE: We guarantee that both the polar 
//	and Cartessian port positions are initialized, if required.
//
//	m_iLastRotation is initialized to the rotation of the owner.

	{
	int i;

	//	See if we've got a special element with docking port geometry

	CXMLElement *pDockingPorts = pElement->GetContentElementByTag(DOCKING_PORTS_TAG);
	if (pDockingPorts)
		{
		//	Initialize max dist
		//
		//	NOTE: pOwner can be NULL because sometimes we init ports from a ship class 
		//	(without an object).
		//
		//	NOTE: This is the max distance from a docking port, not from the center of
		//	the station, so we don't have to change the defaults for different sized
		//	stations.

		m_iMaxDist = pDockingPorts->GetAttributeIntegerBounded(MAX_DIST_ATTRIB, 1, -1, DEFAULT_DOCK_DISTANCE_LS);

		//	Sometimes we specify x,y coordinate but want to convert to rotating 
		//	polar coordinate.

		int iConvertRotation = -1;
		CString sAttrib;
		if (pDockingPorts->FindAttribute(ROTATION_ATTRIB, &sAttrib))
			{
			if (CXMLElement::IsBoolTrueValue(sAttrib))
				iConvertRotation = 0;
			else
				iConvertRotation = strToInt(sAttrib, 0);
			}

		//	If we have sub-elements then these are port definitions.

		m_iPortCount = pDockingPorts->GetContentElementCount();
		if (m_iPortCount > 0)
			{
			m_pPort = new SDockingPort[m_iPortCount];

			for (i = 0; i < m_iPortCount; i++)
				{
				CXMLElement *pPort = pDockingPorts->GetContentElement(i);

				//	See if the port position is specified with polar coordinate.
				//	If NOT, then we load Cartessian coordinates.

				if (!m_pPort[i].Pos.InitFromXML(pPort, C3DObjectPos::FLAG_NO_XY))
					{
					CVector vDockPos((pPort->GetAttributeInteger(X_ATTRIB) * g_KlicksPerPixel),
							(pPort->GetAttributeInteger(Y_ATTRIB) * g_KlicksPerPixel));

					m_pPort[i].vPos = vDockPos;

					//	If we expect this to be rotatable, then we reverse engineer
					//	the polar coordinates.

					if (iConvertRotation != -1)
						{
						if (iScale <= 0)
							iScale = (pOwner ? pOwner->GetImageScale() : 512);

						m_pPort[i].Pos.InitFromXY(iScale, vDockPos, pPort->GetAttributeInteger(POS_Z_ATTRIB));

						//	If the conversion rotation is non-zero then it means that the port
						//	x,y coordinates are for the given rotation, and thus we need to 
						//	compensate.

						if (iConvertRotation != 0)
							m_pPort[i].Pos.SetAngle(m_pPort[i].Pos.GetAngle() - iConvertRotation);
						}
					}

				//	Get the ship's rotation when docked at this port.

				if (pPort->FindAttributeInteger(ROTATION_ATTRIB, &m_pPort[i].iRotation))
					m_pPort[i].iRotation = AngleMod(m_pPort[i].iRotation - (iConvertRotation != -1 ? iConvertRotation : 0));
				else if (!m_pPort[i].Pos.IsEmpty())
					m_pPort[i].iRotation = AngleMod(m_pPort[i].Pos.GetAngle() + 180);
				else
					m_pPort[i].iRotation = AngleMod(VectorToPolar(m_pPort[i].vPos) + 180);

				//	Layer options

				if (pPort->GetAttributeBool(BRING_TO_FRONT_ATTRIB))
					m_pPort[i].iLayer = plBringToFront;
				else if (pPort->GetAttributeBool(SEND_TO_BACK_ATTRIB))
					m_pPort[i].iLayer = plSendToBack;
				}
			}

		//	Otherwise, we expect a port count and radius

		else if ((m_iPortCount = pDockingPorts->GetAttributeIntegerBounded(PORT_COUNT_ATTRIB, 0, -1, 0)) > 0)
			{
			int iRadius = pDockingPorts->GetAttributeIntegerBounded(PORT_RADIUS_ATTRIB, 0, -1, DEFAULT_PORT_POS_RADIUS);
			int iStartAngle = AngleMod(pDockingPorts->GetAttributeInteger(PORT_ANGLE_ATTRIB));
			int iAngle = 360 / m_iPortCount;

			//	Initialize ports

			m_pPort = new SDockingPort[m_iPortCount];
			for (i = 0; i < m_iPortCount; i++)
				{
				int iPortAngle = AngleMod(iStartAngle + i * iAngle);
				m_pPort[i].Pos = C3DObjectPos(iPortAngle, iRadius);
				m_pPort[i].iRotation = AngleMod(iPortAngle + 180);
				}
			}

		//	Otherwise, no ports

		else
			{
			m_iPortCount = 0;
			m_pPort = NULL;
			}

		//	Convert from polar to Cartessian

		InitXYPortPos(pOwner, iScale);
		}

	//	Otherwise, initialize ports based on a count

	else
		InitPorts(pOwner,
				pElement->GetAttributeInteger(DOCKING_PORTS_ATTRIB),
				64 * g_KlicksPerPixel);
	}

void CDockingPorts::InitXYPortPos (CSpaceObject *pOwner, int iScale) const

//	InitXYPortPos
//
//	Initializes the x,y coordinates of the port based on the polar coordinates.

	{
	if (m_iPortCount == 0)
		return;

	//	If we've already computed x,y for this object rotation, then we're done.

	int iRotation = (pOwner ? pOwner->GetRotation() : 0);
	if (m_iLastRotation == iRotation)
		return;

	//	We need the image scale to adjust coordinates
	//
	//	NOTE: Sometimes we don't have an image yet. For example, when
	//	creating a wreck, we're still inside CStation::CreateFromType 
	//	and have not yet assigned an image.

	if (iScale <= 0)
		iScale = (pOwner ? pOwner->GetImageScale() : 512);

	//	Compute coordinates for each port

	InitXYPortPos(iRotation, iScale);
	}

void CDockingPorts::InitXYPortPos (int iRotation, int iScale) const

//	InitXYPortPos
//
//	Initialize to the given rotation and scale.

	{
	int i;

	ASSERT(iScale > 0);

	//	Compute coordinates for each port

	for (i = 0; i < m_iPortCount; i++)
		if (!m_pPort[i].Pos.IsEmpty())
			m_pPort[i].Pos.CalcCoord(iScale, iRotation, &m_pPort[i].vPos);

	//	Done

	m_iLastRotation = iRotation;
	}

bool CDockingPorts::IsDocked (CSpaceObject *pObj, int *retiPort) const

//	IsDocked
//
//	Returns TRUE if the given object is docked here

	{
	for (int i = 0; i < m_iPortCount; i++)
		if (m_pPort[i].iStatus == psInUse)
			if (pObj == m_pPort[i].pObj)
				{
				if (retiPort)
					*retiPort = i;
				return true;
				}

	return false;
	}

bool CDockingPorts::IsDockedOrDocking (CSpaceObject *pObj)

//	IsDockedOrDocking
//
//	Returns TRUE if the given object is docked (or docking) here

	{
	for (int i = 0; i < m_iPortCount; i++)
		if (m_pPort[i].iStatus != psEmpty)
			if (pObj == m_pPort[i].pObj)
				return true;

	return false;
	}

void CDockingPorts::MoveAll (CSpaceObject *pOwner)

//	MoveAll
//
//	Move all docked objects to follow the owner

	{
	DEBUG_TRY

	CVector vVel = pOwner->GetVel();

	for (int i = 0; i < m_iPortCount; i++)
		if (m_pPort[i].iStatus == psInUse)
			{
			m_pPort[i].pObj->SetPos(GetPortPos(pOwner, m_pPort[i], m_pPort[i].pObj));
			m_pPort[i].pObj->SetVel(vVel);
			}

	DEBUG_CATCH
	}

void CDockingPorts::OnDestroyed (void)

//	OnDestroyed
//
//	If we've been destroyed, then release all objects

	{
	DEBUG_TRY

	for (int i = 0; i < m_iPortCount; i++)
		{
		if (m_pPort[i].iStatus == psDocking)
			m_pPort[i].pObj->OnDockingStop();

		m_pPort[i].iStatus = psEmpty;
		m_pPort[i].pObj = NULL;
		}

	DEBUG_CATCH
	}

void CDockingPorts::OnDockObjDestroyed (CSpaceObject *pOwner, const SDestroyCtx &Ctx)

//	OnDockObjDestroyed
//
//	The owner object has been destroyed, so we notify anyone who is docked with
//	it.
//
//	NOTE: This is sent out whenever OnObjDestroyed is sent out, but does not
//	require a subscription. E.g., it is called when a station is destroyed.

	{
	for (int i = 0; i < m_iPortCount; i++)
		if (m_pPort[i].pObj != NULL)
			m_pPort[i].pObj->FireOnDockObjDestroyed(pOwner, Ctx);
	}

void CDockingPorts::OnNewSystem (CSystem *pNewSystem)

//	OnNewSystem
//
//	If we've entered a new system, then release any objects in the previous
//	system.
//
//	NOTE: pNewSystem could be NULL in the case where we're ascending an object.

	{
	for (int i = 0; i < m_iPortCount; i++)
		if (m_pPort[i].pObj != NULL
				&& m_pPort[i].pObj->GetSystem() != pNewSystem)
			{
			if (m_pPort[i].iStatus == psDocking)
				m_pPort[i].pObj->OnDockingStop();

			m_pPort[i].iStatus = psEmpty;
			m_pPort[i].pObj = NULL;
			}
	}

void CDockingPorts::OnObjDestroyed (CSpaceObject *pOwner, CSpaceObject *pObj, bool *retbDestroyed)

//	OnObjDestroyed
//
//	OnObjDestroyed 

	{
	bool bDestroyed = false;

	for (int i = 0; i < m_iPortCount; i++)
		if (m_pPort[i].pObj == pObj)
			{
			m_pPort[i].pObj = NULL;
			m_pPort[i].iStatus = psEmpty;
			bDestroyed = true;
			}

	if (retbDestroyed)
		*retbDestroyed = bDestroyed;
	}

void CDockingPorts::ReadFromStream (CSpaceObject *pOwner, SLoadCtx &Ctx)

//	ReadFromStream
//
//	ReadFromStream 

	{
	DWORD dwLoad;

	Ctx.pStream->Read(m_iPortCount);
	if (m_iPortCount > 0)
		{
		m_pPort = new SDockingPort[m_iPortCount];
		for (int i = 0; i < m_iPortCount; i++)
			{
			Ctx.pStream->Read(dwLoad);
			bool bHas3DPos = (dwLoad & 0x80000000 ? true : false);
			dwLoad &= 0x7fffffff;

			m_pPort[i].iStatus = (DockingPortStatus)LOWORD(dwLoad);
			m_pPort[i].iLayer = (DockingPortLayer)HIWORD(dwLoad);

			//	Read the 3D object coordinates (if we have it)

			if (bHas3DPos)
				m_pPort[i].Pos.ReadFromStream(Ctx);

			//	Other params

			CSystem::ReadObjRefFromStream(Ctx, &m_pPort[i].pObj);
			m_pPort[i].vPos.ReadFromStream(*Ctx.pStream);
			if (Ctx.dwVersion >= 24)
				Ctx.pStream->Read(m_pPort[i].iRotation);

			//	In previous versions we did not set rotation, so set it now

			if (m_pPort[i].iRotation == -1)
				m_pPort[i].iRotation = (VectorToPolar(m_pPort[i].vPos) + 180) % 360;
			}
		}

	if (Ctx.dwVersion >= 81)
		Ctx.pStream->Read(m_iMaxDist);
	else
		m_iMaxDist = DEFAULT_DOCK_DISTANCE_LS;
	}

void CDockingPorts::RepairAll (CSpaceObject *pOwner, int iRepairRate)

//	RepairAll
//
//	RepairAll 

	{
	DEBUG_TRY

	if (iRepairRate > 0)
		{
		for (int i = 0; i < m_iPortCount; i++)
			if (m_pPort[i].iStatus == psInUse
					&& !m_pPort[i].pObj->IsPlayer()
					&& !pOwner->IsEnemy(m_pPort[i].pObj))
				{
				m_pPort[i].pObj->RepairDamage(iRepairRate);
				m_pPort[i].pObj->ClearCondition(CConditionSet::cndRadioactive);
				m_pPort[i].pObj->ScrapeOverlays();
				}
		}

	DEBUG_CATCH
	}

bool CDockingPorts::RequestDock (CSpaceObject *pOwner, CSpaceObject *pObj, int iPort)

//	RequestDock
//
//	RequestDock 

	{
	//	If the requested dock is full, then we fail.

	if (iPort != -1 && m_pPort[iPort].iStatus != psEmpty)
		{
		pObj->SendMessage(pOwner, CONSTLIT("Docking port no longer available"));
		return false;
		}

	//	Get the nearest free port

	int iEmptyPortsLeft;
	if (iPort == -1)
		iPort = FindNearestEmptyPort(pOwner, pObj, NULL, &iEmptyPortsLeft);
	else
		//	If the caller specifies a port then it must be the player, so we
		//	don't worry about leaving an empty port.
		iEmptyPortsLeft = 2;

	//	If we could not find a free port then deny docking service

	if (iPort == -1)
		{
		pObj->SendMessage(pOwner, CONSTLIT("No docking ports available"));
		return false;
		}

	//	If the requester is not the player and there is only one port left, then
	//	fail (we always reserve one port for the player).
	//
	//	[We also make an exception for any ship that the player is escorting,
	//	or for any of the player's wingmen.]

	CSpaceObject *pPlayer = g_pUniverse->GetPlayerShip();
	if (iEmptyPortsLeft < 2
			&& pPlayer
			&& pObj != pPlayer
			&& !pObj->IsPlayerEscortTarget()
			&& !pObj->IsPlayerWingman())
		{
		pObj->SendMessage(pOwner, CONSTLIT("No docking ports available"));
		return false;
		}

	//	Commence docking 

	pOwner->Communicate(pObj, msgDockingSequenceEngaged);
	pObj->OnDockingStart();

	m_pPort[iPort].iStatus = psDocking;
	m_pPort[iPort].pObj = pObj;

	//	Done

	return true;
	}

bool CDockingPorts::ShipsNearPort (CSpaceObject *pOwner, CSpaceObject *pRequestingObj, const CVector &vPortPos)

//	ShipsNearPort
//
//	Returns TRUE if there are ships near the given port

	{
	int i;
	CSystem *pSystem = pOwner->GetSystem();

	for (i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);
		if (pObj
				&& pObj->GetCategory() == CSpaceObject::catShip
				&& pObj->CanAttack()	//	Excludes ship sections
				&& pObj != pRequestingObj)
			{
			Metric rDist2 = (pObj->GetPos() - vPortPos).Length2();
			if (rDist2 < MIN_PORT_DISTANCE2 && !IsDockedOrDocking(pObj))
				return true;
			}
		}

	return false;
	}

void CDockingPorts::Undock (CSpaceObject *pOwner, CSpaceObject *pObj, bool *retbWasDocked)

//	Undock
//
//	Undock the object

	{
	int i;

	if (retbWasDocked)
		*retbWasDocked = false;

	for (i = 0; i < m_iPortCount; i++)
		if (m_pPort[i].pObj == pObj)
			{
			//	If we were docked (not just requesting dock) then return it.

			if (retbWasDocked && m_pPort[i].iStatus == psInUse)
				*retbWasDocked = true;

			//	Unfreeze controls if we're trying to dock

			if (m_pPort[i].iStatus == psDocking)
				pObj->OnDockingStop();

			m_pPort[i].iStatus = psEmpty;
			m_pPort[i].pObj = NULL;
			}
	}

void CDockingPorts::UpdateAll (SUpdateCtx &Ctx, CSpaceObject *pOwner)

//	UpdateAll
//
//	UpdateAll 

	{
	DEBUG_TRY

	int i;

	CSpaceObject *pPlayer = Ctx.pPlayer;
	Metric rDist2 = (pPlayer ? pPlayer->GetDistance2(pOwner) : 0.0);
	Metric rMaxDist = m_iMaxDist * LIGHT_SECOND;
	Metric rMaxDist2 = rMaxDist * rMaxDist;

	//	If owner is destroyed then don't bother checking for nearest player
	//	port.

	if (pPlayer 
			&& (pOwner->IsIntangible() 
				|| pOwner == pPlayer
				|| pPlayer->IsDestroyed()))
		pPlayer = NULL;

	//	Also, don't bother checking if the owner is an enemy of the player.

	if (pPlayer && pPlayer->IsEnemy(pOwner) && !pOwner->IsAbandoned())
		pPlayer = NULL;

	//	If this is a stargate and we are at the center (just came through) 
	//	then don't bother showing docking ports.

	if (pPlayer && pOwner->IsStargate() && rDist2 < GATE_DIST2)
		pPlayer = NULL;

	//	Don't bother if the docking is disabled (for the player)

	if (pPlayer && !pOwner->SupportsDockingFast())
		pPlayer = NULL;

	//	Don't bother checking if the station is too far

	if (pPlayer)
		{
		Metric rMaxRadius = (0.5 * pOwner->GetHitSize()) + (LIGHT_SECOND * m_iMaxDist);
		if (rDist2 > rMaxRadius * rMaxRadius)
			pPlayer = NULL;
		}

	//	Loop over all ports

	for (i = 0; i < m_iPortCount; i++)
		{
		//	If a ship is docking with this port, then maneuver the ship towards
		//	the docking port.

		if (m_pPort[i].iStatus == psDocking)
			{
			if (m_pPort[i].pObj)
				UpdateDockingManeuvers(pOwner, m_pPort[i]);
			else
				m_pPort[i].iStatus = psEmpty;
			}

		//	Otherwise, if the port is open, see if this is the nearest port to
		//	the current player position.

		else if (m_pPort[i].iStatus == psEmpty)
			{
			if (pPlayer)
				{
				//	Compute the distance from the player to the port

				CVector vPortPos = GetPortPos(pOwner, m_pPort[i], pPlayer);
				Metric rDist2 = (vPortPos - pPlayer->GetPos()).Length2();

				//	If this is a better port, then replace the existing 
				//	solution.

				if (rDist2 <= rMaxDist2
						&& (Ctx.pDockingObj == NULL || rDist2 < Ctx.rDockingPortDist2))
					{
					Ctx.pDockingObj = pOwner;
					Ctx.iDockingPort = i;
					Ctx.rDockingPortDist2 = rDist2;
					Ctx.vDockingPort = vPortPos;
					}
				}
			}
		}

	DEBUG_CATCH
	}

void CDockingPorts::UpdateDockingManeuvers (CSpaceObject *pOwner, SDockingPort &Port)

//	UpdateDockingManeuvers
//
//	Updates the motion of a ship docking with a port

	{
	DEBUG_TRY

	CShip *pShip = Port.pObj->AsShip();

	ASSERT(pShip);
	if (pShip == NULL)
		return;

	int iPortRotation;
	CVector vDest = GetPortPos(pOwner, Port, pShip, NULL, &iPortRotation);
	CVector vDestVel = pOwner->GetVel();

	//	Figure out how far we are from where we want to be

	CVector vDelta = vDest - pShip->GetPos();

	//	If the docking object is within the appropriate threshold 
	//	of the port, then complete the docking sequence.

	Metric rDelta2 = vDelta.Length2();
	if (rDelta2 < DOCKING_THRESHOLD2 
			&& (pShip == g_pUniverse->GetPlayerShip() || pShip->IsPointingTo(iPortRotation)))
		{
		pShip->Place(vDest);
		pShip->OnDockingStop();
		IShipController *pController = pShip->GetController();
		pController->SetManeuver(NoRotation);

		Port.iStatus = psInUse;

		//	Tell the owner that someone has docked with it first
		//	(We do this because sometimes we want to handle stuff
		//	in OnObjDocked before we show the player a dock screen)

		if (pOwner 
				&& pOwner->HasOnObjDockedEvent() 
				&& pOwner != pShip
				&& !pOwner->IsDestroyed()
				&& pShip->IsSubscribedToEvents(pOwner))
			pOwner->FireOnObjDocked(pShip, pOwner);

		//	Notify any overlays on the station that a ship docked.
		//	Sometimes overlays react to dockings.

		COverlayList *pOverlays;
		if (pOwner
				&& (pOverlays = pOwner->GetOverlays())
				&& pOwner != pShip
				&& !pOwner->IsDestroyed())
			pOverlays->FireOnObjDocked(pOwner, pShip);

		//	Set the owner as "explored" (if the ship is the player)

		if (pShip->IsPlayer())
			{
			pOwner->SetExplored();

			if (pOwner->IsAutoClearDestinationOnDock())
				pOwner->ClearPlayerDestination();
			}

		//	Dock

		pShip->OnDocked(pOwner);

		//	Notify other subscribers

		pShip->NotifyOnObjDocked(pOwner);
		}

	//	Otherwise accelerate the ship towards the docking port

	else
		{
		Metric rMaxSpeed = pShip->GetMaxSpeed();
		Metric rMinSpeed = rMaxSpeed / 10.0;

		//	We slow down as we get closer

		Metric rSpeed;
		if (rDelta2 < FINAL_DOCKING2)
			rSpeed = rMinSpeed;
		else if (rDelta2 < FINAL_APPROACH2)
			{
			Metric rSpeedRange = rMaxSpeed - rMinSpeed;
			Metric rDelta = sqrt(rDelta2);
			rSpeed = rMinSpeed + (rSpeedRange * (rDelta - FINAL_DOCKING) / (FINAL_APPROACH - FINAL_DOCKING));
			}
		else
			rSpeed = rMaxSpeed;

		//	Figure out the ideal velocity vector that we want to
		//	be following.

		CVector vIdealVel = vDelta.Normal() * rSpeed;

		//	Calculate the delta v that we need

		CVector vDeltaV = vIdealVel - pShip->GetVel();

		//	Rotate

		if (!pShip->IsPlayer())
			{
			IShipController *pController = pShip->GetController();

			//	If we're close enough, align to rotation angle

			if (rDelta2 < FINAL_APPROACH2)
				pController->SetManeuver(pShip->GetManeuverToFace(iPortRotation));

			//	Otherwise, align along delta v

			else
				pController->SetManeuver(pShip->GetManeuverToFace(VectorToPolar(vDeltaV)));

			//	Don't let the AI thrust

			pController->SetThrust(false);
			}

		//	Accelerate

		pShip->Accelerate(vDeltaV * pShip->GetMass() / 10000.0, g_SecondsPerUpdate);
		pShip->ClipSpeed(rSpeed);
		}

	DEBUG_CATCH
	}

void CDockingPorts::UpdatePortsFromXML (CSpaceObject *pOwner, CXMLElement *pElement, int iScale)

//	UpdatePortsFromXML
//
//	Updates the port positions in case they changed.

	{
	int i;

	CDockingPorts NewPorts;
	NewPorts.InitPortsFromXML(pOwner, pElement, iScale);

	int iCount = Min(NewPorts.GetPortCount(), GetPortCount());
	for (i = 0; i < iCount; i++)
		{
		m_pPort[i].Pos = NewPorts.m_pPort[i].Pos;
		m_pPort[i].iLayer = NewPorts.m_pPort[i].iLayer;
		m_pPort[i].iRotation = NewPorts.m_pPort[i].iRotation;
		m_pPort[i].vPos = NewPorts.m_pPort[i].vPos;

		//	LATER: We need to update the position of the actual ship docked
		//	here. Unfortunately, if we call this at load time we won't yet have
		//	the ship object pointer. We'll have to call this at a later point
		//	(or fix up the ships at a later point).
		}

	m_iMaxDist = NewPorts.m_iMaxDist;
	m_iLastRotation = NewPorts.m_iLastRotation;
	}

void CDockingPorts::WriteToStream (CSpaceObject *pOwner, IWriteStream *pStream)

//	WriteToStream
//
//	WriteToStream 

	{
	DWORD dwSave;

	pStream->Write(m_iPortCount);
	for (int i = 0; i < m_iPortCount; i++)
		{
		//	Save flags

		dwSave = MAKELONG((WORD)m_pPort[i].iStatus, (WORD)m_pPort[i].iLayer);

		//	We use the high-bit to store whether or not we have a 3D object position.

		dwSave |= (!m_pPort[i].Pos.IsEmpty() ?	0x80000000 : 0);
		pStream->Write(dwSave);

		//	3D position

		if (!m_pPort[i].Pos.IsEmpty())
			m_pPort[i].Pos.WriteToStream(*pStream);

		//	Other params

		pOwner->WriteObjRefToStream(m_pPort[i].pObj, pStream);
		m_pPort[i].vPos.WriteToStream(*pStream);
		pStream->Write(m_pPort[i].iRotation);
		}

	pStream->Write(m_iMaxDist);
	}
