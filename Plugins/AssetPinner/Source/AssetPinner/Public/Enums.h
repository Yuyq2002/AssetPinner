// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

UENUM()
enum class EPathType : uint8
{
	None,
	Asset,
	Folder
};

UENUM(BlueprintType)
enum class ERotation : uint8
{
	Clockwise,
	CounterClockwise,
	Horizontal
};