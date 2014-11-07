#include <gtest/gtest.h>

#include "test_ccv_model_pure.hpp"

int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  //UnitTest::GetInstance()->listeners().Append(new MemoryLeakDetector());

  return RUN_ALL_TESTS(); 
}
