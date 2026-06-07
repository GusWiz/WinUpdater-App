# Understanding Namespaces in C++

In this project, the `Updater` class is wrapped within the `WinUpdater` namespace. This document explains what a namespace is and why we used it.

## What is a Namespace?

A **namespace** is a keyword used in C++ to define a scope. It groups related code (like classes, functions, and variables) together under a specific name.

Think of it like a **folder** on your computer. You can have two files named `readme.txt` as long as they are in different folders. Similarly, you can have two classes named `Updater` as long as they are in different namespaces.

## Why did we use `namespace WinUpdater`?

### 1. Preventing Name Collisions
The most common reason for using a namespace is to avoid "Name Collisions." If we ever include another library that also has a class called `Updater`, the compiler wouldn't know which one to use. By using `WinUpdater::Updater`, we make it clear that we want *our* version.

### 2. Code Organization
It keeps our code modular and organized. By looking at the code, any developer can immediately see that the logic belongs to the `WinUpdater` module of the application.

### 3. Clear Intent
Using a namespace signals that all the functions and classes inside it are related. In `main.cpp`, when you see `WinUpdater::Updater`, you know exactly which part of the system is being used.

## How to use it

### Definition in `updater.h` and `updater.cpp`:
```cpp
namespace WinUpdater {
    class Updater {
        // ... code ...
    };
}
```

### Accessing it in other files (like `main.cpp`):
You use the **scope resolution operator** (`::`) to "look inside" the namespace:

```cpp
#include "updater.h"

int main() {
    // Explicitly calling the Updater from the WinUpdater namespace
    WinUpdater::Updater myUpdater;
    myUpdater.CheckForUpdates();
    return 0;
}
```

### Difference between Class/Struct and Namespace

| Feature | Class/Struct | Namespace |
| :--- | :--- | :--- |
| **Instantiation** | You can create objects (`myObj`) | cannot be instantiated |
| **Access Control** | Has `public`, `private`, etc. | No access control (all public) |
| **Openness** | Defined once (closed) | Can be "re-opened" across files |
| **Purpose** | To define data and behavior | To organize and prevent conflicts |

## Access Control: Namespace vs Class

While a **namespace** makes everything inside it "public" (meaning anyone can see the functions or classes inside), the elements *inside* that namespace still respect their own rules.

In our project:
- `namespace WinUpdater` is **public**. Anyone can see that `class Updater` exists.
- `class Updater` has its own **private** and **public** sections.
- For example, `ExecuteCommand` is **private** within the class. Even though it is inside a public namespace, only the `Updater` class can use it.

---

## Preprocessor Directives & Conditional Compilation

In `updater.cpp`, we see code like this:
```cpp
#ifdef _WIN32
#define POPEN _popen
#else
#define POPEN popen
#endif
```

### What are they?
- **Preprocessor**: These commands (starting with `#`) are processed *before* the actual compilation starts.
- **`#ifdef _WIN32`**: "If Defined _WIN32". It checks if the code is being compiled on Windows. 
- **`#define`**: This creates a **Macro**. It tells the compiler "Every time you see `POPEN`, replace it with `_popen`."

### Why did we use them?
To make the code **portable**. Windows uses `_popen`, while Linux/macOS use `popen`. By using these directives, we can write our logic once using the name `POPEN`, and the preprocessor handles the system-specific naming for us.

---

## What is a "Pipe"?

In `ExecuteCommand`, we use a **Pipe**.

```cpp
std::unique_ptr<FILE, decltype(&PCLOSE)> pipe(POPEN(command.c_str(), "r"), PCLOSE);
```

### Definition
In programming, a **Pipe** is a form of redirection (a communication channel) used to pass information from one process to another.

### In this context:
1. Our program (the "Parent" process) wants to run `winget`.
2. `POPEN` starts `winget` as a "Child" process.
3. The **Pipe** connects the output of `winget` to our program.
4. We can then read that output line-by-line using `fgets` as if we were reading from a text file, even though it's actually coming from a live running command.

> **Note on Terminology**: Even though we use the `FILE` type, it is **not** a filepath or a physical file on the disk. It is a **Stream Handle**—a live connection to the data coming out of the `winget` process.

## Deep Dive: Smart Pointers and RAII

Line 41 in `updater.cpp` is a classic example of safe resource management:
```cpp
std::unique_ptr<FILE, decltype(&PCLOSE)> pipe(POPEN(command.c_str(), "r"), PCLOSE);
```

### 1. The `std::unique_ptr`
It is a **Smart Pointer**. It acts as a wrapper around a raw resource. Once the `pipe` variable goes out of scope (the function finishes), it **automatically** calls the cleanup function. No more manual "freeing" or "closing" required.

### 2. Template Syntax `< >`
C++ uses `< >` for **Templates**. It's like a fill-in-the-blank for the compiler.
- `FILE`: Tells the pointer it's managing a File object.
- `decltype(&PCLOSE)`: Tells the pointer the **type** of the function it needs to use to close the file.

### 3. The "Deleter" (`PCLOSE`)
Standard pointers just use `delete`. However, system pipes (created by `popen`) **must** be closed with `pclose`. By passing `PCLOSE` into the constructor, we are giving the smart pointer the "key" to properly shut down the process.

### Why do this?
If we used a regular pointer and an exception occurred halfway through the function, the pipe would stay open forever (a **Resource Leak**). The `unique_ptr` guarantees it closes no matter what.

---

## What is `c_str()`?

In `ExecuteCommand`, we use `command.c_str()`.

```cpp
std::unique_ptr<FILE, decltype(&PCLOSE)> pipe(POPEN(command.c_str(), "r"), PCLOSE);
```

### Definition
`c_str()` is a member function of the C++ `std::string` class. It stands for **"C-style string"**.

### Why use it?
- **Modern C++ Strings**: `std::string` is a complex object that manages memory for you.
- **Old C Functions**: Functions like `_popen` or `system()` are inherited from the original C language. They don't know what a `std::string` object is; they only understand a simple **array of characters** (`const char*`) ending in a null terminator (`\0`)

### What it does:
It returns a temporary pointer to the internal character array stored inside the `std::string`. This allows us to use modern, safe C++ strings while still being able to talk to older system functions.

---

## Understanding `fgets` and the Reading Loop

In `ExecuteCommand`, we read the command's output using a `while` loop:

```cpp
while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
    result += buffer.data();
}
```

### 1. What does `fgets` do?
`fgets` stands for **"File Get String"**. It does three things in one go:
1. **Destination**: It takes the first argument (`buffer.data()`) and says "I will store the text here."
2. **Limit**: It takes the second argument (`buffer.size()`) and says "I will only read up to 128 characters so I don't overflow the memory."
3. **Source**: It takes the third argument (`pipe.get()`) and says "I am reading this from the live `winget` process."

### 2. The Step-by-Step Process:
- **Step A (`fgets`)**: `fgets` reaches into the pipe, grabs a small chunk of text (one line or 127 characters), and places it into the `buffer`.
- **Step B (The Check)**: It checks if it actually found any data. If it did, it returns the buffer; if the process is finished, it returns `nullptr` and the loop stops.
- **Step C (`result += ...`)**: This is **Line 48**. Since `fgets` overwrites the `buffer` every time it runs, we need to save that data. We "append" (add) the contents of the buffer to our long `result` string.

### Summary
`fgets` handles the **transfer** of data from the process to the temporary `buffer`. 
Line 48 handles the **collection** of that data into a single final string that our program can actually use.

---

## What does `ExecuteCommand` do as a whole?

`ExecuteCommand` is a **Capture and Return** function.

1. **It Executes**: It sends your string (like `winget upgrade`) to the OS to be run.
2. **It Listens**: It connects to the "output stream" of that command.
3. **It Captures**: It records everything the command prints out.
4. **It Returns**: It hands that record back to your program as a `std::string`.

This allows your code to "see" what happened in the terminal and make logical decisions based on that text output.
