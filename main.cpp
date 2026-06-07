/**
 * @file main.cpp
 * @brief Entry point and User Interface (UI) loop for the WinUpdater App.
 *
 * This file contains the main function which directs the user interface
 * flow, manages user inputs, and interfaces with the Updater class to
 * perform third-party application updates.
 *
 * @author GusWiz
 * @date 2026-06-06
 */

#include <iostream>
#include <string>
#include <limits>
#include "updater.h"

//-------------------------------------------------------------------------
// Helper Functions
//-------------------------------------------------------------------------

/**
 * @brief Clears the input buffer to prevent infinite loops on bad input.
 */
void ClearInputStream() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/**
 * @brief Displays the main menu to the user.
 */
void ShowMenu() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "          WinUpdater App" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "1. Check for application updates" << std::endl;
    std::cout << "2. Exit" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Enter your choice: ";
}

//-------------------------------------------------------------------------
// Main Entry Point
//-------------------------------------------------------------------------
int main() {
    WinUpdater::Updater updater;
    int choice = 0;
    bool isRunning = true;

    while (isRunning) {
        ShowMenu();
        if (!(std::cin >> choice)) {
            std::cout << "[ERROR] Invalid input. Please enter a number." << std::endl;
            ClearInputStream();
            continue;
        }

        switch (choice) {
            case 1: {
                // Step 1: Check if updates are needed
                bool updatesAvailable = updater.CheckForUpdates();

                if (updatesAvailable) {
                    // Step 2: Prompt for final confirmation
                    std::cout << "\n[PROMPT] Updates found. Would you like to install them now? (y/n): ";
                    char confirm;
                    std::cin >> confirm;

                    if (confirm == 'y' || confirm == 'Y') {
                        // Step 3: Run the update
                        std::cout << "\n[INFO] Starting updates..." << std::endl;
                        std::cout << "[INFO] If you need to stop the script, press 'Ctrl + C'." << std::endl;
                        std::cout << "[INFO] Windows will safely cancel at the next stopping point.\n" << std::endl;
                        
                        updater.ApplyUpdates();
                    } else {
                        std::cout << "[INFO] Update canceled by user." << std::endl;
                    }
                }
                break;
            }
            case 2:
                std::cout << "[INFO] Exiting application. Goodbye!" << std::endl;
                isRunning = false;
                break;
            default:
                std::cout << "[ERROR] Invalid choice. Please specify 1 or 2." << std::endl;
                break;
        }
    }

    return 0;
}