# Memory and Flash Budget

Estimates for the ESP32-2432S028R / CYD aquarium firmware
(`pio run -e esp32-2432s028r`), measured after the desk-events work
(tap-a-creature, overfeeding, rare visitors, clock-day behavior).

Night mode is still pending and is not included in these numbers.

## Flash (code / image size)

| Item | Size |
| --- | --- |
| `firmware.bin` | ~1.09 MB (1,139,184 bytes) |
| App partition limit (current build) | ~1.25 MB (1,310,720 bytes) |
| Used | ~86.9% |
| Free for more firmware | ~168 KB |

ELF breakdown (approximate):

| Section | Size |
| --- | --- |
| `.text` (flash code) | ~833 KB |
| `.rodata` (read-only data) | ~190 KB |
| Static `.data` + `.bss` (see RAM) | ~53 KB combined |

The desk events are mostly logic and cost little flash compared with Wi-Fi,
TFT_eSPI, and the aquarium engine.

## Runtime memory

### Static RAM

| Item | Size |
| --- | --- |
| Static RAM (`.data` + `.bss`) | ~53 KB of 320 KB (~16%) |

Static RAM looks comfortable. New small features barely move this number.

### Heap (no PSRAM on common CYD boards)

Known long-lived allocations:

| Item | Size |
| --- | --- |
| Water noise buffer (80×106×RGB) | ~25 KB |
| Dirty-tile tracker (80×106×uint16) | ~17 KB |
| Row/tile render buffer | ~6 KB |
| Full TFT RGB565 framebuffer | ~154 KB (disabled; `CYD_FRAMEBUFFER_RENDERER=0`) |

Creatures, plants, food, boids, and GFX layers also allocate on the heap.
Wi-Fi/NTP temporarily increases peak usage; the default build disconnects Wi-Fi
after NTP sync.

Exact free heap is best read from the serial monitor after boot (for example
`ESP.getFreeHeap()`). Expect on the order of **100+ KB** free once running
without Wi-Fi held open; less while NTP is connecting.

## Room for new features?

**Yes, for small and medium features.**

| Kind of change | Fit? |
| --- | --- |
| More touch behaviors, visitors, clock gags, short text bubbles, tuning | Easy |
| Another creature type, a bit more AI, optional MQTT | Possible; watch flash first (~168 KB left) |
| Full-screen RGB565 buffer, large sprites/GIFs, always-on Wi-Fi, much higher logical resolution | Hard without freeing space |

Flash is the tighter budget for more *code*. Heap is the tighter budget for
more *graphics buffers*.

## Ways to free headroom later

1. Turn off debug flags in `platformio.ini` (`CYD_TOUCH_DEBUG`, `CYD_PERF_DEBUG`).
2. Wire up `partitions_custom.csv` (it already defines ~3.25 MB app slots). The
   current environment does not reference it, which is why the app limit is
   ~1.25 MB.
3. Keep Wi-Fi disconnect-after-NTP (already enabled) so peak RAM stays lower.

## Bottom line

- ~170 KB flash and plenty of static RAM remain.
- Heap is fine for more aquarium logic, not for heavy graphics buffers.
- Prefer small event/behavior features over large framebuffers or asset packs
  unless the partition table or debug flags are adjusted first.

## How these numbers were taken

```sh
pio run -e esp32-2432s028r -t size
ls -la .pio/build/esp32-2432s028r/firmware.bin
```

PlatformIO reported roughly:

```text
RAM:   16.3% (used 53288 bytes from 327680 bytes)
Flash: 86.9% (used 1138772 bytes from 1310720 bytes)
```

Re-run those commands after large changes; this file is a snapshot, not a live
measurement.
