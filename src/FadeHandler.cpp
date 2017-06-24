#include "FadeHandler.h"

FadeHandler::FadeHandler() :
_fadeTime(0), _numFadeSteps(0), _delayMs(0),
_currentFadeStep(0), _active(false) {}


void FadeHandler::init(uint16_t fadeTime) {

  _fadeTime = fadeTime;
  _numFadeSteps = N_FADE_STEPS_DEFAULT;
  _delayMs = fadeTime / _numFadeSteps;
  if (_delayMs < MIN_FADE_TIME){
    _delayMs = MIN_FADE_TIME;
    _numFadeSteps = fadeTime / _delayMs;
  }
  if (_numFadeSteps < 1){
    _numFadeSteps = 1;
  }
  // dont touch _scene, _startFrame, _endFrame and _numLoops
  _currentFadeStep = 0; // will iterate this one up to _numFadeSteps
  _active = true;
}

void FadeHandler::next(){
  if (_currentFadeStep + 1 > _numFadeSteps){
    // finished fading
    _active = false;
    return;
  }
  _currentFadeStep++;
}

float FadeHandler::getCurrentFadeFactor(){
  return _numFadeSteps > 0 ?
    (float)(_numFadeSteps - _currentFadeStep) / _numFadeSteps
    : 0.0;
}

bool FadeHandler::isActive(){
  return _active;
}

bool FadeHandler::isLastStep(){
  return (_currentFadeStep  == _numFadeSteps && _active);
}

uint16_t FadeHandler::getDelayMs(){
  return _delayMs;
}


void FadeHandler::printInfo(){
  printf("Fade to black - fade time: %d fade-steps: %d.\n", _fadeTime, _numFadeSteps);
}

void FadeHandler::printState(){
  printf("Fade to black: Active: %d Current fade step: %d fade factor: %f\n",
    _active, _currentFadeStep, getCurrentFadeFactor());
}
