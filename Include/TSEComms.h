//	TSEComms.h
//
//	Defines classes and interfaces for communications handling
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	DO NOT CHANGE NUMBERS
//	NOTE: These numbers must match-up with CSovereign.cpp and with the
//	definitions in the <Sovereign> elements. And with Utilities.cpp

enum MessageTypes
	{
	msgNone =						0,	//	None
	msgAttack =						1,	//	Order to attack (pParam1 is target)
	msgDestroyBroadcast =			2,	//	Broadcast order to destroy target (pParam1 is target)
	msgHitByFriendlyFire =			3,	//	OBSOLETE: Hit by friendly fire (pParam1 is attacker)
	msgQueryEscortStatus =			4,	//	Replies with Ack if is escorting pParam1
	msgQueryFleetStatus =			5,	//	Replies with Ack if is in formation with pParam1
	msgEscortAttacked =				6,	//	pParam1 attacked escort ship (or defender)
	msgEscortReportingIn =			7,	//	pParam1 is now escorting the ship
	msgWatchTargets =				8,	//	"watch your targets!"
	msgNiceShooting =				9,	//	"nice shooting!"
	msgFormUp =						10,	//	Enter formation (dwParam2 is formation place)
	msgBreakAndAttack =				11,	//	Break formation and attack
	msgQueryCommunications =		12,	//	Returns comms capabilities
	msgAbort =						13,	//	Cancel attack order
	msgWait =						14,	//	Wait there for a while
	msgQueryWaitStatus =			15,	//	Replies with Ack if waiting
	msgAttackInFormation =			16,	//	Fire weapon while staying in formation
	msgAttackDeter =				17, //	Attack target, but return after a while (pParam1 is target)
	msgQueryAttackStatus =			18, //	Replies with Ack if attacking a target
	msgDockingSequenceEngaged =		19,	//	Sent by a station to anyone docking
	msgHitByHostileFire =			20,	//	OBSOLETE: Hit by hostile fire (pParam1 is attacker)
	msgDestroyedByFriendlyFire =	21,	//	OBSOLETE: We were destroyed by friendly fire (pParam1 is attacker)
	msgDestroyedByHostileFire =		22,	//	OBSOLETE: We were destroyed by hostile fire (pParam is attacker)
	msgBaseDestroyedByTarget =		23,	//	Our base was destroyed by target (pParam is target)
	};

enum ResponseTypes
	{
	resNoAnswer =					0,
	resAck =						1,

	resCanBeInFormation =			0x0010,
	resCanAttack =					0x0020,
	resCanBreakAndAttack =			0x0040,
	resCanAbortAttack =				0x0080,
	resCanWait =					0x0100,
	resCanFormUp =					0x0200,
	resCanAttackInFormation =		0x0400,
	};

class CCommunicationsHandler
	{
	public:
		struct SMessage
			{
			CString sID;
			CString sMessage;
			CString sShortcut;

			SEventHandlerDesc InvokeEvent;
			SEventHandlerDesc OnShowEvent;
			};

		CCommunicationsHandler (void);
		~CCommunicationsHandler (void);

		CCommunicationsHandler &operator= (const CCommunicationsHandler &Src);
		ALERROR InitFromXML (CXMLElement *pDesc, CString *retsError);

		void DeleteAll (void);
		bool FindMessage (const CString &sID, const SMessage **retpMessage = NULL) const;
		int FindMessageByID (const CString &sID) const;
		int FindMessageByName (const CString &sMessage) const;
		void FireInvoke (const CString &sID, CSpaceObject *pObj, CSovereign *pSender, ICCItem *pData);
		inline int GetCount (void) const { return m_Messages.GetCount(); }
		inline const SMessage &GetMessage (int iIndex) { return m_Messages[iIndex]; }
		void Merge (CCommunicationsHandler &New);

	private:
		bool FindMergePos (const SMessage &Msg, int *retiPos);

		TArray<SMessage> m_Messages;
	};

class CCommunicationsStack
	{
	public:
		CCommunicationsStack (void);

		CCommunicationsHandler *GetCommsHandler (CDesignType *pParent = NULL);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void Merge (CCommunicationsStack &Src);
		void Unbind (void);

	private:
		CCommunicationsHandler m_CommsHandler;
		CCommunicationsHandler m_Composite;
		bool m_bInitialized;
	};