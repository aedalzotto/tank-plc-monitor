/**
 * @file Util.h
 * 
 * @author
 * Angelo Elias Dalzotto (150633@upf.br)
 * 
 * @brief A set of utilities to use with C++. Some code are a mix of what was
 * found searching online. The links for reference are with the source code.
 * 
 * @copyright
 * Copyright 2019 Angelo Elias Dalzotto
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <vector>
#include <string>

#ifdef _WIN32
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
    #define SP "\\"
#else
    #include <filesystem>
    namespace fs = std::filesystem;
    #define SP "/"
#endif

namespace Util {
    class String;
    class System;
};

class Util::String {
public:
    /**
     * @brief Explodes a string into a vector of strings separated by a character.
     * 
     * @param str The string to explode.
     * @param separator The character separator of the string.
     * 
     * @return A vector of strings exploded. Empty if no separator found.
     */
    static const std::vector<std::string> explode(const std::string& str, const char& separator);

    /**
     * @brief Checks if whole string is integer number.
     * 
     * @param str The string to check.
     * 
     * @return True if is integer.
     */
    static bool is_integer(const std::string &str);
};

class Util::System {
public:
    /**
     * @brief Calls an executable. Waits for it to return before continuing execution.
     * 
     * @param *argv[] Set of strings to call a program
     *                First is the program binary
     *                Sequence is arguments
     *                Last needs to be nullptr.
     * 
     * @return -1 if program not found.
     *         Return code of the program executed.
     */
    static int call_and_wait(int argc, char *argv[]);

    /**
     * @brief Gets the program binary file full path.
     * 
     * @return The string with the program path.
     */
    static std::string get_program_path();

    /**
     * @brief Copy a file. If it already exists, overwrites.
     * 
     * @details This is a workaround to messy std::filesystem bugs in MingW
     * 
     * @param src The source file
     * @param dst The destination file
     */
    static void copy_file_overwrite_workaround(const fs::path &src, const fs::path &dst);

    /**
     * @brief Copy contents of a folder to another. If file already exists, overwrites.
     * 
     * @details This is a workaround to messy std::filesystem bugs in MingW
     * 
     * @param src The source folder
     * @param dst The destination folder
     */
    static void merge_folders_overwrite_workaround(const fs::path &src, const fs::path &dst);

};

#endif /* _UTILS_H_ */