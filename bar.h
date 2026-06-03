#pragma once
#include "printer.h"
#include <algorithm>
#include <cmath>
#include <ncurses.h>
#include <string>

struct Colors {
  int low = 1;
  int middle = 2;
  int high = 3;
};

enum colorDirection {
  normal,  // lower on the bar is better
  reversed // higher on the bar is better
};

struct Bounds {
  float low = 20.0f;
  float high = 70.0f;
};


struct Scale {
  virtual float normalize(float value) const = 0;
  virtual ~Scale() = default;
};

struct LinearScale : Scale {
  float max;
  LinearScale(float max) : max(max) {}
  float normalize(float value) const override {
    return std::clamp(value / max, 0.0f, 1.0f);
  }
};

struct ExponentialScale : Scale {
  float max;
  float exponent;
  ExponentialScale(float max, float exponent = 2.0f) : max(max), exponent(exponent) {}
  float normalize(float value) const override {
    float norm = std::clamp(value / max, 0.0f, 1.0f);
    return std::pow(norm, 1.0f / exponent);
  }
};

struct LogScale : Scale {
  float max;
  float base;
  LogScale(float max, float base = 10.0f): max(max), base(base) {}
  float normalize(float value) const override {
    if (value <= 0.0f) return 0.0f;
    return std::log(1.0f + value * (base - 1.0f) / max) / std::log(base);
  }
};


class Bar : public PanelPrinter {
private:
  int y;
  int x;
  const float& value;
  std::string label;
  bool showValue = true;
  const Scale* scale;
  colorDirection direction = colorDirection::normal;

public:
  Bar(int y, int x, const float& trackedValue, const Scale* scale, std::string label = "");

  void print(WINDOW *window, int height, int width) override;

  void setLabel(std::string label);
  void setShowValue(bool show);
  void setColorDirection(colorDirection direction);
};
