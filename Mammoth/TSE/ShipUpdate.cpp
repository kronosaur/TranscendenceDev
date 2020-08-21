//	ShipUpdate.cpp
//
//	CShip class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CShip::OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick)

//	OnUpdate
//
//	Update

	{
	DEBUG_TRY

	CShipUpdateSet UpdateFlags;

	//	If we passed through a gate, then destroy ourselves

	if (m_fDestroyInGate)
		{
		UpdateDestroyInGate();
		return;
		}

	//	If we're in a gate, then all we do is update the in-gate timer

	if (IsInactive())
		{
		UpdateInactive();
		return;
		}

	//	Allow docking

	if (m_DockingPorts.GetPortCount() > 0)
		m_DockingPorts.UpdateAll(Ctx, this);

	//	Initialize

	int iTick = GetSystem()->GetTick() + GetDestiny();

	//	Trade

	if ((iTick % TRADE_UPDATE_FREQUENCY) == 0)
		UpdateTrade(Ctx, INVENTORY_REFRESHED_PER_UPDATE);

	//	We keep a target list across all device activations/update in case 
	//	multiple weapons on the ship want to access a target list (otherwise we
	//	would have to re-create the target list for every weapon).

	CTargetList TargetList;

	//	Check controls

	if (!IsParalyzed())
		{
		//	See if we're firing. Note that we fire before we rotate so that the
		//	fire behavior code can know which way we're aiming.

		if (!IsDisarmed())
			{
			CDeviceClass::SActivateCtx ActivateCtx(NULL, TargetList);
			if (!UpdateTriggerAllDevices(ActivateCtx, UpdateFlags))
				return;
			}
		else
			{
			if (m_iDisarmedTimer > 0)
				m_iDisarmedTimer--;
			}

		//	Update thrusting and rotation

		UpdateManeuvers(rSecondsPerTick);
		}

	//	If we're paralyzed, rotate in one direction

	else if (ShowParalyzedEffect())
		{
		//	Rotate wildly

		if (!IsAnchored())
			m_Rotation.Update(m_Perf.GetIntegralRotationDesc(), ((GetDestiny() % 2) ? RotateLeft : RotateRight));

		//	Slow down

		AddDrag(g_SpaceDragFactor);

		if (m_iParalysisTimer > 0)
			m_iParalysisTimer--;
		}

	//	Otherwise, we're paralyzed (by an overlay most-likely)

	else
		{
		//	Spin wildly

		if (!IsAnchored() && m_Overlays.GetConditions().IsSet(CConditionSet::cndSpinning))
			m_Rotation.Update(m_Perf.GetIntegralRotationDesc(), ((GetDestiny() % 2) ? RotateLeft : RotateRight));
		}

	//	Slow down if an overlay is imposing drag

	if (m_Overlays.GetConditions().IsSet(CConditionSet::cndDragged)
			&& !ShowParalyzedEffect())
		{
		//	We're too lazy to store the drag coefficient, so we recalculate it here.
		//
		//	(Since it's rare to have this, it shouldn't been too much of a performance
		//	penalty. But if necessary we can add a special function to just get the
		//	drag coefficient from the overlay list.)

		COverlay::SImpactDesc Impact;
		m_Overlays.GetImpact(this, Impact);

		AddDrag(Impact.rDrag);
		}

	//	If we're exceeding our maximum speed, then add a drag factor. This can
	//	happen if we're pushed beyond our max.

	const Metric rMaxSpeed = GetMaxSpeed();
	const Metric rMaxSpeed2 = rMaxSpeed * rMaxSpeed;
	if (GetVel().Length2() > rMaxSpeed2)
		AddDrag(g_SpaceDragFactor);

	//	Update armor

	if (m_Armor.Update(Ctx, this, iTick))
		UpdateFlags.Set(CShipUpdateSet::armorStatus);

	//	Update each device

	CDeviceClass::SDeviceUpdateCtx DeviceCtx(TargetList, iTick);
	if (!UpdateAllDevices(DeviceCtx, UpdateFlags))
		return;

	//	Update reactor

	if (m_pPowerUse)
		if (!UpdateFuel(Ctx, iTick))
			return;

	//	Conditions

	if (!UpdateConditions(UpdateFlags))
		return;

	//	Overlays

	if (!m_Overlays.IsEmpty())
		{
		bool bModified;

		m_Overlays.Update(this, m_pClass->GetImageViewportSize(), GetRotation(), &bModified);
		if (IsDestroyed())
			return;
		else if (bModified)
			{
			UpdateFlags.Set(CShipUpdateSet::overlays);

			UpdateFlags.Set(CShipUpdateSet::armorBonus);
			UpdateFlags.Set(CShipUpdateSet::deviceBonus);
			UpdateFlags.Set(CShipUpdateSet::armorStatus);
			UpdateFlags.Set(CShipUpdateSet::weaponStatus);

			UpdateFlags.Set(CShipUpdateSet::bounds);
			}
		}

	//	Check space environment

	if ((iTick % ENVIRONMENT_ON_UPDATE_CYCLE) == ENVIRONMENT_ON_UPDATE_OFFSET)
		{
		CSpaceEnvironmentType *pEnvironment = GetSystem()->GetSpaceEnvironment(GetPos());
		if (pEnvironment)
			{
			//	See if our LRS is affected

			if (pEnvironment->IsLRSJammer())
				{
				m_fLRSDisabledByNebula = true;
				m_fHiddenByNebula = true;
				}
			else
				{
				m_fLRSDisabledByNebula = false;
				m_fHiddenByNebula = false;
				}

			//	See if the environment causes drag (attached ship compartments
			//	are immune to drag, since they follow the main ship).

			Metric rDrag = pEnvironment->GetDragFactor();
			if (!m_fShipCompartment && rDrag != 1.0)
				AddDrag(rDrag);

			//	Update

			if (pEnvironment->HasOnUpdateEvent())
				{
				CString sError;
				if (pEnvironment->FireOnUpdate(this, &sError) != NOERROR)
					SendMessage(NULL, sError);
				}
			}
		else
			{
			m_fLRSDisabledByNebula = false;
			m_fHiddenByNebula = false;
			}
		}

	//	Update effects. For efficiency, we only update our effects if we painted
	//	last tick.

	if (WasPainted())
		m_Effects.Update(this, m_pClass->GetEffectsDesc(), GetRotation(), CalcEffectsMask());

	//	Invalidate

	if (UpdateFlags.IsSet(CShipUpdateSet::bounds))
		CalcBounds();

	if (UpdateFlags.IsSet(CShipUpdateSet::armorBonus))
		CalcArmorBonus();

	if (UpdateFlags.IsSet(CShipUpdateSet::deviceBonus))
		CalcDeviceBonus();

	if (m_fTrackMass && UpdateFlags.IsSet(CShipUpdateSet::cargo))
		OnComponentChanged(comCargo);

	if (m_fRecalcRotationAccel || UpdateFlags.IsSet(CShipUpdateSet::performance))
		CalcPerformance();

	if (UpdateFlags.IsSet(CShipUpdateSet::performance))
		m_pController->OnStatsChanged();

	if (UpdateFlags.IsSet(CShipUpdateSet::weaponStatus))
		m_pController->OnWeaponStatusChanged();

	if (UpdateFlags.IsSet(CShipUpdateSet::armorStatus))
		m_pController->OnShipStatus(IShipController::statusArmorRepaired, -1);
	
	if (UpdateFlags.IsSet(CShipUpdateSet::cargo))
		InvalidateItemListAddRemove();

	//	Update

	int iCounterIncRate = m_pClass->GetHullDesc().GetCounterIncrementRate();
	if (iCounterIncRate > 0)
		{
		//	If counter increment rate is greater than zero, then we allow the counter value to be unbounded below
		//	but bounded above
		m_iCounterValue = Min(m_iCounterValue + iCounterIncRate, m_pClass->GetHullDesc().GetMaxCounter());
		}
	else
		{
		//	Else we allow the counter value to be unbounded above
		//	but bounded below
		m_iCounterValue = Max(0, m_iCounterValue + iCounterIncRate);
		}

	DEBUG_CATCH
	}

bool CShip::UpdateAllDevices (CDeviceClass::SDeviceUpdateCtx &DeviceCtx, CShipUpdateSet &UpdateFlags)

//	UpdateAllDevices
//
//	Updates all devices. Returns FALSE if the ship was destroyed.

	{
	m_fDeviceDisrupted = false;

	for (CDeviceItem DeviceItem : GetDeviceSystem())
		{
		CInstalledDevice &Device = *DeviceItem.GetInstalledDevice();

		DeviceCtx.ResetOutputs();

		Device.Update(this, DeviceCtx);

		if (IsDestroyed())
			return false;

		if (DeviceCtx.bConsumedItems)
			{
			UpdateFlags.Set(CShipUpdateSet::weaponStatus);
			UpdateFlags.Set(CShipUpdateSet::cargo);
			}

		if (DeviceCtx.bDisrupted)
			m_fDeviceDisrupted = true;

		//	If the device was repaired, then we need to recalc performance, etc.

		if (DeviceCtx.bRepaired)
			{
			UpdateFlags.Set(CShipUpdateSet::armorBonus);
			UpdateFlags.Set(CShipUpdateSet::deviceBonus);
			}

		if (DeviceCtx.bSetDisabled && Device.SetEnabled(this, false))
			{
			UpdateFlags.Set(CShipUpdateSet::armorBonus);
			UpdateFlags.Set(CShipUpdateSet::deviceBonus);
			UpdateFlags.Set(CShipUpdateSet::performance);

			UpdateFlags.Set(CShipUpdateSet::armorStatus);
			UpdateFlags.Set(CShipUpdateSet::weaponStatus);

			m_pController->OnDeviceEnabledDisabled(Device.GetDeviceSlot(), false);
			}
		}

	return true;
	}

bool CShip::UpdateConditions (CShipUpdateSet &UpdateFlags)

//	UpdateConditions
//
//	Update conditions, such a radiation, etc. Returns FALSE if the ship was 
//	destroyed.

	{
	//	Drive damage timer

	if (m_iDriveDamagedTimer > 0
			&& (--m_iDriveDamagedTimer == 0))
		{
		//	If drive is repaired, update performance

		UpdateFlags.Set(CShipUpdateSet::performance);
		}

	//	Radiation

	if (m_fRadioactive)
		{
		m_iContaminationTimer--;
		if (m_iContaminationTimer > 0)
			{
			m_pController->OnShipStatus(IShipController::statusRadiationWarning, m_iContaminationTimer);
			}
		else
			{
			if (m_pIrradiatedBy)
				Destroy(m_pIrradiatedBy->GetCause(), *m_pIrradiatedBy);
			else
				Destroy(killedByRadiationPoisoning, CDamageSource(NULL, killedByRadiationPoisoning));

			//	Shouldn't do anything else after being destroyed
			return false;
			}
		}

	//	Blindness

	if (m_iBlindnessTimer > 0)
		if (--m_iBlindnessTimer == 0)
			m_pController->OnAbilityChanged(ablShortRangeScanner, ablRepair);

	//	LRS blindness

	if (m_iLRSBlindnessTimer > 0)
		if (--m_iLRSBlindnessTimer == 0)
			m_pController->OnAbilityChanged(ablLongRangeScanner, ablRepair);

	return true;
	}

void CShip::UpdateDestroyInGate (void)

//	UpdateDestroyInGate
//
//	Called in OnUpdate when m_fDestroyInGate is true.

	{
	const CAISettings *pAI = m_pController->GetAISettings();

	//	If we're supposed to ascend on gate, then ascend now

	if (GetCharacter() != NULL
			|| (pAI && pAI->AscendOnGate()))
		{
		ResetMaxSpeed();
		m_fDestroyInGate = false;
		GetSystem()->AscendObject(this);
		}

	//	If we're already suspended, then instead of destroying the ship, remove
	//	it from the system and re-add it. We need to do this so that all
	//	OnObjDestroyed notifications go out, etc.
	//
	//	Note that this will also clear out any events registered for the ship.

	else if (IsSuspended())
		{
		CSystem *pSystem = GetSystem();
		if (pSystem)
			{
			Remove(enteredStargate, CDamageSource());
			AddToSystem(*pSystem);
			OnNewSystem(pSystem);
			m_fDestroyInGate = false;
			}
		}
	else
		Destroy(enteredStargate, CDamageSource());
	}

bool CShip::UpdateFuel (SUpdateCtx &Ctx, int iTick)

//	UpdateFuel
//
//	Update fuel. We return FALSE if the ship is destroyed and we cannot continue
//	with update.
//
//	NOTE: iTick is a local system tick, not the global universe tick.

	{
	ASSERT(m_pPowerUse);

	DEBUG_TRY

	//	If we're out of power, then see if power is restored before we die from
	//	life-support failure.

	if (m_pPowerUse->IsOutOfPower())
		{
		//	If we've got power back, then we're OK

		if (m_Perf.GetReactorDesc().GetMaxPower() > 0)
			{
			m_pPowerUse->SetOutOfPower(false);
			m_pController->OnShipStatus(IShipController::statusReactorRestored);
			}

		//	Countdown grace period

		else if (m_pPowerUse->UpdateGraceTimer())
			{
			Destroy(killedByPowerFailure, CDamageSource(NULL, killedByPowerFailure));

			//	Shouldn't do anything else after being destroyed

			return false;
			}
		else
			m_pController->OnShipStatus(IShipController::statusLifeSupportWarning, m_pPowerUse->GetGraceTimer() / g_TicksPerSecond);
		}

	//	If we're out of fuel, then count down until we die

	else if (m_pPowerUse->IsOutOfFuel())
		{
		//	Countdown grace period

		if (m_pPowerUse->UpdateGraceTimer())
			{
			Destroy(killedByRunningOutOfFuel, CDamageSource(NULL, killedByRunningOutOfFuel));

			//	Shouldn't do anything else after being destroyed

			return false;
			}
		else
			m_pController->OnShipStatus(IShipController::statusLifeSupportWarning, m_pPowerUse->GetGraceTimer() / g_TicksPerSecond);
		}

	//	Otherwise, consume fuel

	else
		{
		int iPowerGenerated;
		int iPowerUsed = CalcPowerUsed(Ctx, &iPowerGenerated);

		//	Update our power use for this tick

		Metric rConsumed;
		m_pPowerUse->UpdatePowerUse(iPowerUsed, iPowerGenerated, m_Perf.GetReactorDesc().GetEfficiency(), &rConsumed);
		m_pController->OnFuelConsumed(rConsumed, CReactorDesc::fuelConsume);

		//	Make sure everything is running smoothly.

		if ((iTick % FUEL_CHECK_CYCLE) == 0)
			{
			Metric rFuelLeft;
			int iMaxPower = m_Perf.GetReactorDesc().GetMaxPower();
			int iPowerDrain = m_pPowerUse->GetPowerNeeded();

			//	If we're consuming more power than the reactor can output, then 
			//	we overload.

			if (iPowerDrain > iMaxPower)
				{
				m_pController->OnShipStatus(IShipController::statusReactorOverloadWarning, iTick / FUEL_CHECK_CYCLE);

				//	Consequences of reactor overload

				ReactorOverload(iPowerDrain);
				}

			//	If our reactor has 0 power, then we can't even run life-support,
			//	so we die.

			if (iMaxPower == 0)
				{
				m_pPowerUse->SetOutOfPower(true);
				m_pPowerUse->SetGraceTimer(FUEL_GRACE_PERIOD);
				DisableAllDevices();
				m_pController->OnShipStatus(IShipController::statusReactorPowerFailure);
				}

			//	If we don't consume fuel, then no need to check further

			else if (!m_Perf.GetReactorDesc().UsesFuel())
				NULL;

			//	If we have no fuel left, then we may die

			else if ((rFuelLeft = GetFuelLeft()) <= 0.0)
				{
				//	See if the player has any fuel on board. If they do, then there
				//	is a small grace period

				if (HasFuelItem())
					{
					m_pPowerUse->SetOutOfFuel(true);
					m_pPowerUse->SetGraceTimer(FUEL_GRACE_PERIOD);
					DisableAllDevices();
					m_pController->OnShipStatus(IShipController::statusFuelLowWarning, -1);
					}

				//	Otherwise, the player is out of luck

				else
					{
					Destroy(killedByRunningOutOfFuel, CDamageSource(NULL, killedByRunningOutOfFuel));

					//	Shouldn't do anything else after being destroyed
					return false;
					}
				}

			//	If we have low fuel, then warn the player.

			else if (rFuelLeft < (GetMaxFuel() / 8.0))
				m_pController->OnShipStatus(IShipController::statusFuelLowWarning, iTick / FUEL_CHECK_CYCLE);
			}
		}

	//	Done

	return true;

	DEBUG_CATCH
	}

void CShip::UpdateInactive (void)

//	UpdateInactive
//
//	Updates if we are inactive

	{
	if (IsInGate() && !IsSuspended())
		{
		//	Gate effect

		if (m_iExitGateTimer == GATE_ANIMATION_LENGTH)
			if (m_pExitGate)
				m_pExitGate->OnObjLeaveGate(this);

		//	Done?

		if (--m_iExitGateTimer == 0)
			{
			if (m_pExitGate)
				Place(m_pExitGate->GetPos());

			if (!IsVirtual())
				ClearCannotBeHit();
			FireOnEnteredSystem(m_pExitGate);
			m_pExitGate = NULL;
			}
		}
	}

void CShip::UpdateManeuvers (Metric rSecondsPerTick)

//	UpdateManeuvers
//
//	Update ship maneuvers

	{
	//	Update rotation

	m_Rotation.Update(m_Perf.GetIntegralRotationDesc(), m_pController->GetManeuver());

	//	See if we're accelerating

	if (IsInertialess())
		{
		if (m_pController->GetThrust())
			{
			CVector vVel = PolarToVector(GetRotation(), GetMaxSpeed());
			SetVel(vVel);
			}
		else if (CanBeControlled())
			ClipSpeed(0.0);
		}
	else if (m_pController->GetThrust())
		{
		CVector vAccel = PolarToVector(GetRotation(), GetThrust());

		AddForceLimited(vAccel);
		}
	else if (m_pController->GetStopThrust())
		{
		//	Stop thrust is proportional to main engine thrust and maneuverability

		Metric rManeuverAdj = Min((Metric)0.5, m_Perf.GetIntegralRotationDesc().GetManeuverRatio());
		Metric rThrust = rManeuverAdj * GetThrust();

		AccelerateStop(rThrust, rSecondsPerTick);
		}
	}

bool CShip::UpdateTriggerAllDevices (CDeviceClass::SActivateCtx &ActivateCtx, CShipUpdateSet &UpdateFlags)

//	UpdateTriggerAllDevices
//
//	Activates devices, if appropriate. Returns FALSE if the ship was destroyed.

	{
	for (CDeviceItem DeviceItem : GetDeviceSystem())
		{
		CInstalledDevice &Device = *DeviceItem.GetInstalledDevice();
		if (Device.IsTriggered() && Device.IsReady())
			{
			//	Initialize the target list, if necessary.
			//
			//	NOTE: The target list is initialized with options for 
			//	how to fill it, but we have not yet done the search of 
			//	targets in the system.
			//
			//	Inside the Activate method, a weapon will Realize() the
			//	target list if necessary.

			if (ActivateCtx.TargetList.IsEmpty())
				ActivateCtx.TargetList = GetTargetList();

			ActivateCtx.pTarget = m_pController->GetTarget(&DeviceItem);

			//	Fire

			bool bSuccess = Device.Activate(ActivateCtx);
			if (IsDestroyed())
				return false;

			//	If we succeeded then it means that the weapon fired successfully

			if (bSuccess)
				{
				if (ActivateCtx.bConsumedItems)
					{
					UpdateFlags.Set(CShipUpdateSet::weaponStatus);
					UpdateFlags.Set(CShipUpdateSet::cargo);
					}

				//	Remember the last time we fired a weapon

				if (Device.GetCategory() == itemcatWeapon || Device.GetCategory() == itemcatLauncher)
					m_iLastFireTime = GetUniverse().GetTicks();

				//  Set the delay for the next activation.

				if ((Device.GetSlotLinkedFireOptions() != 0) && Device.GetCycleFireSettings())
					SetFireDelayForCycleWeapons(Device);
				else
					SetFireDelay(&Device);
				}
			}
		}

	return true;
	}
