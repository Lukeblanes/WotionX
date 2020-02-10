#pragma once

#include <string>

enum class elemType {
    DEFAULT,
    RAW_TEXT,		// the text in a textCtrl
    STATIC_TEXT,	// the text in a staticText
    HEADER1,
    HEADER2,
    HEADER3,
    ITALICS,
    BOLD,
    STRIKETHROUGH,
    LIST_ELEMENT,	// ATM no difference in formatting
    NUM_LIST_ELEMENT,
    TASK_LIST_ELEMENT_NONMARKED,
    TASK_LIST_ELEMENT_MARKED,
    CODE_BLOCK,
    LINK_MD, // Link in chosen markdown format
    CHILD_NODE, // Similar to link_MD but contains to a child page it's pointing to
};

struct pageElement {
    elemType type;
    std::string cleanContent;
};

// Analyzes passed string with a state machine to see what type of element it is
pageElement parseElement(const std::string& content);
