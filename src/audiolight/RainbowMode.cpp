#include <audiolight/RainbowMode.h>

String RainbowMode::getId() {
  return "rainbow";
}

void RainbowMode::enable() {
  _lastFrameMicros = micros();
}

void RainbowMode::sampleComplete() {
  for (uint8_t i = 0; i<_numBands; i++) {
    _rollingAverages[i] = _rollingAverageFactor * _bands[i] + (1 - _rollingAverageFactor) * _rollingAverages[i];
  }
}

void RainbowMode::tick() {
  // rotate hue in time based manner
  unsigned long rotateDelayMicros = 1000000 / _rotateSpeed;
  unsigned long currentMicros = micros();
  unsigned long microsElapsed = (unsigned long)(currentMicros - _lastFrameMicros);
  if (microsElapsed >= rotateDelayMicros){
    _lastFrameMicros = currentMicros;
    _initialhue++;
  }
  
  /* Approach 1 uses each frequency in turn*/
  uint16_t currentLed = 0;
  uint8_t hue = _initialhue;
  for (uint8_t i = 0; i<_numBands; i++) {        
    // paint rainbow and fade according to level of each frequency
    fill_rainbow(_leds + currentLed, _ledsPerBand + (i ==_numLeds -1 ? _remainingLeds: 0), hue, _hueDelta);
    // fade each segment if audio enabled
    if (_audioEnabled) {
      fadeToBlackBy(_leds + currentLed, _ledsPerBand + (i ==_numLeds -1 ? _remainingLeds: 0), 255-map(_rollingAverages[i], 0, ADC_MAX_VALUE, 0, 255));
    }    
    // increment hue and current led
    for (int j = 0; j<_ledsPerBand; j++){
      hue += _hueDelta;
    }
    currentLed += _ledsPerBand;    
  }
  _ledController->showLeds(_brightness);
}

void RainbowMode::updateConfig(JsonObject& root) {
  updateByteFromJson(root, &_brightness, RAINBOW_DEFAULT_BRIGHTNESS, "brightness");
  updateByteFromJson(root, &_rotateSpeed, RAINBOW_DEFAULT_ROTATE_SPEED, "rotate_speed");
  updateBoolFromJson(root, &_audioEnabled, RAINBOW_DEFAULT_AUDIO_ENABLED, "audio_enabled"); 
  _rollingAverageFactor = root["rolling_average_factor"] | RAINBOW_DEFAULT_ROLLING_AVG_FACTOR;
  if (_rollingAverageFactor < RAINBOW_MIN_ROLLING_AVG_FACTOR) {
    _rollingAverageFactor = RAINBOW_MIN_ROLLING_AVG_FACTOR;
  }
  if (_rollingAverageFactor > RAINBOW_MAX_ROLLING_AVG_FACTOR){
    _rollingAverageFactor = RAINBOW_MAX_ROLLING_AVG_FACTOR;
  }
  updateByteFromJson(root, &_hueDelta, RAINBOW_DEFAULT_HUE_DELTA, "delta_hue");
}

void RainbowMode::writeConfig(JsonObject& root) {
  writeByteToJson(root, &_brightness, "brightness");
  writeByteToJson(root, &_rotateSpeed, "rotate_speed");
  writeBoolToJson(root, &_audioEnabled, "audio_enabled");
  root["rolling_average_factor"] = _rollingAverageFactor;
  writeByteToJson(root, &_hueDelta, "delta_hue");
}
