// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USHealthComponent;

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void MoveForward(float Value);
	void MoveRight(float Value);

	void BeginCrouch();
	void EndCrouch();

	void Jump();

	// 회전
	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Component")
	UCameraComponent* CameraComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* AnimAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* DeathAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	USHealthComponent* HealthComps;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookupRate;

	//줌기능
	bool bMantsToZoom; // 줌 상태 yes/no
	
	UPROPERTY(EditDefaultsOnly,Category = "Player")
	float ZoomedFOV; // 줌 이된 상태

	float DefaultFOV; // 줌이 아닌 상태

	UPROPERTY(EditDefaultsOnly, Category = "Player",meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	void BegineZoom();

	void EndZoom();

	UPROPERTY(Replicated)
	class ASWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category ="Player")
	TSubclassOf<ASWeapon> StartWeaponClass;

	UPROPERTY(VisibleDefaultsOnly,Category ="Player")
	FName WeaponAttachSocketName;

	void StartFire();
	void StopFire();

	void DIe();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta,
		const class UDamageType* DamageType ,class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	// 죽었는지 안죽었는지 확인 
	UPROPERTY(BlueprintReadOnly,Category = "Player")
	bool bDied;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

};
