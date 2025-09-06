#pragma once

#include <Arduino.h>

// Minimal G-code parser supporting:
// - G0/G00: rapid move (no drawing)
// - G1/G01: linear move (draws if pen is down)
// - G90: absolute positioning (default)
// - G91: relative positioning
// - M3: pen down
// - M5: pen up
// Coordinates are interpreted as steps in the device's coordinate space.

class GCodeParser {
 public:
  typedef void (*MoveCallback)(float x_mm, float y_mm, bool drawing);
  typedef void (*PenCallback)(bool down);

  // The parser uses a simple work-coordinate offset (like a fixed G92) so that
  // absolute program coordinates are interpreted relative to the starting
  // machine position when execution begins. This makes programs that start at
  // X0/Y0 render at the current device position instead of the physical origin.
  GCodeParser(MoveCallback moveCb, PenCallback penCb, float startXmm = 0, float startYmm = 0)
      : _move(moveCb), _pen(penCb), _x(startXmm), _y(startYmm), _ox(startXmm), _oy(startYmm) {}

  void reset(float x_mm, float y_mm) {
    _x = x_mm;
    _y = y_mm;
    _ox = x_mm;
    _oy = y_mm;
    _abs = true;
    // do not change pen state on reset
  }

  void setAbsolute(bool absolute) { _abs = absolute; }
  void setPenState(bool down) { _penDown = down; }
  bool penState() const { return _penDown; }

  float curX() const { return _x; }
  float curY() const { return _y; }

  void process(const String &program) {
    // Split by newlines and semicolons; handle CRLF, comments, and empty lines
    int start = 0;
    int len = program.length();
    while (start < len) {
      int end = start;
      while (end < len && program.charAt(end) != '\n' && program.charAt(end) != '\r' && program.charAt(end) != ';') {
        end++;
      }
      String line = program.substring(start, end);
      processLine(line);
      // skip to next token, consuming any consecutive separators
      start = end + 1;
      while (start < len) {
        char c = program.charAt(start);
        if (c == '\n' || c == '\r' || c == ';') {
          start++;
        } else {
          break;
        }
      }
    }
  }

  void processLine(String line) {
    // Strip comments: remove anything after ';' or within ( ... )
    int sc = line.indexOf(';');
    if (sc >= 0) line.remove(sc);
    // remove (...) comments
    int open = line.indexOf('(');
    while (open >= 0) {
      int close = line.indexOf(')', open + 1);
      if (close < 0) {
        // unmatched, drop rest
        line.remove(open);
        break;
      }
      line.remove(open, close - open + 1);
      open = line.indexOf('(');
    }
    line.trim();
    if (line.length() == 0) return;

    line.toUpperCase();

    // Parse leading word (G or M) and optional number
    int i = 0;
    skipSpaces(line, i);
    if (i >= line.length()) return;

    char code = line.charAt(i);
    if (code == 'G') {
      i++;
      int gnum = parseInt(line, i);
      // Accumulate parameters
  bool hasX = false, hasY = false;
  float xVal = 0, yVal = 0;
      // scan remaining words
      while (i < line.length()) {
        skipSpaces(line, i);
        if (i >= line.length()) break;
        char w = line.charAt(i++);
        if (w == 'X') {
          xVal = parseFloat(line, i);
          hasX = true;
        } else if (w == 'Y') {
          yVal = parseFloat(line, i);
          hasY = true;
        } else {
          // ignore other words for now
          // consume possible numeric
          (void)parseFloat(line, i);
        }
      }

      if (gnum == 0 || gnum == 00) {
        // Rapid move: do not draw regardless of pen state
        float tx = hasX ? coordTarget(xVal, _x, true) : _x;
        float ty = hasY ? coordTarget(yVal, _y, false) : _y;
        doMove(tx, ty, false);
      } else if (gnum == 1 || gnum == 01) {
        // Linear move: draw if pen is down
        float tx = hasX ? coordTarget(xVal, _x, true) : _x;
        float ty = hasY ? coordTarget(yVal, _y, false) : _y;
        doMove(tx, ty, _penDown);
      } else if (gnum == 90) {
        _abs = true;
      } else if (gnum == 91) {
        _abs = false;
      } else {
        // unsupported G-code: ignore
      }
    } else if (code == 'M') {
      i++;
      int mnum = parseInt(line, i);
      if (mnum == 3) {
        _penDown = true;
        if (_pen) _pen(true);
      } else if (mnum == 5) {
        _penDown = false;
        if (_pen) _pen(false);
      } else if (mnum == 300) {
        // Some plotter dialects use M300 S<angle/speed> for pen control; we map S> (threshold) to up/down.
        // Extract S if present
        float sval = NAN;
        while (i < line.length()) {
          skipSpaces(line, i);
          if (i >= line.length()) break;
          char w = line.charAt(i++);
          if (w == 'S') {
            sval = parseFloat(line, i);
            break;
          } else {
            (void)parseFloat(line, i);
          }
        }
        if (!isnan(sval)) {
          // heuristic threshold
          bool down = sval < 60; // typical: S30 down, S50 up
          _penDown = down;
          if (_pen) _pen(down);
        }
      } else {
        // ignore other M codes
      }
    } else {
      // Unknown leading word; try to find inline G/M later (tolerant)
      // Not implemented: ignore line
    }
  }

 private:
  MoveCallback _move;
  PenCallback _pen;
    float _x = 0.0f;
    float _y = 0.0f;
    // Work-coordinate offset applied to absolute targets (acts like a fixed G92 at start)
    float _ox = 0.0f;
    float _oy = 0.0f;
  bool _abs = true;   // G90
  bool _penDown = false;

  static void skipSpaces(const String &s, int &i) {
    while (i < s.length()) {
      char c = s.charAt(i);
      if (c == ' ' || c == '\t') i++;
      else break;
    }
  }

  static int parseInt(const String &s, int &i) {
    skipSpaces(s, i);
    bool neg = false;
    if (i < s.length() && (s.charAt(i) == '-' || s.charAt(i) == '+')) {
      neg = (s.charAt(i) == '-');
      i++;
    }
    long val = 0;
    bool any = false;
    while (i < s.length()) {
      char c = s.charAt(i);
      if (c >= '0' && c <= '9') {
        any = true;
        val = val * 10 + (c - '0');
        i++;
      } else {
        break;
      }
    }
    return any ? (neg ? -val : val) : 0;
  }

  static float parseFloat(const String &s, int &i) {
    skipSpaces(s, i);
    bool neg = false;
    if (i < s.length() && (s.charAt(i) == '-' || s.charAt(i) == '+')) {
      neg = (s.charAt(i) == '-');
      i++;
    }
    long ip = 0;
    bool any = false;
    while (i < s.length()) {
      char c = s.charAt(i);
      if (c >= '0' && c <= '9') {
        any = true;
        ip = ip * 10 + (c - '0');
        i++;
      } else {
        break;
      }
    }
    float val = (float)ip;
    if (i < s.length() && s.charAt(i) == '.') {
      i++;
      float place = 0.1f;
      while (i < s.length()) {
        char c = s.charAt(i);
        if (c >= '0' && c <= '9') {
          any = true;
          val += (c - '0') * place;
          place *= 0.1f;
          i++;
        } else {
          break;
        }
      }
    }
    return any ? (neg ? -val : val) : 0.0f;
  }

  float coordTarget(float v, float current, bool axisIsX = true) const {
    if (_abs) {
      // Apply starting offset so absolute program coordinates are relative to start
      return (axisIsX ? _ox : _oy) + v;
    }
    return current + v;
  }

  void doMove(float x_mm, float y_mm, bool drawing) {
    if (_move) _move(x_mm, y_mm, drawing);
    _x = x_mm;
    _y = y_mm;
  }
};
