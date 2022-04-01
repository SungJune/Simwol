// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"


//전투 변수
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

	//인터핑을 위한 위치에 대한 장면 구성 요소
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USceneComponent* SceneComponent;

	//이 장면 구성 요소 위치에 인터핑하는 항목 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAnimation); // 애니메이션 재생을위한 브로드캐스트 

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

	// 플레이어가 입는 전투피해
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnRate(float Rate);

	// 주어진 속도로 위/아래를 조회하기 위해 입력을 통해 호출됨
	// @param Rate This is  a normalized rate
	void LookupRate(float Rate);

	// 마우스 X 움직임에 따른 속도 컨트롤러
	// @param Value The input Value from mouse movement
	void Turn(float Value);

	// 마우스 Y 움직임에 기반한 속도 컨트롤러
	// @param Value The input Value from mouse movement
	void Lookup(float Value);

	void FireWeapon();

	// 버튼 누름으로 bAiming true 또는 false 설정
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

	// 총알 발사 타이머가 완료되었을때 불러올 함수 
	UFUNCTION()
	void AutoFireReset();

	// 조준에 따라 BaseTurnRate 및 BaseLookupRate 설정
	void SetLookupRates();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult&OutHitResult);

	// 십자선 아래 항목에 대한 선 추적
	bool TraceUnderCrosshair(FHitResult& OutHitResult,FVector& OutHitLocation);

	// OverlappedItemCount > 0인 경우 항목 추적
	void TraceForItems();
	
	// 무기 생성후 장착하는 함수 
	class AWeapon* SpawnDefaultWeapon();

	// 무기를 가져와 메쉬에 부착
	void EquipWeapon(AWeapon* WeaponToEquip,bool bSwapping = false);

	// 무기 교체 ( 드롭 형식 ) 
	void DropWeapon();

	void SelectButtonPressed();
	void SelectButtonReleased();

	// 무기 교체 함수 
	void SwapWeapon(AWeapon* WeaponToSwap);

	// 탄약 값으로 탄약 지도 초기화
	void InitializeAmmoMap();

	// 무기에 탄약이 있는지 확인하십시오.
	bool WeaponHasAmmo();

	// 	FireWeapon함수
	void PlayFireSound();
	void SendBullet();
	void PlayGunFireMontage();

	// Reload 함수 
	void ReloadButtonPressed();
	//무기 재장전 처리
	void ReloadWeapon();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	// EquppedWeapon의 탄약 유형 탄약이 있는지 확인합니다.
	bool CarringAmmo();

	// Grab Clip 알림이 있는 애니메이션 블루프린트에서 호출됨
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	// Release 알림이 있는 애니메이션 블루프린트에서 호출 
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	void CrouchButtonPressed();

	virtual void Jump() override;

	// 웅크리거나 서 있을 때 캡슐 중간 높이를 끼웁니다.
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

	// 무기 교체시 사용되는 함수 
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

	// 카메라를 플레이어 뒤쪽으로 위치하게 해줌 
	// meta =(AllowPrivateAccess = "true") 는 숨겨져있어도 노출이 가능하게 만듬
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Camera",meta =(AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// 캐릭터를 따라오는 카메라 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// 캐릭터 회전률
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Camera", meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float BaseLookupRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* FireMontage;

	// 총알 충돌시 발생하는 임팩트 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle;

	// 총알 연기 흔적
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticle;

	// true when aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	// 기본 카메라 시야 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float CameraDefaultFOV;

	// 확대할 때의 시야 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFOV;

	// 이 프레임의 현재 시야
	float CameraCurrentFOV;

	// 카메라 줌 속도 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	// 조준하지 않을 때 회전율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HipTurnRate;
	// 조준하지 않을 때 조회율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HipLookupRate;
	// 조준 시 회전율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate;
	// 조준 시 조회율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float AimingLookupRate;

	// 조준하지 않을 때 마우스 모양 감도에 대한 배율 요소 회전율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"),meta=(ClampMin = "0.0",ClampMax = "1.0",UIMin = "0.0",UIMax = "1.0"))
	float MouseHipTurnRate;
	// 마우스 모양 감도에 대한 배율 요소 조준하지 않을 때 조회율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookupRate;

	//마우스 모양 감도에 대한 배율 요소 조준 시 회전율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate;
	// 마우스 모양 감도에 대한 배율 요소 조준 시 조회율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookupRate;

	// 십자선의 퍼짐을 결정
	UPROPERTY(VIsibleAnywhere,BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;
	// 십자선의 퍼짐 속도 구성 요소
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;
	// 십자선용 퍼짐구성 요소
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;
	
	//십자선 퍼짐을 위한 조준 구성요소
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor;
	// 십자선 퍼짐을 위한 촬영 구성 요소
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;

	// 왼쪽 마우스 버튼이 눌렸는지 확인
	bool bFireButtonPressed;

	// 총알이 발사되자마자 타이머 시작 여부 
	bool bShouldFire;
	
	float ShooterTimeDuration;

	bool bFiringBullet;
	
	// 총알 발사 사이 시간 
	FTimerHandle AutoFireTimer;

	FTimerHandle CrosshairShooterTimer;

	// 항목의 모든 프레임을 추적해야 하는 경우 
	bool bShouldTraceForItem;

	//중복 항목 수
	int8 OverlappedItemCount;

	// 아이탬에 적중 
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	class AItem* TraceHitItemLastFrame;

	// 시작시 기본 무기 
	UPROPERTY(VIsibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	// 기본 무기 클래스 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	// 현재 TraceForItem의 추적에 의해 적중된 항목
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	// 인터프 대상의 카메라에서 바깥쪽 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Item", meta = (AllowPrivateAccess = "true"))
	float CameraInterpDistance;
	
	// 인터프 대상의 카메라에서 위쪽 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;

	// 다양한 탄약 유형의 탄약을 추적하는 지도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	// 9mm 탄약 시작량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;

	// AR 탄약 시작량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;
	
	//전투 상태, 비어 있는 경우에만 발사 또는 재장전 가능
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* EquipMontage;

	//다시 로드하는 동안 클립을 처음 잡을 때 클립의 변형
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransforms;

	// 다시 로드하는 동안 캐릭터의 손에 부착할 장면 구성 요소
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bCrouching;
	
	//규칙적인 이동 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float BaseMovementSpeed;

	// 웅크리기 이동 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchMovementSpeed;

	// 캡슐의 현재 절반 높이
	float CurrentCapsuleHalfHeight;

	// 앉아있지않을때 캡슐절반 높이 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight;

	// 앉아있을때 캡슐 절반 높이 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchingCapsuleHalfHeight;

	//웅크리고 있지 않은 상태에서 지면 마찰
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float BaseGroundFriction;

	//웅크리고 있는 동안 지면 마찰
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchingGroundFriction;

	// 조준 버튼이 눌렸는지 알기 위해 사용
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

	//interp 위치 구조체의 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FInterpLocation> InterpLocation;

	FTimerHandle PickupSountTimer;
	FTimerHandle EquipSoundTimer;

	bool bShouldPlayPickupSound;
	bool bShouldPlayEquipSound;

	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

	// 다른 픽업 사운드를 재생하기 전에 기다릴 시간입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	float PickupSoundResetTime;

	// 다른 장착 사운드를 재생하기 전에 기다릴 시간입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	float EquipSoundResetTime;

	// 인벤토리 배열
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> Inventory;

	// 인벤토리 사용가능 갯수 
	const int32 INVENTORY_CAPACITY{ 6 };

	// 장착 시 인벤토리바에 슬롯 정보를 전송하는 델리게이트
	UPROPERTY(BlueprintAssignable,Category = Delefate, meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;

	// 장착 시 인벤토리바에 아이콘 정보를 전송하는 델리게이트
	UPROPERTY(BlueprintAssignable, Category = Delefate, meta = (AllowPrivateAccess = "true"))
	FHighlightIconDelegate HighlightIconDelegate;

	// 강조된 표시의 슬롯인덱스 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 HighlightSlot;

	// 플레이어 체력
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	// 피격당했을 때 사운드 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* MeleeImpactSound;

	// 피격시 나올 임팩트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BloodParticles;

	// 기절 몽타주 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	// 적에게 맞았을때 스턴확률
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float StunChange;

	// 캐릭터가 죽었을경우 
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bDead;

public:

	// CameraBoom 에 대한 Getter
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

	//ItemCount가 가장 낮은 InterpLocations 배열의 인덱스를 반환합니다.
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
