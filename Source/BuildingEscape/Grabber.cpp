// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	owner = GetOwner();
	
	PhysicsHandleComponent();

	InputHandleComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (physicsHandle->GrabbedComponent)
	{
		physicsHandle->SetTargetLocation(GetReachLineEnd());

	}
}

FVector UGrabber::GetReachLineEnd()
{
	FVector playerViewPoint;
	FRotator playerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(playerViewPoint, playerViewPointRotation);
	
	FVector lineTraceEnd = playerViewPoint + playerViewPointRotation.Vector() * reach;
	
	return lineTraceEnd;
}

FVector UGrabber::GetReachLineStart()
{
	FVector playerViewPoint;
	FRotator playerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(playerViewPoint, playerViewPointRotation);

	FVector lineTraceStart = playerViewPoint;

	return lineTraceStart;
}

void UGrabber::Grab()
{
	auto hitResult = GetFirstPhysicsBodyInReach();
	auto componentToGrab = hitResult.GetComponent();
	auto actorHit = hitResult.GetActor();

	if (actorHit)
	{
		physicsHandle->GrabComponent(
			componentToGrab,
			NAME_None, //no bones needed
			componentToGrab->GetOwner()->GetActorLocation(),
			true
		);

	}
}

void UGrabber::Release()
{
	UE_LOG(LogClass, Warning, TEXT("Release clicked"));
	physicsHandle->ReleaseComponent();
}

void UGrabber::PhysicsHandleComponent()
{
	physicsHandle = owner->FindComponentByClass<UPhysicsHandleComponent>();
	if (physicsHandle == nullptr)
	{
		UE_LOG(LogClass, Error, TEXT("No physics handler found!"));
	}
}

void UGrabber::InputHandleComponent()
{
	inputComponent = owner->FindComponentByClass<UInputComponent>();
	if (inputComponent)
	{
		inputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		inputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogClass, Error, TEXT("No input component found!"));
	}
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	FHitResult hit;

	FCollisionQueryParams traceParameters(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		hit,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		traceParameters);

	return hit;
}
