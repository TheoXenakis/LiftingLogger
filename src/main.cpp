#include <iostream>
#include <limits>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <ctime>
#include "helperfunctions.h"
#include "json.hpp"

//---------------------------------------------------GAMEPLAN---------------------------------------------------------//

//*** OVERALL IMPROVEMENTS OUTLINE ***
//REMAINING TASKS
//organize functions among files for better readability and functionality ***DO LATER NO NEED RIGHT NOW JUST ENSURE FUNCTIONALITY THEN MOVE ONWARD ***
//filter options (only show push workouts, only show pull workouts, etc)

//*** PR FUNCTIONALITY OUTLINE ***
//need to have functionality so that when a user saves their lift to the exercises vector, the weight is saved alongside the number of reps completed with the weight.
//needs to determine if the current amount of reps is higher than the previous highest recorded number of reps for that weight
//if this is true, then display a little message saying 'Congrats on your PR!' Your strongest attempt of this exercise was *weight* lbs for *reps* reps on *date*

//*** FILTERS FUNCTIONALITY OUTLINE***
//functionality to view the progression of a specific lift over time
//essentially, have a filter in view lifts to be able to view specific exercises
//cycles through all the exercises you have done before, this can be done using a linked list or with a vector
//so FILTER BY: type of session, specific exercise, show all sessions
//if choosing type of session, or specific exercise, and user exits the menu
//return to menu and determine how to display differently
//for this menu, need to have the option to change the filter and then refresh the list

//*** WEIGHT TRACKING MACHINE LEARNING FUNCTIONALITY OUTLINE ***
//functionality so that you can also track your weight
//given the slope of the line in the trends, will approximate when you will reach your goal weight
//need to get the weight and date upon each entry
//calculate the difference between entries (in number of days)
//then determine the average rate of change between the number of days, use that information to predict when you will reach your goal weight

//--------------------------------------------------------------------------------------------------------------------//

//include for clarity and readability
using json = nlohmann::json;
using namespace std;

//--------------------------------------------------------------------------------------------------------------------//
//structs for exercise and session to store data
struct exercise {
    double weight, sets, reps;
    string name, notes;
};

struct session {
    string date;
    string type;
    string name;
    string notes;
    vector<exercise> exercises;
};

struct weightEntry {
    double weight;
    string date;
};
//sends exercise data to the json file
void to_json(json& j, const exercise& e) {
    j = json{{"name", e.name}, {"weight", e.weight}, {"sets", e.sets}, {"reps", e.reps}, {"notes", e.notes}};
}

//retrieve exercise data from the json file
void from_json(const json& j, exercise& e) {
    j.at("name").get_to(e.name);
    j.at("weight").get_to(e.weight);
    j.at("sets").get_to(e.sets);
    j.at("reps").get_to(e.reps);
    j.at("notes").get_to(e.notes);
}

//send session data to the json file
void to_json(json& j, const session& s) {
    j = json{{"date", s.date}, {"type", s.type}, {"name", s.name}, {"notes", s.notes}, {"exercises", s.exercises}};
}

//retrieve session data from the json file
void from_json(const json& j, session& s) {
    j.at("date").get_to(s.date);
    j.at("type").get_to(s.type);
    j.at("name").get_to(s.name);
    j.at("notes").get_to(s.notes);
    j.at("exercises").get_to(s.exercises);
}

//function to save sessions. Takes in sessions vector and the json filename ("sessions.json"), saves accordingly
void save_sessions(const vector<session>& sessions, const string& filename) {
    json j = sessions;
    ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    } else {
        cerr << "Unable to open file for writing: " << filename << endl;
    }
}

//function to load the sessions.json file
vector<session> load_sessions(const string& filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Unable to open file for reading: " << filename << endl;
        return {};
    }

    // Read the entire file into a string
    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();

    if (content.empty()) {
        return {};
    }

    json j;
    try {
        j = json::parse(content);
    } catch (const json::exception& e) {
        cerr << "Error parsing JSON: " << e.what() << endl;
        return {};
    }

    // Check if JSON object is empty
    if (j.is_null() || j.empty()) {
        cerr << "File contains invalid JSON or is empty: " << filename << endl;
        return {};
    }

    try {
        return j.get<vector<session>>();
    } catch (const json::exception& e) {
        cerr << "Error converting JSON to vector<session>: " << e.what() << endl;
        return {};
    }
}

//--------------------------------------------------------------------------------------------------------------------//
//function to log the lifts into the sessions.json file
void logLift(const string& type, const string& date) {

    clearScreen();//reset screen before menu appears

    //declare variables for functionality
    char booleanInput;
    string exerciseName, input;
    string notes = "";
    double weight;
    int sets, reps;

    //loads the sessions vector with the sessions in the "sessions.json" file
    vector<session> sessions = load_sessions("sessions.json");

    bool sessionExists = false;
    session currSession;

    //checks through the sessions vector, if the date and type match a pre-existing session, set current session to this session
    if (!sessions.empty()) {
        for (auto &s: sessions) {
            if (s.date == date && s.type == type) {
                currSession = s;
                sessionExists = true;
                break;
            }
        }
    }
    //if the session does not exist, set the current session information accordingly
    if (!sessionExists) {
        currSession.date = date;
        currSession.type = type;
        currSession.name = type + " | " + date;
    }

    while (true) {

        //print menu for this session
        cout << "+--------------------------- | " << type << " | " << date << " | ------------------------------+\n" << endl;
        cout << "Enter Exercise Name: ";
        cin >> ws;
        getline(cin, exerciseName);

        //retrieve information of exercise from user
        weight = getValidatedInput("Enter the weight used (in lbs): ");
        sets = static_cast<int>(getValidatedInput("Enter number of sets: "));
        reps = static_cast<int>(getValidatedInput("Enter number of reps: "));

        //check if the weight used was a PR, and if the number of reps was 1. Then it would be a ORM.
        //congratulate user on PR and display previous PR
        //check if the user has a higher number of reps then

        cout << "Would you like to add any additional notes? Enter y/n: ";
        cin >> booleanInput;
        if (booleanInput == 'y' || booleanInput == 'Y') {
            cout << "+----------------------------------------------------------------------------------------+" << endl;
            cout << "| " << exerciseName << " Notes: ";
            cin >> ws;
            getline(cin, notes);
            cout << "+----------------------------------------------------------------------------------------+" << endl;
        }
        else {
            notes = ""; //set notes to be empty string to overwrite existing value in notes variable (needed for viewLifts functionality and notes saving properly)
        }

        //create the exercise with this information
        exercise newExercise;
        newExercise.name = exerciseName;
        newExercise.weight = weight;
        newExercise.sets = sets;
        newExercise.reps = reps;
        newExercise.notes = notes;

        //add this exercise to the session
        currSession.exercises.push_back(newExercise);

        //Display confirmation the exercise was successfully logged. Prompt for additional exercises.
        cout << "\nLift Logged Successfully.\n";
        cout << "Would you like to log another exercise? Enter y/n: ";
        cin >> booleanInput;

        //restart process entering new exercise
        if (booleanInput == 'y' || booleanInput == 'Y') {
            cout << "\n" << endl;
            continue;

        } else if (booleanInput == 'n' || booleanInput == 'N') {

            //prompt user to add any additional notes for the workout session
            cout << "Would you like to log any notes from this session? Enter y/n:";
            cin >> booleanInput;

            //open notes section for entire session, save information to this section
            if (booleanInput == 'y' || booleanInput == 'Y') {
                cout << "+----------------------------------------------------------------------------------------+" << endl;
                cout << "| Session Notes: ";
                cin >> ws;
                getline(cin, notes);
                cout << "+----------------------------------------------------------------------------------------+" << endl;
            }
            else {
                notes = "";
            }
            currSession.notes = notes;

            //check if the name for this session already exists, if not, push this current session to the sessions vector
            if (!sessionExists) {
                sessions.push_back(currSession);

            } else { //check the session list, find the existing session and set to the current session
                for (auto& s : sessions) {
                    if (s.date == date && s.type == type) {
                        s = currSession;
                        break;
                    }
                }
            }
            //save the sessions vector to the json file
            save_sessions(sessions, "sessions.json");
            break;
        } else {
            cerr << "Invalid input, returning to main menu.\n";
            break;
        }
    }
}

//function to create a session
void createSession() {

    //creating values to store struct information here avoiding complexities of doing in logLift stage
    string workoutType;
    string sessionDate;

    clearScreen();

    //display list of workout types
    cout << "+------------------------------------------------------+" << endl;
    cout << "| Pick from those below or enter your own custom type. |" << endl;
    cout << "+------------------------------------------------------+" << endl;
    cout << "| Push | Pull | Legs | Chest + Back | Shoulders + Arms |" << endl;
    cout << "+------------------------------------------------------+" << endl;
    cout << "|          Upper Body | Lower Body | Full Body         |" << endl;
    cout << "+------------------------------------------------------+\n" << endl;

    cout << "Choose workout type:";
    cin >> ws;
    getline(cin, workoutType); //this is just getting the workoutType, when we save it with all the exercises later, set the type to be workoutType
    //now input is stored in workoutType

    // determine year, month, day values
    std::time_t current_time = std::time(nullptr);
    std::tm* local_time = std::localtime(&current_time);
    int year = local_time->tm_year + 1900;
    int month = local_time->tm_mon + 1;
    int day = local_time->tm_mday;

    //combine into date
    sessionDate = to_string(month) + "/" + to_string(day) + "/" + to_string(year);
    logLift(workoutType, sessionDate);

}

//function to view list of lifts
void viewLifts() {

    /*-----------------------------------------------------------------------------------------------------------------*
    * This function allows the user to view their logged exercises. Exercises can be displayed/organized in several    *
     * ways. There is functionality to display additional information such as their ORM for this specific lift. This   *
     * also contains functionality to show the improvement from the user's past exercise*                              *
    * ----------------------------------------------------------------------------------------------------------------*/

    clearScreen();//clear screen upon first function call

    // Declare variables
    string menuInput;
    string displayPeriod = "All Time"; // Variable for modifying the time period to display lift information
    string sortBy = "most recent";
    string showORM = "off";
    string showWorkouts =  "all";
    string input;

    while (true) {
        clearScreen();
        // Display menu with default display period
        cout << "+--------------| VIEW LIFTS |----------------+" << endl;
        cout << "|  Options: O  | Displaying Data:  " << displayPeriod << " |" << endl;
        cout << "|              | Sorting By: " << sortBy << " |" << endl;
        cout << "+--------------------------------------------+" << endl;

        //load the sessions in sessions.json
        vector<session> sessions = load_sessions("sessions.json");

        if (sessions.empty()) {
            cout << "No session data available or error reading the file." << endl;
            cout << "Press x to return to main menu" << endl;
            cin >> input;
            return;
        }

        if (sortBy == "most recent") {
            // Display sessions with indexes
            for (size_t i = 0; i < sessions.size(); ++i) {
                cout << "[" << i + 1 << "] " << sessions[i].name << endl;
            }
        }

        else if (sortBy == "least recent") {
            for (size_t i = sessions.size(); i > 0; i--) {
                cout << "[" << i - 1 << "] " << sessions[i - 1].name << endl;
            }
        }


        // End of menu display
        cout << "+-------------------------------------------+" << endl;
        cout << "| EXIT: X |                                 |" << endl;
        cout << "+-------------------------------------------+" << endl;
        cout << "Please enter which session you would like to view:";
        cin >> menuInput;

        if (menuInput == "o" || menuInput == "O") {
            while (true) {
                clearScreen();
                cout << "+--------------------------------------OPTIONS----------------------------------------+\n"
                     << endl;
                cout << "| Change Display Period | Currently Displaying Data from: " << displayPeriod << " | 1 |\n"
                     << endl;
                cout << "| Change Sorting Options | Currently Sorting by: " << sortBy << " | 2 |\n" << endl;
                cout << "| Show Estimated ORM | Currently set to:" << showORM << " | 3 |\n" << endl;
                cout << "| Show Workouts | Currently set to: " << showWorkouts << " | 4 |\n" << endl;
                cout << "| Clear all data from file | 5 |\n" << endl;
                cout << "+-------------------------------------------------------------------------------------+\n" << endl;
                // Open options menu
                // Change display period
                // Change sorting options ex. sort by exercise, sort by date (least recent -> most recent or opposite), sort by exercise day (shows results for just PUSH, just PULL, just LEGS), sort by
                // Include estimated ORM
                // Show % improvement from most recent time that exercise was performed
                // Show option for clearing all data on the file
                cin >> menuInput;
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore any leftover characters

                if (menuInput == "1") {
                    //cycle through all possibly display periods
                    if (displayPeriod == "All Time") {
                        displayPeriod = "This Week";
                    } else if (displayPeriod == "This Week") {
                        displayPeriod = "This Month";
                    } else if (displayPeriod == "This Month") {
                        displayPeriod = "Last 6 Months";
                    }
                    else if (displayPeriod == "Last 6 Months") {
                        displayPeriod = "All Time";
                    }
                }
                else if (menuInput == "2") {
                    if (sortBy == "most recent") {
                        sortBy = "least recent";
                    }
                    else if (sortBy == "least recent") {
                        sortBy = "most recent";
                    }
                }
                else if (menuInput == "3") {

                }
                else if (menuInput == "4") {
                    //show workouts
                    //need to open the json file
                    //read from the list of types
                    //have that list of types as options
                    //then be able to cycle through these options and then update the list of sessions with only those of the corresponding type

                }
                else if (menuInput == "5") {
                    cerr << "*** ARE YOU SURE YOU WANT TO CLEAR ALL DATA FROM THE FILE 'lifts.txt'? THIS CHANGE IS IRREVERSIBLE ***" << endl;
                    cout << "(y/n)" << endl;
                    cin >> menuInput;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore any leftover characters

                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cerr << "Invalid input, please re-enter.\n";
                        continue;
                    }

                    if (menuInput == "y" || menuInput == "Y") {
                        cout << "clearing the file" << endl;
                        ofstream ofs("sessions.json", ofstream::out |
                                                      ofstream::trunc); // Opens the file in write mode (out), then truncates all text. Preserves the file itself.

                        if (ofs.is_open()) {
                            cout << "File: 'sessions.json' was cleared successfully."
                                 << endl; //file was opened and now empty
                            ofs.close();
                        } else {
                            cerr << "File 'sessions.json' could not be opened to clear contents." << endl;
                        }
                        break;
                        // Clear the file
                    } else if (menuInput == "n" || menuInput == "N") {
                        break;
                    }
                }
                else if (menuInput == "x" || menuInput == "X") {
                    break;
                }
            }
        }
        else if (menuInput == "x" || menuInput == "X") {
            break;
        }

        //handle case when the user enters a number, then go into the corresponding session and show the exercises for that session
        else {
            int selectedIndex;
            //get user input as number, catch invalid input and display error message
            try {
                selectedIndex = stoi(menuInput) - 1; //subtract one to account for 0-based indexing
            } catch (const invalid_argument&) {
                cerr << "Input is invalid, please enter a valid number or enter 'x' to return." << endl;
                continue;
            }
            //check if selectedIndex is within correct bounds (checking >= 0 because already converted to 0-based indexing)
            if (selectedIndex >= 0 && selectedIndex < static_cast<int>(sessions.size())) {
                clearScreen();

                //passed error checks, proceed with printing list of exercises for the given section

                //print the session with the corresponding section number
                cout << "|      " << sessions[selectedIndex].name << "      |" << endl;
                cout << "+-------------------------| " << endl;
                cout << "|        Exercises        |" << endl;
                cout << "+-------------------------+" << endl;
                cout << "| Session Notes: " << sessions[selectedIndex].notes << " |" << endl;
                cout << "+-------------------------+" << endl;

                for (auto& ex : sessions[selectedIndex].exercises) { //iterates through each exercise within the session
                    cout << "| " << ex.name << endl;
                    cout << "| " << ex.weight << " lbs" << endl;
                    cout << "| " << ex.sets << " sets" << endl;
                    cout << "| " << ex.reps << " reps" << endl;
                    cout << "| " << "notes: " << ex.notes << endl;
                    cout << "+-------------------------+" << endl;
                }
                cout << "Press 'x' to return:" << endl;
                cin >> menuInput;

                if (menuInput == "x" || menuInput == "X") {
                    continue; //return to past menu
                }
            }
            else {
                cerr << "Invalid session number." << endl;
                continue;
            }
        }
    }
}

//function to open calculator interface for ORM and reps given ORM functionality
void calculator() {

    /*-----------------------------------------------------------------------------------------------------------------*
    * This function allows the user to calculate their ORM (one rep max), as well as display various weights as a % of *
    * their ORM so users can apply these weights into their exercise programs.                                         *
    * ----------------------------------------------------------------------------------------------------------------*/

    clearScreen();

    // Declare variables
    string menuInput;
    double weight;
    double tempWeight;
    double orm;
    double reps;
    double bodyWeight;
    double goalWeight = 0; //store goalWeight in JSON file each time
    double min;
    double max;
    int rangeUpperBound;
    int rangeLowerBound;

    while (true) {
        clearScreen();
        cout << "+---------------------+ CALCULATOR +---------------------+" << endl;
        cout << "| One Rep Max | Reps / Set |    Weight   |    Options    |" << endl;
        cout << "|      1      |     2      |      3      |       O       |" << endl;
        cout << "+-------------+------------+-------------+---------------+" << endl;

        // Take Menu Input
        cin >> menuInput;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore any leftover characters

        // Check if user input is a valid character
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cerr << "Invalid input, please re-enter." << endl;
            continue;
        }

        if (menuInput == "1") {
            clearScreen();
            // Prompt user for lift information
            cout << "What is your current working weight?" << endl;
            weight = getValidatedInput("Enter the weight used: ");
            reps = getValidatedInput("Enter the number of reps performed: ");

            // Perform and display calculation
            orm = weight / (1.0278 - (0.0278 * reps));
            cout << "\nYour one rep max is estimated to be: [" << orm << "].\n" << endl;
            cout << "Enter 'x' to return: ";
            cin >> menuInput;
            if (menuInput == "x" || menuInput == "X") {
                break;
            }
            else {
                cerr << "Invalid input, please re-enter.\n" << endl;
            }


        } else if (menuInput == "2") {
            clearScreen();
            // Prompt user for current one rep max value
            cout << "What is your current One Rep Max?" << endl;
            weight = getValidatedInput("Enter weight: ");

            cout << "\nDisplaying Information for your lift\n" << endl;

            // Perform calculations and display
            for (int i = 100; i >= 50; i -= 5) {
                tempWeight = weight * (0.01 * i);
                cout << i << "% of your ORM: " << tempWeight << "\n" << endl;
            }
            //prompt user to exit menu once information has been displayed
            cout << "Enter 'x' to return: ";
            cin >> menuInput;
            if (menuInput == "x" || menuInput == "X") {
                break;
            }
            else {
                cerr << "Invalid input, please re-enter.\n" << endl;
            }
            cout << "+-----------------------------------------+" << endl;

        } else if (menuInput == "3") {
            clearScreen();
            vector<weightEntry>weightLogger; //set to load weightLogger function when created to retrieve information from json file
            //ALSO MAKE SURE TO RETREIVE GOALWEIGHTVALUE

            cout << "+--------------------------------------------------------------------------------------------------------+" << endl;
            cout << "| GOAL WEIGHT: " << goalWeight << " |" << endl;
            cout << "Log your weight for today! (I recommend weighing yourself fasted in the morning for most accurate results.)" << endl;
            bodyWeight = getValidatedInput("Enter here: ");

            // determine year, month, day values to store information
            std::time_t current_time = std::time(nullptr);
            std::tm* local_time = std::localtime(&current_time);
            int year = local_time->tm_year + 1900;
            int month = local_time->tm_mon + 1;
            int day = local_time->tm_mday;

            weightEntry currEntry; //initialize current Entry

            //store information in weightEntry struct
            currEntry.date = to_string(month) + "/" + to_string(day) + "/" + to_string(year);
            currEntry.weight = bodyWeight;

            cout << "PRINT FOR ERROR CHECK" << endl;

            cout << currEntry.date << endl;
            cout << currEntry.weight << endl;

            //add the currentEntry to the weightLogger vector
            weightLogger.push_back(currEntry);

            //determine the weight difference between this entry and most recent entry
            double difference = weightLogger[weightLogger.size()-1].weight - weightLogger[weightLogger.size()-2].weight;

            //iterate through the vector to determine the lowest weight
            min = weightLogger[0].weight;
            for (int i = 0; i < weightLogger.size(); i++) {
                if (weightLogger[i].weight < min) {
                    min = weightLogger[i].weight;
                }
            }

            //iterate through the vector to determine the highest weight
            max = weightLogger[0].weight;
            for (int i = 0; i < weightLogger.size(); i++) {
                if (weightLogger[i].weight > max) {
                    max = weightLogger[i].weight;
                }
            }

            //determine the upper and lower bounds the graph will be drawn using min and max
            rangeUpperBound = static_cast<int>(max + 50);
            rangeLowerBound = static_cast<int>(min - 50);

            //FORLOOP STARTS HERE

            //determine the percentage the currentEntry was of the rangeUpperBound to determine percentile position
            double percentOfRange = currEntry.weight / rangeUpperBound;

            //convert this to a value out of 20
            double weightY = percentOfRange * 20;

            //subtract and cast to integer
            int newLinesToPlace = static_cast<int>(20 - weightY);

            //CALL DRAWGRAPH FUNCTION
            //drawGraph(fdsasffdas,fdsafdasfdas,fsdadfasasdf,fdasafdasfdas,fdafdasfsdafda,fdasaaaasdf);

            //average the weight change (total weight change / number of days between the first and most recent date)
            //estimate when user will reach their goal weight


            //TODO: CHECK CONDITION IF THE TOTAL NUMBER OF DATES IS LESS THAN 6, THEN DISPLAY DIFFERENTLY
            //TODO: IF THE NUMBER IF LESS THAN 6 THEN SIMPLY DISPLAY FROM LEFT TO RIGHT
            //TODO: IF THE NUMBER IS GREATER THAN 6, DIVIDE THE DATES EQUALLY AND SET THE RANGE FOR ALL DATES
            //TODO: GET THE NUMBER OF DAYS BETWEEN DATES WORKING! THIS IS IMPORTANT FOR ACTUALLY GRAPHING AS WELL AS JUST SIMPLY HANDLING THE MACHINE LEARNING ESTIMATION PROCESS
            //TODO: ONCE YOU HAVE ACCESS TO THE DAYS BETWEEN DATES YOU CAN FIND THE AVERAGE RATE OF CHANGE AND ESTIMATE THE NUMBER OF DAYS UNTIL USER REACHES THEIR GOAL, THEN ADJUST ACCORDINGLY TO DISPLAY PROPER DATE


        } else if (menuInput == "0") {
            clearScreen();
            cout << "+-----------------------------------OPTIONS-------------------------------------+" << endl;
            cout << "| Edit Goal Weight [1]                                                          |" << endl;
            cout << "|                                                                               |" << endl; //add here
            cout << "+-------------------------------------------------------------------------------+" << endl;
        }

        else if (menuInput == "X" || menuInput == "x") {
            // Exit menu
            break;

        } else {
            cerr << "Invalid input, please re-enter.\n" << endl;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------//
int main() {

    clearScreen(); //clear the screen at the start of the program running

    // Declare all variables
    string menuInput;

    while (true) {

        clearScreen();//clear again before printing menu
        // Print Main Menu
        cout << "+-------------+ LIFTING LOGGER +-------------+----------+" << endl;
        cout << "|  Log Lift   |   View Lifts   |  Calculator | Exit:  X |" << endl;
        cout << "|      1      |       2        |      3      |          |" << endl;
        cout << "+-------------+----------------+-------------+----------+" << endl;

        // Take Menu Input
        cin >> menuInput;

        if (menuInput == "1") {
            // Move to log-lift file
            createSession();
        } else if (menuInput == "2") {
            // Move to view-lifts file
            viewLifts();
        } else if (menuInput == "3") {
            // Move to calculator file
            calculator();
        } else if (menuInput == "x" || menuInput == "X") {
            // Close the program completely
            cerr << "Program Closing" << endl;
            break;
        } else {
            cerr << "Invalid input, please re-enter.\n";
        }
    }
}