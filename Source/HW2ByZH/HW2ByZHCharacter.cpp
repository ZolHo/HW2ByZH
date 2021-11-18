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
#include "Components/InputComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// AHW2ByZHCharacter

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
	CameraBoom2->SetRelativeLocation(FVector(0.f,40.f, 100.f));
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

	PlayerInputComponent->BindAction("LeftClick", IE_Pressed,this, &AHW2ByZHCharacter::LeftClickDispatch);
	PlayerInputComponent->BindAction("RightClick", IE_Pressed, this, &AHW2ByZHCharacter::RightClickDispatch);

	// Test function
	PlayerInputComponent->BindAction("Test", IE_Pressed, this, &AHW2ByZHCharacter::Test);
}

void AHW2ByZHCharacter::Test()
{
	// SwitchFightState();
	UE_LOG(LogTemp, Warning, TEXT("Test: "));

	
	
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
		FollowCamera->ToggleActive();
		FollowCamera2->ToggleActive();
		this->bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		break;
		// 拿枪
	case FightState::GUNMODE :
		FollowCamera->ToggleActive();
		FollowCamera2->ToggleActive();
		this->bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		break;
		// 拿雷
	case FightState::LEIMODE :
		FollowCamera->ToggleActive();
		FollowCamera2->ToggleActive();
		this->bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
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
		// 如果附件有武器， 那么随便装一个
		const auto TempIter = WeaponWhichCanPickSet.CreateConstIterator();
		PickUpWeapon(*TempIter);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("There is No Weapon"));
		UE_LOG(LogTemp, Warning, TEXT("There is No Weapon"));
	}
}

void AHW2ByZHCharacter::PickUpWeapon(AActor* Weapon)
{
	if (Weapon != nullptr && !IsValid(EquippedWeapon))
	{
		if (Weapon->GetClass()->IsChildOf(AEquipActor::StaticClass()))
		{
			EquippedWeapon = Cast<AEquipActor>(Weapon);
			
			if (Weapon->GetClass()->IsChildOf(AGunActor::StaticClass()))
			{
				AGunActor* GunActor = Cast<AGunActor>(Weapon);
				GunActor->GetWeaponMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Gun_Socket"));
				
				SwitchFightState(FightState::GUNMODE);
			}
			else if (Weapon->GetClass()->IsChildOf(ALeiActor::StaticClass()))
			{
				ALeiActor* LeiActor = Cast<ALeiActor>(Weapon);
				LeiActor->GetWeaponMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Lei_Socket"));
				
				SwitchFightState(FightState::LEIMODE);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Cast faild"));
			}

			EquippedWeapon->HintTextRenderComponent->SetVisibility(false);
			EquippedWeapon->BoxCollision->RemoveFromRoot();
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
	switch (NowFightState)
	{
	case FightState::OutFight:
		break;

	// 持枪模式
	case FightState::GUNMODE:
		if (Cast<AGunActor>(EquippedWeapon)->AmorNumber > 0)
		{
			// 射线检测
			Cast<AGunActor>(EquippedWeapon)->AmorNumber -= 1;
			FHitResult HitResult(ForceInit);
			FVector CamLoc, dir, TraceEnd, TraceStart;
			FRotator CamRot;

			Controller->GetPlayerViewPoint(CamLoc, CamRot);
			TraceStart = CamLoc;
			dir = CamRot.Vector();
			TraceEnd = CamLoc + (10000 * dir);
		
			FCollisionQueryParams TraceParams (FName(TEXT("Actor")), true, this);
			TraceParams.bReturnPhysicalMaterial = false;
			TraceParams.AddIgnoredActor(this);
			TraceParams.AddIgnoredActor(EquippedWeapon);

			DrawDebugLine(this->GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 5.0f);
			GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd,ECC_Visibility, TraceParams);
			// UE_LOG(LogTemp, Warning, TEXT("Now State:%s ,%s"), *TraceStart.ToString(), *TraceEnd.ToString());
			if (Cast<AActor>(HitResult.GetActor()))
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Blue, HitResult.GetActor()->GetName());
			}

			// 播放动画
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

	case FightState::GUNMODE:
		// 切换开镜状态
		bIsOpenMirror = !bIsOpenMirror;
		// 切换相机视角
		FollowCamera2->SetFieldOfView(90.f+45.f - FollowCamera2->FieldOfView);
		
		break;
	
	case FightState::LEIMODE:
		break;
		
	default:
		break;
	}
}
