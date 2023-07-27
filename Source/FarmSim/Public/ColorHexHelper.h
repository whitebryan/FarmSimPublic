// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ColorHexHelper.generated.h"

/**
 * 
 */
UCLASS()
class FARMSIM_API UColorHexHelper : public UBlueprintFunctionLibrary
{
public:

	GENERATED_BODY()
public:

	/**
	*	Converts a hexadecimal string value to a byte value.
	*	@param Hex				The Hexadecimal value
	*	@return					The value as a byte
	*/
	UFUNCTION(BlueprintPure, Category = "Color Wheel Helper|Conversion", meta = (DisplayName = "Hex ➜ Byte", Keywords = "hex byte convert"))
	static uint8 HexToByte(FString Hex);

	/**
	*	Converts a byte value to a Hexadecimal value represented as a string.
	*	@param Byte				The byte value
	*	@return					The value as a hexadecimal represented in a string
	*/
	UFUNCTION(BlueprintPure, Category = "Color Wheel Helper|Conversion", meta = (DisplayName = "Byte ➜ Hex", Keywords = "hex byte convert"))
	static FORCEINLINE FString ByteToHex(uint8 Byte) { return FString::Printf(TEXT("%02X"), Byte); }

	/**
	*	Converts a Hex from an FString value to a FColor value, needs to contain all channels so simply writing #FF won't
	*	work!
	*	@param Hex				The Hexadecimal string value
	*	@return					Returns the RGBA value of the Hexadecimal value
	*/
	UFUNCTION(BlueprintPure, Category = "Color Wheel Helper|Conversion", meta = (DisplayName = "Hex ➜ Color", Keywords = "hex convert rgb"))
	static FORCEINLINE FColor HexToColor(FString Hex) { return FColor::FromHex(Hex); }

	/**
	*	Converts a Hex from an FString value to a FLinearColor value, needs to contain all channels so simply writing #FF won't
	*	work!
	*	@param Hex				The Hexadecimal string value
	*	@return					Returns the linear color values of the Hexadecimal value
	*/
	UFUNCTION(BlueprintPure, Category = "Color Wheel Helper|Conversion", meta = (DisplayName = "Hex ➜ LinearColor", Keywords = "hex convert rgb linear"))
	static FORCEINLINE FLinearColor HexToLinearColor(FString Hex) { return FLinearColor(FColor::FromHex(Hex)); }

	/**
	*	Converts a FColor value to a FString value in Hexadecimal.
	*	@param Color			The RGBA Color
	*	@return					The Hex value as string
	*/
	UFUNCTION(BlueprintPure, Category = "Color Wheel Helper|Conversion", meta = (DisplayName = "Color ➜ Hex", Keywords = "hex convert rgb"))
	static FORCEINLINE FString ColorToHex(const FColor Color) { return Color.ToHex(); }

	/**
	*	Converts a FLinearColor value to a FString value in Hexadecimal.
	*	@param Color			The linear color
	*	@param IsSRGB			Is the linear color in SRGB?
	*	@return					The Hex value as string
	*/
	UFUNCTION(BlueprintPure, Category = "Color Wheel Helper|Conversion", meta = (DisplayName = "LinearColor ➜ Hex", Keywords = "hex convert rgb linear"))
	static FORCEINLINE FString LinearColorToHex(const FLinearColor Color, const bool IsSRGB) { return Color.ToFColor(IsSRGB).ToHex(); }


	/**
	 * Returns a randomized linear color value
	 * @param RandomAlpha Randomize the Alpha value aswell ?
	 * @param TrueRandom Should each channel of the color be randomized ?
	 * @return A random linear color
	 */
	UFUNCTION(BlueprintPure, Category = "Color Wheel Helper|Misc", meta = (DisplayName = "Random Linear Color", Keywords = "random linear color"))
	static FLinearColor RandomLinearColor(bool RandomAlpha, bool TrueRandom);

	/**
	* Returns a randomized color value
	* @param RandomAlpha Randomize the Alpha value aswell ?
	* @param TrueRandom Should each channel of the color be randomized ?
	* @return A random color
	*/
	UFUNCTION(BlueprintPure, Category = "Color Wheel Helper|Misc", meta = (DisplayName = "Random Color", Keywords = "random color"))
	static FColor RandomColor(bool RandomAlpha, bool TrueRandom);
};
