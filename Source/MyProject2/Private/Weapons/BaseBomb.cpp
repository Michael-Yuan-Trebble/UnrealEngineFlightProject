// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BaseBomb.h"

ABaseBomb::ABaseBomb() {
	canLock = false;
}

void ABaseBomb::BeginPlay() {

}

void ABaseBomb::Tick(float DeltaSeconds) {

}

void ABaseBomb::DestroyBomb() {

}

void ABaseBomb::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult) {

}

void ABaseBomb::OnHit(UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit) {

}

void ABaseBomb::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

}

void ABaseBomb::CheckAndDelete(AActor* OtherActor) {

}