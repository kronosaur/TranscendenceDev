//	IShipController.cpp
//
//	Implementation of IShipController class
//	Copyright (c) 2018 Kronosaur Productions. All Rights Reserved.

#include "PreComp.h"

const IShipController::SOrderTypeData IShipController::m_OrderTypes[] =
	{
		//	name						target	data
		{	"",							"-",	"-",	0	 },

		{	"guard",					"o",	"-",	ORDER_FLAG_DELETE_ON_STATION_DESTROYED },
		{	"dock",						"o",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"attack",					"o",	"i",	ORDER_FLAG_DELETE_ON_STATION_DESTROYED | ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP | ORDER_FLAG_DELETE_ON_OLD_SHIP_WAITS },
		{	"wait",						"-",	"i",	0 },
		{	"gate",						"*",	"-",	0 },

		{	"gateOnThreat",				"-",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"gateOnStationDestroyed",	"-",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"patrol",					"o",	"i",	ORDER_FLAG_DELETE_ON_STATION_DESTROYED | ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP	},
		{	"escort",					"o",	"2",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED | ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP	},
		{	"scavenge",					"-",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },

		{	"followPlayerThroughGate",	"o",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED | ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP },
		{	"attackNearestEnemy",		"-",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"tradeRoute",				"-",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"wander",					"-",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"loot",						"o",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },

		{	"hold",						"-",	"i",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"mine",						"o",	"-",	ORDER_FLAG_DELETE_ON_STATION_DESTROYED | ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"waitForPlayer",			"-",	"-",	0 },
		{	"attackPlayerOnReturn",		"-",	"-",	0 },
		{	"follow",					"o",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED | ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP },

		{	"navPath",					"-",	"i",	0	},
		{	"goto",						"o",	"-",	0 },
		{	"waitForTarget",			"o",	"2",	ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP },
		{	"waitForEnemy",				"-",	"i",	0 },
		{	"bombard",					"o",	"i",	ORDER_FLAG_DELETE_ON_STATION_DESTROYED },

		{	"approach",					"o",	"i",	ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP },
		{	"aim",						"o",	"-",	ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP },
		{	"orbit",					"o",	"2",	ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP },
		{	"holdCourse",				"-",	"2",	0 },
		{	"turnTo",					"-",	"i",	0 },

		{	"attackHold",				"o",	"i",	ORDER_FLAG_DELETE_ON_STATION_DESTROYED | ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP | ORDER_FLAG_DELETE_ON_OLD_SHIP_WAITS },
		{	"attackStation",			"o",	"i",	ORDER_FLAG_DELETE_ON_STATION_DESTROYED },
		{	"fireEvent",				"o",	"s",	0 },
		{	"waitForUndock",			"o",	"i",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"sendMessage",				"o",	"s",	0 },

		{	"attackArea",				"o",	"2",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"holdAndAttack",			"o",	"i",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED | ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP | ORDER_FLAG_DELETE_ON_OLD_SHIP_WAITS },
		{	"gotoPos",					"-",	"v",	0 },
		{	"waitForThreat",			"-",	"i",	0 },
		{	"sentry",					"*",	"i",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },

		{	"fireWeapon",				"-",	"I",	0 },
		{	"useItem",					"-",	"I",	0 },
		{	"orbitExact",				"o",	"k",	0 },
	};

const int IShipController::ORDER_TYPES_COUNT = (sizeof(m_OrderTypes) / sizeof(m_OrderTypes[0]));

IShipController::EDataTypes IShipController::GetOrderDataType (OrderTypes iOrder)

//	GetOrderDataType
//
//	Returns the data type used by the order.

	{
	switch (m_OrderTypes[iOrder].szData[0])
		{
		case '-':
			return dataNone;

		case 'i':
			return dataInteger;

		case 'I':
			return dataItem;

		case 'k':
			return dataOrbitExact;

		case 's':
			return dataString;

		case 'v':
			return dataVector;

		case '2':
			return dataPair;

		default:
			return dataNone;
		}
	}

IShipController::OrderTypes IShipController::GetOrderType (const CString &sString)

//	GetOrderID
//
//	Loads an order type from a string

	{
	int iType;

	for (iType = 0; iType < ORDER_TYPES_COUNT; iType++)
		if (strEquals(sString, CString(m_OrderTypes[iType].szName)))
			return (IShipController::OrderTypes)iType;

	return IShipController::orderNone;
	}

bool IShipController::OrderHasTarget (IShipController::OrderTypes iOrder, bool *retbRequired)

//	OrderHasTarget
//
//	Returns TRUE if the given order has a target

	{
	if (retbRequired)
		*retbRequired = (*m_OrderTypes[iOrder].szTarget == 'o');

	return (*m_OrderTypes[iOrder].szTarget != '-');
	}

bool IShipController::ParseOrderData (CCodeChainCtx &CCX, OrderTypes iOrder, const ICCItem &Args, int iFirstArg, SData &retData)

//	ParseOrderData
//
//	Parses order data arguments to (shpOrder ...)

	{
	IShipController::EDataTypes iDataType = IShipController::GetOrderDataType(iOrder);

	//	If we don't have at least one argument, then we have null data.

	if (Args.GetCount() <= iFirstArg)
		{
		retData.iDataType = dataNone;
		}

	//	Handle based on the data type we expect.

	else
		{
		const ICCItem &Value = *Args.GetElement(iFirstArg);

		switch (iDataType)
			{
			case IShipController::dataItem:
				if (!Value.IsNil())
					{
					retData.iDataType = iDataType;
					retData.Item = CCX.AsItem(&Value);
					}
				else
					retData.iDataType = dataNone;
				break;

			case IShipController::dataOrbitExact:
				{
				retData.iDataType = iDataType;

				//	If we have a struct, then these are orbital parameters.

				if (Value.IsSymbolTable())
					{
					//	Radius in light-seconds

					DWORD dwRadius;
					if (const ICCItem *pRadius = Value.GetElement(CONSTLIT("radius")))
						dwRadius = pRadius->GetIntegerValue();
					else
						dwRadius = 10;

					//	Initial angle (degrees)

					DWORD dwAngle;
					if (const ICCItem *pAngle = Value.GetElement(CONSTLIT("angle")))
						dwAngle = pAngle->GetIntegerValue();
					else
						dwAngle = 0;

					retData.dwData1 = (dwAngle << 16) | dwRadius;

					//	Angular speed in degrees per tick.

					if (const ICCItem *pSpeed = Value.GetElement(CONSTLIT("speed")))
						retData.vData.SetX(pSpeed->GetDoubleValue());
					else
						retData.vData.SetX(2.0);

					if (const ICCItem *pEccentricity = Value.GetElement(CONSTLIT("eccentricity")))
						retData.vData.SetY(pEccentricity->GetDoubleValue());
					else
						retData.vData.SetY(0.0);
					}

				//	Otherwise, we assume just a radius (in light-seconds).

				else
					{
					retData.dwData1 = Value.GetIntegerValue();
					if (retData.dwData1 == 0)
						retData.dwData1 = 10;
					retData.vData = CVector();
					}

				//	If we have a second parameter, then it is a timer in ticks.

				if (Args.GetCount() > (iFirstArg + 1))
					{
					const ICCItem &Value2 = *Args.GetElement(iFirstArg + 1);
					retData.dwData2 = Value2.GetIntegerValue();
					}
				else
					retData.dwData2 = 0;

				break;
				}

			case IShipController::dataString:
				if (!Value.IsNil())
					{
					retData.iDataType = iDataType;
					retData.sData = Value.GetStringValue();
					}
				else
					retData.iDataType = dataNone;
				break;

			case IShipController::dataVector:
				if (!Value.IsNil())
					{
					retData.iDataType = iDataType;
					retData.vData = ::CreateVectorFromList(CCX.GetCC(), &Value);
					}
				else
					retData.iDataType = dataNone;
				break;

			//	Assume 1 or 2 integers

			default:
				{
				if (Args.GetCount() > (iFirstArg + 1))
					{
					const ICCItem &Value2 = *Args.GetElement(iFirstArg + 1);

					if (!Value.IsNil() || !Value2.IsNil())
						{
						retData.iDataType = IShipController::dataPair;
						retData.dwData1 = Value.GetIntegerValue();
						retData.dwData2 = Value2.GetIntegerValue();
						}
					else
						{
						//	If both arguments are Nil, then we omit them both. We do this
						//	because some orders (like escort) behave differently depending
						//	on whether arguments are nil or not.

						retData.iDataType = dataNone;
						}
					}
				else
					{
					if (!Value.IsNil())
						{
						retData.iDataType = IShipController::dataInteger;
						retData.dwData1 = Value.GetIntegerValue();
						}
					else
						retData.iDataType = dataNone;
					}
				}
			}
		}

	return true;
	}

bool IShipController::ParseOrderString (const CString &sValue, OrderTypes *retiOrder, IShipController::SData *retData)

//	ParseOrderString
//
//	Parses an order string of the form:
//
//	{order}:{d1}:{d2}

	{
	const char *pPos = sValue.GetASCIIZPointer();

	//	Parse the order name

	const char *pStart = pPos;
	while (*pPos != '\0' && *pPos != ':')
		pPos++;

	CString sOrder(pStart, (int)(pPos - pStart));

	//	For backwards compatibility we handle some special names.

	IShipController::OrderTypes iOrder;
	if (strEquals(sOrder, CONSTLIT("trade route")))
		iOrder = orderTradeRoute;
	else
		{
		iOrder = GetOrderType(sOrder);

		//	Check for error

		if (iOrder == orderNone && !sOrder.IsBlank())
			return false;
		}

	//	Get additional data

	if (!retData)
		{ }
	else if (*pPos != ':')
		*retData = SData();
	else
		{
		IShipController::EDataTypes iDataType = IShipController::GetOrderDataType(iOrder);

		switch (iDataType)
			{
			case dataOrbitExact:
				{
				*retData = SData();
				retData->iDataType = iDataType;

				pPos++;

				DWORD dwRadius = 10;
				DWORD dwAngle = 0;

				while (*pPos != '\0')
					{
					pStart = pPos;
					while (*pPos != '\0' && *pPos != '=')
						pPos++;

					CString sField(pStart, pPos - pStart);
					if (*pPos != '=')
						return false;

					pPos++;
					pStart = pPos;
					while (*pPos != '\0' && *pPos != ':')
						pPos++;

					CString sValue(pStart, pPos - pStart);

					if (strEquals(sField, CONSTLIT("radius")))
						dwRadius = strToInt(sValue, 0);
					else if (strEquals(sField, CONSTLIT("angle")))
						{
						if (strEquals(sValue, CONSTLIT("random")))
							dwAngle = mathRandom(0, 359);
						else
							dwAngle = strToInt(sValue, 0);
						}
					else if (strEquals(sField, CONSTLIT("speed")))
						retData->vData.SetX(strToDouble(sValue, 0.0));
					else if (strEquals(sField, CONSTLIT("eccentricity")))
						retData->vData.SetY(strToDouble(sValue, 0.0));
					else if (strEquals(sField, CONSTLIT("time")))
						retData->dwData2 = strToInt(sValue, 0);

					if (*pPos == ':')
						pPos++;
					}

				retData->dwData1 = (dwAngle << 16) | dwRadius;

				break;
				}

			default:
				{
				DWORD dwData1 = 0;
				DWORD dwData2 = 0;

				pPos++;
				pStart = pPos;
				while (*pPos != '\0' && *pPos != ':')
					pPos++;

				CString sData(pStart, (int)(pPos - pStart));
				dwData1 = strToInt(sData, 0);

				if (*pPos != ':')
					*retData = SData(dwData1);
				else
					{
					pPos++;
					pStart = pPos;
					while (*pPos != '\0')
						pPos++;

					CString sData(pStart, (int)(pPos - pStart));
					dwData2 = strToInt(sData, 0);

					*retData = SData(dwData1, dwData2);
					}

				break;
				}
			}
		}

	//	Done

	*retiOrder = iOrder;
	return true;
	}

