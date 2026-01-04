#include "allocator.hpp"
#include <algorithm>

void FreeListAllocator::init(uint32_t totalSize)
{
  freeRanges.clear();
  freeRanges.push_back({0, totalSize});
}

uint32_t FreeListAllocator::allocate(uint32_t size)
{
  for (size_t i = 0; i < freeRanges.size(); i++)
  {
    FreeRange &r = freeRanges[i];

    if (r.size >= size)
    {
      uint32_t offset = r.offset;

      r.offset += size;
      r.size -= size;

      if (r.size == 0)
      {
        freeRanges.erase(freeRanges.begin() + i);
      }

      return offset;
    }
  }

  return UINT32_MAX;
}

void FreeListAllocator::free(uint32_t offset, uint32_t size)
{
  freeRanges.push_back({offset, size});

  // merge adjacent ranges
  std::sort(freeRanges.begin(), freeRanges.end(),
            [](const FreeRange &a, const FreeRange &b)
            {
              return a.offset < b.offset;
            });

  for (size_t i = 0; i + 1 < freeRanges.size();)
  {
    FreeRange &a = freeRanges[i];
    FreeRange &b = freeRanges[i + 1];

    if (a.offset + a.size == b.offset)
    {
      a.size += b.size;
      freeRanges.erase(freeRanges.begin() + i + 1);
    }
    else
    {
      i++;
    }
  }
}
