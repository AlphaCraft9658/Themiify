#pragma once
#include <vector>
#include <memory>

namespace ui {
    class Menu {
        private:
            std::unique_ptr<std::vector<Menu>> submenus;
        public:
            Menu();
    };

    class Entry {
        public:
            std::string label;
            Entry(std::string label, int type);
        protected:
            int type;
    };

    class SubmenuEntry: public Entry {
        private:
            SubmenuEntry(std::string label);
    };
}