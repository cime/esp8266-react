#ifndef SPECTRUM_MODE_H
#define SPECTRUM_MODE_H

#include <Arduino.h>
#include <FastLed.h>

#include <audiolight/AudioLightMode.h>
#include <audiolight/JsonUtil.h>

#define MIN_PEAK_DECAY_AMOUNT 1
#define MAX_SAMPLE_SIZE 1024
#define SPECTRUM_DEFAULT_BRIGHTNESS 128

class SpectrumMode : public AudioLightMode {

private:
  uint8_t _brightness = SPECTRUM_DEFAULT_BRIGHTNESS;
  boolean _refresh = true;

  // How long should it take the peaks to decay from 100% to 0%
  uint16_t _peakDecayMs = 2000;
  uint16_t _peakDecayAmount = 0;
  uint16_t *_peaks;

  // Aproximated rolling averages
  float _rollingAverageFactor = 0.3;
  uint16_t *_rollingAverages;

public:
  SpectrumMode(CLEDController *ledController, CRGB *leds, uint16_t numLeds,  uint16_t *bands, uint16_t numBands) 
      : AudioLightMode(ledController, leds, numLeds, bands, numBands) {
      _rollingAverages = (uint16_t *) malloc(sizeof(uint16_t) * numBands);
      _peaks = (uint16_t *) malloc(sizeof(uint16_t) * numBands);
  };
  String getId();
  void tick();
  void sampleComplete() {};  
  void enable();
  void updateConfig(JsonObject &root);
  void writeConfig(JsonObject &root);
};

#endif
