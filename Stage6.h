#pragma once
#include <vector>

#include "Stage.h"

class Stage6 : public Stage {
 private:
  float A, B, C;
  std::vector<float> yBuffer;
  std::vector<float> uBuffer;
  float maxInput, maxOutput;
  const float cutOffVoltage;

 public:
  Stage6();
  void processBlock(std::vector<std::vector<float>>& buffer) override;
  void configure(double sampleRate) override;
};