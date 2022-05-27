# SoundCoinBox

Small easy SoundBox to play Sounds for a Coin.

Features:
- Extreme Low Power for longlife Battery (10K Plays)
- Startsound after Poweron
- Keyswitch to disable the Box
- Battery level by press and holding the Button
- Random 8 Tone Playback

Used [Adafruit SoundBoard](https://github.com/adafruit/Adafruit_Soundboard_library)


Setup:
Audiofiles should look like: "T00.ogg"
T00.ogg is the Startsound - will be played after power on
T01.ogg is the Batterylevel - will be played if you press and Hold the Button for ~3sec
  - The Batterylevel will Start at 0% till 100% in 20% steps. The Sound will stop with the highest measured Batterylevel.

T02.ogg - T10.ogg are the random Soundfiles.

All 9 soundfiles must be available, there is a risk of no sound being played if a coin is detected with a missing file.
If you do not have 9 Files available copy some of the other Files and rename them.

Check:
[Adafruit Soundboard](https://learn.adafruit.com/adafruit-audio-fx-sound-board/creating-audio-files)
or
[This Tutorial](https://learn.adafruit.com/adafruit-wave-shield-audio-shield-for-arduino/convert-files)
to create Soundfiles.

If no coins are detected but the Leds are on try to teach the [Coin detector](https://malangelectronic.com/wp-content/uploads/2013/06/CH-926_multi_coin_selector.pdf).
