#include "../models/meta.h"
#include "files.h"

#include <iostream>
#include <vector>
#include <unordered_set>

int main(int argc, char *argv[]) {
    try {
        if (argc <= 1) {
            std::cout << "Wrong format! Template of this command is 'add/delete *files*'\n";
            return 0;
        }
        std::string action = argv[1];
        if (action != "delete" && action != "add") {
            std::cout
                    << "Wrong format! Second arg should be 'add' or 'delete'. Template of this command is 'add/delete *files*'\n";
            return 0;
        }
        if (argc == 2) {
            std::cout << "Wrong format! There are no files to " << action
                      << ". Template of this command is 'add/delete *files*'\n";
            return 0;
        }

        std::vector<std::string> files;
        for (int i = 2; i < argc; ++i) {
            files.push_back(argv[i]);
        }

        if (action == "delete") {
            DeleteFiles(files);
        } else {
            AddFiles(files);
        }
    } catch (const std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }
}
