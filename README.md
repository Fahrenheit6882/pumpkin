# Pumpkin

Arduino-controlled pumpkin / haunted house prop. A motion sensor triggers a
sound track on a JQ6500-style MP3 module (HW-896 clone) while the LEDs play a
matching light show. When idle it runs a candle flicker.

## Layout

- `pumpkin/` - the sketch and everything it needs. Copy this folder (and rename
  the `.ino` to match) to make a variant.
  - `pumpkin.ino` - the Arduino sketch
  - `sounds/` - the final mp3s, numbered in play order (`001_...`, `002_...`)
  - `scripts/copy-sounds-in-order.ps1` - copies the mp3s to the sound module
- `audio_play/`, `led_*/`, `motion_simple_detect/` - small test sketches
- `sounds_source/` - original raw recordings. Not tracked in git (too large).

## Arduino libraries

Install these in the Arduino IDE (the sketchbook location must contain them):

- **FastLED** - Library Manager
- **JQ6500_Serial** - https://github.com/sleemanj/JQ6500_Serial (Add .ZIP Library)

## Loading sounds onto the module

The HW-896 plays files in the order they were *written* to the drive, not by
filename. With the module plugged in over USB (it shows up as a small FAT
drive), run:

```powershell
.\pumpkin\scripts\copy-sounds-in-order.ps1 -Destination D:\
```

The script clears old mp3/wav files first, then copies in filename order. After
adding, removing or reordering mp3s, update `TRACK_DURATIONS` and the `state`
checks in `loop()` in `pumpkin.ino` to match.

## JQ6500 MusicDownload tool (if ever needed)

Some JQ6500 modules are sold without the `MusicDownload.exe` uploader on their
virtual "CD drive". The HW-896 used here mounts as a plain FAT drive and does
not need it, so a copy used to live in this repo as `JQ6500_MusicDownload/` and
was removed to keep the repo small. If you need it, it can be found here:

- Zip with per-size uploaders (`MusicDownload.exe` + `config.ini` for 8/16/32/64/128 MBit),
  shared in [sleemanj/JQ6500_Serial issue #6](https://github.com/sleemanj/JQ6500_Serial/issues/6):
  [JQ6500.voice.chip.module.zip](https://github.com/sleemanj/JQ6500_Serial/files/1821040/JQ6500.voice.chip.module.zip)
- English-language uploader versions (same issue, Google Drive):
  https://drive.google.com/open?id=1Kt1FV0__WelMJlX1oZE-hFp9kx6NCMFq
- Linux alternative: [NikolaiRadke/JQ6500-rescue-tool](https://github.com/NikolaiRadke/JQ6500-rescue-tool)

Pick the version matching the module's flash size (printed on the SPI flash
chip on the underside, e.g. `25X16` = 16 MBit), plug the module in over USB,
and run `MusicDownload.exe`.
