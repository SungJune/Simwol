// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

//단일 히트스캔 무기 라인트레이스의 정보를 포함합니다.
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()
public:

	UPROPERTY()
	FVector_NetQuantize TraceFrom;

	UPROPERTY()
	FVector_NetQuantize TraceTo;

};


UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Comp")
	class USkeletalMeshComponent* MeshComps;

	void PlayFireEffects(FVector TraceEnd);

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Weapon")
	class TSubclassOf<UDamageType> DamageType;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TraceTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* DefaultsImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* TraceEffect;

	UPROPERTY(EditDefaultsOnly,Category = "Weapon")
	class TSubclassOf<UMatineeCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

	//UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire();

	//UFUNCTION(Server,Reliable,WithValidation)
	UFUNCTION(Server, Reliable ,WithValidation, Client)
	void ServerFire();

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastFireTime;

	// 발사되는 분당총알 
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	// Drived from RateOfFire
	float TimeBetweenShots;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;


public:	
	
	void StartFire();

	void StopFire();
};
