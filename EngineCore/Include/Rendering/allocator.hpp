#pragma once
#include <cstdint>
#include <vector>

struct FreeRange
{
  uint32_t offset;
  uint32_t size;
};

class FreeListAllocator
{
public:
  void init(uint32_t totalSize);
  uint32_t allocate(uint32_t size);
  void free(uint32_t offset, uint32_t size);

private:
  std::vector<FreeRange> freeRanges;
};
