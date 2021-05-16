#include "configs.h"
#include "../models/meta.h"

#include <boost/unordered_set.hpp>
#include <iostream>
#include <vector>

using namespace boost::unordered;

int main(int argc, char *argv[]) {
    try {
        if (argc <= 1) {
            std::cout << "Wrong format! Template of this command is 'show' or 'update *config_name* *value*'\n";
            return 0;
        }
        std::string action = argv[1];
        if (action != "update" && action != "show") {
            std::cout
                    << "Wrong format! Second arg should be 'show' or 'update'. Template of this command is 'show' or 'update *config_name* *value*'\n";
            return 0;
        }
        if (action == "update" && argc < 4) {
            std::cout << "Wrong format! For update template is 'update *config_name* *value*'\n";
            return 0;
        }
        if (action == "update") {
            std::string config_name = argv[2];
            std::vector<std::string> value;
            for (int i = 3; i < argc; ++i) {
                value.push_back(argv[i]);
            }
            UpdateConfig(config_name, value);
        } else {
            ShowConfigsValue();
        }
    } catch (const std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }
}
