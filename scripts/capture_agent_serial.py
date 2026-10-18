#!/usr/bin/env python3
"""Tee PlatformIO / serial lines prefixed with a marker into a debug NDJSON log.

Small local helper for capturing serial output. Not required to build or flash
the firmware; it is only useful during development.
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

PREFIX = "DBG "


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--log",
        default="~/.local/state/cyd-aquarium/debug.log",
    )
    parser.add_argument("--port", default=None, help="Optional serial port")
    parser.add_argument("--baud", type=int, default=115200)
    args = parser.parse_args()

    log_path = Path(args.log).expanduser()
    log_path.parent.mkdir(parents=True, exist_ok=True)

    if args.port:
        try:
            import serial  # type: ignore
        except ImportError:
            print("pyserial required for --port mode", file=sys.stderr)
            return 1
        ser = serial.Serial(args.port, args.baud, timeout=0.2)
        print(f"capturing {args.port} -> {log_path}", flush=True)
        try:
            while True:
                raw = ser.readline()
                if not raw:
                    continue
                line = raw.decode("utf-8", errors="replace").rstrip("\r\n")
                print(line, flush=True)
                if line.startswith(PREFIX):
                    with log_path.open("a", encoding="utf-8") as fh:
                        fh.write(line[len(PREFIX) :] + "\n")
        except KeyboardInterrupt:
            return 0

    # stdin mode: pio device monitor | this script
    print(f"capturing stdin -> {log_path}", flush=True)
    for line in sys.stdin:
        text = line.rstrip("\r\n")
        print(text, flush=True)
        if text.startswith(PREFIX):
            with log_path.open("a", encoding="utf-8") as fh:
                fh.write(text[len(PREFIX) :] + "\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
