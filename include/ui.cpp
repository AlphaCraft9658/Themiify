#include "ui.h"
using namespace ui;
#define SUBMENU_ENTRY (int) 0
#define SELECTION_ENTRY (int) 1

Menu::Menu(std::string header) : header(generate_header_string(header)) {
    this->submenus = std::make_unique<std::vector<Menu>>();
}

std::string Menu::generate_header_string(std::string header) {
    std::string headerString;
    for (int c=0; c < header.size() * 3; c++) {
        headerString.append("=");
    }
    headerString.append("\n");
    for (int c=0; c < header.size(); c++) {
        headerString.append(" ");
    }
    headerString.append(header);
    for (int c=0; c < header.size(); c++) {
        headerString.append(" ");
    }
    headerString.append("\n");
    for (int c=0; c < header.size() * 3; c++) {
        headerString.append("=");
    }
    return headerString;
}

void Menu::update() {
}

Entry::Entry(std::string label="Empty Label", int type) {
    this->label = label;
    this->type = type;
}

SubmenuEntry::SubmenuEntry(std::string label) : Entry(label, SUBMENU_ENTRY) {}

SelectionEntry::SelectionEntry(std::string label) : Entry(label, SUBMENU_ENTRY) {}
