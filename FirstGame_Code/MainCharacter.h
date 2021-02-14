// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
		EMS_Normal UMETA(DisplayName = "Normal"),
		EMS_Sprinting UMETA(DisplayName = "Sprinting"),
		EMS_Dead UMETA(DisplayName = "Dead"),

		EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"), //ESS_Exhausted ���׹̳��� �ּҰ� 
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"), //ESS_ExhaustedRecovering ���׹̳� ȸ��

	ESS_MAX UMETA(DisplayName = "DefaultMAX")

};

UCLASS()
class FIRSTGAMES_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TSubclassOf<class AItemStorage> WeaponStorage;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bHasCombatTarget;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere,Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Combat")
	class USoundCue* HitSound;

	//TArray<FVector> PickupLocations;


	//UFUNCTION(BlueprintCallable)
	//void ShowPickupLocation();

	//  �������� ���� �Լ� 
	UPROPERTY(VisibleAnywhere,BlueprintReadwrite,Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Movement")
	float MinSprintStamina;

	float InterpSpeed;

	bool bInterpToEnemy;

	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	FRotator GetLookAtRotationYaw(FVector Target);

	// �ȿ����̴� �����϶�
	//Set Movement Status and running speed
	void SetMovementStatus(EMovementStatus Status);

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category ="Running")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	bool bShiftKeyDown;

	// Pressed down to enable sprinting
	void ShiftKeyDown();

	//Released to stop sprinting
	void ShiftKeyUp();

	//��𼭵� �����ְ� Camera ���� 
	// �������Ʈ���� �̸� �����Ͽ� �������Ʈ �б� ����� ī�װ��� ������������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess="true"))
	class USpringArmComponent* CameraBoom;

	// ������� ī�޶�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// ��ȸ��
	// ī�޶��� ȸ�� ����� Ȯ���ϱ����� �⺻ ȸ�� �ӵ�
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookupRate;

	/**
	/*
	/* Player Stats
	/*
	*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category ="Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Player Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Player Stats")
	float Stamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = "Player Stats")
	int32 coins;

	// ���ظ� �Ծ����� ���ҵǴ� ü�� 
	void DecrementHealth(float Amount);
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent,
								class AController * EventInstigator,AActor * DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	void Die();

	//virtual void Jump() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// �� �Ʒ� ������
	void MoveForward(float Value);

	// ���� ������ ������
	void MoveRight(float Value);

	// ȸ��
	void Turn(float Value);

	// ����
	void LookUp(float Value);


	bool bMovingForward;
	bool bMovingRight;

	bool CanMove(float Value);

	// �־��� �ӵ��� ȸ���ϱ� ���� �Է��� ���� ȣ��
	// @param Rate This is a  normalized rate, i.e ,1.0 means 100% of desired turn rate
	void TurnAtRate(float Rate);

	// �־��� �ӵ����� �� �Ʒ� ����Ű�� ã��(?)
	// Called via input to look up/down at a given rate
	// @param Rate This is a  normalized rate, i.e ,1.0 means 100% of desired look up/down rate
	void LookUpAtRate(float Rate);

	bool bLMBDown;
	void LMBDown();
	void LMBUp();

	bool bESCDown;
	void ESCDown();
	void ESCUp();

	void Teleport();
	void TeleportEnd();




	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items)
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	class AItem* ActiveOverlappingItem;

	void SetEquippedWeapon(AWeapon* WeaponToSet);
	FORCEINLINE AWeapon* GetEquippedWeapon(){ return EquippedWeapon; }
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Anims")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* TestMontage;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Combat")
	class UParticleSystem* TestParticles;


	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	void UpdateCombatTarget();

	UPROPERTY(EditAnywhere,BlueprintReadwrite , Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	//ȭ�� ��ȯ (������ȯ)
	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();
	 
	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	void LoadGameNoSwitch();

	// ���� �������� 
	FTimerHandle TeleportTimer;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Combat")
	float MinTime;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Combat")
	float MaxTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bTeleport;
	
};
