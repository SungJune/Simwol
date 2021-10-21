// Fill out your copyright notice in the Description page of Project Settings.

#include "InvetoryPickUp.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Interactable.h"
#include "Components/StaticMeshComponent.h"
#include "GenericPlatform/GenericPlatform.h"
#pragma once

// 아직 빌드는안함

AInvetoryPickUp::AInvetoryPickUp()
{
	// 상호작용을 위한 도움말과 아이템 이름등을 나타내는 도우미
	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InvetoryPickup"));
	InteractableMesh->SetSimulatePhysics(true);

	// 항목을 나타냄
	//ItemName = FString(TEXT("Enter an Item name here"));
	//InteractableHelpText = FString(TEXT("습득 가능"));
	CoinValue = 0;
}

void AInvetoryPickUp::BeginPlay()
{
	// 아이템 이름을 나타냄 
	InteractableHelpText = FString::Printf(TEXT(" %s: Press E to pick up "), *ItemName);
}

void AInvetoryPickUp::Interact_Implementation()
{
	AMainCharacter* character = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (character->AddItemToInventory(this))
	{
		OnPickUp();
	}

	
}

void AInvetoryPickUp::Use_Implementation()
{
	//GLog->Log("상호작용");
}

void AInvetoryPickUp::OnPickUp()
{
	InteractableMesh->SetVisibility(false);
	InteractableMesh->SetSimulatePhysics(false);
	InteractableMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
