#pragma once
#include <vector>
#include <memory>
#include <whb/proc.h>
#include <whb/log.h>
#include <whb/log_udp.h>
#include <whb/log_console.h>

namespace ui {
    class Menu {
        private:
            std::unique_ptr<std::vector<Menu>> submenus;
            std::string header;
            std::string generate_header_string(std::string header);
        public:
            Menu(std::string header);
            void input_event();
            void update();
            void open_submenu(int index);
    };

    class Entry {
        public:
            std::string label;
            Entry(std::string label, int type);
            std::unique_ptr<std::vector<Entry>> subEntries;
        protected:
            std::unique_ptr<Entry> parent;
            int layer;
            int type;
    };

    class SubmenuEntry: public Entry {
        private:
            SubmenuEntry(std::string label);
    };

    class SelectionEntry: public Entry {
        private:
            SelectionEntry(std::string label);
    };
}