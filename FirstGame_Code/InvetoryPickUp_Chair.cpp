// Fill out your copyright notice in the Description page of Project Settings.

#include "InvetoryPickUp_Chair.h"
#include "Engine/Engine.h"

AInvetoryPickUp_Chair::AInvetoryPickUp_Chair()
{

}

void AInvetoryPickUp_Chair::Use_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HooBy!"));
}
