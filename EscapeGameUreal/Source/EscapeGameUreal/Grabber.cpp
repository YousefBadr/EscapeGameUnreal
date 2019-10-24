// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Gameframework/Actor.h"
#include "Engine/World.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Engine/EngineTypes.h"


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}



// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	//look for atached physics handler
	FindPhysicsHandleComponent();

	//look for atached input handler
	SetupInputComponent();
}
void UGrabber::SetupInputComponent()
{
    InputComponent =GetOwner()->FindComponentByClass<UInputComponent>();
	if(InputComponent)
	{
		InputComponent->BindAction("Grab",IE_Pressed,this,&UGrabber::Grab);
		InputComponent->BindAction("Grab",IE_Released,this,&UGrabber::Release);
	}
	else
	{
      UE_LOG(LogTemp,Error, TEXT("No input ccc to %s, you idiot !"),*(GetOwner()->GetName()));
	}
}

void UGrabber::FindPhysicsHandleComponent()
{
    PhysicsHandle =GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if(PhysicsHandle == nullptr)
	{
    	UE_LOG(LogTemp,Error, TEXT("There's no handler to %s, you dump fuck !"),*(GetOwner()->GetName()));
	}

}

void UGrabber::Grab()
{
	//LINE TRACE and see if we reach any actors with physics body collision channel set
	auto HitResult=GetFirstPhysicsBodyInReach();   
	auto ComponentToGrab=HitResult.GetComponent();//get mesh
    auto ActorHit=HitResult.GetActor();
	//if we hit something then attach  physics handle
	if(ActorHit)
	{
		if(!PhysicsHandle) return;
		PhysicsHandle->GrabComponentAtLocationWithRotation(
		ComponentToGrab,
		NAME_None,//no bones needed it's a static mesh
		ComponentToGrab->GetOwner()->GetActorLocation(),
		FRotator()
		);
	}
}

void UGrabber::Release()
{
	if(!PhysicsHandle) return;
	PhysicsHandle->ReleaseComponent();
}

FVector UGrabber::GetLineTraceEnd()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
    GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation,PlayerViewPointRotation);
	return PlayerViewPointLocation+PlayerViewPointRotation.Vector()*reach;
}
FVector UGrabber::GetLineTraceStart()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
    GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation,PlayerViewPointRotation);
	return PlayerViewPointLocation;
}


void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);	

	if(!PhysicsHandle) return;

	 //if the physics handle is attached
	 if(PhysicsHandle->GrabbedComponent)
	 {
		 //just move the object we are holding
		 PhysicsHandle->SetTargetLocation(GetLineTraceEnd());
	 }
	   	

}
const FHitResult UGrabber::GetFirstPhysicsBodyInReach() 
{
	//setup query parameters
	//ignore tag name for now false for the arms of the chair and lastly we want to ignore ourself
	FCollisionQueryParams TraceParameters (FName(TEXT("")),false,GetOwner());

	//line tracing ray casting
	FHitResult hit;
	GetWorld()->LineTraceSingleByObjectType(
		hit,
		GetLineTraceStart(),
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters);
		AActor* ActorHit=hit.GetActor();
    if(ActorHit)
	{
       	UE_LOG(LogTemp, Warning, TEXT("Fuck %s i hit it !"),*(hit.GetActor()->GetName()));
	}

return hit;
}

