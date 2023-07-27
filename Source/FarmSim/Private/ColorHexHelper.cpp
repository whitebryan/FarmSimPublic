// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorHexHelper.h"

uint8 UColorHexHelper::HexToByte(FString Hex)
{
    uint8 OutDec;

    Hex.StartsWith("#") ? Hex = Hex.Mid(1, Hex.Len() - 1) : Hex;

    HexToBytes(Hex.ToUpper(), &OutDec);
    return OutDec;

    // Old Method, if you're looking for a way without using functions.
    // const int len = Hex.Len();
    // uint8 Out = 0;
    //
    // for (int i = 0; i < len; i++)
    // {
    //     char digit = Hex[len - 1 - i];
    //     if (digit >= '0' && digit <= '9')
    //     {
    //         Out += (digit - '0') * FMath::Pow(16,i);
    //     }
    //     else
    //     {
    //         Out += (digit - 'A' + 10) * FMath::Pow(16,i);
    //     }
    // }
    //
    // return Out;
}

FLinearColor UColorHexHelper::RandomLinearColor(bool RandomAlpha, bool TrueRandom)
{
    FLinearColor OutColor;

    // True random color
    if (TrueRandom)
    {
        OutColor = FLinearColor
        (
            FMath::FRand(),
            FMath::FRand(),
            FMath::FRand(),
            RandomAlpha ? FMath::FRand() : 1
        );

        return OutColor;
    }

    // Simple random color
    OutColor = FLinearColor::MakeRandomColor();
    if (RandomAlpha) OutColor.A = FMath::FRand();

    return OutColor;
}

FColor UColorHexHelper::RandomColor(bool RandomAlpha, bool TrueRandom)
{
    FColor OutColor;

    // True random color
    if (TrueRandom)
    {
        OutColor = FColor
        (
            FMath::RandRange(0, 255),
            FMath::RandRange(0, 255),
            FMath::RandRange(0, 255),
            RandomAlpha ? FMath::Rand() : 1
        );

        return OutColor;
    }

    // Simple random color
    OutColor = FColor::MakeRandomColor();
    if (RandomAlpha) OutColor.A = FMath::Rand();

    return OutColor;
}
