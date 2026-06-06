# WinUpdater App

## Purpose
This project is a C++ application designed to run a check for installed application updates on a Windows 11 device. If updates are available, it automatically updates them. 
Note: This tool specifically targets **third-party applications** and does not interact with core Windows System updates.

## Features
- **Basic UI / Menu**: Provides a clear and simple interface allowing the user to initiate an update check.
- **Confirmation Prompts**: Before any updates are actually applied, the application requires a final confirmation from the user to prevent accidental upgrades.
- **Safe Cancellation**: While the update script is running, the user is instructed on how to cancel the operation safely (e.g., using `Ctrl + C`). The underlying package manager (like `winget`) handles abrupt cancellations at safe stopping points, preventing application corruption.

---

## Code Structure

```text
WinUpdater App/
├── main.cpp          # Entry point and User Interface (UI) loop
├── updater.cpp       # Controller logic for checking and applying updates
├── updater.h         # Headers defining the updater interface
├── README.md         # Project documentation (this file)
└── build/            # Compiled binaries and object files
```

---

## Architecture

The project follows a **Separation of Concerns (Modular)** architecture, divided into three main layers:

1. **User Interface / Entry Layer (`main.cpp`)**
   - **Responsibility**: Handles all user interactions. Displays the menu, outputs the status, prompts for confirmation, and captures user input (or cancellation via `Ctrl + C`).
   - **Reasoning**: By keeping UI separate from the update logic, we ensure that the interface can be changed later (e.g., from a console text menu to a graphical UI like Qt or ImGui) without needing to rewrite the core update logic.

2. **Controller / Orchestrator Layer (`updater.cpp`)**
   - **Responsibility**: Acts as the brain of the application. It receives triggers from the UI, formats the correct commands (e.g., `winget upgrade`), reads the output to count how many apps need updates, and returns the status back to the UI. 
   - **Reasoning**: This makes the actual business logic easily testable. The controller doesn't care *how* the user clicked "update", it only knows that the update sequence needs to happen.

3. **Platform Execution Layer (Internal to `updater.cpp` or standalone)**
   - **Responsibility**: Interfaces safely with the Windows OS to run external commands (like `winget`). 
   - **Reasoning**: Wrapping standard Windows execution calls prevents command-line injection vulnerabilities and allows us to capture standard output (`stdout`) to show progress directly in our app, rather than popping up random terminal windows. It also allows safe transmission of interrupt signals if the user cancels.

## Why this Architecture?
- **Maintainability**: If Windows changes how it updates apps, you only update the Platform/Controller layer. The Entry/UI remains untouched.
- **Safety & Stability**: Handling the execution safely in its own scoped functionality allows the app to catch errors or interruptions (like `Ctrl+C`) gracefully, letting the Windows package manager roll back or cancel cleanly.
- **Extensibility**: It paves the way to easily add features like logging, configuration files, or skipping specific apps.
