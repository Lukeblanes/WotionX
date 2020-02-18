#include "elemParser.h"

int newState(char currentChar, int currentState)
{
    int returnNewState = 0;
    switch (currentState) {
        case 0: {
            if (currentChar == '#') returnNewState = 1;
            else if (currentChar == '-') returnNewState = 4;
            else if (currentChar == '*' || currentChar == '+') returnNewState = 12;
            else if (currentChar == '[') returnNewState = 13;
            else if ( currentChar == '^') returnNewState = 19;
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 1: {
            if (currentChar == ' ') returnNewState = 21; // END_H1
            else if (currentChar == '#') returnNewState = 2;
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 2: {
            if (currentChar == ' ') returnNewState = 22; // END_H2
            else if (currentChar == '#') returnNewState = 3;
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 3: {
            if (currentChar == ' ') returnNewState = 23; // END_H3
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 4: {
            if (currentChar == ' ') returnNewState = 5;
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 5: {
            if (currentChar == '[') returnNewState = 6;
            else returnNewState = 11; // END_LISTELEMENT
            break;
        }
        case 6: {
            if (currentChar == ' ') returnNewState = 7;
            else if (currentChar == 'x') returnNewState = 9;
            else returnNewState = 11; // END_LISTELEMENT
        break;
        }
        case 7: {
            if (currentChar == ']') returnNewState = 8; // END_TASKLIST_NONMARKED
            else returnNewState = 11; // END_LISTELEMENT
            break;
        }
         case 9: {
            if (currentChar == ']') returnNewState = 10; // END_TASKLIST_MARKED
            else returnNewState = 11; // END_LISTELEMENT
            break;
        }
        case 12: {
            if (currentChar == ' ') returnNewState = 11; // END_LISTELEMENT
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 13: {
            // appart from uri we add space :
            std::string allowedChars = "./_~-abcdefghijklmnopqrstuvwzyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 :'";
            if( allowedChars.find(currentChar) != std::string::npos) returnNewState = 13;
            else if (currentChar == ']') returnNewState = 15;
            else if( currentChar == '#' ) returnNewState = 34;
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 15: {
            if( currentChar == '(' ) returnNewState = 16;
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 16: {
            if( currentChar == 'h' ) returnNewState = 26;
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 19: {
            if ( currentChar == ' ') returnNewState = 20; // END_RAWTEXT
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 26: {
            if( currentChar == 't' ) returnNewState = 27;
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 27: {
            if( currentChar == 't' ) returnNewState = 28;
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 28: {
            if( currentChar == 'p' ) returnNewState = 29;
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 29: {
            if( currentChar == 's' ) returnNewState = 31;
            else if( currentChar == ':' ) returnNewState = 30;
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 30: {
            if( currentChar == '/' ) returnNewState = 32;
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 31: {
            if( currentChar == ':' ) returnNewState = 30;
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 32: {
            if( currentChar == '/' ) returnNewState = 33;
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 33: {
            // Unreserved URI allowed Chars
            std::string allowedChars = "./_~-abcdefghijklmnopqrstuvwzyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789?=&";
            if( allowedChars.find(currentChar) != std::string::npos) returnNewState = 33;
            else if( currentChar == ')' ) returnNewState = 18; // END_LINK_MD
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 34: {
            // Unreserved URI allowed Chars
            std::string allowedChars = "0123456789";
            if( allowedChars.find(currentChar) != std::string::npos) returnNewState = 35;
            else returnNewState = -1; // END_DEFAULT
            break;
        }
        case 35: {
            // [0-9]
            std::string allowedChars = "0123456789";
            if( allowedChars.find(currentChar) != std::string::npos) returnNewState = 35;
            else if( currentChar == ']' ) returnNewState = 36; // END_CHILD_NODE
            else returnNewState = -1; // END_DEFAULT
            break;
        }
    }
    return returnNewState;
}

pageElement parseElement(const std::string& content)
{
    int state = 0;

    for (int i = 0; i < content.size(); ++i) {
        state = newState(content[i], state);
        if ( state == -1) {
            return {elemType::STATIC_TEXT, content};
        } else if (state == 21) {
            return {elemType::HEADER1, content.substr(2)};
        } else if (state == 22) {
            return {elemType::HEADER2, content.substr(3)};
        } else if (state == 23) {
            return {elemType::HEADER3, content.substr(4)};
        } else if (state == 11) {
            return {elemType::LIST_ELEMENT, content.substr(2)};
        } else if (state == 8) {
            return {elemType::TASK_LIST_ELEMENT_NONMARKED, content.substr(5)};
        } else if (state == 10) {
            return {elemType::TASK_LIST_ELEMENT_MARKED, content.substr(5)};
        } else if (state == 20) {
            return {elemType::RAW_TEXT, content.substr(2)};
        } else if (state == 18) {
            return {elemType::LINK_MD, content};
        } else if (state == 36) { // We remove the [# from the start and ] from the end
            return {elemType::CHILD_NODE, content.substr(2, content.length()-3)};
        }

    }

    // It should never enter here
    return {elemType::DEFAULT, content.substr(0)};
}

