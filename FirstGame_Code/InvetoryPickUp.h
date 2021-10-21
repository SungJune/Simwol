// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "InvetoryPickUp.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAMES_API AInvetoryPickUp : public AInteractable
{
	GENERATED_BODY()
	
public:
	AInvetoryPickUp();

	virtual void BeginPlay() override;

	virtual void Interact_Implementation() override;

	UFUNCTION(BlueprintNativeEvent)
		void Use();
	virtual void Use_Implementation();

	UPROPERTY(EditAnywhere, Category = "Pickup Properties")
	UTexture2D* PickupThumbnail;

	UPROPERTY(EditAnywhere, Category = "Pickup Properties")
	FString ItemName;

	UPROPERTY(EditAnywhere, Category = "Pickup Properties")
	int32 CoinValue;

	// 아이템을 픽업할때 호출 
	void OnPickUp();
	
	
	
};
