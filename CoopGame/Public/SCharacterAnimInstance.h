// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API USCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
		virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
		void UpdateAnimationProperties();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		bool bIsAir;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		bool bCrouch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		class APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		class ASCharacter* SCharacter;
};
