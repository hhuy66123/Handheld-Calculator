# Real-Time Parallel Handheld Calculator (STM32 & FreeRTOS)

This project develops a versatile handheld calculator capable of solving complex floating-point equations. It is implemented exclusively on the **STM32** microcontroller, demonstrating high-performance embedded software design and real-time task management.

A core feature of this project is the use of **FreeRTOS** to perform parallel computations, creating a "race" between algorithms to determine which numerical method solves a given equation the fastest.

---

## Key Features

### Real-Time Parallel Processing

The system leverages **FreeRTOS** to optimize mathematical performance through multi-threading:

- **Parallel Execution:** The system launches three concurrent tasks to solve equations using different numerical methods:
  - **Newton-Raphson Method**
  - **Bisection Method**
  - **Secant Method**
- **Race for the Solution:** Using a shared "found" flag and Mutex protection, the algorithm that converges to a valid root first wins the execution race.
- **Task Management & Optimization:** Once the fastest solution is found, the system automatically terminates the remaining tasks to conserve CPU resources and power.
- **Performance Benchmarking:** The execution time (with 0.1ms precision) is measured and displayed on the SSD1306 OLED screen, allowing for a direct comparison of algorithmic efficiency on limited hardware.

---

## Hardware Components

- **Microcontroller:** STM32F103 (Cortex-M3).
- **Display:** SSD1306 OLED (I2C Communication).
- **Input:** 4x5 Matrix Keypad for equation entry.

---

## Software Architecture

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

- **Mutex Protection**: Uses osRecursiveMutex to prevent race conditions during OLED screen updates and global variable access, ensuring data integrity in a multi-threaded environment.

- **Floating Point Safety** : Includes robust checks for NaN (Not a Number) and division-by-zero to handle non-convergent equations gracefully without crashing the system

## Hardware Configuration

- **MCU**: STM32F103 (Cortex-M3).

- **Peripherals**: SSD1306 OLED (I2C), 4x5 Matrix Keypad (GPIO).

- **RTOS**: FreeRTOS (CMSIS-RTOS V1).
