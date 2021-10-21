// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable.h"


// Sets default values
AInteractable::AInteractable()
{
	InteractableHelpText = FString(" E 키를 눌러 상호작용이 가능합니다.");

}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}



void AInteractable::Interact_Implementation()
{

}

