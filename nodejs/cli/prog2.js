require("lib/poiCommands")

loadScene(0)
loopStart(1, 10)
  animateWorm(BLUE, 2, 15, 500)
  playFrames(10, 20, 3, 1000)
loopEnd(1)
