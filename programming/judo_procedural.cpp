/*
================================================================================
 North Sussex Judo - Monthly Training Fee Calculator
 PROCEDURAL PROGRAMMING VERSION (C++)

 Client:     North Sussex Judo (via AQ Digital Solutions)
 Paradigm:   Procedural

 Why this is procedural:
   - The program is a sequence of function calls executed top-down from
     main().
   - Data (the Athlete struct) is a plain record with NO behaviour attached
     to it - it is simply a container of values.
   - All logic that operates on an Athlete (validation, cost calculation,
     display) lives in separate, independent functions instead of being a
     method of the data itself. This separation of data and behaviour is
     the defining characteristic of procedural programming.

 Compile:    g++ -std=c++17 -Wall -o judo_procedural judo_procedural.cpp
 Run:        ./judo_procedural
================================================================================
*/

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <limits>

using namespace std;

// ----------------------------------------------------------------------------
// Constants - pricing rules taken from the North Sussex Judo brief
// ----------------------------------------------------------------------------
const double BEGINNER_WEEKLY_FEE     = 25.00;
const double INTERMEDIATE_WEEKLY_FEE = 30.00;
const double ELITE_WEEKLY_FEE        = 35.00;
const double PRIVATE_COACHING_RATE   = 9.50;   // per hour
const double COMPETITION_FEE         = 22.00;  // per competition
const int    WEEKS_PER_MONTH         = 4;      // club assumes 4 weeks/month
const double MAX_WEEKLY_COACHING_HRS = 5.0;    // business rule

// ----------------------------------------------------------------------------
// Data structure - plain data only, no member functions (procedural style)
// ----------------------------------------------------------------------------
struct Athlete {
    string name;
    string trainingPlan;       // "Beginner", "Intermediate" or "Elite"
    double currentWeight;      // kg
    string weightCategory;     // e.g. "Middleweight"
    int    competitions;       // entered this month
    double coachingHours;      // private coaching hours per week
};

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void   showMainMenu();
void   clearInputError();
string getValidString(const string &prompt);
string getValidTrainingPlan();
double getValidWeight();
string getValidWeightCategory();
int    getValidCompetitions(const string &plan);
double getValidCoachingHours();
Athlete registerAthlete();

double getWeeklyFee(const string &plan);
double getWeightLimit(const string &category);
double calculateTrainingCost(const Athlete &a);
double calculateCoachingCost(const Athlete &a);
double calculateCompetitionCost(const Athlete &a);
double calculateTotalCost(const Athlete &a);
string compareWeight(const Athlete &a);

void displayAthlete(const Athlete &a);
void displayAllAthletes(const vector<Athlete> &athletes);
void displayPricingGuide();

// ----------------------------------------------------------------------------
int main() {
    vector<Athlete> athletes;   // dynamic store - club can register any number
    int choice = 0;

    cout << "================================================\n";
    cout << "  North Sussex Judo - Monthly Fee Calculator\n";
    cout << "  (Procedural Version)\n";
    cout << "================================================\n";

    // displayPricingGuide(); // shown once up-front so rates are visible before registering

    do {
        showMainMenu();
        cin >> choice;

        if (cin.fail()) {
            clearInputError();
            cout << "\nPlease enter a number between 1 and 4.\n\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                Athlete a = registerAthlete();
                athletes.push_back(a);
                cout << "\n" << a.name << " has been registered.\n\n";
                displayAthlete(a);
                break;
            }
            case 2:
                displayAllAthletes(athletes);
                break;
            case 3:
                displayPricingGuide();
                break;
            case 4:
                cout << "\nGoodbye!\n";
                break;
            default:
                cout << "\nPlease choose 1, 2, 3 or 4.\n\n";
        }
    } while (choice != 4);

    return 0;
}

// ----------------------------------------------------------------------------
void showMainMenu() {
    cout << "1. Register a new athlete\n";
    cout << "2. View all athletes and costs\n";
    cout << "3. View pricing & weight category guide\n";
    cout << "4. Exit\n";
    cout << "Enter your choice: ";
}

// ----------------------------------------------------------------------------
// Resets cin after a failed numeric read so the program can re-prompt
// instead of crashing or looping forever - this satisfies the brief's
// "display suitable messages then prompt again" error-handling rule.
// ----------------------------------------------------------------------------
void clearInputError() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ----------------------------------------------------------------------------
string getValidString(const string &prompt) {
    string value;
    do {
        cout << prompt;
        getline(cin, value);
        if (value.empty()) {
            cout << "This field cannot be empty. Please try again.\n";
        }
    } while (value.empty());
    return value;
}

// ----------------------------------------------------------------------------
string getValidTrainingPlan() {
    string plan;
    bool valid = false;

    do {
        cout << "Enter training plan (Beginner / Intermediate / Elite): ";
        getline(cin, plan);

        if (plan == "Beginner" || plan == "Intermediate" || plan == "Elite") {
            valid = true;
        } else {
            cout << "Invalid plan. Please type Beginner, Intermediate or Elite.\n";
        }
    } while (!valid);

    return plan;
}

// ----------------------------------------------------------------------------
double getValidWeight() {
    double weight;
    bool valid = false;

    do {
        cout << "Enter current weight in kg: ";
        cin >> weight;

        if (cin.fail() || weight <= 0) {
            clearInputError();
            cout << "Invalid weight. Please enter a positive number.\n";
        } else {
            valid = true;
        }
    } while (!valid);

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return weight;
}

// ----------------------------------------------------------------------------
string getValidWeightCategory() {
    string category;
    bool valid = false;

    do {
        cout << "Enter competition weight category\n"
             << "(Flyweight / Lightweight / Light-Middleweight / "
             << "Middleweight / Light-Heavyweight / Heavyweight): ";
        getline(cin, category);

        if (category == "Flyweight" || category == "Lightweight" ||
            category == "Light-Middleweight" || category == "Middleweight" ||
            category == "Light-Heavyweight" || category == "Heavyweight") {
            valid = true;
        } else {
            cout << "Invalid category. Please check spelling and try again.\n";
        }
    } while (!valid);

    return category;
}

// ----------------------------------------------------------------------------
// Business rule: only Intermediate and Elite athletes may enter competitions.
// ----------------------------------------------------------------------------
int getValidCompetitions(const string &plan) {
    if (plan == "Beginner") {
        cout << "Beginners cannot enter competitions this month "
             << "(0 entries recorded).\n";
        return 0;
    }

    int competitions;
    bool valid = false;
    do {
        cout << "Enter number of competitions entered this month: ";
        cin >> competitions;

        if (cin.fail() || competitions < 0) {
            clearInputError();
            cout << "Invalid entry. Please enter 0 or a positive whole number.\n";
        } else {
            valid = true;
        }
    } while (!valid);

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return competitions;
}

// ----------------------------------------------------------------------------
// Coaching is optional, capped at 5 hours per week (business rule). The
// figure entered is treated as hours PER WEEK and multiplied by the 4-week
// month later when the cost is calculated.
// ----------------------------------------------------------------------------
double getValidCoachingHours() {
    string answer;
    double hours = 0.0;

    cout << "Would you like to add private coaching hours? (y/n): ";
    getline(cin, answer);

    if (!answer.empty() && (answer[0] == 'y' || answer[0] == 'Y')) {
        bool valid = false;
        do {
            cout << "Enter private coaching hours per week (max "
                 << MAX_WEEKLY_COACHING_HRS << "): ";
            cin >> hours;

            if (cin.fail() || hours < 0 || hours > MAX_WEEKLY_COACHING_HRS) {
                clearInputError();
                cout << "Invalid entry. Hours must be between 0 and "
                     << MAX_WEEKLY_COACHING_HRS << ".\n";
            } else {
                valid = true;
            }
        } while (!valid);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    return hours;
}

// ----------------------------------------------------------------------------
Athlete registerAthlete() {
    Athlete a;
    cout << "\n--- Register New Athlete ---\n";
    a.name           = getValidString("Enter athlete name: ");
    a.trainingPlan   = getValidTrainingPlan();
    a.currentWeight  = getValidWeight();
    a.weightCategory = getValidWeightCategory();
    a.competitions   = getValidCompetitions(a.trainingPlan);
    a.coachingHours  = getValidCoachingHours();
    return a;
}

// ----------------------------------------------------------------------------
double getWeeklyFee(const string &plan) {
    if (plan == "Beginner")     return BEGINNER_WEEKLY_FEE;
    if (plan == "Intermediate") return INTERMEDIATE_WEEKLY_FEE;
    if (plan == "Elite")        return ELITE_WEEKLY_FEE;
    return 0.0; // unreachable - plan is always validated first
}

// ----------------------------------------------------------------------------
// Returns the upper weight limit (kg) for a category, or -1 for Heavyweight
// which has no upper limit.
// ----------------------------------------------------------------------------
double getWeightLimit(const string &category) {
    if (category == "Flyweight")          return 66.0;
    if (category == "Lightweight")        return 73.0;
    if (category == "Light-Middleweight") return 81.0;
    if (category == "Middleweight")       return 90.0;
    if (category == "Light-Heavyweight")  return 100.0;
    return -1.0; // Heavyweight
}

// ----------------------------------------------------------------------------
double calculateTrainingCost(const Athlete &a) {
    return getWeeklyFee(a.trainingPlan) * WEEKS_PER_MONTH;
}

// ----------------------------------------------------------------------------
double calculateCoachingCost(const Athlete &a) {
    return a.coachingHours * WEEKS_PER_MONTH * PRIVATE_COACHING_RATE;
}

// ----------------------------------------------------------------------------
double calculateCompetitionCost(const Athlete &a) {
    return a.competitions * COMPETITION_FEE;
}

// ----------------------------------------------------------------------------
double calculateTotalCost(const Athlete &a) {
    return calculateTrainingCost(a) + calculateCoachingCost(a)
         + calculateCompetitionCost(a);
}

// ----------------------------------------------------------------------------
// Compares the athlete's current weight against the limit for their chosen
// competition category and describes the result in plain English.
// ----------------------------------------------------------------------------
string compareWeight(const Athlete &a) {
    ostringstream oss;
    oss << fixed << setprecision(2);

    if (a.weightCategory == "Heavyweight") {
        if (a.currentWeight > 100.0) {
            oss << "Within category (Heavyweight has no upper limit).";
        } else {
            oss << "Below the typical Heavyweight range (over 100kg) - "
                << "consider a lower category.";
        }
        return oss.str();
    }

    double limit = getWeightLimit(a.weightCategory);
    if (a.currentWeight <= limit) {
        oss << "Within category limit (" << (limit - a.currentWeight)
            << "kg under the " << limit << "kg limit).";
    } else {
        oss << "Exceeds the " << limit << "kg category limit by "
            << (a.currentWeight - limit) << "kg.";
    }
    return oss.str();
}

// ----------------------------------------------------------------------------
void displayAthlete(const Athlete &a) {
    cout << fixed << setprecision(2);
    cout << "------------------------------------------------\n";
    cout << "Athlete:           " << a.name << "\n";
    cout << "Training Plan:     " << a.trainingPlan << "\n";
    cout << "Weight Category:   " << a.weightCategory << "\n";
    cout << "------------------------------------------------\n";
    cout << "Itemised Costs:\n";
    cout << "  Training fee:      GBP " << calculateTrainingCost(a) << "\n";
    cout << "  Private coaching:  GBP " << calculateCoachingCost(a) << "\n";
    cout << "  Competition fees:  GBP " << calculateCompetitionCost(a) << "\n";
    cout << "  ----------------------------\n";
    cout << "  TOTAL:             GBP " << calculateTotalCost(a) << "\n";
    cout << "------------------------------------------------\n";
    cout << "Weight check:      " << compareWeight(a) << "\n";
    cout << "------------------------------------------------\n\n";
}

// ----------------------------------------------------------------------------
void displayAllAthletes(const vector<Athlete> &athletes) {
    if (athletes.empty()) {
        cout << "\nNo athletes registered yet.\n\n";
        return;
    }

    double clubTotal = 0.0;
    cout << "\n================ ALL ATHLETES =================\n";
    for (const Athlete &a : athletes) {
        displayAthlete(a);
        clubTotal += calculateTotalCost(a);
    }
    cout << fixed << setprecision(2);
    cout << "Total monthly income for North Sussex Judo: GBP "
         << clubTotal << "\n";
    cout << "================================================\n\n";
}

// ----------------------------------------------------------------------------
// Prints the "Additional information" reference table from the client
// brief (training plan prices and weight category limits), so the user
// always has the current rates in front of them.
// ----------------------------------------------------------------------------
void displayPricingGuide() {
    cout << "\n================== PRICING & WEIGHT CATEGORY GUIDE ==================\n";
    cout << fixed << setprecision(2);

    cout << "Training Plan Prices (per week):\n";
    cout << "  Beginner (2 sessions/week):       GBP " << BEGINNER_WEEKLY_FEE << "\n";
    cout << "  Intermediate (3 sessions/week):   GBP " << INTERMEDIATE_WEEKLY_FEE << "\n";
    cout << "  Elite (5 sessions/week):          GBP " << ELITE_WEEKLY_FEE << "\n";
    cout << "  Private tuition (per hour):       GBP " << PRIVATE_COACHING_RATE << "\n";
    cout << "  Competition entry (per comp.):    GBP " << COMPETITION_FEE << "\n";

    cout << "\nWeight Categories (upper limit in kg):\n";
    cout << "  Heavyweight:                      Unlimited (over 100)\n";
    cout << "  Light-Heavyweight:                100\n";
    cout << "  Middleweight:                     90\n";
    cout << "  Light-Middleweight:               81\n";
    cout << "  Lightweight:                      73\n";
    cout << "  Flyweight:                        66\n";

    cout << "\nRules:\n";
    cout << "  - Maximum " << static_cast<int>(MAX_WEEKLY_COACHING_HRS) << " hours private coaching per week.\n";
    cout << "  - Only Intermediate and Elite athletes may enter competitions.\n";
    cout << "  - Competitions are held on the second Saturday of each month.\n";
    cout << "  - A month is assumed to be " << WEEKS_PER_MONTH << " weeks.\n";
    cout << "=======================================================================\n\n";
}
