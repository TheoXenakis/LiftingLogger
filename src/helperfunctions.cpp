/*---------------------------------------------------------------------------------------------------------------------*
 * This file contains helper functions for clearing the screen, clearing the input buffer, and error checking so my    *
 * main fle is more readable and better organized.                                                                     *
 * -------------------------------------------------------------------------------------------------------------------*/
#include <iostream>
#include <limits>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <conio.h>
#include <ctime>
#include <locale>
#include "helperfunctions.h"


using namespace std;

//checks if device is windows or mac/linux, sets clearScreen functions to act accordingly.
#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

//clears Screen of any existing text for better readability when running program in command prompt
void clearScreen() {
    system(CLEAR);
}

bool inputValid() {
    // Check if the input failed
    if (cin.fail()) {
        cin.clear(); // Clear the error state
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore the rest of the line
        cerr << "Invalid input. Please enter a valid number.\n";
        return false;
    }
    else {
        return true; //return true to indicate input was valid
    }
}

//takes the prompt string as input, then outputs the prompt, takes in the value the user provides and checks if the input is valid. If not, repeats prompt
double getValidatedInput(const string& prompt) {
    double value;
    while (true) {
        cout << prompt; //display prompt
        cin >> value;
        if (inputValid()) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
            return value; // Return valid input
        }

        // If input failed, the loop will continue and re-prompt the user
    }
}

void drawGraph(){}