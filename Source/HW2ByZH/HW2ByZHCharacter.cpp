// Copyright Epic Games, Inc. All Rights Reserved.

#include "HW2ByZHCharacter.h"

#include "AnimationEditorUtils.h"
#include "DrawDebugHelpers.h"
#include "GunActor.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "LeiActor.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/DecalActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

//////////////////////////////////////////////////////////////////////////
// AHW2ByZHCharacter

class ADecalActor;

AHW2ByZHCharacter::AHW2ByZHCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->SetAutoActivate(true);
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom2 = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom2"));
	CameraBoom2->SetupAttachment(RootComponent);
	CameraBoom2->SetRelativeLocation(FVector(0.f,40.f, 80.f));
	CameraBoom2->TargetArmLength = 140.0f; // The camera follows at this distance behind the character	
	CameraBoom2->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom2->bInheritRoll = false;
	CameraBoom2->SetAutoActivate(true);
	
	
	// Create a follow camera
	FollowCamera2 = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera2"));
	FollowCamera2->SetupAttachment(CameraBoom2, USpringArmComponent::SocketName); 
	FollowCamera2->bUsePawnControlRotation = false;
	FollowCamera2->SetAutoActivate(false);
	
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}


//////////////////////////////////////////////////////////////////////////
// Input

void AHW2ByZHCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHW2ByZHCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHW2ByZHCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AHW2ByZHCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AHW2ByZHCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AHW2ByZHCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AHW2ByZHCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AHW2ByZHCharacter::OnResetVR);

	PlayerInputComponent->BindAction("PickUpWeapon", IE_Pressed, this, &AHW2ByZHCharacter::PickUpWeaponByOverlap);
	
	// 丢弃武器
	PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, this, &AHW2ByZHCharacter::DestroyNowWeapon);

	// 绑定鼠标的左右键
	PlayerInputComponent->BindAction("LeftClick", IE_Pressed,this, &AHW2ByZHCharacter::LeftClickDispatch);
	PlayerInputComponent->BindAction("RightClick", IE_Pressed, this, &AHW2ByZHCharacter::RightClickDispatch);
	PlayerInputComponent->BindAction("LeftClick", IE_Released, this, &AHW2ByZHCharacter::LeftClickReleasedDispatch);
	PlayerInputComponent->BindAction("RightClick", IE_Released, this, &AHW2ByZHCharacter::RightClickReleasedDispatch);
	// 绑定换弹函数
	PlayerInputComponent->BindAction("ReBullet", IE_Pressed, this, &AHW2ByZHCharacter::ReBullet);

	// Test function
	PlayerInputComponent->BindAction("Test", IE_Pressed, this, &AHW2ByZHCharacter::Test);
}

void AHW2ByZHCharacter::Test()
{
	
}
void AHW2ByZHCharacter::OnResetVR()
{
	// If HW2TPS is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in HW2TPS.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AHW2ByZHCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AHW2ByZHCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AHW2ByZHCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AHW2ByZHCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AHW2ByZHCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AHW2ByZHCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}



void AHW2ByZHCharacter::SwitchFightState(enum FightState ChangeFightState)
{
	if (NowFightState == ChangeFightState)
	{
		return;
	}
	NowFightState = ChangeFightState;
	switch (ChangeFightState)
	{
		// 普通情况
	case FightState::OutFight :
		// FollowCamera->ToggleActive();
		// FollowCamera2->ToggleActive();
		FollowCamera->Activate(true);
		FollowCamera2->Deactivate();
		this->bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;
		break;
		// 拿枪
	case FightState::GUNMODE :
		if (bIsOpenMirror) ToggleOpenMirror();  // 重置开镜状态
		// FollowCamera->ToggleActive();
		// FollowCamera2->ToggleActive();
		FollowCamera->Deactivate();
		FollowCamera2->Activate(true);
		this->bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;
		break;
		// 拿雷
	case FightState::LEIMODE :
		// FollowCamera->ToggleActive();
		// FollowCamera2->ToggleActive();
		FollowCamera2->Deactivate();
		FollowCamera->Activate(true);
		this->bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;
		break;
	default:
		break;
	}
	// 广播让蓝图或关卡来处理如HUD的其他变化
	if (ChangeStateDelegate.IsBound())
	{
		ChangeStateDelegate.Broadcast();
	}
	
}

void AHW2ByZHCharacter::PickUpWeaponByOverlap()
{
	if (WeaponWhichCanPickSet.Num()>0)
	{
		// 如果附近有武器， 那么随便装一个
		const auto TempIter = WeaponWhichCanPickSet.CreateConstIterator();
		PickUpWeapon(*TempIter);

		if (OnAmorNumerChangeDelegate.IsBound())
			OnAmorNumerChangeDelegate.Broadcast();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("There is No Weapon"));
		UE_LOG(LogTemp, Warning, TEXT("There is No Weapon"));
	}
}

void AHW2ByZHCharacter::PickUpWeapon(AActor* Weapon)
{
	if (Weapon != nullptr )
	{
		if (IsValid(EquippedWeapon))
		{
			DestroyNowWeapon();
		}
		if (Weapon->GetClass()->IsChildOf(AEquipActor::StaticClass()))
		{
			
			EquippedWeapon = Cast<AEquipActor>(Weapon);
			// 如果是枪
			if (Weapon->GetClass()->IsChildOf(AGunActor::StaticClass()))
			{
				AGunActor* GunActor = Cast<AGunActor>(Weapon);
				GunActor->GetWeaponMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Gun_Socket"));
				GunWeapon = GunActor; // 装备到武器操
				
				SwitchFightState(FightState::GUNMODE);
			}
			// 如果是雷
			else if (Weapon->GetClass()->IsChildOf(ALeiActor::StaticClass()))
			{
				ALeiActor* LeiActor = Cast<ALeiActor>(Weapon);
				LeiActor->GetWeaponMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Lei_Socket"));
				LeiWeapon = LeiActor; // 装备到武器槽
				
				SwitchFightState(FightState::LEIMODE);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Cast faild"));
			}

			EquippedWeapon->HintTextRenderComponent->DestroyComponent();
			EquippedWeapon->BoxCollision->DestroyComponent();
		}
		
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("No Weapon to Equip or Exist Equipped Weapon"));
		UE_LOG(LogTemp, Warning, TEXT("No Weapon to Equip or Exist Equipped Weapon"));
	}
}

void AHW2ByZHCharacter::DestroyNowWeapon()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->Destroy();
		// 退出战斗状态
		SwitchFightState(FightState::OutFight);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("No Equipped Weapon to Drop"));
		UE_LOG(LogTemp, Warning, TEXT("No Equipped Weapon to Drop"));
	}
}

void AHW2ByZHCharacter::LeftClickDispatch()
{
	bIsLeftPressed = true;
	switch (NowFightState)
	{
	case FightState::OutFight:
		break;

	// 持枪模式
	case FightState::GUNMODE:
		if (Cast<AGunActor>(EquippedWeapon)->GunAmorNumber > 0)
		{
			OnGunFire();
		}
		else
		{
			// 自动换弹
			UGameplayStatics::SpawnSound2D(GetWorld(), RebulletSound);
			ReBullet();
		}
		break;
	case FightState::LEIMODE:
		break;
	}
	
}

void AHW2ByZHCharacter::RightClickDispatch()
{
	switch (NowFightState)
	{
	case FightState::OutFight:
		break;

	case FightState::GUNMODE: // 枪 右键
		ToggleOpenMirror(); // 开关镜
		
		break;
	
	case FightState::LEIMODE: // 手雷右键
		// 在tick中开启弹道预测
		bIsPredictProjectile = true;
	
		break;
		
	default:
		break;
	}
}


void AHW2ByZHCharacter::ReBullet()
{
	if (GunWeapon)
	{
		GunWeapon->GunReBullet();
		// 换弹音乐
		if (RebulletSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), RebulletSound, this->GetActorLocation());
		}
		
		if (OnAmorNumerChangeDelegate.IsBound())
			OnAmorNumerChangeDelegate.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("You Have No Gun To ReBullet"));
	}
}

void AHW2ByZHCharacter::OnGunFire()
{
	// 射线检测碰撞
	Cast<AGunActor>(EquippedWeapon)->GunAmorNumber -= 1;
	FHitResult HitResult(ForceInit);
	FVector CamLoc, dir, TraceEnd, TraceStart;
	FRotator CamRot;

	Controller->GetPlayerViewPoint(CamLoc, CamRot); // 获取摄像机loc和rot
	TraceStart = CamLoc;
	dir = CamRot.Vector();
	TraceEnd = CamLoc + (10000 * dir);

	FCollisionQueryParams TraceParams (FName(TEXT("Actor")), true, this);
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(EquippedWeapon);
	
	// DrawDebugLine(this->GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 5.0f);  
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd,ECC_Visibility, TraceParams);
	
	if (Cast<AActor>(HitResult.GetActor()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, HitResult.GetActor()->GetName());
	}

	// 播放开枪动画
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (bIsOpenMirror)
	{
		if (FireAnimation_Aim != nullptr && AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation_Aim, 1.f);
		}
	}
	else
	{
		if (FireAnimation_Hip!= nullptr && AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation_Hip, 1.f);
		}
	}

	// 施加冲击力
	if (HitResult.GetActor())
	{
		if (HitResult.GetActor()->IsRootComponentMovable())
		{
			UStaticMeshComponent* HitMeshComponent = Cast<UStaticMeshComponent>(HitResult.GetActor()->GetRootComponent());
			if (HitMeshComponent && HitMeshComponent->IsSimulatingPhysics())
				HitMeshComponent->AddForce(dir*50000*HitMeshComponent->GetMass());
			else
				UE_LOG(LogTemp, Warning, TEXT("You Hit Nullptr or UnSimulatePhy"));
		}
	}
	
	FRotator effectRotator = UKismetMathLibrary::Conv_VectorToRotator(HitResult.Normal); // 旋转角度
	
	// 弹孔效果
	ADecalActor* decal = GetWorld()->SpawnActor<ADecalActor>(HitResult.Location, FRotator(0.f, 0.f, 0.f));
	if (decal && BulletHoleMaterial)
	{
		decal->AttachToActor(HitResult.GetActor(),FAttachmentTransformRules::KeepWorldTransform);
		decal->SetDecalMaterial(BulletHoleMaterial);
		decal->SetLifeSpan(5.0f);
		decal->GetDecal()->DecalSize = FVector(1.0f, 4.0f, 4.0f);
		decal->SetActorRotation(effectRotator);
		decal->GetDecal()->FadeScreenSize = 0.f;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No decal spawned"));
	}

	// 粒子效果
	if (BulletHitParticleSystem)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHitParticleSystem,HitResult.Location, effectRotator, FVector(0.5f, 0.5f, 0.5f));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No ParticleSystem Spawned"));
	}

	// 通知更新UI
	if (OnAmorNumerChangeDelegate.IsBound())
		OnAmorNumerChangeDelegate.Broadcast();
}

void AHW2ByZHCharacter::ToggleOpenMirror()
{
	// 切换开镜状态
	bIsOpenMirror = !bIsOpenMirror;
	// 切换相机视角
	FollowCamera2->SetFieldOfView(90.f+50.f - FollowCamera2->FieldOfView);
	GetCharacterMovement()->MaxWalkSpeed = (NormalWalkSpeed + 200 - GetCharacterMovement()->MaxWalkSpeed);
}

void AHW2ByZHCharacter::LeftClickReleasedDispatch()
{
	bIsLeftPressed = false;
	switch (NowFightState)
	{
	case FightState::OutFight:
		break;

	case FightState::GUNMODE:
		
		break;
	
	case FightState::LEIMODE:
		if (IsValid(LeiWeapon))
		{
			// 分离手雷
			LeiWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			// LeiWeapon->RemoveFromRoot();
			LeiWeapon->GetWeaponMesh()->SetSimulatePhysics(true);
			LeiWeapon->GetWeaponMesh()->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
			LeiWeapon->GetWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			
			// 注释的是失败方案
			// // LeiWeapon->ProjectileMovementComponent->Velocity = force;
			// LeiWeapon->ProjectileMovementComponent->bSimulationEnabled = true;
			// // LeiWeapon->ProjectileMovementComponent->InitializeComponent();
			// LeiWeapon->GetWeaponMesh()->AddForce(force*100000*LeiWeapon->GetWeaponMesh()->GetMass());

			LeiWeapon->GetWeaponMesh()->SetPhysicsLinearVelocity(GetLeiVelocityForce());
			LeiWeapon->ReadyToBoom();

			// 解除对手雷的控制
			LeiWeapon = nullptr;
			EquippedWeapon = nullptr;
			SwitchFightState(FightState::OutFight);
		}
		break;
		
	default:
		break;
	}
}

void AHW2ByZHCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bIsPredictProjectile && IsValid(LeiWeapon))
	{
		GeneratePredictLine();
	}
	
}

FVector AHW2ByZHCharacter::GetLeiVelocityForce()
{
	FVector CamLoc;
	FRotator CamRot;
	Controller->GetPlayerViewPoint(CamLoc, CamRot);
	// 将抛射角度根据视角提高
	FVector force = ( FVector(CamRot.Vector().X, CamRot.Vector().Y, 0.6f + CamRot.Vector().Z *0.7)).GetSafeNormal()* 800; 
	return force;
}

// 需要清除剩余的spline mesh， 交给蓝图重载
void AHW2ByZHCharacter::RightClickReleasedDispatch()
{
	// 松开右键关闭弹道预测
	bIsPredictProjectile = false;
}
