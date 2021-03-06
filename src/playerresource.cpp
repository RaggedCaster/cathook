/*
 * playerresource.cpp
 *
 *  Created on: Nov 13, 2016
 *      Author: nullifiedcat
 */

#include "common.h"

void TFPlayerResource::Update() {
	IClientEntity *ent;

	entity = 0;
	for (int i = 0; i < HIGHEST_ENTITY; i++) {
		ent = g_IEntityList->GetClientEntity(i);
		if (ent && ent->GetClientClass()->m_ClassID == RCC_PLAYERRESOURCE) {
			entity = i;
			return;
		}
	}
}

int TFPlayerResource::GetMaxHealth(CachedEntity* player) {
	IClientEntity *ent;
	int idx;
	/* :thinking */
	IF_GAME (!IsTF()) return 100;
	ent = g_IEntityList->GetClientEntity(entity);
	if (!ent || ent->GetClientClass()->m_ClassID != RCC_PLAYERRESOURCE) return 0;
	idx = player->m_IDX;
	if (idx >= 64 || idx < 0) return 0;
	return *(int*)((unsigned)ent + netvar.res_iMaxHealth + 4 * idx);
}

int TFPlayerResource::GetMaxBuffedHealth(CachedEntity* player) {
	IClientEntity *ent;
	int idx;

	IF_GAME (!IsTF()) return GetMaxHealth(player);
	ent = g_IEntityList->GetClientEntity(entity);
	if (!ent || ent->GetClientClass()->m_ClassID != RCC_PLAYERRESOURCE) return 0;
	idx = player->m_IDX;
	if (idx >= 64 || idx < 0) return 0;
	return *(int*)((unsigned)ent + netvar.res_iMaxBuffedHealth + 4 * idx);
}

int TFPlayerResource::GetTeam(int idx) {
	IClientEntity *ent;

	if (idx >= 64 || idx < 0) return 0;
	ent = g_IEntityList->GetClientEntity(entity);
	if (!ent || ent->GetClientClass()->m_ClassID != RCC_PLAYERRESOURCE) return 0;
	return *(int*)((unsigned)ent + netvar.res_iTeam + 4 * idx);
}

int TFPlayerResource::GetClass(CachedEntity* player) {
	IClientEntity *ent;
	int idx;

	ent = g_IEntityList->GetClientEntity(entity);
	if (!ent || ent->GetClientClass()->m_ClassID != RCC_PLAYERRESOURCE) return 0;
	idx = player->m_IDX;
	if (idx >= 64 || idx < 0) return 0;
	return *(int*)((unsigned)ent + netvar.res_iPlayerClass + 4 * idx);
}


TFPlayerResource* g_pPlayerResource = 0;
