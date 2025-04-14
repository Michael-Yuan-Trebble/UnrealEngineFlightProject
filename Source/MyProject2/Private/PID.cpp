// Fill out your copyright notice in the Description page of Project Settings.


#include "PID.h"

UPID::UPID() {
	Kp = 0.f;
	Ki = 0.f;
	Kd = 0.f;
	previousError = 0.f;
	Integral = 0.f;
}

void UPID::Initialize(float InKp, float InKi, float InKd) {
	Kp = InKp;
	Ki = InKi;
	Kd = InKd;
}

float UPID::Update(float Setpoint, float CurrentValue, float DeltaTime) {
	float Error = Setpoint - CurrentValue;

	float P = Kp * Error;

	Integral += Error * DeltaTime;
	float I = Ki * Integral;

	float D = Kd * (Error - previousError) / DeltaTime;

	previousError = Error;

	return P + I + D;
}

void UPID::Reset() {
	previousError = 0.f;
	Integral = 0.f;
}