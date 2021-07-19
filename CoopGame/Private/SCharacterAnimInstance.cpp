// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SCharacter.h"

void USCharacterAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			SCharacter = Cast<ASCharacter>(Pawn);
		}
	}
}

void USCharacterAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();

	}
	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LaterealSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LaterealSpeed.Size();

		bCrouch = Pawn->GetMovementComponent()->IsCrouching();
		bIsAir = Pawn->GetMovementComponent()->IsFalling();

		if (SCharacter == nullptr)
		{
			SCharacter = Cast<ASCharacter>(Pawn);
		}
	}
}