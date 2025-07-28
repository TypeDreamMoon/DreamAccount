// Fill out your copyright notice in the Description page of Project Settings.


#include "DreamAccountSettings.h"

UDreamAccountSettings* UDreamAccountSettings::Get()
{
	return GetMutableDefault<UDreamAccountSettings>();
}
