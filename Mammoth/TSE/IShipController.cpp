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

bool IShipController::ParseOrderString (const CString &sValue, OrderTypes *retiOrder, IShipController::SData *retData)

//	ParseOrderString
//
//	Parses an order string of the form:
//
//	{order}:{d1}:{d2}

	{
	char *pPos = sValue.GetASCIIZPointer();

	//	Parse the order name

	char *pStart = pPos;
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

	if (retData)
		{
		DWORD dwData1 = 0;
		DWORD dwData2 = 0;

		if (*pPos != ':')
			*retData = SData();
		else
			{
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
			}
		}

	//	Done

	*retiOrder = iOrder;
	return true;
	}

