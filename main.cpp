#include <iostream>

#include "Info.h"

#include "Generator.h"

#include "Calc.h"

#include <array>

#include <chrono>

#include <vector>

#include "ThreadPool.h"

template < int T >
  void inline generateData(std::unique_ptr < std::array < std::array < float, 6 > , T >> & randomData, int factor) {
    std::unique_ptr < std::array < float, T >> underlyingPrices = Generator::generateNValuesHeap < T > (Info::underlyingPriceMin, Info::underlyingPriceMax, factor);
    std::unique_ptr < std::array < float, T >> strikingPrice = Generator::generateNValuesHeap < T > (Info::strikingPriceMinFac, Info::strikingPriceMaxFac, factor);
    std::unique_ptr < std::array < float, T >> timeExpiry = Generator::generateNValuesHeap < T > (Info::timeExpiryMin, Info::timeExpiryMax, factor);
    std::unique_ptr < std::array < float, T >> riskFreefloaterest = Generator::generateNValuesHeap < T > (Info::riskFreefloaterestMin, Info::riskFreefloaterestMax, factor);
    std::unique_ptr < std::array < float, T >> dividend = Generator::generateNValuesHeap < T > (Info::dividendMin, Info::dividendMax, factor);
    std::unique_ptr < std::array < float, T >> volatility = Generator::generateNValuesHeap < T > (Info::volatilityMin, Info::volatilityMax, factor);
    ThreadPool::waitAll();

    for (int i = 0; i < T; ++i) {
      ( * randomData)[i][0] = ( * underlyingPrices)[i];
      ( * randomData)[i][1] = ( * underlyingPrices)[i] * ( * strikingPrice)[i];
      ( * randomData)[i][2] = ( * timeExpiry)[i];
      ( * randomData)[i][3] = ( * riskFreefloaterest)[i];
      ( * randomData)[i][4] = ( * dividend)[i];
      ( * randomData)[i][5] = ( * volatility)[i];
    }
  }

template < int T >
  void inline greeks(std::unique_ptr < std::array < std::array < float, 6 > , T >> & randomData,
    std::unique_ptr < std::array < std::array < float, 8 > , T >> & greeksData, int factor) {
    int chunks = T / factor;
    int index = 0;
    while (chunks > 0) {

      ThreadPool::enqueue([ & randomData, & greeksData, index, factor]() {
        for (auto i = index; i < index + factor; ++i) {

          ( * greeksData)[i] = Calc::calculateGreeks(
            ( * randomData)[i][0],
            ( * randomData)[i][1],
            ( * randomData)[i][3],
            ( * randomData)[i][4],
            ( * randomData)[i][5],
            ( * randomData)[i][2]
          );
        }
      });

      index += factor;
      chunks--;
    }

    if (T % factor != 0) {
      ThreadPool::enqueue([ & randomData, & greeksData, index, factor]() {
        for (auto i = index; i < T; ++i) {
          ( * greeksData)[i] = Calc::calculateGreeks(
            ( * randomData)[i][0],
            ( * randomData)[i][1],
            ( * randomData)[i][3],
            ( * randomData)[i][4],
            ( * randomData)[i][5],
            ( * randomData)[i][2]
          );
        }
      });
    }
    ThreadPool::waitAll();
  }
int main() {
  {
    ThreadPool pool(10);

    auto start = std::chrono::high_resolution_clock::now();

    constexpr int numbersOfOptions = 30000000;
    int factor = 32768;

    std::unique_ptr < std::array < std::array < float, 6 > , numbersOfOptions >> randomData(new std::array < std::array < float, 6 > , numbersOfOptions > );

    generateData < numbersOfOptions > (randomData, factor);

    auto startCalc = std::chrono::high_resolution_clock::now();
    
    std::unique_ptr < std::array < std::array < float, 8 > , numbersOfOptions >>  greeksData (new std::array < std::array < float, 8 > , numbersOfOptions >);

    greeks < numbersOfOptions > (randomData, greeksData, factor);

    auto endCalc = std::chrono::high_resolution_clock::now();
    auto elapsedCalc = std::chrono::duration_cast < std::chrono::nanoseconds > (endCalc - startCalc).count();
    std::cout << "Latency: " << elapsedCalc << " ns (" << elapsedCalc / 1000000.0 << " ms (total for Calculation))\n";

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast < std::chrono::nanoseconds > (end - start).count();
    std::cout << "Latency: " << elapsed << " ns (" << elapsed / 1000000.0 << " ms (total for data generation to calculation))\n";
  }
  return 0;
}
