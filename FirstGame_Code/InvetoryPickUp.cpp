// Fill out your copyright notice in the Description page of Project Settings.

#include "InvetoryPickUp.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Interactable.h"
#include "Components/StaticMeshComponent.h"
#include "GenericPlatform/GenericPlatform.h"
#pragma once

// ���� ����¾���

AInvetoryPickUp::AInvetoryPickUp()
{
	// ��ȣ�ۿ��� ���� ���򸻰� ������ �̸����� ��Ÿ���� �����
	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InvetoryPickup"));
	InteractableMesh->SetSimulatePhysics(true);

	// �׸��� ��Ÿ��
	//ItemName = FString(TEXT("Enter an Item name here"));
	//InteractableHelpText = FString(TEXT("���� ����"));
	CoinValue = 0;
}

void AInvetoryPickUp::BeginPlay()
{
	// ������ �̸��� ��Ÿ�� 
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
	//GLog->Log("��ȣ�ۿ�");
}

void AInvetoryPickUp::OnPickUp()
{
	InteractableMesh->SetVisibility(false);
	InteractableMesh->SetSimulatePhysics(false);
	InteractableMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
