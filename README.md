## ESPHome AC Climate Mirage Ventus X Inverter mini-split (OVKH241A) component 

ESPHome AC Climate Mirage component. It has been made for a Mirage Ventus X Inverter mini-split AC unit that uses AEHA IR protocol.

The protocol uses an **AEHA (Japanese A/C IR standard) framing format** with a **custom payload layout**.

---

## Overview

- **Protocol Type:** AEHA (extended)
- **Carrier Frequency:** ~38 kHz
- **Address:** `0xC4D3`
- **Payload Length:** 12 bytes
- **Transmission:** Full-state packet (entire HVAC state sent every time)
- **Repeat:** Each command has 2 packets, a "Wake" packet first, followed by the data.

---

## Frame Structure


| Byte | Description |
|------|-------------|
| 0–3  | Header / device identifier |
| 4    | Mode |
| 5    | Temperature |
| 6    | Fan + vertical swing + mute |
| 7–9  | Reserved (always 0x00) |
| 10   | Horizontal swing + flags |
| 11   | Checksum |

---

## Header (Bytes 0–3)

### ID (Byte 0)
Always `0x64`

### Type (Byte 1)

| Value | Meaning |
|------|--------|
| `0x80` | Data Packet |
| `0x40` | Wake or ID Packet |

### Reserved (Byte 2)
Always `0x00`

### Base Flags (Byte 3)

| Value | Meaning |
|------|--------|
| `0x20` | Unit Power On |
| `0x02` | Display On (Inverted) |

### Notes

Bitmap flags

---

## Mode (Byte 4)

Upper nibble encodes HVAC mode:

| Mode | Value |
|------|------|
| Cool | `0xC0` |
| Heat | `0x80` |
| Dry  | `0x40` |
| Fan  | `0xE0` |
| Auto | `0x10` |

---

## Temperature (Byte 5)
Supported Range
Minimum: 61°F
Maximum: 88°F

Temperature is encoded as a lookup in orders of 2 in the upper nibble:

| Byte Value | Temp Meaning |
|------------|--------------|
| 0 | 88f |
| 1 | 73f |
| 2 | 81f |
| 3 | 66f |
| 4 | 84f |
| 5 | 70f |
| 6 | 77f |
| 7 | 63f |
| 8 | 86f |
| 9 | 72f |
| 10 | 79f |
| 11 | 64f |
| 12 | 82f |
| 13 | 68f |
| 14 | 75f |
| 15 | 61f |

Then odd numbers are determined by setting bit `0x20` in byte 10

---

## Fan + Vertical Swing + Mute (Byte 6)
This is a packed field combining multiple features.

### Vertical Swing

| Mode | Value |
|------|------|
| ON | `0x1C` |
| OFF | `0x00` |

### Fan Speed
Fan speed is encoded in the **upper bits** of the byte and supports:

| Fan Mode        | Value |
|-----------------|------|
| Auto            | `0x1C` |
| Low             | `0x5C` |
| Low–Mid         | `0x7C` |
| Mid             | `0xDC` |
| Mid–High        | `0x9C` |
| High            | `0xBC` |
| Turbo           | `0xBC` + modifier |

### Mute
Shares encoding with Low fan (0x5C)
Likely implemented as a flag within fan bits

### Notes
Always preserve unrelated bits when modifying
- Lower bits (`0x0C` / `0x1C`) relate to swing/flags
- Upper bits define fan intensity
- Intermediate speeds are **distinct values**, not computed steps

---

## Reserved (Bytes 7–9)
Always zero in all observed packets
Likely unused padding

## Bit Flags (Byte 10)

| Mode | Value |
|------|------|
| `0x10` | Horizontal swing |
| `0x20` | Odd number for temp (See temperature) |

### Notes
Base value `0x01` always present??

---

## Checksum (Byte 11)

Checksum is calculated by taking the reverse bit pattern for each byte, then sum the bytes and perform a simple function, then reverse the resulting byte again.
```
For a data array d = [d0, d1, ..., d10]:

Compute s = rev(d0) + rev(d1) + ... + rev(d10)
Compute temp = (s - 0x12) % 256
Checksum = rev(temp)
```