/**
 * @file updater.h
 * @brief Header file for the application updater module.
 *
 * This file contains the declarations for the Updater class, which is responsible
 * for interacting with the Windows Package Manager (winget) to query and install
 * updates for third-party applications on the system.
 *
 * @author GusWiz
 * @date 2026-06-06
 */

#ifndef UPDATER_H
#define UPDATER_H

#include <string>

namespace WinUpdater {

    /**
     * @class Updater
     * @brief A controller class that manages application updates.
     *
     * The Updater class encapsulates the logic requiring interfacing with the host
     * operating system capabilities, primarily invoking external processes to check
     * for and perform application updates.
     */
    class Updater {
    public:
        /**
         * @brief Default constructor.
         */
        Updater() = default;

        /**
         * @brief Default destructor.
         */
        ~Updater() = default;

        /**
         * @brief Checks the system for available third-party application updates.
         *
         * Executes a non-intrusive query to retrieve a count and/or list of
         * available app updates without installing them.
         *
         * @return true if updates are available, false otherwise.
         */
        bool CheckForUpdates();

        /**
         * @brief Initiates the update process for all eligible apps.
         *
         * Exclusively targets third-party applications via the package manager
         * (e.g., winget upgrade --all). Logs output to standard streams.
         *
         * @return true if the updates were initiated and completed successfully
         *         or if partial success occurred (depending on package behavior),
         *         false if the process failed or was severely interrupted.
         */
        bool ApplyUpdates();

    private:
        /**
         * @brief Helper function to execute a system command and capture its output.
         *
         * @param command The command-line string to be executed.
         * @return std::string containing the standard output resulting from the command.
         */
        std::string ExecuteCommand(const std::string& command) const;
    };

} // namespace WinUpdater

#endif // UPDATER_H
