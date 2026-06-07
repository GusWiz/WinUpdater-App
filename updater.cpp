/**
 * @file updater.cpp
 * @brief Implementation file for the application updater module.
 *
 * This file contains the implementation details for the Updater class.
 * It utilizes Windows-specific commands (such as winget) to safely
 * evaluate and execute updates for third-party programs.
 * 
 * @author GusWiz
 * @date 2026-06-06
 */

#include "updater.h"
#include <iostream>
#include <memory> // provides smart pointer to prevent resource leaks.
#include <array>
#include <stdexcept> 

// Platform specific macros
#ifdef _WIN32
#define POPEN _popen // this makes the compilar replace POPEN with _popen when ran in Windows system.
#define PCLOSE _pclose
#else
// For non-Windows platforms (if portability was eventually needed)
#define POPEN popen
#define PCLOSE pclose
#endif

namespace WinUpdater {

    //-------------------------------------------------------------------------
    // ExecuteCommand
    //-------------------------------------------------------------------------
    std::string Updater::ExecuteCommand(const std::string& command) const {
        std::array<char, 128> buffer;
        std::string result;

        // Open a pipe to the command for reading the output.
        // Note: _popen is sufficient for simple use cases, though CreateProcess
        // would provide tighter security and better thread-cancel capabilities.
        std::unique_ptr<FILE, decltype(&PCLOSE)> pipe(POPEN(command.c_str(), "r"), PCLOSE);
        if (!pipe) {
            throw std::runtime_error("ExecuteCommand: Failed to open pipe for command execution.");
        }

        // Read the standard output from the command into our string buffer.
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
            result += buffer.data();
        }

        return result;
    }

    //-------------------------------------------------------------------------
    // CheckForUpdates
    //-------------------------------------------------------------------------
    bool Updater::CheckForUpdates() {
        std::cout << "[INFO] Checking for available third-party application updates..." << std::endl;
        
        try {
            // Use 'winget upgrade' to list available updates. 
            // We append typical flags to prevent hanging on prompts.
            std::string cmd = "winget upgrade --accept-source-agreements";
            std::string output = ExecuteCommand(cmd);

            // "No applicable update found" is the general string winget outputs 
            // when everything is up-to-date.
            // If we don't find this string, updates likely exist.
            if (output.find("No applicable update found") != std::string::npos || 
                output.find("No installed package found matching input criteria") != std::string::npos) {
                std::cout << "[INFO] All applications are up to date." << std::endl;
                return false;
            }
            
            // Assume updates are available
            std::cout << "[INFO] Updates are available." << std::endl;
            std::cout << output;
            return true;

        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Exception checking for updates: " << e.what() << std::endl;
            return false;
        }
    }

    //-------------------------------------------------------------------------
    // ApplyUpdates
    //-------------------------------------------------------------------------
    bool Updater::ApplyUpdates() {
        std::cout << "[INFO] Applying updates... (Press Ctrl + C to cancel safely)" << std::endl;

        try {
            // Apply all upgrades. 
            // --all implies upgrading everything.
            // --accept-package-agreements and --accept-source-agreements skip interactive prompts.
            std::string cmd = "winget upgrade --all --accept-package-agreements --accept-source-agreements";
            
            // For the application process, instead of catching all output silently, we can
            // route the command directly to the system to let the user see the progress bars.
            // Using system() here allows the OS to handle stdout directly for progress visualization.
            int retCode = std::system(cmd.c_str());

            if (retCode == 0) {
                std::cout << "[SUCCESS] Applications updated successfully." << std::endl;
                return true;
            } else {
                std::cerr << "[WARNING] Update process finished with non-zero exit code: " << retCode << std::endl;
                std::cerr << "[WARNING] This could indicate some packages failed, or the user canceled the process." << std::endl;
                return false;
            }

        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Exception applying updates: " << e.what() << std::endl;
            return false;
        }
    }

} // namespace WinUpdater