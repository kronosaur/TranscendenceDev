//	CCExtensions.cpp
//
//	CodeChain extensions

#include "PreComp.h"

#define FN_ARM_NAME					1
#define FN_ARM_HITPOINTS			2
#define FN_ARM_REPAIRCOST			3

#define FN_ARM_REPAIRTECH			5
#define FN_ARM_IS_RADIATION_IMMUNE	6

ICCItem *fnArmGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_CURRENCY_EXCHANGE		1

ICCItem *fnCurrency (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_ENV_HAS_ATTRIBUTE		1

ICCItem *fnEnvironmentGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_DEBUG_OUTPUT				1
#define FN_DEBUG_LOG				2
#define FN_PRINT					3
#define FN_PRINT_TO					4
#define FN_API_VERSION				5
#define FN_DEBUG_IS_ACTIVE			6
#define FN_DEBUG_GET				7
#define FN_DEBUG_SET				8

ICCItem *fnDebug (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_CURRENCY					1
#define FN_NAME						2
#define FN_NUMBER                   3
#define FN_POWER					4
#define FN_VERB						5
#define FN_COMPOSE					6

ICCItem *fnFormat (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_ITEM_COUNT				1
#define FN_ITEM_PRICE				2
#define FN_ITEM_NAME				3
#define FN_ITEM_DAMAGED				4
#define FN_ITEM_USE_SCREEN			5
#define FN_ITEM_FIRE_EVENT			6
#define FN_ITEM_SET_KNOWN			7
#define FN_ITEM_KNOWN				8
#define FN_ITEM_ARMOR_TYPE			9
#define FN_ITEM_MATCHES				10
#define FN_ITEM_CATEGORY			11
#define FN_ITEM_ENHANCED			12
#define FN_ITEM_MASS				13
#define FN_ITEM_HAS_MODIFIER		14
#define FN_ITEM_INSTALLED			15
#define FN_ITEM_SET_REFERENCE		16
#define FN_ITEM_REFERENCE			17
#define FN_ITEM_LEVEL				18
#define FN_ITEM_UNID				19
#define FN_ITEM_ARMOR_INSTALLED_LOCATION	20
#define FN_ITEM_CHARGES				21
#define FN_ITEM_INSTALL_COST		22
#define FN_ITEM_ACTUAL_PRICE		23
#define FN_ITEM_MAX_APPEARING		24
#define FN_ITEM_INSTALL_POS			25
#define FN_ITEM_IMAGE_DESC			26
#define FN_ITEM_DAMAGE_TYPE			27
#define FN_ITEM_TYPES				28
#define FN_ITEM_DATA				29
#define FN_ITEM_GET_STATIC_DATA		30
#define FN_ITEM_GET_GLOBAL_DATA		31
#define FN_ITEM_SET_GLOBAL_DATA		32
#define FN_ITEM_AVERAGE_APPEARING	33
#define FN_ITEM_ADD_ENHANCEMENT		34
#define FN_ITEM_PROPERTY			35
#define FN_ITEM_GET_TYPE_DATA		36
#define FN_ITEM_IS_EQUAL			37

ICCItem *fnItemGetTypes (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnItemGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnItemSet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_ITEM_TYPE_SET_KNOWN		1
#define FN_ITEM_DEFAULT_CURRENCY	2
#define FN_ITEM_FREQUENCY			3

ICCItem *fnItemTypeGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnItemTypeSet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_OBJ_GETDATA				1
#define FN_OBJ_SETDATA				2
#define FN_OBJ_GET_OBJREF_DATA		3
#define FN_OBJ_SET_OBJREF_DATA		4
#define FN_OBJ_GET_STATIC_DATA		5
#define FN_OBJ_INCREMENT_DATA		6
#define FN_OBJ_GET_GLOBAL_DATA		7
#define FN_OBJ_SET_GLOBAL_DATA		8
#define FN_OBJ_GET_STATIC_DATA_FOR_STATION_TYPE	9

ICCItem *fnObjAddRandomItems (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnObjData (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnObjSendMessage (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnObjActivateItem(CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_OBJ_NAME					1
#define FN_OBJ_IS_SHIP				2
#define FN_OBJ_DISTANCE				3
#define FN_OBJ_KNOWN				4
#define FN_OBJ_SET_KNOWN			5
#define FN_OBJ_IS_UNDER_ATTACK		6
#define FN_OBJ_IS_ABANDONED			7
#define FN_OBJ_MESSAGE				8
#define FN_OBJ_NEAREST_STARGATE		9
#define FN_OBJ_POSITION				10
#define FN_OBJ_JUMP					11
#define FN_OBJ_REGISTER_EVENTS		12
#define FN_OBJ_UNREGISTER_EVENTS	13
#define FN_OBJ_DAMAGE_TYPE			14
#define FN_OBJ_PARALYSIS			15
#define FN_OBJ_LOWER_SHIELDS		16
#define FN_OBJ_VISIBLE_DAMAGE		17
#define FN_OBJ_TARGET				18
#define FN_OBJ_CARGO_SPACE_LEFT		19
#define FN_OBJ_ATTRIBUTE			20
#define FN_OBJ_ORDER_GIVER			21
#define FN_OBJ_MAX_POWER			22
#define FN_OBJ_DESTINY				23
#define FN_OBJ_SHIELD_LEVEL			24
#define FN_OBJ_INSTALLED_ITEM_DESC	25
#define FN_OBJ_COMBAT_POWER			26
#define FN_OBJ_SOVEREIGN			27
#define FN_OBJ_ADD_SUBORDINATE		28
#define FN_OBJ_ENEMY				29
#define FN_OBJ_DESTROY				30
#define FN_OBJ_CAN_ATTACK			31
#define FN_OBJ_INCREMENT_VELOCITY	32
#define FN_OBJ_GET_SELL_PRICE		33
#define FN_OBJ_GET_BUY_PRICE		34
#define FN_OBJ_CHARGE				35
#define FN_OBJ_GET_BALANCE			36
#define FN_OBJ_GET_ID				37
#define FN_OBJ_FIRE_EVENT			38
#define FN_OBJ_CREDIT				39
#define FN_OBJ_IDENTIFIED			40
#define FN_OBJ_CLEAR_IDENTIFIED		41
#define FN_OBJ_DEPLETE_SHIELDS		42
#define FN_OBJ_OBJECT				43
#define FN_OBJ_MATCHES				44
#define FN_OBJ_DOCKED_AT			45
#define FN_OBJ_IMAGE				46
#define FN_OBJ_VELOCITY				47
#define FN_OBJ_LEVEL				48
#define FN_OBJ_FIRE_ITEM_EVENT		49
#define FN_OBJ_MASS					50
#define FN_OBJ_OPEN_DOCKING_PORT_COUNT	51
#define FN_OBJ_SUSPEND				52
#define FN_OBJ_RESUME				53
#define FN_OBJ_DATA_FIELD			54
#define FN_OBJ_REGISTER_SYSTEM_EVENTS	55
#define FN_OBJ_UNREGISTER_SYSTEM_EVENTS	56
#define FN_OBJ_FIX_PARALYSIS		57
#define FN_OBJ_ADD_BUY_ORDER		58
#define FN_OBJ_ADD_SELL_ORDER		59
#define FN_OBJ_ADD_OVERLAY			60
#define FN_OBJ_REMOVE_OVERLAY		61
#define FN_OBJ_GET_OVERLAY_DATA		62
#define FN_OBJ_SET_OVERLAY_DATA		63
#define FN_OBJ_INC_OVERLAY_DATA		64
#define FN_OBJ_GET_OVERLAY_TYPE		65
#define FN_OBJ_GET_OVERLAY_POS		66
#define FN_OBJ_DAMAGE				67
#define FN_OBJ_GET_OVERLAYS			68
#define FN_OBJ_SET_ITEM_DATA		69
#define FN_OBJ_SET_OVERLAY_ROTATION	70
#define FN_OBJ_GET_OVERLAY_ROTATION	71
#define FN_OBJ_SET_ITEM_CHARGES		72
#define FN_OBJ_INC_ITEM_CHARGES		73
#define FN_OBJ_SET_AS_DESTINATION	74
#define FN_OBJ_CLEAR_AS_DESTINATION	75
#define FN_OBJ_CAN_DETECT_TARGET	76
#define FN_OBJ_GET_NAMED_ITEM		77
#define FN_OBJ_FIRE_OVERLAY_EVENT	78
#define FN_OBJ_ADD_ITEM_ENHANCEMENT	79
#define FN_OBJ_REMOVE_ITEM_ENHANCEMENT	80
#define FN_OBJ_SET_EVENT_HANDLER	81
#define FN_OBJ_ARMOR_LEVEL			82
#define FN_OBJ_PARALYZED			83
#define FN_OBJ_IS_ANGRY_AT			84
#define FN_OBJ_GET_EVENT_HANDLER	85
#define FN_OBJ_DEVICE_ACTIVATION_DELAY	86
#define FN_OBJ_TYPE					87
#define FN_OBJ_ARMOR_TYPE			88
#define FN_OBJ_GET_DISPOSITION		89
#define FN_OBJ_RADIOACTIVE			90
#define FN_OBJ_SHIPWRECK_TYPE		91
#define FN_OBJ_IS_DEVICE_ENABLED	92
#define FN_OBJ_DEFAULT_CURRENCY		93
#define FN_OBJ_DEVICE_FIRE_ARC		94
#define FN_OBJ_DEVICE_POS			95
#define FN_OBJ_ACCELERATE			96
#define FN_OBJ_SET_TRADE_DESC		97
#define FN_OBJ_SET_OVERLAY_POSITION	98
#define FN_OBJ_GET_ABILITY			99
#define FN_OBJ_SET_ABILITY			100
#define FN_OBJ_HAS_ABILITY			101
#define FN_OBJ_TRANSLATE			102
#define FN_OBJ_GET_STARGATE_ID		103
#define FN_OBJ_SET_PROPERTY			104
#define FN_OBJ_GET_PROPERTY			105
#define FN_OBJ_DEVICE_LINKED_FIRE_OPTIONS	106
#define FN_OBJ_ARMOR_CRITICALITY	107
#define FN_OBJ_SET_ITEM_PROPERTY	108
#define FN_OBJ_GET_ITEM_PROPERTY	109
#define FN_OBJ_SET_OVERLAY_EFFECT_PROPERTY	110
#define FN_OBJ_GET_REFUEL_ITEM		111
#define FN_OBJ_ARMOR_REPAIR_PRICE	112
#define FN_OBJ_ARMOR_REPLACE_PRICE	113
#define FN_OBJ_GET_PLAYER_PRICE_ADJ	114
#define FN_OBJ_CAN_INSTALL_ITEM		115
#define FN_OBJ_ADD_ITEM				116
#define FN_OBJ_GET_OVERLAY_PROPERTY	117
#define FN_OBJ_SET_OVERLAY_PROPERTY	118
#define FN_OBJ_GET_SHIP_SELL_PRICE	119
#define FN_OBJ_GET_SHIP_BUY_PRICE	120
#define FN_OBJ_FIRE_ITEM_INVOKE		121
#define FN_OBJ_RECORD_BUY_ITEM		122
#define FN_OBJ_CALC_BEST_TARGET		123
#define FN_OBJ_ADD_CONNECTION		124
#define FN_OBJ_GET_DETECT_RANGE		125
#define FN_OBJ_MESSAGE_TRANSLATE	126
#define FN_OBJ_FIRE_WEAPON			127
#define FN_OBJ_CREATE_REFLECTION	128
#define FN_OBJ_FIRE_POWER_INVOKE	129
#define FN_OBJ_ADD_TRADE_ORDER		130
#define FN_OBJ_GET_CHARACTER_DATA	131
#define FN_OBJ_SET_CHARACTER_DATA	132
#define FN_OBJ_HAS_SERVICE			133
#define FN_OBJ_CONDITION			134

#define NAMED_ITEM_SELECTED_WEAPON		CONSTLIT("selectedWeapon")
#define NAMED_ITEM_SELECTED_LAUNCHER	CONSTLIT("selectedLauncher")
#define NAMED_ITEM_SELECTED_MISSILE		CONSTLIT("selectedMissile")

#define DEVICE_FIRE_ARC_OMNI			CONSTLIT("omnidirectional")

ICCItem *fnObjGet (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnObjSet (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnObjGetOld (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnObjSetOld (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnObjIDGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_OBJ_REMOVE_ITEM			2
#define FN_OBJ_ENUM_ITEMS			3
#define FN_OBJ_HAS_ITEM				4

ICCItem *fnObjItem (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnObjItemOld (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);

#define FN_OBJ_ARMOR_DAMAGE			2
#define FN_OBJ_REPAIR_ARMOR			3
#define FN_OBJ_ARMOR_MAX_HP			5
//	spare
#define FN_OBJ_ARMOR_NAME			7
#define FN_OBJ_ARMOR_ITEM			8

ICCItem *fnObjGetArmor (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);

#define FN_SHIP_DEVICE_SLOT_AVAIL	1
#define FN_SHIP_INSTALL_AUTOPILOT	2
#define FN_SHIP_HAS_AUTOPILOT		3
//	spare
#define FN_SHIP_INSTALL_TARGETING	5
#define FN_SHIP_HAS_TARGETING		6
#define FN_SHIP_CLASS				7
#define FN_SHIP_INSTALL_DEVICE		8
#define FN_SHIP_INSTALL_ARMOR		9
#define FN_SHIP_FUEL_NEEDED			10
#define FN_SHIP_REFUEL_FROM_ITEM	11
#define FN_SHIP_REMOVE_DEVICE		12
#define FN_SHIP_ORDER_DOCK			13
#define FN_SHIP_BLINDNESS			14
#define FN_SHIP_ENHANCE_ITEM		15
#define FN_SHIP_CAN_INSTALL_DEVICE	16
#define FN_SHIP_DECONTAMINATE		17
#define FN_SHIP_IS_RADIOACTIVE		18
#define FN_SHIP_CANCEL_ORDERS		19
#define FN_SHIP_ADD_ENERGY_FIELD	20
#define FN_SHIP_IS_FUEL_COMPATIBLE	21
#define FN_SHIP_ITEM_DEVICE_NAME	22
#define FN_SHIP_ORDER_ATTACK		23
#define FN_SHIP_IS_SRS_ENHANCED		24
#define FN_SHIP_ENHANCE_SRS			25
#define FN_SHIP_SHIELD_ITEM_UNID	26
#define FN_SHIP_SHIELD_DAMAGE		27
#define FN_SHIP_SHIELD_MAX_HP		28
#define FN_SHIP_RECHARGE_SHIELD		29
#define FN_SHIP_MAKE_RADIOACTIVE	30
#define FN_SHIP_ORDER_GATE			31
#define FN_SHIP_ORDER_ESCORT		32
#define FN_SHIP_ORDER_LOOT			33
#define FN_SHIP_FUEL				34
#define FN_SHIP_ORDER_PATROL		35
#define FN_SHIP_ARMOR_COUNT			36
#define FN_SHIP_RECHARGE_ITEM		37
#define FN_SHIP_ITEM_CHARGES		38
#define FN_SHIP_DIRECTION			39
#define FN_SHIP_ORDER_WAIT			40
#define FN_SHIP_FIX_BLINDNESS		41
#define FN_SHIP_ORDER_FOLLOW		42
#define FN_SHIP_DAMAGE_ITEM			43
#define FN_SHIP_CAN_INSTALL_ARMOR	44
#define FN_SHIP_IS_RADIATION_IMMUNE	45
#define FN_SHIP_REPAIR_ITEM			46
#define FN_SHIP_CAN_REMOVE_DEVICE	47
#define FN_SHIP_ORDER_MINE			48
#define FN_SHIP_CONTROLLER			49
#define FN_SHIP_ORDER				50
#define FN_SHIP_ORDER_TARGET		51
#define FN_SHIP_GET_GLOBAL_DATA		52
#define FN_SHIP_SET_GLOBAL_DATA		53
#define FN_SHIP_ORDER_HOLD			54
#define FN_SHIP_ORDER_GUARD			55
#define FN_SHIP_COMMAND_CODE		56
#define FN_SHIP_ORDER_GOTO			57
#define FN_SHIP_IS_NAMED_DEVICE		58
#define FN_SHIP_MAX_SPEED			59
#define FN_SHIP_CLASS_NAME			60
#define FN_SHIP_GET_IMAGE_DESC		62
#define FN_SHIP_DATA_FIELD			63
#define FN_SHIP_PLAYER_WINGMAN		64
#define FN_SHIP_ORDER_DESC			65
#define FN_SHIP_DOCK_OBJ			66
#define FN_SHIP_ORDER_IMMEDIATE		67
#define FN_SHIP_AI_SETTING			68
#define FN_SHIP_DAMAGE_ARMOR		69
#define FN_SHIP_CONSUME_FUEL		70
#define FN_SHIP_CANCEL_ORDER		71
#define FN_SHIP_ORDER_COUNT			72

ICCItem *fnShipGet (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnShipGetOld (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnShipSet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnShipSetOld (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnShipClass (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_STATION_GET_DOCKED_SHIPS		1
#define FN_STATION_IMAGE_VARIANT		2
#define FN_STATION_SET_ACTIVE			3
#define FN_STATION_SET_INACTIVE			4
#define FN_STATION_GET_SUBORDINATES		5
#define FN_STATION_GET_TYPE				6
#define FN_STATION_RECON				7
#define FN_STATION_CLEAR_RECON			8
#define FN_STATION_CLEAR_FIRE_RECON		9
#define FN_STATION_SET_FIRE_RECON		10
#define FN_STATION_GET_GLOBAL_DATA		11
#define FN_STATION_SET_GLOBAL_DATA		12
#define FN_STATION_STRUCTURAL_HP		13
#define FN_STATION_ENCOUNTERED			14
#define FN_STATION_MAX_STRUCTURAL_HP	15
#define FN_STATION_SHOW_MAP_LABEL		16

ICCItem *fnStationGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnStationGetOld (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnStationSet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnStationSetOld (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnStationType (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_SYS_CREATE_WEAPON_FIRE		1
#define FN_SYS_CREATE_SHIPWRECK			2
#define FN_SYS_CREATE_ENCOUNTER			3
#define FN_SYS_PLAY_SOUND				4
#define FN_SYS_CREATE_FLOTSAM			5
#define FN_SYS_CREATE_ENVIRONMENT		6
#define FN_SYS_CREATE_TERRITORY			7
#define FN_SYS_CREATE_LOOKUP			8
#define FN_SYS_CREATE_HIT_EFFECT		9

ICCItem *fnSystemCreate (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);

#define FN_ROLL_DICE					1
#define FN_ROLL_CHANCE					2

ICCItem *fnRollDice (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

ICCItem *fnSystemCreateEffect (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnSystemCreateMarker (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnSystemCreateShip (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnSystemCreateStation (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_SYS_FIND						0
#define FN_SYS_FIND_AT_POS				1

ICCItem *fnSystemFind (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

ICCItem *fnSystemGetObjectByName (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnSystemVectorOffset (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnObjComms (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnObjEnumItems (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnObjGateTo (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnProgramDamage (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnItemEnumTypes (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnItemList (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnItemCreate (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnItemCreateByName (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnItemCreateRandom (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_MISSION_CREATE				0
#define FN_MISSION_FIND					1
#define FN_MISSION_GET_PROPERTY			2
#define FN_MISSION_CLOSED				3
#define FN_MISSION_SUCCESS				4
#define FN_MISSION_FAILURE				5
#define FN_MISSION_ACCEPTED				6
#define FN_MISSION_REWARD				7
#define FN_MISSION_DECLINED				8
#define FN_MISSION_SET_PLAYER_TARGET	9
#define FN_MISSION_ADD_TIMER			10
#define FN_MISSION_ADD_RECURRING_TIMER	11
#define FN_MISSION_CANCEL_TIMER			12
#define FN_MISSION_CAN_CREATE			13

ICCItem *fnMission (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnMissionGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnMissionSet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_ADD_ENCOUNTER_FROM_GATE		0
#define FN_ADD_ENCOUNTER_FROM_DIST		1

ICCItem *fnSystemAddEncounterEvent (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData);

#define FN_ADD_TIMER_NORMAL				0
#define FN_ADD_TIMER_RECURRING			1
#define FN_CANCEL_TIMER					2
#define FN_ADD_TYPE_TIMER_NORMAL		3
#define FN_ADD_TYPE_TIMER_RECURRING		4
#define FN_CANCEL_TYPE_TIMER			5
#define FN_ADD_TYPE_RANGE_EVENT			6

ICCItem *fnSystemAddStationTimerEvent (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_SYS_NAME						1
#define FN_SYS_LEVEL					2
#define FN_SYS_NODE						3
#define FN_SYS_GET_DATA					4
#define FN_SYS_SET_DATA					5
#define FN_SYS_NAV_PATH_POINT			6
#define FN_SYS_GET_TRAVEL_TIME			7
#define FN_SYS_ENVIRONMENT				8
#define FN_SYS_ALL_NODES				9
#define FN_SYS_HAS_ATTRIBUTE			10
#define FN_SYS_CREATE_STARGATE			11
#define FN_SYS_STARGATES				12
#define FN_SYS_STARGATE_DESTINATION_NODE	13
#define FN_SYS_ADD_STARGATE_TOPOLOGY	14
#define FN_SYS_SYSTEM_TYPE				15
#define FN_SYS_GET_TRAVEL_DISTANCE		16
#define FN_SYS_GET_FIRE_SOLUTION		17
#define FN_SYS_CREATE_STATION			18
#define FN_SYS_TOPOLOGY_DISTANCE		19
#define FN_SYS_SET_KNOWN				20
#define FN_SYS_IS_KNOWN					21
#define FN_SYS_STARGATE_DESTINATION		22
#define FN_SYS_GET_PROPERTY				23
#define FN_SYS_SET_PROPERTY				24
#define FN_SYS_ASCEND_OBJECT			25
#define FN_SYS_DESCEND_OBJECT			26
#define FN_SYS_MATCHES					27
#define FN_SYS_SET_POV					28
#define FN_SYS_GET_STD_COMBAT_STRENGTH	29
#define FN_SYS_RANDOM_LOCATION			30
#define FN_SYS_ORBIT_CREATE				31
#define FN_SYS_HIT_SCAN					32
#define FN_SYS_LIGHT_INTENSITY			33
#define FN_SYS_INC_DATA					34
#define FN_SYS_HIT_TEST					35
#define FN_SYS_GET_POV					36
#define FN_SYS_ITEM_BUY_PRICE			37
#define FN_SYS_STARGATE_PROPERTY		38

ICCItem *fnSystemGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_SYS_STOP_TIME				1
#define FN_SYS_START_TIME				2

ICCItem *fnSystemMisc (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_NODE_LEVEL					1
#define FN_NODE_SYSTEM_NAME				2
#define FN_NODE_HAS_ATTRIBUTE			3

ICCItem *fnTopologyGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_DESIGN_GET_GLOBAL_DATA		1
#define FN_DESIGN_SET_GLOBAL_DATA		2
#define FN_DESIGN_GET_STATIC_DATA		3
#define FN_DESIGN_INC_GLOBAL_DATA		4
#define FN_DESIGN_GET_DATA_FIELD		5
#define FN_DESIGN_FIRE_OBJ_EVENT		6
#define FN_DESIGN_HAS_ATTRIBUTE			7
#define FN_DESIGN_CREATE				8
#define FN_DESIGN_DYNAMIC_UNID			9
#define FN_DESIGN_MARK_IMAGES			10
#define FN_DESIGN_TRANSLATE				11
#define FN_DESIGN_MATCHES				12
#define FN_DESIGN_FIRE_TYPE_EVENT		13
#define FN_DESIGN_HAS_EVENT				14
#define FN_DESIGN_GET_XML				15
#define FN_DESIGN_GET_PROPERTY			16
#define FN_DESIGN_ADD_TIMER             17
#define FN_DESIGN_ADD_RECURRING_TIMER   18
#define FN_DESIGN_CANCEL_TIMER          19

ICCItem *fnDesignCreate (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnDesignGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnDesignFind (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_UNIVERSE_TICK				0
#define FN_UNIVERSE_UNID				1
#define FN_UNIVERSE_REAL_DATE			2
#define FN_UNIVERSE_EXTENSION_UNID		3
#define FN_UNIVERSE_GET_EXTENSION_DATA	4
#define FN_UNIVERSE_SET_EXTENSION_DATA	5
#define FN_UNIVERSE_FIND_OBJ			6
#define FN_UNIVERSE_GET_ELAPSED_GAME_TIME	7
#define FN_UNIVERSE_SET_OBJECT_KNOWN	8
#define FN_UNIVERSE_ENTITY				9

ICCItem *fnUniverseGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_RESOURCE_CREATE_IMAGE_DESC	1
#define FN_RESOURCE_COLOR_BLEND			2
#define FN_RESOURCE_GET_IMAGE_PROPERTY	3

ICCItem *fnResourceGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_SYS_ORBIT_POS				0

ICCItem *fnSystemOrbit (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_VECTOR_ADD					0
#define FN_VECTOR_SUBTRACT				1
#define FN_VECTOR_MULTIPLY				2
#define FN_VECTOR_DIVIDE				3
#define FN_VECTOR_DISTANCE				4
#define FN_VECTOR_RANDOM				5
#define FN_VECTOR_ANGLE					6
#define FN_VECTOR_SPEED					7
#define FN_VECTOR_POLAR_VELOCITY		8
#define FN_VECTOR_PIXEL_OFFSET			9
#define FN_VECTOR_DISTANCE_EXACT		10

ICCItem *fnSystemVectorMath (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_SOVEREIGN_DISPOSITION		0
#define FN_SOVEREIGN_GET_DISPOSITION	1
#define FN_SOVEREIGN_MESSAGE			2
#define FN_SOVEREIGN_MESSAGE_FROM_OBJ	3
#define FN_SOVEREIGN_NAME				4

#define DISP_NEUTRAL					CONSTLIT("neutral")
#define DISP_ENEMY						CONSTLIT("enemy")
#define DISP_FRIEND						CONSTLIT("friend")

ICCItem *fnSovereignGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnSovereignSet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FN_XML_GET_TAG					0
#define FN_XML_GET_ATTRIB				1
#define FN_XML_SET_ATTRIB				2

#define FN_XML_GET_ATTRIB_LIST			3
#define FN_XML_GET_SUB_ELEMENT			4
#define FN_XML_GET_SUB_ELEMENT_COUNT	5
#define FN_XML_GET_SUB_ELEMENT_LIST		6
#define FN_XML_GET_TEXT					7
#define FN_XML_APPEND_SUB_ELEMENT		8
#define FN_XML_DELETE_SUB_ELEMENT		9
#define FN_XML_APPEND_TEXT				10
#define FN_XML_SET_TEXT					11

ICCItem *fnXMLCreate (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnXMLGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

#define FIELD_ANGLE_OFFSET				CONSTLIT("angleOffset")
#define FIELD_ARC_OFFSET				CONSTLIT("arcOffset")
#define FIELD_ATTRIBS					CONSTLIT("attribs")
#define FIELD_CENTER					CONSTLIT("center")
#define FIELD_DEVICE_SLOT				CONSTLIT("deviceSlot")
#define FIELD_ERODE						CONSTLIT("erode")
#define FIELD_EXCLUDE_WORLDS			CONSTLIT("excludeWorlds")
#define FIELD_HEIGHT					CONSTLIT("height")
#define FIELD_LENGTH					CONSTLIT("length")
#define FIELD_OBJ_TYPE					CONSTLIT("objType")
#define FIELD_ORBIT						CONSTLIT("orbit")
#define FIELD_POS						CONSTLIT("pos")
#define FIELD_RADIUS_OFFSET				CONSTLIT("radiusOffset")
#define FIELD_REMOVE					CONSTLIT("remove")
#define FIELD_ROTATION					CONSTLIT("rotation")
#define FIELD_SLOT_POS_INDEX			CONSTLIT("slotPosIndex")
#define FIELD_SOURCE_ONLY				CONSTLIT("sourceOnly")
#define FIELD_TYPE						CONSTLIT("type")
#define FIELD_WIDTH						CONSTLIT("width")
#define FIELD_WIDTH_VARIATION			CONSTLIT("widthVariation")

#define SHAPE_ARC						CONSTLIT("arc")
#define SHAPE_ORBITAL					CONSTLIT("orbital")
#define SHAPE_SQUARE					CONSTLIT("square")

#define TYPE_SCHEMATIC					CONSTLIT("schematic")

#define UNID_TYPE_ITEM_TYPE				CONSTLIT("itemtype")
#define UNID_TYPE_SHIP_CLASS			CONSTLIT("shipclass")

#define ERR_NO_CODE_CHAIN_CTX			CONSTLIT("No CodeChainCtx")

static PRIMITIVEPROCDEF g_Extensions[] =
	{
		//	ArmorClass functions
		//	--------------------

		{	"armGetRepairCost",				fnArmGet,		FN_ARM_REPAIRCOST,
			"(armGetRepairCost type) -> Cost to repair 1 hit point",
			"v",	0,	},

		{	"armGetRepairTech",				fnArmGet,		FN_ARM_REPAIRTECH,
			"(armGetRepairTech type) -> Tech level required to repair",
			"v",	0,	},

		//	Debug functions
		//	---------------

		{	"dbgGet",					fnDebug,		FN_DEBUG_GET,
			"(dbgGet property) -> value\n\n"
			
			"property:\n\n"
			
			"   'debugMode\n"
			"   'showBounds\n"
			"   'showFacingsAngle\n"
			"   'showLineOfFire\n"
			"   'showNavPaths\n",

			"s",	0, },

		{	"dbgIsActive",				fnDebug,		FN_DEBUG_IS_ACTIVE,
			"(dbgIsActive) -> True if in debug mode, else Nil",
			NULL,	0, },

		{	"dbgLog",						fnDebug,		FN_DEBUG_LOG,
			"(dbgLog [string]*) -> True if in debug mode, else Nil",
			"*",	PPFLAG_SIDEEFFECTS,	},

		{	"dbgOutput",					fnDebug,		FN_DEBUG_OUTPUT,
			"(dbgOutput [string]*) -> True if in debug mode, else Nil",
			"*",	PPFLAG_SIDEEFFECTS,	},

		{	"dbgSet",					fnDebug,		FN_DEBUG_SET,
			"(dbgSet property value) -> True/Nil\n\n"
			
			"property:\n\n"
			
			"   'showBounds True/Nil\n"
			"   'showFacingsAngle True/Nil\n"
			"   'showLineOfFire True/Nil\n"
			"   'showNavPaths True/Nil\n",

			"sv",	PPFLAG_SIDEEFFECTS, },

		{	"getAPIVersion",				fnDebug,		FN_API_VERSION,
			"(getAPIVersion) -> version",
			NULL,	0,	},

		{	"print",						fnDebug,		FN_PRINT,
			"(print [string]*) -> True",
			"*",	PPFLAG_SIDEEFFECTS,	},

		{	"printTo",						fnDebug,		FN_PRINT_TO,
			"(printTo output [string]*) -> True\n\n"
			
			"output is one or more of:\n\n"

			"   'console\n"
			"   'log",

			"v*",	PPFLAG_SIDEEFFECTS,	},

		//	Item functions
		//	--------------

		{	"itmCreate",					fnItemCreate,	0,	
			"(itmCreate itemUNID count) -> item",
			"ii",	PPFLAG_SIDEEFFECTS,	},

		{	"itmCreateByName",				fnItemCreateByName,	0,	
			"(itmCreateByName criteria name [count]) -> item\n\n"

			"criteria as itmGetTypes",

			"ss*",	PPFLAG_SIDEEFFECTS,	},

		{	"itmCreateRandom",				fnItemCreateRandom,	0,
			"(itmCreateRandom criteria levelDistribution) -> item\n\n"

			"criteria as itmGetTypes",

			"ss",	PPFLAG_SIDEEFFECTS,	},

		{	"itmEnumTypes",					fnItemEnumTypes,	0,
			"(itmEnumTypes criteria item-var exp) -> value of last expression\n\n"

			"criteria as itmGetTypes",

			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"itmFireEvent",				fnItemSet,		FN_ITEM_FIRE_EVENT,
			"(itmFireEvent item|type event [data]) -> result of event",
			"vs*",	PPFLAG_SIDEEFFECTS,	},

		{	"itmGetActualPrice",			fnItemGet,		FN_ITEM_ACTUAL_PRICE,
			"(itmGetActualPrice item|type) -> actual price of a single item",
			"v",	0,	},

		{	"itmGetArmorInstalledLocation",	fnItemGet,		FN_ITEM_ARMOR_INSTALLED_LOCATION,
			"(itmGetArmorInstalledLocation item) -> segment #",
			"v",	0,	},

		{	"itmGetArmorType",				fnItemGet,		FN_ITEM_ARMOR_TYPE,
			"(itmGetArmorType item) -> type",
			"v",	0,	},

		{	"itmGetAverageAppearing",			fnItemGet,		FN_ITEM_AVERAGE_APPEARING,
			"(itmGetAverageAppearing item|type) -> average number that appear randomly",
			"v",	0,	},

		{	"itmGetCategory",				fnItemGet,		FN_ITEM_CATEGORY,
			"(itmGetCategory item|type) -> item category",
			"v",	0,	},

		{	"itmGetCount",					fnItemGet,		FN_ITEM_COUNT,
			"(itmGetCount item) -> count of items",
			"v",	0,	},

		{	"itmGetDamageType",				fnItemGet,		FN_ITEM_DAMAGE_TYPE,
			"(itmGetDamageType item|type) -> damage type",
			"v",	0,	},

		{	"itmGetData",					fnItemGet,		FN_ITEM_DATA,
			"(itmGetData item attrib) -> data",
			"vs",	0,	},

		{	"itmGetFrequency",				fnItemTypeGet,		FN_ITEM_FREQUENCY,
			"(itmGetFrequency item|type [level]) -> frequency",
			"v*",	0,	},

		{	"itmGetImageDesc",				fnItemGet,		FN_ITEM_IMAGE_DESC,
			"(itmGetImageDesc item|type) -> imageDesc",
			"v",	0,	},

		{	"itmGetInstallCost",			fnItemGet,		FN_ITEM_INSTALL_COST,
			"(itmGetInstallCost item|type [currency]) -> cost",
			"v*",	0,	},

		{	"itmGetInstallPos",				fnItemGet,		FN_ITEM_INSTALL_POS,
			"(itmGetInstallPos item) -> installPos",
			"v",	0,	},

		{	"itmGetLevel",					fnItemGet,		FN_ITEM_LEVEL,
			"(itmGetLevel item|type) -> level",
			"v",	0,	},

		{	"itmGetMass",					fnItemGet,		FN_ITEM_MASS,
			"(itmGetMass item|type) -> mass of single item in kg",
			"v",	0,	},

		{	"itmGetMaxAppearing",			fnItemGet,		FN_ITEM_MAX_APPEARING,
			"(itmGetMaxAppearing item|type) -> max number that appear randomly",
			"v",	0,	},

		{	"itmGetName",					fnItemGet,		FN_ITEM_NAME,
			"(itmGetName item|type [flags]) -> name of item\n\n"

			"flags\n\n"

			"   0x0001 'capitalize      capitalize first letter\n"
			"   0x0002 'plural          pluralize name\n"
			"   0x0004 'article         prefix with 'the' or 'a'\n"
			"   0x0008 'count           prefix with count or singular article\n"
			"   0x0010 'countOnly       prefix with count or nothing\n"
			"   0x0020 'noModifiers     no modifiers ('damaged' etc)\n"
			"   0x0040 'demonstrative   prefix with 'the' or 'this' or 'these'\n"
			"   0x0080 'short           use short name\n"
			"   0x0100 'actual          actual name (not unidentified name)\n"
			"   0x0200 'noEvent         do not fire GetName event\n"
			"   0x0400 'titleCapitalize capitalize as a title\n"
			"   0x0800 'installedState  prefix with 'installed' if necessary\n"
			"   0x1000 'countAlways     always prefix with count\n"
			"  0x40000 'noDeterminer    no prefix, but pluralize if necessary\n"
			"  0x80000 'noQuotes        replace double-quotes with single-quotes\n"
			" 0x100000 'escapeQuotes    use for dock screens",

			"v*",	0,	},

		{	"itmGetPrice",					fnItemGet,		FN_ITEM_PRICE,
			"(itmGetPrice item|type [currency]) -> price of a single item",
			"v*",	0,	},

		{	"itmGetProperty",				fnItemGet,		FN_ITEM_PROPERTY,
			"(itmGetProperty item|type property) -> value\n\n"
			
			"property (instance)\n\n"
			"   'canBeUsed\n"
			"   'charges\n"
			"   'damaged\n"
			"   'description\n"
			"   'disrupted\n"
			"   'hasUseScreen\n"
			"   'installed\n"
			"   'level\n"
			"   'reference\n"
			"   'rootName\n"
			"   'used\n"
			"\n"
			"property (device)\n\n"
			"   'canBeDamaged\n"
			"   'canBeDisabled\n"
			"   'canBeDisrupted\n"
			"   'external\n"
			"   'power\n"
			"\n"
			"property (weapon)\n\n"
			"   'ammoTypes\n"
			"   'averageDamage       Average damage per shot\n"
			"   'balance\n"
			"   'balanceDamage\n"
			"   'balanceCost\n"
			"   'balanceExcludeCost\n"
			"   'damage              Average damage per 180 ticks\n"
			"   'damagePerProjectile\n"
			"   'damageWMD180        Average WMD damage per 180 ticks\n"
			"   'effectiveRange\n"
			"   'fireArc\n"
			"   'fireDelay           Number of ticks between shots\n"
			"   'fireRate\n"
			"   'linkedFireOptions\n"
			"   'maxDamage           Maximum damage per shot\n"
			"   'minDamage           Minimum damage per shot\n"
			"   'multiShot\n"
			"   'omnidirectional\n"
			"   'repeating\n"
			"	'shipCounterPerShot\n"
			"   'stdCost\n"
			"\n"
			"property (armor)\n\n"
			"   'blindingImmune\n"
			"   'completeHP\n"
			"   'damageAdj\n"
			"   'deviceDamageImmune\n"
			"   'deviceDisruptImmune\n"
			"   'disintegrationImmune\n"
			"   'EMPImmune\n"
			"   'hp\n"
			"   'hpBonus\n"
			"   'maxHP\n"
			"   'radiationImmune\n"
			"   'repairCost\n"
			"   'repairLevel\n"
			"   'shatterImmune\n"
			"   'stdHP\n"
			"\n"
			"property (all)\n\n"
			"   'category\n"
			"   'componentPrice\n"
			"   'components\n"
			"   'currency\n"
			"   'currencyName\n"
			"   'description\n"
			"   'frequency\n"
			"   'known\n"
			"   'level\n"
			"   'maxCharges\n"
			"   'maxLevel\n"
			"   'minLevel\n"
			"   'massBonusPerCharge\n"
			"   'valueBonusPerCharge\n"
			"   'weaponTypes\n",

			"vs",	0,	},

		{	"itmGetStaticData",				fnItemGet,		FN_ITEM_GET_STATIC_DATA,
			"(itmGetStaticData item attrib) -> data",
			"vs",	0,	},

		{	"itmGetType",					fnItemGet,		FN_ITEM_UNID,
			"(itmGetType item) -> itemUNID",
			"v",	0,	},

		{	"itmGetTypeData",				fnItemGet,		FN_ITEM_GET_TYPE_DATA,
			"(itmGetTypeData item|type attrib) -> data",
			"v*",	0,	},

		{	"itmGetTypes",				fnItemGetTypes,			0,
			"(itmGetTypes criteria) -> list of itemUNIDs\n\n"

			"criteria\n\n"

			"   *                  Include all item categories\n"
			"   a                  Include armor devices\n"
			"   b                  Include misc devices\n"
			"   c                  Include cargo hold devices\n"
			"   d                  Include all devices\n"
			"   f                  Include fuel\n"
			"   l                  Include launcher devices\n"
			"   m/M                Include missiles and ammo / missiles only\n"
			"   p                  Include primary weapon devices\n"
			"   r                  Include reactor devices\n"
			"   s                  Include shield devices\n"
			"   t                  Include misc items\n"
			"   u                  Include useful items\n"
			"   v                  Include drive devices\n"
			"   w                  Include all weapon devices\n"
			"   V                  Include virtual items\n"
			"   ~                  Exclude category\n"
			"   ^                  Require category\n"
			"\n"
			"   +/-xyz             Require / exclude items with attribute\n"
			"   +/-UNID:xyz        Require / exclude items with unid\n"
			"   +/-launchedBy:xyz  Require / exclude ammo launched by unid\n"
			"   +/-damageType:xyz  Require / exclude weapons with damage\n"
			"\n"
			"   F:xyz              Only items with the given frequency\n"
			"   L:x-y              Only Items of level x to y\n"
			"comparison criteria supported: < <= = => >\n"
			"   < x                Only items with level less than x\n"
			"   <$ x               Only items costing less than x\n"
			"   <# x               Only items massing less than x",

			"s",	0,	},

		{	"itmGetUseScreen",				fnItemGet,		FN_ITEM_USE_SCREEN,
			"(itmGetUseScreen item|type) -> screenUNID",
			"v",	0,	},

		{	"itmHasAttribute",				fnItemGet,		FN_ITEM_HAS_MODIFIER,
			"(itmHasAttribute item|type attrib) -> True/Nil",
			"vs",	0,	},

		{	"itmHasReference",				fnItemGet,		FN_ITEM_REFERENCE,
			"(itmHasReference item|type) -> True/Nil",
			"v",	0,	},

		{	"itmIsEnhanced",				fnItemGet,		FN_ITEM_ENHANCED,
			"(itmIsEnhanced item) -> Nil or mods",
			"v",	0,	},

		{	"itmIsEqual",					fnItemGet,		FN_ITEM_IS_EQUAL,
			"(itmIsEqual item1 item2 [options]) -> True/Nil\n\n"
			
			"options\n\n"
			
			"   'ignoreInstalled",

			"vv*",	0,	},

		{	"itmIsInstalled",				fnItemGet,		FN_ITEM_INSTALLED,
			"(itmIsInstalled item) -> True/Nil",
			"v",	0,	},

		{	"itmIsKnown",					fnItemGet,		FN_ITEM_KNOWN,
			"(itmIsKnown item|type) -> True/Nil",
			"v",	0,	},

		{	"itmMatches",					fnItemGet,		FN_ITEM_MATCHES,
			"(itmMatches item|type criteria) -> True/Nil\n\n"

			"criteria as itmGetTypes",

			"vs",	0,	},

		{	"itmSetCount",					fnItemSet,		FN_ITEM_COUNT,
			"(itmSetCount item count) -> item",
			"vi",	0,	},

		{	"itmSetData",					fnItemSet,		FN_ITEM_DATA,
			"(itmSetData item attrib data [count]) -> item",
			"vsv*",	0,	},

		{	"itmSetEnhanced",				fnItemSet,		FN_ITEM_ENHANCED,
			"(itmSetEnhanced item mods) -> item",
			"vv",	0,	},

		{	"itmSetKnown",					fnItemTypeSet,	FN_ITEM_TYPE_SET_KNOWN,
			"(itmSetKnown type|item [True/Nil]) -> True/Nil",
			"v*",	PPFLAG_SIDEEFFECTS,	},

		{	"itmSetProperty",				fnItemSet,		FN_ITEM_PROPERTY,
			"(itmSetProperty item property value) -> item\n\n"
			
			"property\n\n"
			
			"   'charges charges\n"
			"   'damaged [True|Nil]\n"
			"   'disrupted [True|Nil|ticks]\n"
			"   'incCharges charges\n"
			"   'installed [True|Nil]"
			"   'level level",

			"vs*",	0,	},

		{	"itmSetReference",				fnItemGet,		FN_ITEM_SET_REFERENCE,
			"(itmSetReference item) -> True/Nil",
			"v",	PPFLAG_SIDEEFFECTS,	},

		{	"itmSetTypeData",				fnItemGet,		FN_ITEM_SET_GLOBAL_DATA,
			"(itmSetTypeData item attrib data) -> True/Nil",
			"vsv",	0,	},

		//	Miscellaneous functions
		//	-----------------------

		{	"fmtCompose",					fnFormat,		FN_COMPOSE,
			"(fmtCompose text [data]) -> string",
			"s*",	0, },

		{	"fmtCurrency",					fnFormat,		FN_CURRENCY,
			"(fmtCurrency currency [amount]) -> string",
			"v*",	0, },

		{	"fmtNoun",						fnFormat,		FN_NAME,
			"(fmtNoun namePattern [nameFlags] count formatFlags) -> string",
			"v*iv",	0, },

		{	"fmtNumber",					fnFormat,		FN_NUMBER,
			"(fmtNumber [type] value) -> string\n\n"
			
			"type:\n\n"
			
			"   'integer\n"
			"   'power\n"
			"   'real\n"
			"   'regenRate\n"
			"   'speed",

			"v*",	0, },

		{	"fmtPower",						fnFormat,		FN_POWER,
			"(fmtPower powerInKWs) -> string",
			"v",	0, },

		{	"fmtVerb",						fnFormat,		FN_VERB,
			"(fmtVerb verb pluralize) -> string",
			"sv",	0, },

		{	"rollChance",					fnRollDice,		FN_ROLL_CHANCE,
			"(rollChance percentChance [rolls]) -> True/Nil",
			"n*",	0,	},

		{	"rollDice",						fnRollDice,		FN_ROLL_DICE,
			"(rollDice count sides [bonus]) -> value",
			"ii*",	0,	},

		//	Ship functions
		//	--------------

		{	"shpCancelOrder",				fnShipSet,			FN_SHIP_CANCEL_ORDER,
			"(shpCancelOrder ship [orderIndex]) -> True/Nil",
			"i*",	PPFLAG_SIDEEFFECTS,	},

		{	"shpCancelOrders",				fnShipGetOld,		FN_SHIP_CANCEL_ORDERS,
			"(shpCancelOrders ship) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpCanRemoveDevice",			fnShipSet,			FN_SHIP_CAN_REMOVE_DEVICE,
			"(shpCanRemoveDevice ship item) -> resultCode\n\n"

			"resultCode\n\n"

			"   0        OK\n"
			"   1        Too much cargo to remove cargo hold\n"
			"   2        Device not installed\n"
			"   3        Replace only\n"
			"   string   custom fail reason",
			"iv",	0,	},

		{	"shpConsumeFuel",				fnShipSet,			FN_SHIP_CONSUME_FUEL,
			"(shpConsumeFuel ship fuel [useType]) -> fuelLeft\n\n"
				
			"useType:\n\n"
			
			"   'consume\n"
			"   'drain",

			"iv*",	PPFLAG_SIDEEFFECTS,	},

		{	"shpDamageArmor",				fnShipSet,			FN_SHIP_DAMAGE_ARMOR,
			"(shpDamageArmor ship armorSegment damageType damage ['noSRSFlash]) -> damage done",
			"ivvi*",	PPFLAG_SIDEEFFECTS,	},

		{	"shpDecontaminate",				fnShipGetOld,		FN_SHIP_DECONTAMINATE,
			"(shpDecontaminate ship) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpGetAISetting",				fnShipGet,			FN_SHIP_AI_SETTING,
			"(shpGetAISetting ship setting)",
			"is",	0,	},

		{	"shpGetArmor",					fnObjGetArmor,	FN_OBJ_ARMOR_ITEM,
			"(shpGetArmor ship armorSegment) -> item struct",
			NULL,	0,	},

		{	"shpGetArmorCount",				fnShipGetOld,		FN_SHIP_ARMOR_COUNT,
			"(shpGetArmorCount ship) -> number of armor segments",
			NULL,	0,	},

		{	"objGetArmorCriticality",		fnObjGet,		FN_OBJ_ARMOR_CRITICALITY,
			"(objGetArmorCriticality obj item|armorSegment) -> criticalityType",
			"iv",	0,	},

		{	"objGetArmorDamage",			fnObjGetArmor,	FN_OBJ_ARMOR_DAMAGE,
			"(objGetArmorDamage obj item|armorSegment) -> damage to armor segment",
			NULL,	0,	},

		{	"objGetArmorLevel",				fnObjGet,		FN_OBJ_ARMOR_LEVEL,
			"(objGetArmorLevel obj item|armorSegment) -> 0-100%",
			"iv",		0,	},

		{	"shpGetArmorMaxHitPoints",		fnObjGetArmor,	FN_OBJ_ARMOR_MAX_HP,
			"(shpGetArmorMaxHitPoints obj item|armorSegment) -> damage to armor segment",
			NULL,	0,	},

		{	"objGetArmorName",				fnObjGetArmor,	FN_OBJ_ARMOR_NAME,
			"(objGetArmorName obj item|armorSegment) -> name of armor (e.g., 'forward', etc.)",
			NULL,	0,	},

		{	"objGetArmorType",				fnObjGet,		FN_OBJ_ARMOR_TYPE,
			"(objGetArmorType obj item|armorSegment) -> type",
			"iv",	0,	},

		{	"shpGetClassName",				fnShipClass,		FN_SHIP_CLASS_NAME,
			"(shpGetClassName class flags) -> class name",
			"ii",	0,	},

		{	"shpGetDirection",				fnShipGetOld,		FN_SHIP_DIRECTION,
			"(shpGetDirection ship) -> angle",
			NULL,	0,	},

		{	"shpGetDockObj",				fnShipGet,		FN_SHIP_DOCK_OBJ,
			"(shpGetDockObj ship) -> dockObj",
			"i",	0,	},

		{	"shpGetFuelNeeded",				fnShipSetOld,		FN_SHIP_FUEL_NEEDED,
			"(shpGetFuelNeeded ship item) -> items needed",
			NULL,	0,	},

		{	"shpGetFuelLeft",				fnShipGetOld,		FN_SHIP_FUEL,
			"(shpGetFuelLeft ship) -> fuel left",
			NULL,	0,	},

		{	"shpGetImageDesc",				fnShipClass,	FN_SHIP_GET_IMAGE_DESC,
			"(shpGetImageDesc class [options|rotationAngle]) -> imageDesc",
			"i*",	0,	},

		{	"shpGetItemDeviceName",			fnShipSetOld,		FN_SHIP_ITEM_DEVICE_NAME,
			"(shpGetItemDeviceName ship item) -> device name of item (or -1)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objGetMaxPower",				fnObjGetOld,		FN_OBJ_MAX_POWER,
			"(objGetMaxPower obj) -> power (in 1/10 MWs)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpGetMaxSpeed",				fnShipGet,		FN_SHIP_MAX_SPEED,
			"(shpGetMaxSpeed ship) -> max speed in % of lightspeed",
			"i",	0,	},

		{	"shpGetOrder",					fnShipGetOld,		FN_SHIP_ORDER,
			"(shpGetOrder obj) -> order",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpGetOrderCount",				fnShipGet,		FN_SHIP_ORDER_COUNT,
			"(shpGetOrderCount obj) -> count",
			"i",	0,	},

		{	"shpGetOrderDesc",				fnShipGet,			FN_SHIP_ORDER_DESC,
			"(shpGetOrderDesc obj [orderIndex]) -> orderDesc",
			"i*",	0,	},

		{	"shpGetOrderTarget",			fnShipGetOld,		FN_SHIP_ORDER_TARGET,
			"(shpGetOrderTarget obj) -> obj",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objGetOrderGiver",				fnObjGet,			FN_OBJ_ORDER_GIVER,
			"(objGetOrderGiver obj [destroyReason]) -> obj",
			"i*",	0,	},

		{	"shpGetShieldItemUNID",			fnShipGetOld,		FN_SHIP_SHIELD_ITEM_UNID,
			"(shpGetShieldItemUNID ship) -> UNID (or Nil)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpGetShieldDamage",			fnShipGetOld,		FN_SHIP_SHIELD_DAMAGE,
			"(shpGetShieldDamage ship) -> damage to shields",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpGetShieldMaxHitPoints",		fnShipGetOld,		FN_SHIP_SHIELD_MAX_HP,
			"(shpGetShieldMaxHitPoints ship) -> max hp of shields",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objGetStargateID",				fnObjGet,			FN_OBJ_GET_STARGATE_ID,
			"(objGetStargateID obj) -> gateID",
			"i",	0,	},

		{	"shpInstallArmor",				fnShipSet,			FN_SHIP_INSTALL_ARMOR,
			"(shpInstallArmor ship item armorSegment)",
			"ivi",	PPFLAG_SIDEEFFECTS,	},

		{	"shpInstallDevice",				fnShipSet,			FN_SHIP_INSTALL_DEVICE,
			"(shpInstallDevice ship item [deviceSlot])",
			"iv*",	PPFLAG_SIDEEFFECTS,	},

		{	"shpIsBlind",					fnShipGetOld,		FN_SHIP_BLINDNESS,
			"(shpIsBlind ship) -> True/Nil",
			NULL,	0,	},

		{	"shpIsFuelCompatible",			fnShipSetOld,		FN_SHIP_IS_FUEL_COMPATIBLE,
			"(shpIsFuelCompatible ship item) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS, },

		{	"shpIsRadiationImmune",			fnShipGet,		FN_SHIP_IS_RADIATION_IMMUNE,
			"(shpIsRadiationImmune ship [item])",
			"i*",	0,	},

		{	"objIsDeviceSlotAvailable",		fnShipGetOld,		FN_SHIP_DEVICE_SLOT_AVAIL,
			"(objIsDeviceSlotAvailable ship) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objIsShip",					fnObjGetOld,		FN_OBJ_IS_SHIP,
			"(objIsShip obj) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpEnhanceItem",				fnShipSetOld,		FN_SHIP_ENHANCE_ITEM,
			"(shpEnhanceItem ship item [mods]) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpMakeRadioactive",			fnShipGetOld,		FN_SHIP_MAKE_RADIOACTIVE,
			"(shpMakeRadioactive ship) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpOrder",						fnShipSet,		FN_SHIP_ORDER,
			"(shpOrder ship order [target] [count]) -> True/Nil\n\n"

			"order:\n\n"
			"   'aim            obj               Aim, but do not fire\n"
			"   'approach       obj [dist]        Approach target\n"
			"   'attack         obj [time]        Attack target\n"
			"   'attackArea     obj dist [time]   Attack around target\n"
			"   'attackHold     obj [time]\n"
			"   'attackNearestEnemy               Attack nearest enemy\n"
			"   'attackPlayerOnReturn             Wait and attack player\n"
			"   'attackStation  obj [?]           Attack target station\n"
			"   'bombard        obj [time]        Hold and attack target\n"
			"   'dock           obj               Dock with target\n"
			"   'escort         obj [ang] [dist]  Escort target\n"
			"   'fireEvent      obj event         Fire event on target\n"
			"   'follow         obj               Follow without defending\n"
			"   'followPlayerThroughGate\n"
			"   'gate           [obj]             Gate out of system\n"
			"   'gateOnStationDestroyed           Flee if station destroyed\n"
			"   'gateOnThreat                     Flee if threatened\n"
			"   'goto           obj [dist]        Goto target\n"
			"   'gotoPos        pos               Goto position\n"
			"   'guard          obj               Guard target\n"
			"   'hold           [time]            Stay in place\n"
			"   'holdAndAttack  obj [time]\n"
			"   'holdCourse     course dist       Hold course\n"
			"   'loot           obj               Loot target\n"
			"   'mine           obj               Mine asteroids [base]\n"
			"   'navPath        navID             Follow nav path ID\n"
			"   'orbit          obj dist [time]   Orbit target\n"
			"   'patrol         obj [dist]        Patrol around target\n"
			"   'scavenge                         Scavenge for scraps\n"
			"   'sendMessage    obj msg           Send message to target\n"
			"   'sentry         obj [time]        Guard target (turrets)\n"
			"   'tradeRoute                       Move between stations\n"
			"   'turnTo         ang               Turn to face angle\n"
			"   'wait           [time]            Wait, do not attack\n"
			"   'waitForEnemy   [time]            Wait until enemy in LRS\n"
			"   'waitForPlayer                    Wait for player to return\n"
			"   'waitForTarget  obj [dist] [time] Wait until target in range\n"
			"   'waitForThreat  [time]\n"
			"   'waitForUndock  obj [time]        Wait for target to undock\n"
			"   'wander                           Wander, avoiding enemies",

			"is*",	PPFLAG_SIDEEFFECTS,	},

		{	"shpOrderImmediate",			fnShipSet,		FN_SHIP_ORDER_IMMEDIATE,
			"(shpOrderImmediate ship order [target] [count]) -> True/Nil",
			"is*",	PPFLAG_SIDEEFFECTS,	},

		{	"shpRechargeShield",			fnShipSet,		FN_SHIP_RECHARGE_SHIELD,
			"(shpRechargeShield ship [hpToRecharge]) -> shield hp",
			"i*",	PPFLAG_SIDEEFFECTS,	},

		{	"shpRefuelFromItem",			fnShipSetOld,		FN_SHIP_REFUEL_FROM_ITEM,
			"(shpRefuelFromItem ship item) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpRemoveDevice",				fnShipSetOld,		FN_SHIP_REMOVE_DEVICE,
			"(shpRemoveDevice ship item) -> item",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objRepairArmor",				fnObjGetArmor,	FN_OBJ_REPAIR_ARMOR,
			"(objRepairArmor ship item|armorSegment [hpToRepair]) -> hp repaired",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpRepairItem",				fnShipSet,		FN_SHIP_REPAIR_ITEM,
			"(shpRepairItem ship item) -> True/Nil",
			"iv",	PPFLAG_SIDEEFFECTS,	},

		{	"shpSetAISetting",				fnShipSet,			FN_SHIP_AI_SETTING,
			"(shpSetAISetting ship setting value)\n\n"
			
			"setting:\n\n"
			
			"   'combatStyle\n"
			"      = 'advanced\n"
			"      = 'chase\n"
			"      = 'flyby\n"
			"      = 'noRetreat\n"
			"      = 'standard\n"
			"      = 'standOff\n"
			"\n"
			"   'aggressor (True/Nil)\n"
			"   'ascendOnGate (True/Nil)\n"
			"   'flockFormation (True/Nil)\n"
			"   'ignoreShieldsDown (True/Nil)\n"
			"   'noAttackOnThreat (True/Nil)\n"
			"   'noDogfights (True/Nil)\n"
			"   'noFriendlyFire (True/Nil)\n"
			"   'noFriendlyFireCheck (True/Nil)\n"
			"   'noNavPaths (True/Nil)\n"
			"   'noOrderGiver (True/Nil)\n"
			"   'noTargetsOfOpportunity (True/Nil)\n"
			"\n"
			"   'combatSeparation {pixels}\n"
			"   'fireAccuracy {percent}\n"
			"   'fireRangeAdj {percent}\n"
			"   'fireRateAdj {value; 10 = normal; 20 = twice as slow}\n"
			"   'perception {value}",

			"isv",	0,	},

		{	"shpSetCommandCode",			fnShipSet,		FN_SHIP_COMMAND_CODE,
			"(shpSetCommandCode ship code) -> True/Nil",
			"ic",	PPFLAG_SIDEEFFECTS,	},

		{	"shpSetController",				fnShipSetOld,		FN_SHIP_CONTROLLER,
			"(shpSetController ship controller) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		//	SpaceObject functions
		//	---------------------

		{	"objAccelerate",				fnObjSet,		FN_OBJ_ACCELERATE,
			"(objAccelerate obj angle thrust [ticks]) -> velVector",
			"iii*",		PPFLAG_SIDEEFFECTS,	},

		{	"objAddBuyOrder",					fnObjSet,		FN_OBJ_ADD_BUY_ORDER,
			"(objAddBuyOrder obj criteria priceAdj) -> True/Nil",
			"ivi",		PPFLAG_SIDEEFFECTS,	},

		{	"objAddConnection",				fnObjSet,		FN_OBJ_ADD_CONNECTION,
			"(objAddConnection obj1 connectType obj2 [options]) -> connectionID\n\n"

			"connectType:\n\n"

			"   'hinge\n"
			"   'rod\n"
			"   'spine\n\n"
			
			"options:\n\n"
			
			"   'pos1: position relative to obj1\n"
			"   'pos2: position relative to obj2\n",

			"isi*",		PPFLAG_SIDEEFFECTS,	},

		{	"objAddItem",					fnObjSet,		FN_OBJ_ADD_ITEM,
			"(objAddItem obj item|type [count]) -> item",
			"iv*",		PPFLAG_SIDEEFFECTS,	},

		{	"objAddItemEnhancement",		fnObjSet,		FN_OBJ_ADD_ITEM_ENHANCEMENT,
			"(objAddItemEnhancement obj item enhancementType [lifetime]) -> enhancementID",
			"ivi*",	PPFLAG_SIDEEFFECTS,	},

		{	"objAddOverlay",				fnObjSet,		FN_OBJ_ADD_OVERLAY,
			"(objAddOverlay obj overlayType [lifetime]) -> overlayID\n"
			"(objAddOverlay obj overlayType pos rotation [lifetime]) -> overlayID",
			"ii*",	PPFLAG_SIDEEFFECTS,	},

		{	"objAddRandomItems",			fnObjAddRandomItems,	0,
			"(objAddRandomItems obj table count) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objAddSellOrder",					fnObjSet,		FN_OBJ_ADD_SELL_ORDER,
			"(objAddSellOrder obj criteria priceAdj) -> True/Nil",
			"ivi",		PPFLAG_SIDEEFFECTS,	},

		{	"objAddSubordinate",			fnObjSetOld,		FN_OBJ_ADD_SUBORDINATE,
			"(objAddSubordinate obj subordinate) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objAddTradeOrder",					fnObjSet,		FN_OBJ_ADD_TRADE_ORDER,
			"(objAddTradeOrder obj service criteria priceAdj) -> True/Nil",
			"isvi",		PPFLAG_SIDEEFFECTS,	},

		{	"objCalcBestTarget",				fnObjGet,		FN_OBJ_CALC_BEST_TARGET,
			"(objCalcBestTarget obj [objList]) -> targetObj (or Nil)",
			"i*",	0,	},

		{	"objCanAttack",					fnObjGetOld,		FN_OBJ_CAN_ATTACK,
			"(objCanAttack obj) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objCanDetectTarget",				fnObjGet,			FN_OBJ_CAN_DETECT_TARGET,
			"(objCanDetectTarget obj target) -> True/Nil",
			"ii",	0,	},

		{	"objCanInstallItem",				fnObjGet,			FN_OBJ_CAN_INSTALL_ITEM,
			"(objCanInstallItem obj item [armorSeg|deviceSlot]) -> (True/Nil resultCode resultString [itemToReplace])\n\n"
			
			"resultCode\n\n"
			
			"   'ok\n"
			"   'armorTooHeavy\n"
			"   'cannotInstall\n"
			"   'noDeviceSlotsLeft\n"
			"   'noNonWeaponSlotsLeft\n"
			"   'noWeaponSlotsLeft\n"
			"   'notInstallable\n"
			"   'notCompatible\n"
			"   'reactorIncompatible\n"
			"   'reactorOverloaded\n"
			"   'reactorTooWeak\n"
			"   'replacementRequired\n"
			"   'tooMuchCargo",

			"iv*",	0,	},

		{	"objChangeEquipmentStatus",		fnObjSet,		FN_OBJ_SET_ABILITY,
			"(objChangeEquipmentStatus obj equipment command [duration] [options]) -> True/Nil\n\n"

			"equipment\n\n"

			"   'Autopilot\n"
			"   'GalacticMap\n"
			"   'FriendlyFireLock\n"
			"   'LRS\n"
			"   'MiningComputer\n"
			"   'ProtectWingmen\n"
			"   'SRS\n"
			"   'SRSEnhancer\n"
			"   'SystemMap\n"
			"   'TargetingComputer\n"
			"   'TradingComputer\n\n"
			
			"command\n\n"
			
			"   'damage\n"
			"   'install\n"
			"   'remove\n"
			"   'repair\n\n"
			
			"duration is in ticks\n\n"
			
			"options\n\n"
			
			"   'noMessage",

			"iss*",	PPFLAG_SIDEEFFECTS,	},

		{	"objCharge",					fnObjSet,		FN_OBJ_CHARGE,	
			"(objCharge obj [currency] amount) -> remaining balance",
			"iv*",		PPFLAG_SIDEEFFECTS,	},

		{	"objClearIdentified",			fnObjSet,		FN_OBJ_CLEAR_IDENTIFIED,
			"(objClearIdentified obj) -> True/Nil",
			"i",	PPFLAG_SIDEEFFECTS,	},

		{	"objClearShowAsDestination",		fnObjSet,		FN_OBJ_CLEAR_AS_DESTINATION,
			"(objClearShowAsDestination obj) -> True/Nil",
			"i",	PPFLAG_SIDEEFFECTS,	},

		{	"objCommunicate",				fnObjComms,		0,
			"(objCommunicate obj senderObj msg [obj] [data]) -> result\n\n"
			
			"msg\n\n"
			
			"   'AbortAttack\n"
			"   'AttackTarget\n"
			"   'FormUp\n"
			"   'QueryAttackStatus\n"
			"   'Wait",

			"iiv*",	PPFLAG_SIDEEFFECTS,	},

		{	"objCreateReflection",			fnObjSet,	FN_OBJ_CREATE_REFLECTION,
			"(objCreateReflection missile [pos] [angle]) -> reflection",
			"i**", PPFLAG_SIDEEFFECTS, },

		{	"objCredit",					fnObjSet,		FN_OBJ_CREDIT,	
			"(objCredit obj [currency] amount) -> new balance",
			"iv*",		PPFLAG_SIDEEFFECTS,	},

		{	"objDamage",					fnObjGet,		FN_OBJ_DAMAGE,	
			"(objDamage obj weaponType objSource [pos] [options]) -> result\n\n"
				
			"result:\n\n"

			"   'noDamage\n"
			"   'absorbedByShields\n"
			"   'armorHit\n"
			"   'structuralHit\n"
			"   'destroyed\n\n"

			"options:\n\n"

			"   'fullResult           Return result as struct\n"
			"   'ignoreOverlays       Hit shields and below\n"
			"   'ignoreShields        Hit armor and below\n"
			"   'noHitEffect          No hit effect created",

			"ivv*",		PPFLAG_SIDEEFFECTS,	},

		{	"objDepleteShields",			fnObjSet,		FN_OBJ_DEPLETE_SHIELDS,	
			"(objDepleteShields obj) -> True/Nil",
			"i",		PPFLAG_SIDEEFFECTS,	},

		{	"objDestroy",					fnObjSet,		FN_OBJ_DESTROY,
			"(objDestroy obj [objSource]) -> True/Nil",
			"i*",	PPFLAG_SIDEEFFECTS,	},

		{	"objEnumItems",					fnObjEnumItems,	0,
			"(objEnumItems obj criteria itemVar exp) -> value\n\n"

			"criteria as objGetItems",

			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objFireEvent",					fnObjSet,		FN_OBJ_FIRE_EVENT,
			"(objFireEvent obj event [data]) -> result of event",
			"is*",	PPFLAG_SIDEEFFECTS,	},

		{	"objFireItemEvent",				fnObjSet,		FN_OBJ_FIRE_ITEM_EVENT,
			"(objFireItemEvent obj item event [data]) -> result of event",
			"ivs*",	PPFLAG_SIDEEFFECTS,	},

		{	"objFireItemInvoke",				fnObjSet,		FN_OBJ_FIRE_ITEM_INVOKE,
			"(objFireItemInvoke obj item) -> True/Nil",
			"iv",	PPFLAG_SIDEEFFECTS,	},

		{	"objFireOverlayEvent",			fnObjSet,		FN_OBJ_FIRE_OVERLAY_EVENT,
			"(objFireOverlayEvent obj overlayID event [data]) -> result of event",
			"iis*",	PPFLAG_SIDEEFFECTS,	},

		{	"objFirePowerInvoke",			fnObjSet,		FN_OBJ_FIRE_POWER_INVOKE,
			"(objFirePowerInvoke obj power [target] ['noInvokeCheck]) -> result of event",
			"ii**",	PPFLAG_SIDEEFFECTS, },

		{	"objFireWeapon",				fnObjSet,		FN_OBJ_FIRE_WEAPON,
			"(objFireWeapon obj weapon target [fireDelay] [checkFireDelay]) -> True/Nil",
			"ivi*",	PPFLAG_SIDEEFFECTS, },

		{	"objFixParalysis",				fnObjSet,		FN_OBJ_FIX_PARALYSIS,
			"(objFixParalysis obj) -> True/Nil",
			"i",	PPFLAG_SIDEEFFECTS,	},

		{	"objGateTo",					fnObjGateTo,	0,
			"(objGateTo obj node entrypoint [effectID]) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objGetArmorRepairPrice",		fnObjGet,		FN_OBJ_ARMOR_REPAIR_PRICE,
			"(objGetArmorRepairPrice obj [shipObj] armorItem hpToRepair) -> price (at which obj repairs)",
			"i*vi",		0,	},

		{	"objGetArmorReplacePrice",		fnObjGet,		FN_OBJ_ARMOR_REPLACE_PRICE,
			"(objGetArmorReplacePrice obj armorItem) -> price",
			"iv",		0,	},

		{	"objGetBalance",				fnObjGet,		FN_OBJ_GET_BALANCE,	
			"(objGetBalance obj [currency]) -> balance",
			"i*",	0,	},

		{	"objGetBuyPrice",				fnObjGet,		FN_OBJ_GET_BUY_PRICE,	
			"(objGetBuyPrice obj item [options]) -> price (at which obj buys item)\n\n"
			
			"options:\n\n"
			
			"   'noDonations",

			"il*",	0,	},

		{	"objGetCargoSpaceLeft",			fnObjGetOld,		FN_OBJ_CARGO_SPACE_LEFT,
			"(objGetCargoSpaceLeft obj) -> space left in kg",
			NULL,	0,	},

		{	"objGetCharacterData",			fnObjGet,		FN_OBJ_GET_CHARACTER_DATA,
			"(objGetCharacterData obj attrib) -> data",
			"is",	0,	},

		{	"objGetCombatPower",			fnObjGetOld,		FN_OBJ_COMBAT_POWER,
			"(objGetCombatPower obj) -> 0-100",
			NULL,	0,	},

		{	"objGetCondition",				fnObjGet,		FN_OBJ_CONDITION,
			"(objGetCondition obj [condition]) -> True/Nil\n\n"
			
			"condition:\n\n"
			
			"   'blind\n"
			"   'disarmed\n"
			"   'lrsBlind\n"
			"   'paralyzed\n"
			"   'radioactive\n"
			"   'spinning\n"
			"   'timeStopped",

			"i*",	0,	},

		{	"objGetDamageType",				fnObjGetOld,		FN_OBJ_DAMAGE_TYPE,
			"(objGetDamageType obj) -> damage type",
			NULL,	0,	},

		{	"objGetData",					fnObjData,		FN_OBJ_GETDATA,
			"(objGetData obj attrib) -> data",
			NULL,	0,	},

		{	"objGetDataField",				fnObjGet,		FN_OBJ_DATA_FIELD,
			"(objGetDataField obj field) -> data",
			"is",	0,	},

		{	"objGetDetectRange",			fnObjGet,		FN_OBJ_GET_DETECT_RANGE,
			"(objGetDetectRange obj targetObj) -> range in light-seconds",
			"ii",	0,	},

		{	"objGetDisposition",			fnObjGet,		FN_OBJ_GET_DISPOSITION,
			"(objGetDisposition obj targetObj) -> disposition of obj towards targetObj\n\n"
			
			"disposition:\n\n"
			
			"   'enemy\n"
			"   'friend\n"
			"   'neutral",

			"ii",	0,	},

		{	"objGetDistance",				fnObjGetOld,		FN_OBJ_DISTANCE,
			"(objGetDistance obj destObj) -> distance in light-seconds",
			NULL,	0,	},

		{	"objGetEquipmentStatus",		fnObjGet,		FN_OBJ_GET_ABILITY,	
			"(objGetEquipmentStatus obj equipment) -> status\n\n"

			"equipment\n\n"

			"   'Autopilot\n"
			"   'GalacticMap\n"
			"   'FriendlyFireLock\n"
			"   'LRS\n"
			"   'MiningComputer\n"
			"   'ProtectWingmen\n"
			"   'SRS\n"
			"   'SRSEnhancer\n"
			"   'SystemMap\n"
			"   'TargetingComputer\n"
			"   'TradingComputer\n\n"
			
			"status\n\n"
			
			"   'damaged\n"
			"   'notInstalled\n"
			"   'ready",

			"is",	0,	},

		{	"objGetEventHandler",			fnObjGet,		FN_OBJ_GET_EVENT_HANDLER,
			"(objGetEventHandler obj) -> unid or Nil",
			"i",	0,	},

		{	"objGetID",						fnObjGet,		FN_OBJ_GET_ID,
			"(objGetID obj) -> objID",
			"i",	0,	},

		{	"objGetImageDesc",				fnObjGet,		FN_OBJ_IMAGE,
			"(objGetImageDesc obj) -> imageDesc",
			"i",	0,	},

		{	"objGetInstalledItemDesc",		fnObjGetOld,		FN_OBJ_INSTALLED_ITEM_DESC,
			"(objGetInstalledItemDesc obj item) -> 'installed as forward armor'",
			NULL,	0,	},

		{	"objGetItemProperty",			fnObjGet,		FN_OBJ_GET_ITEM_PROPERTY,
			"(objGetItemProperty obj item property) -> value\n\n"

			"property (install/remove)\n\n"

			"   'installDevicePrice   Price to install the given device\n"
			"   'installDeviceStatus  {canInstall, price, descID, upgradeOnly}\n"
			"   'installItemStatus    {canInstall, price, descID, upgradeOnly}\n"
			"   'removeDevicePrice    Price to remove the given device\n"
			"   'removeItemStatus     {canRemove, price, descID, upgradeOnly}\n"
			"\n"

			"property (device)\n\n"

			"   'capacitor\n"
			"   'enabled\n"
			"   'linkedFireOptions\n"
			"   'pos\n"
			"   'secondary\n"
			"   'temperature\n"
			"\n"

			"property (armor)\n\n"

			"   'completeSet     True if part of a complete set\n"
			"   'hp              Current hit points\n"
			"   'primeSegment    True if first segment in a set\n"
			"\n"
			
			"All properties for itmGetProperty are also valid.",

			"ivs",	0,	},

		{	"objGetItems",					fnObjItemOld,		FN_OBJ_ENUM_ITEMS,
			"(objGetItems obj criteria) -> list of items\n\n"

			"criteria as itmGetTypes plus\n\n"

			"   I                  Only installed devices\n"
			"   D                  Only damaged items\n"
			"   N                  Only undamaged items\n"
			"   S                  Only usable items\n"
			"   U                  Only uninstalled items",

			NULL,	0,	},

		{	"objGetLevel",					fnObjGet,		FN_OBJ_LEVEL,
			"(objGetLevel obj) -> level",
			"i",	0,	},

		{	"objGetMass",					fnObjGet,		FN_OBJ_MASS,	
			"(objGetMass obj) -> mass in tons",
			"i",		0,	},

		{	"objGetName",					fnObjGet,		FN_OBJ_NAME,
			"(objGetName obj [flags]) -> Name of the object\n\n"

			"flags\n\n"

			"   0x001 'capitalize    capitalize first letter\n"
			"   0x002 'plural        pluralize name\n"
			"   0x004 'article       prefix with 'the' or 'a'\n"
			"   0x008 'count         prefix with count or singular article\n"
			"   0x010 'countOnly     prefix with count or nothing\n"
			"   0x020 'noModifiers   no modifiers ('damaged' etc)\n"
			"   0x040 'demonstrative prefix with 'the' or 'this' or 'these'\n"
			"   0x080 'short         use short name\n"
			"   0x100 'actual        actual name (not unidentified name)",

			"i*",	PPFLAG_SIDEEFFECTS,	},

		{	"objGetNamedItems",				fnObjGet,		FN_OBJ_GET_NAMED_ITEM,	
			"(objGetNamedItems obj name) -> list of items",
			"is",		0,	},

		{	"objGetNearestStargate",		fnObjGetOld,		FN_OBJ_NEAREST_STARGATE,
			"(objGetNearestStargate obj) -> obj",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objGetObjByID",				fnObjIDGet,			FN_OBJ_OBJECT,
			"(objGetObjByID objID) -> obj",
			"i",	0,	},

		{	"objGetObjRefData",				fnObjData,		FN_OBJ_GET_OBJREF_DATA,
			"(objGetObjRefData obj attrib) -> obj",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objGetOpenDockingPortCount",	fnObjGet,		FN_OBJ_OPEN_DOCKING_PORT_COUNT,	
			"(objGetOpenDockingPortCount obj) -> count of open docking ports",
			"i",		0,	},

		{	"objGetOverlayData",			fnObjGet,		FN_OBJ_GET_OVERLAY_DATA,
			"(objGetOverlayData obj overlayID attrib) -> data",
			"iis",	0,	},

		{	"objGetOverlayPos",			fnObjGet,		FN_OBJ_GET_OVERLAY_POS,
			"(objGetOverlayPos obj overlayID) -> vector",
			"ii",	0,	},

		{	"objGetOverlayProperty",			fnObjGet,		FN_OBJ_GET_OVERLAY_PROPERTY,
			"(objGetOverlayProperty obj overlayID property) -> value\n\n"
			
			"property\n\n"
			
			"   'counter\n"
			"   'counterLabel\n"
			"   'pos\n"
			"   'rotation\n"
			"   'type\n"
			,
			"iis",	0,	},

		{	"objGetOverlayRotation",	fnObjGet,		FN_OBJ_GET_OVERLAY_ROTATION,
			"(objGetOverlayRotation obj overlayID) -> rotation",
			"ii",	0,	},

		{	"objGetOverlays",			fnObjGet,		FN_OBJ_GET_OVERLAYS,
			"(objGetOverlays obj [criteria|overlayType]) -> list of overlayIDs",
			"i*",	0,	},

		{	"objGetOverlayType",			fnObjGet,		FN_OBJ_GET_OVERLAY_TYPE,
			"(objGetOverlayType obj overlayID) -> type",
			"ii",	0,	},

		{	"objGetPlayerPriceAdj"	,		fnObjGet,		FN_OBJ_GET_PLAYER_PRICE_ADJ,	
			"(objGetPlayerPriceAdj obj [data]) -> priceAdj (or Nil if no adj)",
			"i*",		0,	},

		{	"objGetPos",					fnObjGetOld,		FN_OBJ_POSITION,
			"(objGetPos obj) -> vector",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objGetProperty",				fnObjGet,		FN_OBJ_GET_PROPERTY,
			"(objGetProperty obj property) -> value\n\n"

			"property (all)\n\n"

			"   'ascended\n"
			"   'category -> 'beam | 'effect | 'marker | 'missile | 'ship | 'station\n"
			"   'commsKey\n"
			"   'currency -> currency type UNID\n"
			"   'currencyName\n"
			"   'cyberDefenseLevel\n"
			"   'damageDesc -> { shieldLevel:n armorIntegrity:n HullIntegrity:n }\n"
			"   'destiny\n"
			"   'dockingPorts -> list of structs with the keys:\n"
			"       objID: ID of docked object, if any\n"
			"       pos: port position\n"
			"       status: 'empty | 'inUse\n\n"
			
			"   'hasDockingPorts\n"
			"   'id\n"
			"   'installArmorMaxLevel\n"
			"   'installDeviceMaxLevel\n"
			"   'installDeviceUpgradeOnly -> true if it only installs devices as part of a purchase\n"
			"   'known\n"
			"   'level\n"
			"   'mass -> hull mass in tons\n"
			"   'namePattern\n"
			"   'paintLayer -> 'background | 'space | 'stations | 'ships | 'effects | 'overhang\n"
			"   'playerMissionsGiven\n"
			"   'radioactive\n"
			"   'refuelMaxLevel\n"
			"   'removeDeviceMaxLevel\n"
			"   'repairArmorMaxLevel\n"
			"   'scale -> 'star | 'world | 'station | 'ship | 'flotsam\n"
			"   'sovereign\n"
			"   'stealth\n"
			"   'underAttack\n"
			"\n"
			"property (ships)\n\n"
			
			"   'alwaysLeaveWreck\n"
			"   'availableDeviceSlots\n"
			"   'availableNonWeaponSlots\n"
			"   'availableWeaponSlots\n"
			"   'blindingImmune\n"
			"   'cargoSpace -> in tons\n"
			"   'counterIncrementRate\n"
			"   'counterValue\n"
			"   'character\n"
			"   'characterClass   (player ship only)\n"
			"   'deviceDamageImmune\n"
			"   'deviceDisruptImmune\n"
			"   'disintegrationImmune\n"
			"   'dockedAtID\n"
			"   'dockingEnabled\n"
			"   'DockingPortCount\n"
			"   'drivePowerUse -> in kW\n"
			"   'EMPImmune\n"
			"   'fuelCapacity -> in He3 fuel rods\n"
			"   'fuelCapacityExact -> 2500 = 1 He3 fuel rod\n"
			"   'fuelCriteria -> criteria string\n"
			"   'fuelEfficiency -> 15 = standard, more is better\n"
			"   'fuelEfficiencyBonus -> increased efficiency over standard, in %\n"
			"   'fuelLeft -> in He3 fuel rods\n"
			"   'fuelLeftExact -> 2500 = 1 He3 fuel rod\n"
			"   'healerLeft\n"
			"   'hp\n"
			"   'hullPrice -> in object's default currency\n"
			"   'interiorHP\n"
			"   'maxCounter\n"
			"   'maxHp\n"
			"   'maxInteriorHP\n"
			"   'maxFuel -> in He3 fuel rods\n"
			"   'maxFuelExact -> 2500 = 1 He3 fuel rod\n"
			"   'maxSpeed -> in % c\n"
			"   'openDockingPortCount\n"
			"   'operatingSpeed -> 'emergency | 'full | 'half | 'quarter\n"
			"   'playerBlacklisted\n"
			"   'playerWingman\n"
			"   'power -> max reactor output in kW\n"
			"   'powerUse -> current power draw in kW\n"
			"   'price -> total price in default currency\n"
			"   'radiationImmune\n"
			"   'rotation -> current facing in degrees, counterclockwise from right\n"
			"   'rotationSpeed -> in degrees per tick, clockwise positive\n"
			"   'selectedLauncher\n"
			"   'selectedMissile\n"
			"   'selectedWeapon\n"
			"   'shatterImmune\n"
			"   'showMapLabel\n"
			"   'thrust -> in GN\n"
			"   'thrustToWeight -> acceleration, 1 = 500 m/s^2 (ships stats show this / 1000)\n"
			"\n"
			"property (stations)\n\n"

			"   'abandoned\n"
			"   'active\n"
			"   'angry\n"
			"   'barrier\n"
			"   'destNodeID\n"
			"   'destStargateID\n"
			"   'dockingPortCount\n"
			"   'explored -> True if the player has docked with the station\n"
			"   'hp\n"
			"   'ignoreFriendlyFire\n"
			"   'immutable\n"
			"   'maxHP\n"
			"   'maxStructuralHP\n"
			"   'openDockingPortCount\n"
			"   'orbit\n"
			"   'parallax\n"
			"   'playerBlacklisted\n"
			"   'rotation\n"
			"   'rotationSpeed\n"
			"   'shipConstructionEnabled\n"
			"   'shipReinforcementEnabled\n"
			"   'showMapLabel\n"
			"   'stargateID\n"
			"   'structuralHP\n"
			"   'subordinates\n"
			"   'superior\n"
			"\n"
			"property (missiles)\n\n"

			"   'lifeLeft\n"
			"   'rotation\n"
			"   'sourceObj\n"
			"   'target\n"
			"\n"
			"property (markers)\n\n"

			"   'style -> 'smallCross\n"
			"\n"
			"NOTE: All type properties (accessed via typGetProperty) are also valid object properties.",

			"is",	0,	},

		{	"objGetRefuelItemAndPrice",		fnObjGet,		FN_OBJ_GET_REFUEL_ITEM,	
			"(objGetRefuelItemAndPrice obj objToRefuel) -> (item price)",
			"ii",		0,	},

		{	"objGetSellPrice",				fnObjGet,		FN_OBJ_GET_SELL_PRICE,	
			"(objGetSellPrice obj item ['noInventoryCheck]) -> price (at which obj sells item)",
			"il*",		PPFLAG_SIDEEFFECTS,	},

		{	"objGetShieldLevel",			fnObjGetOld,		FN_OBJ_SHIELD_LEVEL,
			"(objGetShieldLevel obj) -> 0-100% (or -1 for no shields)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objGetShipBuyPrice",			fnObjGet,		FN_OBJ_GET_SHIP_BUY_PRICE,	
			"(objGetShipBuyPrice obj shipObj) -> price (at which obj buys ship)",
			"iv",		0,	},

		{	"objGetShipSellPrice",			fnObjGet,		FN_OBJ_GET_SHIP_SELL_PRICE,	
			"(objGetShipSellPrice obj shipObj) -> price (at which obj sells ship)",
			"iv",		0,	},

		{	"objGetShipwreckType",			fnObjGet,		FN_OBJ_SHIPWRECK_TYPE,
			"(objGetShipwreckType obj) -> unid",
			"i",	0,	},

		{	"objGetStaticData",				fnObjData,		FN_OBJ_GET_STATIC_DATA,
			"(objGetStaticData obj attrib) -> data",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objGetTarget",					fnObjGetOld,		FN_OBJ_TARGET,
			"(objGetTarget obj) -> obj",
			NULL,	0,	},

		{	"objGetType",					fnObjGet,		FN_OBJ_TYPE,
			"(objGetType obj) -> unid",
			"i",	0,	},

		{	"objGetTypeData",				fnObjData,		FN_OBJ_GET_GLOBAL_DATA,
			"(objGetTypeData obj attrib) -> data",
			NULL,	0,	},

		{	"objGetVel",					fnObjGet,		FN_OBJ_VELOCITY,	
			"(objGetVel obj) -> velVector",
			"i",		0,	},

		{	"objGetVisibleDamage",			fnObjGetOld,		FN_OBJ_VISIBLE_DAMAGE,
			"(objGetVisibleDamage obj) -> damage %",
			NULL,	0,	},

		{	"objHasAttribute",				fnObjGetOld,		FN_OBJ_ATTRIBUTE,
			"(objHasAttribute obj attrib) -> True/Nil",
			NULL,	0,	},

		{	"objHasItem",					fnObjItem,		FN_OBJ_HAS_ITEM,
			"(objHasItem obj item [count] [options]) -> number of items (or Nil)\n\n"
			
			"options:\n\n"
			
			"   'ignoreCharges\n"
			"   'ignoreData\n"
			"   'ignoreDisruption\n"
			"   'ignoreEnhancements\n"
			"   'ignoreInstalled\n",

			"iv*",	0,	},

		{	"objHasTradeService",			fnObjGet,		FN_OBJ_HAS_SERVICE,
			"(objHasTradeService obj service) -> True/Nil",
			"is",	0,	},

		{	"objIncData",					fnObjData,		FN_OBJ_INCREMENT_DATA,
			"(objIncData obj attrib [increment]) -> new value",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objIncOverlayData",			fnObjGet,		FN_OBJ_INC_OVERLAY_DATA,
			"(objIncOverlayData obj overlayID attrib [increment]) -> new value",
			"iis*",	PPFLAG_SIDEEFFECTS,	},

		{	"objIncVel",					fnObjSet,		FN_OBJ_INCREMENT_VELOCITY,	
			"(objIncVel obj velVector) -> velVector\n\n"

			"velVector in % of light-speed",

			"il",		PPFLAG_SIDEEFFECTS,	},

		{	"objIsAngryAt",				fnObjGet,		FN_OBJ_IS_ANGRY_AT,
			"(objIsAngryAt obj targetObj) -> True/Nil",
			"ii",	0,	},

		{	"objIsDockedAt",				fnObjGet,		FN_OBJ_DOCKED_AT,
			"(objIsDockedAt obj stationObj) -> True/Nil",
			"ii",	0,	},

		{	"objIsEnemy",					fnObjGetOld,		FN_OBJ_ENEMY,
			"(objIsEnemy obj target) -> True/Nil",
			NULL,	0,	},

		{	"objIsIdentified",				fnObjGet,		FN_OBJ_IDENTIFIED,
			"(objIsIdentified obj) -> True/Nil",
			"i",	0,	},

		{	"objIsKnown",					fnObjGetOld,		FN_OBJ_KNOWN,
			"(objIsKnown obj) -> True/Nil",
			NULL,	0,	},

		{	"objIsParalyzed",				fnObjGet,		FN_OBJ_PARALYZED,
			"(objIsParalyzed obj) -> True/Nil",
			"i",	0,	},

		{	"objIsRadioactive",				fnObjGet,		FN_OBJ_RADIOACTIVE,
			"(objIsRadioactive obj) -> True/Nil",
			"i",	0,	},

		{	"objJumpTo",					fnObjSetOld,		FN_OBJ_JUMP,
			"(objJumpTo obj pos) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objLowerShields",				fnObjGetOld,		FN_OBJ_LOWER_SHIELDS,
			"(objLowerShields obj) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objMakeParalyzed",				fnObjSetOld,		FN_OBJ_PARALYSIS,
			"(objMakeParalyzed obj ticks) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objMatches",					fnObjGet,			FN_OBJ_MATCHES,
			"(objMatches obj source filter) -> True/Nil\n\n"

			"criteria as sysFindObject",

			"iis",	0,	},

		{	"objProgramDamage",				fnProgramDamage,0,
			"(objProgramDamage obj hacker progName aiLevel code) -> True/Nil\n\n"

			"Chance to execute code is: 90 + 10 * (aiLevel - cyberDefenseLevel)",

			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objRecordBuyItem",					fnObjSet,		FN_OBJ_RECORD_BUY_ITEM,
			"(objRecordBuyItem buyerObj sellerObj item [currency] price) -> True/Nil",
			"iiv*",	PPFLAG_SIDEEFFECTS,	},

		{	"objRegisterForEvents",			fnObjSetOld,		FN_OBJ_REGISTER_EVENTS,
			"(objRegisterForEvents target obj) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objRegisterForSystemEvents",	fnObjSet,		FN_OBJ_REGISTER_SYSTEM_EVENTS,
			"(objRegisterForSystemEvents target range) -> True/Nil",
			"ii",	PPFLAG_SIDEEFFECTS,	},

		{	"objRemoveItem",				fnObjItem,		FN_OBJ_REMOVE_ITEM,
			"(objRemoveItem obj item [count] [options]) -> True/Nil",
			"iv*",	PPFLAG_SIDEEFFECTS,	},

		{	"objRemoveItemEnhancement",		fnObjSet,		FN_OBJ_REMOVE_ITEM_ENHANCEMENT,
			"(objRemoveItemEnhancement obj item enhancementID) -> True/Nil",
			"ivi",	PPFLAG_SIDEEFFECTS,	},

		{	"objRemoveOverlay",				fnObjSet,		FN_OBJ_REMOVE_OVERLAY,
			"(objRemoveOverlay obj overlayID) -> True/Nil",
			"ii",	PPFLAG_SIDEEFFECTS,	},

		{	"objResume",					fnObjSet,		FN_OBJ_RESUME,
			"(objResume obj [gateObj]) -> True/Nil",
			"i*",	PPFLAG_SIDEEFFECTS,	},

		{	"objSendMessage",				fnObjSendMessage,		FN_OBJ_MESSAGE,
			"(objSendMessage obj sender text) -> True/Nil",
			"ivv",	PPFLAG_SIDEEFFECTS,	},

		{	"objSendMessageTranslate",		fnObjSendMessage,		FN_OBJ_MESSAGE_TRANSLATE,
			"(objSendMessageTranslate obj sender textID [data]) -> True/Nil",
			"ivs*",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetCharacterData",			fnObjSet,		FN_OBJ_SET_CHARACTER_DATA,
			"(objSetCharacterData obj attrib data) -> True/Nil",
			"isv",	0,	},

		{	"objSetData",					fnObjData,		FN_OBJ_SETDATA,
			"(objSetData obj attrib data) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objSetDeviceActivationDelay",	fnObjSet,		FN_OBJ_DEVICE_ACTIVATION_DELAY,
			"(objSetDeviceActivationDelay obj deviceItem [delay]) -> True/Nil",
			"iv*",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetEventHandler",			fnObjSet,		FN_OBJ_SET_EVENT_HANDLER,
			"(objSetEventHandler obj unid) -> True/Nil",
			"ii",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetIdentified",				fnObjSet,		FN_OBJ_IDENTIFIED,
			"(objSetIdentified obj) -> True/Nil",
			"i",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetItemData",				fnObjSet,		FN_OBJ_SET_ITEM_DATA,
			"(objSetItemData obj item attrib data [count]) -> item",
			"ivsv*",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetItemProperty",			fnObjSet,		FN_OBJ_SET_ITEM_PROPERTY,
			"(objSetItemProperty obj item property value [count]) -> item\n\n"
			
			"property\n\n"

			"   'charges charges\n"
			"   'damaged [True|Nil]\n"
			"   'disrupted [True|Nil|ticks]\n"
			"   'enabled [True|Nil|'silentDisabled|'silentEnabled]\n"
			"   'fireArc Nil|(min max)|'omnidirectional\n"
			"   'hp hitPoints\n"
			"   'incCharges charges\n"
			"   'linkedFireOptions list-of-options\n"
			"   'pos (angle radius [z])\n"
			"   'secondary [True|Nil]",

			"ivs*",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetKnown",					fnObjGetOld,		FN_OBJ_SET_KNOWN,
			"(objSetKnown obj) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objSetName",					fnObjSetOld,		FN_OBJ_NAME,
			"(objSetName obj name [flags]) -> True/Nil\n\n"
			
			"flags\n\n"
			
			"   0x0001 Definite article\n"
			"   0x0002 Pluralize first word\n"
			"   0x0004 Add 'es' to pluralize\n"
			"   0x0008 Custom plural after semicolon\n"
			"   0x0010 Pluralize second word\n"
			"   0x0020 Reverse 'a' vs 'an'\n"
			"   0x0040 No article\n"
			"   0x0080 Personal name",

			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objSetObjRefData",				fnObjData,		FN_OBJ_SET_OBJREF_DATA,
			"(objSetObjRefData obj attrib obj) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objSetOverlayData",			fnObjGet,		FN_OBJ_SET_OVERLAY_DATA,
			"(objSetOverlayData obj overlayID attrib data)",
			"iisv",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetOverlayEffectProperty",			fnObjGet,		FN_OBJ_SET_OVERLAY_EFFECT_PROPERTY,
			"(objSetOverlayEffectProperty obj overlayID property value)",
			"iisv",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetOverlayPos",			fnObjGet,		FN_OBJ_SET_OVERLAY_POSITION,
			"(objSetOverlayPos obj overlayID pos)",
			"iiv",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetOverlayProperty",			fnObjGet,		FN_OBJ_SET_OVERLAY_PROPERTY,
			"(objSetOverlayProperty obj overlayID property value)\n\n"
			
			"property:\n\n"
			
			"   'counter\n"
			"   'counterLabel\n"
			"   'pos position\n"
			"   'rotation angle",

			"iisv",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetOverlayRotation",		fnObjGet,		FN_OBJ_SET_OVERLAY_ROTATION,
			"(objSetOverlayRotation obj overlayID rotation)",
			"iii",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetPos",					fnObjSet,		FN_OBJ_POSITION,
			"(objSetPos obj vector [rotation])",
			"iv*",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetProperty",				fnObjSet,		FN_OBJ_SET_PROPERTY,
			"(objSetProperty obj property value) -> True/Nil\n\n"
			
			"property (ships)\n\n"

			"   'alwaysLeaveWreck True|Nil\n"
			"   'counterValue value\n"
			"   'counterValueIncrement value\n"
			"   'dockingEnabled True|Nil\n"
			"   'commsKey key\n"
			"   'known True|Nil\n"
			"   'operatingSpeed 'full|'half|'quarter|'emergency\n"
			"   'playerBlacklisted True|Nil\n"
			"   'playerWingman True|Nil\n"
			"   'rotation angle\n"
			"   'selectedMissile type|item\n"
			"   'selectedWeapon type|item\n"
			"   'showMapLabel True|Nil\n"
			"   'sovereign type\n"
			"\n"
			"property (stations)\n\n"

			"   'angry True|Nil|ticks\n"
			"   'barrier True|Nil\n"
			"   'explored True|Nil\n"
			"   'hp hitPoints\n"
			"   'ignoreFriendlyFire True|Nil\n"
			"   'immutable True|Nil\n"
			"   'known True|Nil\n"
			"   'maxHP hitPoints\n"
			"   'maxStructuralHP hitPoints\n"
			"   'orbit orbit|Nil\n"
			"   'paintLayer 'overhang|Nil\n"
			"   'parallax factor\n"
			"   'playerBlacklisted True|Nil\n"
			"   'shipConstructionEnabled True|Nil\n"
			"   'shipReinforcementEnabled True|Nil\n"
			"   'showMapLabel True|Nil\n"
			"   'sovereign type\n"
			"   'structuralHP hitPoints",

			"isv",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetShowAsDestination",			fnObjSet,		FN_OBJ_SET_AS_DESTINATION,
			"(objSetShowAsDestination obj [options]) -> True/Nil\n\n"
			
			"options:\n\n"
			
			"   'autoClear            Clear when in SRS range\n"
			"   'autoClearOnDestroy   Clear when destroyed\n"
			"   'autoClearOnDock      Clear when player docks\n"
			"   'showDistance         Show distance\n"
			"   'showHighlight        Show target highlight",

			"i*",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetTradeDesc",				fnObjSet,		FN_OBJ_SET_TRADE_DESC,
			"(objSetTradeDesc obj currency [maxCurrency replenishCurrency]) -> True/Nil",
			"iv*",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetTypeData",				fnObjData,		FN_OBJ_SET_GLOBAL_DATA,
			"(objSetTypeData obj attrib data)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objSetVel",					fnObjSet,		FN_OBJ_VELOCITY,
			"(objSetVel obj velVector)",
			"il",	PPFLAG_SIDEEFFECTS,	},

		{	"objSuspend",					fnObjSet,		FN_OBJ_SUSPEND,
			"(objSuspend obj)",
			"i",	PPFLAG_SIDEEFFECTS,	},

		{	"objTranslate",					fnObjGet,		FN_OBJ_TRANSLATE,
			"(objTranslate obj textID [data] [default]) -> text (or Nil)",
			"iv*",	0,	},

		{	"objUnregisterForEvents",		fnObjSetOld,		FN_OBJ_UNREGISTER_EVENTS,
			"(objUnregisterForEvents target obj)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objUnregisterForSystemEvents",	fnObjSet,		FN_OBJ_UNREGISTER_SYSTEM_EVENTS,
			"(objUnregisterForSystemEvents target)",
			"i",	PPFLAG_SIDEEFFECTS,	},

		//	Station functions
		//	-----------------

		{	"staClearFireReconEvent",		fnStationGetOld,	FN_STATION_CLEAR_FIRE_RECON,
			"(staClearFireReconEvent station)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"staClearReconned",				fnStationGetOld,	FN_STATION_CLEAR_RECON,
			"(staClearReconned station)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"staGetDockedShips",			fnStationGetOld,	FN_STATION_GET_DOCKED_SHIPS,
			"(staGetDockedShips station) -> list of docked ships",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"staGetImageVariant",			fnStationGetOld,	FN_STATION_IMAGE_VARIANT,
			"(staGetImageVariant station) -> variant",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"staGetSubordinates",			fnStationGetOld,	FN_STATION_GET_SUBORDINATES,
			"(staGetSubordinates station) -> list of subordinates (e.g., guardians)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"staIsEncountered",				fnStationType,	FN_STATION_ENCOUNTERED,	
			"(staIsEncountered type) -> True/Nil",
			"i",	0,	},

		{	"staIsReconned",				fnStationGetOld,	FN_STATION_RECON,
			"(staIsReconned station) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"staSetActive",					fnStationSet,		FN_STATION_SET_ACTIVE,
			"(staSetActive station [True/Nil])",
			"i*",	PPFLAG_SIDEEFFECTS,	},

		{	"staSetFireReconEvent",			fnStationGetOld,	FN_STATION_SET_FIRE_RECON,
			"(staSetFireReconEvent station)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"staSetImageVariant",			fnStationSetOld,	FN_STATION_IMAGE_VARIANT,
			"(staSetImageVariant station variant)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"staSetShowMapLabel",			fnStationSet,		FN_STATION_SHOW_MAP_LABEL,
			"(staSetShowMapLabel station True/Nil)",
			"iv",	PPFLAG_SIDEEFFECTS,	},

		//	Mission functions
		//	-----------------

		{	"msnAccept",					fnMissionSet,		FN_MISSION_ACCEPTED,
			"(msnAccept missionObj)",
			"i",	PPFLAG_SIDEEFFECTS,	},

		{	"msnAddTimerEvent",				fnMissionSet,		FN_MISSION_ADD_TIMER,
			"(msnAddTimerEvent missionObj delay event)\n\n"
			
			"delay in ticks",

			"iis",	PPFLAG_SIDEEFFECTS,	},

		{	"msnAddRecurringTimerEvent",	fnMissionSet,		FN_MISSION_ADD_RECURRING_TIMER,	
			"(msnAddRecurringTimerEvent missionObj interval event)\n\n"
			
			"interval in ticks",

			"iis",	PPFLAG_SIDEEFFECTS,	},

		{	"msnCancelTimerEvent",				fnMissionSet,	FN_MISSION_CANCEL_TIMER,
			"(msnCancelTimerEvent missionObj event) -> True/Nil",
			"is",	0,	},

		{	"msnCanCreate",					fnMission,			FN_MISSION_CAN_CREATE,
			"(msnCanCreate unid [owner [data]]) -> True|Nil",
			"v*",	0,	},

		{	"msnCreate",					fnMission,			FN_MISSION_CREATE,
			"(msnCreate unid owner [data]) -> missionObj|Nil\n"
			"(msnCreate unid-list owner [data]) -> missionObj|Nil",
			"vi*",	PPFLAG_SIDEEFFECTS,	},

		{	"msnDecline",					fnMissionSet,		FN_MISSION_DECLINED,
			"(msnDecline missionObj)",
			"i",	PPFLAG_SIDEEFFECTS,	},

		{	"msnDestroy",					fnObjSet,			FN_OBJ_DESTROY,
			"(msnDestroy missionObj) -> True/Nil",
			"i*",	PPFLAG_SIDEEFFECTS,	},

		{	"msnFailure",				fnMissionSet,		FN_MISSION_FAILURE,
			"(msnFailure missionObj [data])",
			"i*",	PPFLAG_SIDEEFFECTS,	},

		{	"msnFind",						fnMission,			FN_MISSION_FIND,
			"(msnFind [source] criteria) -> list of missionObjs\n\n"
			
			"criteria\n\n"
			
			"   *                  Include all missions states\n"
			"   a                  Include active player missions\n"
			"   c                  Include completed missions (not necessarily debriefed)\n"
			"   o                  Include open missions\n"
			"   r                  Include already debriefed (recorded) missions\n"
			"   u                  Include non-player missions\n"
			"   D                  Only missions debriefed by source\n"
			"   P                  Return only the mission with highest priority\n"
			"   S                  Only missions owned by source\n"
			"   +/-{attrib}        Require/exclude missions with given attribute\n"
			"   +/-ownerID:{id}    Require/exclude missions with given owner\n"
			"   +/-unid:{unid}     Require/exclude missions of given unid",
			"*s",	0,	},

		{	"msnFireEvent",					fnObjSet,		FN_OBJ_FIRE_EVENT,
			"(msnFireEvent missionObj event [data]) -> result of event",
			"is*",	PPFLAG_SIDEEFFECTS,	},

		{	"msnGetData",					fnObjData,		FN_OBJ_GETDATA,
			"(msnGetData missionObj attrib) -> data",
			NULL,	0,	},

		{	"msnGetObjRefData",				fnObjData,		FN_OBJ_GET_OBJREF_DATA,
			"(msnGetObjRefData missionObj attrib) -> obj",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"msnGetProperty",				fnObjGet,		FN_OBJ_GET_PROPERTY,
			"(msnGetProperty missionObj property) -> value\n\n"
			
			"property\n\n"
			
			"   'acceptedOn        Tick on which player accepted mission (or Nil)\n"
			"   'canBeDeclined     Mission can be declined by player\n"
			"   'canBeDeleted      Mission can be deleted by player\n"
			"   'debrieferID       ID of the object that will debrief the player\n"
			"   'forceUndockAfterDebrief  Force undock after showing debrief screen\n"
			"   'hasDebrief        Mission has a debrief phase\n"
			"   'id                Mission object ID\n"
			"   'isActive          Is an active player mission\n"
			"   'isCompleted       Is a completed mission (player or non-player)\n"
			"   'isDebriefed       Player has been debriefed\n"
			"   'isDeclined        Player has declined mission\n"
			"   'isFailure         Mission has failed\n"
			"   'isIntroShown      Player has been shown intro text\n"
			"   'isOpen            Mission is available to player\n"
			"   'isRecorded        Mission has been completed and debriefed\n"
			"   'isSuccess         Mission has succeeded\n"
			"   'isUnavailable     Mission is unavailable to player\n"
			"   'maxAppearing      Max number of this type that can exist\n"
			"   'name              The name of the mission\n"
			"   'nodeID            ID of the mission's owner system\n"
			"   'ownerID           ID of the mission's owner object\n"
			"   'priority          Mission priority\n"
			"   'summary           A summary description of the mission\n"
			"   'totalAccepted     Count of this type accepted by player\n"
			"   'totalExisting     Count of this type currently existing in universe\n"
			"   'unid              Mission type UNID",

			"is",	0,	},

		{	"msnGetStaticData",				fnObjData,		FN_OBJ_GET_STATIC_DATA,
			"(msnGetStaticData missionObj attrib) -> data",
			NULL,	0,	},

		{	"msnGetTypeData",				fnObjData,		FN_OBJ_GET_GLOBAL_DATA,
			"(msnGetTypeData missionObj attrib) -> data",
			NULL,	0,	},

		{	"msnIncData",					fnObjData,		FN_OBJ_INCREMENT_DATA,
			"(msnIncData missionObj attrib [increment]) -> new value",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"msnRegisterForEvents",			fnObjSetOld,		FN_OBJ_REGISTER_EVENTS,
			"(msnRegisterForEvents missionObj obj)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"msnReward",				fnMissionSet,		FN_MISSION_REWARD,
			"(msnReward missionObj [data])",
			"i*",	PPFLAG_SIDEEFFECTS,	},

		{	"msnSetData",					fnObjData,		FN_OBJ_SETDATA,
			"(msnSetData missionObj attrib data)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"msnSetObjRefData",				fnObjData,		FN_OBJ_SET_OBJREF_DATA,
			"(msnSetObjRefData missionObj attrib obj)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"msnSetPlayerTarget",			fnMissionSet,		FN_MISSION_SET_PLAYER_TARGET,
			"(msnSetPlayerTarget missionObj)",
			"i",	PPFLAG_SIDEEFFECTS,	},

		{	"msnSetProperty",				fnObjSet,		FN_OBJ_SET_PROPERTY,
			"(msnSetProperty obj property value) -> True/Nil\n\n"
			
			"property\n\n"

			"   'debrieferID obj\n"
			"   'isDebriefed True|Nil\n"
			"   'isDeclined True|Nil\n"
			"   'isIntroShown True|Nil\n"
			"   'name newName\n"
			"   'summary newSummary",

			"isv",	PPFLAG_SIDEEFFECTS,	},

		{	"msnSetTypeData",				fnObjData,		FN_OBJ_SET_GLOBAL_DATA,
			"(msnSetTypeData missionObj attrib data)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"msnSetUnavailable",			fnMissionSet,		FN_MISSION_CLOSED,
			"(msnSetUnavailable missionObj)",
			"i",	PPFLAG_SIDEEFFECTS,	},

		{	"msnSuccess",				fnMissionSet,		FN_MISSION_SUCCESS,
			"(msnSuccess missionObj [data])",
			"i*",	PPFLAG_SIDEEFFECTS,	},

		{	"msnTranslate",					fnObjGet,		FN_OBJ_TRANSLATE,
			"(msnTranslate missionObj textID [data] [default]) -> text (or Nil)",
			"iv*",	0,	},

		//	System functions
		//	----------------

		{	"sysAddEncounterEvent",			fnSystemAddEncounterEvent,	FN_ADD_ENCOUNTER_FROM_GATE,
			"(sysAddEncounterEvent delay target encounterID gateObj|pos)\n\n"

			"delay in ticks",

			"iiiv",	PPFLAG_SIDEEFFECTS,	},

		{	"sysAddEncounterEventAtDist",	fnSystemAddEncounterEvent,	FN_ADD_ENCOUNTER_FROM_DIST,
			"(sysAddEncounterEventAtDist delay target encounterID distance)\n\n"

			"delay in ticks",

			"iiii",	PPFLAG_SIDEEFFECTS,	},

		{	"sysAddObjTimerEvent",			fnSystemAddStationTimerEvent,	FN_ADD_TIMER_NORMAL,	
			"(sysAddObjTimerEvent delay obj event)\n\n"

			"delay in ticks",

			"iis",	PPFLAG_SIDEEFFECTS,	},

		{	"sysAddObjRecurringTimerEvent",	fnSystemAddStationTimerEvent,	FN_ADD_TIMER_RECURRING,	
			"(sysAddObjRecurringTimerEvent interval obj event)\n\n"

			"interval in ticks",

			"iis",	PPFLAG_SIDEEFFECTS,	},

		{	"sysAddTypeRangeEvent",			fnSystemAddStationTimerEvent,	FN_ADD_TYPE_RANGE_EVENT,
			"(sysAddTypeRangeEvent type event options)\n\n"
				
			"options:\n\n"
				
			"   'center            Fire event when target gets close to this point\n"
			"   'radius            Within this radius (light-seconds)\n"
			"   'criteria          Objects that will trigger. If Nil, player triggers",

			"isv",	PPFLAG_SIDEEFFECTS,	},

		{	"sysAddTypeTimerEvent",			fnSystemAddStationTimerEvent,	FN_ADD_TYPE_TIMER_NORMAL,
			"(sysAddTypeTimerEvent delay type event)",
			"iis",	PPFLAG_SIDEEFFECTS,	},

		{	"sysAddTypeRecurringTimerEvent",	fnSystemAddStationTimerEvent,	FN_ADD_TYPE_TIMER_RECURRING,	
			"(sysAddTypeRecurringTimerEvent interval type event)\n\n"

			"interval in ticks",

			"iis",	PPFLAG_SIDEEFFECTS,	},

		{	"sysAddStargateTopology",			fnSystemGet,	FN_SYS_ADD_STARGATE_TOPOLOGY,
			"(sysAddStargateTopology [nodeID] gateID destNodeID destGateID) -> True/Nil",
			"sss*",	PPFLAG_SIDEEFFECTS,	},

		{	"sysAscendObject",				fnSystemGet,	FN_SYS_ASCEND_OBJECT,
			"(sysAscendObject obj) -> True/Nil",
			"i",	PPFLAG_SIDEEFFECTS,	},

		{	"sysCalcFireSolution",				fnSystemGet,	FN_SYS_GET_FIRE_SOLUTION,
			"(sysCalcFireSolution targetPos targetVel speed) -> angle to shoot (Nil, if no solution)",
			"vvv",	0,	},

		{	"sysCalcStdCombatStrength",			fnSystemGet,	FN_SYS_GET_STD_COMBAT_STRENGTH,
			"(sysCalcStdCombatStrength level) -> standard combat strength for level",
			"i",	0,	},

		{	"sysCalcTravelDistance",			fnSystemGet,	FN_SYS_GET_TRAVEL_DISTANCE,
			"(sysCalcTravelDistance speed time) -> distance in light-seconds",
			"vi",	0,	},

		{	"sysCalcTravelTime",				fnSystemGet,	FN_SYS_GET_TRAVEL_TIME,
			"(sysCalcTravelTime distance speed) -> time in ticks",
			"ii",	0,	},

		{	"sysCancelTimerEvent",				fnSystemAddStationTimerEvent,	FN_CANCEL_TIMER,	
			"(sysCancelTimerEvent obj event) -> True/Nil",
			"is",	0,	},

		{	"sysCancelTypeTimerEvent",				fnSystemAddStationTimerEvent,	FN_CANCEL_TYPE_TIMER,	
			"(sysCancelTypeTimerEvent type event) -> True/Nil",
			"is",	0,	},

		{	"sysCreateEffect",				fnSystemCreateEffect,	0,
			"(sysCreateEffect effectID anchorObj pos [rotation] [params]) -> True/Nil",
			"vvv*",	PPFLAG_SIDEEFFECTS,	},

		{	"sysCreateEncounter",			fnSystemCreate,		FN_SYS_CREATE_ENCOUNTER,
			"(sysCreateEncounter unid [options]) -> True/Nil\n\n"
				
				"options:\n\n"
				
				"   'distance      Encounter distance (light-seconds), if gate is Nil\n"
				"   'gate          Gate to appear at (if Nil, use distance)\n"
				"   'target        Target of encounter",

			"i*",	PPFLAG_SIDEEFFECTS,	},

		{	"sysCreateFlotsam",			fnSystemCreate,		FN_SYS_CREATE_FLOTSAM,
			"(sysCreateFlotsam item|unid pos sovereignID) -> obj",
		//		pos is either a position vector or a gate object
			"vvi",	PPFLAG_SIDEEFFECTS,	},

		{	"sysCreateHitEffect",				fnSystemCreate,		FN_SYS_CREATE_HIT_EFFECT,
			"(sysCreateHitEffect weaponUNID hitObj hitPos hitDir damageHP) -> True/Nil",
			"iivii",	PPFLAG_SIDEEFFECTS,	},

		{	"sysCreateLookup",				fnSystemCreate,	FN_SYS_CREATE_LOOKUP,
			"(sysCreateLookup tableName orbit) -> True/Nil",
			"sv",	PPFLAG_SIDEEFFECTS,	},

		{	"sysCreateMarker",				fnSystemCreateMarker,	0,
			"(sysCreateMarker name pos sovereignID) -> marker",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"sysCreateShip",				fnSystemCreateShip,	0,
			"(sysCreateShip unid pos sovereignID [options|eventHandler|controller]) -> ship or list\n\n"
				
			"pos is either a position vector or a space object (gate, station, ship)\n\n"

			"options:\n\n"
			
			"   'controller\n"
			"   'eventHandler\n"
			"   'target (for ship tables)\n"
			"\n"
			"controller:\n\n"
			"                     standard\n"
			"   'auton            auton\n"
			"   'ferian           Ferian\n"
			"   'fleet            fleet member\n"
			"   'fleetcommand     fleet squad leader\n"
			"   'gaianprocessor   Gaian processor",

			"ivi*",	PPFLAG_SIDEEFFECTS,	},

		{	"sysCreateShipwreck",			fnSystemCreate,		FN_SYS_CREATE_SHIPWRECK,
			"(sysCreateShipwreck unid pos sovereignID) -> shipwreck\n\n"

			"pos is either a position vector or a space object",

			"ivi",	PPFLAG_SIDEEFFECTS,	},

		{	"sysCreateStargate",			fnSystemCreateStation,	FN_SYS_CREATE_STARGATE,
			"(sysCreateStargate unid pos gateID [destNodeID destGateID]) -> obj",
			"ivs*",	PPFLAG_SIDEEFFECTS,	},

		{	"sysCreateStation",				fnSystemCreateStation,	FN_SYS_CREATE_STATION,
			"(sysCreateStation unid pos [eventHandler]) -> obj",
			"iv*",	PPFLAG_SIDEEFFECTS,	},

		{	"sysCreateTerritory",			fnSystemCreate,			FN_SYS_CREATE_TERRITORY,
			"(sysCreateTerritory orbit minRadius maxRadius attributes [criteria]) -> True/Nil",
			"viiv*",	PPFLAG_SIDEEFFECTS,	},

		{	"sysCreateWeaponFire",			fnSystemCreate,			FN_SYS_CREATE_WEAPON_FIRE,
			"(sysCreateWeaponFire weaponID objSource pos dir speed objTarget [options] [bonus%]) -> obj\n\n"
			
			"options:\n\n"
			
			"   'detonateNow\n"
			"   'fireEffect\n"
			"   'soundEffect",

			"vvviii*",	PPFLAG_SIDEEFFECTS,	},

		{	"sysDescendObject",				fnSystemGet,	FN_SYS_DESCEND_OBJECT,
			"(sysDescendObject objID pos) -> obj",
			"iv",	PPFLAG_SIDEEFFECTS,	},

		{	"sysFindObject",				fnSystemFind,	FN_SYS_FIND,	
			"(sysFindObject source criteria) -> list of objects\n\n"
			
			"criteria is a string that must specify one or more of the following categories:\n\n"
			
			"   *           Include all categories\n"
			"   b           Include beams\n"
			"   G           Include ONLY stargates\n"
			"   G:xyz;      Include ONLY stargate with ID 'xyz'\n"
			"   k           Include markers\n"
			"   m           Include missiles\n"
			"   s           Include ships\n"
			"   t           Include stations (including planets)\n"
			"   T           Include structure-scale stations\n"
			"   t:xyz;      Same as \"t +xyz;\"\n"
			"   T:xyz;      Same as \"T +xyz;\"\n"
			"   z           Include the player\n\n"
			
			"and may contain any number of the following options:\n\n"
			
			"   A           Active objects only (i.e., objects that can attack)\n"
			//"   B:xyz       Only objects with attribute 'xyz'\n"
			//"   C           (unused)\n"
			"   D:xyz;      Only objects with data 'xyz'\n"
			"   E           Enemies of the source only\n"
			"   F           Friends of the source only\n"
			"   H           Only objects whose base = source\n"
			"   I:angle;    Only objects that intersect line from source\n"
			"   J           Same sovereign as source\n"
			"   J:unid;     Sovereign unid = unid\n"
			"   K           Killed objects only (i.e., objects that cannot attack)\n"
			"   L:x-y;      Objects of level x to y.\n"
			"   M           Manufactured objects only (i.e., no planets or asteroids)\n"
			"   N           Return only the nearest object to the source\n"
			"   N:nn;       Return only objects within nn light-seconds\n"
			"   O:docked;   Ships that are currently docked at source\n"
			"   O:escort;   Ships ordered to escort source\n"
			"   O:guard;    Ships ordered to guard source\n"
			"   P           Only objects that can be detected (perceived) by source\n"
			//"   Q           (unused)\n"
			"   R           Return only the farthest object from the source\n"
			"   R:nn;       Return only objects greater than nn light-seconds away\n"
			"   S:sort;     Sort order ('d' = distance ascending; 'D' = distance descending\n"
			//"   U           (unused)\n"
			"   V           Include virtual objects\n"
			//"   W           (unused)\n"
			"   X           Only objects whose target is the source\n"
			"   Y           Only objects angry at (or enemies of) the source\n"
			"   Z           Exclude the player\n"
			"   +xyz;       Only objects with attribute 'xyz'\n"
			"   -xyz;       Exclude objects with attribute 'xyz'\n"
			"   =n;         Only objects of level n. You can also replace = with >, <, >=, or <=,\n"
			"				but they need to be escaped in XML.\n\n"
			
			"   +/-data:xyz;        Include only/exclude objects with data 'xyz'\n"
			"   +/-isPlanet:true;   Include only/exclude planets\n"
			"   +/-property:xyz;    Include only/exclude objects with property 'xyz'\n"
			"   +/-unid:xyz;        Include only/exclude objects with UNID 'xyz'\n\n"
			
			"Order doesn't matter as long as multi-character items end with semicolons.\n"
			"If the source is nil, the center of the system is used for position, "
			"and other criteria related to the source are ignored.",

			"is",	0,	},

		{	"sysFindObjectAtPos",			fnSystemFind,	FN_SYS_FIND_AT_POS,
			"(sysFindObjectAtPos source criteria pos [destPos]) -> list of objects",
			"isv*",	0,	},

		{	"sysGetData",					fnSystemGet,	FN_SYS_GET_DATA,
			"(sysGetData [nodeID] attrib) -> data",
			"s*",	0,	},

		{	"sysGetEnvironment",			fnSystemGet,	FN_SYS_ENVIRONMENT,
			"(sysGetEnvironment pos) -> environmentUNID",
			"v",	0,	},

		{	"sysGetItemBuyPrice",			fnSystemGet,	FN_SYS_ITEM_BUY_PRICE,
			"(sysGetItemBuyPrice [nodeID] item [typeCriteria]) -> price (or Nil)",
			"*v",	0,	},

		{	"sysGetLevel",					fnSystemGet,	FN_SYS_LEVEL,
			"(sysGetLevel [nodeID]) -> level",
			"*",	0,	},

		{	"sysGetLightIntensity",			fnSystemGet,	FN_SYS_LIGHT_INTENSITY,
			"(sysGetLightIntensity pos) -> intensity (0-100)",
			"v",	0,	},

		{	"sysGetName",					fnSystemGet,	FN_SYS_NAME,
			"(sysGetName [nodeID]) -> name",
			"*",	0,	},

		{	"sysGetNavPathPoint",			fnSystemGet,	FN_SYS_NAV_PATH_POINT,
			"(sysGetNavPathPoint sovereignID objFrom objTo %path) -> vector",
			"iiii",	0,	},

		{	"sysGetNode",					fnSystemGet,	FN_SYS_NODE,
			"(sysGetNode) -> nodeID",
			NULL,	0,	},

		{	"sysGetNodes",					fnSystemGet,	FN_SYS_ALL_NODES,
			"(sysGetNodes [criteria]) -> list of nodeIDs\n\n"
			
			"criteria:\n\n"
			
			"   knownOnly:True  Only nodes known to player\n"
			"   maxDist:n       Only nodes n or fewer gates away.\n"
			"   minDist:n       Only nodes n or more gates away.\n",

			"*",	0,	},

		{	"sysGetObjectByName",			fnSystemGetObjectByName,	0,
			"(sysGetObjectByName [source] name) -> obj",
			"*s",	0,	},

		{	"sysGetProperty",	fnSystemGet,	FN_SYS_GET_PROPERTY,
			"(sysGetProperty [nodeID] property) -> value\n\n"
			
			"property:\n\n"
			
			"   'lastVisitOn       Tick on which player last visited\n"
			"   'lastVisitSeconds  Game seconds since player last visited\n"
			"   'level             The level of the system\n"
			"   'name              The name of the system\n"
			"   'pos               Node position on map (x y)",

			"*s",	0,	},

		{	"sysGetPOV",					fnSystemGet,	FN_SYS_GET_POV,
			"(sysGetPOV) -> obj",
			NULL,	0,  },

		{	"sysGetRandomLocation",	fnSystemGet,	FN_SYS_RANDOM_LOCATION,
			"(sysGetRandomLocation criteria [options]) -> location or Nil\n\n"
			
			"options:\n\n"
			
			"   'objType           Type (UNID) of object to place (optional)\n"
			"   'remove            If True, remove location\n\n"
			
			"location:\n\n"
			
			"   'attribs           The attributes for the location\n"
			"   'orbit             The orbital parameters\n"
			"   'pos               The location position",

			"s*",	PPFLAG_SIDEEFFECTS,	},

		{	"sysGetStargateDestination",	fnSystemGet,	FN_SYS_STARGATE_DESTINATION,
			"(sysGetStargateDestination [nodeID] gateID) -> (nodeID gateID)",
			"s*",	0,	},

		{	"sysGetStargateDestinationNode",	fnSystemGet,	FN_SYS_STARGATE_DESTINATION_NODE,
			"(sysGetStargateDestinationNode [nodeID] gateID) -> nodeID",
			"s*",	0,	},

		{	"sysGetStargateProperty",		fnSystemGet,	FN_SYS_STARGATE_PROPERTY,
			"(sysGetStargateProperty [nodeID] gateID property) -> value\n\n"
			
			"property:\n\n"
			
			"   'destGateID: Destination gate ID\n"
			"   'destID: Destination node\n"
			"   'gateID: ID of this gate\n"
			"   'nodeID: NodeID of this gate\n"
			"   'uncharted: True if uncharted\n",

			"ss*",	0,	},

		{	"sysGetStargates",				fnSystemGet,	FN_SYS_STARGATES,
			"(sysGetStargates [nodeID]) -> list of gateIDs",
			"*",	0,	},

		{	"sysGetSystemType",				fnSystemGet,	FN_SYS_SYSTEM_TYPE,
			"(sysGetSystemType [nodeID]) -> systemUNID",
			"*",	0,	},

		{	"sysGetTopologyDistance",		fnSystemGet,	FN_SYS_TOPOLOGY_DISTANCE,
			"(sysGetTopologyDistance fromID toID) -> distance (or Nil)",
			"ss",	0,	},

		{	"sysHasAttribute",				fnSystemGet,	FN_SYS_HAS_ATTRIBUTE,
			"(sysHasAttribute [nodeID] attrib) -> True/Nil",
			"s*",	0,	},

		{	"sysHitScan",					fnSystemGet,	FN_SYS_HIT_SCAN,
			"(sysHitScan source startPos endPos [options]) -> (obj hitPos) or Nil\n\n"
				
			"options\n\n"
				
			"   'excludeWorlds",

			"ivv*",	0,	},

		{	"sysHitTest",					fnSystemGet,	FN_SYS_HIT_TEST,
			"(sysHitTest source pos [options]) -> obj or Nil\n\n"
				
			"options\n\n"
				
			"   'excludeWorlds\n"
			"   'sourceOnly",

			"iv*",	0,	},

		{	"sysIncData",					fnSystemGet,	FN_SYS_INC_DATA,
			"(sysIncData [nodeID] attrib increment) -> new value",
			"s*",	PPFLAG_SIDEEFFECTS,	},

		{	"sysIsKnown",					fnSystemGet,	FN_SYS_IS_KNOWN,
			"(sysIsKnown [nodeID]) -> True/Nil",
			"*",	0,	},

		{	"sysMatches",					fnSystemGet,	FN_SYS_MATCHES,
			"(sysMatches [nodeID] criteria) -> True/Nil",
			"*s",	0,	},

		{	"sysOrbit",						fnSystemGet,	FN_SYS_ORBIT_CREATE,
			"(sysOrbit center radius angle [eccentricity rotation]) -> orbit",

			"vnn*",	0,	},

		{	"sysOrbitPos",					fnSystemOrbit,	FN_SYS_ORBIT_POS,
			"(sysOrbitPos orbit [options]) -> vector\n\n"
			
			"Options parameter is a struct with the following fields:\n\n"
			
			"   'angleOffset:n              +/- n degrees along orbit arc\n"
			"   'arcOffset:n                +/- n light-seconds along orbit arc\n"
			"   'radiusOffset:n             +/- n light-seconds radius\n\n"
			
			"For arcOffset and radiusOffset, n may also be a list with the following"
			"formats:\n\n"
			
			"   (list 'gaussian min max)",

			"v*",	0,	},

		{	"sysPlaySound",					fnSystemCreate,	FN_SYS_PLAY_SOUND,
			"(sysPlaySound unid [sourceObj]) -> True/Nil",
			"i*",	0,	},

		{	"sysSetData",					fnSystemGet,	FN_SYS_SET_DATA,
			"(sysSetData [nodeID] attrib data) -> data",
			"sv*",	PPFLAG_SIDEEFFECTS,	},

		{	"sysSetEnvironment",			fnSystemCreate,		FN_SYS_CREATE_ENVIRONMENT,
			"(sysSetEnvironment unid shape options) -> True/Nil\n\n"

			"shape\n\n"

			"   'arc\n"
			"      Creates an arc along an orbit. Options must include the\n"
			"      following fields:\n\n"

			"         'orbit: This is the orbit to use. The arc will be\n"
			"            centered on the current orbit position (e.g., the\n"
			"            planet location).\n"
			"         'length: The length of the arc (in degrees).\n"
			"         'width: The width at the center of the arc (in light-\n"
			"            seconds).\n"
			"\n"
			"   'orbital\n"
			"      Creates a random environment along the orbit. Options\n"
			"      must include the following fields:\n\n"

			"         'orbit: This is the orbit to use.\n"
			"         'width: The average width of the ring, in light-\n"
			"            seconds.\n"
			"\n"
			"   'square\n"
			"      Creates a square patch. Options must include the\n"
			"         following fields:\n\n"

			"         'center: The center position of the patch.\n"
			"         'height: The height of the patch (in light-seconds).\n"
			"         'width: The width of the patch (in light-seconds).",

			"isv",	PPFLAG_SIDEEFFECTS,	},

		{	"sysSetKnown",					fnSystemGet,	FN_SYS_SET_KNOWN,
			"(sysSetKNown [nodeID] [True/Nil]) -> True/Nil",
			"*",	PPFLAG_SIDEEFFECTS,	},

		{	"sysSetPOV",					fnSystemGet,	FN_SYS_SET_POV,
			"(sysSetPOV obj|vector) -> True/Nil",
			"v",	PPFLAG_SIDEEFFECTS,	},

		{	"sysSetProperty",	fnSystemGet,	FN_SYS_SET_PROPERTY,
			"(sysSetProperty [nodeID] property value) -> True/Nil\n\n"
			
			"property:\n\n"
			
			"   'pos               Node position on map (x y)",

			"*sv",	PPFLAG_SIDEEFFECTS,	},

		{	"sysStartTime",					fnSystemMisc,	FN_SYS_START_TIME,
			"(sysStartTime) -> True/Nil",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"sysStopTime",					fnSystemMisc,	FN_SYS_STOP_TIME,
			"(sysStopTime duration except) -> True/Nil\n"
			"(sysStopTime targetList duration) -> True/Nil",
			"vv",	PPFLAG_SIDEEFFECTS,	},

		{	"sysVectorAdd",					fnSystemVectorMath,		FN_VECTOR_ADD,	
			"(sysVectorAdd vector vector) -> vector",
			"vv",	0,	},

		{	"sysVectorAngle",				fnSystemVectorMath,		FN_VECTOR_ANGLE,	
			"(sysVectorAngle vector) -> angle of vector\n"
			"(sysVectorAngle pos1 pos2) -> angle of pos1 relative to pos2",
			"v*",	0,	},

		{	"sysVectorDistance",			fnSystemVectorMath,		FN_VECTOR_DISTANCE,
			"(sysVectorDistance vector [vector]) -> distance in light-seconds (int32)",
			"v*",	0,	},

		{	"sysVectorDistanceExact",		fnSystemVectorMath,		FN_VECTOR_DISTANCE_EXACT,
			"(sysVectorDistanceExact vector [vector]) -> distance in light-seconds or speed as a fraction of c (real)",
			"v*",	0,	},

		{	"sysVectorDivide",				fnSystemVectorMath,		FN_VECTOR_DIVIDE,
			"(sysVectorDivide vector scalar) -> vector",
			"vn",	0,	},

		{	"sysVectorMultiply",			fnSystemVectorMath,		FN_VECTOR_MULTIPLY,
			"(sysVectorMultiply vector scalar) -> vector",
			"vn",	0,	},

		{	"sysVectorPixelOffset",			fnSystemVectorMath,		FN_VECTOR_PIXEL_OFFSET,
			"(sysVectorPixelOffset center x y) -> vector\n\n"

			"center is either Nil, an object, or a vector",

			"vnn",	0,	},

		{	"sysVectorPolarOffset",			fnSystemVectorOffset,	0,
			"(sysVectorPolarOffset center angle radius) -> vector\n\n"

			"center is either Nil, an object, or a vector\n"
			"radius in light-seconds",

			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"sysVectorPolarVelocity",			fnSystemVectorMath,		FN_VECTOR_POLAR_VELOCITY,
			"(sysVectorPolarVelocity angle speed) -> velVector",
			"nn",	0,	},

		{	"sysVectorRandom",				fnSystemVectorMath,		FN_VECTOR_RANDOM,
			"(sysVectorRandom center radius minSeparation [filter]) -> vector\n\n"

			"center is either Nil, an object, or a vector\n"
			"radius in light-seconds from center (or a function)\n"
			"minSeparation is the min distance from other objects (in light-seconds)\n"
			"filter defines the set of objects to be away from",

			"vv*",	0,	},

		{	"sysVectorSpeed",			fnSystemVectorMath,		FN_VECTOR_SPEED,
			"(sysVectorSpeed velVector) -> % of light speed",
			"v",	0,	},

		{	"sysVectorSubtract",			fnSystemVectorMath,		FN_VECTOR_SUBTRACT,
			"(sysVectorSubtract vector vector) -> vector",
			"vv",	0,	},

		//	Design Type functions
		//	---------------------

		{	"typAddTimerEvent",				fnDesignGet,		FN_DESIGN_ADD_TIMER,
			"(typAddTimerEvent unid delay event)\n\n"
			
			"delay in ticks",

			"iis",	PPFLAG_SIDEEFFECTS,	},

		{	"typAddRecurringTimerEvent",	fnDesignGet,		FN_DESIGN_ADD_RECURRING_TIMER,	
			"(typAddRecurringTimerEvent unid interval event)\n\n"
			
			"interval in ticks",

			"iis",	PPFLAG_SIDEEFFECTS,	},

		{	"typCancelTimerEvent",				fnDesignGet,	FN_DESIGN_CANCEL_TIMER,
			"(typCancelTimerEvent unid event) -> True/Nil",
			"is",	0,	},

		{	"typCreate",					fnDesignCreate,		FN_DESIGN_CREATE,
			"(typCreate unid XML) -> True/Nil",
			"iv",	PPFLAG_SIDEEFFECTS,	},

		{	"typDynamicUNID",				fnDesignCreate,		FN_DESIGN_DYNAMIC_UNID,
			"(typDynamicUNID uniqueName) -> UNID",
			"s",	PPFLAG_SIDEEFFECTS,	},

		{	"typFind",						fnDesignFind,		0,
			"(typFind criteria) -> list of UNIDs\n\n"

			"criteria\n\n"
			
			"   *                  Include all Types\n"
			"   a                  AdventureDesc\n"
			"   b                  ItemTable\n"
			"   c                  EffectType\n"
			"   d                  DockScreen\n"
			"   e                  SpaceEnvironmentType\n"
			"   f                  OverlayType\n"
			"   h                  ShipTable\n"
			"   i                  ItemType\n"
			"   m                  Image\n"
			"   n                  MissionType\n"
			"   p                  Power\n"
			"   q                  SystemTable\n"
			"   s                  ShipClass\n"
			"   t                  StationType\n"
			"   T                  StationType (structure-scale)\n"
			"   u                  Sound\n"
			"   v                  Sovereign\n"
			"   x                  Type (generic)\n"
			"   y                  SystemType\n"
			"   z                  SystemMap\n"
			"   $                  EconomyType\n"
			"\n"
			"   L:x-y              Only Types of level x to y\n"
			"   V                  Include virtual types\n"
			"   +/-{attrib}        Require/exclude types with given attribute\n"
			"   +/-event:xyz;      Require/exclude types with given event\n"
			"   +/-isEnemyOf:xyz;  Require/exclude types which are enemy of sovereign\n"
			"   =n;                Level comparisons (also supports < etc.)",

			"s",	0,	},

		{	"typFireEvent",					fnDesignGet,		FN_DESIGN_FIRE_TYPE_EVENT,
			"(typFireEvent unid event [data]) -> result of event",
			"is*",	PPFLAG_SIDEEFFECTS,	},

		{	"typFireObjEvent",					fnDesignGet,		FN_DESIGN_FIRE_OBJ_EVENT,
			"(typFireObjEvent unid obj event) -> result of event",
			"iis",	PPFLAG_SIDEEFFECTS,	},

		{	"typGetData",				fnDesignGet,		FN_DESIGN_GET_GLOBAL_DATA,
			"(typGetData unid attrib) -> data",
			"is",	0,	},

		{	"typGetDataField",				fnDesignGet,		FN_DESIGN_GET_DATA_FIELD,
			"(typGetDataField unid field) -> data\n\n"
			
			"field (all):\n\n"
			"   'entity           entity name of this type\n"
			"   'extensionUNID    extension which defined this type\n"
			"   'name             class name of the type\n"
			"   'unid             UNID of this type\n"
			"   'version          extension API version\n\n"

			"field (armor):\n\n"
			"   'adjustedHP\n"
			"   'balance\n"
			"   'damageAdj\n"
			"   'effectiveHP\n"
			"   'hp\n"
			"   'hpBonus\n"
			"   'installCost\n"
			"   'regen\n"
			"   'repairCost\n"
			"   'shieldInterference\n\n"

			"field (drives):\n\n"
			"   'maxSpeed\n"
			"   'power\n"
			"   'thrust\n\n"

			"field (reactors):\n\n"
			"   'fuelCapacity\n"
			"   'fuelCriteria\n"
			"   'fuelEfficiency\n"
			"   'power\n\n"

			"field (shields):\n\n"
			"   'adjustedHp\n"
			"   'balance\n"
			"   'damageAdj\n"
			"   'effectiveHp\n"
			"   'hp\n"
			"   'hpBonus\n"
			"   'power\n"
			"   'regen\n"
			"   'weaponSuppress\n\n"

			"field (items):\n\n"
			"   'averageCount\n"
			"   'category\n"
			"   'cost\n"
			"   'description\n"
			"   'deviceSlots\n"
			"   'frequency\n"
			"   'fuelCapacity\n"
			"   'imageDesc\n"
			"   'installCost\n"
			"   'level\n"
			"   'mass\n"
			"   'shortName\n"
			"   'slotCategory\n"
			"   'treasureValue\n"
			"   'unknownType\n"
			"   'useKey\n\n"

			"field (player ships):\n\n"
			"   'dockServicesScreen  UNID of dock services screen\n"
			"   'playerDesc          Description for player\n"
			"   'shipConfigScreen    UNID of ship config screen\n"
			"   'shipStatusScreen    UNID of ship status screen\n"
			"   'startingSystem      Starting node\n\n"

			"field (ships):\n\n"
			"   'armorCount     Number of armor segments\n"
			"   'armorHP        HP of first armor segment\n"
			"   'armorItems\n"
			"   'balanceType    Integer 0 to 11 representing: Unknown Minion Standard Elite Boss NonCombatant TooWeak TooStrong ArmorTooWeak ArmorTooStrong WeaponsTooWeak WeaponsTooStrong\n"
			"   'cargoSpace\n"
			"   'combatStrength\n"
			"   'damage\n"
			"   'defenseStrength\n"
			"   'deviceSlots\n"
			"   'deviceSlotsNonWeapons\n"
			"   'deviceSlotsWeapons\n"
			"   'deviceItems\n"
			"   'dodgeRate\n"
			"   'driveImage\n"
			"   'drivePowerUse\n"
			"   'explosionType\n"
			"   'fireAccuracy\n"
			"   'fireRangeAdj\n"
			"   'fireRateAdj\n"
			"   'genericName\n"
			"   'hullMass\n"
			"   'installDeviceMaxLevel\n"
			"   'launcher\n"
			"   'launcherUNID\n"
			"   'level\n"
			"   'maneuver\n"
			"   'manufacturer\n"
			"   'mass\n"
			"   'maxArmorMass\n"
			"   'maxCargoSpace\n"
			"   'maxRotation\n"
			"   'maxSpeed\n"
			"   'maxStructuralHP\n"
			"   'primaryArmor\n"
			"   'primaryArmorUNID\n"
			"   'primaryWeapon\n"
			"   'primaryWeaponRange\n"
			"   'primaryWeaponRangeAdj\n"
			"   'primaryWeaponUNID\n"
			"   'score\n"
			"   'size\n"
			"   'shield\n"
			"   'shieldsUNID\n"
			"   'thrust\n"
			"   'thrustToWeight\n"
			"   'treasureValue\n"
			"   'wreckChance\n\n"

			"field (stations):\n\n"
			"   'abandonedDockScreen\n"
			"   'armorClass\n"
			"   'armorLevel\n"
			"   'balance\n"
			"   'canAttack\n"
			"   'category\n"
			"   'defenderStrength\n"
			"   'dockScreen\n"
			"   'explosionType\n"
			"   'fireRateAdj\n"
			"   'hitsToDestroy\n"
			"   'installDeviceMaxLevel\n"
			"   'hp\n"
			"   'level\n"
			"   'locationCriteria\n"
			"   'maxLightRadius\n"
			"   'name\n"
			"   'regen                 hp repaired per 180 ticks\n"
			"   'satelliteStrength\n"
			"   'size\n"
			"   'treasureBalance       100 = appropriate for defenses\n"
			"   'treasureValue\n"
			"   'weaponStrength        100 = level weapon @ 1/4 fire rate\n\n"

			"field (missions):\n\n"
			"   'level\n"
			"   'maxLevel\n"
			"   'minLevel\n\n"

			"",
			"is",	0,	},

		{	"typGetProperty",				fnDesignGet,		FN_DESIGN_GET_PROPERTY,
			"(typGetProperty unid property) -> value\n\n"
			
			"property (all):\n\n"
			
			"   'apiVersion\n"
			"   'attributes       List of type attributes\n"
			"   'class            Design type (e.g. ItemType etc.)\n"
			"   'extension        UNID of extension where type is defined\n"
			"   'mapDescription   Type description used for map labels\n"
			"   'merged           True if type inherits from another type\n"
			"   'namePattern\n"
			"   'obsoleteVersion  Type is obsolete at this API version\n\n"

			"property (sovereign):\n\n"

			"   'name\n"
			"   'playerThreatLevel     0 to 4 representing: None minorPiracy minorRaiding major existential\n"
			"   'plural\n"
			"   'shipsDestroyedByPlayer\n"
			"   'stationsDestroyedByPlayer\n\n"

			"property (ships):\n\n"

			"   'currency\n"
			"   'currencyName\n"
			"   'defaultSovereign\n"
			"   'drivePowerUse\n"
			"   'fuelCapacity\n"
			"   'fuelCapacityExact\n"
			"   'fuelCriteria\n"
			"   'fuelEfficiency\n"
			"   'fuelEfficiencyBonus\n"
			"   'hasTradeDesc\n"
			"   'hasVariants\n"
			"   'hullValue\n"
			"   'maxArmorMass\n"
			"   'maxSpeed\n"
			"   'maxSpeedAtMaxArmor\n"
			"   'maxSpeedAtMinArmor\n"
			"   'maxSpeedByArmorMass\n"
			"   'power\n"
			"   'stdArmorMass\n"
			"   'thrust\n"
			"   'thrustRatio\n"
			"   'thrustToWeight\n"
			"   'thrusterPower\n"
			"   'wreckStructuralHP\n\n"

			"property (stations):\n\n"

			"   'sovereign\n"
			"   'sovereignName\n\n"

			"NOTE: All data fields (accessed via typGetDataField) are also valid properties.",

			"is",	0,	},

		{	"typGetStaticData",				fnDesignGet,		FN_DESIGN_GET_STATIC_DATA,
			"(typGetStaticData unid attrib) -> data",
			"is",	0,	},

		{	"typGetXML",				fnDesignGet,		FN_DESIGN_GET_XML,
			"(typGetXML unid) -> xmlElement",
			"i",	0,	},

		{	"typHasAttribute",				fnDesignGet,		FN_DESIGN_HAS_ATTRIBUTE,
			"(typHasAttribute unid attrib) -> True/Nil",
			"is",	0,	},

		{	"typHasEvent",				fnDesignGet,		FN_DESIGN_HAS_EVENT,
			"(typHasEvent unid event) -> True/Nil",
			"is",	0,	},

		{	"typIncData",				fnDesignGet,		FN_DESIGN_INC_GLOBAL_DATA,
			"(typIncData unid attrib [increment]) -> new value",
			"is*",	0,	},

		{	"typMarkImages",				fnDesignGet,		FN_DESIGN_MARK_IMAGES,
			"(typMarkImages unid) -> True/Nil",
			"i",	0,	},

		{	"typMatches",				fnDesignGet,		FN_DESIGN_MATCHES,
			"(typMatches unid criteria) -> True/Nil",
			"is",	0,	},

		{	"typSetData",				fnDesignGet,		FN_DESIGN_SET_GLOBAL_DATA,
			"(typSetData unid attrib data) -> True/Nil",
			"isv",	PPFLAG_SIDEEFFECTS,	},

		{	"typTranslate",				fnDesignGet,		FN_DESIGN_TRANSLATE,
			"(typTranslate unid textID [data] [default]) -> text (or Nil)",
			"iv*",	0,	},

		//	Economy function
		//	----------------

		{	"ecoExchange",					fnCurrency,		FN_CURRENCY_EXCHANGE,
			"(ecoExchange amount fromCurrency toCurrency) -> amount",
			"ivv",	0,	},

		//	Environment functions
		//	---------------------

		//	Sovereign functions
		//	----------------

		{	"sovGetDisposition",			fnSovereignSet,			FN_SOVEREIGN_GET_DISPOSITION,
			"(sovGetDisposition sovereignID targetSovereignID) -> disposition of sovereign to target\n\n"

			"output is one of:\n\n"

			"   'enemy\n"
			"   'neutral\n"
			"   'friend",

			"ii",	0,	},

		{	"sovGetName",					fnSovereignGet,			FN_SOVEREIGN_NAME,
			"(sovGetName sovereignID [flags]) -> name",
			"i*",	0,	},

		{	"sovMessage",					fnSovereignSet,			FN_SOVEREIGN_MESSAGE,
			"(sovMessage sovereignID text) -> True/Nil",
			"iv",	0,	},

		{	"sovMessageFromObj",			fnSovereignSet,			FN_SOVEREIGN_MESSAGE_FROM_OBJ,
			"(sovMessageFromObj sovereignID obj text) -> True/Nil",
			"iiv",	0,	},

		{	"sovSetDisposition",			fnSovereignSet,			FN_SOVEREIGN_DISPOSITION,
			"(sovSetDisposition sovereignID targetSovereignID disposition)\n\n"

				"disposition:\n\n"

				"   0 / 'enemy\n"
				"   1 / 'neutral\n"
				"   2 / 'friend",

			"iiv",	PPFLAG_SIDEEFFECTS,	},

		//	Resource functions
		//	------------------

		{	"resColorBlend",				fnResourceGet,	FN_RESOURCE_COLOR_BLEND,
			"(resColorBlend rgbDest rgbSource srcOpacity) -> rgbColor",
			"ssn",	0,	},

		{	"resCreateImageDesc",			fnResourceGet,	FN_RESOURCE_CREATE_IMAGE_DESC,
			"(resCreateImageDesc imageUNID x y width height) -> imageDesc",
			"iiiii",	0,	},

		{	"resGetImageProperty",			fnResourceGet,	FN_RESOURCE_GET_IMAGE_PROPERTY,
			"(resGetImageProperty imageDesc property) -> value\n\n"
			
			"property\n\n"
			
			"   'height\n"
			"   'width",

			"vs",	0,	},

		//	Universe functions
		//	----------------

		{	"unvFindObject",					fnUniverseGet,		FN_UNIVERSE_FIND_OBJ,
			"(unvFindObject [nodeID] criteria) -> list of entries\n\n"

			"criteria\n\n"
			
			"   *                  Include all types\n"
			"   s                  ShipClass\n"
			"   t                  StationType\n"
			"   T                  StationType (structure-scale)\n"
			"\n"
			"   A                  Active objects only\n"
			"   K                  Killed objects only\n"
			"   L:x-y;             Objects of level x to y\n"
			"   V                  Include virtual objects\n"
			"   +/-{attrib}        Require/exclude types with given attribute\n"
			"   +/-unid:{unid}     Require/exclude types of given unid\n"
			"   =n;                Level comparisons (also supports < etc.)\n\n"

			"entry\n\n"
			
			"   ({objID} {type} {nodeID} {objName} {objNameFlags})",

			"*s",	0,	},

		{	"unvGetCurrentExtensionUNID",	fnUniverseGet,	FN_UNIVERSE_EXTENSION_UNID,
			"(unvGetCurrentExtensionUNID) -> UNID",
			NULL,	0,	},

		{	"unvGetElapsedGameTime",		fnUniverseGet,	FN_UNIVERSE_GET_ELAPSED_GAME_TIME,
			"(unvGetElapsedGameTime [startTick] endTick format) -> result\n\n"
			
			"format\n\n"
			
			"   display:           Elapsed time in display format.\n"
			"   seconds:           Elapsed time in game seconds.",

			"*is",	0,	},

		{	"unvGetExtensionData",			fnUniverseGet,	FN_UNIVERSE_GET_EXTENSION_DATA,
			"(unvGetExtensionData scope attrib) -> data",
			"ss",	0,	},

		{	"unvGetRealDate",				fnUniverseGet,	FN_UNIVERSE_REAL_DATE,
			"(unvGetRealDate) -> (year month day) GMT",
			NULL,	0,	},

		{	"unvGetTick",					fnUniverseGet,	FN_UNIVERSE_TICK,
			"(unvGetTick) -> time",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"unvSetExtensionData",			fnUniverseGet,	FN_UNIVERSE_SET_EXTENSION_DATA,
			"(unvSetExtensionData scope attrib data) -> True/Nil\n\n"

			"scope\n\n"

			"   'local\n"
			"   'serviceExtension\n"
			"   'serviceUser",

			"ssv",	0,	},

		{	"unvSetObjectKnown",			fnUniverseGet,	FN_UNIVERSE_SET_OBJECT_KNOWN,
			"(unvSetObjectKnown [nodeID] criteria [True/Nil]) -> True/Nil",
			"s*",	PPFLAG_SIDEEFFECTS,	},

		{	"unvUNID",						fnUniverseGet,	FN_UNIVERSE_UNID,
			"(unvUNID string) -> (unid 'itemtype name) or (unid 'shipclass name)",
			"s",	0,	},

		{	"unvEntity",						fnUniverseGet,	FN_UNIVERSE_ENTITY,
			"(unvEntity entity) -> unid\n"
			"(unvEntity unid) -> entity",
			"v",	0,	},

		//	XML functions
		//	-------------

		{	"xmlAppendSubElement",			fnXMLGet,		FN_XML_APPEND_SUB_ELEMENT,
			"(xmlAppendSubElement xml xmlToAdd [index]) -> True/Nil",
			"vv*",	PPFLAG_SIDEEFFECTS,	},

		{	"xmlAppendText",				fnXMLGet,		FN_XML_APPEND_TEXT,
			"(xmlAppendText xml text [index]) -> True/Nil",
			"vs*",	PPFLAG_SIDEEFFECTS,	},

		{	"xmlCreate",					fnXMLCreate,	0,
			"(xmlCreate xml) -> xml",
			"v",	0,	},

		{	"xmlDeleteSubElement",			fnXMLGet,		FN_XML_DELETE_SUB_ELEMENT,
			"(xmlDeleteSubElement xml index) -> True/Nil",
			"vi",	PPFLAG_SIDEEFFECTS,	},

		{	"xmlGetAttrib",					fnXMLGet,		FN_XML_GET_ATTRIB,
			"(xmlGetAttrib xml attrib) -> value",
			"vs",	0,	},

		{	"xmlGetAttribList",					fnXMLGet,		FN_XML_GET_ATTRIB_LIST,
			"(xmlGetAttribList xml) -> list of attribs",
			"v",	0,	},

		{	"xmlGetSubElement",				fnXMLGet,		FN_XML_GET_SUB_ELEMENT,
			"(xmlGetSubElement xml tag|index) -> xml",
			"vv",	0,	},

		{	"xmlGetSubElementCount",				fnXMLGet,		FN_XML_GET_SUB_ELEMENT_COUNT,
			"(xmlGetSubElementCount xml) -> number of sub-elements",
			"v",	0,	},

		{	"xmlGetSubElementList",				fnXMLGet,		FN_XML_GET_SUB_ELEMENT_LIST,
			"(xmlGetSubElementList xml [tag]) -> list of xml",
			"v*",	0,	},

		{	"xmlGetText",					fnXMLGet,		FN_XML_GET_TEXT,
			"(xmlGetText xml index) -> text (index should usually be 0, i.e. before the first subelement)",
			"v*",	0,	},

		{	"xmlGetTag",					fnXMLGet,		FN_XML_GET_TAG,
			"(xmlGetTag xml) -> tag",
			"v",	0,	},

		{	"xmlSetAttrib",					fnXMLGet,		FN_XML_SET_ATTRIB,
			"(xmlSetAttrib xml attrib value) -> value",
			"vsv",	PPFLAG_SIDEEFFECTS,	},

		{	"xmlSetText",					fnXMLGet,		FN_XML_SET_TEXT,
			"(xmlSetText xml text [index]) -> True/Nil",
			"vs*",	PPFLAG_SIDEEFFECTS,	},

		//	DEPRECATED FUNCTIONS
		//	--------------------

		{	"objSetSovereign",				fnObjSetOld,		FN_OBJ_SOVEREIGN,
			"DEPRECATED: Use (objSetProperty obj 'sovereign sovereign) instead.",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objGetSovereign",				fnObjGetOld,		FN_OBJ_SOVEREIGN,
			"DEPRECATED: Use (objGetProperty obj 'sovereign) instead.",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objGetDestiny",				fnObjGetOld,		FN_OBJ_DESTINY,
			"DEPRECATED: Use (objGetProperty obj 'destiny) instead.",
			NULL,	0,	},

		{	"unvFindObj",					fnUniverseGet,		FN_UNIVERSE_FIND_OBJ,
			"DEPRECATED: Use unvFindObject instead.",
			"*s",	0,	},

		{	"objGetDefaultCurrency",		fnObjGet,		FN_OBJ_DEFAULT_CURRENCY,
			"DEPRECATED: Use (objGetProperty obj 'currency) instead.",
			"i",	0,	},

		{	"itmGetDefaultCurrency",		fnItemTypeGet,	FN_ITEM_DEFAULT_CURRENCY,
			"DEPRECATED: Use (itmGetProperty item 'currency) instead.",
			"v",	0,	},

		{	"shpCanInstallArmor",			fnShipSet,			FN_SHIP_CAN_INSTALL_ARMOR,
			"DEPRECATED: Use objCanInstallItem instead.",
			"iv*",	0,	},

		{	"shpCanInstallDevice",			fnShipSet,			FN_SHIP_CAN_INSTALL_DEVICE,
			"DEPRECATED: Use objCanInstallItem instead.",
			"iv",	0,	},

		{	"shpSetPlayerWingman",			fnShipSet,		FN_SHIP_PLAYER_WINGMAN,
			"DEPRECATED: Use (objSetProperty ship 'playerWingman ...) instead.",
			"i*",	PPFLAG_SIDEEFFECTS,	},

		{	"armGetHitPoints",				fnArmGet,		FN_ARM_HITPOINTS,
			"DEPRECATED: Use (itmGetProperty item 'maxHP) instead.",
			"v",	0,	},

		{	"armGetName",					fnArmGet,		FN_ARM_NAME,
			"DEPRECATED: Use itmGetName instead.",
			"v",	0,	},

		{	"objIsUnderAttack",				fnObjGetOld,		FN_OBJ_IS_UNDER_ATTACK,
			"DEPRECATED: Use objGetProperty instead.",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"armIsRadiationImmune",			fnArmGet,		FN_ARM_IS_RADIATION_IMMUNE,
			"DEPRECATED: Use itmGetProperty instead.",
			"v",	0,	},

		{	"objIsAbandoned",				fnObjGetOld,		FN_OBJ_IS_ABANDONED,
			"DEPRECATED: Use objGetProperty instead.",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"staGetMaxStructuralHP",		fnStationGet,		FN_STATION_MAX_STRUCTURAL_HP,
			"DEPRECATED: Use objGetProperty instead.",
			"i",	0,	},

		{	"staGetStructuralHP",			fnStationGetOld,	FN_STATION_STRUCTURAL_HP,
			"DEPRECATED: Use objGetProperty instead.",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"staSetStructuralHP",			fnStationSetOld,	FN_STATION_STRUCTURAL_HP,
			"DEPRECATED: Use objSetProperty instead.",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objIsDeviceEnabled",			fnObjGet,		FN_OBJ_IS_DEVICE_ENABLED,
			"DEPRECATED: Use objGetItemProperty instead.",
			"iv",	0,	},

		{	"objSetDevicePos",				fnObjSet,		FN_OBJ_DEVICE_POS,
			"DEPRECATED: Use objSetItemProperty instead.",
			"ivv*",	PPFLAG_SIDEEFFECTS,	},

		{	"objGetDevicePos",				fnObjGet,		FN_OBJ_DEVICE_POS,
			"DEPRECATED: Use objGetItemProperty instead.",
			"iv",	0,	},

		{	"objGetDeviceFireArc",			fnObjGet,		FN_OBJ_DEVICE_FIRE_ARC,
			"DEPRECATED: Use objGetItemProperty instead.",
			"iv",	0,	},

		{	"objGetDeviceLinkedFireOptions",	fnObjGet,		FN_OBJ_DEVICE_LINKED_FIRE_OPTIONS,
			"DEPRECATED: Use objGetItemProperty instead.",
			"iv",	0,	},

		{	"objSetDeviceFireArc",			fnObjSet,		FN_OBJ_DEVICE_FIRE_ARC,
			"DEPRECATED: Use objSetItemProperty instead.",
			"ivv*",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetDeviceLinkedFireOptions",	fnObjSet,		FN_OBJ_DEVICE_LINKED_FIRE_OPTIONS,
			"DEPRECATED: Use objSetItemProperty instead.",
			"ivv",	PPFLAG_SIDEEFFECTS,	},

		{	"itmAtCursor",					fnItemList,		0,
			"DEPRECATED",
			"v",	0,	},

		{	"itmGetCharges",				fnItemGet,		FN_ITEM_CHARGES,
			"DEPRECATED: Use (itmGetProperty item 'charges) instead.",
			"v",	0,	},

		{	"itmGetGlobalData",				fnItemGet,		FN_ITEM_GET_GLOBAL_DATA,
			"DEPRECATED: Use itemGetTypeData instead.",
			"vs",	0,	},

		{	"itmSetGlobalData",				fnItemGet,		FN_ITEM_SET_GLOBAL_DATA,
			"DEPRECATED: Use itmSetTypeData instead.",
			"vsv",	0,	},

		{	"objGetGlobalData",				fnObjData,		FN_OBJ_GET_GLOBAL_DATA,
			"DEPRECATED: Use objGetTypeData instead.",
			NULL,	0,	},

		{	"objSetGlobalData",				fnObjData,		FN_OBJ_SET_GLOBAL_DATA,
			"DEPRECATED: Use objSetTypeData instead.",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"typGetGlobalData",				fnDesignGet,		FN_DESIGN_GET_GLOBAL_DATA,
			"DEPRECATED: Use typGetData instead.",
			"is",	0,	},

		{	"typIncGlobalData",				fnDesignGet,		FN_DESIGN_INC_GLOBAL_DATA,
			"DEPRECATED: Use typIncData instead.",
			"is*",	0,	},

		{	"typSetGlobalData",				fnDesignGet,		FN_DESIGN_SET_GLOBAL_DATA,
			"DEPRECATED: Use typSetData instead.",
			"isv",	PPFLAG_SIDEEFFECTS,	},

		{	"itmGetUNID",					fnItemGet,		FN_ITEM_UNID,
			"DEPRECATED: Use itemGetType instead",
			"v",	0,	},

		{	"itmHasModifier",				fnItemGet,		FN_ITEM_HAS_MODIFIER,
			"DEPRECATED: Use itmHasAttribute instead.",
			"vs",	0,	},

		{	"itmIsDamaged",					fnItemGet,		FN_ITEM_DAMAGED,
			"DEPRECATED: Use (itmGetProperty item 'damaged) instead",
			"v",	0,	},

		{	"itmSetCharges",				fnItemSet,		FN_ITEM_CHARGES,
			"DEPRECATED: Use (itmSetProperty item 'charges ...) instead",
			"vi",	0,	},

		{	"itmSetDamaged",				fnItemSet,		FN_ITEM_DAMAGED,
			"DEPRECATED: Use (itmSetProperty item 'damaged ...) instead",
			"v*",	0,	},

		{	"shpAddEnergyField",			fnShipSetOld,		FN_SHIP_ADD_ENERGY_FIELD,
			"DEPRECATED: Use objAddOverlay instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpDamageItem",				fnShipSetOld,		FN_SHIP_DAMAGE_ITEM,
			"DEPRECATED: Use (objSetItemProperty obj item 'damaged ...) instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpEnhanceSRS",				fnShipGetOld,		FN_SHIP_ENHANCE_SRS,
			"DEPRECATED: Use objChangeEquipmentStatus instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpFixBlindness",				fnShipSet,			FN_SHIP_FIX_BLINDNESS,
			"DEPRECATED: Use objChangeEquipmentStatus instead",
			"i*",	PPFLAG_SIDEEFFECTS,	},

		{	"shpGetClass",					fnShipGetOld,		FN_SHIP_CLASS,
			"DEPRECATED: Use objGetType instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpGetDataField",					fnShipClass,		FN_SHIP_DATA_FIELD,
			"DEPRECATED: Use typGetDataField instead",
			"is",	0,	},

		{	"shpGetGlobalData",				fnShipClass,	FN_SHIP_GET_GLOBAL_DATA,
			"DEPRECATED: Use typGetGlobalData instead",
			"is",	0,	},

		{	"shpGetItemCharges",			fnShipSetOld,		FN_SHIP_ITEM_CHARGES,
			"DEPRECATED: Use itmGetCharges instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objIncItemCharges",			fnObjSet,		FN_OBJ_INC_ITEM_CHARGES,
			"DEPRECATED: Use (objSetItemProperty obj item 'incCharges ...) instead",
			"iv*",	PPFLAG_SIDEEFFECTS,	},

		{	"shpInstallAutopilot",			fnShipGetOld,		FN_SHIP_INSTALL_AUTOPILOT,
			"DEPRECATED: Use objChangeEquipmentStatus instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpInstallTargetingComputer",	fnShipGetOld,		FN_SHIP_INSTALL_TARGETING,
			"DEPRECATED: Use objChangeEquipmentStatus instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpIsAutopilotInstalled",		fnShipGetOld,		FN_SHIP_HAS_AUTOPILOT,
			"DEPRECATED: Use objGetEquipmentStatus instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpIsRadioactive",				fnShipGetOld,		FN_SHIP_IS_RADIOACTIVE,
			"DEPRECATED: Use objIsRadioactive instead",
			NULL,	0,	},

		{	"shpIsSRSEnhanced",				fnShipGetOld,		FN_SHIP_IS_SRS_ENHANCED,
			"DEPRECATED: Use objGetEquipmentStatus instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpIsTargetingComputerInstalled",		fnShipGetOld,		FN_SHIP_HAS_TARGETING,
			"DEPRECATED: Use objGetEquipmentStatus instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpMakeBlind",					fnShipSetOld,		FN_SHIP_BLINDNESS,
			"DEPRECATED: Use objChangeEquipmentStatus instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"objMoveTo",					fnObjSetOld,		FN_OBJ_POSITION,
			"DEPRECATED: Use objSetPos instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpOrderAttack",				fnShipSetOld,		FN_SHIP_ORDER_ATTACK,
			"DEPRECATED: Use (shpOrder ship 'attack target) instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpOrderDock",					fnShipSetOld,		FN_SHIP_ORDER_DOCK,
			"DEPRECATED: Use (shpOrder ship 'dock target) instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpOrderEscort",				fnShipSetOld,		FN_SHIP_ORDER_ESCORT,
			"DEPRECATED: Use (shpOrder ship 'escort obj [formation]) instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpOrderFollow",				fnShipSetOld,		FN_SHIP_ORDER_FOLLOW,
			"DEPRECATED: Use (shpOrder ship 'follow target) instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpOrderGate",					fnShipSetOld,		FN_SHIP_ORDER_GATE,
			"DEPRECATED: Use (shpOrder ship 'gate [gate]) instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpOrderGoto",				fnShipSet,		FN_SHIP_ORDER_GOTO,
			"DEPRECATED: Use (shpOrder ship 'goto obj) instead",
			"ii",	PPFLAG_SIDEEFFECTS,	},

		{	"shpOrderGuard",				fnShipSetOld,		FN_SHIP_ORDER_GUARD,
			"DEPRECATED: Use (shpOrder ship 'guard target) instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpOrderHold",					fnShipSetOld,		FN_SHIP_ORDER_HOLD,
			"DEPRECATED: Use (shpOrder ship 'hold [time]) instead",
		//		wait time in seconds (real time)
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpOrderLoot",					fnShipSetOld,		FN_SHIP_ORDER_LOOT,
			"DEPRECATED: Use (shpOrder ship 'loot obj) instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpOrderMine",					fnShipSetOld,		FN_SHIP_ORDER_MINE,
			"DEPRECATED: Use (shpOrder ship 'mine baseObj) instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpOrderPatrol",				fnShipSetOld,		FN_SHIP_ORDER_PATROL,
			"DEPRECATED: Use (shpOrder ship 'patrol baseObj dist) instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpOrderWait",					fnShipSetOld,		FN_SHIP_ORDER_WAIT,
			"DEPRECATED: Use (shpOrder ship 'wait [time]) instead",
		//		wait time in seconds (real time)
		//			(0 = infinite)
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpRechargeItem",				fnShipSetOld,		FN_SHIP_RECHARGE_ITEM,
			"DEPRECATED: Use (objSetItemProperty obj item 'incCharges ...) instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shpSetGlobalData",				fnShipClass,	FN_SHIP_SET_GLOBAL_DATA,
			"DEPRECATED: Use typSetGlobalData instead",
			"isv",	PPFLAG_SIDEEFFECTS,	},

		{	"objSetItemCharges",			fnObjSet,		FN_OBJ_SET_ITEM_CHARGES,
			"DEPRECATED: Use (objSetItemProperty obj item 'charges ...) instead",
			"ivi*",	PPFLAG_SIDEEFFECTS,	},

		{	"objGetStaticDataForStationType",fnObjData,		FN_OBJ_GET_STATIC_DATA_FOR_STATION_TYPE,
			"DEPRECATED: Use typGetStaticData instead.",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"staGetGlobalData",				fnStationType,	FN_STATION_GET_GLOBAL_DATA,	
			"DEPRECATED: Use typGetGlobalData instead",
			"is",	0,	},

		{	"staGetType",					fnStationGetOld,	FN_STATION_GET_TYPE,
			"DEPRECATED: Use objGetType instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"staSetGlobalData",				fnStationType,	FN_STATION_SET_GLOBAL_DATA,	
			"DEPRECATED: Use typSetGlobalData instead",
			"isv",	PPFLAG_SIDEEFFECTS,	},

		{	"staSetInactive",				fnStationGetOld,	FN_STATION_SET_INACTIVE,
			"DEPRECATED: Use staSetActive instead",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"envHasAttribute",				fnEnvironmentGet,		FN_ENV_HAS_ATTRIBUTE,
			"DEPRECATED: Use typHasAttribute instead",
			"is",	0,	},
	};

#define EXTENSIONS_COUNT		(sizeof(g_Extensions) / sizeof(g_Extensions[0]))

ALERROR CUniverse::InitCodeChainPrimitives (void)

//	InitCodeChainPrimitives
//
//	Registers extensions

	{
	ALERROR error;
	int i;

	//	Define some global variables (we do this because otherwise when we fire
	//	events, we may get undefined variable errors when trying to save
	//	old versions of the globals)

	m_CC.DefineGlobal(CONSTLIT("gPlayer"), m_CC.CreateNil());
	m_CC.DefineGlobal(CONSTLIT("gPlayerShip"), m_CC.CreateNil());
	m_CC.DefineGlobal(CONSTLIT("gSource"), m_CC.CreateNil());
	m_CC.DefineGlobal(CONSTLIT("gItem"), m_CC.CreateNil());
	m_CC.DefineGlobal(CONSTLIT("gType"), m_CC.CreateNil());

	//	Register primitives

	for (i = 0; i < EXTENSIONS_COUNT; i++)
		if (error = m_CC.RegisterPrimitive(&g_Extensions[i]))
			return error;

	return NOERROR;
	}

ICCItem *fnArmGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnArmGet
//
//	Gets data about an armor type
//
//	(armGetName type) -> Name of the armor
//	(armGetHitPoints type) -> Hit points of armor
//	(armGetRepairCost type) -> Cost to repair 1 hit point

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Convert the first argument into an armor type

	CItemType *pType = GetItemTypeFromArg(*pCC, pArgs->GetElement(0));
	if (pType == NULL)
		return pCC->CreateError(CONSTLIT("Invalid armor type"), pArgs->GetElement(0));

	CArmorClass *pArmor = pType->GetArmorClass();
	if (pArmor == NULL)
		return pCC->CreateError(CONSTLIT("Invalid armor type"), pArgs->GetElement(0));

	CItem ArmorItem(pType, 1);

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_ARM_NAME:
			return pCC->CreateString(pArmor->GetName());

		case FN_ARM_HITPOINTS:
			return pCC->CreateInteger(pArmor->GetMaxHP(CItemCtx(&ArmorItem)));

		case FN_ARM_REPAIRCOST:
			return pCC->CreateInteger(pArmor->GetRepairCost(CItemCtx(&ArmorItem)));

		case FN_ARM_REPAIRTECH:
			return pCC->CreateInteger(pArmor->GetRepairTech());

		case FN_ARM_IS_RADIATION_IMMUNE:
			return pCC->CreateBool(pArmor->IsImmune(CItemCtx(&ArmorItem), specialRadiation));

		default:
			ASSERT(FALSE);
			return pCC->CreateNil();
		}
	}

ICCItem *fnCurrency (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnCurrency
//
//	Currency functions

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Implement

	switch (dwData)
		{
		case FN_CURRENCY_EXCHANGE:
			{
			int iValue = pArgs->GetElement(0)->GetIntegerValue();

			CEconomyType *pEconFrom = GetEconomyTypeFromItem(*pCC, pArgs->GetElement(1));
			if (pEconFrom == NULL)
				return pCC->CreateError(CONSTLIT("Invalid economy type"), pArgs->GetElement(1));

			CEconomyType *pEconTo = GetEconomyTypeFromItem(*pCC, pArgs->GetElement(2));
			if (pEconTo == NULL)
				return pCC->CreateError(CONSTLIT("Invalid economy type"), pArgs->GetElement(2));

			//	Exchange

			return pCC->CreateInteger((int)pEconTo->Exchange(pEconFrom, iValue));
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

ICCItem *fnDebug (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnDebug
//
//	Debug functions

	{
	int i;
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	switch (dwData)
		{
		case FN_API_VERSION:
			return pCC->CreateInteger(pCtx->GetAPIVersion());

		case FN_DEBUG_GET:
			{
			CString sProperty = pArgs->GetElement(0)->GetStringValue();
			ICCItemPtr pResult = g_pUniverse->GetDebugProperty(sProperty);
			return pResult->Reference();
			}

		case FN_DEBUG_IS_ACTIVE:
			return g_pUniverse->InDebugMode() ? pCC->CreateTrue() : pCC->CreateNil();

		case FN_DEBUG_SET:
			{
			CString sProperty = pArgs->GetElement(0)->GetStringValue();
			ICCItem *pValue = pArgs->GetElement(1);
			CString sError;
			if (!g_pUniverse->SetDebugProperty(sProperty, pValue, &sError))
				{
				if (sError.IsBlank())
					return pCC->CreateError(CONSTLIT("Invalid property"), pValue);
				else
					return pCC->CreateError(sError);
				}

			return pCC->CreateTrue();
			}

		case FN_DEBUG_OUTPUT:
		case FN_DEBUG_LOG:
		case FN_PRINT:
		case FN_PRINT_TO:
			{
			const DWORD OUTPUT_CONSOLE	= 0x00000001;
			const DWORD OUTPUT_LOG		= 0x00000002;

			//	Only in debug mode

			if ((dwData == FN_DEBUG_OUTPUT || dwData == FN_DEBUG_LOG)
					&& !g_pUniverse->InDebugMode())
				return pCC->CreateNil();

			//	Figure out where to output and when to start parsing for text 
			//	to output.

			DWORD dwOutput;
			int iStart;
			if (dwData == FN_DEBUG_OUTPUT || dwData == FN_PRINT)
				{
				dwOutput = OUTPUT_CONSOLE;
				iStart = 0;
				}
			else if (dwData == FN_DEBUG_LOG)
				{
				dwOutput = OUTPUT_LOG;
				iStart = 0;
				}
			else if (dwData == FN_PRINT_TO)
				{
				//	First argument is output options

				dwOutput = 0;
				ICCItem *pOptions = pArgs->GetElement(0);
				for (i = 0; i < pOptions->GetCount(); i++)
					{
					CString sOutput = pOptions->GetElement(i)->GetStringValue();
					if (strEquals(sOutput, CONSTLIT("console")))
						dwOutput |= OUTPUT_CONSOLE;
					else if (strEquals(sOutput, CONSTLIT("log")))
						dwOutput |= OUTPUT_LOG;
					else
						return pCC->CreateError(CONSTLIT("Invalid output option"), pOptions->GetElement(i));
					}

				iStart = 1;
				}
			else
				{
				ASSERT(false);
				return pCC->CreateNil();
				}

			//	Figure out what to return

			bool bReturnString = (dwData == FN_DEBUG_LOG);

			//	Append each of the arguments together

			CString sResult;
			for (i = iStart; i < pArgs->GetCount(); i++)
				{
				if (pArgs->GetElement(i)->IsList() && !pArgs->GetElement(i)->IsSymbolTable())
					sResult.Append(pCC->Unlink(pArgs->GetElement(i)));
				else
					sResult.Append(pArgs->GetElement(i)->Print(pCC, PRFLAG_ENCODE_FOR_DISPLAY));
				}

			//	Output to console or log

			if (dwOutput & OUTPUT_CONSOLE)
				g_pUniverse->GetHost()->ConsoleOutput(sResult);

			if (dwOutput & OUTPUT_LOG)
				kernelDebugLogString(sResult);

			//	Result

			if (bReturnString)
				return pCC->CreateString(sResult);
			else
				return pCC->CreateTrue();
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

ICCItem *fnDesignFind (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnDesignFind
//
//	(typFind criteria) -> list of UNIDs

	{
	int i;
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Parse the criteria

	CDesignTypeCriteria Criteria;
	if (CDesignTypeCriteria::ParseCriteria(pArgs->GetElement(0)->GetStringValue(), &Criteria) != NOERROR)
		return pCC->CreateError(CONSTLIT("Invalid design type criteria"), pArgs->GetElement(0));

	//	Create a list to return

	ICCItem *pResult = pCC->CreateLinkedList();
	if (pResult->IsError())
		return pResult;

	CCLinkedList *pList = (CCLinkedList *)pResult;

	//	Find

	for (i = 0; i < g_pUniverse->GetDesignTypeCount(); i++)
		{
		CDesignType *pType = g_pUniverse->GetDesignType(i);
		if (pType->MatchesCriteria(Criteria))
			{
			ICCItem *pUNID = pCC->CreateInteger(pType->GetUNID());
			pList->Append(*pCC, pUNID);
			pUNID->Discard(pCC);
			}
		}

	return pResult;
	}

ICCItem *fnDesignCreate (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnDesignCreate
//
//	(typCreate unid XML)

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	//	Implement

	switch (dwData)
		{
		case FN_DESIGN_CREATE:
			{
			DWORD dwUNID = pArgs->GetElement(0)->GetIntegerValue();

			//	Add it

			CString sError;
			if (g_pUniverse->AddDynamicType(pCtx->GetExtension(), 
					dwUNID, 
					pArgs->GetElement(1),
					pCtx->InEvent(eventOnGlobalTypesInit),
					&sError) != NOERROR)
				return pCC->CreateError(sError);

			//	Done

			return pCC->CreateTrue();
			}

		case FN_DESIGN_DYNAMIC_UNID:
			{
			CString sName = pArgs->GetElement(0)->GetStringValue();
			if (sName.IsBlank())
				return pCC->CreateError(CONSTLIT("Invalid dynamic name"), pArgs->GetElement(0));

			DWORD dwUNID = g_pUniverse->GetDesignCollection().GetDynamicUNID(sName);
			return pCC->CreateInteger((int)dwUNID);
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

ICCItem *fnDesignGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnDesignGet
//
//	Returns design data

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	//	The first argument is an UNID

	CDesignType *pType;
	if (pArgs->GetElement(0)->IsNil())
		return pCC->CreateNil();

	else if ((pType = g_pUniverse->FindDesignType(pArgs->GetElement(0)->GetIntegerValue())) == NULL)
		{
		if (dwData == FN_DESIGN_GET_PROPERTY 
				|| pArgs->GetElement(0)->GetIntegerValue() == 0)
			return pCC->CreateNil();
		else
			return pCC->CreateError(CONSTLIT("Unknown type"), pArgs->GetElement(0));
		}

	//	Implement

	switch (dwData)
		{
        case FN_DESIGN_ADD_TIMER:
        case FN_DESIGN_ADD_RECURRING_TIMER:
            {
			int iTime = pArgs->GetElement(1)->GetIntegerValue();
			if (iTime < 0 || (iTime == 0 && dwData == FN_DESIGN_ADD_RECURRING_TIMER))
				return pCC->CreateError(CONSTLIT("Invalid recurring time"), pArgs->GetElement(1));

			CString sEvent = pArgs->GetElement(2)->GetStringValue();

			CSystemEvent *pEvent;
			if (dwData == FN_DESIGN_ADD_TIMER)
				pEvent = new CTimedTypeEvent(
						g_pUniverse->GetTicks() + iTime,
						0,
						pType,
						sEvent);
			else
				pEvent = new CTimedTypeEvent(
						g_pUniverse->GetTicks() + mathRandom(0, iTime),
						iTime,
						pType,
						sEvent);

			g_pUniverse->AddEvent(pEvent);

			return pCC->CreateTrue();
            }

        case FN_DESIGN_CANCEL_TIMER:
			return pCC->CreateBool(g_pUniverse->CancelEvent(pType, pArgs->GetElement(1)->GetStringValue(), pCtx->InEvent(eventDoEvent)));

		case FN_DESIGN_FIRE_OBJ_EVENT:
			{
			CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			if (pObj == NULL)
				return pCC->CreateNil();

			CString sEvent = pArgs->GetElement(2)->GetStringValue();
			ICCItem *pData = (pArgs->GetCount() >= 4 ? pArgs->GetElement(3) : NULL);

			ICCItem *pResult;
			pType->FireObjCustomEvent(sEvent, pObj, pData, &pResult);
			return pResult;
			}

		case FN_DESIGN_FIRE_TYPE_EVENT:
			{
			CString sEvent = pArgs->GetElement(1)->GetStringValue();
			ICCItem *pData = (pArgs->GetCount() >= 3 ? pArgs->GetElement(2) : NULL);

			ICCItem *pResult;
			pType->FireCustomEvent(sEvent, eventDoEvent, pData, &pResult);
			return pResult;
			}

		case FN_DESIGN_GET_DATA_FIELD:
			return CreateResultFromDataField(*pCC, pType->GetDataField(pArgs->GetElement(1)->GetStringValue()));

		case FN_DESIGN_GET_GLOBAL_DATA:
			return pType->GetGlobalData(pArgs->GetElement(1)->GetStringValue())->Reference();

		case FN_DESIGN_GET_PROPERTY:
			return pType->GetProperty(*pCtx, pArgs->GetElement(1)->GetStringValue());

		case FN_DESIGN_GET_STATIC_DATA:
			return pType->GetStaticData(pArgs->GetElement(1)->GetStringValue())->Reference();

		case FN_DESIGN_GET_XML:
			{
			CXMLElement *pXML = pType->GetXMLElement();
			if (pXML == NULL)
				return pCC->CreateNil();

			return new CCXMLWrapper(pXML);
			}

		case FN_DESIGN_HAS_ATTRIBUTE:
			return pCC->CreateBool(pType->HasAttribute(pArgs->GetElement(1)->GetStringValue()));

		case FN_DESIGN_HAS_EVENT:
			return pCC->CreateBool(pType->FindEventHandler(pArgs->GetElement(1)->GetStringValue()));

		case FN_DESIGN_INC_GLOBAL_DATA:
			{
			CString sAttrib = pArgs->GetElement(1)->GetStringValue();
            ICCItem *pValue = (pArgs->GetCount() > 2 ? pArgs->GetElement(2) : NULL);
            return pType->IncGlobalData(sAttrib, pValue)->Reference();
			}

		case FN_DESIGN_MARK_IMAGES:
			{
			pType->MarkImages();
			return pCC->CreateTrue();
			}

		case FN_DESIGN_MATCHES:
			{
			CDesignTypeCriteria Criteria;
			if (CDesignTypeCriteria::ParseCriteria(pArgs->GetElement(1)->GetStringValue(), &Criteria) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid design type criteria"), pArgs->GetElement(1));

			return pCC->CreateBool(pType->MatchesCriteria(Criteria));
			}

		case FN_DESIGN_SET_GLOBAL_DATA:
			{
			CString sAttrib = pArgs->GetElement(1)->GetStringValue();
			if (sAttrib.IsBlank())
				return pCC->CreateNil();

			pType->SetGlobalData(sAttrib, pArgs->GetElement(2));
			return pCC->CreateTrue();
			}

		case FN_DESIGN_TRANSLATE:
			{
			//	Get parameters

			int iArg = 1;
			ICCItem *pText = pArgs->GetElement(iArg++);
			if (pText->IsNil())
				return pCC->CreateNil();

			CString sText = pText->GetStringValue();
			ICCItem *pData = NULL;
			if (pArgs->GetCount() > iArg)
				{
				if (pArgs->GetElement(iArg)->IsSymbolTable())
					pData = pArgs->GetElement(iArg++);
				}

			ICCItem *pDefault = NULL;
			if (pArgs->GetCount() > iArg)
				pDefault = pArgs->GetElement(iArg++);

			//	Translate

			ICCItem *pResult;
			if (!pType->Translate(NULL, sText, pData, &pResult) || pResult->IsNil())
				{
				if (pDefault)
					return pDefault->Reference();
				else
					return pCC->CreateNil();
				}

			return pResult;
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

ICCItem *fnEnvironmentGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnEnvironmentGet
//
//	Environment functions

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	The first argument is an environment UNID

	CSpaceEnvironmentType *pEnv = g_pUniverse->FindSpaceEnvironment(pArgs->GetElement(0)->GetIntegerValue());
	if (pEnv == NULL)
		return pCC->CreateNil();

	//	Implement

	switch (dwData)
		{
		case FN_ENV_HAS_ATTRIBUTE:
			return pCC->CreateBool(pEnv->HasLiteralAttribute(pArgs->GetElement(1)->GetStringValue()));

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

ICCItem *fnFormat (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnFormat
//
//	Format functions

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	//	Implement

	switch (dwData)
		{
		case FN_COMPOSE:
			{
			CString sText = pArgs->GetElement(0)->GetStringValue();
			ICCItem *pData = (pArgs->GetCount() > 1 ? pArgs->GetElement(1) : NULL);
			return pCC->CreateString(CLanguage::Compose(sText, pData));
			}

		case FN_CURRENCY:
			{
			CEconomyType *pEcon = GetEconomyTypeFromItem(*pCC, pArgs->GetElement(0));
			if (pEcon == NULL)
				return pCC->CreateError(CONSTLIT("Invalid economy type"), pArgs->GetElement(0));

			//	If no value then return the singular name for the currency

			if (pArgs->GetCount() == 1)
				return pCC->CreateString(pEcon->GetCurrencyNameSingular());

			//	If the second arg is an integer, then compose a string

			else if (pArgs->GetElement(1)->IsInteger())
				{
				int iValue = pArgs->GetElement(1)->GetIntegerValue();
				if (iValue == 1)
					return pCC->CreateString(strPatternSubst(CONSTLIT("1 %s"), pEcon->GetCurrencyNameSingular()));
				else
					return pCC->CreateString(strPatternSubst(CONSTLIT("%s %s"), strFormatInteger(iValue, -1, FORMAT_THOUSAND_SEPARATOR), pEcon->GetCurrencyNamePlural()));
				}

			//	If the second arg is Nil, then return singular

			else if (pArgs->GetElement(1)->IsNil())
				return pCC->CreateString(pEcon->GetCurrencyNameSingular());

			//	Otherwise, return plural
			
			else
				return pCC->CreateString(pEcon->GetCurrencyNamePlural());
			}

		case FN_NAME:
			{
			//	We accept either 3 or 4 arguments

			ICCItem *pName;
			DWORD dwNameFlags;
			int iCount;
			DWORD dwFlags;

			if (pArgs->GetCount() >= 4)
				{
				pName = pArgs->GetElement(0);
				dwNameFlags = (DWORD)pArgs->GetElement(1)->GetIntegerValue();
				iCount = pArgs->GetElement(2)->GetIntegerValue();
				dwFlags = pCtx->AsNameFlags(pArgs->GetElement(3));
				}
			else
				{
				pName = pArgs->GetElement(0);
				dwNameFlags = 0;
				iCount = pArgs->GetElement(1)->GetIntegerValue();
				dwFlags = pCtx->AsNameFlags(pArgs->GetElement(2));
				}

			//	Get a name pattern or a string.

			CString sName;
			if (pName->IsSymbolTable())
				{
				sName = pName->GetStringAt(CONSTLIT("pattern"));
				dwNameFlags = pName->GetIntegerAt(CONSTLIT("flags"));
				}
			else
				sName = pName->GetStringValue();

			//	Compose

			return pCC->CreateString(CLanguage::ComposeNounPhrase(sName, iCount, NULL_STR, dwNameFlags, dwFlags));
			}

        case FN_NUMBER:
            {
			if (pArgs->GetCount() == 1)
				return pCC->CreateString(CLanguage::ComposeNumber(CLanguage::numberInteger, pArgs->GetElement(0)->GetIntegerValue()));
			else
				{
				CLanguage::ENumberFormatTypes iFormat = CLanguage::ParseNumberFormat(pArgs->GetElement(0)->GetStringValue());
				if (iFormat == CLanguage::numberError)
					return pCC->CreateError(CONSTLIT("Unknown number format"), pArgs->GetElement(0));

				return pCC->CreateString(CLanguage::ComposeNumber(iFormat, pArgs->GetElement(1)));
				}
            }

		case FN_POWER:
			return pCC->CreateString(CLanguage::ComposeNumber(CLanguage::numberPower, pArgs->GetElement(0)->GetDoubleValue()));

		case FN_VERB:
			{
			CString sVerb = pArgs->GetElement(0)->GetStringValue();

			DWORD dwFlags = 0;
			ICCItem *pPlural = pArgs->GetElement(1);
			if (pPlural->IsInteger())
				dwFlags |= ((pPlural->GetIntegerValue() != 1) ? CLanguage::verbPluralize : 0);
			else
				dwFlags |= (pPlural->IsNil() ? 0 : CLanguage::verbPluralize);

			return pCC->CreateString(CLanguage::ComposeVerb(sVerb, dwFlags));
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

ICCItem *fnItemCreate (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	ItemCreate
//
//	Creates an item struct
//
//	(itmCreate type count) -> item struct

	{
	CCodeChain *pCC = pEvalCtx->pCC;


	CItemType *pType = GetItemTypeFromArg(*pCC, pArgs->GetElement(0));
	if (pType == NULL)
		return pCC->CreateError(CONSTLIT("Unknown item type"), pArgs->GetElement(0));

	CItem Item(pType, pArgs->GetElement(1)->GetIntegerValue());

	return CreateListFromItem(*pCC, Item);
	}

ICCItem *fnItemCreateByName (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	ItemCreateByName
//
//	Creates a random item struct
//
//	(itmCreateByName criteria name [count]) -> item

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	CString sCriteria = pArgs->GetElement(0)->GetStringValue();
	CString sName = pArgs->GetElement(1)->GetStringValue();
	int iCount = -1;
	if (pArgs->GetCount() > 2)
		iCount = pArgs->GetElement(2)->GetIntegerValue();

	//	Parse criteria

	CItemCriteria Criteria;
	CItem::ParseCriteria(sCriteria, &Criteria);

	//	Create the item

	CItem Item = CItem::CreateItemByName(sName, Criteria, true);
	if (iCount > 0)
		Item.SetCount(iCount);

	if (Item.GetType() == NULL)
		return pCC->CreateNil();
	else
		return CreateListFromItem(*pCC, Item);
	}

ICCItem *fnItemCreateRandom (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	ItemCreateRandom
//
//	Creates a random item struct
//
//	(itmCreateRandom criteria levelDistribution) -> item struct

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	CString sCriteria = pArgs->GetElement(0)->GetStringValue();
	CString sLevelFrequency = pArgs->GetElement(1)->GetStringValue();

	//	Parse criteria

	CItemCriteria Criteria;
	CItem::ParseCriteria(sCriteria, &Criteria);

	//	Create the item

	CItem Item;
	if (g_pUniverse->CreateRandomItem(Criteria, sLevelFrequency, &Item) != NOERROR)
		return pCC->CreateNil();

	return CreateListFromItem(*pCC, Item);
	}

ICCItem *fnItemGetTypes (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnItemGetTypes
//
//	Returns a list of items types that match criteria

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Get the criteria

	CItemCriteria Criteria;
	CItem::ParseCriteria(pArgs->GetElement(0)->GetStringValue(), &Criteria);

	//	Create a linked list for the result

	ICCItem *pResult = pCC->CreateLinkedList();
	if (pResult->IsError())
		return pResult;

	CCLinkedList *pList = (CCLinkedList *)pResult;

	//	Loop over the items

	for (int i = 0; i < g_pUniverse->GetItemTypeCount(); i++)
		{
		CItemType *pType = g_pUniverse->GetItemType(i);
		CItem Item(pType, 1);

		if (Item.MatchesCriteria(Criteria))
			{
			ICCItem *pItem = pCC->CreateInteger(pType->GetUNID());
			pList->Append(*pCC, pItem);
			pItem->Discard(pCC);
			}
		}

	if (pList->GetCount() == 0)
		{
		pList->Discard(pCC);
		return pCC->CreateNil();
		}
	else
		return pResult;
	}

ICCItem *fnItemGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnItemGet
//
//	Gets and sets items
//
//	(itmGetCount item)

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	ICCItem *pResult;

	//	Convert the first argument into an item

	CItem Item = GetItemFromArg(*pCC, pArgs->GetElement(0));
	CItemType *pType = Item.GetType();
	if (pType == NULL)
		return pCC->CreateNil();

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_ITEM_ACTUAL_PRICE:
			//	Avoid recursion

			if ((pCtx->InEvent(eventGetTradePrice) || pCtx->InEvent(eventGetGlobalPlayerPriceAdj))
					&& pCtx->GetItemType() == Item.GetType())
				pResult = pCC->CreateInteger(Item.GetRawPrice(true));
			else
				pResult = pCC->CreateInteger(Item.GetTradePrice(NULL, true));
			break;

		case FN_ITEM_ARMOR_INSTALLED_LOCATION:
			pResult = pCC->CreateInteger(Item.GetInstalled());
			break;

		case FN_ITEM_ARMOR_TYPE:
			{
			CArmorClass *pArmor = pType->GetArmorClass();
			if (pArmor)
				pResult = pCC->CreateInteger((int)pArmor->GetUNID());
			else
				pResult = pCC->CreateError(CONSTLIT("Item is not an armor segment"), pArgs->GetElement(0));

			break;
			}

		case FN_ITEM_CHARGES:
			pResult = pCC->CreateInteger(Item.GetCharges());
			break;

		case FN_ITEM_COUNT:
			pResult = pCC->CreateInteger(Item.GetCount());
			break;

		case FN_ITEM_DAMAGE_TYPE:
			{
			int iDamageType;
			CDeviceClass *pClass = pType->GetDeviceClass();
			if (pClass)
				iDamageType = pClass->GetDamageType(CItemCtx());
			else
				iDamageType = -1;

			if (iDamageType != -1)
				pResult = pCC->CreateInteger(iDamageType);
			else
				pResult = pCC->CreateNil();
			break;
			}

		case FN_ITEM_DATA:
			return Item.GetDataAsItem(pArgs->GetElement(1)->GetStringValue())->Reference();

		case FN_ITEM_IMAGE_DESC:
			pResult = CreateListFromImage(*pCC, pType->GetImage());
			break;

		case FN_ITEM_INSTALL_COST:
			{
			int iCost = pType->GetInstallCost(CItemCtx(Item));
			if (iCost == -1)
				return pCC->CreateNil();

			if (pArgs->GetCount() < 2 || pArgs->GetElement(1)->IsNil())
				return pCC->CreateInteger(iCost);
			else
				{
				CEconomyType *pEconFrom = pType->GetCurrencyType();

				CEconomyType *pEconTo = GetEconomyTypeFromItem(*pCC, pArgs->GetElement(1));
				if (pEconTo == NULL)
					return pCC->CreateError(CONSTLIT("Invalid economy type"), pArgs->GetElement(1));

				//	Exchange

				return pCC->CreateInteger((int)pEconTo->Exchange(pEconFrom, iCost));
				}
			break;
			}

		case FN_ITEM_INSTALL_POS:
			{
			if (!Item.IsInstalled())
				pResult = pCC->CreateNil();
			else
				pResult = pCC->CreateInteger(Item.GetInstalled());
			break;
			}

		case FN_ITEM_IS_EQUAL:
			{
			int i;
			CItem Item2 = GetItemFromArg(*pCC, pArgs->GetElement(1));

			//	Options

			DWORD dwFlags = 0;
			ICCItem *pOptions = (pArgs->GetCount() > 2 ? pArgs->GetElement(2) : NULL);
			if (pOptions)
				{
				for (i = 0; i < pOptions->GetCount(); i++)
					{
					ICCItem *pOption = pOptions->GetElement(i);
					if (strEquals(pOption->GetStringValue(), CONSTLIT("ignoreInstalled")))
						dwFlags |= CItem::FLAG_IGNORE_INSTALLED;
					}
				}

			pResult = pCC->CreateBool(Item.IsEqual(Item2, dwFlags));
			break;
			}

		case FN_ITEM_GET_STATIC_DATA:
			return pType->GetStaticData(pArgs->GetElement(1)->GetStringValue())->Reference();

		case FN_ITEM_GET_GLOBAL_DATA:
			return pType->GetGlobalData(pArgs->GetElement(1)->GetStringValue())->Reference();

		case FN_ITEM_GET_TYPE_DATA:
			{
			//	Because of backwards compatibility, if we only have one argument
			//	then we assume that we want type data.

			if (pArgs->GetCount() == 1)
				pResult = pCC->Link(pType->GetData(), 0, NULL);

			//	Otherwise, we get global data from the design type

			else
				return pType->GetGlobalData(pArgs->GetElement(1)->GetStringValue())->Reference();

			break;
			}

		case FN_ITEM_SET_GLOBAL_DATA:
			{
			CString sAttrib = pArgs->GetElement(1)->GetStringValue();
			if (sAttrib.IsBlank())
				{
				pResult = pCC->CreateNil();
				break;
				}

			pType->SetGlobalData(sAttrib, pArgs->GetElement(2));
			pResult = pCC->CreateTrue();
			break;
			}

		case FN_ITEM_LEVEL:
			pResult = pCC->CreateInteger(Item.GetLevel());
			break;

		case FN_ITEM_PRICE:
			{
			//	Avoid recursion

			int iPrice;
			if ((pCtx->InEvent(eventGetTradePrice) || pCtx->InEvent(eventGetGlobalPlayerPriceAdj))
					&& pCtx->GetItemType() == Item.GetType())
				iPrice = Item.GetRawPrice();
			else
				iPrice = Item.GetTradePrice();

			//	Convert currency, if necessary

			if (pArgs->GetCount() < 2 || pArgs->GetElement(1)->IsNil())
				pResult = pCC->CreateInteger(iPrice);
			else
				{
				CEconomyType *pEconFrom = pType->GetCurrencyType();

				CEconomyType *pEconTo = GetEconomyTypeFromItem(*pCC, pArgs->GetElement(1));
				if (pEconTo == NULL)
					return pCC->CreateError(CONSTLIT("Invalid economy type"), pArgs->GetElement(1));

				//	Exchange

				return pCC->CreateInteger((int)pEconTo->Exchange(pEconFrom, iPrice));
				}
			break;
			}

		case FN_ITEM_PROPERTY:
			return Item.GetItemProperty(*pCtx, CItemCtx(Item), pArgs->GetElement(1)->GetStringValue());

		case FN_ITEM_DAMAGED:
			pResult = pCC->CreateBool(Item.IsDamaged());
			break;

		case FN_ITEM_ENHANCED:
			if (Item.GetMods().IsNotEmpty())
				pResult = pCC->CreateInteger(Item.GetMods().AsDWORD());
			else if (Item.IsEnhanced())
				pResult = pCC->CreateTrue();
			else
				pResult = pCC->CreateNil();
			break;

		case FN_ITEM_INSTALLED:
			pResult = pCC->CreateBool(Item.IsInstalled());
			break;

		case FN_ITEM_KNOWN:
			pResult = pCC->CreateBool(pType->IsKnown());
			break;

		case FN_ITEM_REFERENCE:
			pResult = pCC->CreateBool(pType->ShowReference());
			break;

		case FN_ITEM_UNID:
			pResult = pCC->CreateInteger(pType->GetUNID());
			break;

		case FN_ITEM_MASS:
			pResult = pCC->CreateInteger(Item.GetMassKg());
			break;

		case FN_ITEM_MAX_APPEARING:
			pResult = pCC->CreateInteger(pType->GetNumberAppearing().GetMaxValue());
			break;

		case FN_ITEM_AVERAGE_APPEARING:
			pResult = pCC->CreateInteger(pType->GetNumberAppearing().GetAveValue());
			break;

		case FN_ITEM_NAME:
			{
			DWORD dwFlags;
			if (pArgs->GetCount() > 1)
				dwFlags = pCtx->AsNameFlags(pArgs->GetElement(1));
			else
				dwFlags = 0;

			//	If we're inside the GetName event, then don't recurse

			if (pCtx->InEvent(eventGetName) && pCtx->GetItemType() == Item.GetType())
				dwFlags |= nounNoEvent;

			pResult = pCC->CreateString(Item.GetNounPhrase(CItemCtx(), dwFlags));
			break;
			}

		case FN_ITEM_CATEGORY:
			pResult = pCC->CreateInteger(pType->GetCategory());
			break;

		case FN_ITEM_USE_SCREEN:
			{
			CItemType::SUseDesc UseDesc;
			if (pType->GetUseDesc(&UseDesc) && UseDesc.pScreenRoot)
				{
				//	We want the UNID, not the screen itself

				pResult = pCC->CreateInteger(UseDesc.pScreenRoot->GetUNID());
				}
			else
				pResult = pCC->CreateNil();
			break;
			}

		case FN_ITEM_SET_KNOWN:
			pType->SetKnown();
			pResult = pCC->CreateTrue();
			break;

		case FN_ITEM_SET_REFERENCE:
			pType->SetShowReference();
			pResult = pCC->CreateTrue();
			break;

		case FN_ITEM_MATCHES:
			{
			CString sCriteria = pArgs->GetElement(1)->GetStringValue();
			CItemCriteria Criteria;
			CItem::ParseCriteria(sCriteria, &Criteria);
			pResult = pCC->CreateBool(Item.MatchesCriteria(Criteria));
			break;
			}

		case FN_ITEM_HAS_MODIFIER:
			{
#ifdef DEBUG_HASMODIFIER
			::OutputDebugString("(hasModifier)\n");
#endif
			CString sCriteria = pArgs->GetElement(1)->GetStringValue();
			pResult = pCC->CreateBool(pType->HasLiteralAttribute(sCriteria));
			break;
			}

		default:
			ASSERT(FALSE);
		}

	return pResult;
	}

ICCItem *fnItemList (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnItemList
//
//	(itmAtCursor itemListCursor) -> item

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	return pArgs->GetElement(0)->Reference();
	}

ICCItem *fnItemSet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnItemSet
//
//	Sets data to an item

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Convert the first argument into an item

	CItem Item = GetItemFromArg(*pCC, pArgs->GetElement(0));
	CItemType *pType = Item.GetType();
	if (pType == NULL)
		return pCC->CreateNil();

	//	Set the values

	switch (dwData)
		{
		case FN_ITEM_CHARGES:
			Item.SetCharges(Max(0, pArgs->GetElement(1)->GetIntegerValue()));
			return CreateListFromItem(*pCC, Item);

		case FN_ITEM_COUNT:
			{
			int iCount = pArgs->GetElement(1)->GetIntegerValue();
			if (iCount > 0)
				{
				Item.SetCount(iCount);
				return CreateListFromItem(*pCC, Item);
				}
			else
				return pCC->CreateNil();
			}

		case FN_ITEM_DAMAGED:
			{
			bool bDamaged;
			if (pArgs->GetCount() > 1)
				bDamaged = !pArgs->GetElement(1)->IsNil();
			else
				bDamaged = true;

			Item.SetDamaged(bDamaged);
			return CreateListFromItem(*pCC, Item);
			}

		case FN_ITEM_DATA:
			{
			int iCount = (pArgs->GetCount() > 3 ? Max(0, pArgs->GetElement(3)->GetIntegerValue()) : -1);
			if (iCount == 0)
				return pCC->CreateNil();

			Item.SetData(pArgs->GetElement(1)->GetStringValue(), pArgs->GetElement(2));

			if (iCount != -1)
				Item.SetCount(iCount);

			return CreateListFromItem(*pCC, Item);
			}

		case FN_ITEM_ENHANCED:
			{
			CItemEnhancement Mods;
			CString sError;
			if (Mods.InitFromDesc(pArgs->GetElement(1), &sError) != NOERROR)
				return pCC->CreateError(sError, pArgs->GetElement(1));

			Item.AddEnhancement(Mods);
			return CreateListFromItem(*pCC, Item);
			}

		case FN_ITEM_FIRE_EVENT:
			{
			ICCItem *pResult;
			ICCItem *pData = (pArgs->GetCount() > 2 ? pArgs->GetElement(2) : NULL);
			Item.FireCustomEvent(CItemCtx(&Item), pArgs->GetElement(1)->GetStringValue(), pData, &pResult);
			return pResult;
			}

		case FN_ITEM_PROPERTY:
			{
			CString sError;
			CItemCtx ItemCtx(&Item);
			if (!Item.SetProperty(ItemCtx, pArgs->GetElement(1)->GetStringValue(), (pArgs->GetCount() > 2 ? pArgs->GetElement(2) : NULL), &sError))
				{
				if (sError.IsBlank())
					return pCC->CreateNil();
				else
					return pCC->CreateError(sError);
				}

			return CreateListFromItem(*pCC, Item);
			}

		default:
			ASSERT(false);
		}

	return pCC->CreateNil();
	}

ICCItem *fnItemTypeGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnItemTypeGet
//
//	(itmGetDefaultCurrency item|type)

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Convert the first argument into an item type

	CItemType *pType = GetItemTypeFromArg(*pCC, pArgs->GetElement(0));
	if (pType == NULL)
		return pCC->CreateError(CONSTLIT("Invalid item type"), pArgs->GetElement(0));

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_ITEM_DEFAULT_CURRENCY:
			return pCC->CreateInteger(pType->GetCurrencyType()->GetUNID());

		case FN_ITEM_FREQUENCY:
			if (pArgs->GetCount() > 1)
				return pCC->CreateInteger(pType->GetFrequencyByLevel(pArgs->GetElement(1)->GetIntegerValue()));
			else
				return pCC->CreateInteger(pType->GetFrequency());

		default:
			ASSERT(FALSE);
			return pCC->CreateNil();
		}
	}

ICCItem *fnItemTypeSet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnItemTypeSet
//
//	(itmSetKnown item)

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Convert the first argument into an item type

	CItemType *pType = GetItemTypeFromArg(*pCC, pArgs->GetElement(0));
	if (pType == NULL)
		return pCC->CreateError(CONSTLIT("Invalid item type"), pArgs->GetElement(0));

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_ITEM_TYPE_SET_KNOWN:
			{
			bool bSet = (pArgs->GetCount() >= 2 ? !pArgs->GetElement(1)->IsNil() : true);

			if (bSet)
				pType->SetKnown();
			else
				{
				//	If this type does not have an unknown type, then we cannot clear it

				if (pType->GetUnknownType() == NULL)
					return pCC->CreateNil();

				//	Otherwise, OK

				pType->ClearKnown();
				}

			return pCC->CreateTrue();
			}

		default:
			ASSERT(FALSE);
			return pCC->CreateNil();
		}
	}

ICCItem *fnObjAddRandomItems (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnObjAddRandomItems
//
//	Adds random items from a table

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	ICCItem *pArgs;

	//	Evaluate the arguments and validate them

	pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("iii"));
	if (pArgs->IsError())
		return pArgs;

	//	Convert the first argument into a space object

	CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0));
	if (pObj == NULL)
		{
		pArgs->Discard(pCC);
		return pCC->CreateNil();
		}

	//	Other args

	DWORD dwTableID = pArgs->GetElement(1)->GetIntegerValue();
	int iCount = pArgs->GetElement(2)->GetIntegerValue();

	//	Do it

	CItemListManipulator theList(pObj->GetItemList());
	SItemAddCtx Ctx(theList);
	Ctx.pSystem = pObj->GetSystem();
	Ctx.iLevel = (pObj->GetSystem() ? pObj->GetSystem()->GetLevel() : 1);
	Ctx.vPos = pObj->GetPos();

	CItemTable *pTable = g_pUniverse->FindItemTable(dwTableID);
	if (pTable == NULL)
		return pCC->CreateError(CONSTLIT("Item table not found"), pArgs->GetElement(1));

	for (int j = 0; j < iCount; j++)
		pTable->AddItems(Ctx);

	pObj->OnComponentChanged(comCargo);
	pObj->ItemsModified();
	pObj->InvalidateItemListAddRemove();

	//	Done

	pArgs->Discard(pCC);
	return pCC->CreateTrue();
	}

ICCItem *fnObjData (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnObjData
//
//	Deals with opaque object data

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	ICCItem *pArgs;
	ICCItem *pResult;

	//	Evaluate the arguments and validate them

	if (dwData == FN_OBJ_GETDATA 
			|| dwData == FN_OBJ_GET_OBJREF_DATA 
			|| dwData == FN_OBJ_GET_STATIC_DATA
			|| dwData == FN_OBJ_GET_STATIC_DATA_FOR_STATION_TYPE
			|| dwData == FN_OBJ_GET_GLOBAL_DATA)
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("is"));
	else if (dwData == FN_OBJ_SET_OBJREF_DATA)
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("isi"));
	else if (dwData == FN_OBJ_INCREMENT_DATA)
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("is*"));
	else
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("isv"));
	if (pArgs->IsError())
		return pArgs;

	//	Convert the first argument into a space object

	CSpaceObject *pObj = NULL;
	CStationType *pStationType = NULL;
	if (dwData == FN_OBJ_GET_STATIC_DATA_FOR_STATION_TYPE)
		{
		pStationType = g_pUniverse->FindStationType(pArgs->GetElement(0)->GetIntegerValue());
		if (pStationType == NULL)
			{
			pArgs->Discard(pCC);
			return pCC->CreateError(CONSTLIT("Unknown station type"), pArgs->GetElement(0));
			}
		}
	else
		{
		pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0));
		if (pObj == NULL)
			{
			pArgs->Discard(pCC);
			return pCC->CreateNil();
			}
		}

	//	Second argument is the attribute

	CString sAttrib = pArgs->GetElement(1)->GetStringValue();

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_OBJ_GETDATA:
			{
			pResult = pObj->GetData(sAttrib)->Reference();
			pArgs->Discard(pCC);
			break;
			}

		case FN_OBJ_GET_STATIC_DATA:
			{
			pResult = pObj->GetStaticData(sAttrib)->Reference();
			pArgs->Discard(pCC);
			break;
			}

		case FN_OBJ_GET_STATIC_DATA_FOR_STATION_TYPE:
			{
			pResult = pStationType->GetStaticData(sAttrib)->Reference();
			pArgs->Discard(pCC);
			break;
			}

		case FN_OBJ_GET_GLOBAL_DATA:
			{
			pResult = pObj->GetGlobalData(sAttrib)->Reference();
			pArgs->Discard(pCC);
			break;
			}

		case FN_OBJ_SETDATA:
			{
#ifdef DEBUG_SETDATA_CHECK
			if (g_pUniverse->InDebugMode())
				{
				//	Make sure this is not an object (use objSetObjRefData instead)

				if (CObject::IsValidPointer((CObject *)pArgs->GetElement(2)->GetIntegerValue()))
					{
					CSpaceObject *pTest = dynamic_cast<CSpaceObject *>((CObject *)pArgs->GetElement(2)->GetIntegerValue());
					if (pTest)
						{
						DebugBreak();
						pArgs->Discard(pCC);
						pResult = pCC->CreateError(CONSTLIT("Use objSetObjRefData for objects"), NULL);
						break;
						}
					}
				}
#endif
			pObj->SetData(sAttrib, pArgs->GetElement(2));

			//	Done

			pResult = pCC->CreateTrue();
			pArgs->Discard(pCC);
			break;
			}

		case FN_OBJ_SET_GLOBAL_DATA:
			{
			pObj->SetGlobalData(sAttrib, pArgs->GetElement(2));
			pResult = pCC->CreateTrue();
			pArgs->Discard(pCC);
			break;
			}

		case FN_OBJ_INCREMENT_DATA:
			{
			pResult = pObj->IncData(sAttrib, (pArgs->GetCount() > 2 ? pArgs->GetElement(2) : NULL))->Reference();
			pArgs->Discard(pCC);
			break;
			}

		case FN_OBJ_GET_OBJREF_DATA:
			{
			CSpaceObject *pRef = pObj->GetObjRefData(sAttrib);
			if (pRef)
				pResult = pCC->CreateInteger((int)pRef);
			else
				pResult = pCC->CreateNil();
			pArgs->Discard(pCC);
			break;
			}

		case FN_OBJ_SET_OBJREF_DATA:
			{
			if (!pArgs->GetElement(2)->IsNil())
				{
				CSpaceObject *pRef = CreateObjFromItem(*pCC, pArgs->GetElement(2));
				pObj->SetObjRefData(sAttrib, pRef);
				}
			else
				pObj->SetObjRefData(sAttrib, NULL);

			pResult = pCC->CreateTrue();
			pArgs->Discard(pCC);
			break;
			}

		default:
			ASSERT(FALSE);
		}

	return pResult;
	}

ICCItem *fnObjComms (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnObjComms
//
//	(objCommunicate obj senderObj msg [obj] [data])

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0));
	if (pObj == NULL)
		return pCC->CreateNil();

	CSpaceObject *pSender = CreateObjFromItem(*pCC, pArgs->GetElement(1));
	if (pSender == NULL)
		return pCC->CreateNil();

	int iMessage;
	if (pArgs->GetElement(2)->IsInteger())
		iMessage = pArgs->GetElement(2)->GetIntegerValue();
	else
		iMessage = GetMessageFromID(pArgs->GetElement(2)->GetStringValue());

	//	If this is not a built-in comms message, then see if it as the ID of a 
	//	comms message.

	if (iMessage == msgNone)
		{
		int iIndex = pObj->FindCommsMessage(pArgs->GetElement(2)->GetStringValue());
		if (iIndex == -1)
			return pCC->CreateNil();

		if (!pObj->IsCommsMessageValidFrom(pSender, iIndex))
			return pCC->CreateNil();

		//	Invoke the message

		pObj->CommsMessageFrom(pSender, iIndex);
		return pCC->CreateTrue();
		}

	//	Optional args

	CSpaceObject *pParam1 = NULL;
	if (pArgs->GetCount() > 3)
		pParam1 = CreateObjFromItem(*pCC, pArgs->GetElement(3));

	DWORD dwParam2 = 0;
	if (pArgs->GetCount() > 4)
		dwParam2 = (DWORD)pArgs->GetElement(4)->GetIntegerValue();

	//	Done

	DWORD dwResult = pSender->Communicate(pObj, (MessageTypes)iMessage, pParam1, dwParam2);
	if (dwResult == resNoAnswer)
		return pCC->CreateNil();
	else if (dwResult == resAck)
		return pCC->CreateTrue();
	else
		return pCC->CreateInteger(dwResult);
	}

ICCItem *fnObjEnumItems (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnObjEnumItems
//
//	(objEnumItems obj criteria item-var exp)

	{
	int i;
	CCodeChain *pCC = pEvalCtx->pCC;
	ICCItem *pArgs;
	ICCItem *pResult;
	ICCItem *pVar;
	ICCItem *pBody;
	ICCItem *pLocalSymbols;
	ICCItem *pOldSymbols;
	ICCItem *pError;
	int iVarOffset;

	//	Evaluate the arguments and validate them

	pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("isqu"));
	if (pArgs->IsError())
		return pArgs;

	CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0));
	if (pObj == NULL)
		{
		pArgs->Discard(pCC);
		return pCC->CreateNil();
		}

	CItemCriteria Criteria;
	CItem::ParseCriteria(pArgs->GetElement(1)->GetStringValue(), &Criteria);

	pVar = pArgs->GetElement(2);
	pBody = pArgs->GetElement(3);

	//	Setup the locals. We start by creating a local symbol table

	pLocalSymbols = pCC->CreateSymbolTable();
	if (pLocalSymbols->IsError())
		{
		pArgs->Discard(pCC);
		return pLocalSymbols;
		}

	//	Associate the enumaration variable

	pError = pLocalSymbols->AddEntry(pCC, pVar, pCC->CreateNil());
	if (pError->IsError())
		{
		pArgs->Discard(pCC);
		return pError;
		}

	pError->Discard(pCC);

	//	Setup the context

	if (pEvalCtx->pLocalSymbols)
		pLocalSymbols->SetParent(pEvalCtx->pLocalSymbols);
	else
		pLocalSymbols->SetParent(pEvalCtx->pLexicalSymbols);
	pOldSymbols = pEvalCtx->pLocalSymbols;
	pEvalCtx->pLocalSymbols = pLocalSymbols;

	//	Start with a default result

	pResult = pCC->CreateNil();

	//	Get the offset of the variable so we don't have to
	//	search for it all the time

	iVarOffset = pLocalSymbols->FindOffset(pCC, pVar);

	//	Generate a list of all the items

	CItemListManipulator ItemList(pObj->GetItemList());
	int iCount = 0;
	CItem *pTempList = new CItem [ItemList.GetCount()];
	while (ItemList.MoveCursorForward())
		{
		if (ItemList.GetItemAtCursor().MatchesCriteria(Criteria))
			pTempList[iCount++] = ItemList.GetItemAtCursor();
		}

	//	Loop over all items

	for (i = 0; i < iCount; i++)
		{
		//	Clean up the previous result

		pResult->Discard(pCC);

		//	Set the element

		ICCItem *pItem = CreateListFromItem(*pCC, pTempList[i]);
		pLocalSymbols->AddByOffset(pCC, iVarOffset, pItem);
		pItem->Discard(pCC);

		//	Eval

		pResult = pCC->Eval(pEvalCtx, pBody);
		if (pResult->IsError())
			break;
		}

	//	Clean up

	delete [] pTempList;
	pEvalCtx->pLocalSymbols = pOldSymbols;
	pLocalSymbols->Discard(pCC);

	//	Done

	pArgs->Discard(pCC);
	return pResult;
	}

ICCItem *fnObjGateTo (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnObjGateTo
//
//	(objGateTo obj node entryPoint)

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Evaluate the arguments and validate them

	ICCItem *pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("iss*"));
	if (pArgs->IsError())
		return pArgs;

	CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0));
	if (pObj == NULL)
		{
		pArgs->Discard(pCC);
		return pCC->CreateNil();
		}

	CString sNode = pArgs->GetElement(1)->GetStringValue();
	CString sEntryPoint = pArgs->GetElement(2)->GetStringValue();

	CEffectCreator *pEffect = NULL;
	if (pArgs->GetCount() > 3)
		{
		DWORD dwEffectUNID = pArgs->GetElement(3)->GetIntegerValue();
		pEffect = g_pUniverse->FindEffectType(dwEffectUNID);
		}

	pArgs->Discard(pCC);

	//	Find the node
	//
	//	NOTE: We cannot validate the entrypoint because it could be a named 
	//	object and we don't know the names until we load the system. Thus we 
	//	wait until the system is loaded to check.

	CTopologyNode *pNode = g_pUniverse->FindTopologyNode(sNode);
	if (pNode == NULL)
		return pCC->CreateError(strPatternSubst(CONSTLIT("Invalid node: %s"), sNode));

	//	Make sure we're allowed to gate

	if (pObj->IsPlayer() && !pObj->OnGateCheck(pNode, sEntryPoint, NULL))
		return pCC->CreateNil();

	//	Effect

	if (pEffect)
		pEffect->CreateEffect(pObj->GetSystem(),
				NULL,
				pObj->GetPos(),
				NullVector,
				0);

	//	Jump

	pObj->EnterGate(pNode, sEntryPoint, NULL);
	return pCC->CreateTrue();
	}

ICCItem *fnItemEnumTypes (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnItemEnumTypes
//
//	(itmEnumTypes criteria item-var exp)

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	ICCItem *pArgs;
	ICCItem *pResult;
	ICCItem *pVar;
	ICCItem *pBody;
	ICCItem *pLocalSymbols;
	ICCItem *pOldSymbols;
	ICCItem *pError;
	int iVarOffset;

	//	Evaluate the arguments and validate them

	pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("squ"));
	if (pArgs->IsError())
		return pArgs;

	CItemCriteria Criteria;
	CItem::ParseCriteria(pArgs->GetElement(0)->GetStringValue(), &Criteria);

	pVar = pArgs->GetElement(1);
	pBody = pArgs->GetElement(2);

	//	Setup the locals. We start by creating a local symbol table

	pLocalSymbols = pCC->CreateSymbolTable();
	if (pLocalSymbols->IsError())
		{
		pArgs->Discard(pCC);
		return pLocalSymbols;
		}

	//	Associate the enumaration variable

	pError = pLocalSymbols->AddEntry(pCC, pVar, pCC->CreateNil());
	if (pError->IsError())
		{
		pArgs->Discard(pCC);
		return pError;
		}

	pError->Discard(pCC);

	//	Setup the context

	if (pEvalCtx->pLocalSymbols)
		pLocalSymbols->SetParent(pEvalCtx->pLocalSymbols);
	else
		pLocalSymbols->SetParent(pEvalCtx->pLexicalSymbols);
	pOldSymbols = pEvalCtx->pLocalSymbols;
	pEvalCtx->pLocalSymbols = pLocalSymbols;

	//	Start with a default result

	pResult = pCC->CreateNil();

	//	Get the offset of the variable so we don't have to
	//	search for it all the time

	iVarOffset = pLocalSymbols->FindOffset(pCC, pVar);

	//	Loop over all items

	for (int i = 0; i < g_pUniverse->GetItemTypeCount(); i++)
		{
		CItemType *pType = g_pUniverse->GetItemType(i);
		CItem Item(pType, 1);

		if (Item.MatchesCriteria(Criteria))
			{
			//	Associate item list

			ICCItem *pItem = CreateListFromItem(*pCC, Item);
			pLocalSymbols->AddByOffset(pCC, iVarOffset, pItem);
			pItem->Discard(pCC);

			//	Clean up the previous result

			pResult->Discard(pCC);

			//	Eval

			pResult = pCC->Eval(pEvalCtx, pBody);
			if (pResult->IsError())
				break;
			}
		}

	//	Clean up

	pEvalCtx->pLocalSymbols = pOldSymbols;
	pLocalSymbols->Discard(pCC);

	//	Done

	pArgs->Discard(pCC);
	return pResult;
	}

ICCItem *fnObjGetArmor (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnObjGetArmor
//
//	Gets data about a ship's armor
//
//	(objGetArmorDamage obj armorSegment) -> damage to armor segment
//	(objRepairArmor obj armorSegment [hpToRepair])

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	ICCItem *pArgs;
	ICCItem *pResult;

	//	Evaluate the arguments and validate them

	pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("iv*"));
	if (pArgs->IsError())
		return pArgs;

	//	Convert the first argument into a space object

	CShip *pShip = CreateShipObjFromItem(*pCC, pArgs->GetElement(0));
	if (pShip == NULL)
		{
		pArgs->Discard(pCC);
		return pCC->CreateNil();
		}

	//	Second argument is the armor segment

	int iArmorSeg = -1;
	CInstalledArmor *pSection = NULL;
	if (pArgs->GetElement(1)->IsList())
		{
		CItem Item = CreateItemFromList(*pCC, pArgs->GetElement(1));
		if (Item.GetType() && Item.IsInstalled())
			{
			iArmorSeg = Item.GetInstalled();
			pSection = pShip->GetArmorSection(iArmorSeg);
			}
		else
			{
			pArgs->Discard(pCC);
			return pCC->CreateNil();
			}
		}
	else
		{
		iArmorSeg = pArgs->GetElement(1)->GetIntegerValue();
		if (iArmorSeg >= 0 && iArmorSeg < pShip->GetArmorSectionCount())
			pSection = pShip->GetArmorSection(iArmorSeg);
		if (pSection == NULL)
			{
			pArgs->Discard(pCC);
			return pCC->CreateNil();
			}
		}

	//	If we're repairing armor, then there may be a third parameter

	int iRepairHP = -1;
	if (pArgs->GetElement(2))
		iRepairHP = pArgs->GetElement(2)->GetIntegerValue();

	//	No longer needed

	pArgs->Discard(pCC);

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_OBJ_ARMOR_DAMAGE:
			pResult = pCC->CreateInteger(pSection->GetMaxHP(pShip) - pSection->GetHitPoints());
			break;

		case FN_OBJ_ARMOR_ITEM:
			{
			CItemListManipulator ItemList(pShip->GetItemList());
			pShip->SetCursorAtArmor(ItemList, iArmorSeg);
			if (ItemList.IsCursorValid())
				pResult = CreateListFromItem(*pCC, ItemList.GetItemAtCursor());
			else
				pResult = pCC->CreateNil();
			break;
			}

		case FN_OBJ_ARMOR_NAME:
			{
			CString sName = pShip->GetClass()->GetHullSectionName(iArmorSeg);
			pResult = (!sName.IsBlank() ? pCC->CreateString(sName) : pCC->CreateNil());
			break;
			}

		case FN_OBJ_REPAIR_ARMOR:
			{
			int iRepaired;
			pShip->RepairArmor(iArmorSeg, iRepairHP, &iRepaired);
			pResult = pCC->CreateInteger(iRepaired);
			break;
			}

		case FN_OBJ_ARMOR_MAX_HP:
			pResult = pCC->CreateInteger(pSection->GetMaxHP(pShip));
			break;

		default:
			ASSERT(FALSE);
		}

	return pResult;
	}

ICCItem *fnObjIDGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnObjIDGet
//
//	Gets data from an object ID

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	switch (dwData)
		{
		case FN_OBJ_OBJECT:
			{
			CSpaceObject *pObj = g_pUniverse->FindObject(pArgs->GetElement(0)->GetIntegerValue());
			if (pObj)
				return pCC->CreateInteger((int)pObj);
			else
				return pCC->CreateNil();
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

ICCItem *fnObjGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnObjGet
//
//	Gets data about a space object

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	//	Get the object

	CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0));
	if (pObj == NULL)
		return pCC->CreateNil();

	//	Set the data as appropriate

	switch (dwData)
		{
		case FN_OBJ_ARMOR_CRITICALITY:
			{
			CInstalledArmor *pSection = GetArmorSectionArg(*pCC, pArgs->GetElement(1), pObj);
			if (pSection == NULL)
				return pCC->CreateNil();

			//	Need the ship class (for now)

			CShip *pShip = pObj->AsShip();
			if (pShip == NULL)
				return pCC->CreateNil();

			CShipClass *pClass = pShip->GetClass();
			if (pSection->GetSect() >= pClass->GetHullSectionCount())
				return pCC->CreateNil();

            DWORD dwCritical = pClass->GetHullSection(pSection->GetSect()).GetCriticalArea();
			if (dwCritical & CShipClass::sectCritical)
				return pCC->CreateString(CONSTLIT("critical"));
			else
				return pCC->CreateString(CONSTLIT("nonCritical"));
			}

		case FN_OBJ_ARMOR_LEVEL:
			{
			CInstalledArmor *pSection = GetArmorSectionArg(*pCC, pArgs->GetElement(1), pObj);
			if (pSection == NULL)
				return pCC->CreateNil();

			int iMaxHP = pSection->GetMaxHP(pObj);
			if (iMaxHP == 0)
				return pCC->CreateNil();

			//	Calculate level

			return pCC->CreateInteger(100 * pSection->GetHitPoints() / iMaxHP);
			}

		case FN_OBJ_ARMOR_REPAIR_PRICE:
			{
            int iArg = 1;

            //  If we have more than 3 args then we expect the second arg to be
            //  the object that has the armor.

            CSpaceObject *pSource = NULL;
            if (pArgs->GetCount() > 3)
                pSource = CreateObjFromItem(*pCC, pArgs->GetElement(iArg++));

            //  Get the armor item

			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(iArg++)));
			if (Item.GetType() == NULL)
				return pCC->CreateNil();

            //  Get HP to repair

            ICCItem *pHPToRepair = pArgs->GetElement(iArg++);
            int iHPToRepair = ((!pHPToRepair->IsNil() || pSource == NULL) ? pHPToRepair->GetIntegerValue() : -1);

			//	Ask the object

			int iPrice;
			if (!pObj->GetArmorRepairPrice(pSource, Item, iHPToRepair, 0, &iPrice))
				return pCC->CreateNil();

			//	Done

			return pCC->CreateInteger(iPrice);
			}

		case FN_OBJ_ARMOR_REPLACE_PRICE:
			{
			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
			if (Item.GetType() == NULL)
				return pCC->CreateNil();

			//	Ask the object

			int iPrice;
			if (!pObj->GetArmorInstallPrice(Item, 0, &iPrice))
				return pCC->CreateNil();

			//	Done

			return pCC->CreateInteger(iPrice);
			}

		case FN_OBJ_ARMOR_TYPE:
			{
			CInstalledArmor *pSection = GetArmorSectionArg(*pCC, pArgs->GetElement(1), pObj);
			if (pSection == NULL)
				return pCC->CreateNil();

			return pCC->CreateInteger((int)pSection->GetClass()->GetUNID());
			}

		case FN_OBJ_CALC_BEST_TARGET:
			{
			ICCItem *pObjList = (pArgs->GetCount() > 1 ? pArgs->GetElement(1) : NULL);

			//	If no list, then we just return the nearest visible enemy.

			if (pObjList == NULL || pObjList->GetCount() == 0)
				{
				CSpaceObject *pTarget = pObj->GetNearestVisibleEnemy();
				if (pTarget)
					return pCC->CreateInteger((int)pTarget);
				else
					return pCC->CreateNil();
				}

			//	Otherwise, we loop over the input list and find the best object.

			else
				{
				CSpaceObject *pBestTarget = NULL;
				Metric rBestDist2 = 0.0;

				CPerceptionCalc Perception(pObj->GetPerception());

				for (int i = 0; i < pObjList->GetCount(); i++)
					{
					CSpaceObject *pTarget = CreateObjFromItem(*pCC, pObjList->GetElement(i), CCUTIL_FLAG_CHECK_DESTROYED);
					if (pTarget == NULL || !pTarget->CanAttack() || pObj->IsFriend(pTarget))
						continue;

					Metric rDist2 = pObj->GetDistance2(pTarget);
					if (Perception.CanBeTargeted(pTarget, rDist2)
							&& (pBestTarget == NULL || rDist2 < rBestDist2))
						{
						pBestTarget = pTarget;
						rBestDist2 = rDist2;
						}
					}

				if (pBestTarget)
					return pCC->CreateInteger((int)pBestTarget);
				else
					return pCC->CreateNil();
				}
			}

		case FN_OBJ_CAN_DETECT_TARGET:
			{
			CSpaceObject *pTarget = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			if (pTarget == NULL)
				return pCC->CreateNil();

			CPerceptionCalc Perception(pObj->GetPerception());
			return pCC->CreateBool(Perception.CanBeTargeted(pTarget, pObj->GetDistance2(pTarget)));
			}

		case FN_OBJ_CAN_INSTALL_ITEM:
			{
			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
			if (Item.GetType() == NULL)
				return pCC->CreateError(CONSTLIT("Invalid item"), pArgs->GetElement(1));

			int iSlot = ((pArgs->GetCount() > 2 && !pArgs->GetElement(2)->IsNil()) ? pArgs->GetElement(2)->GetIntegerValue() : -1);

			//	Validate the slot

			if (iSlot != -1)
				{
				CShip *pShip = pObj->AsShip();
				if (Item.IsArmor() 
						&& pShip 
						&& (iSlot < 0 || iSlot >= pShip->GetArmorSectionCount()))
					return pCC->CreateError(CONSTLIT("Invalid armor segment"), pArgs->GetElement(2));
				if (Item.IsDevice()
						&& pShip
						&& (iSlot < 0 || iSlot >= pShip->GetDeviceCount() || pShip->GetDevice(iSlot)->IsEmpty()))
					return pCC->CreateError(CONSTLIT("Invalid device slot"), pArgs->GetElement(2));
				}

			//	Ask the object

			CSpaceObject::InstallItemResults iResult;
			CString sResult;
			CItem ItemToReplace;
			bool bCanInstall = pObj->CanInstallItem(Item, iSlot, &iResult, &sResult, &ItemToReplace);

			//	Generate the result

			ICCItem *pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;
			pList->Append(*pCC, pCC->CreateBool(bCanInstall));
			pList->AppendString(*pCC, CSpaceObject::ConvertToID(iResult));
			pList->AppendString(*pCC, sResult);
			if (ItemToReplace.GetType())
				{
				ICCItem *pItem = CreateListFromItem(*pCC, ItemToReplace);
				pList->Append(*pCC, pItem);
				pItem->Discard(pCC);
				}

			//	Done

			return pResult;
			}

		case FN_OBJ_CONDITION:
			{
			ICCItem *pCondition = pArgs->GetElement(1);
			CString sCondition = (pCondition && !pCondition->IsNil() ? pCondition->GetStringValue() : NULL_STR);

			//	If we want all conditions, then we return a list of conditions
			//	(or Nil)

			if (sCondition.IsBlank())
				{
				ICCItemPtr pResult = pObj->GetConditions().WriteAsCCItem();
				return pResult->Reference();
				}

			//	Otherwise, we just return the value for the given condition.

			else
				{
				CConditionSet::ETypes iCondition = CConditionSet::ParseCondition(sCondition);
				if (iCondition == CConditionSet::cndNone)
					return pCC->CreateError(CONSTLIT("Unknown condition"), pCondition);

				return pCC->CreateBool(pObj->GetCondition(iCondition));
				}
			}

		case FN_OBJ_DAMAGE:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			//	If we're already destroyed, then we're done

			if (pObj->IsDestroyed())
				return pCC->CreateNil();

			//	If we're inside the Damage function then we cannot proceed

			if (pObj->IsInDamageCode())
				return pCC->CreateError(CONSTLIT("objDamage: Cannot cause damage while inside a damage event"), pArgs);

			//	Weapon fire desc

			CWeaponFireDesc *pDesc = GetWeaponFireDescArg(*pCC, pArgs->GetElement(1));
			if (pDesc == NULL)
				return pCC->CreateNil();

			//	Optional arguments

			int iArg = 3;

			//	See if this is a position

			CVector vHitPos;
			if (pArgs->GetCount() > iArg
					&& IsVectorItem(pArgs->GetElement(iArg)))
				vHitPos = CreateVectorFromList(*pCC, pArgs->GetElement(iArg++));
			else
				vHitPos = pObj->GetPos();

			//	Get options

			bool bFullResult = false;
			bool bNoHitEffect = false;
			bool bIgnoreOverlays = false;
			bool bIgnoreShields = false;
			if (pArgs->GetCount() > iArg)
				{
				ICCItem *pOptions = pArgs->GetElement(iArg++);
				bFullResult = pOptions->GetBooleanAt(CONSTLIT("fullResult"));
				bNoHitEffect = pOptions->GetBooleanAt(CONSTLIT("noHitEffect"));
				bIgnoreOverlays = pOptions->GetBooleanAt(CONSTLIT("ignoreOverlays"));
				bIgnoreShields = pOptions->GetBooleanAt(CONSTLIT("ignoreShields"));
				}

			//	Direction

			int iDir = VectorToPolar(pObj->GetPos() - vHitPos);

			//	Damage
			//	Note that damage could destroy the object.
			//	After the call to pObj->Damage we must consider that pObj might
			//	be destroyed (though not freed)

			CDamageSource Attacker = GetDamageSourceArg(*pCC, pArgs->GetElement(2));
			SDamageCtx DamageCtx(pObj,
					pDesc,
					NULL,
					Attacker,
					Attacker.GetObj(),
					AngleMod(iDir + mathRandom(0, 30) - 15),
					vHitPos);

			DamageCtx.bNoHitEffect = bNoHitEffect;
			DamageCtx.bIgnoreOverlays = bIgnoreOverlays;
			DamageCtx.bIgnoreShields = bIgnoreShields;

			EDamageResults result = pObj->Damage(DamageCtx);

			//	Create fragments

			if (pDesc->HasFragments() && pSystem)
				{
				SShotCreateCtx FragCtx;
				FragCtx.pDesc = pDesc;
				FragCtx.Source = DamageCtx.Attacker;
				FragCtx.pTarget = (!pObj->IsDestroyed() ? pObj : NULL);
				FragCtx.vPos = vHitPos;

				pSystem->CreateWeaponFragments(FragCtx, DamageCtx.pCause);
				}

			//	No need to expose the concept of passthrough

			if (result == damagePassthrough
					|| result == damageNoDamageNoPassthrough)
				result = damageNoDamage;
			else if (result == damagePassthroughDestroyed
					|| result == damageDestroyedAbandoned)
				result = damageDestroyed;

			//	Compose result

			if (bFullResult)
				{
				ICCItem *pResult = pCC->CreateSymbolTable();

				pResult->SetStringAt(*pCC, CONSTLIT("result"), GetDamageResultsName(result));
				pResult->SetIntegerAt(*pCC, CONSTLIT("armorSeg"), DamageCtx.iSectHit);
				pResult->SetIntegerAt(*pCC, CONSTLIT("overlayHitDamage"), DamageCtx.iOverlayHitDamage);
				pResult->SetIntegerAt(*pCC, CONSTLIT("shieldHitDamage"), DamageCtx.iShieldHitDamage);
				pResult->SetIntegerAt(*pCC, CONSTLIT("armorHitDamage"), DamageCtx.iArmorHitDamage);

				return pResult;
				}
			else
				return pCC->CreateString(GetDamageResultsName(result));
			}

		case FN_OBJ_DATA_FIELD:
			return CreateResultFromDataField(*pCC, pObj->GetDataField(pArgs->GetElement(1)->GetStringValue()));

		case FN_OBJ_DEFAULT_CURRENCY:
			return pCC->CreateInteger(pObj->GetDefaultEconomy()->GetUNID());

		case FN_OBJ_DEVICE_FIRE_ARC:
			{
			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
			return pObj->GetItemProperty(*pCtx, Item, CONSTLIT("fireArc"));
			}

		case FN_OBJ_DEVICE_LINKED_FIRE_OPTIONS:
			{
			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
			return pObj->GetItemProperty(*pCtx, Item, CONSTLIT("linkedFireOptions"));
			}

		case FN_OBJ_DEVICE_POS:
			{
			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
			return pObj->GetItemProperty(*pCtx, Item, CONSTLIT("pos"));
			}

		case FN_OBJ_DOCKED_AT:
			{
			CSpaceObject *pStation = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			if (pStation == NULL)
				return pCC->CreateNil();
			else
				return pCC->CreateBool(pStation->IsObjDocked(pObj));
			}

		case FN_OBJ_GET_ABILITY:
			{
			Abilities iAbility = AbilityDecode(pArgs->GetElement(1)->GetStringValue());
			if (iAbility == ablUnknown)
				return pCC->CreateError(CONSTLIT("Unknown equipment"), pArgs->GetElement(1));

			return pCC->CreateString(AbilityStatusEncode(pObj->GetAbility(iAbility)));
			}

		case FN_OBJ_GET_BALANCE:
			{
			DWORD dwEconomyUNID;
			if (GetEconomyUNIDOrDefault(*pCC, (pArgs->GetCount() > 1 ? pArgs->GetElement(1) : NULL), &dwEconomyUNID) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid economy type"), pArgs->GetElement(1));

			return pCC->CreateInteger((int)pObj->GetBalance(dwEconomyUNID));
			}

		case FN_OBJ_GET_BUY_PRICE:
			{
			int i;
			CItem Item = CreateItemFromList(*pCC, pArgs->GetElement(1));
			ICCItem *pOptions = (pArgs->GetCount() >= 3 ? pArgs->GetElement(2) : NULL);

			//	Parse options

			bool bNoDonations = false;
			if (pOptions)
				{
				for (i = 0; i < pOptions->GetCount(); i++)
					{
					CString sOption = pOptions->GetElement(i)->GetStringValue();
					if (strEquals(sOption, CONSTLIT("noDonations")))
						bNoDonations = true;
					else
						return pCC->CreateError(CONSTLIT("Invalid option"), pOptions->GetElement(i));
					}
				}

			//	Calc value

			int iValue;

			//	Avoid recursion

			if ((pCtx->InEvent(eventGetTradePrice) || pCtx->InEvent(eventGetGlobalPlayerPriceAdj))
					&& pCtx->GetItemType() == Item.GetType())
				iValue = Item.GetRawPrice();

			//	Get the value from the station that is buying

			else
				{
				DWORD dwFlags = 0;
				if (bNoDonations)
					dwFlags |= CTradingDesc::FLAG_NO_DONATION;

				iValue = pObj->GetBuyPrice(Item, dwFlags);
				}

			if (iValue >= 0)
				return pCC->CreateInteger(iValue);
			else
				return pCC->CreateNil();
			}

		case FN_OBJ_GET_CHARACTER_DATA:
			{
			CDesignType *pCharacter = pObj->GetCharacter();
			if (pCharacter == NULL)
				pCharacter = pObj->GetType();
			if (pCharacter == NULL)
				return pCC->CreateNil();

			return pCharacter->GetGlobalData(pArgs->GetElement(1)->GetStringValue())->Reference();
			}

		case FN_OBJ_GET_DETECT_RANGE:
			{
			CSpaceObject *pTarget = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			if (pTarget == NULL)
				return pCC->CreateNil();

			CPerceptionCalc Perception(pObj->GetPerception());
			return pCC->CreateInteger(mathRound(Perception.GetMaxDist(pTarget) / LIGHT_SECOND));
			}

		case FN_OBJ_GET_DISPOSITION:
			{
			CSpaceObject *pTarget = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			if (pTarget == NULL)
				return pCC->CreateNil();

			return CreateDisposition(*pCC, pObj->GetDispositionTowards(pTarget));
			}

		case FN_OBJ_GET_EVENT_HANDLER:
			{
			CDesignType *pOverride = pObj->GetOverride();
			if (pOverride)
				return pCC->CreateInteger(pOverride->GetUNID());
			else
				return pCC->CreateNil();
			}

		case FN_OBJ_GET_ID:
			return pCC->CreateInteger(pObj->GetID());

		case FN_OBJ_GET_ITEM_PROPERTY:
			{
			//	Get the item

			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
			if (Item.GetType() == NULL)
				return pCC->CreateNil();

			//	Get the property

			CString sProperty = pArgs->GetElement(2)->GetStringValue();

			//	Get it

			return pObj->GetItemProperty(*pCtx, Item, sProperty);
			}

		case FN_OBJ_GET_NAMED_ITEM:
			{
			CString sName = pArgs->GetElement(1)->GetStringValue();
			CShip *pShip = pObj->AsShip();

			//	Create a result

			ICCItem *pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;

			//	Get the item(s)

			if (pShip)
				{
				if (strEquals(sName, NAMED_ITEM_SELECTED_WEAPON))
					{
					CItem theItem = pShip->GetNamedDeviceItem(devPrimaryWeapon);
					if (theItem.GetType())
						{
						ICCItem *pItem = CreateListFromItem(*pCC, theItem);
						pList->Append(*pCC, pItem);
						pItem->Discard(pCC);
						}
					}
				else if (strEquals(sName, NAMED_ITEM_SELECTED_LAUNCHER))
					{
					CItem theItem = pShip->GetNamedDeviceItem(devMissileWeapon);
					if (theItem.GetType())
						{
						ICCItem *pItem = CreateListFromItem(*pCC, theItem);
						pList->Append(*pCC, pItem);
						pItem->Discard(pCC);
						}
					}
				else if (strEquals(sName, NAMED_ITEM_SELECTED_MISSILE))
					{
					CInstalledDevice *pLauncher = pShip->GetNamedDevice(devMissileWeapon);
					if (pLauncher)
						{
						CItemType *pType;
						pLauncher->GetSelectedVariantInfo(pShip, NULL, NULL, &pType);
						if (pType)
							{
							if (pType->IsMissile())
								{
								CItemListManipulator ItemList(pShip->GetItemList());
								CItem theItem(pType, 1);
								if (ItemList.SetCursorAtItem(theItem))
									{
									ICCItem *pItem = CreateListFromItem(*pCC, theItem);
									pList->Append(*pCC, pItem);
									pItem->Discard(pCC);
									}
								}
							else
								{
								CItem theItem = pShip->GetNamedDeviceItem(devMissileWeapon);
								if (theItem.GetType())
									{
									ICCItem *pItem = CreateListFromItem(*pCC, theItem);
									pList->Append(*pCC, CreateListFromItem(*pCC, theItem));
									pItem->Discard(pCC);
									}
								}
							}
						}
					}
				}

			//	If the list has content, return it; otherwise, return Nil

			if (pList->GetCount() > 0)
				return pList;
			else
				{
				pList->Discard(pCC);
				return pCC->CreateNil();
				}
			}

		case FN_OBJ_GET_OVERLAY_DATA:
			{
			DWORD dwID = (DWORD)pArgs->GetElement(1)->GetIntegerValue();
			CString sAttrib = pArgs->GetElement(2)->GetStringValue();
			return pObj->GetOverlayData(dwID, sAttrib)->Reference();
			}

		case FN_OBJ_GET_OVERLAY_POS:
			{
			DWORD dwID = (DWORD)pArgs->GetElement(1)->GetIntegerValue();
			return CreateListFromVector(*pCC, pObj->GetOverlayPos(dwID));
			}

		case FN_OBJ_GET_OVERLAY_PROPERTY:
			{
			DWORD dwID = (DWORD)pArgs->GetElement(1)->GetIntegerValue();
			CString sProperty = pArgs->GetElement(2)->GetStringValue();
			return pObj->GetOverlayProperty(pCtx, dwID, sProperty);
			}

		case FN_OBJ_GET_OVERLAY_ROTATION:
			{
			DWORD dwID = (DWORD)pArgs->GetElement(1)->GetIntegerValue();
			return pCC->CreateInteger(pObj->GetOverlayRotation(dwID));
			}

		case FN_OBJ_GET_OVERLAYS:
			{
            //  See if we have a criteria

			DWORD dwType = 0;
            CDesignTypeCriteria Criteria;
            if (pArgs->GetCount() >= 2)
                {
				if (pArgs->GetElement(1)->IsInteger())
					{
					dwType = pArgs->GetElement(1)->GetIntegerValue();
					}
				else
					{
					if (CDesignTypeCriteria::ParseCriteria(pArgs->GetElement(1)->GetStringValue(), &Criteria) != NOERROR)
						return pCC->CreateError(CONSTLIT("Invalid criteria"), pArgs->GetElement(1));

					//  Make sure we include overlay types

					Criteria.IncludeType(designOverlayType);
					}
                }

            //  Get the list of overlays

			TArray<COverlay *> List;
			pObj->GetOverlayList(&List);

            //  Generate result 

            ICCItem *pResult = NULL;

			for (int i = 0; i < List.GetCount(); i++)
				{
				//	If we don't match the type, skip.

				if (dwType != 0 && List[i]->GetType()->GetUNID() != dwType)
					continue;

                //  If we don't match the criteria, skip

                if (!Criteria.IsEmpty() && !List[i]->GetType()->MatchesCriteria(Criteria))
                    continue;

                //  Create a list if necessary

                if (pResult == NULL)
                    pResult = pCC->CreateLinkedList();

                //  Add to list

                pResult->AppendInteger(*pCC, List[i]->GetID());
				}

            return (pResult ? pResult : pCC->CreateNil());
			}

		case FN_OBJ_GET_OVERLAY_TYPE:
			{
			DWORD dwID = (DWORD)pArgs->GetElement(1)->GetIntegerValue();
			COverlayType *pType = pObj->GetOverlayType(dwID);
			if (pType)
				return pCC->CreateInteger(pType->GetUNID());
			else
				return pCC->CreateNil();
			}

		case FN_OBJ_GET_PLAYER_PRICE_ADJ:
			{
			//	Get opaque data, if available

			ICCItem *pData = (pArgs->GetCount() > 1 ? pArgs->GetElement(1) : NULL);

			//	Generate context

			STradeServiceCtx ServiceCtx;
			ServiceCtx.iService = serviceCustom;
			ServiceCtx.pProvider = pObj;
			ServiceCtx.pCurrency = pObj->GetDefaultEconomy();

			//	Fire event

			int iPriceAdj;
			if (!g_pUniverse->GetDesignCollection().FireGetGlobalPlayerPriceAdj(ServiceCtx, pData, &iPriceAdj))
				return pCC->CreateNil();

			//	Done

			return pCC->CreateInteger(iPriceAdj);
			}

		case FN_OBJ_GET_PROPERTY:
			return pObj->GetProperty(*pCtx, pArgs->GetElement(1)->GetStringValue());

		case FN_OBJ_GET_REFUEL_ITEM:
			{
			CSpaceObject *pObjToRefuel = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			if (pObjToRefuel == NULL)
				return pCC->CreateError(CONSTLIT("Invalid object to refuel."));

			CItemType *pFuelType;
			int iPrice;
			if (!pObj->GetRefuelItemAndPrice(pObjToRefuel, &pFuelType, &iPrice))
				return pCC->CreateNil();

			//	Create a linked list to return the two values

			ICCItem *pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;

			//	Add the fuel item

			ASSERT(pFuelType);
			ICCItem *pFuelItem = CreateListFromItem(*pCC, CItem(pFuelType, 1));
			pList->Append(*pCC, pFuelItem);
			pFuelItem->Discard(pCC);

			//	Add the price

			pList->AppendInteger(*pCC, iPrice);

			//	Done

			return pResult;
			}

		case FN_OBJ_GET_SELL_PRICE:
			{
			CItem Item = CreateItemFromList(*pCC, pArgs->GetElement(1));
            if (Item.IsEmpty())
                return pCC->CreateNil();

			bool bNoInventoryCheck = (pArgs->GetCount() >= 3 && !pArgs->GetElement(2)->IsNil());

			int iValue;

			//	Avoid recursion

			if ((pCtx->InEvent(eventGetTradePrice) || pCtx->InEvent(eventGetGlobalPlayerPriceAdj))
					&& pCtx->GetItemType() == Item.GetType())
				iValue = Item.GetRawPrice();

			//	Get the value from the station that is selling

			else
				iValue = pObj->GetSellPrice(Item, (bNoInventoryCheck ? CTradingDesc::FLAG_NO_INVENTORY_CHECK : 0));

			if (iValue > 0)
				return pCC->CreateInteger(iValue);
			else
				return pCC->CreateNil();
			}

		case FN_OBJ_GET_SHIP_BUY_PRICE:
			{
			ICCItem *pArg;
			switch (CTLispConvert::ArgType(pArgs->GetElement(1), CTLispConvert::typeSpaceObject, &pArg))
				{
				case CTLispConvert::typeShipClass:
					{
					CShipClass *pClass = g_pUniverse->FindShipClass(pArg->GetIntegerValue());
					if (pClass == NULL)
						return pCC->CreateError(CONSTLIT("Invalid ship class"), pArg);

					//	Get the value from the station that is selling

					int iValue;
					if (!pObj->GetShipBuyPrice(pClass, 0, &iValue) || iValue <= 0)
						return pCC->CreateNil();

					return pCC->CreateInteger(iValue);
					}

				case CTLispConvert::typeSpaceObject:
					{
					CSpaceObject *pShip = CreateObjFromItem(*pCC, pArg);
					if (pShip == NULL)
						return pCC->CreateError(CONSTLIT("Invalid ship"), pArg);

					//	Get the value from the station that is selling

					int iValue;
					if (!pObj->GetShipBuyPrice(pShip, 0, &iValue) || iValue <= 0)
						return pCC->CreateNil();

					return pCC->CreateInteger(iValue);
					}

				case CTLispConvert::typeNil:
					return pCC->CreateNil();

				default:
					return pCC->CreateError(CONSTLIT("Invalid ship"), pArg);
				}
			}

		case FN_OBJ_GET_SHIP_SELL_PRICE:
			{
			ICCItem *pArg;
			switch (CTLispConvert::ArgType(pArgs->GetElement(1), CTLispConvert::typeSpaceObject, &pArg))
				{
				case CTLispConvert::typeShipClass:
					{
					CShipClass *pClass = g_pUniverse->FindShipClass(pArg->GetIntegerValue());
					if (pClass == NULL)
						return pCC->CreateError(CONSTLIT("Invalid ship class"), pArg);

					//	Get the value from the station that is selling

					int iValue;
					if (!pObj->GetShipSellPrice(pClass, 0, &iValue) || iValue <= 0)
						return pCC->CreateNil();

					return pCC->CreateInteger(iValue);
					}

				case CTLispConvert::typeSpaceObject:
					{
					CSpaceObject *pShip = CreateObjFromItem(*pCC, pArg);
					if (pShip == NULL)
						return pCC->CreateError(CONSTLIT("Invalid ship"), pArg);

					//	Get the value from the station that is selling

					int iValue;
					if (!pObj->GetShipSellPrice(pShip, 0, &iValue) || iValue <= 0)
						return pCC->CreateNil();

					return pCC->CreateInteger(iValue);
					}

				case CTLispConvert::typeNil:
					return pCC->CreateNil();

				default:
					return pCC->CreateError(CONSTLIT("Invalid ship"), pArg);
				}
			}

		case FN_OBJ_GET_STARGATE_ID:
			{
			if (!pObj->IsStargate())
				return pCC->CreateNil();

			CSystem *pSystem = pObj->GetSystem();
			if (pSystem == NULL)
				return pCC->CreateNil();

			CString sGateID;
			if (!pSystem->FindObjectName(pObj, &sGateID))
				return pCC->CreateNil();

			return pCC->CreateString(sGateID);
			}

		case FN_OBJ_HAS_SERVICE:
			{
			//	Get the service

			ETradeServiceTypes iService = CTradingDesc::ParseService(pArgs->GetElement(1)->GetStringValue());
			if (iService == serviceNone)
				return pCC->CreateError(CONSTLIT("Unknown service type"), pArgs->GetElement(1));

			return pCC->CreateBool(pObj->HasTradeService(iService));
			}

		case FN_OBJ_IDENTIFIED:
			return pCC->CreateBool(pObj->IsIdentified());

		case FN_OBJ_IMAGE:
			return CreateListFromImage(*pCC, pObj->GetImage());

		case FN_OBJ_INC_OVERLAY_DATA:
			{
			DWORD dwID = (DWORD)pArgs->GetElement(1)->GetIntegerValue();
			CString sAttrib = pArgs->GetElement(2)->GetStringValue();

            COverlayList *pOverlays = pObj->GetOverlays();
            if (pOverlays == NULL)
                return pCC->CreateNil();

            return pOverlays->IncData(dwID, sAttrib, (pArgs->GetCount() > 3 ? pArgs->GetElement(3) : NULL))->Reference();
			}

		case FN_OBJ_IS_ANGRY_AT:
			{
			CSpaceObject *pTargetObj = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			if (pTargetObj == NULL)
				return pCC->CreateNil();
			else
				return pCC->CreateBool(pObj->IsAngryAt(pTargetObj));
			}

		case FN_OBJ_IS_DEVICE_ENABLED:
			{
			//	Get the device

			CInstalledDevice *pDevice = GetDeviceFromItem(*pCC, pObj, pArgs->GetElement(1));
			if (pDevice == NULL)
				return pCC->CreateError(CONSTLIT("Item is not an installed device on object"), pArgs->GetElement(1));

			//	Return status

			return pCC->CreateBool(pDevice->IsEnabled());
			}

		case FN_OBJ_LEVEL:
			return pCC->CreateInteger(pObj->GetLevel());

		case FN_OBJ_MASS:
			return pCC->CreateInteger((int)pObj->GetMass());

		case FN_OBJ_MATCHES:
			{
			CSpaceObject *pSource = CreateObjFromItem(*pCC, pArgs->GetElement(1));

			CString sFilter = pArgs->GetElement(2)->GetStringValue();
			CSpaceObjectCriteria Criteria(pSource, sFilter);

			CSpaceObjectCriteria::SCtx Ctx(Criteria);
			return pCC->CreateBool(pObj->MatchesCriteria(Ctx, Criteria));
			}

		case FN_OBJ_NAME:
			{
			DWORD dwFlags;
			if (pArgs->GetCount() > 1)
				dwFlags = pCtx->AsNameFlags(pArgs->GetElement(1));
			else
				dwFlags = 0;
			return pCC->CreateString(pObj->GetNounPhrase(dwFlags));
			}

		case FN_OBJ_OPEN_DOCKING_PORT_COUNT:
			return pCC->CreateInteger(pObj->GetOpenDockingPortCount());

		case FN_OBJ_ORDER_GIVER:
			{
			DestructionTypes iCause;
			if (pArgs->GetCount() > 1)
				iCause = GetDestructionCause(pArgs->GetElement(1)->GetStringValue());
			else
				iCause = killedNone;

			CSpaceObject *pOrderGiver = pObj->GetOrderGiver(iCause);
			if (pOrderGiver)
				return pCC->CreateInteger((int)pOrderGiver);
			else
				return pCC->CreateNil();
			}

		case FN_OBJ_PARALYZED:
			return pCC->CreateBool(pObj->IsParalyzed());

		case FN_OBJ_RADIOACTIVE:
			return pCC->CreateBool(pObj->IsRadioactive());

		case FN_OBJ_SET_OVERLAY_DATA:
			{
			DWORD dwID = (DWORD)pArgs->GetElement(1)->GetIntegerValue();
			CString sAttrib = pArgs->GetElement(2)->GetStringValue();

			pObj->SetOverlayData(dwID, sAttrib, pArgs->GetElement(3));
			return pCC->CreateTrue();
			}

		case FN_OBJ_SET_OVERLAY_EFFECT_PROPERTY:
			{
			DWORD dwID = (DWORD)pArgs->GetElement(1)->GetIntegerValue();
			CString sAttrib = pArgs->GetElement(2)->GetStringValue();
			ICCItem *pValue = pArgs->GetElement(3);

			return pCC->CreateBool(pObj->SetOverlayEffectProperty(dwID, sAttrib, pValue));
			}

		case FN_OBJ_SET_OVERLAY_POSITION:
			{
			DWORD dwID = (DWORD)pArgs->GetElement(1)->GetIntegerValue();
			CVector vPos = CreateVectorFromList(*pCC, pArgs->GetElement(2));

			//	Do it

			pObj->SetOverlayPos(dwID, vPos);
			return pCC->CreateTrue();
			}

		case FN_OBJ_SET_OVERLAY_PROPERTY:
			{
			DWORD dwID = (DWORD)pArgs->GetElement(1)->GetIntegerValue();

			CString sError;
			if (!pObj->SetOverlayProperty(dwID, pArgs->GetElement(2)->GetStringValue(), pArgs->GetElement(3), &sError))
				{
				if (sError.IsBlank())
					return pCC->CreateError(CONSTLIT("Invalid property"), pArgs->GetElement(2));
				else
					return pCC->CreateError(sError);
				}

			return pCC->CreateTrue();
			}

		case FN_OBJ_SET_OVERLAY_ROTATION:
			{
			DWORD dwID = (DWORD)pArgs->GetElement(1)->GetIntegerValue();
			int iRotation = pArgs->GetElement(2)->GetIntegerValue();
			pObj->SetOverlayRotation(dwID, iRotation);
			return pCC->CreateInteger(iRotation);
			}

		case FN_OBJ_SHIPWRECK_TYPE:
			{
			CDesignType *pType = pObj->GetWreckType();
			if (pType)
				return pCC->CreateInteger(pType->GetUNID());
			else
				return pCC->CreateNil();
			}

		case FN_OBJ_TRANSLATE:
			{
			//	Get parameters

			int iArg = 1;
			ICCItem *pText = pArgs->GetElement(iArg++);
			if (pText->IsNil())
				return pCC->CreateNil();

			CString sText = pText->GetStringValue();
			ICCItem *pData = NULL;
			if (pArgs->GetCount() > iArg)
				{
				if (pArgs->GetElement(iArg)->IsSymbolTable())
					pData = pArgs->GetElement(iArg++);
				}

			ICCItem *pDefault = NULL;
			if (pArgs->GetCount() > iArg)
				pDefault = pArgs->GetElement(iArg++);

			//	Translate

			ICCItem *pResult;
			if (!pObj->Translate(sText, pData, &pResult) || pResult->IsNil())
				{
				if (pDefault)
					return pDefault->Reference();
				else
					return pCC->CreateNil();
				}

			return pResult;
			}

		case FN_OBJ_VELOCITY:
			return CreateListFromVector(*pCC, pObj->GetVel());

		case FN_OBJ_TYPE:
			{
			CDesignType *pType = pObj->GetType();
			if (pType)
				return pCC->CreateInteger(pType->GetUNID());
			else
				return pCC->CreateNil();
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

ICCItem *fnObjGetOld (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnObjGetOld
//
//	Gets data about a space object
//
//	(objGetName obj) -> Name of the object

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	ICCItem *pArgs;
	ICCItem *pResult;

	//	Evaluate the arguments and validate them

	if (dwData == FN_OBJ_DISTANCE || dwData == FN_OBJ_NAME || FN_OBJ_INSTALLED_ITEM_DESC || FN_OBJ_ENEMY)
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("i*"));
	else if (dwData == FN_OBJ_ATTRIBUTE)
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("is"));
	else
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("i"));
	if (pArgs->IsError())
		return pArgs;

	//	Valid pObj is optional for some functions

	bool bRequireValidObj;
	if (dwData == FN_OBJ_IS_ABANDONED)
		bRequireValidObj = false;
	else
		bRequireValidObj = true;

	//	Convert the first argument into a space object

	CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0));
	if (bRequireValidObj && pObj == NULL)
		{
		pArgs->Discard(pCC);
		return pCC->CreateNil();
		}

	//	Second object

	DWORD dwFlags = 0;
	CSpaceObject *pObj2 = NULL;
	CString sAttribute;
	CItem Item;
	if (dwData == FN_OBJ_DISTANCE || dwData == FN_OBJ_ENEMY)
		{
		if (pArgs->GetCount() == 2)
			{
			pObj2 = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			if (pObj2 == NULL)
				{
				pArgs->Discard(pCC);
				return pCC->CreateNil();
				}
			}
		}
	else if (dwData == FN_OBJ_NAME)
		{
		if (pArgs->GetCount() == 2)
			dwFlags = pArgs->GetElement(1)->GetIntegerValue();
		}
	else if (dwData == FN_OBJ_ATTRIBUTE)
		sAttribute = pArgs->GetElement(1)->GetStringValue();
	else if (dwData == FN_OBJ_INSTALLED_ITEM_DESC)
		{
		Item = CreateItemFromList(*pCC, pArgs->GetElement(1));
		}

	//	No longer needed

	pArgs->Discard(pCC);

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_OBJ_ATTRIBUTE:
			pResult = pCC->CreateBool(pObj->HasAttribute(sAttribute));
			break;

		case FN_OBJ_CAN_ATTACK:
			pResult = pCC->CreateBool(pObj->CanAttack());
			break;

		case FN_OBJ_CARGO_SPACE_LEFT:
			pResult = pCC->CreateInteger((int)(pObj->GetCargoSpaceLeft() * 1000.0));
			break;

		case FN_OBJ_COMBAT_POWER:
			pResult = pCC->CreateInteger(pObj->GetCombatPower());
			break;

		case FN_OBJ_DAMAGE_TYPE:
			pResult = pCC->CreateInteger(pObj->GetDamageType());
			break;

		case FN_OBJ_DESTINY:
			pResult = pCC->CreateInteger(pObj->GetDestiny());
			break;

		case FN_OBJ_SHIELD_LEVEL:
			pResult = pCC->CreateInteger(pObj->GetShieldLevel());
			break;

		case FN_OBJ_INSTALLED_ITEM_DESC:
			if (Item.GetType() && Item.IsInstalled())
				pResult = pCC->CreateString(pObj->GetInstallationPhrase(Item));
			else
				pResult = pCC->CreateNil();
			break;

		case FN_OBJ_IS_ABANDONED:
			if (pObj)
				pResult = pCC->CreateBool(pObj->IsAbandoned());
			else
				pResult = pCC->CreateBool(true);
			break;

		case FN_OBJ_IS_SHIP:
			pResult = pCC->CreateBool(pObj->GetCategory() == CSpaceObject::catShip);
			break;

		case FN_OBJ_ENEMY:
			if (pObj2)
				pResult = pCC->CreateBool(pObj->IsEnemy(pObj2) 
						&& !pObj2->IsEscortingFriendOf(pObj)
						&& !pObj->IsEscortingFriendOf(pObj2));
			else
				pResult = pCC->CreateNil();
			break;

		case FN_OBJ_KNOWN:
			pResult = pCC->CreateBool(pObj->IsKnown());
			break;

		case FN_OBJ_IS_UNDER_ATTACK:
			pResult = pCC->CreateBool(pObj->IsUnderAttack());
			break;

		case FN_OBJ_MAX_POWER:
			pResult = pCC->CreateInteger(pObj->GetMaxPower());
			break;

		case FN_OBJ_POSITION:
			pResult = CreateListFromVector(*pCC, pObj->GetPos());
			break;

		case FN_OBJ_SOVEREIGN:
			{
			CSovereign *pSovereign = pObj->GetSovereign();
			if (pSovereign == NULL)
				pResult = pCC->CreateNil();
			else
				pResult = pCC->CreateInteger(pSovereign->GetUNID());
			break;
			}

		case FN_OBJ_TARGET:
			{
			CSpaceObject *pTarget = pObj->GetTarget(CItemCtx(), true);
			if (pTarget)
				pResult = pCC->CreateInteger((int)pTarget);
			else
				pResult = pCC->CreateNil();
			break;
			}

		case FN_OBJ_VISIBLE_DAMAGE:
			pResult = pCC->CreateInteger(pObj->GetVisibleDamage());
			break;

		case FN_OBJ_SET_KNOWN:
			pObj->SetKnown();
			pResult = pCC->CreateTrue();
			break;

		case FN_OBJ_DISTANCE:
			{
			Metric rDist;

			if (pObj2)
				{
				CVector vDist = pObj->GetPos() - pObj2->GetPos();
				rDist = vDist.Length();
				}
			else
				rDist = pObj->GetPos().Length();

			int iDistInLightSeconds = (int)((rDist / LIGHT_SECOND) + 0.5);
			pResult = pCC->CreateInteger(iDistInLightSeconds);
			break;
			}

		case FN_OBJ_NEAREST_STARGATE:
			{
			CSpaceObject *pGate = pObj->GetNearestStargate(true);
			if (pGate == NULL)
				pResult = pCC->CreateNil();
			else
				pResult = pCC->CreateInteger((int)pGate);
			break;
			}

		case FN_OBJ_LOWER_SHIELDS:
			{
			pObj->DeactivateShields();
			pResult = pCC->CreateTrue();
			break;
			}

		default:
			ASSERT(FALSE);
		}

	return pResult;
	}

ICCItem *fnObjSendMessage (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnObjSendMessage
//
//	(objSendMessage ...)

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Get the object (OK if nil)

	CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0));

	//	Handle it

	switch (dwData)
		{
		case FN_OBJ_MESSAGE:
			{
			//	Second param is the sender; third is message

			CSpaceObject *pSender = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			CString sMessage = pArgs->GetElement(2)->GetStringValue();

			//	If no message, nothing to do

			if (sMessage.IsBlank())
				return pCC->CreateNil();

			//	If target is nil, then send to player

			if (pObj == NULL)
				{
				IPlayerController *pPlayer = g_pUniverse->GetPlayer();
				if (pPlayer)
					pPlayer->OnMessageFromObj(pSender, sMessage);
				}

			//	Otherwise, send to object (which might send it to the player or
			//	whatever).

			else
				pObj->SendMessage(pSender, sMessage);

			return pCC->CreateTrue();
			}

		case FN_OBJ_MESSAGE_TRANSLATE:
			{
			//	Second param is the sender; third is message ID

			CSpaceObject *pSender = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			ICCItem *pMessageID = pArgs->GetElement(2);
			if (pMessageID->IsNil())
				return pCC->CreateNil();

			//	If no message, nothing to do

			CString sMessageID = pMessageID->GetStringValue();
			if (sMessageID.IsBlank())
				return pCC->CreateNil();

			//	See if we have a data element

			ICCItem *pData = (pArgs->GetCount() > 3 ? pArgs->GetElement(3) : NULL);

			//	By default we translate from the sender's table, but if no sender, 
			//	then we use the recipient.

			CSpaceObject *pTranslator = (pSender ? pSender : pObj);
			if (pTranslator == NULL)
				return pCC->CreateNil();

			//	Translate

			ICCItem *pResult;
			if (!pTranslator->Translate(sMessageID, pData, &pResult) || pResult->IsNil())
				return pCC->CreateNil();

			CString sMessage = pResult->GetStringValue();
			pResult->Discard(pCC);

			//	If target is nil, then send to player

			if (pObj == NULL)
				{
				IPlayerController *pPlayer = g_pUniverse->GetPlayer();
				if (pPlayer)
					pPlayer->OnMessageFromObj(pSender, sMessage);
				}

			//	Otherwise, send to object (which might send it to the player or
			//	whatever).

			else
				pObj->SendMessage(pSender, sMessage);

			return pCC->CreateTrue();
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

ICCItem *fnObjSet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnObjSet
//
//	Sets data about a space object

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	//	Get the object

	CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0));
	if (pObj == NULL)
		return pCC->CreateNil();

	//	Set the data as appropriate

	switch (dwData)
		{
		case FN_OBJ_ACCELERATE:
			{
			CVector vAccel = PolarToVector(pArgs->GetElement(1)->GetIntegerValue(), pArgs->GetElement(2)->GetIntegerValue());
			Metric rTime = (pArgs->GetCount() > 3 ? g_SecondsPerUpdate * pArgs->GetElement(3)->GetIntegerValue() : g_SecondsPerUpdate);

			pObj->Accelerate(vAccel, rTime);
			pObj->ClipSpeed(pObj->GetMaxSpeed());

			return CreateListFromVector(*pCC, pObj->GetVel());
			}

		case FN_OBJ_ADD_BUY_ORDER:
		case FN_OBJ_ADD_SELL_ORDER:
			{
			CItemType *pType = NULL;
			CString sCriteria;

			//	Accept either an item type or a criteria

			if (pArgs->GetElement(1)->IsIdentifier())
				sCriteria = pArgs->GetElement(1)->GetStringValue();
			else
				{
				pType = GetItemTypeFromArg(*pCC, pArgs->GetElement(1));
				if (pType == NULL)
					return pCC->CreateNil();
				}

			//	Price adjustment

			int iPriceAdj = pArgs->GetElement(2)->GetIntegerValue();

			if (dwData == FN_OBJ_ADD_BUY_ORDER)
				pObj->AddBuyOrder(pType, sCriteria, iPriceAdj);
			else
				pObj->AddSellOrder(pType, sCriteria, iPriceAdj);

			return pCC->CreateTrue();
			}

		case FN_OBJ_ADD_CONNECTION:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			CObjectJoint::ETypes iType = CObjectJoint::ParseType(pArgs->GetElement(1)->GetStringValue());
			if (iType == CObjectJoint::jointNone)
				return pCC->CreateError(CONSTLIT("Unknown connection type"), pArgs->GetElement(1));

			CSpaceObject *pObj2 = CreateObjFromItem(*pCC, pArgs->GetElement(2), CCUTIL_FLAG_CHECK_DESTROYED);
			if (pObj2 == NULL)
				return pCC->CreateError(CONSTLIT("Invalid object"), pArgs->GetElement(2));

			ICCItem *pOptions = (pArgs->GetCount() > 3 ? pArgs->GetElement(3) : NULL);

			DWORD dwID;
			if (!pSystem->AddJoint(iType, pObj, pObj2, pOptions, &dwID))
				return pCC->CreateNil();

			return pCC->CreateInteger(dwID);
			}

		case FN_OBJ_ADD_ITEM:
			{
			CItem Item(GetItemFromArg(*pCC, pArgs->GetElement(1)));
			if (2 < pArgs->GetCount())
				Item.SetCount(pArgs->GetElement(2)->GetIntegerValue());

			if (Item.GetType() == NULL || Item.GetCount() <= 0)
				return pCC->CreateNil();

			//	Do not allow adding installed items
			Item.SetInstalled(-1);

			CItemListManipulator ObjList(pObj->GetItemList());
			ObjList.AddItem(Item);
			pObj->OnComponentChanged(comCargo);
			pObj->ItemsModified();
			pObj->InvalidateItemListAddRemove();

			//	Return the item.

			return CreateListFromItem(*pCC, Item);
			}

		case FN_OBJ_ADD_ITEM_ENHANCEMENT:
			{
			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));

			//	Find the enhancement type

			CItemType *pType = GetItemTypeFromArg(*pCC, pArgs->GetElement(2));
			if (pType == NULL)
				return pCC->CreateError(CONSTLIT("Unknown item type"), NULL);

			//	Get optional lifetime

			int iLifetime = -1;
			if (pArgs->GetCount() > 3 && !pArgs->GetElement(3)->IsNil())
				iLifetime = pArgs->GetElement(3)->GetIntegerValue();

			//	Add

			DWORD dwID;
			EnhanceItemStatus iResult = pObj->AddItemEnhancement(Item, pType, iLifetime, &dwID);

			//	Done

			if (dwID != OBJID_NULL)
				return pCC->CreateInteger(dwID);
			else
				return pCC->CreateNil();
			}

		case FN_OBJ_ADD_OVERLAY:
			{
			//	Find the overlay type

			DWORD dwUNID = (DWORD)pArgs->GetElement(1)->GetIntegerValue();
			COverlayType *pField = g_pUniverse->FindShipEnergyFieldType(dwUNID);
			if (pField == NULL)
				return pCC->CreateError(CONSTLIT("Unknown overlay type"), NULL);

			//	Optional args

			int iLifetime = -1;
			int iPosAngle = 0;
			int iPosRadius = 0;
			int iRotation = 0;

			if (pArgs->GetCount() == 3)
				iLifetime = pArgs->GetElement(2)->GetIntegerValue();
			else if (pArgs->GetCount() >= 4)
				{
				//	Convert from position to angle and radius

				CVector vPos = CreateVectorFromList(*pCC, pArgs->GetElement(2));
				Metric rRadius;
				int iDirection = VectorToPolar(vPos - pObj->GetPos(), &rRadius);
				int iRotationOrigin = (pField->RotatesWithShip() ? pObj->GetRotation() : 0);
				iPosAngle = AngleMod(iDirection - iRotationOrigin);
				iPosRadius = (int)(rRadius / g_KlicksPerPixel);

				//	Rotation

				iRotation = pArgs->GetElement(3)->GetIntegerValue();

				//	Lifetime

				int iArg = 4;
				if (pArgs->GetCount() > iArg && pArgs->GetElement(iArg)->IsInteger())
					iLifetime = pArgs->GetElement(iArg++)->GetIntegerValue();
				}

			//	Add it

			DWORD dwID;
			pObj->AddOverlay(pField, iPosAngle, iPosRadius, iRotation, iLifetime, &dwID);
			if (dwID == 0)
				return pCC->CreateNil();
			else
				return pCC->CreateInteger(dwID);
			}

		case FN_OBJ_ADD_TRADE_ORDER:
			{
			//	Get the service

			ETradeServiceTypes iService = CTradingDesc::ParseService(pArgs->GetElement(1)->GetStringValue());
			if (iService == serviceNone)
				return pCC->CreateError(CONSTLIT("Unknown service type"), pArgs->GetElement(1));

			//	Accept either an item type or a criteria

			CItemType *pType = NULL;
			CString sCriteria;
			if (pArgs->GetElement(2)->IsIdentifier())
				{
				sCriteria = pArgs->GetElement(2)->GetStringValue();
				if (sCriteria.IsBlank())
					return pCC->CreateError(CONSTLIT("Invalid criteria"), pArgs->GetElement(2));
				}
			else
				{
				pType = GetItemTypeFromArg(*pCC, pArgs->GetElement(2));
				if (pType == NULL)
					return pCC->CreateNil();
				}

			//	Price adjustment

			int iPriceAdj = pArgs->GetElement(3)->GetIntegerValue();

			//	Add the order

			pObj->AddTradeOrder(iService, sCriteria, pType, iPriceAdj);

			return pCC->CreateTrue();
			}

		case FN_OBJ_CHARGE:
			{
			DWORD dwEconomyUNID;
			if (GetEconomyUNIDOrDefault(*pCC, (pArgs->GetCount() > 2 ? pArgs->GetElement(1) : NULL), &dwEconomyUNID) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid economy type"), pArgs->GetElement(1));

			int iValue = (pArgs->GetCount() > 2 ? pArgs->GetElement(2) : pArgs->GetElement(1))->GetIntegerValue();
			int iRemaining = (int)pObj->ChargeMoney(dwEconomyUNID, iValue);
			if (iRemaining == -1)
				return pCC->CreateNil();
			else
				return pCC->CreateInteger(iRemaining);
			}

		case FN_OBJ_CLEAR_IDENTIFIED:
			{
			pObj->SetIdentified(false);
			return pCC->CreateTrue();
			}

		case FN_OBJ_CREATE_REFLECTION:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			CMissile *pMissile = pObj->AsMissile();
			if (pMissile == NULL)
				return pCC->CreateError(CONSTLIT("Invalid missile"));
			
			CVector vPos;
			if (pArgs->GetCount() >= 2 && GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vPos) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));
			else
				vPos = pMissile->GetPos();

			int iDirection;
			if (pArgs->GetCount() == 3)
				iDirection = pArgs->GetElement(2)->GetIntegerValue();
			else
				iDirection = pMissile->GetRotation();

			CMissile *pReflection;

			pMissile->CreateReflection(vPos, iDirection, &pReflection);
			return pCC->CreateInteger((int) pReflection);
			}
		case FN_OBJ_CREDIT:
			{
			DWORD dwEconomyUNID;
			if (GetEconomyUNIDOrDefault(*pCC, (pArgs->GetCount() > 2 ? pArgs->GetElement(1) : NULL), &dwEconomyUNID) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid economy type"), pArgs->GetElement(1));

			int iValue = (pArgs->GetCount() > 2 ? pArgs->GetElement(2) : pArgs->GetElement(1))->GetIntegerValue();
			int iRemaining = (int)pObj->CreditMoney(dwEconomyUNID, iValue);
			return pCC->CreateInteger(iRemaining);
			}

		case FN_OBJ_DEPLETE_SHIELDS:
			{
			pObj->DepleteShields();
			return pCC->CreateTrue();
			}

		case FN_OBJ_DESTROY:
			{
			CDamageSource DamageSource(NULL, removedFromSystem);
			if (pArgs->GetCount() > 1)
				DamageSource = GetDamageSourceArg(*pCC, pArgs->GetElement(1));

			pObj->Destroy(DamageSource.GetCause(), DamageSource);
			return pCC->CreateTrue();
			}

		case FN_OBJ_DEVICE_ACTIVATION_DELAY:
			{
			//	Get the device

			CInstalledDevice *pDevice = GetDeviceFromItem(*pCC, pObj, pArgs->GetElement(1));
			if (pDevice == NULL)
				return pCC->CreateError(CONSTLIT("Item is not an installed device on object"), pArgs->GetElement(1));

			//	Reset or set it

			if (pArgs->GetCount() > 2)
				pObj->SetFireDelay(pDevice, Max(0, pArgs->GetElement(2)->GetIntegerValue()));
			else
				pObj->SetFireDelay(pDevice);

			//	Done

			return pCC->CreateTrue();
			}

		case FN_OBJ_DEVICE_FIRE_ARC:
			{
			//	Get the device

			CInstalledDevice *pDevice = GetDeviceFromItem(*pCC, pObj, pArgs->GetElement(1));
			if (pDevice == NULL)
				return pCC->CreateError(CONSTLIT("Item is not an installed device on object"), pArgs->GetElement(1));

			//	Get the parameters. We accept any of the following args:
			//
			//	*	Two integers reprenting minArc and maxArc
			//	*	A single list of two integers, minArc and maxArc
			//	*	The special value 'omnidirectional
			//	*	Nil (meaning no fire arc and not omnidirectional)

			int iMinFireArc;
			int iMaxFireArc;
			if (pArgs->GetCount() > 3)
				{
				iMinFireArc = AngleMod(pArgs->GetElement(2)->GetIntegerValue());
				iMaxFireArc = AngleMod(pArgs->GetElement(3)->GetIntegerValue());

				pDevice->SetOmniDirectional(false);
				pDevice->SetFireArc(iMinFireArc, iMaxFireArc);
				}
			else if (pArgs->GetElement(2)->IsNil())
				{
				pDevice->SetOmniDirectional(false);
				pDevice->SetFireArc(0, 0);
				}
			else if (strEquals(pArgs->GetElement(2)->GetStringValue(), DEVICE_FIRE_ARC_OMNI))
				{
				pDevice->SetOmniDirectional(true);
				pDevice->SetFireArc(0, 0);
				}
			else if (pArgs->GetElement(2)->IsList() && pArgs->GetElement(2)->GetCount() >= 2)
				{
				iMinFireArc = AngleMod(pArgs->GetElement(2)->GetElement(0)->GetIntegerValue());
				iMaxFireArc = AngleMod(pArgs->GetElement(2)->GetElement(1)->GetIntegerValue());

				pDevice->SetOmniDirectional(false);
				pDevice->SetFireArc(iMinFireArc, iMaxFireArc);
				}
			else
				return pCC->CreateError(CONSTLIT("Invalid fire arc"), pArgs->GetElement(2));

			//	Done

			return pCC->CreateTrue();
			}

		case FN_OBJ_DEVICE_LINKED_FIRE_OPTIONS:
			{
			//	Get the device

			CInstalledDevice *pDevice = GetDeviceFromItem(*pCC, pObj, pArgs->GetElement(1));
			if (pDevice == NULL)
				return pCC->CreateError(CONSTLIT("Item is not an installed device on object"), pArgs->GetElement(1));

			//	Parse the options

			DWORD dwOptions;
			CString sError;
			if (!GetLinkedFireOptions(pArgs->GetCount() >= 3 ? pArgs->GetElement(2) : NULL, &dwOptions, &sError))
				return pCC->CreateError(sError, pArgs->GetElement(2));

			//	Set

			pDevice->SetLinkedFireOptions(dwOptions);

			//	Done

			return pCC->CreateTrue();
			}

		case FN_OBJ_DEVICE_POS:
			{
			//	Get the device

			CInstalledDevice *pDevice = GetDeviceFromItem(*pCC, pObj, pArgs->GetElement(1));
			if (pDevice == NULL)
				return pCC->CreateError(CONSTLIT("Item is not an installed device on object"), pArgs->GetElement(1));

			//	Get the parameters. We accept either angle/radius/z as parameters
			//	to the function or a single list parameter with angle/radius/z.
			//	(The latter is compatible with the return of objGetDevicePos.)

			int iPosAngle;
			int iPosRadius;
			int iZ = 0;
			if (pArgs->GetCount() > 3)
				{
				iPosAngle = pArgs->GetElement(2)->GetIntegerValue();
				iPosRadius = pArgs->GetElement(3)->GetIntegerValue();

				if (pArgs->GetCount() > 4)
					iZ = pArgs->GetElement(4)->GetIntegerValue();
				}
			else if (pArgs->GetElement(2)->IsList() && pArgs->GetElement(2)->GetCount() >= 2)
				{
				iPosAngle = pArgs->GetElement(2)->GetElement(0)->GetIntegerValue();
				iPosRadius = pArgs->GetElement(2)->GetElement(1)->GetIntegerValue();

				if (pArgs->GetElement(2)->GetCount() >= 3)
					iZ = pArgs->GetElement(2)->GetElement(2)->GetIntegerValue();
				}
			else
				return pCC->CreateError(CONSTLIT("Invalid angle and radius"), pArgs->GetElement(2));

			//	Set it

			pDevice->SetPosAngle(iPosAngle);
			pDevice->SetPosRadius(iPosRadius);
			pDevice->SetPosZ(iZ);

			//	Done

			return pCC->CreateTrue();
			}

		case FN_OBJ_FIRE_EVENT:
			{
			ICCItem *pResult;
			ICCItem *pData = (pArgs->GetCount() > 2 ? pArgs->GetElement(2) : NULL);
			pObj->FireCustomEvent(pArgs->GetElement(1)->GetStringValue(), eventObjFireEvent, pData, &pResult);
            if (pResult->IsError() && g_pUniverse->InDebugMode())
                ::kernelDebugLogPattern("[%s %s]: %s", pObj->GetNounPhrase(), pArgs->GetElement(1)->GetStringValue(), pResult->GetStringValue());
			return pResult;
			}

		case FN_OBJ_FIRE_ITEM_EVENT:
			{
			ICCItem *pResult;
			CItem Item = GetItemFromArg(*pCC, pArgs->GetElement(1));
			ICCItem *pData = (pArgs->GetCount() >= 4 ? pArgs->GetElement(3) : NULL);

			pObj->FireCustomItemEvent(pArgs->GetElement(2)->GetStringValue(), Item, pData, &pResult);
			return pResult;
			}

		case FN_OBJ_FIRE_ITEM_INVOKE:
			{
			CItem Item = GetItemFromArg(*pCC, pArgs->GetElement(1));
			CItemType *pType = Item.GetType();
			if (pType == NULL)
				return pCC->CreateNil();

			CItemType::SUseDesc UseDesc;
			if (!pType->GetUseDesc(&UseDesc)
					|| !UseDesc.bUsableInCockpit)
				return pCC->CreateNil();;

			CString sError;
			pObj->UseItem(Item, &sError);
			if (sError.IsBlank())
				{
				::kernelDebugLogPattern("[%s %s Invoke]: %s", pObj->GetNounPhrase(), pType->GetNounPhrase(), sError);
				return pCC->CreateNil();
				}

			return pCC->CreateTrue();
			}

		case FN_OBJ_FIRE_OVERLAY_EVENT:
			{
			ICCItem *pResult;
			DWORD dwOverlayID = pArgs->GetElement(1)->GetIntegerValue();
			ICCItem *pData = (pArgs->GetCount() > 3 ? pArgs->GetElement(3) : NULL);
			pObj->FireCustomOverlayEvent(pArgs->GetElement(2)->GetStringValue(), dwOverlayID, pData, &pResult);
			return pResult;
			}
		case FN_OBJ_FIRE_POWER_INVOKE:
		{
			DWORD dwPowerUNID = pArgs->GetElement(1)->GetIntegerValue();
			CPower *pPower = g_pUniverse->FindPower(dwPowerUNID);

			//If we don't specify a target, get the object's target
			CSpaceObject *pTarget;
			if (pArgs->GetCount() >= 3)
				pTarget = CreateObjFromItem(*pCC, pArgs->GetElement(2));
			else
				pTarget = pObj->GetTarget(CItemCtx(), true);

			CString sError;

			//	See if we need to fire the pre-invoke check

			if (pArgs->GetCount() >= 4 && strEquals(pArgs->GetElement(3)->GetStringValue(), CONSTLIT("noInvokeCheck")))
				pPower->Invoke(pObj, pTarget, &sError);
			else if (pObj->IsPlayer())
				pPower->InvokeByPlayer(pObj, pTarget, &sError);
			else
				pPower->InvokeByNonPlayer(pObj, pTarget, &sError);

			if (!sError.IsBlank())
				{
				::kernelDebugLogPattern("[%s %s Invoke]: %s", pObj->GetNounPhrase(), pPower->GetNounPhrase(), sError);
				return pCC->CreateNil();
				}
			return pCC->CreateTrue();
			}

		case FN_OBJ_FIRE_WEAPON:
			{
			CInstalledDevice *pDevice = GetDeviceFromItem(*pCC, pObj, pArgs->GetElement(1));
			if (pDevice == NULL)
				return pCC->CreateError(CONSTLIT("Item is not an installed device on object"), pArgs->GetElement(1));

			CItemCtx WeaponCtx(pObj, pDevice);

			CSpaceObject *pTarget = CreateObjFromItem(*pCC, pArgs->GetElement(2));

			if (pTarget) pTarget->SetDestructionNotify();
			pDevice->SetFireAngle(-1);
			pDevice->SetTarget(pTarget);

			bool bSourceDestroyed = false;
			bool bConsumedItems = false;
			bool bSuccess = false;

			bool ReadyToFire;

			if (pArgs->GetCount() >= 5 && !(pArgs->GetElement(4)->IsNil()))
				ReadyToFire = pDevice->IsReady();
			else
				ReadyToFire = true;

			// Save the variables changed in OnFireWeapon first.
			if (ReadyToFire)
				{
				ICCItem *p_OldFireAngle = pCC->LookupGlobal(CONSTLIT("aFireAngle"), pCtx);
				CVector vOldFirePos;
				GetPosOrObject(pEvalCtx, pCC->LookupGlobal(CONSTLIT("aFirePos"), pCtx), &vOldFirePos);
				ICCItem *p_OldFireRepeat = pCC->LookupGlobal(CONSTLIT("aFireRepeat"), pCtx);
				ICCItem *p_OldTargetObj = pCC->LookupGlobal(CONSTLIT("aTargetObj"), pCtx);
				ICCItem *p_OldWeaponBonus = pCC->LookupGlobal(CONSTLIT("aWeaponBonus"), pCtx);
				ICCItem *p_OldWeaponType = pCC->LookupGlobal(CONSTLIT("aWeaponType"), pCtx);

				bSuccess = pDevice->Activate(pObj, pTarget, &bSourceDestroyed, &bConsumedItems);

				pCtx->DefineInteger(CONSTLIT("aFireAngle"), p_OldFireAngle->GetIntegerValue());
				pCtx->DefineVector(CONSTLIT("aFirePos"), vOldFirePos);
				pCtx->DefineInteger(CONSTLIT("aFireRepeat"), p_OldFireRepeat->GetIntegerValue());
				pCtx->DefineSpaceObject(CONSTLIT("aTargetObj"), CreateObjFromItem(*pCC, p_OldTargetObj));
				pCtx->DefineInteger(CONSTLIT("aWeaponBonus"), p_OldWeaponBonus->GetIntegerValue());
				pCtx->DefineItemType(CONSTLIT("aWeaponType"), GetItemFromArg(*pCC, p_OldWeaponType).GetType());
				}

			if (bSourceDestroyed)
				return pCC->CreateTrue();

			if (bSuccess)
				{
				DEBUG_TRY
					//enhancements
					int iFireDelay;
				if (pArgs->GetCount() >= 4 && !(pArgs->GetElement(3)->IsNil()))
					iFireDelay = pArgs->GetElement(3)->GetIntegerValue();
				else
					iFireDelay = pDevice->GetClass()->GetActivateDelay(WeaponCtx);

				pDevice->SetTimeUntilReady(iFireDelay);
				DEBUG_CATCH
					return pCC->CreateTrue();
				}
			else return pCC->CreateNil();

			}

		case FN_OBJ_FIX_PARALYSIS:
			{
			pObj->ClearCondition(CConditionSet::cndParalyzed);
			return pCC->CreateTrue();
			}

		case FN_OBJ_IDENTIFIED:
			{
			pObj->SetIdentified(true);
			return pCC->CreateTrue();
			}

		case FN_OBJ_INC_ITEM_CHARGES:
			{
			//	Get the item

			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));

			//	Get the count

			ICCItem *pValue = (pArgs->GetCount() > 2 ? pArgs->GetElement(2) : NULL);
			int iCount = (pArgs->GetCount() > 3 ? pArgs->GetElement(3)->GetIntegerValue() : 1);

			//	Set it

			CString sError;
			if (!pObj->SetItemProperty(Item, CONSTLIT("incCharges"), pValue, iCount, &Item, &sError))
				{
				if (sError.IsBlank())
					return pCC->CreateNil();
				else
					return pCC->CreateError(sError);
				}

			//	Return the newly changed item

			return CreateListFromItem(*pCC, Item);
			}

		case FN_OBJ_INCREMENT_VELOCITY:
			{
			//	Set velocity (vel in percent of light-speed)

			CVector vVel = CreateVectorFromList(*pCC, pArgs->GetElement(1));
			const CVector &vNewVel = pObj->DeltaV(vVel);

			//	Return the resulting speed

			return CreateListFromVector(*pCC, vNewVel);
			}

		case FN_OBJ_POSITION:
			{
			//	Set vector

			CVector vPos;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vPos) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));

			pObj->Place(vPos, pObj->GetVel());

			//	Set rotation

			if (pArgs->GetCount() >= 3)
				{
				CString sError;
				if (!pObj->SetProperty(FIELD_ROTATION, pArgs->GetElement(2), &sError))
					return pCC->CreateError(sError);
				}

			//	Return the resulting position

			return pArgs->GetElement(1)->Reference();
			}

		case FN_OBJ_RECORD_BUY_ITEM:
			{
			int iArg = 1;
			CSpaceObject *pSellerObj = CreateObjFromItem(*pCC, pArgs->GetElement(iArg++));
			if (pSellerObj == NULL)
				return pCC->CreateNil();

			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(iArg++)));

			CCurrencyAndValue Value;
			if (pArgs->GetCount() >= 5)
				{
				ICCItem *pValue = pArgs->GetElement(iArg++);
				Value.SetCurrencyType(GetEconomyTypeFromItem(*pCC, pValue));
				if (Value.GetCurrencyType() == NULL)
					return pCC->CreateError(CONSTLIT("Invalid economy type"), pValue);
				}
			else if (pArgs->GetCount() < 4)
				return pCC->CreateNil();
			else
				Value.SetCurrencyType(pSellerObj->GetDefaultEconomy());

			Value.SetValue(pArgs->GetElement(iArg++)->GetIntegerValue());

			pObj->RecordBuyItem(pSellerObj, Item, Value);
			return pCC->CreateTrue();
			}

		case FN_OBJ_REGISTER_SYSTEM_EVENTS:
			if (pObj->IsDestroyed())
				return pCC->CreateNil();

			pObj->GetSystem()->RegisterEventHandler(pObj, pArgs->GetElement(1)->GetIntegerValue() * LIGHT_SECOND);
			return pCC->CreateTrue();

		case FN_OBJ_REMOVE_ITEM_ENHANCEMENT:
			{
			//	Item and enhancement ID

			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
			DWORD dwID = (DWORD)pArgs->GetElement(2)->GetIntegerValue();

			//	Remove

			pObj->RemoveItemEnhancement(Item, dwID);

			//	Done

			return pCC->CreateTrue();
			}

		case FN_OBJ_REMOVE_OVERLAY:
			pObj->RemoveOverlay(pArgs->GetElement(1)->GetIntegerValue());
			return pCC->CreateTrue();

		case FN_OBJ_RESUME:
			{
			CVector vPos;
			CSpaceObject *pGate = NULL;

			if (pObj->IsDestroyed())
				return pCC->CreateNil();

			pObj->Resume();

			if (pArgs->GetCount() > 1)
				{
				if (GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vPos, &pGate) != NOERROR)
					return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));

				//	Place the object at the gate or position

				if (pGate)
					pObj->GetSystem()->PlaceInGate(pObj, pGate);
				else
					pObj->Place(vPos);
				}

			return pCC->CreateTrue();
			}

		case FN_OBJ_CLEAR_AS_DESTINATION:
			pObj->ClearPlayerDestination();
			return pCC->CreateTrue();

		case FN_OBJ_SET_ABILITY:
			{
			int i;
			int iArg = 1;

			//	Get the ability to change

			Abilities iAbility = ::AbilityDecode(pArgs->GetElement(iArg)->GetStringValue());
			if (iAbility == ablUnknown)
				return pCC->CreateError(CONSTLIT("Unknown equipment"), pArgs->GetElement(iArg));
			iArg++;

			//	Get the desired change

			AbilityModifications iChange = ::AbilityModificationDecode(pArgs->GetElement(iArg)->GetStringValue());
			if (iChange == ablModificationUnknown)
				return pCC->CreateError(CONSTLIT("Unknown equipment command"), pArgs->GetElement(iArg));
			iArg++;

			//	Get the optional duration

			int iDuration = -1;
			if (pArgs->GetCount() > iArg && pArgs->GetElement(iArg)->IsInteger())
				{
				iDuration = pArgs->GetElement(iArg)->GetIntegerValue();
				iArg++;
				}

			//	Get optional options

			DWORD dwOptions = 0;
			if (pArgs->GetCount() > iArg)
				{
				for (i = 0; i < pArgs->GetElement(iArg)->GetCount(); i++)
					{
					DWORD dwOption = ::AbilityModificationOptionDecode(pArgs->GetElement(iArg)->GetElement(i)->GetStringValue());
					if (dwOption == ablOptionUnknown)
						return pCC->CreateError(CONSTLIT("Unknown equipment option"), pArgs->GetElement(iArg)->GetElement(i));

					dwOptions |= dwOption;
					}
				iArg++;
				}

			//	Set the ability

			bool bChanged = pObj->SetAbility(iAbility, iChange, iDuration, dwOptions);

			//	Done

			return pCC->CreateBool(bChanged);
			}

		case FN_OBJ_SET_AS_DESTINATION:
			{
			int i;
			bool bShowDistanceAndBearing = false;
			bool bAutoClearDestination = false;
			bool bAutoClearOnDestroy = false;
			bool bAutoClearOnDock = false;
			bool bShowHighlight = false;

			//	If we have an extra argument then it is either a boolean (which
			//	is old-style) or it is a string or list of string.

			if (pArgs->GetCount() == 2)
				{
				ICCItem *pOptions = pArgs->GetElement(1);
				if (!pOptions->IsNil() 
						&& (pOptions->IsIdentifier() || pOptions->IsList()))
					{
					for (i = 0; i < pOptions->GetCount(); i++)
						{
						CString sOption = pOptions->GetElement(i)->GetStringValue();

						if (strEquals(sOption, CONSTLIT("autoClear")))
							bAutoClearDestination = true;
						else if (strEquals(sOption, CONSTLIT("autoClearOnDestroy")))
							bAutoClearOnDestroy = true;
						else if (strEquals(sOption, CONSTLIT("autoClearOnDock")))
							bAutoClearOnDock = true;
						else if (strEquals(sOption, CONSTLIT("showDistance")))
							bShowDistanceAndBearing = true;
						else if (strEquals(sOption, CONSTLIT("showHighlight")))
							bShowHighlight = true;
						else
							return pCC->CreateError(strPatternSubst(CONSTLIT("Invalid option: %s"), sOption));
						}
					}
				else
					bShowDistanceAndBearing = !pOptions->IsNil();
				}

			//	Otherwise, if we have more than two arguments, then we expect the
			//	old-style flags

			else if (pArgs->GetCount() > 2)
				{
				bShowDistanceAndBearing = !pArgs->GetElement(1)->IsNil();
				bAutoClearDestination = !pArgs->GetElement(2)->IsNil();
				}

			//	Set the options

			pObj->SetPlayerDestination();
			if (bShowDistanceAndBearing)
				pObj->SetShowDistanceAndBearing();
			if (bAutoClearDestination)
				pObj->SetAutoClearDestination();
			if (bAutoClearOnDestroy)
				pObj->SetAutoClearDestinationOnDestroy();
			if (bAutoClearOnDock)
				pObj->SetAutoClearDestinationOnDock();
			if (bShowHighlight)
				pObj->SetShowHighlight();

			return pCC->CreateTrue();
			}

		case FN_OBJ_SET_CHARACTER_DATA:
			{
			CDesignType *pCharacter = pObj->GetCharacter();
			if (pCharacter == NULL)
				pCharacter = pObj->GetType();
			if (pCharacter == NULL)
				return pCC->CreateNil();

			CString sAttrib = pArgs->GetElement(1)->GetStringValue();
			if (sAttrib.IsBlank())
				return pCC->CreateNil();

			pCharacter->SetGlobalData(sAttrib, pArgs->GetElement(2));
			return pCC->CreateTrue();
			}

		case FN_OBJ_SET_ITEM_CHARGES:
			{
			//	Get the item

			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));

			//	Get the (optional) count

			int iCount = (pArgs->GetCount() > 3 ? pArgs->GetElement(3)->GetIntegerValue() : 1);

			//	Set charges

			CString sError;
			if (!pObj->SetItemProperty(Item, CONSTLIT("charges"), pArgs->GetElement(2), iCount, &Item, &sError))
				{
				if (sError.IsBlank())
					return pCC->CreateNil();
				else
					return pCC->CreateError(sError);
				}

			//	Return the newly changed item

			return CreateListFromItem(*pCC, Item);
			}

		case FN_OBJ_SET_ITEM_DATA:
			{
			//	Get the item

			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));

			//	Get the attribute

			CString sAttrib = pArgs->GetElement(2)->GetStringValue();
			int iCount = (pArgs->GetCount() > 4 ? Max(0, pArgs->GetElement(4)->GetIntegerValue()) : 1);

			//	Select the item

			CItemListManipulator ItemList(pObj->GetItemList());
			if (!ItemList.SetCursorAtItem(Item))
				{
				if (pCtx->GetAPIVersion() >= 18)
					return pCC->CreateError(CONSTLIT("Unable to find specified item in object."), pArgs->GetElement(1));
				else
					return pCC->CreateNil();
				}

			//	Set the data

			ItemList.SetDataAtCursor(sAttrib, pArgs->GetElement(3), iCount);

			//	Update the object

			pObj->InvalidateItemListState();

			//	Return the newly changed item

			return CreateListFromItem(*pCC, ItemList.GetItemAtCursor());
			}

		case FN_OBJ_SET_ITEM_PROPERTY:
			{
			//	Get the item

			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
			if (Item.GetType() == NULL)
				return pCC->CreateNil();

			//	Get the property

			CString sProperty = pArgs->GetElement(2)->GetStringValue();
			ICCItem *pValue = (pArgs->GetCount() > 3 ? pArgs->GetElement(3) : NULL);
			int iCount = (pArgs->GetCount() > 4 ? Max(0, pArgs->GetElement(4)->GetIntegerValue()) : 1);

			//	Set it

			CString sError;
			if (!pObj->SetItemProperty(Item, sProperty, pValue, iCount, &Item, &sError))
				{
				if (sError.IsBlank())
					return pCC->CreateNil();
				else
					return pCC->CreateError(sError);
				}

			//	Return the newly changed item

			return CreateListFromItem(*pCC, Item);
			}

		case FN_OBJ_SET_EVENT_HANDLER:
			if (pArgs->GetElement(1)->IsNil())
				//	NOTE: This will set it back to the default handler from the class
				//	(if any).
				pObj->SetOverride(NULL);
			else
				{
				DWORD dwUNID = pArgs->GetElement(1)->GetIntegerValue();
				CDesignType *pType = g_pUniverse->FindDesignType(dwUNID);
				if (pType == NULL)
					return pCC->CreateError(strPatternSubst(CONSTLIT("Unable to find design type: %x"), dwUNID), pArgs);

				pObj->SetOverride(pType);
				}
			return pCC->CreateTrue();

		case FN_OBJ_SET_PROPERTY:
			{
			CString sError;
			if (!pObj->SetProperty(pArgs->GetElement(1)->GetStringValue(), pArgs->GetElement(2), &sError))
				{
				if (sError.IsBlank())
					return pCC->CreateError(CONSTLIT("Invalid property"), pArgs->GetElement(1));
				else
					return pCC->CreateError(sError);
				}

			return pCC->CreateTrue();
			}

		case FN_OBJ_SET_TRADE_DESC:
			{
			CEconomyType *pEcon = GetEconomyTypeFromItem(*pCC, pArgs->GetElement(1));
			if (pEcon == NULL)
				return pCC->CreateError(CONSTLIT("Invalid economy type"), pArgs->GetElement(1));

			int iMaxCurrency;
			if (pArgs->GetCount() > 2)
				{
				iMaxCurrency = pArgs->GetElement(2)->GetIntegerValue();
				if (iMaxCurrency < 0)
					return pCC->CreateError(CONSTLIT("Invalid currency value"), pArgs->GetElement(2));
				}
			else
				iMaxCurrency = 0;

			int iReplenishCurrency;
			if (pArgs->GetCount() > 3)
				{
				iReplenishCurrency = pArgs->GetElement(3)->GetIntegerValue();
				if (iReplenishCurrency < 0)
					return pCC->CreateError(CONSTLIT("Invalid currency value"), pArgs->GetElement(3));
				}
			else
				iReplenishCurrency = 0;

			pObj->SetTradeDesc(pEcon, iMaxCurrency, iReplenishCurrency);
			return pCC->CreateTrue();
			}

		case FN_OBJ_SUSPEND:
			if (pObj->IsDestroyed())
				return pCC->CreateNil();

			pObj->Suspend();
			return pCC->CreateTrue();

		case FN_OBJ_UNREGISTER_SYSTEM_EVENTS:
			pObj->GetSystem()->UnregisterEventHandler(pObj);
			return pCC->CreateTrue();

		case FN_OBJ_VELOCITY:
			{
			//	Set velocity 

			CVector vVel = CreateVectorFromList(*pCC, pArgs->GetElement(1));
			pObj->SetVel(vVel);

			//	Return the resulting speed

			return CreateListFromVector(*pCC, pObj->GetVel());
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

ICCItem *fnObjSetOld (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnObjSetOld
//
//	Sets data about a space object

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	ICCItem *pArgs;
	ICCItem *pResult;

	//	Evaluate the arguments and validate them

	pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("i*"));
	if (pArgs->IsError())
		return pArgs;

	//	Convert the first argument into a space object

	CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0));
	if (pObj == NULL)
		{
		pArgs->Discard(pCC);
		return pCC->CreateNil();
		}

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_OBJ_ADD_SUBORDINATE:
			{
			CSpaceObject *pSubordinate = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			pArgs->Discard(pCC);

			if (pSubordinate && !pSubordinate->IsDestroyed())
				{
				pObj->AddSubordinate(pSubordinate);
				pResult = pCC->CreateTrue();
				}
			else
				pResult = pCC->CreateNil();
			break;
			}

		case FN_OBJ_NAME:
			{
			//	If name is Nil then we clear the name

			if (pArgs->GetElement(1)->IsNil())
				{
				pObj->SetName(NULL_STR, 0);
				}

			//	Otherwise...

			else
				{
				//	Third parameter is (optional) flags

				DWORD dwFlags;
				if (pArgs->GetCount() > 2)
					dwFlags = (DWORD)pArgs->GetElement(2)->GetIntegerValue();
				else
					dwFlags = 0;

				//	Set name

				pObj->SetName(pArgs->GetElement(1)->GetStringValue(), dwFlags);
				}

			pArgs->Discard(pCC);
			pResult = pCC->CreateTrue();
			break;
			}

		case FN_OBJ_JUMP:
			{
			//	Second param is vector

			CVector vPos;
			ALERROR error = GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vPos);
			pArgs->Discard(pCC);

			if (error)
				{
				pResult = pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));
				break;
				}

			//	Jump

			pObj->Jump(vPos);

			//	Done

			pResult = pCC->CreateTrue();
			break;
			}

		case FN_OBJ_PARALYSIS:
			{
			int iTime = pArgs->GetElement(1)->GetIntegerValue();
			pArgs->Discard(pCC);

			pObj->SetCondition(CConditionSet::cndParalyzed, iTime);
			pResult = pCC->CreateTrue();
			break;
			}

		case FN_OBJ_POSITION:
			{
			//	Second param is vector

			CVector vPos;
			ALERROR error = GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vPos);
			pArgs->Discard(pCC);

			if (error)
				{
				pResult = pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));
				break;
				}

			//	Move the ship

			pObj->Place(vPos);
			pResult = pCC->CreateTrue();
			break;
			}

		case FN_OBJ_REGISTER_EVENTS:
			{
			CSpaceObject *pObj2 = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			pArgs->Discard(pCC);

			if (pObj2)
				{
				pObj2->AddEventSubscriber(pObj);
				pResult = pCC->CreateTrue();
				}
			else
				pResult = pCC->CreateNil();
			break;
			}

		case FN_OBJ_SOVEREIGN:
			{
			DWORD dwSovereignID = pArgs->GetElement(1)->GetIntegerValue();
			pArgs->Discard(pCC);

			CSovereign *pSovereign = g_pUniverse->FindSovereign(dwSovereignID);
			if (pSovereign)
				{
				pObj->SetSovereign(pSovereign);
				pResult = pCC->CreateTrue();
				}
			else
				pResult = pCC->CreateNil();

			break;
			}

		case FN_OBJ_UNREGISTER_EVENTS:
			{
			CSpaceObject *pObj2 = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			pArgs->Discard(pCC);

			pObj2->RemoveEventSubscriber(pObj);
			pResult = pCC->CreateTrue();
			break;
			}

		default:
			ASSERT(FALSE);
		}

	return pResult;
	}

ICCItem *fnObjItem (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnObjItem
//
//	Gets and sets items

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Convert the first argument into a space object

	CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0));
	if (pObj == NULL)
		return pCC->CreateNil();

	//	Second argument is an item

	CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
	if (Item.IsEmpty())
		return pCC->CreateNil();

	//	Do command

	switch (dwData)
		{
		case FN_OBJ_HAS_ITEM:
			{
			//	We have two optional arguments: a count and options.

			int iCount = Item.GetCount();
			ICCItem *pOptions = NULL;

			int iArg = 2;
			if (pArgs->GetCount() > iArg && pArgs->GetElement(iArg)->IsNumber())
				iCount = pArgs->GetElement(iArg++)->GetIntegerValue();

			if (pArgs->GetCount() > iArg)
				pOptions = pArgs->GetElement(iArg++);

			//	Generate flags

			DWORD dwFlags = CItem::ParseFlags(pOptions);

			//	Find

			CItemListManipulator ObjList(pObj->GetItemList());
			if (!ObjList.SetCursorAtItem(Item, dwFlags))
				return pCC->CreateNil();

			if (ObjList.GetItemAtCursor().GetCount() < iCount)
				return pCC->CreateNil();

			return pCC->CreateInteger(ObjList.GetItemAtCursor().GetCount());
			}

		case FN_OBJ_REMOVE_ITEM:
			{
			//	We have two optional arguments: a count and options.

			int iCount = Item.GetCount();
			ICCItem *pOptions = NULL;

			int iArg = 2;
			if (pArgs->GetCount() > iArg && pArgs->GetElement(iArg)->IsNumber())
				iCount = pArgs->GetElement(iArg++)->GetIntegerValue();

			if (pArgs->GetCount() > iArg)
				pOptions = pArgs->GetElement(iArg++);

			//	Generate flags

			DWORD dwFlags = CItem::ParseFlags(pOptions);

			//	Remove

			CItemListManipulator ObjList(pObj->GetItemList());
			if (!ObjList.SetCursorAtItem(Item, dwFlags))
				return pCC->CreateNil();

			if (ObjList.GetItemAtCursor().IsInstalled())
				return pCC->CreateError(CONSTLIT("Installed items cannot be removed; use (shpRemoveDevice) instead"));

			ObjList.DeleteAtCursor(iCount);

			pObj->OnComponentChanged(comCargo);
			pObj->ItemsModified();
			pObj->InvalidateItemListAddRemove();
			return pCC->CreateTrue();
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

ICCItem *fnObjItemOld (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnObjItemOld
//
//	Gets and sets items
//
//	(objAddItem obj item)
//	(objRemoveItem obj item [count])

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	ICCItem *pArgs;
	ICCItem *pResult;

	//	Evaluate the arguments and validate them

	if (dwData == FN_OBJ_ENUM_ITEMS)
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("is"));
	else
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("il*"));
	if (pArgs->IsError())
		return pArgs;

	//	Convert the first argument into a space object

	CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0));
	if (pObj == NULL)
		{
		pArgs->Discard(pCC);
		return pCC->CreateNil();
		}

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_OBJ_REMOVE_ITEM:
			{
			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
			int iCount = Item.GetCount();
			if (pArgs->GetElement(2))
				iCount = pArgs->GetElement(2)->GetIntegerValue();
			pArgs->Discard(pCC);

			CItemListManipulator ObjList(pObj->GetItemList());
			if (!ObjList.SetCursorAtItem(Item))
				return pCC->CreateNil();

			if (ObjList.GetItemAtCursor().IsInstalled())
				return pCC->CreateError(CONSTLIT("Installed items cannot be removed; use (shpRemoveDevice) instead"));

			ObjList.DeleteAtCursor(iCount);

			pObj->OnComponentChanged(comCargo);
			pObj->ItemsModified();
			pObj->InvalidateItemListAddRemove();
			pResult = pCC->CreateTrue();
			break;
			}

		case FN_OBJ_ENUM_ITEMS:
			{
			CString sCriteria = pArgs->GetElement(1)->GetStringValue();
			pArgs->Discard(pCC);

			CItemCriteria Criteria;
			CItem::ParseCriteria(sCriteria, &Criteria);

			pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;

			CItemListManipulator ObjList(pObj->GetItemList());
			ObjList.ResetCursor();
			while (ObjList.MoveCursorForward())
				{
				if (ObjList.GetItemAtCursor().MatchesCriteria(Criteria))
					{
					ICCItem *pItem = CreateListFromItem(*pCC, ObjList.GetItemAtCursor());
					pList->Append(*pCC, pItem);
					pItem->Discard(pCC);
					}
				}
			break;
			}

		default:
			ASSERT(FALSE);
		}

	return pResult;
	}

ICCItem *fnProgramDamage (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnProgramDamage
//
//	(objProgramDamage obj hacker progName aiLevel code)

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Evaluate the arguments and validate them

	ICCItem *pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("iisiu"));
	if (pArgs->IsError())
		return pArgs;

	CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0));
	if (pObj == NULL)
		{
		pArgs->Discard(pCC);
		return pCC->CreateNil();
		}

	CSpaceObject *pHacker = CreateObjFromItem(*pCC, pArgs->GetElement(1));
	if (pHacker == NULL)
		{
		pArgs->Discard(pCC);
		return pCC->CreateNil();
		}

	//	Create the program structure

	ProgramDesc Program;
	Program.iProgram = progCustom;
	Program.sProgramName = pArgs->GetElement(2)->GetStringValue();
	Program.iAILevel = pArgs->GetElement(3)->GetIntegerValue();
	Program.pCtx = pEvalCtx;
	Program.ProgramCode = pArgs->GetElement(4);

	//	Call the object

	pObj->ProgramDamage(pHacker, Program);

	//	Done

	pArgs->Discard(pCC);
	return pCC->CreateTrue();
	}

ICCItem *fnMission (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnMission
//
//	(msnCreate unid owner [data])

	{
	ALERROR error;
	int i, j;
	CCodeChain *pCC = pEvalCtx->pCC;

	switch (dwData)
		{
		case FN_MISSION_CAN_CREATE:
			{
			CMissionType *pType = g_pUniverse->FindMissionType(pArgs->GetElement(0)->GetIntegerValue());
			if (pType == NULL)
				return pCC->CreateError(CONSTLIT("Unknown mission type"), pArgs->GetElement(0));

			//	Get arguments

			CSpaceObject *pOwner = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			ICCItem *pData = pArgs->GetElement(2);

			//	See if we can create the mission

			return pCC->CreateBool(pType->CanBeCreated(pOwner, pData));
			}

		case FN_MISSION_CREATE:
			{
			//	Get the list of mission types, categorized by priority

			TSortMap<int, TArray<CMissionType *>> MissionsByPriority(DescendingSort);
			ICCItem *pList = pArgs->GetElement(0);
			for (i = 0; i < pList->GetCount(); i++)
				{
				CMissionType *pType = g_pUniverse->FindMissionType((DWORD)pList->GetElement(i)->GetIntegerValue());
				if (pType == NULL)
					return pCC->CreateError(strPatternSubst(CONSTLIT("Unknown mission type: %x."), pList->GetElement(i)->GetIntegerValue()), pList->GetElement(i));

				//	Get the proper category

				TArray<CMissionType *> *pCategory = MissionsByPriority.SetAt(pType->GetPriority());
				pCategory->Insert(pType);
				}

			//	Now generate a properly ordered list by priority (but randomized within
			//	each priority).

			TArray<CMissionType *> Missions;
			for (i = 0; i < MissionsByPriority.GetCount(); i++)
				{
				//	Randomize this category

				MissionsByPriority[i].Shuffle();

				//	Add in order

				for (j = 0; j < MissionsByPriority[i].GetCount(); j++)
					Missions.Insert(MissionsByPriority[i][j]);
				}

			//	Get arguments

			CSpaceObject *pOwner = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			ICCItem *pData = (pArgs->GetCount() >= 3 ? pArgs->GetElement(2) : NULL);

			//	Create the mission

			CString sError;
			CMission *pMission;
			if (error = g_pUniverse->CreateRandomMission(Missions, pOwner, pData, &pMission, &sError))
				{
				//	ERR_NOTFOUND means that conditions do not allow for the
				//	mission to be created. This is not technically an error; it
				//	just means that the caller must do something else.

				if (error = ERR_NOTFOUND)
					return pCC->CreateNil();

				//	Otherwise, we report an error

				else
					return pCC->CreateError(sError);
				}

			//	Return the mission object

			return pCC->CreateInteger((int)pMission);
			}

		case FN_MISSION_FIND:
			{
			//	Get arguments

			CSpaceObject *pSource;
			CString sCriteria;
			if (pArgs->GetCount() >= 2)
				{
				pSource = CreateObjFromItem(*pCC, pArgs->GetElement(0));
				sCriteria = pArgs->GetElement(1)->GetStringValue();
				}
			else
				{
				pSource = NULL;
				sCriteria = pArgs->GetElement(0)->GetStringValue();
				}

			//	Parse criteria

			CMission::SCriteria Criteria;
			if (!CMission::ParseCriteria(sCriteria, &Criteria))
				return pCC->CreateError(CONSTLIT("Unable to parse criteria"), pArgs->GetElement(1));

			//	Get list of missions

			TArray<CMission *> List;
			g_pUniverse->GetMissions(pSource, Criteria, &List);
			if (List.GetCount() == 0)
				return pCC->CreateNil();

			if (Criteria.bPriorityOnly)
				{
				//	Return a single mission

				int bestPriority = -1;
				j = 0;
				for (i = 0; i < List.GetCount(); i++)
					if (List[i]->GetPriority() > bestPriority)
						{
						j = i;
						bestPriority = List[i]->GetPriority();
						}
				
				return pCC->CreateInteger((int)List[j]);
				}
			else
				{
				//	Create a list to return

				ICCItem *pResult = pCC->CreateLinkedList();
				if (pResult->IsError())
					return pResult;

				CCLinkedList *pList = (CCLinkedList *)pResult;
				for (i = 0; i < List.GetCount(); i++)
					pList->AppendInteger(*pCC, (int)List[i]);

				//	Done

				return pResult;
				}
			}

		default:
			ASSERT(false);
			return NULL;
		}
	}

ICCItem *fnMissionGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnMissionGet
//
//

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_MISSION_GET_PROPERTY:
			{
			}

		default:
			ASSERT(false);
			return NULL;
		}
	}

ICCItem *fnMissionSet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnMissionSet
//
//	(msnSetAccepted missionObj)

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	//	Get the mission object

	CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0));
	if (pObj == NULL)
		return pCC->CreateError(CONSTLIT("Invalid object"), pArgs->GetElement(0));

	CMission *pMission = pObj->AsMission();
	if (pMission == NULL)
		return pCC->CreateError(CONSTLIT("Invalid mission object"), pArgs->GetElement(0));

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_MISSION_ACCEPTED:
			return pCC->CreateBool(pMission->SetAccepted());

		case FN_MISSION_ADD_TIMER:
		case FN_MISSION_ADD_RECURRING_TIMER:
			{
			int iTime = pArgs->GetElement(1)->GetIntegerValue();
			if (iTime <= 0 && dwData == FN_MISSION_ADD_RECURRING_TIMER)
				return pCC->CreateError(CONSTLIT("Invalid recurring time"), pArgs->GetElement(1));

			CString sEvent = pArgs->GetElement(2)->GetStringValue();

			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			//	Not valid for closed missions because we would never know when 
			//	to delete the event.

			if (pMission->IsClosed())
				return pCC->CreateError(CONSTLIT("Mission already closed"), pArgs->GetElement(0));

			//	Create the event

			CSystemEvent *pEvent;
			if (dwData == FN_MISSION_ADD_TIMER)
				pEvent = new CTimedMissionEvent(
						Max(0, g_pUniverse->GetTicks() + iTime),
						0,
						pMission,
						sEvent);
			else
				pEvent = new CTimedMissionEvent(
						Max(0, g_pUniverse->GetTicks() + mathRandom(0, iTime)),
						iTime,
						pMission,
						sEvent);

			g_pUniverse->AddEvent(pEvent);

			return pCC->CreateTrue();
			}

		case FN_MISSION_CANCEL_TIMER:
			return pCC->CreateBool(g_pUniverse->CancelEvent(pMission, pArgs->GetElement(1)->GetStringValue(), pCtx->InEvent(eventDoEvent)));

		case FN_MISSION_CLOSED:
			return pCC->CreateBool(pMission->SetUnavailable());

		case FN_MISSION_DECLINED:
			{
			ICCItem *pResult;
			if (!pMission->SetDeclined(&pResult))
				return pCC->CreateNil();
			else if (pResult == NULL)
				return pCC->CreateNil();
			else
				return pResult;
			}
			
		case FN_MISSION_FAILURE:
			return pCC->CreateBool(pMission->SetFailure((pArgs->GetCount() >= 2 ? pArgs->GetElement(1) : NULL)));

		case FN_MISSION_REWARD:
			{
			ICCItem *pData = (pArgs->GetCount() >= 2 ? pArgs->GetElement(1) : NULL);
			ICCItem *pResult;

			if (!pMission->Reward(pData, &pResult))
				return pCC->CreateNil();
			else if (pResult == NULL)
				return pCC->CreateNil();
			else
				return pResult;
			}

		case FN_MISSION_SET_PLAYER_TARGET:
			return pCC->CreateBool(pMission->SetPlayerTarget());
			
		case FN_MISSION_SUCCESS:
			return pCC->CreateBool(pMission->SetSuccess((pArgs->GetCount() >= 2 ? pArgs->GetElement(1) : NULL)));
			
		default:
			ASSERT(false);
			return NULL;
		}
	}

ICCItem *fnResourceGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnResourceGet
//
//	Gets data about resources

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_RESOURCE_COLOR_BLEND:
			{
			CG32bitPixel rgbDest = ::LoadRGBColor(pArgs->GetElement(0)->GetStringValue());
			CG32bitPixel rgbSrc = ::LoadRGBColor(pArgs->GetElement(1)->GetStringValue());
			
			//	The slider is either an integer from 0-255 or a double from 0-1.

			Metric rFade;
			if (pArgs->GetElement(2)->IsDouble())
				rFade = pArgs->GetElement(2)->GetDoubleValue();
			else
				rFade = (Metric)pArgs->GetElement(2)->GetIntegerValue() / 255.0;

			//	Make sure we're in range and generate the new color

			rFade = Max(0.0, Min(rFade, 1.0));
			CG32bitPixel rgbResult = CG32bitPixel::Blend(rgbDest, rgbSrc, rFade);

			//	Return the color

			return pCC->CreateString(GetRGBColor(rgbResult));
			}

		case FN_RESOURCE_CREATE_IMAGE_DESC:
			//	For now, image descs are pretty simple
			return pArgs->Reference();

		case FN_RESOURCE_GET_IMAGE_PROPERTY:
			{
			ICCItem *pImageDesc = pArgs->GetElement(0);
			CString sProperty = pArgs->GetElement(1)->GetStringValue();

			return GetImageDescProperty(*pCC, pImageDesc, sProperty);
			}

		default:
			ASSERT(FALSE);
		}

	return pCC->CreateNil();
	}

ICCItem *fnRollDice (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnRollDice
//
//	Rolls dice

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	switch (dwData)
		{
		case FN_ROLL_CHANCE:
			{
			Metric rProb = pArgs->GetElement(0)->GetDoubleValue() / 100.0;
			
			//	If we're trying multiple times, then we adjust the probability

			if (pArgs->GetCount() > 1)
				{
				Metric rRolls = pArgs->GetElement(1)->GetDoubleValue();
				if (rRolls <= 0.0)
					return pCC->CreateError(CONSTLIT("Number of rolls must be positive."), pArgs->GetElement(1));

				rProb = 1.0 - pow(1.0 - rProb, rRolls);
				}

			//	Roll

			return pCC->CreateBool(mathRandomDouble() <= rProb);
			}

		case FN_ROLL_DICE:
			{
			int iCount = pArgs->GetElement(0)->GetIntegerValue();
			int iFaces = pArgs->GetElement(1)->GetIntegerValue();
			int iBonus = 0;
			if (pArgs->GetElement(2))
				iBonus = pArgs->GetElement(2)->GetIntegerValue();

			DiceRange Dice(iFaces, iCount, iBonus);
			return pCC->CreateInteger(Dice.Roll());
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

ICCItem *fnShipClass (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnShipClass
//
//	Sets data about a ship class

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	ICCItem *pResult;

	//	Convert the first argument into a ship class

	DWORD dwUNID = pArgs->GetElement(0)->GetIntegerValue();
	CShipClass *pClass = g_pUniverse->FindShipClass(dwUNID);
	if (pClass == NULL)
		{
		CShip *pShip = CreateShipObjFromItem(*pCC, pArgs->GetElement(0));
		if (pShip)
			pClass = pShip->GetClass();

		if (pClass == NULL)
			return pCC->CreateError(CONSTLIT("ship class expected"), pArgs->GetElement(0));
		}

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_SHIP_CLASS_NAME:
			{
			//	We always add generic, or else we could get a personal name or 
			//	randomized ship name.

			DWORD dwFlags = pCtx->AsNameFlags(pArgs->GetElement(1)) | nounGeneric;

			pResult = pCC->CreateString(pClass->GetNounPhrase(dwFlags));
			break;
			}

		case FN_SHIP_DATA_FIELD:
			pResult = CreateResultFromDataField(*pCC, pClass->GetDataField(pArgs->GetElement(1)->GetStringValue()));
			break;

		case FN_SHIP_GET_GLOBAL_DATA:
			return pClass->GetGlobalData(pArgs->GetElement(1)->GetStringValue())->Reference();

		case FN_SHIP_GET_IMAGE_DESC:
			{
			int iRotation = 0;
			bool bHeroImage = false;

			if (pArgs->GetCount() > 1)
				{
				ICCItem *pOptions = pArgs->GetElement(1);
				if (pOptions->IsSymbolTable())
					{
					iRotation = pOptions->GetIntegerAt(FIELD_ROTATION);
					CString sType = pOptions->GetStringAt(FIELD_TYPE);
					if (strEquals(sType, TYPE_SCHEMATIC))
						bHeroImage = true;
					}
				else
					iRotation = pClass->Angle2Direction(pOptions->GetIntegerValue());
				}

			if (bHeroImage)
				pResult = CreateListFromImage(*pCC, pClass->GetHeroImage());
			else
				pResult = CreateListFromImage(*pCC, pClass->GetImage(), iRotation);

			break;
			}

		case FN_SHIP_SET_GLOBAL_DATA:
			{
			CString sAttrib = pArgs->GetElement(1)->GetStringValue();
			pClass->SetGlobalData(sAttrib, pArgs->GetElement(2));
			pResult = pCC->CreateTrue();
			break;
			}

		default:
			ASSERT(FALSE);
		}

	return pResult;
	}

ICCItem *fnShipGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnShipGet
//
//	Gets data about a ship

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Get the ship arg

	CShip *pShip = CreateShipObjFromItem(*pCC, pArgs->GetElement(0));
	if (pShip == NULL)
		return pCC->CreateNil();

	//	Do it

	switch (dwData)
		{
		case FN_SHIP_AI_SETTING:
			{
			//	LATER: For now the controller is responsible for returning all
			//	settings as strings (even if they are integers). In the future
			//	we should be smarter about this.

			CString sValue = pShip->GetController()->GetAISettingString(pArgs->GetElement(1)->GetStringValue());
			if (sValue.IsBlank())
				return pCC->CreateNil();
			else
				{
				bool bFailed;
				int iValue = strToInt(sValue, 0, &bFailed);
				if (!bFailed)
					return pCC->CreateInteger(iValue);
				else
					return pCC->CreateString(sValue);
				}
			}

		case FN_SHIP_DOCK_OBJ:
			{
			CSpaceObject *pObj = pShip->GetDockedObj();
			if (pObj == NULL)
				return pCC->CreateNil();

			return pCC->CreateInteger((int)pObj);
			}

		case FN_SHIP_IS_NAMED_DEVICE:
			{
			CItemListManipulator *pItemList = (CItemListManipulator *)pArgs->GetElement(1)->GetIntegerValue();
			const CItem &Item = pItemList->GetItemAtCursor();
			if (!Item.IsInstalled())
				return pCC->CreateNil();

			int iDevInt = pArgs->GetElement(2)->GetIntegerValue();
			if (iDevInt < 0 || iDevInt >= devNamesCount)
				return pCC->CreateNil();

			DeviceNames iDev = (DeviceNames)iDevInt;

			CItemListManipulator TestList(pShip->GetItemList());
			pShip->SetCursorAtNamedDevice(TestList, iDev);

			return pCC->CreateBool(TestList.GetItemAtCursor().GetInstalled() == Item.GetInstalled());
			}

		case FN_SHIP_IS_RADIATION_IMMUNE:
			{
			if (pArgs->GetCount() > 1)
				{
				CItem Item = GetItemFromArg(*pCC, pArgs->GetElement(1));
				CItemType *pType = Item.GetType();
				if (pType == NULL)
					return pCC->CreateNil();

				CArmorClass *pArmor = pType->GetArmorClass();
				if (pArmor == NULL)
					return pCC->CreateNil();

				CInstalledArmor *pInstalled = NULL;
				if (Item.IsInstalled())
					pInstalled = pShip->GetArmorSection(Item.GetInstalled());

				return pCC->CreateBool(pArmor->IsImmune(CItemCtx(pShip, pInstalled), specialRadiation));
				}
			else
				return pCC->CreateBool(pShip->IsImmuneTo(CConditionSet::cndRadioactive));
			break;
			}

		case FN_SHIP_MAX_SPEED:
			return pCC->CreateInteger((int)((100.0 * pShip->GetMaxSpeed() / LIGHT_SPEED) + 0.5));

		case FN_SHIP_ORDER_COUNT:
			return pCC->CreateInteger(pShip->GetController()->GetOrderCount());

		case FN_SHIP_ORDER_DESC:
			{
			IShipController *pController = pShip->GetController();
			if (pController == NULL)
				return pCC->CreateNil();

			int iIndex = (pArgs->GetCount() > 1 ? pArgs->GetElement(1)->GetIntegerValue() : 0);

			CSpaceObject *pTarget;
			IShipController::SData Data;
			IShipController::OrderTypes iOrder = pController->GetOrder(iIndex, &pTarget, &Data);
			if (iOrder == IShipController::orderNone)
				return pCC->CreateNil();

			//	Create result list

			ICCItem *pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;

			//	Add order name

			ICCItem *pItem = pCC->CreateString(GetOrderName(iOrder));
			pList->Append(*pCC, pItem);
			pItem->Discard(pCC);

			//	Add the target

			if (::OrderHasTarget(iOrder))
				{
				pItem = pCC->CreateInteger((int)pTarget);
				pList->Append(*pCC, pItem);
				pItem->Discard(pCC);
				}

			//	Add order data

			switch (Data.iDataType)
				{
				case IShipController::dataInteger:
					pList->AppendInteger(*pCC, Data.dwData1);
					break;

				case IShipController::dataPair:
					pList->AppendInteger(*pCC, Data.dwData1);
					pList->AppendInteger(*pCC, Data.dwData2);
					break;

				case IShipController::dataString:
					pList->AppendString(*pCC, Data.sData);
					break;

				case IShipController::dataVector:
					{
					ICCItem *pVector = ::CreateListFromVector(*pCC, Data.vData);
					pList->Append(*pCC, pVector);
					pVector->Discard(pCC);
					break;
					}
				}

			//	Done

			return pResult;
			}

		default:
			ASSERT(false);
		}

	return pCC->CreateNil();
	}

ICCItem *fnShipGetOld (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnShipGetOld
//
//	Gets data about a ship

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	ICCItem *pArgs;
	ICCItem *pResult;

	//	Evaluate the arguments and validate them

	pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("i"));
	if (pArgs->IsError())
		return pArgs;

	//	Convert the first argument into a ship

	CShip *pShip = CreateShipObjFromItem(*pCC, pArgs->GetElement(0));
	if (pShip == NULL)
		{
		pArgs->Discard(pCC);
		return pCC->CreateNil();
		}

	//	No longer needed

	pArgs->Discard(pCC);

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_SHIP_DEVICE_SLOT_AVAIL:
			pResult = pCC->CreateBool(pShip->IsDeviceSlotAvailable());
			break;

		case FN_SHIP_DIRECTION:
			pResult = pCC->CreateInteger(pShip->GetRotation());
			break;

		case FN_SHIP_FUEL:
			pResult = pCC->CreateDouble(Min(pShip->GetFuelLeft(), pShip->GetMaxFuel()));
			break;

		case FN_SHIP_HAS_AUTOPILOT:
			pResult = pCC->CreateBool(pShip->GetAbility(ablAutopilot) > 0);
			break;

		case FN_SHIP_INSTALL_AUTOPILOT:
			pShip->SetAbility(ablAutopilot, ablInstall, -1, 0);
			pResult = pCC->CreateTrue();
			break;

		case FN_SHIP_HAS_TARGETING:
			pResult = pCC->CreateBool(pShip->GetAbility(ablTargetingSystem) > 0);
			break;

		case FN_SHIP_INSTALL_TARGETING:
			pShip->SetAbility(ablTargetingSystem, ablInstall, -1, 0);
			pResult = pCC->CreateTrue();
			break;

		case FN_SHIP_DECONTAMINATE:
			pShip->ClearCondition(CConditionSet::cndRadioactive);
			pResult = pCC->CreateTrue();
			break;

		case FN_SHIP_MAKE_RADIOACTIVE:
			pShip->SetCondition(CConditionSet::cndRadioactive);
			pResult = pCC->CreateTrue();
			break;

		case FN_SHIP_IS_RADIOACTIVE:
			pResult = pCC->CreateBool(pShip->IsRadioactive());
			break;

		case FN_SHIP_ARMOR_COUNT:
			pResult = pCC->CreateInteger(pShip->GetArmorSectionCount());
			break;

		case FN_SHIP_IS_SRS_ENHANCED:
			pResult = pCC->CreateBool(pShip->GetAbility(ablExtendedScanner) > 0);
			break;

		case FN_SHIP_BLINDNESS:
			pResult = pCC->CreateBool(pShip->IsBlind());
			break;

		case FN_SHIP_ENHANCE_SRS:
			pShip->SetAbility(ablExtendedScanner, ablInstall, -1, 0);
			pResult = pCC->CreateTrue();
			break;

		case FN_SHIP_CANCEL_ORDERS:
			pShip->GetController()->CancelAllOrders();
			pResult = pCC->CreateTrue();
			break;

		case FN_SHIP_CLASS:
			pResult = pCC->CreateInteger(pShip->GetClass()->GetUNID());
			break;

		case FN_SHIP_SHIELD_ITEM_UNID:
			{
			CInstalledDevice *pShield = pShip->GetNamedDevice(devShields);
			if (pShield == NULL)
				{
				pResult = pCC->CreateNil();
				break;
				}

			CItemType *pType = pShield->GetClass()->GetItemType();
			if (pType == NULL)
				{
				pResult = pCC->CreateNil();
				break;
				}

			pResult = pCC->CreateInteger(pType->GetUNID());
			break;
			}

		case FN_SHIP_SHIELD_DAMAGE:
			{
			CInstalledDevice *pShield = pShip->GetNamedDevice(devShields);
			if (pShield == NULL)
				pResult = pCC->CreateInteger(0);
			else
				{
				int iHP, iMaxHP;
				pShield->GetStatus(pShip, &iHP, &iMaxHP);
				pResult = pCC->CreateInteger(iMaxHP - iHP);
				}

			break;
			}

		case FN_SHIP_SHIELD_MAX_HP:
			{
			CInstalledDevice *pShield = pShip->GetNamedDevice(devShields);
			if (pShield == NULL)
				pResult = pCC->CreateInteger(0);
			else
				{
				int iHP, iMaxHP;
				pShield->GetStatus(pShip, &iHP, &iMaxHP);
				pResult = pCC->CreateInteger(iMaxHP);
				}

			break;
			}

		case FN_SHIP_ORDER:
			{
			IShipController *pController = pShip->GetController();
			if (pController)
				{
				CString sOrder = GetOrderName(pController->GetCurrentOrderEx());
				if (!sOrder.IsBlank())
					pResult = pCC->CreateString(sOrder);
				else
					pResult = pCC->CreateNil();
				}
			else
				pResult = pCC->CreateNil();
			break;
			}

		case FN_SHIP_ORDER_TARGET:
			{
			IShipController *pController = pShip->GetController();
			if (pController)
				{
				CSpaceObject *pTarget;
				IShipController::OrderTypes iOrder = pController->GetCurrentOrderEx(&pTarget);
				if (::OrderHasTarget(iOrder) && pTarget)
					pResult = pCC->CreateInteger((int)pTarget);
				else
					pResult = pCC->CreateNil();
				}
			else
				pResult = pCC->CreateNil();
			break;
			}

		default:
			ASSERT(FALSE);
		}

	return pResult;
	}

ICCItem *fnShipSet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnShipSet
//
//	Sets data about a ship

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	//	Get the ship arg

	CShip *pShip = CreateShipObjFromItem(*pCC, pArgs->GetElement(0));
	if (pShip == NULL)
		return pCC->CreateNil();

	//	Do it

	switch (dwData)
		{
		case FN_SHIP_AI_SETTING:
			{
			CString sSetting = pArgs->GetElement(1)->GetStringValue();
			ICCItem *pValue = pArgs->GetElement(2);

			if (pValue->IsInteger())
				{
				int iNewValue = pShip->SetAISettingInteger(sSetting, pValue->GetIntegerValue());
				return pCC->CreateInteger(iNewValue);
				}
			else
				{
				CString sNewValue = pShip->SetAISettingString(sSetting, (pValue->IsNil() ? NULL_STR : pValue->GetStringValue()));
				pShip->UpdateNoFriendlyFire();
				if (sNewValue.IsBlank())
					return pCC->CreateNil();
				else
					{
					bool bFailed;
					int iValue = strToInt(sNewValue, 0, &bFailed);
					if (!bFailed)
						return pCC->CreateInteger(iValue);
					else
						return pCC->CreateString(sNewValue);
					}
				}
			}

		case FN_SHIP_CANCEL_ORDER:
			{
			int iIndex = (pArgs->GetCount() > 1 ? pArgs->GetElement(1)->GetIntegerValue() : 0);
			return pCC->CreateBool(pShip->GetController()->CancelOrder(iIndex));
			}

		case FN_SHIP_CAN_INSTALL_ARMOR:
		case FN_SHIP_CAN_INSTALL_DEVICE:
			{
			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
			int iSlot = (pArgs->GetCount() > 2 ? pArgs->GetElement(2)->GetIntegerValue() : -1);

			//	Check standard conditions

			CSpaceObject::InstallItemResults iResult;
			CString sResult;
			pShip->CanInstallItem(Item, iSlot, &iResult, &sResult);

			if (!sResult.IsBlank())
				return pCC->CreateString(sResult);
			else
				return pCC->CreateInteger(CSpaceObject::ConvertToCompatibleIndex(Item, iResult));
			}

		case FN_SHIP_CAN_REMOVE_DEVICE:
			{
			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));

			//	See if we can remove it

			CString sResult;
			CShip::RemoveDeviceStatus iStatus = pShip->CanRemoveDevice(Item, &sResult);

			//	Result based on status

			switch (iStatus)
				{
				case CShip::remOK:
					return pCC->CreateInteger(0);

				case CShip::remCannotRemove:
					return pCC->CreateString(sResult);

				default:
					return pCC->CreateInteger((int)iStatus);
				}
			}

		case FN_SHIP_COMMAND_CODE:
			{
			if (pArgs->GetElement(1)->IsNil())
				pShip->SetCommandCode(NULL);
			else
				pShip->SetCommandCode(pArgs->GetElement(1));
			return pCC->CreateTrue();
			}

		case FN_SHIP_CONSUME_FUEL:
			{
			Metric rFuel = pArgs->GetElement(1)->GetDoubleValue();
			CString sUse = (pArgs->GetCount() > 2 ? pArgs->GetElement(2)->GetStringValue() : NULL_STR);
			CReactorDesc::EFuelUseTypes iUse;
			if (strEquals(sUse, CONSTLIT("drain")))
				iUse = CReactorDesc::fuelDrain;
			else
				iUse = CReactorDesc::fuelConsume;

			pShip->ConsumeFuel(rFuel, iUse);
			return pCC->CreateDouble(Min(pShip->GetFuelLeft(), pShip->GetMaxFuel()));
			}

		case FN_SHIP_DAMAGE_ARMOR:
			{
			CInstalledArmor *pArmor = GetArmorSectionArg(*pCC, pArgs->GetElement(1), pShip);
			if (pArmor == NULL)
				return pCC->CreateError("Invalid armor segment", pArgs->GetElement(1));

			DamageTypes iDamageType = GetDamageTypeFromArg(*pCC, pArgs->GetElement(2));
			if (iDamageType == damageError)
				return pCC->CreateError("Invalid damage type", pArgs->GetElement(2));

			int iDamage = pArgs->GetElement(3)->GetIntegerValue();

			//	Compose damage desc

			DamageDesc Damage(iDamageType, DiceRange(0, 0, iDamage));

			//	Optionally disable SRS flashing

			if (pArgs->GetCount() > 4)
				if (strEquals(pArgs->GetElement(4)->GetStringValue(), CONSTLIT("noSRSFlash")))
					Damage.SetNoSRSFlash();

			//	Damage

			int iResult = pShip->DamageArmor(pArmor->GetSect(), Damage);

			//	Done

			return pCC->CreateInteger(iResult);
			}

		case FN_SHIP_FIX_BLINDNESS:
			{
			bool bNoMessage = (pArgs->GetCount() > 1 && !pArgs->GetElement(1)->IsNil());
			pShip->SetAbility(ablShortRangeScanner, 
					ablRepair, 
					-1, 
					(bNoMessage ? ablOptionNoMessage : 0));
			return pCC->CreateTrue();
			}

		case FN_SHIP_INSTALL_ARMOR:
			{
			CItem ArmorItem = GetItemFromArg(*pCC, pArgs->GetElement(1));
			int iSegment = pArgs->GetElement(2)->GetIntegerValue();

			//	Validate the armor item

			CItemType *pArmorType = ArmorItem.GetType();
			if (pArmorType == NULL
					|| pArmorType->GetArmorClass() == NULL
					|| ArmorItem.IsInstalled())
				return pCC->CreateError(CONSTLIT("Invalid armor item."));

			//	Make sure the segment is valid

			if (iSegment < 0 || iSegment >= pShip->GetArmorSectionCount())
				return pCC->CreateError(CONSTLIT("Invalid armor segment."));

			//	Select the item in the ship (the item must be on the ship for
			//	us to install it.

			CItemListManipulator ItemList(pShip->GetItemList());
			if (!ItemList.SetCursorAtItem(ArmorItem))
				{
				if (pCtx->GetAPIVersion() >= 18)
					return pCC->CreateError(CONSTLIT("Unable to find specified item in object."));
				else
					return pCC->CreateNil();
				}

			//	Install

			pShip->InstallItemAsArmor(ItemList, iSegment);
			return pCC->CreateTrue();
			}

		case FN_SHIP_INSTALL_DEVICE:
			{
			CItemListManipulator ItemList(pShip->GetItemList());

			//	If the argument is a list then it is an item (which means we have to find
			//	the item in the manipulator).

			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
			if (!ItemList.SetCursorAtItem(Item))
				{
				if (pCtx->GetAPIVersion() >= 18)
					return pCC->CreateError(CONSTLIT("Unable to find specified item in object."));
				else
					return pCC->CreateNil();
				}

			//	Make sure the item is in the proper state

			if (ItemList.GetItemAtCursor().IsInstalled())
				return pCC->CreateNil();

			//	See if we passed in a device slot

			int iDeviceSlot = -1;
			int iSlotPosIndex = -1;
			if (pArgs->GetCount() >= 3)
				{
				ICCItem *pOptions = pArgs->GetElement(2);
				if (pOptions->IsInteger())
					iDeviceSlot = pOptions->GetIntegerValue();
				else
					{
					ICCItem *pDeviceSlot = pOptions->GetElement(FIELD_DEVICE_SLOT);
					if (pDeviceSlot && !pDeviceSlot->IsNil())
						iDeviceSlot = pDeviceSlot->GetIntegerValue();

					ICCItem *pSlotPosIndex = pOptions->GetElement(FIELD_SLOT_POS_INDEX);
					if (pSlotPosIndex && !pSlotPosIndex->IsNil())
						iSlotPosIndex = pSlotPosIndex->GetIntegerValue();
					}

				if (iDeviceSlot != -1)
					{
					if (Item.IsDevice()
							&& (iDeviceSlot < 0 || iDeviceSlot >= pShip->GetDeviceCount() || pShip->GetDevice(iDeviceSlot)->IsEmpty()))
						return pCC->CreateError(CONSTLIT("Invalid device slot"), pArgs->GetElement(2));
					}
				}

			//	Otherwise, install or remove

			pShip->InstallItemAsDevice(ItemList, iDeviceSlot, iSlotPosIndex);

			//	Make sure the cursor is still valid

			if (!ItemList.IsCursorValid())
				return pCC->CreateError(CONSTLIT("Installation did not preserve cursor."));

			//	Done

			return CreateListFromItem(*pCC, ItemList.GetItemAtCursor());
			}

		case FN_SHIP_ORDER:
		case FN_SHIP_ORDER_IMMEDIATE:
			{
			IShipController::OrderTypes iOrder = GetOrderType(pArgs->GetElement(1)->GetStringValue());
			if (iOrder == IShipController::orderNone)
				return pCC->CreateError(CONSTLIT("Unknown order"), pArgs->GetElement(1));

			int iArg = 2;

			//	Get the target

			CSpaceObject *pTarget = NULL;
			bool bRequiredTarget;
			bool bHasTarget = OrderHasTarget(iOrder, &bRequiredTarget);
			if (pArgs->GetCount() > 2)
				{
				if (bHasTarget || pArgs->GetElement(iArg)->IsNil())
					{
					pTarget = CreateObjFromItem(*pCC, pArgs->GetElement(iArg));
					iArg++;
					}
				}

			//	Validate

			if (bRequiredTarget && pTarget == NULL)
				{
				if (g_pUniverse->InDebugMode())
					return pCC->CreateError(CONSTLIT("shpOrder requires target"), pArgs->GetElement(1));

				::kernelDebugLogPattern("ERROR: shpOrder %s requires target.", pArgs->GetElement(1)->GetStringValue());
				return pCC->CreateNil();
				}

			if (pTarget && pTarget->IsDestroyed())
				{
				if (g_pUniverse->InDebugMode())
					return pCC->CreateError(CONSTLIT("shpOrder target already destroyed"), pArgs->GetElement(1));

				::kernelDebugLogPattern("ERROR: shpOrder %s target already destroyed.", pArgs->GetElement(1)->GetStringValue());
				return pCC->CreateNil();
				}

			//	Get the data

			IShipController::SData Data;
			if (pArgs->GetCount() > (iArg + 1))
				{
				if (pArgs->GetElement(iArg)->IsNil() && pArgs->GetElement(iArg+1)->IsNil())
					{
					//	If both arguments are Nil, then we omit them both. We do this
					//	because some orders (like escort) behave differently depending
					//	on whether arguments are nil or not.
					}
				else
					{
					Data.iDataType = IShipController::dataPair;
					Data.dwData1 = pArgs->GetElement(iArg)->GetIntegerValue();
					Data.dwData2 = pArgs->GetElement(iArg+1)->GetIntegerValue();
					}
				}
			else if (pArgs->GetCount() > iArg)
				{
				if (pArgs->GetElement(iArg)->IsNil())
					{
					//	Nil argument is empty. We treat this the same as if the
					//	caller specified no argument.
					}
				else if (OrderHasDataString(iOrder))
					{
					Data.iDataType = IShipController::dataString;
					Data.sData = pArgs->GetElement(iArg)->GetStringValue();
					}
				else if (OrderHasDataVector(iOrder))
					{
					Data.iDataType = IShipController::dataVector;
					Data.vData = ::CreateVectorFromList(*pCC, pArgs->GetElement(iArg));
					}
				else
					{
					Data.iDataType = IShipController::dataInteger;
					Data.dwData1 = pArgs->GetElement(iArg)->GetIntegerValue();
					}
				}

			//	Done

			pShip->GetController()->AddOrder(iOrder, pTarget, Data, (dwData == FN_SHIP_ORDER_IMMEDIATE));
			return pCC->CreateTrue();
			}

		case FN_SHIP_ORDER_GOTO:
			{
			CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(1));

			if (pObj && !pObj->IsDestroyed())
				{
				pShip->GetController()->AddOrder(IShipController::orderGoTo, pObj, IShipController::SData());
				return pCC->CreateTrue();
				}
			else
				return pCC->CreateNil();
			}

		case FN_SHIP_PLAYER_WINGMAN:
			{
			bool bIsWingman = (pArgs->GetCount() > 1 ? !pArgs->GetElement(1)->IsNil() : true);
			pShip->SetPlayerWingman(bIsWingman);
			return pCC->CreateTrue();
			}

		case FN_SHIP_RECHARGE_SHIELD:
			{
			CInstalledDevice *pShield = pShip->GetNamedDevice(devShields);
			if (pShield == NULL)
				return pCC->CreateNil();

			CItemCtx ItemCtx(pShip, pShield);

			//	If no second argument then we always recharge to full

			int iHPToRecharge;
			if (pArgs->GetCount() < 2)
				pShield->GetHitPoints(ItemCtx, &iHPToRecharge);
			else
				iHPToRecharge = pArgs->GetElement(1)->GetIntegerValue();

			//	Recharge and return new HPs

			pShield->Recharge(pShip, iHPToRecharge);
			return pCC->CreateInteger(pShield->GetHitPoints(ItemCtx));
			}

		case FN_SHIP_REPAIR_ITEM:
			{
			CItemListManipulator *pItemList = NULL;
			CItemListManipulator ItemList(pShip->GetItemList());

			//	If the argument is a list then it is an item (which means we have to find
			//	the item in the manipulator). If the argument is an integer then we expect
			//	an item list manipulator pointer.

			if (pArgs->GetElement(1)->IsInteger())
				pItemList = (CItemListManipulator *)pArgs->GetElement(1)->GetIntegerValue();
			else
				{
				CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
				if (!ItemList.SetCursorAtItem(Item))
					{
					if (pCtx->GetAPIVersion() >= 18)
						return pCC->CreateError(CONSTLIT("Unable to find specified item in object."), pArgs->GetElement(1));
					else
						return pCC->CreateNil();
					}

				pItemList = &ItemList;
				}

			if (pItemList == NULL)
				return pCC->CreateNil();
			else if (pItemList->GetItemAtCursor().IsDamaged())
				{
				pShip->EnhanceItem(*pItemList, etBinaryEnhancement);
				return pCC->CreateTrue();
				}
			else
				return pCC->CreateNil();

			break;
			}

		default:
			ASSERT(false);
		}

	return pCC->CreateNil();
	}

ICCItem *fnShipSetOld (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnShipSetOld
//
//	Sets data about a ship

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	ICCItem *pArgs;
	ICCItem *pResult;

	//	Evaluate the arguments and validate them

	if (dwData == FN_SHIP_REMOVE_DEVICE 
			|| dwData == FN_SHIP_ITEM_CHARGES 
			|| dwData == FN_SHIP_DAMAGE_ITEM)
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("iv"));
	else if (dwData == FN_SHIP_ADD_ENERGY_FIELD)
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("iii"));
	else if (dwData == FN_SHIP_RECHARGE_ITEM)
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("ivi"));
	else if (dwData == FN_SHIP_FUEL_NEEDED 
			|| dwData == FN_SHIP_REFUEL_FROM_ITEM 
			|| dwData == FN_SHIP_ITEM_DEVICE_NAME
			|| dwData == FN_SHIP_IS_FUEL_COMPATIBLE)
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("il"));
	else if (dwData == FN_SHIP_ORDER_GATE 
			|| dwData == FN_SHIP_ORDER_PATROL 
			|| dwData == FN_SHIP_ORDER_ESCORT 
			|| dwData == FN_SHIP_ORDER_FOLLOW 
			|| dwData == FN_SHIP_ORDER_MINE
			|| dwData == FN_SHIP_ORDER_HOLD
			|| dwData == FN_SHIP_CONTROLLER)
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("i*"));
	else if (dwData == FN_SHIP_ENHANCE_ITEM)
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("iv*"));
	else
		pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("iv"));
	if (pArgs->IsError())
		return pArgs;

	//	Convert the first argument into a ship

	CShip *pShip = CreateShipObjFromItem(*pCC, pArgs->GetElement(0));
	if (pShip == NULL)
		{
		pArgs->Discard(pCC);
		return pCC->CreateNil();
		}

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_SHIP_ADD_ENERGY_FIELD:
			{
			DWORD dwUNID = (DWORD)pArgs->GetElement(1)->GetIntegerValue();
			int iLifetime = pArgs->GetElement(2)->GetIntegerValue();
			pArgs->Discard(pCC);

			COverlayType *pField = g_pUniverse->FindShipEnergyFieldType(dwUNID);
			if (pField == NULL)
				return pCC->CreateError(CONSTLIT("Unknown ship energy field"), NULL);

			pShip->AddOverlay(pField, 0, 0, 0, iLifetime);
			pResult = pCC->CreateTrue();
			break;
			}

		case FN_SHIP_REMOVE_DEVICE:
			{
			CItemListManipulator *pItemList = NULL;
			CItemListManipulator ItemList(pShip->GetItemList());

			//	If the argument is a list then it is an item (which means we have to find
			//	the item in the manipulator). If the argument is an integer then we expect
			//	an item list manipulator pointer.

			if (pArgs->GetElement(1)->IsInteger())
				pItemList = (CItemListManipulator *)pArgs->GetElement(1)->GetIntegerValue();
			else
				{
				CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
				if (!ItemList.SetCursorAtItem(Item))
					{
					pArgs->Discard(pCC);
					if (pCtx->GetAPIVersion() >= 18)
						return pCC->CreateError(CONSTLIT("Unable to find specified item in object."));
					else
						return pCC->CreateNil();
					}

				pItemList = &ItemList;
				}

			pArgs->Discard(pCC);

			//	If no list, then we're done

			if (pItemList == NULL)
				return pCC->CreateNil();

			//	Make sure the item is in the proper state

			if (pItemList->GetItemAtCursor().IsInstalled() != (dwData == FN_SHIP_REMOVE_DEVICE))
				return pCC->CreateNil();

			//	Otherwise, install or remove

			if (pShip->RemoveItemAsDevice(*pItemList) != NOERROR)
				return pCC->CreateNil();

			//	Done

			pResult = CreateListFromItem(*pCC, pItemList->GetItemAtCursor());
			break;
			}

		case FN_SHIP_FUEL_NEEDED:
		case FN_SHIP_REFUEL_FROM_ITEM:
			{
			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
			pArgs->Discard(pCC);

			//	Do it

			if (dwData == FN_SHIP_FUEL_NEEDED)
				{
				//	Figure out how much fuel each item contains

				Metric rFuelPerItem = Max(1.0, strToDouble(Item.GetType()->GetData(), 0.0, NULL));

				Metric rFuelNeeded = Max(0.0, pShip->GetMaxFuel() - pShip->GetFuelLeft());
				Metric rUnits = (rFuelNeeded / rFuelPerItem);
				int iWhole = (int)(rUnits + 0.5);

				pResult = pCC->CreateInteger(iWhole);
				}
			else
				{
				pShip->Refuel(Item);
				pResult = pCC->CreateTrue();
				}

			break;
			}

		case FN_SHIP_ITEM_DEVICE_NAME:
			{
			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
			pArgs->Discard(pCC);
			pResult = pCC->CreateInteger(pShip->GetItemDeviceName(Item));
			break;
			}

		case FN_SHIP_IS_FUEL_COMPATIBLE:
			{
			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
			pArgs->Discard(pCC);
			pResult = pCC->CreateBool(pShip->IsFuelCompatible(Item));
			break;
			}

		case FN_SHIP_ORDER_ATTACK:
			{
			CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			pArgs->Discard(pCC);

			if (pObj && !pObj->IsDestroyed())
				{
				pShip->GetController()->AddOrder(IShipController::orderDestroyTarget, pObj, IShipController::SData());
				pResult = pCC->CreateTrue();
				}
			else
				pResult = pCC->CreateNil();
			break;
			}

		case FN_SHIP_ORDER_DOCK:
			{
			CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			pArgs->Discard(pCC);

			if (pObj && !pObj->IsDestroyed())
				{
				pShip->GetController()->AddOrder(IShipController::orderDock, pObj, IShipController::SData());
				pResult = pCC->CreateTrue();
				}
			else
				pResult = pCC->CreateNil();
			break;
			}

		case FN_SHIP_ORDER_ESCORT:
			{
			CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			DWORD dwFormation = 0;
			if (pArgs->GetCount() > 2)
				dwFormation = pArgs->GetElement(2)->GetIntegerValue();
			pArgs->Discard(pCC);

			if (pObj && !pObj->IsDestroyed())
				{
				pShip->GetController()->AddOrder(IShipController::orderEscort, pObj, IShipController::SData(dwFormation));
				pResult = pCC->CreateTrue();
				}
			else
				pResult = pCC->CreateNil();
			break;
			}

		case FN_SHIP_ORDER_FOLLOW:
			{
			CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			pArgs->Discard(pCC);

			if (pObj && !pObj->IsDestroyed())
				{
				pShip->GetController()->AddOrder(IShipController::orderFollow, pObj, IShipController::SData());
				pResult = pCC->CreateTrue();
				}
			else
				pResult = pCC->CreateNil();
			break;
			}

		case FN_SHIP_ORDER_GATE:
			{
			CSpaceObject *pGate = NULL;
			if (pArgs->GetCount() == 2)
				pGate = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			pArgs->Discard(pCC);

			if (pGate == NULL || !pGate->IsDestroyed())
				{
				//	Note: OK if pGate == NULL.
				pShip->GetController()->AddOrder(IShipController::orderGate, pGate, IShipController::SData());
				pResult = pCC->CreateTrue();
				}
			else
				pResult = pCC->CreateNil();

			break;
			}

		case FN_SHIP_ORDER_GUARD:
			{
			CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			pArgs->Discard(pCC);

			if (pObj && !pObj->IsDestroyed())
				{
				pShip->GetController()->AddOrder(IShipController::orderGuard, pObj, IShipController::SData());
				pResult = pCC->CreateTrue();
				}
			else
				pResult = pCC->CreateNil();
			break;
			}

		case FN_SHIP_ORDER_LOOT:
			{
			CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			pArgs->Discard(pCC);

			if (pObj && !pObj->IsDestroyed())
				{
				pShip->GetController()->AddOrder(IShipController::orderLoot, pObj, IShipController::SData());
				pResult = pCC->CreateTrue();
				}
			else
				pResult = pCC->CreateNil();
			break;
			}

		case FN_SHIP_ORDER_MINE:
			{
			CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			pArgs->Discard(pCC);

			if (pObj && !pObj->IsDestroyed())
				{
				pShip->GetController()->AddOrder(IShipController::orderMine, pObj, IShipController::SData());
				pResult = pCC->CreateTrue();
				}
			else
				pResult = pCC->CreateNil();
			break;
			}

		case FN_SHIP_ORDER_PATROL:
			{
			if (pArgs->GetCount() == 3)
				{
				CSpaceObject *pCenter = CreateObjFromItem(*pCC, pArgs->GetElement(1));
				int iRadius = pArgs->GetElement(2)->GetIntegerValue();
				pArgs->Discard(pCC);

				if (pCenter && !pCenter->IsDestroyed())
					{
					pShip->GetController()->AddOrder(IShipController::orderPatrol, pCenter, IShipController::SData(iRadius));
					pResult = pCC->CreateTrue();
					}
				else
					pResult = pCC->CreateNil();
				}
			else
				pResult = pCC->CreateError(CONSTLIT("Not enough parameters for shpOrderPatrol"), NULL);

			break;
			}

		case FN_SHIP_ORDER_WAIT:
			{
			int iWaitTime = pArgs->GetElement(1)->GetIntegerValue();
			pArgs->Discard(pCC);
			pShip->GetController()->AddOrder(IShipController::orderWait, NULL, IShipController::SData(iWaitTime));
			pResult = pCC->CreateTrue();
			break;
			}

		case FN_SHIP_ORDER_HOLD:
			{
			int iWaitTime = 0;
			if (pArgs->GetCount() >= 2)
				iWaitTime = pArgs->GetElement(1)->GetIntegerValue();
			pArgs->Discard(pCC);
			pShip->GetController()->AddOrder(IShipController::orderHold, NULL, IShipController::SData(iWaitTime));
			pResult = pCC->CreateTrue();
			break;
			}

		case FN_SHIP_BLINDNESS:
			{
			int iBlindTime = pArgs->GetElement(1)->GetIntegerValue();
			pArgs->Discard(pCC);

			pShip->SetAbility(ablShortRangeScanner, ablDamage, iBlindTime, 0);
			pResult = pCC->CreateTrue();
			break;
			}

		case FN_SHIP_DAMAGE_ITEM:
			{
			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));

			CString sError;
			if (!pShip->SetItemProperty(Item, CONSTLIT("damaged"), NULL, 1, NULL, &sError))
				{
				if (sError.IsBlank())
					pResult = pCC->CreateNil();
				else
					pResult = pCC->CreateError(sError);
				}
			else
				pResult = pCC->CreateTrue();

			pArgs->Discard(pCC);
			break;
			}

		case FN_SHIP_CONTROLLER:
			{
			CString sController;
			if (pArgs->GetCount() > 1 && !pArgs->GetElement(1)->IsNil())
				sController = pArgs->GetElement(1)->GetStringValue();
			IShipController *pController = g_pUniverse->CreateShipController(sController);
			pArgs->Discard(pCC);

			if (pController)
				{
				pShip->SetController(pController);
				pResult = pCC->CreateTrue();
				}
			else
				pResult = pCC->CreateNil();
			break;
			}

		case FN_SHIP_ENHANCE_ITEM:
			{
			CItemListManipulator *pItemList = NULL;
			CItemListManipulator ItemList(pShip->GetItemList());

			//	If the argument is a list then it is an item (which means we have to find
			//	the item in the manipulator). If the argument is an integer then we expect
			//	an item list manipulator pointer.

			if (pArgs->GetElement(1)->IsInteger())
				pItemList = (CItemListManipulator *)pArgs->GetElement(1)->GetIntegerValue();
			else
				{
				CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
				if (!ItemList.SetCursorAtItem(Item))
					{
					pArgs->Discard(pCC);
					if (pCtx->GetAPIVersion() >= 18)
						return pCC->CreateError(CONSTLIT("Unable to find specified item in object."));
					else
						return pCC->CreateNil();
					}

				pItemList = &ItemList;
				}

			//	Get the enhancement

			CItemEnhancement Mods;
			CString sError;
			if (pArgs->GetCount() > 2)
				{
				if (Mods.InitFromDesc(pArgs->GetElement(2), &sError) != NOERROR)
					{
					pArgs->Discard(pCC);
					pResult = pCC->CreateError(sError);
					break;
					}
				}

			if (pItemList == NULL)
				{
				pArgs->Discard(pCC);
				pResult = pCC->CreateNil();
				}
			else if (pItemList->GetItemAtCursor().GetType()->IsArmor())
				{
				EnhanceItemStatus iResult = pShip->EnhanceItem(*pItemList, Mods);
				pArgs->Discard(pCC);
				pResult = pCC->CreateInteger(iResult);
				}
			else if (pItemList->GetItemAtCursor().GetType()->IsDevice())
				{
				if (Mods.IsEmpty())
					Mods = CItemEnhancement(etBinaryEnhancement);

				EnhanceItemStatus iResult = pShip->EnhanceItem(*pItemList, Mods);
				pArgs->Discard(pCC);
				pResult = pCC->CreateInteger(iResult);
				}
			else
				{
				pShip->EnhanceItem(*pItemList, etBinaryEnhancement);
				pArgs->Discard(pCC);
				pResult = pCC->CreateTrue();
				}
			break;
			}

		case FN_SHIP_ITEM_CHARGES:
			{
			CItemListManipulator *pItemList = NULL;
			CItemListManipulator ItemList(pShip->GetItemList());

			//	If the argument is a list then it is an item (which means we have to find
			//	the item in the manipulator). If the argument is an integer then we expect
			//	an item list manipulator pointer.

			if (pArgs->GetElement(1)->IsInteger())
				pItemList = (CItemListManipulator *)pArgs->GetElement(1)->GetIntegerValue();
			else
				{
				CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));
				if (!ItemList.SetCursorAtItem(Item))
					{
					pArgs->Discard(pCC);
					if (pCtx->GetAPIVersion() >= 18)
						return pCC->CreateError(CONSTLIT("Unable to find specified item in object."));
					else
						return pCC->CreateNil();
					}

				pItemList = &ItemList;
				}

			pArgs->Discard(pCC);

			//	If no list, then we're done

			if (pItemList == NULL)
				{
				pResult = pCC->CreateNil();
				break;
				}

			//	Otherwise, return the charges for this item

			pResult = pCC->CreateInteger(pItemList->GetItemAtCursor().GetCharges());
			break;
			}

		case FN_SHIP_RECHARGE_ITEM:
			{
			//	Get the item

			CItem Item(CreateItemFromList(*pCC, pArgs->GetElement(1)));

			//	Set the charges

			CString sError;
			if (!pShip->SetItemProperty(Item, CONSTLIT("incCharges"), pArgs->GetElement(2), 1, NULL, &sError))
				{
				if (sError.IsBlank())
					pResult = pCC->CreateNil();
				else
					pResult = pCC->CreateError(sError);

				pArgs->Discard(pCC);
				break;
				}

			pArgs->Discard(pCC);
			pResult = pCC->CreateTrue();
			break;
			}

		default:
			ASSERT(FALSE);
		}

	return pResult;
	}

ICCItem *fnSovereignGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnSovereignGet

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	//	Get the sovereign

	DWORD dwSovereignID = pArgs->GetElement(0)->GetIntegerValue();
	CSovereign *pSovereign = g_pUniverse->FindSovereign(dwSovereignID);
	if (pSovereign == NULL)
		return pCC->CreateNil();

	//	Handle the command

	switch (dwData)
		{
		case FN_SOVEREIGN_NAME:
			{
			DWORD dwFlags;
			if (pArgs->GetCount() > 1)
				dwFlags = pCtx->AsNameFlags(pArgs->GetElement(1));
			else
				dwFlags = 0;

			return pCC->CreateString(pSovereign->GetNounPhrase(dwFlags));
			}

		default:
			ASSERT(false);
			return NULL;
		}
	}

ICCItem *fnSovereignSet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnSovereignSet
//
//	(sovSetDisposition sovereignID targetSovereignID disposition)

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Get the sovereign

	DWORD dwSovereignID = pArgs->GetElement(0)->GetIntegerValue();
	CSovereign *pSovereign = g_pUniverse->FindSovereign(dwSovereignID);
	if (pSovereign == NULL)
		return pCC->CreateNil();

	//	Handle the command

	switch (dwData)
		{
		case FN_SOVEREIGN_DISPOSITION:
			{
			//	Get the target sovereign and disposition

			DWORD dwTargetID = pArgs->GetElement(1)->GetIntegerValue();
			CSovereign *pTarget = g_pUniverse->FindSovereign(dwTargetID);
			if (pTarget == NULL)
				return pCC->CreateError(CONSTLIT("Invalid sovereign"), pArgs->GetElement(1));

			CSovereign::Disposition iDisp;
			if (pArgs->GetElement(2)->IsInteger())
				iDisp = (CSovereign::Disposition)pArgs->GetElement(2)->GetIntegerValue();
			else
				{
				CString sValue = pArgs->GetElement(2)->GetStringValue();
				if (strEquals(sValue, DISP_NEUTRAL))
					iDisp = CSovereign::dispNeutral;
				else if (strEquals(sValue, DISP_ENEMY))
					iDisp = CSovereign::dispEnemy;
				else if (strEquals(sValue, DISP_FRIEND))
					iDisp = CSovereign::dispFriend;
				else
					return pCC->CreateError(CONSTLIT("Invalid disposition"), pArgs->GetElement(2));
				}

			//	Do it

			pSovereign->SetDispositionTowards(pTarget, iDisp);
			return pCC->CreateTrue();
			}

		case FN_SOVEREIGN_GET_DISPOSITION:
			{
			//	Get the target sovereign and disposition

			DWORD dwTargetID = pArgs->GetElement(1)->GetIntegerValue();
			CSovereign *pTarget = g_pUniverse->FindSovereign(dwTargetID);
			if (pTarget == NULL)
				return pCC->CreateError(CONSTLIT("Invalid sovereign"), pArgs->GetElement(1));

			return CreateDisposition(*pCC, pSovereign->GetDispositionTowards(pTarget));
			}

		case FN_SOVEREIGN_MESSAGE:
		case FN_SOVEREIGN_MESSAGE_FROM_OBJ:
			{
			int iArg = 1;
			CSpaceObject *pSenderObj;

			//	See if we have an obj sender

			if (dwData == FN_SOVEREIGN_MESSAGE_FROM_OBJ)
				pSenderObj = CreateObjFromItem(*pCC, pArgs->GetElement(iArg++));
			else
				pSenderObj = NULL;

			//	Get the text.

			CString sText = pArgs->GetElement(iArg)->GetStringValue();
			if (sText.IsBlank())
				return pCC->CreateNil();

			//	Send the message

			pSovereign->MessageFromObj(pSenderObj, sText);
			return pCC->CreateTrue();
			}

		default:
			ASSERT(false);
		}

	return pCC->CreateNil();
	}

ICCItem *fnStationGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnStationGet
//
//	Gets data about a station

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Convert the first argument into a station

	CStation *pStation = CreateStationObjFromItem(*pCC, pArgs->GetElement(0));
	if (pStation == NULL)
		return pCC->CreateNil();

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_STATION_MAX_STRUCTURAL_HP:
			return pCC->CreateInteger(pStation->GetHull().GetMaxStructuralHP());

		default:
			ASSERT(FALSE);
		}

	return pCC->CreateNil();
	}

ICCItem *fnStationGetOld (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnStationGetOld
//
//	Gets data about a station

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	ICCItem *pArgs;
	ICCItem *pResult;

	//	Evaluate the arguments and validate them

	pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("i"));
	if (pArgs->IsError())
		return pArgs;

	//	Convert the first argument into a station

	CStation *pStation = CreateStationObjFromItem(*pCC, pArgs->GetElement(0));
	if (pStation == NULL)
		{
		pArgs->Discard(pCC);
		return pCC->CreateNil();
		}

	pArgs->Discard(pCC);

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_STATION_SET_INACTIVE:
			{
			pStation->SetInactive();
			pResult = pCC->CreateTrue();
			break;
			}

		case FN_STATION_GET_DOCKED_SHIPS:
			{
			pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;

			//	Add each docked ship

			for (int i = 0; i < pStation->GetDockingPortCount(); i++)
				{
				CSpaceObject *pShip = pStation->GetShipAtDockingPort(i);
				if (pShip)
					{
					ICCItem *pInt = pCC->CreateInteger((int)pShip);
					pList->Append(*pCC, pInt);
					pInt->Discard(pCC);
					}
				}

			break;
			}

		case FN_STATION_GET_SUBORDINATES:
			{
			pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;

			//	Add each subordinate

			for (int i = 0; i < pStation->GetSubordinateCount(); i++)
				{
				CSpaceObject *pShip = pStation->GetSubordinate(i);
				if (pShip)
					{
					ICCItem *pInt = pCC->CreateInteger((int)pShip);
					pList->Append(*pCC, pInt);
					pInt->Discard(pCC);
					}
				}

			break;
			}

		case FN_STATION_GET_TYPE:
			{
			pResult = pCC->CreateInteger(pStation->GetType()->GetUNID());
			break;
			}

		case FN_STATION_RECON:
			{
			pResult = pCC->CreateBool(pStation->IsReconned());
			break;
			}

		case FN_STATION_CLEAR_RECON:
			{
			pStation->ClearReconned();
			pResult = pCC->CreateTrue();
			break;
			}

		case FN_STATION_CLEAR_FIRE_RECON:
			{
			pStation->ClearFireReconEvent();
			pResult = pCC->CreateTrue();
			break;
			}

		case FN_STATION_SET_FIRE_RECON:
			{
			pStation->SetFireReconEvent();
			pResult = pCC->CreateTrue();
			break;
			}

		case FN_STATION_IMAGE_VARIANT:
			{
			pResult = pCC->CreateInteger(pStation->GetImageVariant());
			break;
			}

		case FN_STATION_STRUCTURAL_HP:
			{
			pResult = pCC->CreateInteger(pStation->GetHull().GetStructuralHP());
			break;
			}

		default:
			ASSERT(FALSE);
		}

	return pResult;
	}

ICCItem *fnStationSet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnStationSet
//
//	Sets data about a station

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Convert the first argument into a station

	CStation *pStation = CreateStationObjFromItem(*pCC, pArgs->GetElement(0));
	if (pStation == NULL)
		return pCC->CreateNil();

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_STATION_SET_ACTIVE:
			if (pArgs->GetCount() < 2 || !pArgs->GetElement(1)->IsNil())
				pStation->SetActive();
			else
				pStation->SetInactive();
			return pCC->CreateTrue();

		case FN_STATION_SHOW_MAP_LABEL:
			pStation->SetShowMapLabel(!pArgs->GetElement(1)->IsNil());
			return pCC->CreateTrue();

		default:
			ASSERT(FALSE);
		}

	return pCC->CreateNil();
	}

ICCItem *fnStationSetOld (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnStationSetOld
//
//	Sets data about a station

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	ICCItem *pArgs;
	ICCItem *pResult;

	//	Evaluate the arguments and validate them

	pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("ii"));
	if (pArgs->IsError())
		return pArgs;

	//	Convert the first argument into a station

	CStation *pStation = CreateStationObjFromItem(*pCC, pArgs->GetElement(0));
	if (pStation == NULL)
		{
		pArgs->Discard(pCC);
		return pCC->CreateNil();
		}

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_STATION_IMAGE_VARIANT:
			{
			int iVariant = pArgs->GetElement(1)->GetIntegerValue();
			if (iVariant >= 0 && iVariant < pStation->GetImageVariantCount())
				{
				pStation->SetImageVariant(pArgs->GetElement(1)->GetIntegerValue());
				pResult = pCC->CreateTrue();
				}
			else
				pResult = pCC->CreateError(strPatternSubst(CONSTLIT("Invalid variant: %d"), iVariant));

			pArgs->Discard(pCC);
			break;
			}

		case FN_STATION_STRUCTURAL_HP:
			{
			pStation->SetStructuralHitPoints(pArgs->GetElement(1)->GetIntegerValue());
			pArgs->Discard(pCC);
			pResult = pCC->CreateTrue();
			break;
			}

		default:
			ASSERT(FALSE);
		}

	return pResult;
	}

ICCItem *fnStationType (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnStationType
//
//	Sets data about a station type

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	ICCItem *pResult;

	//	Convert the first argument into a station type

	DWORD dwUNID = pArgs->GetElement(0)->GetIntegerValue();
	CStationType *pType = g_pUniverse->FindStationType(dwUNID);
	if (pType == NULL)
		return pCC->CreateError(CONSTLIT("station type expected"), pArgs->GetElement(0));

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_STATION_ENCOUNTERED:
			{
			pResult = pCC->CreateBool(!pType->CanBeEncountered());
			break;
			}

		case FN_STATION_GET_GLOBAL_DATA:
			return pType->GetGlobalData(pArgs->GetElement(1)->GetStringValue())->Reference();

		case FN_STATION_SET_GLOBAL_DATA:
			{
			CString sAttrib = pArgs->GetElement(1)->GetStringValue();
			pType->SetGlobalData(sAttrib, pArgs->GetElement(2));
			pResult = pCC->CreateTrue();
			break;
			}

		default:
			ASSERT(FALSE);
		}

	return pResult;
	}

ICCItem *fnSystemAddEncounterEvent (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnSystemAddEncounterEvent
//
//	Adds an encounter

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Arguments

	int iTime = pArgs->GetElement(0)->GetIntegerValue();
	CSpaceObject *pTarget = CreateObjFromItem(*pCC, pArgs->GetElement(1));
	if (pTarget == NULL)
		return pCC->CreateNil();

	DWORD dwEncounterID = (DWORD)pArgs->GetElement(2)->GetIntegerValue();

	//	Either from a gate or from a distance from the target

	int iDistance = 0;
	CSpaceObject *pGate = NULL;
	CVector vPos;

	if (dwData == FN_ADD_ENCOUNTER_FROM_DIST)
		iDistance =	pArgs->GetElement(3)->GetIntegerValue();
	else
		{
		if (::GetPosOrObject(pEvalCtx, pArgs->GetElement(3), &vPos, &pGate) != NOERROR)
			return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(3));
		}
	
	//	Create the event

	CSystem *pSystem = g_pUniverse->GetCurrentSystem();
	if (pSystem == NULL)
		return StdErrorNoSystem(*pCC);

	CTimedEncounterEvent *pEvent = new CTimedEncounterEvent(
			pSystem->GetTick() + iTime,
			pTarget,
			dwEncounterID,
			pGate,
			vPos,
			iDistance * LIGHT_SECOND);

	pSystem->AddTimedEvent(pEvent);

	return pCC->CreateTrue();
	}

ICCItem *fnSystemAddStationTimerEvent (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnSystemAddStationTimerEvent
//
//	Adds an station timer

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	switch (dwData)
		{
		case FN_ADD_TIMER_NORMAL:
		case FN_ADD_TIMER_RECURRING:
			{
			int iTime = pArgs->GetElement(0)->GetIntegerValue();
			CSpaceObject *pTarget = (CSpaceObject *)CreateObjFromItem(*pCC, pArgs->GetElement(1));
			if (pTarget == NULL)
				return pCC->CreateNil();
			CString sEvent = pArgs->GetElement(2)->GetStringValue();

			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			CSystemEvent *pEvent;
			if (dwData == FN_ADD_TIMER_NORMAL)
				pEvent = new CTimedCustomEvent(
						pSystem->GetTick() + iTime,
						pTarget,
						sEvent);
			else
				pEvent = new CTimedRecurringEvent(
						iTime,
						pTarget,
						sEvent);

			pSystem->AddTimedEvent(pEvent);

			return pCC->CreateTrue();
			}

		case FN_ADD_TYPE_RANGE_EVENT:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			CDesignType *pType = g_pUniverse->FindDesignType(pArgs->GetElement(0)->GetIntegerValue());
			if (pType == NULL)
				return pCC->CreateNil();
			CString sEvent = pArgs->GetElement(1)->GetStringValue();

			//	Options

			ICCItem *pOptions = pArgs->GetElement(2);

			CVector vCenter;
			CSpaceObject *pCenter;
			::GetPosOrObject(pEvalCtx, pOptions->GetElement(CONSTLIT("center")), &vCenter, &pCenter);
			
			Metric rRadius = pOptions->GetDoubleAt(CONSTLIT("radius")) * LIGHT_SECOND;
			CString sCriteria = pOptions->GetStringAt(CONSTLIT("criteria"));

			//	If we have a center object, use that. Otherwise, we use a vector

			CSystemEvent *pEvent;
			if (pCenter)
				pEvent = new CRangeTypeEvent(pType, sEvent, pCenter, rRadius, sCriteria);
			else
				pEvent = new CRangeTypeEvent(pType, sEvent, vCenter, rRadius, sCriteria);

			//	Add to the system

			pSystem->AddTimedEvent(pEvent);
			return pCC->CreateTrue();
			}

		case FN_ADD_TYPE_TIMER_NORMAL:
		case FN_ADD_TYPE_TIMER_RECURRING:
			{
			int iTime = pArgs->GetElement(0)->GetIntegerValue();
			if (iTime < 0 || (iTime == 0 && dwData == FN_ADD_TYPE_TIMER_RECURRING))
				return pCC->CreateError(CONSTLIT("Invalid recurring time"), pArgs->GetElement(0));

			CDesignType *pTarget = g_pUniverse->FindDesignType(pArgs->GetElement(1)->GetIntegerValue());
			if (pTarget == NULL)
				return pCC->CreateNil();
			CString sEvent = pArgs->GetElement(2)->GetStringValue();

			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			CSystemEvent *pEvent;
			if (dwData == FN_ADD_TYPE_TIMER_NORMAL)
				pEvent = new CTimedTypeEvent(
						pSystem->GetTick() + iTime,
						0,
						pTarget,
						sEvent);
			else
				pEvent = new CTimedTypeEvent(
						pSystem->GetTick() + mathRandom(0, iTime),
						iTime,
						pTarget,
						sEvent);

			pSystem->AddTimedEvent(pEvent);

			return pCC->CreateTrue();
			}

		case FN_CANCEL_TIMER:
			{
			CSpaceObject *pTarget = (CSpaceObject *)CreateObjFromItem(*pCC, pArgs->GetElement(0));
			if (pTarget == NULL)
				return pCC->CreateNil();
			CString sEvent = pArgs->GetElement(1)->GetStringValue();
			
			pTarget->GetSystem()->CancelTimedEvent(pTarget, sEvent, pCtx->InEvent(eventDoEvent));

			return pCC->CreateTrue();
			}

		case FN_CANCEL_TYPE_TIMER:
			{
			CDesignType *pTarget = g_pUniverse->FindDesignType(pArgs->GetElement(0)->GetIntegerValue());
			if (pTarget == NULL)
				return pCC->CreateNil();
			CString sEvent = pArgs->GetElement(1)->GetStringValue();
			
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			pSystem->CancelTimedEvent(pTarget, sEvent, pCtx->InEvent(eventDoEvent));

			return pCC->CreateTrue();
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

ICCItem *fnSystemCreate (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnSystemCreate
//
//	Creates an object
//
//	(sysCreateWeaponFire weaponID objSource pos dir speed objTarget) -> obj
//	(sysCreateShipwreck unid pos sovereignID) -> shipwreck

	{
	ALERROR error;
	int i;

	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	switch (dwData)
		{
		case FN_SYS_CREATE_ENCOUNTER:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			CDesignType *pType = g_pUniverse->FindDesignType(pArgs->GetElement(0)->GetIntegerValue());
			ICCItem *pOptions = (pArgs->GetCount() > 1 ? pArgs->GetElement(1) : pCC->CreateNil());

			CSpaceObject *pTarget = CreateObjFromItem(*pCC, pOptions->GetElement(CONSTLIT("target")));
			if (pTarget == NULL)
				pTarget = pSystem->GetPlayerShip();

			Metric rDist = pOptions->GetDoubleAt(CONSTLIT("distance"), 100.0) * LIGHT_SECOND;

			CSpaceObject *pGate = CreateObjFromItem(*pCC, pOptions->GetElement(CONSTLIT("gate")));

			//	If we have a station type, then create its random encounter

			if (pType->GetType() == designStationType)
				{
				CStationType *pEncounter = CStationType::AsType(pType);
				if (pEncounter == NULL)
					return pCC->CreateError(CONSTLIT("Unknown station type ID"), pArgs->GetElement(0));

				IShipGenerator *pTable = pEncounter->GetEncountersTable();
				if (pTable == NULL)
					return pCC->CreateNil();

				pSystem->CreateRandomEncounter(pTable, NULL, pEncounter->GetSovereign(), pTarget);
				}

			//	If this is a ship table, create an encounter

			else if (pType->GetType() == designShipTable)
				{
				CShipTable *pEncounter = CShipTable::AsType(pType);

				SShipCreateCtx Ctx;
				Ctx.pSystem = pSystem;
				Ctx.pTarget = pTarget;
				Ctx.dwFlags = SShipCreateCtx::ATTACK_NEAREST_ENEMY;

				//	Figure out where the encounter will come from

				if (rDist > 0.0)
					{
					if (pTarget)
						Ctx.vPos = pTarget->GetPos() + ::PolarToVector(mathRandom(0, 359), rDist);
					Ctx.PosSpread = DiceRange(3, 1, 2);
					}
				else if (pGate && pGate->IsActiveStargate())
					Ctx.pGate = pGate;
				else if (pGate)
					{
					Ctx.vPos = pGate->GetPos();
					Ctx.PosSpread = DiceRange(2, 1, 2);
					}
				else if (pTarget)
					Ctx.pGate = pTarget->GetNearestStargate(true);

				//	Create ships

				pEncounter->CreateShips(Ctx);
				}

			return pCC->CreateTrue();
			}

		case FN_SYS_CREATE_ENVIRONMENT:
			{
			//	Get basic parameters

			CSpaceEnvironmentType *pEnvType = g_pUniverse->FindSpaceEnvironment(pArgs->GetElement(0)->GetIntegerValue());
			if (pEnvType == NULL)
				return pCC->CreateError(CONSTLIT("Unknown environment type"), pArgs->GetElement(0));

			CString sShape = pArgs->GetElement(1)->GetStringValue();
			ICCItem *pOptions = pArgs->GetElement(2);

			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			CEnvironmentGrid *pEnvironment = pSystem->GetEnvironmentGrid();

			//	Make sure we mark the environment so that it properly loads its tileset

			pEnvType->MarkImages();

			//	Compose create context

			CEnvironmentGrid::SCreateCtx CreateCtx;
			CreateCtx.pSystem = pSystem;
			CreateCtx.pEnv = pEnvType;

			//	Processing depends on shape

			if (strEquals(sShape, SHAPE_ARC))
				{
				COrbit OrbitDesc;
				if (!CreateOrbitFromList(*pCC, pOptions->GetElement(FIELD_ORBIT), &OrbitDesc))
					return pCC->CreateError(CONSTLIT("Invalid orbit object"), pOptions->GetElement(FIELD_ORBIT));

				CreateCtx.pOrbitDesc = &OrbitDesc;
				CreateCtx.rWidth = pOptions->GetIntegerAt(FIELD_WIDTH) * LIGHT_SECOND;
				CreateCtx.iWidthVariation = pOptions->GetIntegerAt(FIELD_WIDTH_VARIATION, 100);
				CreateCtx.iErode = pOptions->GetIntegerAt(FIELD_ERODE);

				pEnvironment->CreateCircularNebula(CreateCtx, NULL);
				}
			else if (strEquals(sShape, SHAPE_ORBITAL))
				{
				COrbit OrbitDesc;
				if (!CreateOrbitFromList(*pCC, pOptions->GetElement(FIELD_ORBIT), &OrbitDesc))
					return pCC->CreateError(CONSTLIT("Invalid orbit object"), pOptions->GetElement(FIELD_ORBIT));

				CreateCtx.pOrbitDesc = &OrbitDesc;
				CreateCtx.rWidth = pOptions->GetIntegerAt(FIELD_WIDTH) * LIGHT_SECOND;
				CreateCtx.iWidthVariation = pOptions->GetIntegerAt(FIELD_WIDTH_VARIATION, 100);
				CreateCtx.iSpan = pOptions->GetIntegerAt(FIELD_LENGTH);
				CreateCtx.iErode = pOptions->GetIntegerAt(FIELD_ERODE);

				pEnvironment->CreateArcNebula(CreateCtx, NULL);
				}
			else if (strEquals(sShape, SHAPE_SQUARE))
				{
				ICCItem *pCenter = pOptions->GetElement(FIELD_CENTER);
				CVector vPos;
				if (GetPosOrObject(pEvalCtx, pCenter, &vPos) != NOERROR)
					return pCC->CreateError(CONSTLIT("Invalid pos"), pCenter);

				COrbit OrbitDesc(vPos, 0.0);

				CreateCtx.pOrbitDesc = &OrbitDesc;
				CreateCtx.rWidth = pOptions->GetIntegerAt(FIELD_WIDTH) * LIGHT_SECOND;
				CreateCtx.rHeight = pOptions->GetIntegerAt(FIELD_HEIGHT) * LIGHT_SECOND;

				pEnvironment->CreateSquareNebula(CreateCtx, NULL);
				}
			else
				return pCC->CreateError(CONSTLIT("Unknown shape"), pArgs->GetElement(2));

			return pCC->CreateTrue();
			}

		case FN_SYS_CREATE_FLOTSAM:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			//	Get parameters

			CItem Item = GetItemFromArg(*pCC, pArgs->GetElement(0));
			if (Item.GetType() == NULL)
				return pCC->CreateError(CONSTLIT("Unknown item type"), pArgs->GetElement(0));

			CVector vPos;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vPos) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));

			CSovereign *pSovereign = g_pUniverse->FindSovereign(pArgs->GetElement(2)->GetIntegerValue());
			if (pSovereign == NULL)
				return pCC->CreateError(CONSTLIT("Unknown sovereign type"), pArgs->GetElement(2));

			//	Create

			CStation *pFlotsam;
			if (pSystem->CreateFlotsam(Item, vPos, CVector(), pSovereign, &pFlotsam) != NOERROR)
				return pCC->CreateError(CONSTLIT("Error creating flotsam"), pArgs->GetElement(0));

			//	Done

			return pCC->CreateInteger((int)pFlotsam);
			}

		case FN_SYS_CREATE_HIT_EFFECT:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			//	Get the weapon descriptor

			CItemType *pItemType = g_pUniverse->FindItemType(pArgs->GetElement(0)->GetIntegerValue());
			if (pItemType == NULL)
				return pCC->CreateError(CONSTLIT("Unknown weapon UNID"), pArgs->GetElement(0));

            CString sError;
			CWeaponFireDesc *pDesc = pItemType->GetWeaponFireDesc(CItemCtx(), &sError);
			if (pDesc == NULL)
				return pCC->CreateError(sError, pArgs->GetElement(0));

			//	Get Parameters to initialize context

			CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			CVector vHitPos = CreateVectorFromList(*pCC, pArgs->GetElement(2));
			int iDir = pArgs->GetElement(3)->GetIntegerValue();

			SDamageCtx DamageCtx(pObj,
					pDesc,
					NULL,
					CDamageSource(),
					NULL,
					iDir,
					vHitPos);

			DamageCtx.iDamage = pArgs->GetElement(4)->GetIntegerValue();

			//	Create the effect

			pDesc->CreateHitEffect(pSystem, DamageCtx);
			return pCC->CreateTrue();
			}

		case FN_SYS_CREATE_LOOKUP:
			{
			SSystemCreateCtx *pSysCreateCtx = pCtx->GetSystemCreateCtx();

			//	We can't always control if we're called inside of system create or not,
			//	so if we're NOT, then we fail without an error.

			if (pSysCreateCtx == NULL)
				return pCC->CreateNil();

			CString sTableName = pArgs->GetElement(0)->GetStringValue();

			//	Get the orbit

			COrbit OrbitDesc;
			if (!CreateOrbitFromList(*pCC, pArgs->GetElement(1), &OrbitDesc))
				return pCC->CreateError(CONSTLIT("Invalid orbit object"), pArgs->GetElement(0));

			//	Create

			if (error = pSysCreateCtx->pSystem->CreateLookup(pSysCreateCtx, sTableName, OrbitDesc, NULL))
				{
				//	If we can't find the table, we fail without an error (because
				//	we might be creating an optional object).

				if (error == ERR_NOTFOUND)
					return pCC->CreateNil();
				else
					return pCC->CreateError(pSysCreateCtx->sError);
				}

			//	Done

			return pCC->CreateTrue();
			}

		case FN_SYS_CREATE_SHIPWRECK:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			//	Get parameters

			CShipClass *pClass = g_pUniverse->FindShipClass(pArgs->GetElement(0)->GetIntegerValue());
			if (pClass == NULL)
				return pCC->CreateError(CONSTLIT("Unknown ship class ID"), pArgs->GetElement(0));

			CVector vPos;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vPos) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));

			CSovereign *pSovereign = g_pUniverse->FindSovereign(pArgs->GetElement(2)->GetIntegerValue());
			if (pSovereign == NULL)
				return pCC->CreateError(CONSTLIT("Unknown sovereign ID"), pArgs->GetElement(2));

			//	Create

			CStation *pWreck;
			if (pSystem->CreateShipwreck(pClass, vPos, CVector(), pSovereign, &pWreck) != NOERROR)
				return pCC->CreateError(CONSTLIT("Error creating shipwreck"), pArgs->GetElement(0));

			//	Done

			return pCC->CreateInteger((int)pWreck);
			}

		case FN_SYS_CREATE_TERRITORY:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			//	Get the orbit

			COrbit OrbitDesc;
			if (!CreateOrbitFromList(*pCC, pArgs->GetElement(0), &OrbitDesc))
				return pCC->CreateError(CONSTLIT("Invalid orbit object"), pArgs->GetElement(0));

			//	Get the radii

			int iMinRadius = pArgs->GetElement(1)->GetIntegerValue();
			int iMaxRadius = pArgs->GetElement(2)->GetIntegerValue();
			if (iMinRadius < 0 || iMaxRadius < 0 || (iMaxRadius != 0 && iMaxRadius < iMinRadius))
				return pCC->CreateError(CONSTLIT("Invalid radius"));

			Metric rMinRadius = iMinRadius * LIGHT_SECOND;
			Metric rMaxRadius = iMaxRadius * LIGHT_SECOND;

			//	Get the attributes

			CString sAttribs;
			ICCItem *pAttribs = pArgs->GetElement(3);
			if (pAttribs->IsList() && pAttribs->GetCount() > 0)
				{
				sAttribs = pAttribs->GetElement(0)->GetStringValue();
				for (i = 1; i < pAttribs->GetCount(); i++)
					sAttribs.Append(strPatternSubst(CONSTLIT(", %s"), pAttribs->GetElement(i)->GetStringValue()));
				}
			else
				sAttribs = pAttribs->GetStringValue();

			//	Criteria

			CString sCriteria;
			if (pArgs->GetCount() > 4
					&& !pArgs->GetElement(4)->IsNil())
				sCriteria = pArgs->GetElement(4)->GetStringValue();

			//	Create the territory

			CTerritoryDef *pTerritory = new CTerritoryDef;
			pTerritory->AddRegion(OrbitDesc, rMinRadius, rMaxRadius);
			pTerritory->AddAttributes(sAttribs);
			if (!sCriteria.IsBlank())
				pTerritory->SetCriteria(sCriteria);

			//	Add it 

			if (pSystem->AddTerritory(pTerritory) != NOERROR)
				{
				delete pTerritory;
				return pCC->CreateNil();
				}

			return pCC->CreateTrue();
			}

		case FN_SYS_CREATE_WEAPON_FIRE:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			//	Get parameters

			CWeaponFireDesc *pDesc = GetWeaponFireDescArg(*pCC, pArgs->GetElement(0));
			if (pDesc == NULL)
				return pCC->CreateError(CONSTLIT("Unable to find specified weapon"));

			CDamageSource Source = GetDamageSourceArg(*pCC, pArgs->GetElement(1));
			CSpaceObject *pSource = Source.GetObj();
			if (pSource && pSource->IsDestroyed())
				pSource = NULL;

			CVector vPos;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(2), &vPos) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(2));

			Metric rSpeed = LIGHT_SPEED * pArgs->GetElement(4)->GetIntegerValue() / 100.0;
			CSpaceObject *pTarget = CreateObjFromItem(*pCC, pArgs->GetElement(5));

			//	Options

			bool bDetonateNow = false;
			bool bFireEffect = false;
			bool bSoundEffect = false;

			if (pArgs->GetCount() > 6)
				{
				ICCItem *pOptions = pArgs->GetElement(6);
				if (pOptions->IsList())
					{
					for (i = 0; i < pOptions->GetCount(); i++)
						{
						if (strEquals(pOptions->GetElement(i)->GetStringValue(), CONSTLIT("detonateNow")))
							bDetonateNow = true;
						else if (strEquals(pOptions->GetElement(i)->GetStringValue(), CONSTLIT("fireEffect")))
							bFireEffect = true;
						else if (strEquals(pOptions->GetElement(i)->GetStringValue(), CONSTLIT("soundEffect")))
							bSoundEffect = true;
						else
							return pCC->CreateError(CONSTLIT("Invalid option"), pOptions->GetElement(i));
						}
					}
				else if (pOptions->IsIdentifier() && strEquals(pOptions->GetStringValue(), CONSTLIT("detonateNow")))
					bDetonateNow = true;
				else if (pOptions->IsIdentifier() && strEquals(pOptions->GetStringValue(), CONSTLIT("fireEffect")))
					bFireEffect = true;
				else if (pOptions->IsIdentifier() && strEquals(pOptions->GetStringValue(), CONSTLIT("soundEffect")))
					bSoundEffect = true;
				else if (!pOptions->IsNil())
					bDetonateNow = true;
				}

			//	Bonus

			int iBonus = ((pArgs->GetCount() > 7) ? pArgs->GetElement(7)->GetIntegerValue() : 0);

			//	If direction is Nil then we compute intercept course

			int iDir;
			if (pArgs->GetElement(3)->IsNil())
				{
				CVector vLaunchPos = vPos;
				CVector vLaunchVel;
				if (Source.GetObj())
					vLaunchVel = Source.GetObj()->GetVel();

				if (pTarget)
					{
					CVector vTarget = pTarget->GetPos() - vLaunchPos;
					CVector vTargetVel = pTarget->GetVel() - vLaunchVel;
					Metric rTimeToIntercept = CalcInterceptTime(vTarget, vTargetVel, rSpeed);
					if (rTimeToIntercept > 0.0)
						{
						CVector vInterceptPoint = vTarget + vTargetVel * rTimeToIntercept;
						iDir = VectorToPolar(vInterceptPoint);
						}
					else
						iDir = VectorToPolar(vTarget);
					}
				else
					iDir = 0;
				}
			else
				iDir = AngleMod(pArgs->GetElement(3)->GetIntegerValue());

			//	Create barrel flash effect

			if (bFireEffect)
				pDesc->CreateFireEffect(pSystem, pSource, vPos, CVector(), iDir);

			//	If we have a bonus, we need an enhancement stack

			TSharedPtr<CItemEnhancementStack> pEnhancements;
			if (iBonus != 0)
				{
				pEnhancements.TakeHandoff(new CItemEnhancementStack);
				pEnhancements->InsertHPBonus(iBonus);
				}

			//	Create the weapon shot

			SShotCreateCtx Ctx;
			Ctx.pDesc = pDesc;
			Ctx.pEnhancements = pEnhancements;
			Ctx.Source = Source;
			Ctx.vPos = vPos;
			Ctx.vVel = PolarToVector(iDir, rSpeed);
			Ctx.iDirection = iDir;
			Ctx.pTarget = pTarget;
			Ctx.dwFlags = (bDetonateNow ? SShotCreateCtx::CWF_EXPLOSION : SShotCreateCtx::CWF_WEAPON_FIRE);

			CSpaceObject *pObj;
			if (error = pSystem->CreateWeaponFire(Ctx, &pObj))
				return pCC->CreateNil();

			//	Detonate the shot

			if (bDetonateNow)
				pObj->DetonateNow(NULL);

			//	Sound

			if (bSoundEffect)
				pDesc->PlayFireSound(pObj);

			//	DOne

			return pCC->CreateInteger((int)pObj);
			}

		case FN_SYS_PLAY_SOUND:
			{
			int iChannel = g_pUniverse->FindSound(pArgs->GetElement(0)->GetIntegerValue());
			if (iChannel == -1)
				return pCC->CreateNil();

			CSpaceObject *pPos = NULL;
			if (pArgs->GetCount() > 1)
				pPos = CreateObjFromItem(*pCC, pArgs->GetElement(1));

			g_pUniverse->PlaySound(pPos, iChannel);
			return pCC->CreateTrue();
			}

		default:
			ASSERT(false);
		}

	return pCC->CreateNil();
	}

ICCItem *fnSystemCreateEffect (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnSystemCreateEffect
//
//	(sysCreateEffect effectID anchor pos [rotation] [variant] [data])

	{
	ALERROR error;

	CCodeChain *pCC = pEvalCtx->pCC;

	//	Get the arguments

	int iArg = 0;
	ICCItem *pEffectType = pArgs->GetElement(iArg++);
	CSpaceObject *pAnchor = CreateObjFromItem(*pCC, pArgs->GetElement(iArg++));

	CVector vPos;
	if (GetPosOrObject(pEvalCtx, pArgs->GetElement(iArg++), &vPos) != NOERROR)
		return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(2));

	int iRotation = 0;
	int iVariant = 0;
	ICCItem *pData = NULL;

	if (pArgs->GetCount() > 3)
		{
		if (pArgs->GetElement(iArg)->IsSymbolTable())
			pData = pArgs->GetElement(iArg++);
		else
			{
			iRotation = pArgs->GetElement(iArg++)->GetIntegerValue();

			if (pArgs->GetCount() > 4)
				{
				if (pArgs->GetElement(iArg)->IsSymbolTable())
					pData = pArgs->GetElement(iArg++);
				else
					{
					iVariant = pArgs->GetElement(iArg++)->GetIntegerValue();

					if (pArgs->GetCount() > 5)
						{
						if (pArgs->GetElement(iArg)->IsSymbolTable())
							pData = pArgs->GetElement(iArg++);
						}
					}
				}
			}
		}

	//	Get the effect type

	CEffectCreator *pCreator = g_pUniverse->FindEffectType(pEffectType->GetIntegerValue());
	if (pCreator == NULL)
		return pCC->CreateError(CONSTLIT("Unknown effect type"), pEffectType);

	//	Create

	CSystem *pSystem = g_pUniverse->GetCurrentSystem();
	if (pSystem == NULL)
		return StdErrorNoSystem(*pCC);

	if (error = pCreator->CreateEffect(pSystem,
			pAnchor,
			vPos,
			NullVector,
			iRotation,
			iVariant,
			pData))
		return pCC->CreateError(CONSTLIT("Error creating effect"), pCC->CreateInteger(error));

	//	Done

	return pCC->CreateTrue();
	}

ICCItem *fnSystemCreateMarker (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnSystemCreateMarker
//
//	(sysCreateMarker name pos sovereignID) -> marker

	{
	ALERROR error;

	CCodeChain *pCC = pEvalCtx->pCC;

	//	Evaluate the arguments and validate them

	ICCItem *pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("svi"));
	if (pArgs->IsError())
		return pArgs;

	CString sName = pArgs->GetElement(0)->GetStringValue();

	//	The position can be either a list (in which case it is a position)
	//	or an integer (in which case it is a gate object)

	CVector vPos;
	if (GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vPos) != NOERROR)
		return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));

	//	Sovereign

	DWORD dwSovereignID = pArgs->GetElement(2)->GetIntegerValue();
	CSovereign *pSovereign = g_pUniverse->FindSovereign(dwSovereignID);

	pArgs->Discard(pCC);

	//	Create

	CSystem *pSystem = g_pUniverse->GetCurrentSystem();
	if (pSystem == NULL)
		return StdErrorNoSystem(*pCC);

	CMarker *pObj;
	if (error = CMarker::Create(pSystem,
			pSovereign,
			vPos,
			NullVector,
			sName,
			&pObj))
		return pCC->CreateError(CONSTLIT("Error creating marker"), pCC->CreateInteger(error));

	//	Done

	return pCC->CreateInteger((int)pObj);
	}

ICCItem *fnSystemCreateShip (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnSystemCreateShip
//
//	(sysCreateShip unid pos sovereignID [controller]) -> ship

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	int i;

	CSystem *pSystem = g_pUniverse->GetCurrentSystem();
	if (pSystem == NULL)
		return StdErrorNoSystem(*pCC);

	//	Get the arguments

	DWORD dwClassID = pArgs->GetElement(0)->GetIntegerValue();
	DWORD dwSovereignID = pArgs->GetElement(2)->GetIntegerValue();

	//	The position can be either a list (in which case it is a position)
	//	or an integer (in which case it is a gate object)

	CVector vPos;
	CSpaceObject *pGate;
	if (GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vPos, &pGate) != NOERROR)
		return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));

	//	Validate

	CDesignType *pType = g_pUniverse->FindDesignType(dwClassID);
	if (pType == NULL)
		return pCC->CreateError(CONSTLIT("Unknown ship class ID"), pArgs->GetElement(0));

	if (pType->GetType() != designShipClass
			&& pType->GetType() != designShipTable)
		return pCC->CreateError(CONSTLIT("Type must be <ShipClass> or <ShipTable>"), pArgs->GetElement(0));

	CSovereign *pSovereign = g_pUniverse->FindSovereign(dwSovereignID);
	if (pSovereign == NULL)
		{
		//	Error only if we specified a non-nil sovereign and if we're not
		//	creating a ship table (ship tables can specify a sovereign themselves).

		if (pType->GetType() != designShipTable || dwSovereignID != 0)
			return pCC->CreateError(CONSTLIT("Unknown sovereign ID"), pArgs->GetElement(2));
		}

	//	Options

	IShipController *pController = NULL;
	CDesignType *pOverride = NULL;
	CSpaceObject *pTarget = NULL;
	CSpaceObject *pBase = NULL;
	DWORD dwTableFlags = SShipCreateCtx::RETURN_RESULT;
	if (pArgs->GetCount() > 3)
		{
		if (pArgs->GetElement(3)->IsSymbolTable())
			{
			ICCItem *pOptions = pArgs->GetElement(3);
			ICCItem *pArg;

			if (pArg = pOptions->GetElement(CONSTLIT("base")))
				pBase = CreateObjFromItem(*pCC, pArg, CCUTIL_FLAG_CHECK_DESTROYED);

			if ((pArg = pOptions->GetElement(CONSTLIT("controller")))
					&& !pArg->IsNil())
				{
				pController = g_pUniverse->CreateShipController(pArg->GetStringValue());
				if (pController == NULL)
					return pCC->CreateError(CONSTLIT("Unknown controller"), pArg);
				}

			if ((pArg = pOptions->GetElement(CONSTLIT("eventHandler")))
					&& !pArg->IsNil())
				{
				pOverride = g_pUniverse->FindDesignType(pArg->GetIntegerValue());
				if (pOverride == NULL)
					return pCC->CreateError(CONSTLIT("Unknown event handler"), pArg);
				}

			if (pArg = pOptions->GetElement(CONSTLIT("target")))
				pTarget = CreateObjFromItem(*pCC, pArg, CCUTIL_FLAG_CHECK_DESTROYED);

			if (pOptions->GetBooleanAt(CONSTLIT("returnEscorts")))
				dwTableFlags |= SShipCreateCtx::RETURN_ESCORTS;
			}
		else if (pArgs->GetElement(3)->IsIdentifier())
			pController = g_pUniverse->CreateShipController(pArgs->GetElement(3)->GetStringValue());
		else
			pOverride = g_pUniverse->FindDesignType(pArgs->GetElement(3)->GetIntegerValue());
		}

	//	If we have a ship table, then we go through a totally different path

	if (pType->GetType() == designShipTable)
		{
		CShipTable *pTable = CShipTable::AsType(pType);
		if (pTable == NULL)
			return pCC->CreateNil();

		SShipCreateCtx CreateCtx;
		CreateCtx.pSystem = pSystem;
		CreateCtx.pGate = pGate;
		CreateCtx.vPos = vPos;
		CreateCtx.pBaseSovereign = pSovereign;
		CreateCtx.pEncounterInfo = NULL;
		CreateCtx.pOverride = pOverride;
		CreateCtx.pTarget = pTarget;
		CreateCtx.dwFlags = dwTableFlags;

		//	Create

		pTable->CreateShips(CreateCtx);

		//	Return at least one of the ships created

		if (CreateCtx.Result.GetCount() == 0)
			return pCC->CreateNil();
		else if (CreateCtx.Result.GetCount() == 1)
			return pCC->CreateInteger((int)CreateCtx.Result.GetObj(0));
		else
			{
			ICCItem *pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;

			for (i = 0; i < CreateCtx.Result.GetCount(); i++)
				pList->AppendInteger(*pCC, (int)CreateCtx.Result.GetObj(i));

			return pResult;
			}
		}

	//	Otherwise, we create a ship

	else
		{
		CShip *pShipCreated;
		ALERROR error;
		if (error = pSystem->CreateShip(dwClassID,
				pController,
				pOverride,
				pSovereign,
				vPos,
				CVector(),
				mathRandom(0, 359),
				pGate,
				NULL,
				&pShipCreated))
			{
			//	ERR_NOTFOUND is not an error--it means that a ship table did not
			//	create any ships.

			if (error == ERR_NOTFOUND)
				return pCC->CreateNil();
			else
				return pCC->CreateError(CONSTLIT("Error creating ship"), pCC->CreateInteger(error));
			}

		//	Done

		return pCC->CreateInteger((int)pShipCreated);
		}
	}

ICCItem *fnSystemCreateStation (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnSystemCreateStation
//
//	(sysCreateStargate unid pos gateID [destNodeID destGateID]) -> obj
//	(sysCreateStation unid pos) -> station

	{
	ALERROR error;
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	SSystemCreateCtx *pSysCreateCtx = pCtx->GetSystemCreateCtx();

	CSystem *pSystem = g_pUniverse->GetCurrentSystem();
	if (pSystem == NULL)
		return StdErrorNoSystem(*pCC);

	//	Get the station type

	CStationType *pType = g_pUniverse->FindStationType(pArgs->GetElement(0)->GetIntegerValue());
	if (pType == NULL)
		return pCC->CreateError(CONSTLIT("Unknown station type"), pArgs->GetElement(0));

	//	Get the position

	CVector vPos;
	int iLocID;
	if (error = GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vPos, NULL, &iLocID))
		{
		//	If we couldn't find a location, then we abort, but return Nil
		//	so that callers can recover.

		if (error == ERR_NOTFOUND)
			{
			if (g_pUniverse->InDebugMode())
				::kernelDebugLogPattern("WARNING: Unable to create station at %s (in %s)", pArgs->GetElement(1)->Print(pCC), pSystem->GetName());
			return pCC->CreateNil();
			}

		//	Otherwise, this is a real error.

		else
			return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));
		}

	//	If we're creating a stargate, get some extra information

	CDesignType *pEventHandler = NULL;
	CString sStargateName;
	CTopologyNode *pDestNode = NULL;
	CString sDestNode;
	CString sDestName;
	if (dwData == FN_SYS_CREATE_STARGATE)
		{
		sStargateName = pArgs->GetElement(2)->GetStringValue();
		if (sStargateName.IsBlank())
			return pCC->CreateError(CONSTLIT("Stargate must have a name"), pArgs->GetElement(2));

		//	Look for the stargate in the topology

		pDestNode = pSystem->GetStargateDestination(sStargateName, &sDestName);
		
		//	If the stargate doesn't exist yet, then see if we have enough parameters
		//	to create it.

		if (pDestNode == NULL)
			{
			if (pArgs->GetCount() < 5)
				return pCC->CreateError(CONSTLIT("Topology does not have stargate of that name"), pArgs->GetElement(2));

			sDestNode = pArgs->GetElement(3)->GetStringValue();
			if (sDestNode.IsBlank())
				return pCC->CreateError(CONSTLIT("Invalid destination node"), pArgs->GetElement(3));

			sDestName = pArgs->GetElement(4)->GetStringValue();
			if (sDestName.IsBlank())
				return pCC->CreateError(CONSTLIT("Invalid destination stargate"), pArgs->GetElement(4));
			}
		else
			sDestNode = pDestNode->GetID();
		}
	else
		{
		if (pArgs->GetCount() >= 3)
			{
			pEventHandler = g_pUniverse->FindDesignType(pArgs->GetElement(2)->GetIntegerValue());
			if (pEventHandler == NULL)
				return pCC->CreateError(CONSTLIT("Invalid event handler"), pArgs->GetElement(2));
			}
		}

	//	Make sure we can encounter the station

	if (!pType->CanBeEncountered(pSystem))
		return pCC->CreateNil();

	//	Create the station (or ship encounter). If we are in the middle of system
	//	create, then we can use the proper context. Otherwise, we do without.

	CSpaceObject *pStation;
	if (pSysCreateCtx && iLocID != -1)
		{
		CLocationDef *pLoc = pSystem->GetLocation(iLocID);

		SObjCreateCtx CreateCtx(*pSysCreateCtx);
		CreateCtx.vPos = vPos;
		CreateCtx.pLoc = pLoc;
		CreateCtx.pOrbit = &pLoc->GetOrbit();
		CreateCtx.bCreateSatellites = true;
		CreateCtx.pEventHandler = pEventHandler;

		if (pSystem->CreateStation(pSysCreateCtx,
				pType,
				CreateCtx,
				&pStation) != NOERROR)
			return pCC->CreateError(CONSTLIT("Unable to create station"), NULL);
		}

	//	Otherwise, we do without a context

	else
		{
		if (pSystem->CreateStation(pType, pEventHandler, vPos, &pStation) != NOERROR)
			return pCC->CreateError(CONSTLIT("Unable to create station"), NULL);
		}

	//	If we have a location, remove it from the list of available locations

	if (iLocID != -1 && pStation)
		pSystem->SetLocationObjID(iLocID, pStation->GetID());

	//	If we're creating a stargate, do some extra stuff

	if (dwData == FN_SYS_CREATE_STARGATE)
		{
		CStation *pStargate = (pStation ? pStation->AsStation() : NULL);

		//	Must have a station

		if (pStargate == NULL)
			return pCC->CreateError(CONSTLIT("Stargate must be a station"), NULL);

		//	If we don't have a destination node, then we need to add the stargate

		if (pDestNode == NULL)
			{
			CTopologyNode *pNode = pSystem->GetTopology();
			if (pNode == NULL)
				return pCC->CreateError(CONSTLIT("No topology for current system"), NULL);

			CTopologyNode::SStargateDesc GateDesc;
			GateDesc.sName = sStargateName;
			GateDesc.sDestNode = sDestNode;
			GateDesc.sDestName = sDestName;
			if (pNode->AddStargateAndReturn(GateDesc) != NOERROR)
				return pCC->CreateError(CONSTLIT("Unable to add stargate to topology node"), NULL);

			pDestNode = g_pUniverse->FindTopologyNode(sDestNode);
			if (pDestNode == NULL)
				return pCC->CreateError(CONSTLIT("Unknown topology node"), NULL);
			}

		//	Add a named object

		pSystem->NameObject(sStargateName, pStargate);

		//	Set stargate properties (note: CreateStation also looks at objName and adds the name
		//	to the named-objects system table.)

		pStargate->SetStargate(sDestNode, sDestName);

		//	If we haven't already set the name, set the name of the stargate
		//	to include the name of the destination system

		if (!pStargate->IsNameSet())
			pStargate->SetName(strPatternSubst(CONSTLIT("%s Stargate"), pDestNode->GetSystemName()), nounDefiniteArticle);
		}

	//	Done

	if (pStation)
		return pCC->CreateInteger((int)pStation);
	else
		return pCC->CreateNil();
	}

ICCItem *fnSystemFind (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnSystemFind
//
//	Finds an object in the system

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	int i;

	//	Convert the first argument into an object (may be Nil)

	CSpaceObject *pSource = CreateObjFromItem(*pCC, pArgs->GetElement(0));

	//	Second argument is the filter

	CString sFilter = pArgs->GetElement(1)->GetStringValue();
	CSpaceObjectCriteria Criteria(pSource, sFilter);

	//	If we're checking for position, we need to do some extra work

	if (dwData == FN_SYS_FIND_AT_POS)
		{
		CVector vPos1;
		CVector vPos2;

		if (GetPosOrObject(pEvalCtx, pArgs->GetElement(2), &vPos1) != NOERROR)
			return pCC->CreateError(CONSTLIT("Invalid position"), pArgs->GetElement(2));

		//	If we have two position parameters, then this is a line intersection

		if (pArgs->GetCount() >= 4)
			{
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(3), &vPos2) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid position"), pArgs->GetElement(3));

			Criteria.SetLineIntersect(vPos1, vPos2);
			}

		//	Otherwise, just a point intersection

		else
			{
			Criteria.SetPosIntersect(vPos1);
			}
		}

	//	Get the system

	CSystem *pSystem = g_pUniverse->GetCurrentSystem();
	if (pSystem == NULL)
		return StdErrorNoSystem(*pCC);

	//	Prepare result list (if necessary)
	
	ICCItem *pResult;
	CCLinkedList *pList;
	if (!Criteria.MatchesNearestOnly() && !Criteria.MatchesFarthestOnly())
		{
		pResult = pCC->CreateLinkedList();
		if (pResult->IsError())
			return pResult;

		pList = (CCLinkedList *)pResult;
		}
	else
		{
		pResult = NULL;
		pList = NULL;
		}

	//	If we want a sorted list then we let MatchesCriteria do the work.
	//	NOTE: We have this convoluted code path because we want to optimize
	//	adding items to our list [not sure if it's worth it, though].

	bool bGenerateOurOwnList = (pList && (Criteria.GetSort() == CSpaceObjectCriteria::sortNone));

	//	Do the search

	CSpaceObjectCriteria::SCtx Ctx(Criteria);
	for (i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);

		//	NOTE: Sometimes we want to find virtual objects, so always include 
		//	them if they match.

		if (pObj 
				&& pObj->MatchesCriteria(Ctx, Criteria)
				&& (!pObj->IsIntangible() || pObj->IsVirtual()))
			{
			if (bGenerateOurOwnList)
				pList->AppendInteger(*pCC, (int)pObj);
			}
		}

	//	If we only want the nearest/farthest object, then find it now

	if (Criteria.MatchesNearestOnly() || Criteria.MatchesFarthestOnly())
		{
		//	Return the object

		if (Ctx.pBestObj)
			return pCC->CreateInteger((int)Ctx.pBestObj);
		else
			return pCC->CreateNil();
		}

	//	If the search generated a list for us, then we need to add the items
	//	to our result set now.

	else if (!bGenerateOurOwnList)
		{
		for (i = 0; i < Ctx.DistSort.GetCount(); i++)
			pList->AppendInteger(*pCC, (int)Ctx.DistSort[i]);

		return pResult;
		}

	//	Otherwise the result is ready

	else
		return pResult;
	}

ICCItem *fnSystemGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnSystemGet
//
//	(sysGetName)

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	switch (dwData)
		{
		case FN_SYS_ADD_STARGATE_TOPOLOGY:
			{
			CTopologyNode *pNode;
			int iArg = 0;

			//	If we have more than one arg, then the first arg is
			//	the node ID.

			if (pArgs->GetCount() == 3)
				{
				CSystem *pSystem = g_pUniverse->GetCurrentSystem();
				if (pSystem == NULL)
					return StdErrorNoSystem(*pCC);

				pNode = pSystem->GetTopology();
				}
			else
				pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(iArg++)->GetStringValue());

			if (pNode == NULL)
				return pCC->CreateError(CONSTLIT("Invalid nodeID"), pArgs->GetElement(0));

			CTopologyNode::SStargateDesc GateDesc;
			GateDesc.sName = pArgs->GetElement(iArg++)->GetStringValue();
			GateDesc.sDestNode = pArgs->GetElement(iArg++)->GetStringValue();
			GateDesc.sDestName = pArgs->GetElement(iArg++)->GetStringValue();

			if (pNode->FindStargate(GateDesc.sName))
				return pCC->CreateNil();

			if (pNode->AddStargateAndReturn(GateDesc) != NOERROR)
				return pCC->CreateError(CONSTLIT("Unable to add stargate"), pArgs);

			return pCC->CreateTrue();
			}

		case FN_SYS_ASCEND_OBJECT:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0));
			if (pObj == NULL)
				return pCC->CreateNil();

			CString sError;
			if (!pSystem->AscendObject(pObj, &sError))
				return pCC->CreateError(sError);

			return pCC->CreateTrue();
			}

		case FN_SYS_DESCEND_OBJECT:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			DWORD dwObjID = pArgs->GetElement(0)->GetIntegerValue();

			CVector vPos;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vPos) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));

			CSpaceObject *pObj;
			if (!pSystem->DescendObject(dwObjID, vPos, &pObj))
				return pCC->CreateNil();

			return pCC->CreateInteger((int)pObj);
			}

		case FN_SYS_ENVIRONMENT:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			CVector vPos;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(0), &vPos) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(0));

			CSpaceEnvironmentType *pEnvironment = pSystem->GetSpaceEnvironment(vPos);
			if (pEnvironment)
				return pCC->CreateInteger(pEnvironment->GetUNID());
			else
				return pCC->CreateNil();
			}

		case FN_SYS_GET_FIRE_SOLUTION:
			{
			CVector vPos;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(0), &vPos) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(0));

			CVector vVel = CreateVectorFromList(*pCC, pArgs->GetElement(1));
			Metric rSpeed;
			if (pArgs->GetElement(2)->IsList())
				{
				CVector vVel = CreateVectorFromList(*pCC, pArgs->GetElement(2));
				rSpeed = vVel.Length();
				}
			else
				{
				rSpeed = LIGHT_SPEED * pArgs->GetElement(2)->GetIntegerValue() / 100.0;
				if (rSpeed < 0.0)
					return pCC->CreateError(CONSTLIT("Speed cannot be negative"), pArgs->GetElement(2));
				}

			Metric rTimeToIntercept = CalcInterceptTime(vPos, vVel, rSpeed);
			if (rTimeToIntercept < 0.0)
				return pCC->CreateNil();

			CVector vInterceptPoint = vPos + vVel * rTimeToIntercept;
			return pCC->CreateInteger(VectorToPolar(vInterceptPoint));
			}

		case FN_SYS_GET_PROPERTY:
			{
			CTopologyNode *pNode;
			int iArg = 0;

			//	If we have more than one arg, then the first arg is
			//	the node ID.

			if (pArgs->GetCount() == 1)
				{
				CSystem *pSystem = g_pUniverse->GetCurrentSystem();
				if (pSystem == NULL)
					return StdErrorNoSystem(*pCC);

				pNode = pSystem->GetTopology();
				}
			else
				pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(iArg++)->GetStringValue());

			if (pNode == NULL)
				return pCC->CreateError(CONSTLIT("Invalid nodeID"), pArgs->GetElement(0));

			CString sProperty = pArgs->GetElement(iArg++)->GetStringValue();

			//	Get the property

			return pNode->GetProperty(sProperty);
			}
		case FN_SYS_GET_POV:
			{
			//	Get the POV

			CSpaceObject *pObj = g_pUniverse->GetPOV();
			return ::CreateObjPointer(*pCC, pObj);
			}
		case FN_SYS_GET_STD_COMBAT_STRENGTH:
			{
			int iLevel = pArgs->GetElement(0)->GetIntegerValue();
			if (iLevel <= 0 || iLevel > MAX_SYSTEM_LEVEL)
				return pCC->CreateNil();

			return pCC->CreateInteger((int)(CShipClass::GetStdCombatStrength(iLevel) + 0.5));
			}

		case FN_SYS_HIT_SCAN:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			int iArg = -1;
			CSpaceObject *pSource = NULL;
			pSource = CreateObjFromItem(*pCC, pArgs->GetElement(++iArg));

			//	Get ray

			CVector vStart;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(++iArg), &vStart) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(iArg));

			CVector vEnd;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(++iArg), &vEnd) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(iArg));

			//	Options

			bool bExcludeWorlds = false;
			if (pArgs->GetCount() >= 4)
				{
				ICCItem *pOptions = pArgs->GetElement(++iArg);
				bExcludeWorlds = pOptions->GetBooleanAt(FIELD_EXCLUDE_WORLDS);
				}

			//	Hit test

			CVector vHitPos;
			CSpaceObject *pHitObj = pSystem->HitScan(pSource, vStart, vEnd, bExcludeWorlds, &vHitPos);
			if (pHitObj == NULL)
				return pCC->CreateNil();

			//	Create a list

			ICCItem *pResult = pCC->CreateLinkedList();
			ICCItem *pObjItem = ::CreateObjPointer(*pCC, pHitObj);
			pResult->Append(*pCC, pObjItem);
			pObjItem->Discard(pCC);

			ICCItem *pVector = CreateListFromVector(*pCC, vHitPos);
			pResult->Append(*pCC, pVector);
			pVector->Discard(pCC);

			//	Done

			return pResult;
			}

		case FN_SYS_HIT_TEST:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			//	Source

			int iArg = -1;
			CSpaceObject *pSource = NULL;
			pSource = CreateObjFromItem(*pCC, pArgs->GetElement(++iArg));

			//	Get the position

			CVector vPos;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(++iArg), &vPos) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(iArg));

			//	Options

			bool bSourceOnly = false;
			bool bExcludeWorlds = false;
			if (pArgs->GetCount() >= 2)
				{
				ICCItem *pOptions = pArgs->GetElement(++iArg);
				bSourceOnly = pOptions->GetBooleanAt(FIELD_SOURCE_ONLY);
				bExcludeWorlds = pOptions->GetBooleanAt(FIELD_EXCLUDE_WORLDS);
				}

			//	If we only care about the source, then just test it.

			if (bSourceOnly)
				{
				if (pSource && pSource->PointInObject(pSource->GetPos(), vPos))
					return ::CreateObjPointer(*pCC, pSource);
				else
					return pCC->CreateNil();
				}

			//	Hit test

			return ::CreateObjPointer(*pCC, pSystem->HitTest(pSource, vPos, bExcludeWorlds));
			}

		case FN_SYS_ITEM_BUY_PRICE:
			{
			int iArg = 0;

			//	If we have more than 1 args, and the arg is a string,
			//	then the first arg is the node ID.

			CTopologyNode *pNode;
			if (pArgs->GetCount() > 1 && pArgs->GetElement(0)->IsIdentifier())
				{
				pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(iArg++)->GetStringValue());
				if (pNode == NULL)
					return pCC->CreateError(CONSTLIT("Invalid nodeID"), pArgs->GetElement(0));
				}

			//	Otherwise, we assume the current system.

			else
				{
				CSystem *pSystem = g_pUniverse->GetCurrentSystem();
				if (pSystem == NULL)
					return StdErrorNoSystem(*pCC);

				pNode = pSystem->GetTopology();
				if (pNode == NULL)
					return pCC->CreateError(CONSTLIT("No topology node"));
				}

			//	Get the item type

			CItem Item = GetItemFromArg(*pCC, pArgs->GetElement(iArg++));
			CItemType *pType = Item.GetType();
			if (pType == NULL)
				return pCC->CreateNil();

			//	If we have more args then this is a type criteria

			CDesignTypeCriteria Criteria;
			if (iArg < pArgs->GetCount())
				{
				ICCItem *pCriteria = pArgs->GetElement(iArg++);
				if (pCriteria && !pCriteria->IsNil())
					{
					if (CDesignTypeCriteria::ParseCriteria(pCriteria->GetStringValue(), &Criteria) != NOERROR)
						return pCC->CreateError(CONSTLIT("Invalid criteria"), pCriteria);
					}
				}

			//	Get the item price (in item's default currency)

			int iPrice = CTradingComputer::GetItemBuyPrice(*g_pUniverse, pNode, Criteria, Item);
			if (iPrice == 0)
				return pCC->CreateNil();

			//	Done

			return pCC->CreateInteger(iPrice);
			}

		case FN_SYS_RANDOM_LOCATION:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			//	Get the criteria and parse it

			CString sCriteria = pArgs->GetElement(0)->GetStringValue();
			if (sCriteria.IsBlank())
				return pCC->CreateNil();

			SLocationCriteria Criteria;
			if (Criteria.AttribCriteria.Parse(sCriteria) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid location criteria"), pArgs->GetElement(0));

			//	Get some options

			ICCItem *pOptions = (pArgs->GetCount() > 1 ? pArgs->GetElement(1) : NULL);
			ICCItem *pRemove = (pOptions ? pOptions->GetElement(FIELD_REMOVE) : NULL);
			bool bRemoveLoc = (pRemove && !pRemove->IsNil());

			//	See if we specified an object type

			ICCItem *pObjType = (pOptions ? pOptions->GetElement(FIELD_OBJ_TYPE) : NULL);
			CStationType *pType = (pObjType ? g_pUniverse->FindStationType(pObjType->GetIntegerValue()) : NULL);

			//	Get a random location. If we can't find one, we return Nil.

			int iLocID;
			if (!pSystem->FindRandomLocation(Criteria, 0, COrbit(), pType, &iLocID))
				return pCC->CreateNil();

			//	Get the data

			CLocationDef *pLoc = pSystem->GetLocation(iLocID);
			ICCItem *pPos = CreateListFromVector(*pCC, pLoc->GetOrbit().GetObjectPos());
			ICCItem *pOrbit = CreateListFromOrbit(*pCC, pLoc->GetOrbit());

			//	Prepare a result

			ICCItem *pResult = pCC->CreateSymbolTable();
			pResult->SetStringAt(*pCC, FIELD_ATTRIBS, pLoc->GetAttributes());
			pResult->SetAt(*pCC, FIELD_POS, pPos);
			pPos->Discard(pCC);
			pResult->SetAt(*pCC, FIELD_ORBIT, pOrbit);
			pOrbit->Discard(pCC);

			//	If necessary, remove the location

			if (bRemoveLoc)
				pLoc->SetBlocked();

			//	Done

			return pResult;
			}

		case FN_SYS_SET_PROPERTY:
			{
			CTopologyNode *pNode;
			int iArg = 0;

			//	If we have more than two args, then the first arg is
			//	the node ID.

			CSystem *pSystem = NULL;
			if (pArgs->GetCount() <= 2)
				{
				pSystem = g_pUniverse->GetCurrentSystem();
				if (pSystem == NULL)
					return StdErrorNoSystem(*pCC);

				pNode = pSystem->GetTopology();
				}
			else
				pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(iArg++)->GetStringValue());

			if (pNode == NULL)
				return pCC->CreateError(CONSTLIT("Invalid nodeID"), pArgs->GetElement(0));

			CString sProperty = pArgs->GetElement(iArg++)->GetStringValue();
			ICCItem *pValue = pArgs->GetElement(iArg++);

			//	Set the property

			CString sError;
			if (!pNode->SetProperty(sProperty, pValue, &sError))
				{
				if (sError.IsBlank())
					return pCC->CreateError(strPatternSubst(CONSTLIT("Invalid property: %s"), sProperty));
				else
					return pCC->CreateError(sError);
				}

			return pCC->CreateTrue();
			}

		case FN_SYS_GET_TRAVEL_DISTANCE:
			{
			Metric rSpeed;
			if (pArgs->GetElement(0)->IsList())
				{
				CVector vVel = CreateVectorFromList(*pCC, pArgs->GetElement(0));
				rSpeed = g_SecondsPerUpdate * vVel.Length() / LIGHT_SPEED;
				}
			else
				{
				rSpeed = g_SecondsPerUpdate * pArgs->GetElement(0)->GetIntegerValue() / 100.0;
				if (rSpeed < 0.0)
					return pCC->CreateError(CONSTLIT("Speed cannot be negative"), pArgs->GetElement(0));
				}

			Metric rTime = pArgs->GetElement(1)->GetIntegerValue();
			if (rTime < 0.0)
				return pCC->CreateError(CONSTLIT("Time cannot be negative"), pArgs->GetElement(1));

			return pCC->CreateInteger((int)((rSpeed * rTime) + 0.5));
			}

		case FN_SYS_GET_TRAVEL_TIME:
			{
			Metric rDistance = pArgs->GetElement(0)->GetIntegerValue();
			if (rDistance < 0.0)
				return pCC->CreateError(CONSTLIT("Distance cannot be negative"), pArgs->GetElement(0));

			Metric rSpeed = g_SecondsPerUpdate * pArgs->GetElement(1)->GetIntegerValue() / 100.0;
			if (rSpeed < 0.0)
				return pCC->CreateError(CONSTLIT("Speed cannot be negative"), pArgs->GetElement(1));
			else if (rSpeed == 0.0)
				return pCC->CreateNil();

			return pCC->CreateInteger((int)((rDistance / rSpeed) + 0.5));
			}

		case FN_SYS_INC_DATA:
			{
			int iArg = 0;

			//	If we have 3 args, then we assume the first arg is a node ID.

			CTopologyNode *pNode;
			if (pArgs->GetCount() >= 3)
				pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(iArg++)->GetStringValue());

			//	Otherwise, we assume the current system.

			else
				{
				CSystem *pSystem = g_pUniverse->GetCurrentSystem();
				if (pSystem == NULL)
					return StdErrorNoSystem(*pCC);

				pNode = pSystem->GetTopology();
				}

			//	Node better be valid

			if (pNode == NULL)
				return pCC->CreateNil();

			//	Get the attribute

			CString sAttrib = pArgs->GetElement(iArg++)->GetStringValue();

			//	Get the increment

			ICCItem *pInc = pArgs->GetElement(iArg++);

			//	Increment

			return pNode->IncData(sAttrib, pInc)->Reference();
			}

		case FN_SYS_GET_DATA:
		case FN_SYS_SET_DATA:
			{
			CTopologyNode *pNode;
			CString sAttrib;
			int iArg = 0;

			//	If we have more than one arg, then the first arg is
			//	the node ID.

			if ((pArgs->GetCount() == 1 && dwData == FN_SYS_GET_DATA)
					|| (pArgs->GetCount() == 2 && dwData == FN_SYS_SET_DATA))
				{
				CSystem *pSystem = g_pUniverse->GetCurrentSystem();
				if (pSystem == NULL)
					return StdErrorNoSystem(*pCC);

				pNode = pSystem->GetTopology();
				}
			else
				pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(iArg++)->GetStringValue());

			//	Node better be valid

			if (pNode == NULL)
				return pCC->CreateNil();

			//	Get the attribute

			sAttrib = pArgs->GetElement(iArg++)->GetStringValue();

			//	Do it

			if (dwData == FN_SYS_GET_DATA)
				return pNode->GetData(sAttrib)->Reference();
			else
				{
				pNode->SetData(sAttrib, pArgs->GetElement(iArg));
				return pCC->CreateTrue();
				}
			}

		case FN_SYS_IS_KNOWN:
			{
			CTopologyNode *pNode;

			//	If 0 args, then use current system

			if (pArgs->GetCount() == 0)
				pNode = g_pUniverse->GetCurrentSystem()->GetTopology();

			//	If 1 arg, then we expect a node ID

			else
				{
				pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(0)->GetStringValue());
				if (pNode == NULL)
					return pCC->CreateError(CONSTLIT("Invalid nodeID"), pArgs->GetElement(0));
				}

			//	Done

			return pCC->CreateBool(pNode->IsKnown());
			}

		case FN_SYS_LEVEL:
			{
			if (pArgs->GetCount() == 0)
				{
				CSystem *pSystem = g_pUniverse->GetCurrentSystem();
				if (pSystem == NULL)
					return StdErrorNoSystem(*pCC);

				return pCC->CreateInteger(pSystem->GetLevel());
				}
			else
				return fnTopologyGet(pEvalCtx, pArgs, FN_NODE_LEVEL);
			}

		case FN_SYS_LIGHT_INTENSITY:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			CVector vPos;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(0), &vPos) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(0));

			return pCC->CreateInteger(pSystem->CalculateLightIntensity(vPos));
			}

		case FN_SYS_MATCHES:
			{
			//	Parse criteria

			CTopologyNode::SCriteria Criteria;
			CString sError;
			if (CTopologyNode::ParseCriteria(pArgs->GetElement(pArgs->GetCount() - 1)->GetStringValue(), &Criteria, &sError) != NOERROR)
				return pCC->CreateError(sError);

			//	Get the topology node

			CTopologyNode *pNode;
			if (pArgs->GetCount() == 1)
				{
				CSystem *pSystem = g_pUniverse->GetCurrentSystem();
				if (pSystem == NULL)
					return StdErrorNoSystem(*pCC);

				pNode = pSystem->GetTopology();
				if (pNode == NULL)
					return pCC->CreateNil();
				}
			else
				{
				pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(0)->GetStringValue());
				if (pNode == NULL)
					return pCC->CreateError(CONSTLIT("Invalid nodeID"), pArgs->GetElement(0));
				}

			CTopologyNode::SCriteriaCtx CriteriaCtx;
			CriteriaCtx.pTopology = &g_pUniverse->GetTopology();
			return pCC->CreateBool(pNode->MatchesCriteria(CriteriaCtx, Criteria));
			}

		case FN_SYS_ORBIT_CREATE:
			{
			CVector vCenter;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(0), &vCenter) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(0));

			Metric rRadius = Max(0.0, pArgs->GetElement(1)->GetDoubleValue() * LIGHT_SECOND);
			Metric rAngle = mathDegreesToRadians(pArgs->GetElement(2)->GetDoubleValue());
			Metric rEccentricity = Max(0.0, Min((pArgs->GetCount() >= 4 ? pArgs->GetElement(3)->GetDoubleValue() : 0.0), 0.99));
			Metric rRotation = (pArgs->GetCount() >= 5 ? mathDegreesToRadians(pArgs->GetElement(4)->GetDoubleValue()) : 0.0);

			COrbit Orbit(vCenter, rRadius, rEccentricity, rRotation, rAngle);
			return CreateListFromOrbit(*pCC, Orbit);
			}

		case FN_SYS_NAME:
			{
			if (pArgs->GetCount() == 0)
				{
				CSystem *pSystem = g_pUniverse->GetCurrentSystem();
				if (pSystem == NULL)
					return StdErrorNoSystem(*pCC);

				return pCC->CreateString(pSystem->GetName());
				}
			else
				return fnTopologyGet(pEvalCtx, pArgs, FN_NODE_SYSTEM_NAME);
			}

		case FN_SYS_SET_KNOWN:
			{
			CTopologyNode *pNode;
			bool bKnown;

			//	If 0 args, then we set the state of current system to known

			if (pArgs->GetCount() == 0)
				{
				pNode = g_pUniverse->GetCurrentSystem()->GetTopology();
				bKnown = true;
				}

			//	If 1 arg, then we expect a node ID

			else if (pArgs->GetCount() == 1)
				{
				pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(0)->GetStringValue());
				if (pNode == NULL)
					{
					pNode = g_pUniverse->GetCurrentSystem()->GetTopology();
					bKnown = !pArgs->GetElement(0)->IsNil();
					}
				else
					bKnown = true;
				}

			//	If 2 args, then node ID plus True/Nil

			else
				{
				pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(0)->GetStringValue());
				if (pNode == NULL)
					return pCC->CreateError(CONSTLIT("Invalid nodeID"), pArgs->GetElement(0));

				bKnown = !pArgs->GetElement(1)->IsNil();
				}

			//	Do it

			pNode->SetKnown(bKnown);

			//	Done

			return pCC->CreateTrue();
			}

		case FN_SYS_SET_POV:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			//	Get the new POV

			CVector vCenter;
			CSpaceObject *pObj;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(0), &vCenter, &pObj) != NOERROR)
				return pCC->CreateError(CONSTLIT("Expected vector or object"), pArgs->GetElement(0));

			//	If we have an object, set the POV

			if (pObj)
				g_pUniverse->SetPOV(pObj);

			//	Otherwise we create an auto-destroy marker

			else
				{
				CPOVMarker *pMarker;
				if (CPOVMarker::Create(pSystem, vCenter, NullVector, &pMarker) != NOERROR)
					return pCC->CreateError(CONSTLIT("Out of memory."));

				g_pUniverse->SetPOV(pMarker);
				}

			//	Done

			return pCC->CreateTrue();
			}

		case FN_SYS_SYSTEM_TYPE:
			{
			CTopologyNode *pNode;
			if (pArgs->GetCount() == 0)
				pNode = g_pUniverse->GetCurrentSystem()->GetTopology();
			else
				pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(0)->GetStringValue());
			if (pNode == NULL)
				return pCC->CreateError(CONSTLIT("Invalid nodeID"), pArgs->GetElement(0));

			return pCC->CreateInteger(pNode->GetSystemTypeUNID());
			}

		case FN_SYS_STARGATES:
			{
			CTopologyNode *pNode;
			if (pArgs->GetCount() == 0)
				pNode = g_pUniverse->GetCurrentSystem()->GetTopology();
			else
				pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(0)->GetStringValue());
			if (pNode == NULL)
				return pCC->CreateError(CONSTLIT("Invalid nodeID"), pArgs->GetElement(0));

			//	Create a list

			ICCItem *pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;

			//	Loop over all stargates in this node

			for (int i = 0; i < pNode->GetStargateCount(); i++)
				{
				ICCItem *pValue = pCC->CreateString(pNode->GetStargate(i));
				pList->Append(*pCC, pValue);
				pValue->Discard(pCC);
				}

			return pResult;
			}

		case FN_SYS_STARGATE_DESTINATION:
			{
			CString sGateID;
			CTopologyNode *pNode;
			if (pArgs->GetCount() == 2)
				{
				pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(0)->GetStringValue());
				sGateID = pArgs->GetElement(1)->GetStringValue();
				}
			else
				{
				pNode = g_pUniverse->GetCurrentSystem()->GetTopology();
				sGateID = pArgs->GetElement(0)->GetStringValue();
				}

			if (pNode == NULL)
				return pCC->CreateError(CONSTLIT("Invalid nodeID"), pArgs->GetElement(0));

			CString sDestNode;
			CString sDestEntryPoint;
			pNode->FindStargate(sGateID, &sDestNode, &sDestEntryPoint);

			//	Create a list

			ICCItem *pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;
			pList->AppendString(*pCC, sDestNode);
			pList->AppendString(*pCC, sDestEntryPoint);

			//	Done

			return pResult;
			}

		case FN_SYS_STARGATE_DESTINATION_NODE:
			{
			CString sGateID;
			CTopologyNode *pNode;
			if (pArgs->GetCount() == 2)
				{
				pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(0)->GetStringValue());
				sGateID = pArgs->GetElement(1)->GetStringValue();
				}
			else
				{
				pNode = g_pUniverse->GetCurrentSystem()->GetTopology();
				sGateID = pArgs->GetElement(0)->GetStringValue();
				}

			if (pNode == NULL)
				return pCC->CreateError(CONSTLIT("Invalid nodeID"), pArgs->GetElement(0));

			CString sDestNode;
			pNode->FindStargate(sGateID, &sDestNode);

			return pCC->CreateString(sDestNode);
			}

		case FN_SYS_STARGATE_PROPERTY:
			{
			int iArg = 0;

			CTopologyNode *pNode;
			if (pArgs->GetCount() > 2)
				{
				pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(iArg++)->GetStringValue());
				if (pNode == NULL)
					return pCC->CreateError(CONSTLIT("Invalid nodeID"), pArgs->GetElement(0));
				}
			else
				{
				pNode = g_pUniverse->GetCurrentTopologyNode();
				if (pNode == NULL)
					return pCC->CreateNil();
				}

			CString sGateID = pArgs->GetElement(iArg++)->GetStringValue();
			CString sProperty = pArgs->GetElement(iArg++)->GetStringValue();

			ICCItemPtr pResult = pNode->GetStargateProperty(sGateID, sProperty);
			return pResult->Reference();
			}

		case FN_SYS_TOPOLOGY_DISTANCE:
			{
			CString sSource = pArgs->GetElement(0)->GetStringValue();
			if (g_pUniverse->FindTopologyNode(sSource) == NULL)
				return pCC->CreateError(CONSTLIT("Unknown topology node"), pArgs->GetElement(0));

			CString sDest = pArgs->GetElement(1)->GetStringValue();
			if (g_pUniverse->FindTopologyNode(sDest) == NULL)
				return pCC->CreateError(CONSTLIT("Unknown topology node"), pArgs->GetElement(1));

			int iDist = g_pUniverse->GetTopology().GetDistance(sSource, sDest);
			if (iDist >= 0)
				return pCC->CreateInteger(iDist);
			else
				return pCC->CreateNil();
			}

		case FN_SYS_HAS_ATTRIBUTE:
			{
			if (pArgs->GetCount() == 1)
				{
				CSystem *pSystem = g_pUniverse->GetCurrentSystem();
				if (pSystem == NULL)
					return StdErrorNoSystem(*pCC);

				CTopologyNode *pNode = pSystem->GetTopology();
				if (pNode == NULL)
					{
					//	HACK: If we have no node then we must be in the intro screen.
					//	We only look for the special "demoSystem" attribute.

					return pCC->CreateBool(strEquals(pArgs->GetElement(0)->GetStringValue(), CONSTLIT("demoSystem")));
					}

				return pCC->CreateBool(pNode->HasAttribute(pArgs->GetElement(0)->GetStringValue()));
				}
			else
				return fnTopologyGet(pEvalCtx, pArgs, FN_NODE_HAS_ATTRIBUTE);
			}

		case FN_SYS_NAV_PATH_POINT:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			//	Get the parameters

			CSovereign *pSovereign = g_pUniverse->FindSovereign(pArgs->GetElement(0)->GetIntegerValue());
			CSpaceObject *pStart = CreateObjFromItem(*pCC, pArgs->GetElement(1));
			CSpaceObject *pEnd = CreateObjFromItem(*pCC, pArgs->GetElement(2));
			if (pStart == NULL || pEnd == NULL)
				return CreateListFromVector(*pCC, CVector());
			int iPercent = pArgs->GetElement(3)->GetIntegerValue();
			iPercent = Max(0, Min(iPercent, 100));

			//	Get the navigation path

			CNavigationPath *pNavPath = pSystem->GetNavPath(pSovereign, pStart, pEnd);
			if (pNavPath == NULL)
				return CreateListFromVector(*pCC, pStart->GetPos());

			//	Compute the point on the path

			Metric rPathLen = pNavPath->ComputePathLength(pSystem);
			CVector vPos = pNavPath->ComputePointOnPath(pSystem, rPathLen * (iPercent / 100.0));
			return CreateListFromVector(*pCC, vPos);
			}

		case FN_SYS_NODE:
			{
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			CTopologyNode *pNode = pSystem->GetTopology();
			if (pNode == NULL)
				return pCC->CreateNil();

			return pCC->CreateString(pNode->GetID());
			}

		case FN_SYS_ALL_NODES:
			{
			int i;

			//	Create a list

			ICCItem *pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			//	If we have no parameters, then we return all nodes

			if (pArgs->GetCount() == 0)
				{
				//	Loop over all topology nodes and add the IDs to the list

				for (i = 0; i < g_pUniverse->GetTopologyNodeCount(); i++)
					{
					CTopologyNode *pNode = g_pUniverse->GetTopologyNode(i);
					if (pNode->IsEndGame())
						continue;

					pResult->AppendString(*pCC, pNode->GetID());
					}
				}

			//	Otherwise, we expect a criteria parameter

			else
				{
				ICCItem *pCriteria = pArgs->GetElement(0);
				CTopologyNode::SCriteria Criteria;

				CString sError;
				if (CTopologyNode::ParseCriteria(pCriteria, Criteria, &sError) != NOERROR)
					return pCC->CreateError(sError, pCriteria);

				//	Loop

				CTopologyNode::SCriteriaCtx Ctx;
				CTopologyNode::InitCriteriaCtx(Ctx, Criteria);

				for (i = 0; i < g_pUniverse->GetTopologyNodeCount(); i++)
					{
					CTopologyNode *pNode = g_pUniverse->GetTopologyNode(i);
					if (pNode->IsEndGame())
						continue;

					if (!pNode->MatchesCriteria(Ctx, Criteria))
						continue;

					pResult->AppendString(*pCC, pNode->GetID());
					}
				}

			return pResult;
			}

		default:
			ASSERT(false);
		}

	return pCC->CreateNil();
	}

ICCItem *fnSystemGetObjectByName (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnSystemGetObjectByName
//
//	(sysGetObjectByName [source] name) -> obj

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Get the system

	CSystem *pSystem = g_pUniverse->GetCurrentSystem();
	if (pSystem == NULL)
		return StdErrorNoSystem(*pCC);

	//	The first argument is ignored (in previous versions we used that to
	//	get the system).

	CString sName;
	if (pArgs->GetCount() == 1)
		sName = pArgs->GetElement(0)->GetStringValue();
	else
		sName = pArgs->GetElement(1)->GetStringValue();

	//	Find the object

	CSpaceObject *pObj = pSystem->GetNamedObject(sName);
	if (pObj == NULL)
		return pCC->CreateNil();
	else
		return pCC->CreateInteger((int)pObj);
	}

ICCItem *fnSystemMisc (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnSystemMisc
//
//	(sysStartTime)
//	(sysStopTime duration except)

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	int i;

	//	Get the current system

	CSystem *pSystem = g_pUniverse->GetCurrentSystem();
	if (pSystem == NULL)
		return StdErrorNoSystem(*pCC);

	//	Do it

	switch (dwData)
		{
		case FN_SYS_START_TIME:
			{
			if (!pSystem->IsTimeStopped())
				return pCC->CreateNil();

			pSystem->RestartTime();
			return pCC->CreateTrue();
			}

		case FN_SYS_STOP_TIME:
			{
			//	If the first argument is a list, then we have a target list

			if (pArgs->GetElement(0)->IsList())
				{
				CSpaceObjectList TargetList;
				for (i = 0; i < pArgs->GetElement(0)->GetCount(); i++)
					{
					CSpaceObject *pObj = CreateObjFromItem(*pCC, pArgs->GetElement(0)->GetElement(i));
					if (pObj)
						TargetList.Add(pObj);
					}

				if (TargetList.GetCount() == 0)
					return pCC->CreateNil();

				int iDuration = (pArgs->GetElement(1)->IsNil() ? -1 : pArgs->GetElement(1)->GetIntegerValue());

				pSystem->StopTime(TargetList, iDuration);
				return pCC->CreateTrue();
				}

			//	Otherwise, we have a duration and exception

			else
				{
				int iDuration = (pArgs->GetElement(0)->IsNil() ? -1 : pArgs->GetElement(0)->GetIntegerValue());
				CSpaceObject *pExcept = CreateObjFromItem(*pCC, pArgs->GetElement(1));
				if (pExcept == NULL)
					return pCC->CreateNil();

				//	Compute

				pSystem->StopTimeForAll(iDuration, pExcept);
				return pCC->CreateTrue();
				}
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

ICCItem *fnSystemOrbit (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnSystemOrbit
//
//	(sysOrbitPos orbit [options]) -> vector

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Get the orbit

	COrbit OrbitDesc;
	if (!CreateOrbitFromList(*pCC, pArgs->GetElement(0), &OrbitDesc))
		return pCC->CreateError(CONSTLIT("Invalid orbit object"), pArgs->GetElement(0));

	//	Process

	switch (dwData)
		{
		case FN_SYS_ORBIT_POS:
			{
			//	Get options, if we have any

			ICCItem *pOptions = (pArgs->GetCount() > 1 ? pArgs->GetElement(1) : NULL);

			//	Optimize case where we have no options.

			if (pOptions == NULL)
				return CreateListFromVector(*pCC, OrbitDesc.GetObjectPos());

			//	Get options

			ICCItem *pAngleOffset = (pOptions ? pOptions->GetElement(FIELD_ANGLE_OFFSET) : NULL);
			ICCItem *pArcOffset = (pOptions ? pOptions->GetElement(FIELD_ARC_OFFSET) : NULL);
			ICCItem *pRadiusOffset = (pOptions ? pOptions->GetElement(FIELD_RADIUS_OFFSET) : NULL);

			//	Compute an angle offset and a radius offset

			Metric rRadiusOffset = 0.0;
			Metric rAngleOffset = 0.0;

			if (pRadiusOffset)
				rRadiusOffset = CalcRandomMetric(*pCC, pRadiusOffset);

			if (pAngleOffset)
				rAngleOffset += mathDegreesToRadians(pAngleOffset->GetIntegerValue());

			if (pArcOffset)
				{
				double rArcOffset = CalcRandomMetric(*pCC, pArcOffset);

				//	Convert from linear to angular. NOTE: We use a circle 
				//	instead of an ellipse because we're lazy.

				Metric rCirc = (OrbitDesc.GetSemiMajorAxis() + rRadiusOffset);
				rAngleOffset += (rCirc > 0.0 ? rArcOffset / rCirc : 0.0);
				}

			//	Adjust the orbit

			COrbit NewOrbitDesc(OrbitDesc.GetFocus(),
					Max(1.0, OrbitDesc.GetSemiMajorAxis() + rRadiusOffset),
					OrbitDesc.GetEccentricity(),
					OrbitDesc.GetRotation(),
					OrbitDesc.GetObjectAngle() + rAngleOffset);

			//	Done

			return CreateListFromVector(*pCC, NewOrbitDesc.GetObjectPos());
			}

		default:
			ASSERT(false);
			return NULL;
		}
	}

ICCItem *fnSystemVectorMath (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnSystemVectorMath
//
//	(sysVectorAdd vector vector) -> vector
//	(sysVectorDivide vector scalar) -> vector
//	(sysVectorMultiply vector scalar) -> vector
//	(sysVectorSubtract vector vector) -> vector

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	switch (dwData)
		{
		case FN_VECTOR_ADD:
			{
			CVector vPos1;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(0), &vPos1) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(0));

			CVector vPos2;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vPos2) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));

			return CreateListFromVector(*pCC, vPos1 + vPos2);
			}

		case FN_VECTOR_ANGLE:
			{
			CVector vVec1;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(0), &vVec1) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(0));

			//	If we have two vectors, then we return the bearing of vector 1
			//	as seen from vector 2.

			if (pArgs->GetCount() > 1)
				{
				CVector vVec2;
				if (GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vVec2) != NOERROR)
					return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));

				return pCC->CreateInteger(VectorToPolar(vVec1 - vVec2));
				}

			//	Otherwise, we just return the angle of vector 1
			else
				return pCC->CreateInteger(VectorToPolar(vVec1));
			}

		case FN_VECTOR_DIVIDE:
			{
			CVector vPos1;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(0), &vPos1) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(0));

			Metric rFactor = pArgs->GetElement(1)->GetDoubleValue();

			if (rFactor == 0.0)
				return pCC->CreateError(CONSTLIT("division by zero"), NULL);

			return CreateListFromVector(*pCC, vPos1 / rFactor);
			}

		case FN_VECTOR_MULTIPLY:
			{
			CVector vPos1;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(0), &vPos1) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(0));

			Metric rFactor = pArgs->GetElement(1)->GetDoubleValue();

			return CreateListFromVector(*pCC, vPos1 * rFactor);
			}

		case FN_VECTOR_PIXEL_OFFSET:
			{
			//	Get the center

			CVector vCenter;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(0), &vCenter) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(0));

			//	Get the offset

			Metric xOffset = pArgs->GetElement(1)->GetDoubleValue();
			Metric yOffset = pArgs->GetElement(2)->GetDoubleValue();

			//	Return the vector

			return CreateListFromVector(*pCC, vCenter + CVector(xOffset * g_KlicksPerPixel, yOffset * g_KlicksPerPixel));
			}

		case FN_VECTOR_RANDOM:
			{
			CSpaceObject *pSource = NULL;
			CSystem *pSystem = g_pUniverse->GetCurrentSystem();
			if (pSystem == NULL)
				return StdErrorNoSystem(*pCC);

			//	Get the center

			CVector vCenter;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(0), &vCenter, &pSource) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(0));

			//	Get the radius and separation

			ICCItem *pRadiusFunc = NULL;
			Metric rRadius = 0.0;
			if (pArgs->GetElement(1)->IsFunction())
				pRadiusFunc = pArgs->GetElement(1);
			else
				rRadius = LIGHT_SECOND * pArgs->GetElement(1)->GetDoubleValue();

			Metric rSeparation = 0.0;
			if (pArgs->GetCount() > 2)
				rSeparation = LIGHT_SECOND * pArgs->GetElement(2)->GetDoubleValue();

			//	Get the filter

			if (pSource == NULL)
				pSource = pSystem->GetPlayerShip();
			if (pSource == NULL)
				pSource = g_pUniverse->GetPOV();

			CString sFilter = CONSTLIT("st");
			if (pArgs->GetCount() > 3)
				sFilter = pArgs->GetElement(3)->GetStringValue();

			CSpaceObjectCriteria Criteria(pSource, sFilter);

			//	Keep trying random positions until we find something that works
			//	(or until we run out of tries)

			int iTries = 100;
			while (iTries > 0)
				{
				CVector vOffset;

				//	If the radius is a function, compute it now

				if (pRadiusFunc)
					{
					ICCItem *pResult = pRadiusFunc->Execute(pEvalCtx, pCC->CreateNil());

					//	Function can return either a radius or a vector

					if (pResult->IsList())
						vOffset = CreateVectorFromList(*pCC, pResult);
					else
						vOffset = PolarToVector(mathRandom(0, 359), LIGHT_SECOND * pResult->GetDoubleValue());

					pResult->Discard(pCC);
					}
				else
					vOffset = PolarToVector(mathRandom(0, 359), rRadius);

				CVector vTry = vCenter + vOffset;

				//	See if any object is within the separation range. If there 
				//	is, then we continue.

				if (pSystem->FindObjectInRange(vTry, rSeparation, Criteria)
						&& --iTries > 0)
					continue;

				//	Otherwise, use the vector

				return CreateListFromVector(*pCC, vTry);
				}
			}

		case FN_VECTOR_SUBTRACT:
			{
			CVector vPos1;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(0), &vPos1) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(0));

			CVector vPos2;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vPos2) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));

			return CreateListFromVector(*pCC, vPos1 - vPos2);
			}

		case FN_VECTOR_DISTANCE:
			{
			CVector vPos1;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(0), &vPos1) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(0));

			CVector vPos2;
			if (pArgs->GetCount() > 1)
				{
				if (GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vPos2) != NOERROR)
					return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));
				}

			CVector vDist = vPos1 - vPos2;
			return pCC->CreateInteger((int)((vDist.Length() / LIGHT_SECOND) + 0.5));
			}

		case FN_VECTOR_DISTANCE_EXACT:
			{
			CVector vPos1;
			if (GetPosOrObject(pEvalCtx, pArgs->GetElement(0), &vPos1) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(0));

			CVector vPos2;
			if (pArgs->GetCount() > 1)
				{
				if (GetPosOrObject(pEvalCtx, pArgs->GetElement(1), &vPos2) != NOERROR)
					return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(1));
				}

			CVector vDist = vPos1 - vPos2;
			return pCC->CreateDouble(vDist.Length() / LIGHT_SECOND);
			}

		case FN_VECTOR_POLAR_VELOCITY:
			{
			Metric rAngle = pArgs->GetElement(0)->GetDoubleValue();
			Metric rSpeed = (Metric)(LIGHT_SPEED / 100.0) * pArgs->GetElement(1)->GetDoubleValue();

			return CreateListFromVector(*pCC, PolarToVector((int)rAngle, rSpeed));
			}

		case FN_VECTOR_SPEED:
			{
			CVector vPos(CreateVectorFromList(*pCC, pArgs->GetElement(0)));
			return pCC->CreateInteger((int)(vPos.Length() * (100.0 / LIGHT_SPEED) + 0.5));
			}

		default:
			ASSERT(false);
			return NULL;
		}
	}

ICCItem *fnSystemVectorOffset (CEvalContext *pEvalCtx, ICCItem *pArguments, DWORD dwData)

//	fnSystemVectorOffset
//
//	(sysVectorPolarOffset obj angle radius) -> vector

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	ICCItem *pArgs;

	//	Evaluate the arguments and validate them

	pArgs = pCC->EvaluateArgs(pEvalCtx, pArguments, CONSTLIT("vnn"));
	if (pArgs->IsError())
		return pArgs;

	//	Get the arguments

	CVector vCenter;
	if (GetPosOrObject(pEvalCtx, pArgs->GetElement(0), &vCenter) != NOERROR)
		return pCC->CreateError(CONSTLIT("Invalid pos"), pArgs->GetElement(0));

	Metric rAngle = pArgs->GetElement(1)->GetDoubleValue();
	Metric rRadius = LIGHT_SECOND * pArgs->GetElement(2)->GetDoubleValue();

	pArgs->Discard(pCC);

	//	Compute

	CVector vVec = vCenter + PolarToVector((int)rAngle, rRadius);

	//	Done

	return CreateListFromVector(*pCC, vVec);
	}

ICCItem *fnTopologyGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnTopologyGet

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	//	Ignore nil. (We fail silently because sometimes our input comes from a
	//	different function, which might have failed. Thus this is a runtime
	//	situation, not a programmer error.)

	if (pArgs->GetElement(0)->IsNil())
		return pCC->CreateNil();

	//	First arg is topology node

	CTopologyNode *pNode = g_pUniverse->FindTopologyNode(pArgs->GetElement(0)->GetStringValue());
	if (pNode == NULL)
		return pCC->CreateError(CONSTLIT("Invalid nodeID"), pArgs->GetElement(0));

	switch (dwData)
		{
		case FN_NODE_HAS_ATTRIBUTE:
			return pCC->CreateBool(pNode->HasAttribute(pArgs->GetElement(1)->GetStringValue()));

		case FN_NODE_LEVEL:
			return pCC->CreateInteger(pNode->GetLevel());

		case FN_NODE_SYSTEM_NAME:
			return pCC->CreateString(pNode->GetSystemName());

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

ICCItem *fnUniverseGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnUniverseGet
//
//	Gets data about the universe

	{
	int i;
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	if (pCtx == NULL)
		return pCC->CreateError(ERR_NO_CODE_CHAIN_CTX);

	ICCItem *pResult;

	//	Do the appropriate command

	switch (dwData)
		{
		case FN_UNIVERSE_EXTENSION_UNID:
			{
			CExtension *pExtension = pCtx->GetExtension();
			if (pExtension == NULL)
				return pCC->CreateNil();

			return pCC->CreateInteger(pExtension->GetUNID());
			}

		case FN_UNIVERSE_GET_ELAPSED_GAME_TIME:
			{
			int iArg = 0;
			DWORD dwStartTick = (pArgs->GetCount() > 2 ? pArgs->GetElement(iArg++)->GetIntegerValue() : 0);
			DWORD dwEndTick = pArgs->GetElement(iArg++)->GetIntegerValue();
			CString sFormat = pArgs->GetElement(iArg++)->GetStringValue();

			CTimeSpan Span = g_pUniverse->GetElapsedGameTimeAt(dwEndTick) - g_pUniverse->GetElapsedGameTimeAt(dwStartTick);

			if (strEquals(sFormat, CONSTLIT("display")))
				return pCC->CreateString(Span.Format(NULL_STR));
			else if (strEquals(sFormat, CONSTLIT("seconds")))
				return pCC->CreateInteger(Span.Seconds());
			else
				return pCC->CreateError(strPatternSubst(CONSTLIT("Unknown format: %s"), sFormat));
			}

		case FN_UNIVERSE_FIND_OBJ:
			{
			int iArg = 0;

			//	If we have two args, then load the node ID

			CString sNodeID;
			if (pArgs->GetCount() > 1)
				sNodeID = pArgs->GetElement(iArg++)->GetStringValue();

			//	Parse the criteria

			CObjectTrackerCriteria Criteria;
			if (!Criteria.ParseCriteria(pArgs->GetElement(iArg++)->GetStringValue()))
				return pCC->CreateError(CONSTLIT("Invalid criteria"), pArgs->GetElement(0));

			//	Get the list of entries

			TArray<CObjectTracker::SObjEntry> Result;
			if (!g_pUniverse->GetGlobalObjects().Find(sNodeID, Criteria, &Result))
				return pCC->CreateNil();

			//	Create a list to hold the results

			ICCItem *pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;
			for (i = 0; i < Result.GetCount(); i++)
				{
				//	Create a list for each entry

				ICCItem *pEntry = pCC->CreateLinkedList();
				if (pEntry->IsError())
					{
					pEntry->Discard(pCC);
					continue;
					}

				CCLinkedList *pEntryList = (CCLinkedList *)pEntry;
				pEntryList->AppendInteger(*pCC, Result[i].dwObjID);
				pEntryList->AppendInteger(*pCC, Result[i].pType->GetUNID());
				pEntryList->AppendString(*pCC, Result[i].pNode->GetID());
				pEntryList->AppendString(*pCC, Result[i].sName);
				pEntryList->AppendInteger(*pCC, Result[i].dwNameFlags);

				pList->Append(*pCC, pEntry);
				pEntry->Discard(pCC);
				}

			//	Done

			return pResult;
			}

		case FN_UNIVERSE_GET_EXTENSION_DATA:
			{
			CExtension *pExtension = pCtx->GetExtension();
			if (pExtension == NULL || pExtension->GetUNID() == 0)
				return pCC->CreateError(CONSTLIT("Extension UNID undefined in this context"));

			EStorageScopes iScope = ParseStorageScopeID(pArgs->GetElement(0)->GetStringValue());
			if (iScope == storeUnknown)
				return pCC->CreateError(CONSTLIT("Unknown storage scope"), pArgs->GetElement(0));
			else if (iScope == storeServiceExtension || iScope == storeServiceUser)
				return pCC->CreateError(CONSTLIT("Service scope not yet implemented"), pArgs->GetElement(0));

			CString sData = g_pUniverse->GetExtensionData(iScope, pExtension->GetUNID(), pArgs->GetElement(1)->GetStringValue());
			if (sData.IsBlank())
				return pCC->CreateNil();

			//	Result

			return pCC->Link(sData, 0, NULL);
			}

		case FN_UNIVERSE_ENTITY:
			{
			//	If we have a UNID, then find the matching entity
			//	Otherwise, we have an entity so we find the matching UNID
			CString sName;
			DWORD dwUNID;
			if (pArgs->GetElement(0)->IsInteger())
				{
				dwUNID = pArgs->GetElement(0)->GetIntegerValue();
				sName = g_pUniverse->GetExtensionCollection().GetEntityName(dwUNID);
				if (!sName.IsBlank())
					return pCC->CreateString(sName);
				else
					return pCC->CreateNil();
				}
			else
				{
				sName = pArgs->GetElement(0)->GetStringValue();
				dwUNID = g_pUniverse->GetExtensionCollection().GetEntityValue(sName);
				if (dwUNID)
					return pCC->CreateInteger(dwUNID);
				else
					return pCC->CreateNil();
				}

			}

		case FN_UNIVERSE_REAL_DATE:
			{
			CTimeDate Date(CTimeDate::Today);

			pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				break;

			CCLinkedList *pList = (CCLinkedList *)pResult;

			ICCItem *pValue = pCC->CreateInteger(Date.Year());
			pList->Append(*pCC, pValue);
			pValue->Discard(pCC);

			pValue = pCC->CreateInteger(Date.Month());
			pList->Append(*pCC, pValue);
			pValue->Discard(pCC);

			pValue = pCC->CreateInteger(Date.Day());
			pList->Append(*pCC, pValue);
			pValue->Discard(pCC);
			break;
			}

		case FN_UNIVERSE_SET_EXTENSION_DATA:
			{
			CExtension *pExtension = pCtx->GetExtension();
			if (pExtension == NULL || pExtension->GetUNID() == 0)
				return pCC->CreateError(CONSTLIT("Extension UNID undefined in this context"));

			EStorageScopes iScope = ParseStorageScopeID(pArgs->GetElement(0)->GetStringValue());
			if (iScope == storeUnknown)
				return pCC->CreateError(CONSTLIT("Unknown storage scope"), pArgs->GetElement(0));
			else if (iScope == storeServiceExtension || iScope == storeServiceUser)
				return pCC->CreateError(CONSTLIT("Service scope not yet implemented"), pArgs->GetElement(0));

			if (!g_pUniverse->SetExtensionData(iScope, pExtension->GetUNID(), pArgs->GetElement(1)->GetStringValue(), CreateDataFromItem(*pCC, pArgs->GetElement(2))))
				return pCC->CreateError(CONSTLIT("Unable to store data"), pArgs->GetElement(1));

			//	Result

			return pCC->CreateTrue();
			}

		case FN_UNIVERSE_SET_OBJECT_KNOWN:
			{
			//	If the last argument is True or Nil, then we know what the 
			//	others are.

			CString sNodeID;
			CString sCriteria;
			bool bKnown;

			ICCItem *pLastArg = pArgs->GetElement(pArgs->GetCount() - 1);
			if (pLastArg->IsNil() || pLastArg->IsTrue())
				{
				if (pArgs->GetCount() >= 3)
					{
					sNodeID = pArgs->GetElement(0)->GetStringValue();
					sCriteria = pArgs->GetElement(1)->GetStringValue();
					}
				else
					{
					sNodeID = NULL_STR;
					sCriteria = pArgs->GetElement(0)->GetStringValue();
					}

				bKnown = !pLastArg->IsNil();
				}

			//	Otherwise, we either have nodeID + criteria or just criteria

			else if (pArgs->GetCount() >= 2)
				{
				sNodeID = pArgs->GetElement(0)->GetStringValue();
				sCriteria = pArgs->GetElement(1)->GetStringValue();
				bKnown = true;
				}
			else
				{
				sNodeID = NULL_STR;
				sCriteria = pArgs->GetElement(0)->GetStringValue();
				bKnown = true;
				}

			//	Parse the criteria

			CDesignTypeCriteria Criteria;
			if (CDesignTypeCriteria::ParseCriteria(sCriteria, &Criteria) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid design type criteria"), pArgs->GetElement(0));

			//	Do it

			g_pUniverse->GetGlobalObjects().SetKnown(sNodeID, Criteria, bKnown);

			//	Done

			return pCC->CreateTrue();
			}

		case FN_UNIVERSE_TICK:
			pResult = pCC->CreateInteger(g_pUniverse->GetTicks());
			break;

		case FN_UNIVERSE_UNID:
			{
			CString sName = pArgs->GetElement(0)->GetStringValue();

			//	Figure out what the name refers to

			bool bFound = false;
			CItemType *pFoundItem = NULL;
			CShipClass *pFoundShip = NULL;

			//	Look for an exact match with an item name

			if (!bFound)
				{
				for (i = 0; i < g_pUniverse->GetItemTypeCount(); i++)
					{
					CItemType *pType = g_pUniverse->GetItemType(i);
					if (!pType->IsVirtual() 
							&& strEquals(sName, pType->GetNounPhrase(nounActual)))
						{
						bFound = true;
						pFoundItem = pType;
						break;
						}
					}
				}

			//	Look for a ship class match

			if (!bFound)
				{
				pFoundShip = g_pUniverse->FindShipClassByName(sName);
				bFound = (pFoundShip != NULL);
				}

			//	Look for partial item name match

			if (!bFound)
				{
				CItemCriteria Crit;
				CItem::InitCriteriaAll(&Crit);
				CItem Item = CItem::CreateItemByName(sName, Crit, true);
				if (Item.GetType())
					{
					bFound = true;
					pFoundItem = Item.GetType();
					}
				}

			//	If we found something, generate a list 

			if (bFound)
				{
				//	Create the result list

				pResult = pCC->CreateLinkedList();
				if (pResult->IsError())
					return pResult;

				CCLinkedList *pList = (CCLinkedList *)pResult;

				//	Figure out the elements of the list

				CString sType;
				DWORD dwUNID;
				CString sName;

				if (pFoundItem)
					{
					sType = UNID_TYPE_ITEM_TYPE;
					dwUNID = pFoundItem->GetUNID();
					sName = pFoundItem->GetNounPhrase(nounActual);
					}
				else if (pFoundShip)
					{
					sType = UNID_TYPE_SHIP_CLASS;
					dwUNID = pFoundShip->GetUNID();
					sName = pFoundShip->GetNounPhrase();
					}
				else
					ASSERT(false);

				ICCItem *pValue = pCC->CreateInteger(dwUNID);
				pList->Append(*pCC, pValue);
				pValue->Discard(pCC);

				pValue = pCC->CreateString(sType);
				pList->Append(*pCC, pValue);
				pValue->Discard(pCC);

				pValue = pCC->CreateString(sName);
				pList->Append(*pCC, pValue);
				pValue->Discard(pCC);
				}
			else
				pResult = pCC->CreateNil();

			break;
			}

		default:
			ASSERT(FALSE);
		}

	return pResult;
	}

ICCItem *fnXMLCreate (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)
	{
	CCodeChain *pCC = pEvalCtx->pCC;
	ICCItem *pValue = pArgs->GetElement(0);

	//	If we have an XML element, then we create a clone of it.

	if (strEquals(pValue->GetTypeOf(), CONSTLIT("xmlElement")))
		{
		CCXMLWrapper *pWrapper = CreateXMLElementFromItem(*pCC, pValue);
		if (pWrapper == NULL)
			return pCC->CreateError(CONSTLIT("Invalid XML element"), pArgs->GetElement(0));

		return new CCXMLWrapper(pWrapper->GetXMLElement());
		}

	//	Otherwise, we expect XML text

	else if (pValue->IsIdentifier())
		{
		CString sValue = pValue->GetStringValue();
		char *pPos = sValue.GetASCIIZPointer();
		if (*pPos != '<')
			return pCC->CreateError(CONSTLIT("Invalid XML"), pValue);

		//	Parse it

		CString sError;
		CBufferReadBlock Stream(sValue);
		CXMLElement *pElement;
		if (CXMLElement::ParseSingleElement(&Stream, NULL, &pElement, &sError) != NOERROR)
			return pCC->CreateError(sError, pValue);

		ICCItem *pResult = new CCXMLWrapper(pElement);
		delete pElement;
		return pResult;
		}

	//	Otherwise, we don't know what

	else
		return pCC->CreateError(CONSTLIT("Invalid XML"), pValue);
	}

ICCItem *fnXMLGet (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)
	{
	CCodeChain *pCC = pEvalCtx->pCC;
	int i;

	//	Get the XML element

	CCXMLWrapper *pWrapper = CreateXMLElementFromItem(*pCC, pArgs->GetElement(0));
	if (pWrapper == NULL)
		return pCC->CreateError(CONSTLIT("Invalid XML element"), pArgs->GetElement(0));

	CXMLElement *pXML = pWrapper->GetXMLElement();

	//	Function

	switch (dwData)
		{
		case FN_XML_APPEND_SUB_ELEMENT:
			{
			CCXMLWrapper *pWrapper = CreateXMLElementFromItem(*pCC, pArgs->GetElement(1));
			if (pWrapper == NULL)
				return pCC->CreateError(CONSTLIT("Invalid XML element"), pArgs->GetElement(1));

			CXMLElement *pSubElement = pWrapper->GetXMLElement()->OrphanCopy();
			int iIndex = (pArgs->GetCount() >= 3 ? pArgs->GetElement(2)->GetIntegerValue() : -1);

			//	Sanity check the index

			if (iIndex < 0 || iIndex > pXML->GetContentElementCount())
				iIndex = -1;

			//	Append

			if (pXML->AppendSubElement(pSubElement, iIndex) != NOERROR)
				{
				delete pSubElement;
				return pCC->CreateError(CONSTLIT("Invalid index"), pArgs->GetElement(2));
				}

			return pCC->CreateTrue();
			}

		case FN_XML_APPEND_TEXT:
			{
			CString sText = pArgs->GetElement(1)->GetStringValue();
			int iIndex = (pArgs->GetCount() >= 3 ? pArgs->GetElement(2)->GetIntegerValue() : -1);

			if (pXML->AppendContent(sText, iIndex) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid index"), pArgs->GetElement(2));

			return pCC->CreateTrue();
			}

		case FN_XML_DELETE_SUB_ELEMENT:
			{
			int iIndex = pArgs->GetElement(1)->GetIntegerValue();
			if (iIndex < 0 || iIndex >= pXML->GetContentElementCount())
				return pCC->CreateNil();

			pXML->DeleteSubElement(iIndex);
			return pCC->CreateTrue();
			}

		case FN_XML_GET_ATTRIB:
			{
			CString sValue;
			if (!pXML->FindAttribute(pArgs->GetElement(1)->GetStringValue(), &sValue))
				return pCC->CreateNil();

			//	We return either a number or a string, as appropriate.

			return pCC->CreateVariant(sValue);
			}

		case FN_XML_GET_ATTRIB_LIST:
			{
			if (pXML->GetAttributeCount() == 0)
				return pCC->CreateNil();

			ICCItem *pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;
			for (i = 0; i < pXML->GetAttributeCount(); i++)
				pList->AppendString(*pCC, pXML->GetAttributeName(i));

			return pResult;
			}

		case FN_XML_GET_SUB_ELEMENT:
			{
			ICCItem *pIndex = pArgs->GetElement(1);

			//	If the index is an integer, then we get the nth sub element

			if (pIndex->IsInteger())
				{
				int iIndex = pIndex->GetIntegerValue();
				if (iIndex < 0 || iIndex >= pXML->GetContentElementCount())
					return pCC->CreateNil();

				return new CCXMLWrapper(pXML->GetContentElement(iIndex), pWrapper);
				}

			//	Otherwise we expect a tag

			else
				{
				CXMLElement *pSub = pXML->GetContentElementByTag(pIndex->GetStringValue());
				if (pSub == NULL)
					return pCC->CreateNil();

				return new CCXMLWrapper(pSub, pWrapper);
				}
			}

		case FN_XML_GET_SUB_ELEMENT_COUNT:
			return pCC->CreateInteger(pXML->GetContentElementCount());

		case FN_XML_GET_SUB_ELEMENT_LIST:
			{
			ICCItem *pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;

			//	If we have a tag, we only return elements with the given tag

			if (pArgs->GetCount() >= 2)
				{
				CString sTag = pArgs->GetElement(1)->GetStringValue();

				for (i = 0; i < pXML->GetContentElementCount(); i++)
					{
					CXMLElement *pSub = pXML->GetContentElement(i);
					if (strEquals(pSub->GetTag(), sTag))
						{
						CCXMLWrapper *pNewItem = new CCXMLWrapper(pSub, pWrapper);
						pList->Append(*pCC, pNewItem);
						pNewItem->Discard(pCC);
						}
					}
				}

			//	Otherwise, we return all elements

			else
				{
				for (i = 0; i < pXML->GetContentElementCount(); i++)
					{
					CXMLElement *pSub = pXML->GetContentElement(i);
					CCXMLWrapper *pNewItem = new CCXMLWrapper(pSub, pWrapper);
					pList->Append(*pCC, pNewItem);
					pNewItem->Discard(pCC);
					}
				}

			//	Optimize case with no items

			if (pList->GetCount() == 0)
				{
				pResult->Discard(pCC);
				return pCC->CreateNil();
				}

			//	Done

			return pResult;
			}
			
		case FN_XML_GET_TEXT:
			{
			int iIndex = pArgs->GetElement(1)->GetIntegerValue();
			return pCC->CreateString(pXML->GetContentText(iIndex));
			}

		case FN_XML_GET_TAG:
			return pCC->CreateString(pXML->GetTag());

		case FN_XML_SET_ATTRIB:
			{
			CString sAttrib = pArgs->GetElement(1)->GetStringValue();
			CString sValue = pArgs->GetElement(2)->GetStringValue();
			pXML->SetAttribute(sAttrib, sValue);
			return pCC->CreateString(sValue);
			}

		case FN_XML_SET_TEXT:
			{
			CString sText = pArgs->GetElement(1)->GetStringValue();
			int iIndex = (pArgs->GetCount() >= 3 ? pArgs->GetElement(2)->GetIntegerValue() : -1);

			if (pXML->SetContentText(sText, iIndex) != NOERROR)
				return pCC->CreateError(CONSTLIT("Invalid index"), pArgs->GetElement(2));

			return pCC->CreateTrue();
			}

		default:
			ASSERT(false);
		}

	return pCC->CreateNil();
	}
