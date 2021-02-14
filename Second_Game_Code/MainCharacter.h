// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName ="Dead")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering	UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX	UMETA(DisplayName = "DefaultMax"),
};

UCLASS()
class OPENRPG_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class ABossPlayerController* BossPlayerController;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Combat")
	class UBoxComponent* LeftHandCombatCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Combat")
	class UBoxComponent* RightHandCombatCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Combat")
	class UBoxComponent* LeftTobaseCombatCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Combat")
	class UBoxComponent* RightTobaseCombatCollision;

	// �÷��̾ �ǰ��� �������� ���´� ����Ʈ 
	UPROPERTY(EditAnywhere,BlueprintReadwrite,Category = "Combat")
	class UParticleSystem* PlayerHitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Combat")
	class UParticleSystem* BossPlayerHitParticles;

	// ���Ͱ� �÷��̾� ������ ���� 
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Combat")
	class USoundCue* EnemyHitSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category ="Combat")
	class AEnemy* CombatTarget;

	// ���� ���ͷ� Ÿ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class ABossEnemy* BossEnemyCombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookupRate;

	// PlayerStat
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Player Stat")
	float MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Player Stat")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stat")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Player Stat")
	float Stamina;

	UPROPERTY(EditAnywhere,BlueprintReadwrite,Category = "Player Stat")
	float PlayerExp;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Player Stat")
	float PlayerMaxExp;

	

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "PlayerStat")
	int32 PlayerLevel;

	int ComboAttack_num;

	UPROPERTY(EditDefaultsOnly,Category = Attack)
	UAnimMontage* AttackMeleeAnim;

	UPROPERTY(EditDefaultsOnly, Category = Attack)
	UAnimMontage* AttackMeleeAnimCombo;

	UPROPERTY(EditDefaultsOnly, Category = Attack)
	UAnimMontage* AttackMeleeAnimCombo2;

	UPROPERTY(EditDefaultsOnly, Category = Attack)
	UAnimMontage* AttackCombo;
	
	UPROPERTY(EditDefaultsOnly, Category = Attack)
	UAnimMontage* LastAttackCombo;

	UPROPERTY(EditDefaultsOnly, Category = Attack)
	UAnimMontage* SkillAttack;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "AI")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "AI")
	float SkillDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	bool bHaseSkillHit;
	
	//Skill Key �������� 
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Combat")
	bool bSkillDown;
	
	// ĳ���Ͱ� ù��° ����� ����ߴ��� Ȯ�� 
	UPROPERTY(EditAnywhere,BlueprintReadwrite, Category = "Abliltys")
	bool hasUsedAblilty1;
	
	// ĳ���Ͱ� �ι�° ����� ����ߴ��� Ȯ�� 
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Abliltys")
	bool hasUsedAblilty2;

	//ĳ���Ͱ� ù��° ��� ��� �ð� 
	float ablilty1Duration;

	float ablilty2Duration;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Abliltys")
	float ablilty1ColldownTime;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Abliltys")
	float ablilty2ColldownTime;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Abliltys")
	FTimerHandle ablilty1TimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Abliltys")
	FTimerHandle ablilty2TimerHandle;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Rate);
	
	void LookUpAtRate(float Rate);

	void AttackMelee();

	void DecrementHealth(float Amount);

	void Die();

	void PlayerExpBar();

	void PlayerLevelText(int32 Level);

	void SkillKeyDown();

	void SkillKeyDown_two();

	// ��ų �� ��������� �ð��� �� Ÿ�� ���
	void ResetAblilty1();

	void ResetAblilty2();

	//��ų ��Ÿ���� �ٵǾ����� ���� ���� ��ȯ
	void Ablilty1CooldownComplete();

	void Ablilty2CooldownComplete();

	//��ų Ű �� Ŭ���̾ȵǾ����� True �����ֱ⋚���� ó������ False �� �ۼ�
	FORCEINLINE void SkillKeyup() { bSkillDown = false; }
	FORCEINLINE void SkillKeyup_two() { bSkillDown = false; }

	FORCEINLINE void SetMovementStatus(EMovementStatus Status) { MovementStatus = Status; }

	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const & DamageEvent,class AController * EventInstigator,AActor * DamageCauser) override;

	//virtual void Jump() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	UPROPERTY(VisibleAnywhere,BlueprintReadwrite,Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Combat")
	FVector BossCombatTargetLocation;

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	UFUNCTION(BlueprintCallable)
	void ActivateCollisiontwo();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollisiontwo();

	UFUNCTION(BlueprintCallable)
	void ActivateCollisionthree();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollisionthree();

	UFUNCTION(BlueprintCallable)
	void LastActivateCollision();

	UFUNCTION(BlueprintCallable)
	void LastDeactivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	UFUNCTION(BlueprintCallable)
	void AttackMeleeEnd();

	UFUNCTION(BlueprintCallable)
	void SkillAttack_Brake();

	UFUNCTION(BlueprintCallable)
	void SkillAttack_BrakeEnd();

	UFUNCTION(BlueprintCallable)
	void SkillAttack_Dance();

	UFUNCTION(BlueprintCallable)
	void SkillAttack_DanceEnd();
	

	//void LMBDown();
	//void LMBup() { bLMBDown = false; }
	//bool bLMBDown;

	// ���� ���� ����ġ 
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Anims")
	bool isDuringAttack;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	bool bHasCombatTarget;

	// ���� ���� true/false
	//bool CanSetWeapon();

	// ������ ���� (�����Ҷ� ���� ������ ��) 
	float InterpSpeed;

	UPROPERTY(EditAnywhere,BlueprintReadwrite,Category = "movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "movement")
	float MinSprintStamina;

	bool bInterpToEnemy;

	bool bShiftKeyDown;

	void SetInterpToEnemy(bool Interp);

	FRotator GetLookAtRotaionYaw(FVector Target);

	FRotator GetBossLookAtRotaionYaw(FVector BossTarget);

	//void SetWeapon(class AWeaponKatana* NewWeapon);

	//���� ����(?)
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = items)
	//class AWeaponKatana* Equippedweapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = items)
	class Aitem* ActiveOverlappingItem;

	//������ ���� ����
	//void SetEquippedWeapon(AWeaponKatana* WeaponToSet);
	//FORCEINLINE AWeaponKatana* GetEquippedWeapon() { return Equippedweapon; }

	//���� ��ü ����
	FORCEINLINE void SetActiveOverlapping(Aitem* item) { ActiveOverlappingItem = item; }

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera ; }

	// �� ���� ���� 
	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }
	FORCEINLINE void SetBossCombatTarget(ABossEnemy* BossTarget) { BossEnemyCombatTarget = BossTarget; }

	UFUNCTION()
	virtual void PlayerCombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void PlayerCombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(EditAnywhere,BlueprintReadwrite,Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Combat")
	TSubclassOf<UDamageType> BossDamageTypeClass;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Combat")
	AController* PauchInstigator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AController* BossPauchInstigator;

	FORCEINLINE void SetInstigator(AController* Inst) { PauchInstigator = Inst; }

	FORCEINLINE void SetBossInstigator(AController* BossInst) { BossPauchInstigator = BossInst; }

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	FORCEINLINE void SetHasBossCombatTarget(bool BossHasTarget) { bHasCombatTarget = BossHasTarget; }

};
