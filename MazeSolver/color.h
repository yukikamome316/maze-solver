#pragma once
#include <iostream>


class Color {
 public:
  uint32_t red, green, blue;

  Color() : red(0), green(0), blue(0) {}
  Color(uint32_t c, uint32_t r, uint32_t g, uint32_t b)
      : red(r), green(g), blue(b) {
    // clampValues();
  }

  void print() const {
    std::cout << "Red : " << (int)red << ", Green: " << (int)green
              << ", Blue: " << (int)blue << std::endl;
  }

 private:
  // void clampValues() {
  //   red = min(red, (uint8_t)255);
  //   green = min(green, (uint8_t)255);
  //   blue = min(blue, (uint8_t)255);
  // }
};

class ColorRange {
 public:
  uint32_t redMin, redMax, greenMin, greenMax, blueMin, blueMax;

  ColorRange(uint32_t rmin, uint32_t rmax, uint32_t gmin, uint32_t gmax,
             uint32_t bmin, uint32_t bmax)
      : redMin(rmin),
        redMax(rmax),
        greenMin(gmin),
        greenMax(gmax),
        blueMin(bmin),
        blueMax(bmax) {}

  bool isInRange(const Color& color) const {
    return (color.red >= redMin && color.red <= redMax) &&
           (color.green >= greenMin && color.green <= greenMax) &&
           (color.blue >= blueMin && color.blue <= blueMax);
  }
};


static const ColorRange redRange(60, 500, 20, 35, 15, 30);
static const ColorRange blackRange(15, 60, 15, 60, 0, 60);
