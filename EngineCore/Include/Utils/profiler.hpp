#pragma once
#include <chrono>
#include <iostream>

class Timer
{
public:
  void start()
  {
    t_start = std::chrono::high_resolution_clock::now();
  }

  void stop(const std::string &name = "Code")
  {
    auto t_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start).count();
    std::cout << name << " took " << duration << " microseconds\n";
  }

private:
  std::chrono::high_resolution_clock::time_point t_start;
};