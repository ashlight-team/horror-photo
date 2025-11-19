// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDefinition.h"

bool FItemStats::operator==(const FItemStats& ItemStats) const
{
	return Weight == ItemStats.Weight &&
		   Durability == ItemStats.Durability &&
		   Capacity == ItemStats.Capacity &&
		   Damage == ItemStats.Damage;
}
