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
- **Repeat:** Each command is transmitted **twice**

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

### Byte 0-2 (Constants)
[0x64, 0x80, 0x00]

### Byte 3 (Flags)

| Value | Meaning |
|------|--------|
| `0x24` | Normal ON state |
| `0x04` | Power OFF |
| `0x26` | Display OFF |

### Bit Meaning (Byte 3)

- `0x20` → Unit ON
- `0x04` → Power command
- `0x02` → Display toggle

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

Temperature is encoded as:

```c
data[5] = (temp_f - 60) * 8;
```

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

## Horizontal Swing (Byte 10)

| Mode | Value |
|------|------|
| OFF | `0x21` |
| ON | `0x31` |

### Bit Meaning
`0x10` → Horizontal swing ON
Base value `0x21` always present

---

## Checksum (Byte 11)

Checksum is a simple additive sum:
```c
checksum = (sum of bytes 0–10) & 0xFF;
```