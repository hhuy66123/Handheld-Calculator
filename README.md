# Multi-Platform Handheld Calculator (Raspberry Pi 3 & STM32)

This project aims to develop a versatile handheld calculator capable of solving complex floating-point equations. It is implemented across two distinct hardware platforms: **Raspberry Pi 3** and **STM32**, providing both high-level processing power and low-level real-time execution.

A core feature of this project is the use of **FreeRTOS** on the STM32 to perform parallel computations and determine which numerical method solves a given equation the fastest.

---

## 🚀 Key Features

### 1. STM32 Implementation (Real-Time Parallel Processing)

The STM32 version leverages **FreeRTOS** to optimize mathematical performance through multi-threading:

- **Parallel Execution:** The system launches three concurrent tasks to solve equations using different numerical methods:
  - **Newton-Raphson Method**
  - **Bisection Method**
  - **Secant Method**
- **Race for the Solution:** Using a shared "found" flag and Mutex protection, the algorithm that converges to a valid root first wins.
- **Task Management:** Once the fastest solution is found, the system automatically terminates the remaining tasks to conserve CPU resources and power.
- **Performance Benchmarking:** The execution time (with 0.1ms precision) is measured and displayed on the SSD1306 OLED screen, allowing for a direct comparison of algorithmic efficiency.

### 2. Raspberry Pi 3 Implementation

- Handles complex equation parsing and high-precision floating-point arithmetic.
- Provides a robust environment for testing advanced mathematical operations.

---

## 🛠 Hardware Components

- **Microcontrollers:** STM32F103 (Cortex-M3) & Raspberry Pi 3 Model B.
- **Display:** SSD1306 OLED (I2C Communication).
- **Input:** 4x4 or 4x5 Matrix Keypad for equation entry.
- **Communication:** I2C for peripherals and GPIO for keypad scanning.

---

## 🏗 Software Architecture (STM32 & FreeRTOS)

The project utilizes the **CMSIS-RTOS V1** API. Below is the core logic for managing the parallel solver tasks:

```c
// Initialize 3 tasks with equal priority to compare performance fairly
osThreadDef(NewtonTask, StartNewtonTask, osPriorityNormal, 0, 512);
taskHandles[0] = osThreadCreate(osThread(NewtonTask), &threadData[0]);

osThreadDef(BisectionTask, StartBisectionTask, osPriorityNormal, 0, 512);
taskHandles[1] = osThreadCreate(osThread(BisectionTask), &threadData[1]);

osThreadDef(SecantTask, StartSecantTask, osPriorityNormal, 0, 512);
taskHandles[2] = osThreadCreate(osThread(SecantTask), &threadData[2]);
```
## Synchronization & Safety
Mutex Protection: Uses osRecursiveMutex to prevent race conditions during OLED screen updates and global variable access.

Floating Point Safety: Includes checks for NaN (Not a Number) to handle non-convergent equations gracefully.

## 🛠 Hardware Configuration

MCU: STM32F103 (Cortex-M3) & Raspberry Pi 3.

Peripherals: SSD1306 OLED (I2C), 4x4 Matrix Keypad (GPIO).

RTOS: FreeRTOS (CMSIS-RTOS V1).