// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvetoryPickUp.h"
#include "InvetoryPickUp_Chair.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAMES_API AInvetoryPickUp_Chair : public AInvetoryPickUp
{
	GENERATED_BODY()
public:
	AInvetoryPickUp_Chair();

	virtual void Use_Implementation() override;
	
	
	
	
};
