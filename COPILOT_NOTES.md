# Copilot notes for Label Maker (Arduino Nano ESP32)

This project is an Arduino sketch for an ESP32-based label plotter with BLE control and a simple Web Bluetooth client.

## Build, upload, and layout
- Board: Arduino Nano ESP32 (FQBN: `arduino:esp32:nano_nora`).
- Entry point: `label-maker.ino`.
- G-code parser: `GCodeParser.h` (mm-based, float coords).
- Web client: `test-client.html` (Chrome/Web Bluetooth).
- VS Code tasks:
  - Build: “Arduino Nano Esp32: Compile”
  - Upload: “Arduino Nano Esp32: Upload” (depends on compile, so it will automatically compile and upload new firmware)

## BLE protocol (write-only)
Commands are ASCII: `command,params` (comma required, params may be empty).
- `print-text,<text>`: plots vector text using internal alphabet.
- `print-raw,<gcode>`: executes G-code program (see G-code support below).
- `pen-up,` and `pen-down,`: immediate pen control.

On write, firmware reads and clears the characteristic; missing comma is ignored.

## Coordinates, units, and limits
- G-code units: millimeters (mm). Supports G90 (absolute) and G91 (relative).
- Device internal units are “steps”; conversion happens in the G-code bridge:
  - X: 1600 units = 36 mm → X_UNITS_PER_MM = 1600/36 ≈ 44.444…
  - Y: 1600 units = 14 mm → Y_UNITS_PER_MM = 1600/14 ≈ 114.2857
- Origin (0,0) is bottom-left of the printable area.
- Assumed printable limits (not strictly enforced yet): Max X = 260 mm, Max Y = 35 mm.
- After `print-raw` completes:
  - Cursor advances to `maxPrintedX + 5 mm` (pen up), then Y returns to `0` (pen up).

If enforcing limits later, clamp X/Y before calling `line()`.

## Motion and pen control
- `line(int newx, int newy, bool drawing)`: Bresenham in device units.
- `penUp()` / `penDown()` set servo (25° up, 80° down). `plot(bool)` is the internal helper.
- Global position in device units: `xpos`, `ypos`. The G-code bridge keeps these in sync.

## G-code support (minimal)
Implemented in `GCodeParser.h`:
- G0/G00 rapid (no draw), G1/G01 linear (draw if pen down), G90, G91.
- M3 = pen down, M5 = pen up.
- M300 S… heuristic (S < ~60 → down; else up) for plotter dialects.
- Comments: `;…` and `(…)` removed. Semicolons can separate commands inline.

To extend:
1. Parse additional words (e.g., F feedrate) in `processLine`.
2. Map to new behavior in the move/pen callbacks or expand the bridge in `label-maker.ino`.

## Web Bluetooth client
- `test-client.html` features:
  - Connect/Disconnect
  - Pen Down / Pen Up buttons
  - Text input + “Send Text” → `print-text,<text>`
  - “Send Sample G-code” → draws an outer rectangle with a centered triangle within 260×35 mm, starting at (0,0).
- For large programs, consider chunking/streaming (BLE MTU limits).

## Common tasks
- New G-code feature: update `GCodeParser.h` (mm), convert in bridge, call `line()`.
- Adjust scaling/limits: edit constants in the move bridge in `label-maker.ino`.
- Debug BLE: ensure commands include a comma; use Chrome DevTools on the web client.

## Quick sanity workflow
1. Compile/Upload via VS Code tasks.
2. Open `test-client.html` in Chrome, connect.
3. Try: `G90;M5;G0 X0 Y0;M3;G1 X20 Y0;G1 X20 Y10;G1 X0 Y10;G1 X0 Y0;M5`.
4. Verify cursor advances past drawing and Y returns to 0.

## Notes
- Vector text plotting is independent of G-code and uses internal `x_scale`/`y_scale`.
- Stepper speeds set in `setup()`. Don’t push too fast.
- Servo has small delays; avoid rapid toggles without motion.
