// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"


//���� ����
UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied			UMETA(DisplayName = "Unoccupied" ),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading			UMETA(DisplayName = "Reloading"),
	ECS_Equipping			UMETA(DisplayName = "Equipping"),
	ECS_Stunned				UMETA(DisplayName = "Stunned"),

	ECS_MAX					UMETA(DisplayName = "DefaultMax")
};

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

	//�������� ���� ��ġ�� ���� ��� ���� ���
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USceneComponent* SceneComponent;

	//�� ��� ���� ��� ��ġ�� �������ϴ� �׸� ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAnimation); // �ִϸ��̼� ��������� ��ε�ĳ��Ʈ 

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

	// �÷��̾ �Դ� ��������
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnRate(float Rate);

	// �־��� �ӵ��� ��/�Ʒ��� ��ȸ�ϱ� ���� �Է��� ���� ȣ���
	// @param Rate This is  a normalized rate
	void LookupRate(float Rate);

	// ���콺 X �����ӿ� ���� �ӵ� ��Ʈ�ѷ�
	// @param Value The input Value from mouse movement
	void Turn(float Value);

	// ���콺 Y �����ӿ� ����� �ӵ� ��Ʈ�ѷ�
	// @param Value The input Value from mouse movement
	void Lookup(float Value);

	void FireWeapon();

	// ��ư �������� bAiming true �Ǵ� false ����
	void AimingButtonPressed();
	void AimingButtonReleased();

	void CameraInterpZoom(float DeltaTime);

	void CalculateCrosshairSpread(float DeltaTime);
	

	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();

	// �Ѿ� �߻� Ÿ�̸Ӱ� �Ϸ�Ǿ����� �ҷ��� �Լ� 
	UFUNCTION()
	void AutoFireReset();

	// ���ؿ� ���� BaseTurnRate �� BaseLookupRate ����
	void SetLookupRates();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult&OutHitResult);

	// ���ڼ� �Ʒ� �׸� ���� �� ����
	bool TraceUnderCrosshair(FHitResult& OutHitResult,FVector& OutHitLocation);

	// OverlappedItemCount > 0�� ��� �׸� ����
	void TraceForItems();
	
	// ���� ������ �����ϴ� �Լ� 
	class AWeapon* SpawnDefaultWeapon();

	// ���⸦ ������ �޽��� ����
	void EquipWeapon(AWeapon* WeaponToEquip,bool bSwapping = false);

	// ���� ��ü ( ��� ���� ) 
	void DropWeapon();

	void SelectButtonPressed();
	void SelectButtonReleased();

	// ���� ��ü �Լ� 
	void SwapWeapon(AWeapon* WeaponToSwap);

	// ź�� ������ ź�� ���� �ʱ�ȭ
	void InitializeAmmoMap();

	// ���⿡ ź���� �ִ��� Ȯ���Ͻʽÿ�.
	bool WeaponHasAmmo();

	// 	FireWeapon�Լ�
	void PlayFireSound();
	void SendBullet();
	void PlayGunFireMontage();

	// Reload �Լ� 
	void ReloadButtonPressed();
	//���� ������ ó��
	void ReloadWeapon();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	// EquppedWeapon�� ź�� ���� ź���� �ִ��� Ȯ���մϴ�.
	bool CarringAmmo();

	// Grab Clip �˸��� �ִ� �ִϸ��̼� �������Ʈ���� ȣ���
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	// Release �˸��� �ִ� �ִϸ��̼� �������Ʈ���� ȣ�� 
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	void CrouchButtonPressed();

	virtual void Jump() override;

	// ��ũ���ų� �� ���� �� ĸ�� �߰� ���̸� ����ϴ�.
	void InterpCapsuleHalfHegiht(float DeltaTime);

	void Aim();
	void StopAiming();

	void PickupAmmo(class AAmmo* Ammo);

	void InitializeInterpLocation();

	void FKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();

	// ���� ��ü�� ���Ǵ� �Լ� 
	void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);

	int32 GetEmptyInventorySlot();
	
	void HighlightInventorySlot();

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();

	UFUNCTION(BlueprintCallable)
	void EndStun();
	
	void Die();

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	// ī�޶� �÷��̾� �������� ��ġ�ϰ� ���� 
	// meta =(AllowPrivateAccess = "true") �� �������־ ������ �����ϰ� ����
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Camera",meta =(AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// ĳ���͸� ������� ī�޶� 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// ĳ���� ȸ����
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Camera", meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseLookupRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* FireMontage;

	// �Ѿ� �浹�� �߻��ϴ� ����Ʈ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle;

	// �Ѿ� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticle;

	// true when aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	// �⺻ ī�޶� �þ� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float CameraDefaultFOV;

	// Ȯ���� ���� �þ� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFOV;

	// �� �������� ���� �þ�
	float CameraCurrentFOV;

	// ī�޶� �� �ӵ� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	// �������� ���� �� ȸ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;
	// �������� ���� �� ��ȸ��
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HipLookupRate;
	// ���� �� ȸ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;
	// ���� �� ��ȸ��
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AimingLookupRate;

	// �������� ���� �� ���콺 ��� ������ ���� ���� ��� ȸ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"),meta=(ClampMin = "0.0",ClampMax = "1.0",UIMin = "0.0",UIMax = "1.0"))
	float MouseHipTurnRate;
	// ���콺 ��� ������ ���� ���� ��� �������� ���� �� ��ȸ��
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookupRate;

	//���콺 ��� ������ ���� ���� ��� ���� �� ȸ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;
	// ���콺 ��� ������ ���� ���� ��� ���� �� ��ȸ��
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookupRate;

	// ���ڼ��� ������ ����
	UPROPERTY(VIsibleAnywhere,BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;
	// ���ڼ��� ���� �ӵ� ���� ���
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;
	// ���ڼ��� �������� ���
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;
	
	//���ڼ� ������ ���� ���� �������
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;
	// ���ڼ� ������ ���� �Կ� ���� ���
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	// ���� ���콺 ��ư�� ���ȴ��� Ȯ��
	bool bFireButtonPressed;

	// �Ѿ��� �߻���ڸ��� Ÿ�̸� ���� ���� 
	bool bShouldFire;
	
	float ShooterTimeDuration;

	bool bFiringBullet;
	
	// �Ѿ� �߻� ���� �ð� 
	FTimerHandle AutoFireTimer;

	FTimerHandle CrosshairShooterTimer;

	// �׸��� ��� �������� �����ؾ� �ϴ� ��� 
	bool bShouldTraceForItem;

	//�ߺ� �׸� ��
	int8 OverlappedItemCount;

	// �����ƿ� ���� 
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	class AItem* TraceHitItemLastFrame;

	// ���۽� �⺻ ���� 
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	// �⺻ ���� Ŭ���� 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	// ���� TraceForItem�� ������ ���� ���ߵ� �׸�
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	// ������ ����� ī�޶󿡼� �ٱ��� �Ÿ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Item", meta = (AllowPrivateAccess = "true"))
	float CameraInterpDistance;
	
	// ������ ����� ī�޶󿡼� ���� �Ÿ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;

	// �پ��� ź�� ������ ź���� �����ϴ� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	// 9mm ź�� ���۷�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;

	// AR ź�� ���۷�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;
	
	//���� ����, ��� �ִ� ��쿡�� �߻� �Ǵ� ������ ����
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* EquipMontage;

	//�ٽ� �ε��ϴ� ���� Ŭ���� ó�� ���� �� Ŭ���� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransforms;

	// �ٽ� �ε��ϴ� ���� ĳ������ �տ� ������ ��� ���� ���
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bCrouching;
	
	//��Ģ���� �̵� �ӵ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float BaseMovementSpeed;

	// ��ũ���� �̵� �ӵ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchMovementSpeed;

	// ĸ���� ���� ���� ����
	float CurrentCapsuleHalfHeight;

	// �ɾ����������� ĸ������ ���� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight;

	// �ɾ������� ĸ�� ���� ���� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchingCapsuleHalfHeight;

	//��ũ���� ���� ���� ���¿��� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float BaseGroundFriction;

	//��ũ���� �ִ� ���� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchingGroundFriction;

	// ���� ��ư�� ���ȴ��� �˱� ���� ���
	bool bAimingButtonPressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponInterpComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp6;

	//interp ��ġ ����ü�� �迭
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FInterpLocation> InterpLocation;

	FTimerHandle PickupSountTimer;
	FTimerHandle EquipSoundTimer;

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;

	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

	// �ٸ� �Ⱦ� ���带 ����ϱ� ���� ��ٸ� �ð��Դϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	float PickupSoundResetTime;

	// �ٸ� ���� ���带 ����ϱ� ���� ��ٸ� �ð��Դϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	float EquipSoundResetTime;

	// �κ��丮 �迭
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> Inventory;

	// �κ��丮 ��밡�� ���� 
	const int32 INVENTORY_CAPACITY{ 6 };

	// ���� �� �κ��丮�ٿ� ���� ������ �����ϴ� ��������Ʈ
	UPROPERTY(BlueprintAssignable,Category = Delefate, meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;

	// ���� �� �κ��丮�ٿ� ������ ������ �����ϴ� ��������Ʈ
	UPROPERTY(BlueprintAssignable, Category = Delefate, meta = (AllowPrivateAccess = "true"))
	FHighlightIconDelegate HighlightIconDelegate;

	// ������ ǥ���� �����ε��� 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 HighlightSlot;

	// �÷��̾� ü��
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	// �ǰݴ����� �� ���� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* MeleeImpactSound;

	// �ǰݽ� ���� ����Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BloodParticles;

	// ���� ��Ÿ�� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	// ������ �¾����� ����Ȯ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float StunChange;

	// ĳ���Ͱ� �׾������ 
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bDead;

public:

	// CameraBoom �� ���� Getter
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetAiming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	void IncrementOverlappedItemCount(int8 Amount);

	//FVector GetCameraInterpLocation();

	void GetPickupItem(AItem* Item);

	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE bool GetCrouching() const { return bCrouching; }
	FInterpLocation GetInterpLocation(int32 Index);

	//ItemCount�� ���� ���� InterpLocations �迭�� �ε����� ��ȯ�մϴ�.
	int32 GetInterpLocationIndex();

	void IncrementInterpLocItemCount(int32 Index, int32 Amount);

	FORCEINLINE bool ShouldPlayPickupSound() const {return bShouldPlayPickupSound;}
	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

	void StartPickupSoundTimer();
	void StartEquipSoundTimer();

	void UnHighlightInventorySlot();
	
	void Stun();

	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE USoundCue* GetMeleeImpactSound() const { return MeleeImpactSound; }
	FORCEINLINE UParticleSystem* GetBloodParticles() const { return BloodParticles; }

	FORCEINLINE float GetStunChance() const { return StunChange; }

};
