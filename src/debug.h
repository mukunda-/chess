#ifndef FRAG_DEBUG_H_
#define FRAG_DEBUG_H_

#include <chrono>

#define DEBUG_TIME_ENABLE true

#define DEBUG_TIME_DECLARE(name)                                               \
  static std::chrono::time_point<std::chrono::high_resolution_clock>           \
      start_##name;                                                            \
  static std::chrono::time_point<std::chrono::high_resolution_clock>           \
      end_##name;                                                              \
  static double count_##name = 0;                                              \
  static double total_##name = 0;                                              \
  static double max_##name = 0;

#define DEBUG_TIME_START(name)                                                 \
  if (DEBUG_TIME_ENABLE) {                                                     \
    start_##name = std::chrono::high_resolution_clock::now();                  \
  }

#define DEBUG_TIME_END(name)                                                   \
  if (DEBUG_TIME_ENABLE) {                                                     \
    end_##name = std::chrono::high_resolution_clock::now();                    \
    double elapsed_##name =                                                    \
        std::chrono::duration<double, std::milli>(end_##name - start_##name)   \
            .count();                                                          \
    if (elapsed_##name > max_##name) {                                         \
      max_##name = elapsed_##name;                                             \
    }                                                                          \
    total_##name += elapsed_##name;                                            \
    count_##name += 1;                                                         \
    std::cout << "debug-timer " #name << " (max: " << max_##name               \
              << "ms, avg: " << total_##name / count_##name << ", est: "       \
              << (901.06 * total_##name / count_##name) / 36 / 1000            \
              << " hours): " << elapsed_##name << "ms" << std::endl;           \
  }

#endif
