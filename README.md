# Pumpkinator

Source code and sounds for the **Pumpkinator**, a Halloween prop built by
[Fahrenheit Robotics](https://fahrenheitrobotics.org) Team 6882 students. When
someone walks past, it plays a spooky sound effect and a matching RGB light
show from inside a real pumpkin.

Full build instructions (printing, soldering, assembly, troubleshooting) live
on the website:
https://fahrenheitrobotics.org/site/docs/category/pumpkinator

This repo is the code that goes with that project.

## How it works

1. LEDs flicker like a candle while idle.
2. A PIR motion sensor detects someone approaching.
3. The Arduino tells the sound card to play the next track.
4. The LEDs run a light show timed to that track.
5. When the track's duration is up, it goes back to the candle flicker.

Tracks play in order (`SEQUENTIAL_MODE`) and wrap around after the last one.

## Hardware

| Part | Notes |
| --- | --- |
| Arduino Nano | CH340 USB chip on most clones |
| JQ6500 MP3 module (or HW-896 V2.0.2 clone) | plays mp3s stored in its onboard flash |
| 8 ohm small speaker | |
| HC-SR501 PIR motion sensor | needs 10-60 s to warm up after power-on |
| 2-3 WS2812B LEDs | cut from a strip; the sketch drives the first two |
| 4x AA battery box + SPST rocker switch | ~100-160 mA, roughly 15-20+ h on alkalines |

Pins used by `pumpkin.ino`:

| Function | Arduino pin |
| --- | --- |
| WS2812B data | D6 |
| PIR signal | D5 |
| Sound card TX | D2 (SoftwareSerial RX) |
| Sound card RX | D3 through a 1 kOhm resistor (SoftwareSerial TX) |
| Power to LEDs, PIR, sound card | 5V and GND |
| Battery pack | VIN and GND |

Never let the speaker wires touch each other, GND or VCC; that shorts the
amplifier output and can damage the sound module.

## Repo layout

- `pumpkin/pumpkin.ino` - the Arduino sketch. It has to stay inside a folder
  with the same name (an Arduino IDE rule).
- `sounds/` - the final mp3s, numbered in play order (`001_...`, `002_...`)
- `scripts/copy-sounds-in-order.ps1` - copies the mp3s onto the sound card
- `sounds_source/` - original raw recordings. Git-ignored (too large), so it
  only exists on the machine that made the sounds.

## Loading the code

1. Install the [Arduino IDE](https://www.arduino.cc/en/software). On Windows,
   install the CH340 driver if the Nano doesn't show up under
   *Ports (COM & LPT)* in Device Manager.
2. Tools > Board > **Arduino Nano**, Tools > Processor >
   **ATmega328P (Old Bootloader)**, and pick the COM port.
3. Install the libraries (Sketch > Include Library > Manage Libraries):
   - **FastLED** by Daniel Garcia
   - **JQ6500_Serial** - if it isn't in the Library Manager, download it from
     https://github.com/sleemanj/JQ6500_Serial and use *Add .ZIP Library*.
     Make sure the IDE's sketchbook location is the folder that actually
     contains these libraries (OneDrive can redirect Documents).
4. Open `pumpkin/pumpkin.ino`, Verify, then Upload. The LEDs should flicker
   like a candle.

If you get "programmer is not responding", try the other processor
setting, check the port, try another USB cable, and make sure nothing (such as
a stray wire) is tied to the Nano's RST pin.

## Loading the sounds

**The sound card plays files in the order they were *written* to it, not by
filename.** Load them one at a time in number order, and clear old files first;
leftovers keep their old position.

### HW-896 V2.0.2 clone (shows up as a plain drive)

Plug the sound card in over USB, then from the repo root:

```powershell
.\scripts\copy-sounds-in-order.ps1 -Destination D:
```

Replace `D:` with the sound card's drive letter. The script deletes the old
mp3/wav files and copies `sounds/*` in filename order. Eject the drive from
File Explorer before unplugging.

```powershell
# if script execution is blocked
powershell -ExecutionPolicy Bypass -File .\scripts\copy-sounds-in-order.ps1 -Destination D:

# preview only, copies nothing
.\scripts\copy-sounds-in-order.ps1 -Destination D: -WhatIf
```

### True JQ6500 (has `MusicDownload.exe` on its drive)

Run `MusicDownload.exe` from the sound card, select the mp3s in
`sounds/` in number order (001 first), and upload.

If your JQ6500 is missing `MusicDownload.exe`, a copy used to live in this repo
as `JQ6500_MusicDownload/` and was removed to keep the repo small. It can be
found here:

- Zip with per-size uploaders (`MusicDownload.exe` + `config.ini` for 8/16/32/64/128 MBit),
  shared in [sleemanj/JQ6500_Serial issue #6](https://github.com/sleemanj/JQ6500_Serial/issues/6):
  [JQ6500.voice.chip.module.zip](https://github.com/sleemanj/JQ6500_Serial/files/1821040/JQ6500.voice.chip.module.zip)
- English-language uploader versions (same issue, Google Drive):
  https://drive.google.com/open?id=1Kt1FV0__WelMJlX1oZE-hFp9kx6NCMFq
- Linux alternative: [NikolaiRadke/JQ6500-rescue-tool](https://github.com/NikolaiRadke/JQ6500-rescue-tool)

Pick the version matching the module's flash size (printed on the SPI flash
chip on the underside, e.g. `25X16` = 16 MBit).

## Light and sound shows

Each track has a light show, chosen by `state` (the track number) in `loop()`:

| # | Track | Light show |
| --- | --- | --- |
| 1 | `001_disco` | `loopDisco` - random colors |
| 2 | `002_crazy_clown_laugh` | `loopLaugh` - white strobe |
| 3 | `003_music_box` | `loopMusicBox` - red, blue, yellow, green |
| 4 | `004_organ` | `loopOrgan` - solid blue |
| 5 | `005_cat` | default (`loopLaugh`) |
| 6 | `006_electricty` | `loopElectricity` - purple blinks, then white |
| 7 | `007_beast` | `loopGreen` - solid green |
| 8 | `008_space_weasel` | `loopSpace` - purple/yellow alternating |
| 9 | `009_ufo` | default (`loopLaugh`) |
| 10 | `010_wolf` | `loopBlood` - solid red |

Any track without its own `state` check falls through to `loopLaugh`.

### Changing the sounds

1. Put your mp3s in `sounds/` with numeric prefixes (`001_...`,
   `002_...`) in the order you want them played.
2. Edit `TRACK_DURATIONS` near the top of `pumpkin.ino`: one length in
   milliseconds per track, in the same order. `TRACK_COUNT` is derived from
   this array. A missing or extra entry makes the sketch read past the end of
   the array and get stuck, so keep the array and the files in sync.
   `ffprobe -show_entries format=duration` gives exact lengths.
3. Update the `state == N` checks in `loop()` so each light show follows the
   right track.
4. Reload the sounds onto the sound card and upload the sketch.

## Troubleshooting

See the
[troubleshooting page](https://fahrenheitrobotics.org/site/docs/projects/pumpkinator/troubleshooting)
for the common kit problems (power, loose breadboard wires, speaker, sensor).
Notes from building kits:

- The PIR sensor needs 10-60 s after power-on before it detects motion.
- No sound but the lights work: check the speaker wires and that the sound
  card has the files in order.
- Stuck flashing white and silent: `TRACK_COUNT` / `TRACK_DURATIONS` don't
  match the number of mp3s on the card.
- The sketch seems not to run while the Arduino IDE is open: the IDE holds the
  serial port and resets the board. Close the IDE or unplug and replug USB.
