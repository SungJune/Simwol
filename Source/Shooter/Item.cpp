// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Curves/CurveVector.h"


// Sets default values
AItem::AItem() :
	ItemName(FString("Default")),
	ItemCount(0),
	ItemRarity(EItemRarity::EIR_Common),
	ItemState(EItemState::EIS_Pickup),
	//�׸� Interp ����
	ZCurveTime(0.7f),
	ItemInterpStartLocation(FVector(0.f)),
	CameraTargetLocation(FVector(0.f)),
	bInterping(false),
	ItemInterpX(0.f),
	ItemInterpY(0.f),
	InterpInitialYawOffset(0.f)	,
	ItemType(EItemType::EIT_MAX),
	InterpLocIndex(0),
	MaterialIndex(0),
	bCanChangeCustomDepth(true),
	//���� ��� �Ű�����
	GlowAmount(150.f),
	FresnelExponent(3.f),
	FresnelReflectFraction(4.f),
	PulseCurveTime(5.f)	,
	SlotIndex(0),
	bCharacterInventoryFull(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);//�浹 ä�� ����
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (PickupWidget)
	{
		// ������ ������ ���� 
		PickupWidget->SetVisibility(false);
	}
	
	//������ ��͵��� ���� ActivaStarts �迭�� �����մϴ�.
	SetActivateStars();

	// AreaSphere�� ���� ���� ��ħ
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	//ItemState�� ������� �׸� �Ӽ� ����
	SetItemProperties(ItemState);

	//����� ���� ���̸� ��Ȱ��ȭ�� ����
	InitializeCustomDepth();

	StartPulseTimer();
}

void AItem::OnSphereOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharactor = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharactor)
		{
			ShooterCharactor->IncrementOverlappedItemCount(1);
		}
	}
}
void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent*OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharactor = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharactor)
		{
			ShooterCharactor->IncrementOverlappedItemCount(-1);
			ShooterCharactor->UnHighlightInventorySlot();
		}
	}
}

void AItem::SetActivateStars()
{
	for (int32 i = 0; i <= 5; i++)
	{
		ActiveStars.Add(false);
	}
	
	switch (ItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		ActiveStars[1] = true;
		break;

	case EItemRarity::EIR_Common:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;

	case EItemRarity::EIR_UnCommon:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		break;

	case EItemRarity::EIR_Rare:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;

	case EItemRarity::EIR_Legendary:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		ActiveStars[5] = true;
		break;

	default:
		break;
	}
}

void AItem::SetItemProperties(EItemState State)
{
	
	switch (State)
	{
		
	case EItemState::EIS_Pickup:
		//�޽� �Ӽ� ����
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// AreaSphere �Ӽ� ����
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		
		// CollisionBox �Ӽ� ����
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EItemState::EIS_Equipped: // ����
		// �޽� �Ӽ� ����
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// AreaSphere �Ӽ� ����
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// CollisionBox �Ӽ� ����
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_Falling:
		// �޽� �Ӽ� ����
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		// AreaSphere �Ӽ� ����
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// CollisionBox �Ӽ� ����
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_EquipInterping:
		// �޽� �Ӽ� ����
		PickupWidget->SetVisibility(false);
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// AreaSphere �Ӽ� ����
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// CollisionBox �Ӽ� ����
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_PickedUp:
		// �޽� �Ӽ� ����
		PickupWidget->SetVisibility(false);
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// AreaSphere �Ӽ� ����
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// CollisionBox �Ӽ� ����
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_MAX:
		break;
	default:
		break;
	}

}

FVector AItem::GetInterpLocation()
{
	if (Character == nullptr) return FVector(0.f);

	switch (ItemType)
	{
	case EItemType::EIT_Ammo:
		return Character->GetInterpLocation(InterpLocIndex).SceneComponent->GetComponentLocation();
		break;

	case EItemType::EIT_Weapon:
		return Character->GetInterpLocation(0).SceneComponent->GetComponentLocation();
		break;
	}

	return FVector();
}

void AItem::PlayPickupSound(bool bForcePlaySound)
{
	if (Character)
	{
		if (bForcePlaySound)
		{
			if (PickupSound)
			{
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}

		else if (Character->ShouldPlayPickupSound())
		{
			
			Character->StartPickupSoundTimer();
			if (PickupSound)
			{
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
	}
}

void AItem::EnableCusomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemMesh->SetRenderCustomDepth(true);
	}
	
}

void AItem::DisableCustomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemMesh->SetRenderCustomDepth(false);
	}
	
}

void AItem::InitializeCustomDepth()
{
	DisableCustomDepth();
}

void AItem::OnConstruction(const FTransform & Transform)
{
	
	//������ ��͵� ������ ���̺� ������ �ҷ�����
	// RarityTablePath = Editer�� ������ ���̺� ���۷��� �ּ� 
	FString RarityTablePath(TEXT("DataTable'/Game/_Game/DataTable/ItemRarityDataTable.ItemRarityDataTable'"));
	UDataTable* RarityTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *RarityTablePath));
	if (RarityTableObject)
	{
		FItemRarityTable* RarityRow = nullptr; //���̺� ����ü�� ���� nullptr
		switch (ItemRarity)
		{
		case EItemRarity::EIR_Damaged:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Damaged"), TEXT(""));
			break;

		case EItemRarity::EIR_Common:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Common"), TEXT(""));
			break;

		case EItemRarity::EIR_UnCommon:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("UnCommon"), TEXT(""));
			break;

		case EItemRarity::EIR_Rare:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Rare"), TEXT(""));
			break;

		case EItemRarity::EIR_Legendary:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Legendary"), TEXT(""));
			break;
		}
		if (RarityRow)
		{
			GlowColor = RarityRow->GlowColor;
			LightColor = RarityRow->LightColor;
			DarkColor = RarityRow->DarkColor;
			NumberOfStars = RarityRow->NumberOfStars;
			IconBackground = RarityRow->IconBackground;
			if (GetItemMesh())
			{
				GetItemMesh()->SetCustomDepthStencilValue(RarityRow->CustomDepthStencil);
			}
		}
	}
	if (MaterialInstance)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("FresnelColor"), GlowColor);
		ItemMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);
		EnableGlowMaterial();
	}


}

void AItem::EnableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 0);
	}
}

void AItem::UpdatePulse()
{
	float ElapsedTime{};
	FVector CurveValue{};
	
	switch (ItemState)
	{
	case EItemState::EIS_Pickup:

		if (PulseCurve)
		{
			ElapsedTime = GetWorldTimerManager().GetTimerElapsed(PulseTimer);
			CurveValue = PulseCurve->GetVectorValue(ElapsedTime);
		}
		break;

	case EItemState::EIS_EquipInterping:

		if (InterpPulseCurve)
		{
			ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
			CurveValue = InterpPulseCurve->GetVectorValue(ElapsedTime);
		}

		break;
	}

	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowAmount"), CurveValue.X * GlowAmount);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelExponent"), CurveValue.Y * FresnelExponent);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelReflectFraction"), CurveValue.Z * FresnelReflectFraction);

	}
}

void AItem::DisableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 1);
	}
}

void AItem::PlayEquipSound(bool bForcePlaySound)
{
	if (Character)
	{
		// ��ü�� ���� ����
		if (bForcePlaySound)
		{
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}

		if (Character->ShouldPlayEquipSound())
		{
			Character->StartEquipSoundTimer();
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// EquipInterping ������ �� �׸� �������� ó���մϴ�.
	ItemInterp(DeltaTime);
	
	//PulseCurve���� � �� �������� �� ���� ��� �Ű����� ����
	UpdatePulse();
}

void AItem::ResetPulseTimer()
{
	StartPulseTimer();
}

void AItem::StartPulseTimer()
{
	if (ItemState == EItemState::EIS_Pickup)
	{
		GetWorldTimerManager().SetTimer(PulseTimer, this, &AItem::ResetPulseTimer, PulseCurveTime);

	}
}

void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

// AShooterCharacter���� ȣ�� 
void AItem::StartItemCurve(AShooterCharacter* Char, bool bForcePlaySound)
{
	// ĳ���Ϳ� ���� �ڵ� ����
	Character = Char;

	// itemCount�� ���� ���� InterpLocations���� �迭 �ε��� ��������
	InterpLocIndex = Character->GetInterpLocationIndex();

	// �� interp ��ġ ����ü�� �׸� ���� 1�� �߰��մϴ�.
	Character->IncrementInterpLocItemCount(InterpLocIndex, 1);

	PlayPickupSound(bForcePlaySound);

	// � Ÿ�̸Ӱ� �������ڸ����� ��ġ 
	ItemInterpStartLocation = GetActorLocation();
	bInterping = true;
	SetItemState(EItemState::EIS_EquipInterping);
	GetWorldTimerManager().ClearTimer(PulseTimer);

	GetWorldTimerManager().SetTimer(ItemInterpTimer, this, &AItem::Interping, ZCurveTime);

	// ī�޶��� �ʱ� Yaw �� ��������
	const float CameraLocationYaw{ Character->GetFollowCamera()->GetComponentRotation().Yaw };
	// �������� �ʱ� Yaw �� ��������
	const float ItemRotationYaw{ GetActorRotation().Yaw };
	// ī�޶�� �׸� ���� �ʱ� Yaw ������
	InterpInitialYawOffset = ItemRotationYaw - CameraLocationYaw;

	bCanChangeCustomDepth = false;
}

// ItemInterpTimer�� �Ϸ�Ǹ� ȣ��
void AItem::Interping()
{
	bInterping = false;
	if (Character)
	{
		// interp ��ġ ����ü�� �׸� ������ 1�� ���ϴ�.
		Character->IncrementInterpLocItemCount(InterpLocIndex,-1);
		Character->GetPickupItem(this);

		Character->UnHighlightInventorySlot();
		
	}
	// �������� �ٽ� �������� ����
	SetActorScale3D(FVector(1.f));

	DisableGlowMaterial();

	bCanChangeCustomDepth = true;

	DisableCustomDepth();

	
}
// EquipInterping ������ �� �׸� ������ ó���մϴ�.
void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping) return;

	if (Character && ItemZCurve)
	{
		//ItemInterpTimer�� ������ �� ����� �ð�
		const float ElasedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		// ElasedTime�� �ش��ϴ� Curve �� ��������
		const float CurveValue = ItemZCurve->GetFloatValue(ElasedTime);

		// ��� ���۵� �� �׸��� �ʱ� ��ġ�� �����ɴϴ�.
		FVector ItemLocation = ItemInterpStartLocation;

		//ī�޶� �տ��� ��ġ ��������
		const FVector CameraInterpLocation{ GetInterpLocation() };
		
		// �׸񿡼� ī�޶� Interp ��ġ X �� Y������ ���ʹ� 0�� �˴ϴ�.
		const FVector ItemToCamera{ FVector(0.f, 0.f, (CameraInterpLocation - ItemLocation).Z) };

		// CurveValue�� ���� ���� �μ�
		const float DeltaZ = ItemToCamera.Size();

		const FVector CurrentLocation{ GetActorLocation() };
		// ������ X ��
		const float InterpXValue = FMath::FInterpTo(CurrentLocation.X, CameraInterpLocation.X, DeltaTime, 30.0f);
		// ������ Y ��
		const float InterpYValue = FMath::FInterpTo(CurrentLocation.Y, CameraInterpLocation.Y, DeltaTime, 30.0f);

		// ItemLocation�� X�� Y�� Interped ������ ����
		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;

		// �ʱ� ��ġ�� Z ������ҿ� � �� �߰�(DeltaZ�� ũ�� ������)
		ItemLocation.Z += CurveValue * DeltaZ;

		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		// �� �������� ī�޶� ȸ��
		const FRotator CameraRotation{ Character->GetFollowCamera()->GetComponentRotation() };
		// ī�޶� ȸ�� �� �ʱ� �� ������
		FRotator ItemRotation{ 0.f,CameraRotation.Yaw + InterpInitialYawOffset,0.f };
		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

		if (ItemScaleCurve)
		{
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElasedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
		
	}
}

