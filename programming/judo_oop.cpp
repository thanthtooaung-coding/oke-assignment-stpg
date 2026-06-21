/*
================================================================================
 North Sussex Judo - Monthly Training Fee Calculator
 OBJECT-ORIENTED PROGRAMMING VERSION (C++)

 Client:     North Sussex Judo (via AQ Digital Solutions)
 Paradigm:   Object-Oriented

 OOP characteristics demonstrated:
   - Encapsulation : Athlete keeps its data private and only exposes
                      behaviour (calculateTotalCost, displaySummary, etc.)
                      through public methods.
   - Inheritance    : TrainingPlan is an abstract base class; Beginner,
                      Intermediate and Elite plans each inherit from it and
                      provide their own fee/eligibility rules.
   - Polymorphism   : Athlete holds a TrainingPlan pointer and calls
                      plan->getWeeklyFee() / plan->canEnterCompetitions()
                      without knowing which derived plan it is - the
                      correct version runs automatically at runtime.
   - Abstraction    : JudoClub and Athlete hide HOW a cost is worked out
                      behind simple method names like calculateTotalCost().

 Compile:    g++ -std=c++17 -Wall -o judo_oop judo_oop.cpp
 Run:        ./judo_oop
================================================================================
*/

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <iomanip>
#include <sstream>
#include <limits>

using namespace std;

// ----------------------------------------------------------------------------
// TrainingPlan hierarchy (inheritance + polymorphism)
// ----------------------------------------------------------------------------
class TrainingPlan {
public:
    virtual ~TrainingPlan() = default;
    virtual string getName() const = 0;
    virtual double getWeeklyFee() const = 0;
    virtual int    getSessionsPerWeek() const = 0;
    virtual bool   canEnterCompetitions() const = 0;
};

class BeginnerPlan : public TrainingPlan {
public:
    string getName() const override { return "Beginner"; }
    double getWeeklyFee() const override { return 25.00; }
    int    getSessionsPerWeek() const override { return 2; }
    bool   canEnterCompetitions() const override { return false; }
};

class IntermediatePlan : public TrainingPlan {
public:
    string getName() const override { return "Intermediate"; }
    double getWeeklyFee() const override { return 30.00; }
    int    getSessionsPerWeek() const override { return 3; }
    bool   canEnterCompetitions() const override { return true; }
};

class ElitePlan : public TrainingPlan {
public:
    string getName() const override { return "Elite"; }
    double getWeeklyFee() const override { return 35.00; }
    int    getSessionsPerWeek() const override { return 5; }
    bool   canEnterCompetitions() const override { return true; }
};

// Factory function - turns a validated plan name into the right object.
// This is the one place that needs to know about all three derived classes.
shared_ptr<TrainingPlan> createTrainingPlan(const string &planName) {
    if (planName == "Beginner")     return make_shared<BeginnerPlan>();
    if (planName == "Intermediate") return make_shared<IntermediatePlan>();
    if (planName == "Elite")        return make_shared<ElitePlan>();
    return nullptr; // unreachable - planName is always validated first
}

// ----------------------------------------------------------------------------
// WeightCategory - encapsulates the weight-category lookup table and the
// logic for comparing an athlete's weight against it.
// ----------------------------------------------------------------------------
class WeightCategory {
private:
    string name;
    double upperLimit; // kg; -1 means unlimited (Heavyweight)

public:
    WeightCategory(string n, double limit) : name(move(n)), upperLimit(limit) {}

    string getName() const { return name; }

    // Human-readable limit, reused by both compareWeight() below and the
    // pricing guide later in this file - one place owns the wording.
    string getLimitDescription() const {
        if (upperLimit < 0) return "Unlimited (over 100)";
        ostringstream oss;
        oss << fixed << setprecision(0) << upperLimit;
        return oss.str();
    }

    string compareWeight(double currentWeight) const {
        ostringstream oss;
        oss << fixed << setprecision(2);

        if (upperLimit < 0) { // Heavyweight
            if (currentWeight > 100.0) {
                oss << "Within category (Heavyweight has no upper limit).";
            } else {
                oss << "Below the typical Heavyweight range (over 100kg) - "
                    << "consider a lower category.";
            }
            return oss.str();
        }

        if (currentWeight <= upperLimit) {
            oss << "Within category limit (" << (upperLimit - currentWeight)
                << "kg under the " << upperLimit << "kg limit).";
        } else {
            oss << "Exceeds the " << upperLimit << "kg category limit by "
                << (currentWeight - upperLimit) << "kg.";
        }
        return oss.str();
    }

    // Static "lookup table" - the only place that knows the kg limits.
    static WeightCategory lookup(const string &categoryName) {
        if (categoryName == "Flyweight")          return WeightCategory("Flyweight", 66.0);
        if (categoryName == "Lightweight")        return WeightCategory("Lightweight", 73.0);
        if (categoryName == "Light-Middleweight") return WeightCategory("Light-Middleweight", 81.0);
        if (categoryName == "Middleweight")       return WeightCategory("Middleweight", 90.0);
        if (categoryName == "Light-Heavyweight")  return WeightCategory("Light-Heavyweight", 100.0);
        return WeightCategory("Heavyweight", -1.0);
    }
};

// ----------------------------------------------------------------------------
// Athlete - encapsulates an athlete's data AND the behaviour that acts on it.
// This is the key difference from the procedural version, where the same
// data (struct Athlete) and behaviour (free functions) were kept separate.
// ----------------------------------------------------------------------------
class Athlete {
private:
    static const int WEEKS_PER_MONTH = 4;
    static constexpr double PRIVATE_COACHING_RATE = 9.50;
    static constexpr double COMPETITION_FEE       = 22.00;

    string name;
    double currentWeight;
    WeightCategory weightCategory;
    shared_ptr<TrainingPlan> plan;
    int competitions;
    double weeklyCoachingHours;

public:
    Athlete(string name, double weight, WeightCategory category,
            shared_ptr<TrainingPlan> plan, int competitions, double coachingHours)
        : name(move(name)), currentWeight(weight), weightCategory(move(category)),
          plan(move(plan)), competitions(competitions),
          weeklyCoachingHours(coachingHours) {}

    string getName() const { return name; }

    // Public accessors for the otherwise-private pricing constants, so the
    // pricing guide (a free function further down) has one source of
    // truth instead of duplicating these figures as separate literals.
    static double getCoachingRate()   { return PRIVATE_COACHING_RATE; }
    static double getCompetitionFee() { return COMPETITION_FEE; }
    static int    getWeeksPerMonth()  { return WEEKS_PER_MONTH; }

    double calculateTrainingCost() const {
        return plan->getWeeklyFee() * WEEKS_PER_MONTH;
    }

    double calculateCoachingCost() const {
        return weeklyCoachingHours * WEEKS_PER_MONTH * PRIVATE_COACHING_RATE;
    }

    double calculateCompetitionCost() const {
        return competitions * COMPETITION_FEE;
    }

    double calculateTotalCost() const {
        return calculateTrainingCost() + calculateCoachingCost()
             + calculateCompetitionCost();
    }

    void displaySummary() const {
        cout << fixed << setprecision(2);
        cout << "------------------------------------------------\n";
        cout << "Athlete:           " << name << "\n";
        cout << "Training Plan:     " << plan->getName() << "\n";
        cout << "Weight Category:   " << weightCategory.getName() << "\n";
        cout << "------------------------------------------------\n";
        cout << "Itemised Costs:\n";
        cout << "  Training fee:      GBP " << calculateTrainingCost() << "\n";
        cout << "  Private coaching:  GBP " << calculateCoachingCost() << "\n";
        cout << "  Competition fees:  GBP " << calculateCompetitionCost() << "\n";
        cout << "  ----------------------------\n";
        cout << "  TOTAL:             GBP " << calculateTotalCost() << "\n";
        cout << "------------------------------------------------\n";
        cout << "Weight check:      "
             << weightCategory.compareWeight(currentWeight) << "\n";
        cout << "------------------------------------------------\n\n";
    }
};

// Out-of-class definitions required pre-C++17 for constexpr statics; kept
// here for compiler compatibility even though C++17 inlines them already.
constexpr double Athlete::PRIVATE_COACHING_RATE;
constexpr double Athlete::COMPETITION_FEE;

// ----------------------------------------------------------------------------
// JudoClub - manages the collection of registered athletes (an example of
// one object, the club, being composed of many other objects, the athletes).
// ----------------------------------------------------------------------------
class JudoClub {
private:
    vector<Athlete> athletes;

public:
    void addAthlete(const Athlete &a) {
        athletes.push_back(a);
    }

    void displayAll() const {
        if (athletes.empty()) {
            cout << "\nNo athletes registered yet.\n\n";
            return;
        }

        double clubTotal = 0.0;
        cout << "\n================ ALL ATHLETES =================\n";
        for (const Athlete &a : athletes) {
            a.displaySummary();
            clubTotal += a.calculateTotalCost();
        }
        cout << fixed << setprecision(2);
        cout << "Total monthly income for North Sussex Judo: GBP "
             << clubTotal << "\n";
        cout << "================================================\n\n";
    }
};

// ----------------------------------------------------------------------------
// Input validation helpers. These remain free functions because they are
// console I/O details, not behaviour that belongs to Athlete or TrainingPlan.
// ----------------------------------------------------------------------------
void clearInputError() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

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

string getValidTrainingPlanName() {
    string plan;
    do {
        cout << "Enter training plan (Beginner / Intermediate / Elite): ";
        getline(cin, plan);
        if (plan != "Beginner" && plan != "Intermediate" && plan != "Elite") {
            cout << "Invalid plan. Please type Beginner, Intermediate or Elite.\n";
            plan.clear();
        }
    } while (plan.empty());
    return plan;
}

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

string getValidWeightCategoryName() {
    string category;
    do {
        cout << "Enter competition weight category\n"
             << "(Flyweight / Lightweight / Light-Middleweight / "
             << "Middleweight / Light-Heavyweight / Heavyweight): ";
        getline(cin, category);
        if (category != "Flyweight" && category != "Lightweight" &&
            category != "Light-Middleweight" && category != "Middleweight" &&
            category != "Light-Heavyweight" && category != "Heavyweight") {
            cout << "Invalid category. Please check spelling and try again.\n";
            category.clear();
        }
    } while (category.empty());
    return category;
}

// Polymorphic call: the same line of code (plan->canEnterCompetitions())
// behaves differently depending on whether `plan` points to a Beginner,
// Intermediate or Elite object.
int getValidCompetitions(const shared_ptr<TrainingPlan> &plan) {
    if (!plan->canEnterCompetitions()) {
        cout << plan->getName()
             << " athletes cannot enter competitions this month "
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

double getValidCoachingHours() {
    const double MAX_HOURS = 5.0;
    string answer;
    double hours = 0.0;

    cout << "Would you like to add private coaching hours? (y/n): ";
    getline(cin, answer);

    if (!answer.empty() && (answer[0] == 'y' || answer[0] == 'Y')) {
        bool valid = false;
        do {
            cout << "Enter private coaching hours per week (max "
                 << MAX_HOURS << "): ";
            cin >> hours;
            if (cin.fail() || hours < 0 || hours > MAX_HOURS) {
                clearInputError();
                cout << "Invalid entry. Hours must be between 0 and "
                     << MAX_HOURS << ".\n";
            } else {
                valid = true;
            }
        } while (!valid);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return hours;
}

// ----------------------------------------------------------------------------
// Gathers validated input and assembles it into the correct collaborating
// objects (TrainingPlan + WeightCategory + Athlete).
// ----------------------------------------------------------------------------
Athlete registerNewAthlete() {
    cout << "\n--- Register New Athlete ---\n";
    string name         = getValidString("Enter athlete name: ");
    string planName      = getValidTrainingPlanName();
    double weight        = getValidWeight();
    string categoryName  = getValidWeightCategoryName();

    shared_ptr<TrainingPlan> plan = createTrainingPlan(planName);
    WeightCategory category       = WeightCategory::lookup(categoryName);

    int competitions      = getValidCompetitions(plan);
    double coachingHours  = getValidCoachingHours();

    return Athlete(name, weight, category, plan, competitions, coachingHours);
}

// ----------------------------------------------------------------------------
// Prints the "Additional information" reference table from the client
// brief. Notice this pulls its numbers from the TrainingPlan objects and
// WeightCategory::lookup() rather than hardcoding them again - if the
// club ever changes a price, there is exactly one place to update it.
// ----------------------------------------------------------------------------
void displayPricingGuide() {
    vector<shared_ptr<TrainingPlan>> plans = {
        make_shared<BeginnerPlan>(), make_shared<IntermediatePlan>(), make_shared<ElitePlan>()
    };
    const vector<string> categoryOrder = {
        "Heavyweight", "Light-Heavyweight", "Middleweight",
        "Light-Middleweight", "Lightweight", "Flyweight"
    };
    const int LABEL_WIDTH = 36; // matches the procedural version's column

    cout << "\n================== PRICING & WEIGHT CATEGORY GUIDE ==================\n";
    cout << fixed << setprecision(2);

    cout << "Training Plan Prices (per week):\n";
    for (auto &plan : plans) {
        ostringstream label;
        label << "  " << plan->getName() << " (" << plan->getSessionsPerWeek()
              << " sessions/week):";
        cout << left << setw(LABEL_WIDTH) << label.str()
             << "GBP " << plan->getWeeklyFee() << "\n";
    }
    cout << left << setw(LABEL_WIDTH) << "  Private tuition (per hour):"
         << "GBP " << Athlete::getCoachingRate() << "\n";
    cout << left << setw(LABEL_WIDTH) << "  Competition entry (per comp.):"
         << "GBP " << Athlete::getCompetitionFee() << "\n";

    cout << "\nWeight Categories (upper limit in kg):\n";
    for (const string &name : categoryOrder) {
        WeightCategory wc = WeightCategory::lookup(name);
        ostringstream label;
        label << "  " << wc.getName() << ":";
        cout << left << setw(LABEL_WIDTH) << label.str()
             << wc.getLimitDescription() << "\n";
    }

    cout << "\nRules:\n";
    cout << "  - Maximum 5 hours private coaching per week.\n";
    cout << "  - Only Intermediate and Elite athletes may enter competitions.\n";
    cout << "  - Competitions are held on the second Saturday of each month.\n";
    cout << "  - A month is assumed to be " << Athlete::getWeeksPerMonth() << " weeks.\n";
    cout << "=======================================================================\n\n";
}

// ----------------------------------------------------------------------------
int main() {
    JudoClub club;
    int choice = 0;

    cout << "================================================\n";
    cout << "  North Sussex Judo - Monthly Fee Calculator\n";
    cout << "  (Object-Oriented Version)\n";
    cout << "================================================\n";

    // displayPricingGuide(); // shown once up-front so rates are visible before registering

    do {
        cout << "1. Register a new athlete\n";
        cout << "2. View all athletes and costs\n";
        cout << "3. View pricing & weight category guide\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            clearInputError();
            cout << "\nPlease enter a number between 1 and 4.\n\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                Athlete a = registerNewAthlete();
                club.addAthlete(a);
                cout << "\n" << a.getName() << " has been registered.\n\n";
                a.displaySummary();
                break;
            }
            case 2:
                club.displayAll();
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
