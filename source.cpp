#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>
#include <sstream>
#include <cstdlib>
#include <windows.h>
#include <algorithm>
#undef max

namespace fs = std::filesystem;

const std::string VALID_KEY = "realhash.on.top";
const std::string KEY_FILE = "key.txt";
const std::string CONFIG_FILE = "config.txt";

void check_key() {
    if (!fs::exists(KEY_FILE)) {
        std::ofstream key_file(KEY_FILE);
        key_file << "Enter your key here";
        std::cout << "\033[31m[\033[37m-\033[31m] Key file '" << KEY_FILE << "' created. Please enter the valid key and restart the script.\033[0m" << std::endl;
        std::cin.get();
        exit(0);
    }

    std::ifstream key_file(KEY_FILE);
    std::string key;
    std::getline(key_file, key);

    if (key != VALID_KEY) {
        std::cout << "\033[31m[\033[37m-\033[31m] Invalid key in '" << KEY_FILE << "'. Please enter the correct key and restart the script.\033[0m" << std::endl;
        std::cin.get();
        exit(0);
    }
}

void create_config_file_if_needed() {
    if (!fs::exists(CONFIG_FILE)) {
        std::ofstream config_file(CONFIG_FILE);
        config_file << "Your watermark text goes here.";
        std::cout << "\033[31m[\033[37m-\033[31m] Configuration file '" << CONFIG_FILE << "' not found. Creating default configuration.\033[0m" << std::endl;
        std::cout << "\n\033[36m    Press any key to continue...\033[0m";
        std::cin.get();
    }
}

void create_readme_in_folders(const std::string& folder_path, const std::string& watermark) {
    for (const auto& entry : fs::recursive_directory_iterator(folder_path)) {
        if (fs::is_directory(entry)) {
            const auto& dir_path = entry.path();
            const auto readme_file = dir_path / "README.md";
            if (!fs::exists(readme_file)) {
                std::ofstream readme_stream(readme_file);
                readme_stream << "# Rewind.hub\n\n" << watermark;
                std::cout << "\033[32m[\033[37m+\033[32m] README.md created in: \033[37m" << dir_path << "\033[0m" << std::endl;
            }
        }
    }
}

void add_watermark_to_files() {
    system("cls");
    std::cout << "\033[37m\n";
    std::cout << "                   ____  _______        _____ _   _ ____   _   _ _   _ ____  \n";
    std::cout << "                  |  _ \\| ____\\ \\      / /_ _| \\ | |  _ \\ | | | | | | | __ ) \n";
    std::cout << "                  | |_) |  _|  \\ \\ /\\ / / | ||  \\| | | | || |_| | | | |  _ \\ \n";
    std::cout << "                  |  _ <| |___  \\ V  V /  | || |\\  | |_| ||  _  | |_| | |_) |\n";
    std::cout << "                  |_| \\_\\_____|  \\_/\\_/  |___|_| \\_|____(_)_| |_|\\___/|____/ \n";
    std::cout << "                                                            \n";
    std::cout << "\n";
    std::cout << "                     \033[33m.:.:;.. \033[37mRewind.hub Mass Watermark Editor V2.6 \033[33m..:.:;.\n";
    std::cout << "                       \033[33m.:.:;.. \033[37m@ Rewind.hub. All Rights Reserved \033[33m..:.:;.\n";
    std::cout << " \n";

    std::string folder_path;
    std::cout << "\033[33m[\033[37m>\033[33m] Enter the folder path: \033[0m";
    std::getline(std::cin, folder_path);

    create_config_file_if_needed();

    std::ifstream config_file(CONFIG_FILE);
    std::string watermark((std::istreambuf_iterator<char>(config_file)), std::istreambuf_iterator<char>());

    char create_readme_choice;
    std::cout << "\033[33m[\033[37m?\033[33m] Do you want to create a README.md file with the watermark in every folder? (y/n): \033[0m";
    std::cin >> create_readme_choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (!fs::is_directory(folder_path)) {
        std::cout << "\033[31m[\033[37m-\033[31m] Folder '" << folder_path << "' does not exist.\033[0m\n";
        std::cout << "\n\033[36m    Press any key to continue...\033[0m";
        std::cin.get();
        return;
    }

    if (create_readme_choice == 'y' || create_readme_choice == 'Y') {
        create_readme_in_folders(folder_path, watermark);
    }

    for (const auto& entry : fs::recursive_directory_iterator(folder_path)) {
        const auto& file = entry.path();
        std::string extension = file.extension().string();

        static const std::map<std::string, std::string> comment_styles = {
            {".py", "#"}, {".lua", "--"}, {".cpp", "//"}, {".c", "//"}, {".java", "//"}, {".js", "//"},
            {".html", "<!--"}, {".css", "/*"}, {".php", "//"}, {".rb", "#"}, {".go", "//"}, {".sh", "#"},
            {".pl", "#"}
        };

        if (comment_styles.find(extension) == comment_styles.end()) continue;

        std::string comment_style = comment_styles.at(extension);
        std::string file_contents;

        std::ifstream file_stream(file);
        std::getline(file_stream, file_contents, '\0');

        if (extension == ".html") {
            watermark = "<!--\n" + watermark + "\n-->";
        } else if (extension == ".css") {
            watermark = "/*\n" + watermark + "\n*/";
        } else {
            std::stringstream watermark_stream;
            std::istringstream watermark_lines(watermark);
            std::string line;
            while (std::getline(watermark_lines, line)) {
                watermark_stream << comment_style << " " << line << "\n";
            }
            watermark = watermark_stream.str();
        }

        try {
            std::ofstream out_file(file);
            out_file << watermark << "\n\n" << file_contents;
            std::cout << "\033[32m[\033[37m+\033[32m] Watermark added to: \033[37m" << file << "\033[0m\n";
        } catch (const std::exception& e) {
            std::cout << "\033[31m[\033[37m-\033[31m] Error processing file \033[37m" << file << ": " << e.what() << "\033[0m\n";
        }
    }

    std::cout << "\n\033[36m    Press any key to continue...\033[0m";
    std::cin.get();
}

int main() {
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    while (true) {
        try {
            check_key();
            add_watermark_to_files();
        } catch (const std::exception& e) {
            std::cout << "\033[31mAn error occurred: " << e.what() << "\033[0m\n";
        }
    }
    
    return 0;
}
