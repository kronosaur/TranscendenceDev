//	CDeviceTable.cpp
//
//	IDeviceGenerator objects

#include "PreComp.h"

#define DEVICE_TAG								CONSTLIT("Device")
#define DEVICES_TAG								CONSTLIT("Devices")
#define DEVICE_SLOT_TAG							CONSTLIT("DeviceSlot")
#define DEVICE_SLOTS_TAG						CONSTLIT("DeviceSlots")
#define ENHANCE_ABILITIES_TAG					CONSTLIT("EnhancementAbilities")
#define GROUP_TAG								CONSTLIT("Group")
#define ITEM_TAG								CONSTLIT("Item")
#define ITEMS_TAG								CONSTLIT("Items")
#define LEVEL_TABLE_TAG							CONSTLIT("LevelTable")
#define NULL_TAG								CONSTLIT("Null")
#define TABLE_TAG								CONSTLIT("Table")

#define CANNOT_BE_EMPTY_ATTRIB					CONSTLIT("cannotBeEmpty")
#define CATEGORIES_ATTRIB						CONSTLIT("categories")
#define CHANCE_ATTRIB							CONSTLIT("chance")
#define CHARGES_ATTRIB							CONSTLIT("charges")
#define COUNT_ATTRIB							CONSTLIT("count")
#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define DAMAGED_ATTRIB							CONSTLIT("damaged")
#define DEVICE_ID_ATTRIB						CONSTLIT("deviceID")
#define ENHANCED_ATTRIB							CONSTLIT("enhanced")
#define ENHANCEMENT_ATTRIB						CONSTLIT("enhancement")
#define EXTERNAL_ATTRIB							CONSTLIT("external")
#define FATE_ATTRIB								CONSTLIT("fate")
#define FIRE_ANGLE_ATTRIB						CONSTLIT("fireAngle")
#define FIRE_ARC_ATTRIB							CONSTLIT("fireArc")
#define HP_BONUS_ATTRIB							CONSTLIT("hpBonus")
#define ID_ATTRIB								CONSTLIT("id")
#define ITEM_ATTRIB								CONSTLIT("item")
#define LEVEL_ATTRIB							CONSTLIT("level")
#define LEVEL_CURVE_ATTRIB						CONSTLIT("levelCurve")
#define LEVEL_FREQUENCY_ATTRIB					CONSTLIT("levelFrequency")
#define LINKED_FIRE_ATTRIB						CONSTLIT("linkedFire")
#define MAX_COUNT_ATTRIB						CONSTLIT("maxCount")
#define MAX_FIRE_ARC_ATTRIB						CONSTLIT("maxFireArc")
#define MIN_FIRE_ARC_ATTRIB						CONSTLIT("minFireArc")
#define OMNIDIRECTIONAL_ATTRIB					CONSTLIT("omnidirectional")
#define SECONDARY_WEAPON_ATTRIB					CONSTLIT("secondaryWeapon")
#define SHOT_SEPARATION_SCALE_ATTRIB			CONSTLIT("shotSeparationScale")
#define SLOT_ID_ATTRIB							CONSTLIT("slotID")
#define TABLE_ATTRIB							CONSTLIT("table")
#define UNID_ATTRIB								CONSTLIT("unid")

class CNullDevice : public IDeviceGenerator
	{
	public:
		virtual bool IsVariant (void) const override { return false; }
	};

class CSingleDevice : public IDeviceGenerator
	{
	public:
		~CSingleDevice (void);

		virtual void AddDevices (SDeviceGenerateCtx &Ctx) override;
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR FinishBind (SDesignLoadCtx &Ctx) override;
		virtual bool HasItemAttribute (const CString &sAttrib) const override;
		virtual bool IsVariant (void) const override;
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

	private:
		bool FindSlot (SDeviceGenerateCtx &Ctx, const CItem &Item, SDeviceDesc &retSlotDesc) const;

		//	Item creation parameters

		CItemTypeRef m_pItemType;				//	Device for this slot
		DiceRange m_Count;						//	Number of slots to create
        DiceRange m_Level;						//  For scalable items

		int m_iDamaged = 0;						//	Chance device is damaged
		CRandomEnhancementGenerator m_Enhanced;	//	Procedure for enhancing device item
		IItemGenerator *m_pExtraItems = NULL;	//	Extra items to add when device is added
		int m_iCharges = 0;						//	Set charges on device

		//	Slot properties

		CString m_sSlotID;						//	Place in the given slot ID (may be blank)

		int m_iPosAngle = 0;					//	Slot position
		int m_iPosRadius = 0;
		int m_iPosZ = 0;
		bool m_b3DPosition = false;				//	Backwards compatibility
		bool m_bDefaultPos = false;				//	This slot does not define a position

		bool m_bOmnidirectional = false;		//	This slot has a turret
		int m_iMinFireArc = 0;					//	This slot swivels
		int m_iMaxFireArc = 0;
		bool m_bDefaultFireArc = false;			//	This slot does not define swivel

		DWORD m_dwLinkedFireOptions = 0;		//	This slot has linked-fire properties
		bool m_bDefaultLinkedFire = false;		//	This slot does not define linked-fire
		bool m_bSecondary = false;				//	Secondary weapon (for AI)

		bool m_bExternal = false;				//	This slot is external
		bool m_bCannotBeEmpty = false;			//	This slot cannot be empty
		ItemFates m_iFate = fateNone;			//	What happens to item when ship is destroyed

		CEnhancementDesc m_Enhancements;		//	This slot enhances the installed device
		int m_iSlotBonus = 0;					//	HP bonus to devices in this slot
		bool m_bDefaultSlotBonus = false;		//	This slot does not define a bonus
		double m_rShotSeparationScale = 1.;		//	Governs scaling of shot separation for dual etc weapons

	};

class CLevelTableOfDeviceGenerators : public IDeviceGenerator
	{
	public:
		virtual ~CLevelTableOfDeviceGenerators (void);
		virtual void AddDevices (SDeviceGenerateCtx &Ctx) override;
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR FinishBind (SDesignLoadCtx &Ctx) override;
		virtual IDeviceGenerator *GetGenerator (int iIndex) override { return m_Table[iIndex].pDevice; }
		virtual int GetGeneratorCount (void) override { return m_Table.GetCount(); }
		virtual bool HasItemAttribute (const CString &sAttrib) const override;
		virtual bool IsVariant (void) const override;
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

	private:
		struct SEntry
			{
			IDeviceGenerator *pDevice;
			CString sLevelFrequency;
			int iChance;
			};

		DiceRange m_Count;
		TArray<SEntry> m_Table;
		int m_iComputedLevel;
		int m_iTotalChance;
	};

class CTableOfDeviceGenerators : public IDeviceGenerator
	{
	public:
		virtual ~CTableOfDeviceGenerators (void);
		virtual void AddDevices (SDeviceGenerateCtx &Ctx) override;
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR FinishBind (SDesignLoadCtx &Ctx) override;
		virtual IDeviceGenerator *GetGenerator (int iIndex) override { return m_Table[iIndex].pDevice; }
		virtual int GetGeneratorCount (void) override { return m_Table.GetCount(); }
		virtual bool HasItemAttribute (const CString &sAttrib) const override;
		virtual bool IsVariant (void) const override;
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

	private:
		struct SEntry
			{
			IDeviceGenerator *pDevice;
			int iChance;
			};

		DiceRange m_Count;
		TArray<SEntry> m_Table;
		int m_iTotalChance;
	};

class CGroupOfDeviceGenerators : public IDeviceGenerator
	{
	public:
		virtual ~CGroupOfDeviceGenerators (void);
		virtual void AddDevices (SDeviceGenerateCtx &Ctx) override;
		virtual void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual Metric CalcHullPoints (void) const override;
		virtual ALERROR FinishBind (SDesignLoadCtx &Ctx) override;
		virtual IDeviceGenerator *GetGenerator (int iIndex) override { return m_Table[iIndex].pDevice; }
		virtual int GetGeneratorCount (void) override { return m_Table.GetCount(); }
		virtual bool HasItemAttribute (const CString &sAttrib) const override;
		virtual bool IsVariant (void) const override;
		virtual ALERROR LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx) override;

		virtual bool FindDefaultDesc (DeviceNames iDev, SDeviceDesc *retDesc) const override;
		virtual bool FindDefaultDesc (CSpaceObject *pObj, const CItem &Item, SDeviceDesc *retDesc) const override;
		virtual bool FindDefaultDesc (const CDeviceDescList &DescList, const CItem &Item, SDeviceDesc *retDesc) const override;
		virtual bool FindDefaultDesc (const CDeviceDescList &DescList, const CString &sID, SDeviceDesc *retDesc) const override;

	private:
		struct SEntry
			{
			IDeviceGenerator *pDevice;
			int iChance;
			};

		struct SSlotDesc
			{
			CItemCriteria Criteria;
			SDeviceDesc DefaultDesc;
			int iMaxCount;
			};

		const SSlotDesc *FindSlotDesc (CSpaceObject *pObj, const CItem &Item) const;

		DiceRange m_Count;

		TArray<SEntry> m_Table;
		TArray<SSlotDesc> m_SlotDesc;
	};

ALERROR IDeviceGenerator::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, IDeviceGenerator **retpGenerator)

//	CreateFromXML
//
//	Creates a new generator

	{
	ALERROR error;
	IDeviceGenerator *pGenerator = NULL;

	if (strEquals(pDesc->GetTag(), DEVICE_TAG) || strEquals(pDesc->GetTag(), ITEM_TAG))
		pGenerator = new CSingleDevice;
	else if (strEquals(pDesc->GetTag(), TABLE_TAG))
		pGenerator = new CTableOfDeviceGenerators;
	else if (strEquals(pDesc->GetTag(), GROUP_TAG) || strEquals(pDesc->GetTag(), DEVICES_TAG) || strEquals(pDesc->GetTag(), DEVICE_SLOTS_TAG))
		pGenerator = new CGroupOfDeviceGenerators;
	else if (strEquals(pDesc->GetTag(), LEVEL_TABLE_TAG))
		pGenerator = new CLevelTableOfDeviceGenerators;
	else if (strEquals(pDesc->GetTag(), NULL_TAG))
		pGenerator = new CNullDevice;
	else
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Unknown device generator: %s"), pDesc->GetTag());
		return ERR_FAIL;
		}

	if (error = pGenerator->LoadFromXML(Ctx, pDesc))
		{
		if (pGenerator)
			delete pGenerator;
		return error;
		}

	*retpGenerator = pGenerator;

	return NOERROR;
	}

ALERROR IDeviceGenerator::InitDeviceDescFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, SDeviceDesc *retDesc)

//	InitDeviceDescFromXML
//
//	Loads a device desc from XML.

	{
	ALERROR error;

	retDesc->sID = pDesc->GetAttribute(ID_ATTRIB);

	C3DObjectPos Pos;
	if (Pos.InitFromXML(pDesc, C3DObjectPos::FLAG_CALC_POLAR, &retDesc->b3DPosition))
		{
		retDesc->iPosAngle = Pos.GetAngle();
		retDesc->iPosRadius = Pos.GetRadius();
		retDesc->iPosZ = Pos.GetZ();
		}
	else
		{
		retDesc->iPosAngle = 0;
		retDesc->iPosRadius = 0;
		retDesc->iPosZ = 0;
		retDesc->b3DPosition = false;
		}

	retDesc->rShotSeparationScale = pDesc->GetAttributeDoubleBounded(SHOT_SEPARATION_SCALE_ATTRIB, -1.0, 1.0, 1.0);

	retDesc->bExternal = pDesc->GetAttributeBool(EXTERNAL_ATTRIB);
	retDesc->bCannotBeEmpty = pDesc->GetAttributeBool(CANNOT_BE_EMPTY_ATTRIB);
	retDesc->bOmnidirectional = pDesc->GetAttributeBool(OMNIDIRECTIONAL_ATTRIB);

	if (error = CItemType::ParseFate(Ctx, pDesc->GetAttribute(FATE_ATTRIB), &retDesc->iFate))
		return error;

	//	If we have a fireArc attribute, then we're defining the arc in terms of center angle
	//	and arc.

	int iArcAngle;
	if (pDesc->FindAttributeInteger(FIRE_ARC_ATTRIB, &iArcAngle))
		{
		//	Fire out the center angle

		int iCenterAngle;
		if (!pDesc->FindAttributeInteger(FIRE_ANGLE_ATTRIB, &iCenterAngle))
			iCenterAngle = retDesc->iPosAngle;

		//	Calculate min and max fire arc

		int iHalfArc = Max(iArcAngle / 2, 0);
		retDesc->iMinFireArc = AngleMod(iCenterAngle - iHalfArc);
		retDesc->iMaxFireArc = AngleMod(iCenterAngle + iHalfArc);
		}

	//	Otherwise, we support min/max fire arc

	else
		{
		retDesc->iMinFireArc = AngleMod(pDesc->GetAttributeInteger(MIN_FIRE_ARC_ATTRIB));
		retDesc->iMaxFireArc = AngleMod(pDesc->GetAttributeInteger(MAX_FIRE_ARC_ATTRIB));
		}

	if (error = CDeviceClass::ParseLinkedFireOptions(Ctx, pDesc->GetAttribute(LINKED_FIRE_ATTRIB), &retDesc->dwLinkedFireOptions))
		return error;

	retDesc->bSecondary = pDesc->GetAttributeBool(SECONDARY_WEAPON_ATTRIB);

	//	Slot enhancements

	CXMLElement *pEnhanceList = pDesc->GetContentElementByTag(ENHANCE_ABILITIES_TAG);
	if (pEnhanceList)
		{
		if (error = retDesc->Enhancements.InitFromXML(Ctx, pEnhanceList, NULL))
			return error;
		}

	retDesc->iSlotBonus = pDesc->GetAttributeInteger(HP_BONUS_ATTRIB);

	return NOERROR;
	}

//	CSingleDevice -------------------------------------------------------------

CSingleDevice::~CSingleDevice (void)

//	CSingleDevice destructor

	{
	if (m_pExtraItems)
		delete m_pExtraItems;
	}

void CSingleDevice::AddDevices (SDeviceGenerateCtx &Ctx)

//	AddDevices
//
//	Add devices to list

	{
	int i;

	ASSERT(Ctx.pResult);
	if (m_pItemType == NULL || Ctx.pResult == NULL)
		return;

	int iCount = m_Count.Roll();
	for (i = 0; i < iCount; i++)
		{
		//	Initialize the desc

		SDeviceDesc Desc;
		Desc.Item = CItem(m_pItemType, 1);

        //  Level

        if (!m_Level.IsEmpty())
            {
            CString sError;
            if (!Desc.Item.SetLevel(m_Level.Roll(), &sError))
                {
                if (Ctx.GetUniverse().InDebugMode())
                    ::kernelDebugLogString(sError);
                }
            }

		//	Charges

		if (m_iCharges)
			Desc.Item.SetCharges(m_iCharges);

        //  Damage/enhancement

		if (mathRandom(1, 100) <= m_iDamaged)
			Desc.Item.SetDamaged();
		else
			m_Enhanced.EnhanceItem(Desc.Item);

		//	Find the default settings for the device slot for this device

		SDeviceDesc SlotDesc;
		bool bUseSlotDesc = FindSlot(Ctx, Desc.Item, SlotDesc);

		//	Set the device position appropriately, either from the <Device> element,
		//	from the slot descriptor at the root, or from defaults.

		if (!m_bDefaultPos)
			{
			Desc.iPosAngle = m_iPosAngle;
			Desc.iPosRadius = m_iPosRadius;
			Desc.iPosZ = m_iPosZ;
			Desc.b3DPosition = m_b3DPosition;
			}
		else if (bUseSlotDesc)
			{
			Desc.sID = SlotDesc.sID;
			Desc.iPosAngle = SlotDesc.iPosAngle;
			Desc.iPosRadius = SlotDesc.iPosRadius;
			Desc.iPosZ = SlotDesc.iPosZ;
			Desc.b3DPosition = SlotDesc.b3DPosition;
			}

		if (bUseSlotDesc)
			Desc.rShotSeparationScale = SlotDesc.rShotSeparationScale;
		else
			Desc.rShotSeparationScale = m_rShotSeparationScale;

		//	External

		if (bUseSlotDesc)
			Desc.bExternal = SlotDesc.bExternal;
		else
			Desc.bExternal = m_bExternal;

		//	Cannot be empty

		if (bUseSlotDesc)
			Desc.bCannotBeEmpty = SlotDesc.bCannotBeEmpty;
		else
			Desc.bCannotBeEmpty = m_bCannotBeEmpty;

		//	Fate

		if (bUseSlotDesc)
			Desc.iFate = SlotDesc.iFate;
		else
			Desc.iFate = m_iFate;

		//	Set the device fire arc appropriately.

		if (!m_bDefaultFireArc)
			{
			Desc.bOmnidirectional = m_bOmnidirectional;
			Desc.iMinFireArc = m_iMinFireArc;
			Desc.iMaxFireArc = m_iMaxFireArc;
			}
		else if (bUseSlotDesc)
			{
			Desc.bOmnidirectional = SlotDesc.bOmnidirectional;
			Desc.iMinFireArc = SlotDesc.iMinFireArc;
			Desc.iMaxFireArc = SlotDesc.iMaxFireArc;
			}

		//	Set linked fire

		if (!m_bDefaultLinkedFire)
			Desc.dwLinkedFireOptions = m_dwLinkedFireOptions;
		else if (bUseSlotDesc)
			Desc.dwLinkedFireOptions = SlotDesc.dwLinkedFireOptions;
		else
			Desc.dwLinkedFireOptions = 0;

		Desc.bSecondary = m_bSecondary || (bUseSlotDesc && SlotDesc.bSecondary);

		//	Enhancements

		if (!m_Enhancements.IsEmpty())
			Desc.Enhancements = m_Enhancements;
		else if (bUseSlotDesc)
			Desc.Enhancements = SlotDesc.Enhancements;

		//	Slot bonus

		if (!m_bDefaultSlotBonus)
			Desc.iSlotBonus = m_iSlotBonus;
		else if (bUseSlotDesc)
			Desc.iSlotBonus = SlotDesc.iSlotBonus;
		else
			Desc.iSlotBonus = 0;

		//	Add extra items

		if (m_pExtraItems)
			{
			CItemListManipulator ItemList(Desc.ExtraItems);
			SItemAddCtx ItemCtx(ItemList);
			ItemCtx.iLevel = Ctx.iLevel;

			m_pExtraItems->AddItems(ItemCtx);
			}

		//	Done
		
		Ctx.pResult->AddDeviceDesc(Desc);
		}
	}

void CSingleDevice::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds list of types used.

	{
	if (m_pItemType)
		retTypesUsed->SetAt(m_pItemType->GetUNID(), true);

	if (m_pExtraItems)
		m_pExtraItems->AddTypesUsed(retTypesUsed);
	}

bool CSingleDevice::FindSlot (SDeviceGenerateCtx &Ctx, const CItem &Item, SDeviceDesc &retSlotDesc) const

//	FindSlot
//
//	Finds an existing slot to put the new device in. If we find one, we return
//	TRUE and retSlotDesc is initialized.

	{
	//	If no slot definitions, then we're done.

	if (Ctx.pRoot == NULL)
		return false;

	//	If we have a slot ID, then we need to look for that slot.

	else if (!m_sSlotID.IsBlank())
		{
		if (!Ctx.pRoot->FindDefaultDesc(*Ctx.pResult, m_sSlotID, &retSlotDesc))
			{
			if (Ctx.GetUniverse().InDebugMode())
				::kernelDebugLogPattern("WARNING: Unable to find device slot %s", m_sSlotID);
			return false;
			}

		return true;
		}

	//	Otherwise, see if a slot wants this item

	else if (Ctx.pRoot->FindDefaultDesc(*Ctx.pResult, Item, &retSlotDesc))
		return true;

	//	Otherwise, not found

	else
		return false;
	}

ALERROR CSingleDevice::FinishBind (SDesignLoadCtx &Ctx)

//	FinishBind
//
//	Resolve references

	{
	if (m_pExtraItems)
		if (ALERROR error = m_pExtraItems->FinishBind(Ctx))
			return error;

	return NOERROR;
	}

bool CSingleDevice::HasItemAttribute (const CString &sAttrib) const

//	HasItemAttribute
//
//	Returns TRUE if any items have the given attribute.

	{
	if (m_pItemType && m_pItemType->HasAttribute(sAttrib))
		return true;

	if (m_pExtraItems && m_pExtraItems->HasItemAttribute(sAttrib))
		return true;

	return false;
	}

bool CSingleDevice::IsVariant (void) const

//	IsVariant
//
//	Returns TRUE if we the devices we return can vary.

	{
	if (m_iDamaged != 0 && m_iDamaged != 100)
		return true;
	else if (m_Enhanced.IsVariant())
		return true;
	else
		return false;
	}

ALERROR CSingleDevice::LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	LoadFromXML
//
//	Load from XML

	{
	ALERROR error;

	//	Load the item type

	CString sUNID = pDesc->GetAttribute(DEVICE_ID_ATTRIB);
	if (sUNID.IsBlank())
		sUNID = pDesc->GetAttribute(ITEM_ATTRIB);

	if (error = m_pItemType.LoadUNID(Ctx, sUNID))
		return error;

	if (m_pItemType.GetUNID() == 0)
		{
		Ctx.sError = strPatternSubst(CONSTLIT("<%s> element missing item attribute."), pDesc->GetTag());
		return ERR_FAIL;
		}

	//	Load the count

	m_Count.LoadFromXML(pDesc->GetAttribute(COUNT_ATTRIB));
	if (m_Count.IsEmpty())
		m_Count.SetConstant(1);

	//	Load enhancement chance

	if (error = m_Enhanced.InitFromXML(Ctx, pDesc))
		return error;

    //  Various properties 

	m_iDamaged = pDesc->GetAttributeInteger(DAMAGED_ATTRIB);
    m_Level.LoadFromXML(pDesc->GetAttribute(LEVEL_ATTRIB));
	m_sSlotID = pDesc->GetAttribute(SLOT_ID_ATTRIB);
	m_iCharges = pDesc->GetAttributeIntegerBounded(CHARGES_ATTRIB, 0, -1, 0);

	//	Load device position attributes

	C3DObjectPos Pos;
	if (Pos.InitFromXML(pDesc, C3DObjectPos::FLAG_CALC_POLAR, &m_b3DPosition))
		{
		m_iPosAngle = Pos.GetAngle();
		m_iPosRadius = Pos.GetRadius();
		m_iPosZ = Pos.GetZ();
		m_bDefaultPos = false;
		}
	else
		{
		m_iPosAngle = 0;
		m_iPosRadius = 0;
		m_iPosZ = 0;
		m_b3DPosition = false;
		m_bDefaultPos = true;
		}

	m_rShotSeparationScale = pDesc->GetAttributeDoubleBounded(SHOT_SEPARATION_SCALE_ATTRIB, -1.0, 1.0, 1.0);

	//	Load fire arc attributes

	int iFireArc;
	if (pDesc->FindAttributeInteger(FIRE_ARC_ATTRIB, &iFireArc))
		{
		m_bOmnidirectional = false;

		int iFireAngle;
		if (!pDesc->FindAttributeInteger(FIRE_ANGLE_ATTRIB, &iFireAngle))
			iFireAngle = m_iPosAngle;

		int iHalfArc = Max(0, iFireArc / 2);
		m_iMinFireArc = AngleMod(iFireAngle - iHalfArc);
		m_iMaxFireArc = AngleMod(iFireAngle + iHalfArc);

		m_bDefaultFireArc = false;
		}
	else if (pDesc->FindAttributeInteger(MIN_FIRE_ARC_ATTRIB, &m_iMinFireArc))
		{
		m_bOmnidirectional = false;
		m_iMinFireArc = AngleMod(m_iMinFireArc);
		m_iMaxFireArc = AngleMod(pDesc->GetAttributeInteger(MAX_FIRE_ARC_ATTRIB));
		m_bDefaultFireArc = false;
		}
	else if (pDesc->FindAttributeBool(OMNIDIRECTIONAL_ATTRIB, &m_bOmnidirectional))
		{
		m_iMinFireArc = 0;
		m_iMaxFireArc = 0;
		m_bDefaultFireArc = false;
		}
	else
		{
		m_bOmnidirectional = false;
		m_iMinFireArc = 0;
		m_iMaxFireArc = 0;
		m_bDefaultFireArc = true;
		}

	m_bExternal = pDesc->GetAttributeBool(EXTERNAL_ATTRIB);
	m_bCannotBeEmpty = pDesc->GetAttributeBool(CANNOT_BE_EMPTY_ATTRIB);

	//	Fate

	CString sFate;
	if (pDesc->FindAttribute(FATE_ATTRIB, &sFate))
		{
		if (error = CItemType::ParseFate(Ctx, sFate, &m_iFate))
			return error;
		}
	else
		m_iFate = fateNone;

	//	Linked fire options

	CString sLinkedFire;
	if (pDesc->FindAttribute(LINKED_FIRE_ATTRIB, &sLinkedFire))
		{
		if (error = CDeviceClass::ParseLinkedFireOptions(Ctx, sLinkedFire, &m_dwLinkedFireOptions))
			return error;

		m_bDefaultLinkedFire = false;
		}
	else
		{
		m_dwLinkedFireOptions = 0;
		m_bDefaultLinkedFire = true;
		}

	m_bSecondary = pDesc->GetAttributeBool(SECONDARY_WEAPON_ATTRIB);

	//	Slot enhancements

	CXMLElement *pEnhanceList = pDesc->GetContentElementByTag(ENHANCE_ABILITIES_TAG);
	if (pEnhanceList)
		{
		if (error = m_Enhancements.InitFromXML(Ctx, pEnhanceList, NULL))
			return error;
		}

	//	Slot bonus

	if (pDesc->FindAttributeInteger(HP_BONUS_ATTRIB, &m_iSlotBonus))
		m_bDefaultSlotBonus = false;
	else
		{
		m_iSlotBonus = 0;
		m_bDefaultSlotBonus = true;
		}

	//	Load extra items

	CXMLElement *pItems = pDesc->GetContentElementByTag(ITEMS_TAG);
	if (pItems)
		{
		if (error = IItemGenerator::CreateFromXML(Ctx, pItems, &m_pExtraItems))
			return error;
		}

	return NOERROR;
	}

ALERROR CSingleDevice::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Resolve references

	{
	ALERROR error;

	if (error = m_pItemType.Bind(Ctx))
		return error;

	if (m_pExtraItems)
		if (error = m_pExtraItems->OnDesignLoadComplete(Ctx))
			return error;

	if (m_Enhancements.Bind(Ctx))
		return error;

	//	Error checking

	if (m_pItemType)
		if (m_pItemType->GetDeviceClass() == NULL)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("%s is not a device"), m_pItemType->GetNounPhrase(nounActual));
			return ERR_FAIL;
			}

	return NOERROR;
	}

//	CTableOfDeviceGenerators --------------------------------------------------

CTableOfDeviceGenerators::~CTableOfDeviceGenerators (void)

//	CTableOfDeviceGenerators destructor

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pDevice)
			delete m_Table[i].pDevice;
	}

void CTableOfDeviceGenerators::AddDevices (SDeviceGenerateCtx &Ctx)

//	AddDevices
//
//	Add devices

	{
	int i, j;

	int iCount = m_Count.Roll();
	for (j = 0; j < iCount; j++)
		{
		int iRoll = mathRandom(1, m_iTotalChance);

		for (i = 0; i < m_Table.GetCount(); i++)
			{
			iRoll -= m_Table[i].iChance;

			if (iRoll <= 0)
				{
				m_Table[i].pDevice->AddDevices(Ctx);
				break;
				}
			}
		}
	}

void CTableOfDeviceGenerators::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds list of types used.

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		m_Table[i].pDevice->AddTypesUsed(retTypesUsed);
	}

ALERROR CTableOfDeviceGenerators::FinishBind (SDesignLoadCtx &Ctx)

//	FinishBind
//
//	Resolve references

	{
	for (int i = 0; i < m_Table.GetCount(); i++)
		{
		if (ALERROR error = m_Table[i].pDevice->FinishBind(Ctx))
			return error;
		}

	return NOERROR;
	}

bool CTableOfDeviceGenerators::HasItemAttribute (const CString &sAttrib) const

//	HasItemAttribute
//
//	Returns if any item has the given attribute

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pDevice->HasItemAttribute(sAttrib))
			return true;

	return false;
	}

bool CTableOfDeviceGenerators::IsVariant (void) const

//	IsVariant
//
//	Returns TRUE if we can vary.

	{
	if (m_Table.GetCount() == 0)
		return false;
	else if (m_Table.GetCount() == 1)
		return m_Table[0].pDevice->IsVariant();
	else
		return true;
	}

ALERROR CTableOfDeviceGenerators::LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	LoadFromXML
//
//	Load from XML

	{
	int i;
	ALERROR error;

	m_Count.LoadFromXML(pDesc->GetAttribute(COUNT_ATTRIB));
	if (m_Count.IsEmpty())
		m_Count.SetConstant(1);

	m_iTotalChance = 0;
	int iCount = pDesc->GetContentElementCount();
	if (iCount > 0)
		{
		m_Table.InsertEmpty(iCount);
		for (i = 0; i < iCount; i++)
			{
			CXMLElement *pEntry = pDesc->GetContentElement(i);
			
			m_Table[i].iChance = pEntry->GetAttributeInteger(CHANCE_ATTRIB);
			m_iTotalChance += m_Table[i].iChance;

			if (error = IDeviceGenerator::CreateFromXML(Ctx, pEntry, &m_Table[i].pDevice))
				{
				m_Table[i].pDevice = NULL;
				return error;
				}
			}
		}

	return NOERROR;
	}

ALERROR CTableOfDeviceGenerators::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Resolve references

	{
	ALERROR error;
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		{
		if (error = m_Table[i].pDevice->OnDesignLoadComplete(Ctx))
			return error;
		}

	return NOERROR;
	}

//	CLevelTableOfDeviceGenerators ---------------------------------------------

CLevelTableOfDeviceGenerators::~CLevelTableOfDeviceGenerators (void)

//	CLevelTableOfDeviceGenerators destructor

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pDevice)
			delete m_Table[i].pDevice;
	}

void CLevelTableOfDeviceGenerators::AddDevices (SDeviceGenerateCtx &Ctx)

//	AddDevices
//
//	Adds devices

	{
	int i, j;

	//	Compute probabilities

	if (Ctx.iLevel != m_iComputedLevel)
		{
		m_iTotalChance = 0;
		for (i = 0; i < m_Table.GetCount(); i++)
			{
			m_Table[i].iChance = GetFrequencyByLevel(m_Table[i].sLevelFrequency, Ctx.iLevel);
			m_iTotalChance += m_Table[i].iChance;
			}

		m_iComputedLevel = Ctx.iLevel;
		}

	//	Generate

	if (m_iTotalChance)
		{
		int iCount = m_Count.Roll();

		for (i = 0; i < iCount; i++)
			{
			int iRoll = mathRandom(1, m_iTotalChance);

			for (j = 0; j < m_Table.GetCount(); j++)
				{
				iRoll -= m_Table[j].iChance;

				if (iRoll <= 0)
					{
					m_Table[j].pDevice->AddDevices(Ctx);
					break;
					}
				}
			}
		}
	}

void CLevelTableOfDeviceGenerators::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds list of types used.

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		m_Table[i].pDevice->AddTypesUsed(retTypesUsed);
	}

ALERROR CLevelTableOfDeviceGenerators::FinishBind (SDesignLoadCtx &Ctx)

//	FinishBind
//
//	Bind design

	{
	for (int i = 0; i < m_Table.GetCount(); i++)
		{
		if (ALERROR error = m_Table[i].pDevice->FinishBind(Ctx))
			return error;
		}

	return NOERROR;
	}

bool CLevelTableOfDeviceGenerators::HasItemAttribute (const CString &sAttrib) const

//	HasItemAttribute
//
//	Returns TRUE if we have any item with the given attribute.

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pDevice->HasItemAttribute(sAttrib))
			return true;

	return false;
	}

bool CLevelTableOfDeviceGenerators::IsVariant (void) const

//	IsVariant
//
//	Returns TRUE if we can vary.

	{
	if (m_Table.GetCount() == 0)
		return false;
	else
		return true;
	}

ALERROR CLevelTableOfDeviceGenerators::LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	LoadFromXML
//
//	Loads from XML

	{
	int i;
	ALERROR error;

	m_Count.LoadFromXML(pDesc->GetAttribute(COUNT_ATTRIB));
	if (m_Count.IsEmpty())
		m_Count.SetConstant(1);

	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pEntry = pDesc->GetContentElement(i);
		SEntry *pNewEntry = m_Table.Insert();

		pNewEntry->sLevelFrequency = pEntry->GetAttribute(LEVEL_FREQUENCY_ATTRIB);

		if (error = IDeviceGenerator::CreateFromXML(Ctx, pEntry, &pNewEntry->pDevice))
			{
			pNewEntry->pDevice = NULL;
			return error;
			}
		}

	m_iComputedLevel = -1;

	return NOERROR;
	}

ALERROR CLevelTableOfDeviceGenerators::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Bind design

	{
	ALERROR error;
	int i;

	m_iComputedLevel = -1;

	for (i = 0; i < m_Table.GetCount(); i++)
		{
		if (error = m_Table[i].pDevice->OnDesignLoadComplete(Ctx))
			return error;
		}

	return NOERROR;
	}

//	CGroupOfDeviceGenerators --------------------------------------------------

CGroupOfDeviceGenerators::~CGroupOfDeviceGenerators (void)

//	CGroupOfDeviceGenerators destructor

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pDevice)
			delete m_Table[i].pDevice;
	}

void CGroupOfDeviceGenerators::AddDevices (SDeviceGenerateCtx &Ctx)

//	AddDevices
//
//	Add devices

	{
	int i, j;

	int iCount = m_Count.Roll();
	for (j = 0; j < iCount; j++)
		{
		for (i = 0; i < m_Table.GetCount(); i++)
			{
			if (mathRandom(1, 100) <= m_Table[i].iChance)
				m_Table[i].pDevice->AddDevices(Ctx);
			}
		}
	}

void CGroupOfDeviceGenerators::AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsed
//
//	Adds list of types used.

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		m_Table[i].pDevice->AddTypesUsed(retTypesUsed);
	}

Metric CGroupOfDeviceGenerators::CalcHullPoints (void) const

//	CalcHullPoints
//
//	Returns the number of hull points (which goes towards calculating hull value)
//	from these device slots.

	{
	static const Metric CANNOT_BE_EMPTY_PENALTY = -0.2;

	Metric rPoints = 0.0;

	for (int i = 0; i < m_SlotDesc.GetCount(); i++)
		{
		if (m_SlotDesc[i].DefaultDesc.bCannotBeEmpty)
			rPoints += CANNOT_BE_EMPTY_PENALTY;
		}

	return rPoints;
	}

bool CGroupOfDeviceGenerators::FindDefaultDesc (DeviceNames iDev, SDeviceDesc *retDesc) const

//	FindDefaultDesc
//
//	Looks for a slot descriptor that might take the named device. Note that
//	this is somewhat inaccurate, since we're not guaranteed that any particular
//	item category would be accepted.

	{
	int i;

	ItemCategories Category = CItemType::GetCategoryForNamedDevice(iDev);

	//	Make a list of all slot entries that match the given item category and 
	//	pick the one with the shortest criteria description.

	TSortMap<int, int> BestEntry;
	for (i = 0; i < m_SlotDesc.GetCount(); i++)
		{
		if (m_SlotDesc[i].Criteria.MatchesItemCategory(Category))
			{
			BestEntry.Insert(m_SlotDesc[i].Criteria.AsString().GetLength(), i);
			}
		}

	//	If nothing matches, then not found

	if (BestEntry.GetCount() == 0)
		return false;

	//	Otherwise, pick the shortest (first) entry

	*retDesc = m_SlotDesc[BestEntry[0]].DefaultDesc;
	return true;
	}

bool CGroupOfDeviceGenerators::FindDefaultDesc (CSpaceObject *pObj, const CItem &Item, SDeviceDesc *retDesc) const

//	FindDefaultDesc
//
//	Looks for a slot descriptor that matches the given item and returns it.

	{
	int i;

	//	Look for a matching slot

	for (i = 0; i < m_SlotDesc.GetCount(); i++)
		{
		//	Skip if this slot does not meet criteria

		if (!Item.MatchesCriteria(m_SlotDesc[i].Criteria))
			continue;

		//	If this slot has an ID and maximum counts and if we've already 
		//	exceeded those counts, then skip.

		if (m_SlotDesc[i].iMaxCount != -1 
				&& !m_SlotDesc[i].DefaultDesc.sID.IsBlank()
				&& pObj
				&& pObj->GetDeviceSystem()
				&& pObj->GetDeviceSystem()->GetCountByID(m_SlotDesc[i].DefaultDesc.sID) >= m_SlotDesc[i].iMaxCount)
			continue;

		//	If we get this far, then this is a valid slot.

		*retDesc = m_SlotDesc[i].DefaultDesc;
		return true;
		}

	//	Otherwise we go with default (we assume that retDesc is already 
	//	initialized to default values).
	//
	//	For backwards compatibility, however, we place all weapons 20 pixels
	//	forward.

	ItemCategories iCategory = Item.GetType()->GetCategory();
	if (iCategory == itemcatWeapon || iCategory == itemcatLauncher)
		retDesc->iPosRadius = 20;

	//	Done

	return true;
	}

bool CGroupOfDeviceGenerators::FindDefaultDesc (const CDeviceDescList &DescList, const CItem &Item, SDeviceDesc *retDesc) const

//	FindDefaultDesc
//
//	Looks for a slot descriptor that matches the given item and returns it.

	{
	int i;

	//	Look for a matching slot

	for (i = 0; i < m_SlotDesc.GetCount(); i++)
		{
		//	Skip if this slot does not meet criteria

		if (!Item.MatchesCriteria(m_SlotDesc[i].Criteria))
			continue;

		//	If this slot has an ID and maximum counts and if we've already 
		//	exceeded those counts, then skip.

		if (m_SlotDesc[i].iMaxCount != -1 
				&& !m_SlotDesc[i].DefaultDesc.sID.IsBlank()
				&& DescList.GetCountByID(m_SlotDesc[i].DefaultDesc.sID) >= m_SlotDesc[i].iMaxCount)
			continue;

		//	If we get this far, then this is a valid slot.

		*retDesc = m_SlotDesc[i].DefaultDesc;
		return true;
		}

	//	Otherwise we go with default (we assume that retDesc is already 
	//	initialized to default values).
	//
	//	For backwards compatibility, however, we place all weapons 20 pixels
	//	forward.

	ItemCategories iCategory = Item.GetType()->GetCategory();
	if (iCategory == itemcatWeapon || iCategory == itemcatLauncher)
		retDesc->iPosRadius = 20;

	//	Done

	return true;
	}

bool CGroupOfDeviceGenerators::FindDefaultDesc (const CDeviceDescList &DescList, const CString &sID, SDeviceDesc *retDesc) const

//	FindDefaultDesc
//
//	Looks for a slot descriptor that matches the given ID and returns it.

	{
	int i;

	//	Look for a matching slot

	for (i = 0; i < m_SlotDesc.GetCount(); i++)
		{
		//	Skip if not the desired id

		if (!strEquals(m_SlotDesc[i].DefaultDesc.sID, sID))
			continue;

		//	If this slot has an ID and maximum counts and if we've already 
		//	exceeded those counts, then skip.

		if (m_SlotDesc[i].iMaxCount != -1 
				&& !m_SlotDesc[i].DefaultDesc.sID.IsBlank()
				&& DescList.GetCountByID(m_SlotDesc[i].DefaultDesc.sID) >= m_SlotDesc[i].iMaxCount)
			continue;

		//	If we get this far, then this is a valid slot.

		*retDesc = m_SlotDesc[i].DefaultDesc;
		return true;
		}

	//	Not found

	return false;
	}

const CGroupOfDeviceGenerators::SSlotDesc *CGroupOfDeviceGenerators::FindSlotDesc (CSpaceObject *pObj, const CItem &Item) const

//	FindSlotDesc
//
//	Returns the first slot descriptor that matches the item. If none of the
//	descriptors match, we return NULL.

	{
	int i;

	for (i = 0; i < m_SlotDesc.GetCount(); i++)
		if (Item.MatchesCriteria(m_SlotDesc[i].Criteria))
			return &m_SlotDesc[i];

	return NULL;
	}

ALERROR CGroupOfDeviceGenerators::FinishBind (SDesignLoadCtx &Ctx)

//	FinishBind
//
//	Resolve references

	{
	for (int i = 0; i < m_Table.GetCount(); i++)
		if (ALERROR error = m_Table[i].pDevice->FinishBind(Ctx))
			return error;

	return NOERROR;
	}

bool CGroupOfDeviceGenerators::HasItemAttribute (const CString &sAttrib) const

//	HasItemAttribute
//
//	Returns TRUE if any item has the given attribute

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		if (m_Table[i].pDevice->HasItemAttribute(sAttrib))
			return true;

	return false;
	}

bool CGroupOfDeviceGenerators::IsVariant (void) const

//	IsVariant
//
//	Returns TRUE if we can vary.

	{
	int i;

	for (i = 0; i < m_Table.GetCount(); i++)
		{
		if (m_Table[i].iChance != 0 && m_Table[i].iChance != 100)
			return true;
		else if (m_Table[i].pDevice->IsVariant())
			return true;
		}

	return false;
	}

ALERROR CGroupOfDeviceGenerators::LoadFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	LoadFromXML
//
//	Load from XML

	{
	int i;
	ALERROR error;

	m_Count.LoadFromXML(pDesc->GetAttribute(COUNT_ATTRIB));
	if (m_Count.IsEmpty())
		m_Count.SetConstant(1);

	//	Load either a <DeviceSlot> element or another device generator.

	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pEntry = pDesc->GetContentElement(i);

		if (strEquals(pEntry->GetTag(), DEVICE_SLOT_TAG))
			{
			SSlotDesc *pSlotDesc = m_SlotDesc.Insert();

			pSlotDesc->Criteria.Init(pEntry->GetAttribute(CRITERIA_ATTRIB));

			if (error = IDeviceGenerator::InitDeviceDescFromXML(Ctx, pEntry, &pSlotDesc->DefaultDesc))
				return error;

			pSlotDesc->iMaxCount = pEntry->GetAttributeIntegerBounded(MAX_COUNT_ATTRIB, 0, -1, -1);

			//	If we've got an ID, max count defaults to 1.

			if (pSlotDesc->iMaxCount == -1 && !pSlotDesc->DefaultDesc.sID.IsBlank())
				pSlotDesc->iMaxCount = 1;
			}
		else
			{
			SEntry *pTableEntry = m_Table.Insert();

			pTableEntry->iChance = pEntry->GetAttributeIntegerBounded(CHANCE_ATTRIB, 0, -1, 100);
			if (error = IDeviceGenerator::CreateFromXML(Ctx, pEntry, &pTableEntry->pDevice))
				{
				pTableEntry->pDevice = NULL;
				return error;
				}
			}
		}

	return NOERROR;
	}

ALERROR CGroupOfDeviceGenerators::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Resolve references

	{
	ALERROR error;

	for (int i = 0; i < m_Table.GetCount(); i++)
		if (error = m_Table[i].pDevice->OnDesignLoadComplete(Ctx))
			return error;

	for (int i = 0; i < m_SlotDesc.GetCount(); i++)
		if (error = m_SlotDesc[i].DefaultDesc.Enhancements.Bind(Ctx))
			return error;

	return NOERROR;
	}

//	CDeviceDescList -----------------------------------------------------------

CDeviceDescList::CDeviceDescList (void)

//	CDeviceDescList constructor

	{
	}

CDeviceDescList::~CDeviceDescList (void)

//	CDeviceDescList destructor

	{
	}

void CDeviceDescList::AddDeviceDesc (const SDeviceDesc &Desc)

//	AddDeviceDesc
//
//	Adds a device desc to the list

	{
	m_List.Insert(Desc);
	}

int CDeviceDescList::GetCountByID (const CString &sID) const

//	GetCountByID
//
//	Returns the number of entries with the given ID

	{
	int i;
	int iCount = 0;

	for (i = 0; i < GetCount(); i++)
		if (strEquals(m_List[i].sID, sID))
			iCount++;

	return iCount;
	}

const SDeviceDesc *CDeviceDescList::GetDeviceDescByName (DeviceNames iDev) const

//  GetDeviceDescByName
//
//  Returns a descriptor for a named device (or NULL if not found)

    {
	int i;
	ItemCategories iCatToFind = CDeviceClass::GetItemCategory(iDev);

	for (i = 0; i < GetCount(); i++)
		{
		CDeviceClass *pDevice = GetDeviceClass(i);

		//	See if this is the category that we want to find

        if (pDevice->GetCategory() == iCatToFind)
            return &GetDeviceDesc(i);
		}

	return NULL;
    }

CDeviceClass *CDeviceDescList::GetNamedDevice (DeviceNames iDev) const

//	GetNamedDevice
//
//	Returns the named device (or NULL if not found)

	{
    const SDeviceDesc *pDesc = GetDeviceDescByName(iDev);
    if (pDesc)
        return pDesc->Item.GetType()->GetDeviceClass();

	return NULL;
	}

