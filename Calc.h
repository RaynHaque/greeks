#include <cmath>
#pragma once
#include <array>
class Calc
{

public:
    static inline float timeToExpirationSquareRoot(float timeToExpiration)
    {
        return std::sqrt(timeToExpiration);
    }
    static inline float dOne(float underlyingPrice, float strikePrice, float interestRate, float dividend, float volatility, float timeToExpiration, float timeToExpirationSquareRoot)
    {
        return (std::log(underlyingPrice / strikePrice) + (interestRate - dividend + 0.5f * volatility * volatility) * timeToExpiration) / (volatility * timeToExpirationSquareRoot);
    }

    static inline float dTwo(float dOne, float volatility, float timeToExpirationSquareRoot)
    {
        return (dOne - volatility * timeToExpirationSquareRoot);
    }

    static inline float cdf(float x)
    {
        return 0.5f * (1.0f + std::erf(x * 0.70710678f)); // 0.70710678f = 1/sqrt(2)
    }

    static inline float pdf(float x)
    {
        // 1.0 / sqrt(2 * pi)= 0.3989422804014327
        return 0.39894228f * std::exp(-0.5f * x * x);
    }

    static inline float deltaCall(float eToDividendXTimeToExpiration, float cdfDOne)
    {
        return eToDividendXTimeToExpiration * cdfDOne;
    }

    static inline float deltaPut(float eToDividendXTimeToExpiration, float cdfDOne)
    {
        return eToDividendXTimeToExpiration * (cdfDOne - 1.0f);
    }

    static inline float gamma(float eToDividendXTimeToExpiration, float timeToExpirationSquareRoot, float pdfDOne, float underlyingPrice, float volatility)
    {
        return (eToDividendXTimeToExpiration * pdfDOne) / (underlyingPrice * volatility * timeToExpirationSquareRoot);
    }

    static inline float thetaLeft(float underlyingPrice, float volatility, float eToDividendXTimeToExpiration, float pdfDOne, float timeToExpirationSquareRoot)
    {
        return (underlyingPrice * volatility * eToDividendXTimeToExpiration * pdfDOne) / (2.0f * timeToExpirationSquareRoot);
    }

    static inline float thetaMiddle(float interestRate, float strikePrice, float eToInterestXTimeToExpiration)
    {
        return interestRate * strikePrice * eToInterestXTimeToExpiration;
    }

    static inline float thetaRight(float dividend, float underlyingPrice, float eToDividendXTimeToExpiration)
    {
        return dividend * underlyingPrice * eToDividendXTimeToExpiration;
    }

    static inline float thetaCall(float thetaLeftV, float cdfDOne, float cdfDTwo, float thetaMiddleV, float thetaRightV)
    {
        return -thetaLeftV - thetaMiddleV * cdfDTwo + thetaRightV * cdfDOne;
    }

    static inline float thetaPut(float thetaLeftV, float cdfMinusDOne, float cdfMinusDTwo, float thetaMiddleV, float thetaRightV)
    {
        return -thetaLeftV + thetaMiddleV * cdfMinusDTwo - thetaRightV * cdfMinusDOne;
    }

    static inline float vega(float underlyingPrice, float eToDividendXTimeToExpiration, float timeToExpirationSquareRoot, float pdfDOne)
    {
        return underlyingPrice * eToDividendXTimeToExpiration * timeToExpirationSquareRoot * pdfDOne;
    }

    static inline float rhoLeft(float strikePrice, float timeToExpiration, float eToInterestXTimeToExpiration)
    {
        return strikePrice * timeToExpiration * eToInterestXTimeToExpiration;
    }

    static inline float rhoCall(float rhoLeftV, float cdfDTwo)
    {
        return rhoLeftV * cdfDTwo;
    }

    static inline float rhoPut(float rhoLeftV, float cdfMinusDTwo)
    {
        return -rhoLeftV * cdfMinusDTwo;
    }

    static inline std::array<float,8> calculateGreeks(float underlyingPrice, float strikePrice, float interestRate, float dividend, float volatility, float timeToExpiration)
    {
        float timeToExpirationSquareRoot = std::sqrt(timeToExpiration);
        float dOneValue = dOne(underlyingPrice, strikePrice, interestRate, dividend, volatility, timeToExpiration, timeToExpirationSquareRoot);
        float dTwoValue = dTwo(dOneValue, volatility, timeToExpirationSquareRoot);
        float cdfDOne = cdf(dOneValue);
        float cdfDTwo = cdf(dTwoValue);
        float cdfMinusDTwo = cdf(-dTwoValue);
        float cdfMinusDOne = cdf(-dOneValue);
        float pdfDOne = pdf(dOneValue);
        float eToDividendXTimeToExpiration = std::exp(-dividend * timeToExpiration);
        float eToInterestXTimeToExpiration = std::exp(-interestRate * timeToExpiration);

        std::array<float,8> greeks;

        float deltaCallValue = deltaCall(eToDividendXTimeToExpiration, cdfDOne);
        float deltaPutValue = deltaPut(eToDividendXTimeToExpiration, cdfDOne);

        float gammaValue = gamma(eToDividendXTimeToExpiration, timeToExpirationSquareRoot, pdfDOne, underlyingPrice, volatility);

        float thetaLeftValue = thetaLeft(underlyingPrice, volatility, eToDividendXTimeToExpiration, pdfDOne, timeToExpirationSquareRoot);
        float thetaMiddleValue = thetaMiddle(interestRate, strikePrice, eToInterestXTimeToExpiration);
        float thetaRightValue = thetaRight(dividend, underlyingPrice, eToDividendXTimeToExpiration);
        float thetaCallValue = thetaCall(thetaLeftValue, cdfDOne, cdfDTwo, thetaMiddleValue, thetaRightValue);
        float thetaPutValue = thetaPut(thetaLeftValue, cdfMinusDOne, cdfMinusDTwo, thetaMiddleValue, thetaRightValue);

        float vegaValue = vega(underlyingPrice, eToDividendXTimeToExpiration, timeToExpirationSquareRoot, pdfDOne);

        float rhoLeftValue = rhoLeft(strikePrice, timeToExpiration, eToInterestXTimeToExpiration);
        float rhoCallValue = rhoCall(rhoLeftValue, cdfDTwo);
        float rhoPutValue = rhoPut(rhoLeftValue, cdfMinusDTwo);

        greeks[0] = deltaCallValue;
        greeks[1] = deltaPutValue;
        greeks[2] = gammaValue;
        greeks[3] = thetaCallValue;
        greeks[4] = thetaPutValue;
        greeks[5] = vegaValue;
        greeks[6] = rhoCallValue;
        greeks[7] = rhoPutValue;
        return greeks;
    }
};