// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Gameframework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	owner=GetOwner();
	if(!owner)
	{
		UE_LOG(LogTemp,Error, TEXT("owner not found"))
	}
	if(!PressurePlate1)
	{
		UE_LOG(LogTemp,Error, TEXT("Pressure plate 1 not attached"))
	}  
	if(!PressurePlate2)
	{
		UE_LOG(LogTemp,Error, TEXT("Pressure plate 2 not attached"))
	}  
}

bool UOpenDoor::CanIOpenTheDoor()
{
	if(!PressurePlate1)
	{
		UE_LOG(LogTemp,Error, TEXT("Pressure plate 1 not attached"))
		return 0.f;
	} 
	if(!PressurePlate2)
	{
		UE_LOG(LogTemp,Error, TEXT("Pressure plate 2 not attached"))
		return 0.f;
	} 

	bool OpenForPressurePlate1=false;
	bool OpenForPressurePlate2=false;
	TArray <AActor*> OverlappingActors1;
	TArray <AActor*> OverlappingActors2;
	//find all overlaping
	PressurePlate1->GetOverlappingActors(OverlappingActors1);
	PressurePlate2->GetOverlappingActors(OverlappingActors2);
	for(const auto * Actor :OverlappingActors1 )//const means we won't change actor contents
	{
		if(Actor->GetName().Equals(TEXT("SM_Chair3")))
			OpenForPressurePlate1=true;
		//TotalMass1+=Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp,Warning, TEXT("1=>Overlapping actor name is %s"),*(Actor->GetName()))
	}
	for(const auto * Actor :OverlappingActors2 )//const means we won't change actor contents
	{
		if(Actor->GetName().Equals(TEXT("SM_TableRound_15")))
			OpenForPressurePlate2=true;
		//TotalMass1+=Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp,Warning, TEXT("2 =>Overlapping actor name is %s"),*(Actor->GetName()))
	}

	if(OpenForPressurePlate1 && OpenForPressurePlate2)
		return true;	
	else
		return false;

}



// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    //pull the triger volume every frame
	if(CanIOpenTheDoor())
	{
		//if our actor steps in volume then open door using BP
		OnOpen.Broadcast();
	}
	else
	{
		OnClose.Broadcast();
	}
}

