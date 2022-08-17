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

	//���ڸ� ȸ�� ó�� ����
	void TurnInPlace();

	// �޸��� ���� ���� ��� ó��
	void Lean(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Movement, meta=(AllowPrivateAccess= "true"))
	class AShooterCharacter* shooterCharacter;

	// ĳ���� ���ǵ� 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	float Speed;

	// ĳ���� �� ���������� �ƴ��� 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsInAir;

	// ĳ���Ͱ� �����̰��ִ��� �Ǻ� 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	// ��Ʈ���ο� ���Ǵ� ������ 
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	// �츮�� �������� ���߱� ���� �������� ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	// �� �������� ĳ������ �� , ���߿� ���� �ʰ� ������ ���� ���� ������Ʈ
	float TIPCharacterYaw;

	// ĳ������ ��(Yaw) ���� ������ ,���߿� ���� �ʰ� ������ ���� ���� ������Ʈ
	float TIPCharacterYawLastFram;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;

	// �� �������� ȸ�� � ��
	float RotationCurve;

	//ȸ�� � �� ������ ������
	float RotationCurveLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Pitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bReloading;

	// � ���ػ��¸� ������� �����Ǵ� ������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState;

	// �� �������� ĳ������ ��
	FRotator CharacterRotation;

	// ĳ������ ��(Yaw) ���� ������
	FRotator CharacterRotationLastFrame;

	// ���� ���� ���彺���̽����� ����̴� �� ���Ǵ� �� ��Ÿ
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category  = Lean, meta = (AllowPrivateAccess = "true"))
	float YawDelta;

	// ��ư�� ������ ���� 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouching, meta = (AllowPrivateAccess = "true"))
	bool bCrouching;

	// ��ư�� ������ ��ü 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouching, meta = (AllowPrivateAccess = "true"))
	bool bEquipping;

	// ���ڸ� ��ȸ�� ������ ���� �ݵ����� ������
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float RecoilWeight;

	// ���ڸ� ȸ�� true/false
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bTurningInPlace;

	// ���� �������� ������ ���� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	EWeaponType EquippedWeaponType;

	// �������ϰų� �������������
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bShouldUseFABRIK;
};
