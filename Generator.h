#pragma once

#include <random>

#include <array>

#include <thread>

#include <vector>

#include <memory>

#include "ThreadPool.h"

class Generator {

  public: template < int T >
    static inline std::unique_ptr < std::array < float,
  T >> generateNValuesHeap(float min, float max, int factor) {
    std::mt19937 gen {
      std::random_device {}()
    };
    std::uniform_real_distribution < float > dist(min, max);
    std::unique_ptr < std::array < float, T >> values(new std::array < float, T > );
    float * data = values -> data();
    if (T <= factor || factor <= 0) {
      for (int i = 0; i < T; ++i) {
        ( * values)[i] = dist(gen);
      }
    } else {
      int chunks = T / factor;
      auto index = 0;
      while (chunks > 0) {

        ThreadPool::enqueue([data, index, factor, min, max]() {
          std::mt19937 gen {
            std::random_device {}()
          };
          std::uniform_real_distribution < float > dist(min, max);
          for (auto i = index; i < index + factor; ++i) {
            data[i] = dist(gen);
          }
        });
        index += factor;
        chunks--;
      }
      if (T % factor != 0) {
        ThreadPool::enqueue([data, index, factor, min, max]() {
          std::mt19937 gen {
            std::random_device {}()
          };
          std::uniform_real_distribution < float > dist(min, max);
          for (auto i = index; i < T; ++i) {
            data[i] = dist(gen);
          }
        });
      }
    }

    return values;
  }

  template < int T >
  static inline std::array < float,
  T > generateNValuesStack(float min, float max, int factor) {
    std::mt19937 gen {
      std::random_device {}()
    };
    std::uniform_real_distribution < float > dist(min, max);
    std::array < float, T > values;
    float * data = values -> data();
    if (T <= factor || factor <= 0) {
      for (int i = 0; i < T; ++i) {
        ( * values)[i] = dist(gen);
      }
    } else {
      int chunks = T / factor;
      auto index = 0;
      while (chunks > 0) {

        ThreadPool::enqueue([data, index, factor, min, max]() {
          std::mt19937 gen {
            std::random_device {}()
          };
          std::uniform_real_distribution < float > dist(min, max);
          for (auto i = index; i < index + factor; ++i) {
            data[i] = dist(gen);
          }
        });
        index += factor;
        chunks--;
      }
      if (T % factor != 0) {
        ThreadPool::enqueue([data, index, factor, min, max]() {
          std::mt19937 gen {
            std::random_device {}()
          };
          std::uniform_real_distribution < float > dist(min, max);
          for (auto i = index; i < T; ++i) {
            data[i] = dist(gen);
          }
        });
      }
    }

    return values;
  }
};