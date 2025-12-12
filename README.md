# Handheld Calculator (Raspberry Pi 3) — C

![build](https://github.com/hhuy66123/Handheld-Calculator/actions/workflows/build.yml/badge.svg)

C-based calculator that parses and evaluates floating-point expressions (parentheses, `+ - * / ^`) and supports root solving.

**Timeline:** Jan 2025 – Apr 2025

## Build & Run

### Windows (MSYS2 MinGW64 / gcc)

```bash
gcc -O2 -Wall -std=c11 -o handheld_calc.exe main.c postfix.c findroot.c -lm
./handheld_calc.exe
```

### Linux / Raspberry Pi

```bash
sudo apt-get update
sudo apt-get install -y build-essential
gcc -O2 -Wall -std=c11 -o handheld_calc main.c postfix.c findroot.c -lm
./handheld_calc
```

> PowerShell: run local exe with `.\handheld_calc.exe`

## Demo Outputs

- Simple: [docs/output-x-1.txt](docs/output-x-1.txt)
- Polynomial: [docs/output-poly.txt](docs/output-poly.txt)

## Example Input

```
(x^14-3*x^12+7*x^9)-(5*x^8+2*x^6)+(4*x^5-11*x^3+6*x^2)-(20*x-50)
```

### Sample Result (Secant)

```
root ≈ -2.034229
f(root) ≈ 0.002
```
