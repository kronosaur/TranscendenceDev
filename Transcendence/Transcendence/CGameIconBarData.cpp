//	CGameIconBarData.cpp
//
//	CGameIconBarData class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

const CGameIconBarData::SEntry CGameIconBarData::m_Defaults[DEFAULT_COUNT] =
	{
		{	CGameKeys::keyShipStatus,		0	},
		{	CGameKeys::keyShowMap,			1	},
		{	CGameKeys::keyShowGalacticMap,	2	},
		{	CGameKeys::keyInvokePower,		3	},
		{	CGameKeys::keyUseItem,			4	},
		{	CGameKeys::keyCommunications,	5	},
		{	CGameKeys::keyEnableDevice,		6	},
	};

CMenuData CGameIconBarData::CreateIconBar (const CDesignCollection &Design, const CGameSettings &Settings) const

//	CreateIconBar
//
//	Returns icon bar data.

	{
	//	Initialize if invalid

	if (!m_bValid)
		{
		//	Load the icon images

		m_pImage = TSharedPtr<CG32bitImage>(Design.GetImage(UNID_CORE_ICON_BAR, CDesignCollection::FLAG_IMAGE_COPY));
		m_Images.DeleteAll();
		m_Images.InsertEmpty(DEFAULT_COUNT);

		//	Initialize

		for (int i = 0; i < DEFAULT_COUNT; i++)
			{
			auto &Entry = m_Defaults[i];

			RECT rcImage;
			rcImage.left = ICON_WIDTH * i;
			rcImage.top = 0;
			rcImage.right = rcImage.left + ICON_WIDTH;
			rcImage.bottom = rcImage.top + ICON_HEIGHT;

			m_Images[i].InitFromBitmap(m_pImage, rcImage, 1, 0, false);
			}

		m_bValid = true;
		}

	//	Create a menu

	CMenuData Data;
	auto &KeyMap = Settings.GetKeyMap();

	for (int i = 0; i < DEFAULT_COUNT; i++)
		{
		auto &Entry = m_Defaults[i];

		Data.AddMenuItem(CGameKeys::GetCommandID(Entry.iCmd),
				CVirtualKeyData::GetKeyID(KeyMap.GetKey(Entry.iCmd)),
				NULL_STR,
				&m_Images[i],
				0,
				NULL_STR,
				NULL_STR,
				0,
				(DWORD)Entry.iCmd);
		}

	//	Done

	return Data;
	}
