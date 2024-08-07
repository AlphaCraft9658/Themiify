#pragma once
#include <vector>
#include <memory>
#include <whb/proc.h>
#include <whb/log.h>
#include <whb/log_udp.h>
#include <whb/log_console.h>
#include <utils/DrawUtils.h>

namespace ui {
    class Menu {
        private:
            std::unique_ptr<std::vector<Menu>> submenus;
            std::string header;
            std::string headerDecoration;
            void generate_header_decoration(std::string header);
        public:
            Menu(std::string header);
            void input_event();
            void update();
            void render_header();
            void open_submenu(int index);
            std::string get_header_string();
            std::string get_header_decoration();
    };

    class Entry {
        public:
            Entry(std::string label);
            std::unique_ptr<std::vector<Entry>> subEntries;
        protected:
            const std::string label;
            const std::unique_ptr<Entry> parent;
    };

    class SubmenuEntry: public Entry {
        public:
            SubmenuEntry(std::string label, Menu& submenu);
        private:
            const int type;
            const std::unique_ptr<Menu> submenu;
    };

    class SelectionEntry: public Entry {
        public:
            SelectionEntry(std::string label);
        private:
            const int type;
            bool state;
    };
}