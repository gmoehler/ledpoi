#include "FrameFader.h"

FrameFader::FrameFader() :
_fadeTime(0), _numFadeSteps(0), _delayMs(100),
_currentFadeStep(0), _active(false) {}


void FrameFader::init(uint16_t fadeTime) {

  _fadeTime = fadeTime;
  _numFadeSteps = N_FADE_STEPS_DEFAULT;
  _delayMs = fadeTime / _numFadeSteps;
  if (_delayMs < MIN_FADE_TIME){
    _delayMs = MIN_FADE_TIME;
    _numFadeSteps = fadeTime / _delayMs;
  }
  // dont touch _scene, _startFrame, _endFrame and _numLoops
  _currentFadeStep = 0; // will iterate this one up to _numFadeSteps
  _active = true;
}

void FrameFader::next(){

_currentFadeStep++;

  if (_currentFadeStep > _numFadeSteps){
    // finished fading
    _active = false;
    return;
  }
}

float FrameFader::getCurrentFadeFactor(){
  return (float)(_numFadeSteps - _currentFadeStep) / _numFadeSteps;
}

bool FrameFader::isActive(){
  return _active;
}

uint8_t FrameFader::getDelayMs(){
  return _delayMs;
}


void FrameFader::printInfo(){
  printf("Fade to black - fade time: %ld fade-steps: %d.\n", _fadeTime, _numFadeSteps);
}

void FrameFader::printState(){
  printf("Fade to black: Active: %d Current fade step: %d fade factor: %f\n",
    _active, _currentFadeStep, getCurrentFadeFactor());
}
