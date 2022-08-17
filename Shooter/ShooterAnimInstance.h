// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponType.h"
#include "ShooterAnimInstance.generated.h"


UENUM(BlueprintType)
enum class EOffsetState :uint8
{
	EOS_Aiming		UMETA(DisplayName ="Aiming"),
	EOS_Hip			UMETA(DisplayName = "Hip"),
	EOS_Reloading	UMETA(DisplayName = "Reloading"),
	EOS_InAir		UMETA(DisplayName = "InAir"),
	

	EOS_MAX			UMETA(DisplayName = "DefaultMax")
};

/**
 * 
 */
UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UShooterAnimInstance();

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

protected:

	//제자리 회전 처리 변수
	void TurnInPlace();

	// 달리는 동안 기울기 계산 처리
	void Lean(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Movement, meta=(AllowPrivateAccess= "true"))
	class AShooterCharacter* shooterCharacter;

	// 캐릭터 스피드 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	float Speed;

	// 캐릭터 가 점프중인지 아닌지 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsInAir;

	// 캐릭터가 움직이고있는지 판별 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	// 스트래핑에 사용되는 오프셋 
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	// 우리가 움직임을 멈추기 전에 프레임을 오프셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	// 이 프레임의 캐릭터의 요 , 공중에 있지 않고 가만히 있을 때만 업데이트
	float TIPCharacterYaw;

	// 캐릭터의 요(Yaw) 이전 프레임 ,공중에 있지 않고 가만히 있을 때만 업데이트
	float TIPCharacterYawLastFram;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;

	// 이 프레임의 회전 곡선 값
	float RotationCurve;

	//회전 곡선 값 마지막 프레임
	float RotationCurveLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Pitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bReloading;

	// 어떤 조준상태를 사용할지 결정되는 오프셋
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState;

	// 이 프레임의 캐릭터의 요
	FRotator CharacterRotation;

	// 캐릭터의 요(Yaw) 이전 프레임
	FRotator CharacterRotationLastFrame;

	// 실행 중인 블렌드스페이스에서 기울이는 데 사용되는 요 델타
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category  = Lean, meta = (AllowPrivateAccess = "true"))
	float YawDelta;

	// 버튼이 눌리면 앉음 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouching, meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	// 버튼이 눌리면 교체 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouching, meta = (AllowPrivateAccess = "true"))
	bool bEquipping;

	// 제자리 선회와 조준의 따라 반동무게 를변경
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float RecoilWeight;

	// 제자리 회전 true/false
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bTurningInPlace;

	// 현재 장착중인 상태의 무기 유형
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	EWeaponType EquippedWeaponType;

	// 재장전하거나 장비하지않을때
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bShouldUseFABRIK;
};
