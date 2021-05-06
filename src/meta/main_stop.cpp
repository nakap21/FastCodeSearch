#include <iostream>

#include "../models/meta.h"

int main() {
    try {
        Meta cur_meta;
        cur_meta.StopEngine();
        cur_meta.SaveMeta();
    } catch (const std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }
}
