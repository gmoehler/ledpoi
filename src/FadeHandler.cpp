#include "FadeHandler.h"

FadeHandler::FadeHandler(ImageCache imageCache) :
_fadeTime(0), _numFadeSteps(0), _delayMs(0),
_currentFadeStep(0), _active(false), _imageCache(imageCache) {}


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

  // we take what is in register 0 and remember it in register 1
  // later we will copy pixels back using a factor on the rgb values
  _imageCache.copyRegisterToRegister(0, 1);
}

void FadeHandler::next(){
  if (_currentFadeStep + 1 > _numFadeSteps){
    // finished fading
    _active = false;
    return;
  }
  _currentFadeStep++;

  if (_active){
    // un-faded frame is in register 1
    _imageCache.copyRegisterToRegister(1, 0, _getCurrentFadeFactor());
  }
}

float FadeHandler::_getCurrentFadeFactor(){
  return _numFadeSteps > 0 ?
    (float)(_numFadeSteps - _currentFadeStep) / _numFadeSteps
    : 0.0;
}

bool FadeHandler::isActive(){
  return _active;
}

rgbVal* FadeHandler::getDisplayFrame(){
	return _imageCache.getRegister(0);
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
    _active, _currentFadeStep, _getCurrentFadeFactor());
}
