#include "ui.h"
using namespace ui;
#define SUBMENU_ENTRY (int) 0;
#define SELECTION_ENTRY (int) 1;

Menu::Menu() {
    this->submenus = std::make_unique<std::vector<Menu>>();
}

Entry::Entry(std::string label="Empty Label", int type) {
    this->label = label;
    this->type = type;
}

SubmenuEntry::SubmenuEntry(std::string label) {
    Entry(label, SUBMENU_ENTRY);
}