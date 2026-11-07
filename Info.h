
#pragma once
class Info
{
public:
    inline static float underlyingPriceMin = 10;
    inline static float underlyingPriceMax = 500;

    inline static float strikingPriceMinFac = 0.5;
    inline static float strikingPriceMaxFac = 1.5;

    inline static float timeExpiryMin = 0.0027;
    inline static float timeExpiryMax = 2;

    inline static float riskFreefloaterestMin = 0;
    inline static float riskFreefloaterestMax = 0.1;

    inline static float dividendMin = 0;
    inline static float dividendMax = 0.08;

    inline static float volatilityMin = 0.05;
    inline static float volatilityMax = 1;
};