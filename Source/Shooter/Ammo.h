// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API AAmmo : public AItem
{
	GENERATED_BODY()
public:
	AAmmo();
	virtual void Tick(float DeltaTime) override;


protected:

	virtual void BeginPlay() override;
	// �̹� ������ �׸��� �� ���� �Ͽ� ź��޽� ����  
	virtual void SetItemProperties(EItemState State) override;

	UFUNCTION()
		void AmmoSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


private:
	
	// ź�� Mesh 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AmmoMesh;

	// ź�� ���� 
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	// ź�� ������ �ؽ�ó
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIconTexture;

	// ź�� ��ħ ��ü 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AmmoCollisionSphere;

public:
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }


	virtual void EnableCusomDepth() override;
	virtual void DisableCustomDepth() override;

};
