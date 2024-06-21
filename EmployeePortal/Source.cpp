#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <memory>
#include <stdexcept>
#include <map>
#include <regex>
#include <fstream>
#include <sstream>

using namespace std;

class invalidUser : public runtime_error {
public:
    invalidUser(string msg) : runtime_error(msg) {}
};

class Time {
private:
    bool set;
    int hour, minute;
public:
    Time() : hour(0), minute(0), set(false) {}
    Time(int hour, int minute) : hour(hour), minute(minute), set(true) {}

    string output() {
        return to_string(hour) + ":" + to_string(minute);
    }

    friend ostream& operator<<(ostream& os, const Time& t) {
        os << t.hour << ":" << t.minute;
        return os;
    }

    bool isSet() { return set; }

    void setTime(int hour, int minute) {
        this->hour = hour;
        this->minute = minute;
        set = true;
    }

    bool operator>(const Time& t) {
        return (this->hour > t.hour || (this->hour == t.hour && this->minute > t.minute));
    }
};

class User {
private:
    char type;
    int ID, pay;
    string username, password;
    bool permanent;
    Time schedule[7][2]; // 7 Days per Week | [Clock In Time, Clock Out Time]
    vector<string> alerts;
public:
    virtual ~User() = default; // Makes the user class a polymorphic type to allow for dynamic_cast
    User(int id, string user, string pass, bool perm, int Pay, string sched, char type) : ID(id), username(user), password(pass), permanent(perm), pay(Pay), type(type) {
        // split sched string into the schedule array
        string tempHour, tempMinute;
        int day = 0, counter = 0;
        for (char c : sched) {
            if (c == ' ') {
                Time t(stoi(tempHour), stoi(tempMinute));
                tempHour = "";
                tempMinute = "";

                schedule[day][counter] = t;
                if (counter == 0) {
                    counter += 1;
                }
                else {
                    counter = 0;
                    day += 1;
                }
            }
            else if (c == ':') {
                tempHour = tempMinute;
                tempMinute = "";
            }
            else {
                tempMinute += c;
            }
        }
        // Handle the last Time object
        if (!tempMinute.empty()) {
            Time t(stoi(tempHour), stoi(tempMinute));
            schedule[day][counter] = t;
        }
    }
    void viewSchedule() {
        map<int, string> weekDays{
            {0, "Monday" },
            {1, "Tuesday" },
            {2, "Wednesday" },
            {3, "Thursday" },
            {4, "Friday" },
            {5, "Saturday" },
            {6, "Sunday" }
        };
        for (int i = 0; i < 7; ++i) {
            cout << weekDays[i] << " " << schedule[i][0] << " to " << schedule[i][1] << endl;
        }
    }
    string getScheduleRaw() {
        string output;
        for (int i = 0; i < 7; ++i) {
            output += schedule[i][0].output() + " " + schedule[i][1].output() + " ";
        }
        return output;
    }
    Time getScheduleTime(int day, int index) {
        return schedule[day][index];
    }
    void viewPay() const {
        cout << "Pay: " << pay << endl;
    }
    void setSchedule(int day, int iteration, Time t) {
        schedule[day][iteration] = t;
    }

    string getName() { return username; }
    string getPass() { return password; }
    int getID() { return ID; }
    int getPay() { return pay; }
    bool isPermanent() { return permanent; }
    char getType() { return type; }
};

shared_ptr<User> findUserByID(int ID, vector<shared_ptr<User>>& users) {
    for (auto& u : users) {
        if (u->getID() == ID) {
            return u;
        }
    }
    throw invalidUser("User not found");
}

template <class T>
T validateNumericalInput(string text) {
    T num;
    while (true) {
        cout << text;
        cin >> num;

        if (cin.fail()) {
            cin.clear(); // Clear error flags
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard input
            cout << "Invalid input. Please enter a valid number.\n";
        }
        else {
            return num;
        }
    }
}

class Employee : public User {
private:
    Time clockedIn[7][2];
    void recordTime(int index) {
        struct tm newtime;
        time_t now = time(0);
        localtime_s(&newtime, &now);

        // Round minute to nearest 15min
        int newMin = newtime.tm_min + 15 / 2;
        newMin -= newMin % 15;

        clockedIn[newtime.tm_wday - 1][index].setTime(newtime.tm_hour, newMin);
    }
public:
    Employee(int id, string user, string pass, bool perm, int Pay, string sched) : User(id, user, pass, perm, Pay, sched, 'E') {}
    void clockIn() { recordTime(0); }
    void clockOut() { recordTime(1); }
    Time getClockTime(int day, int index) {
        return clockedIn[day][index];
    }
};

class Administrative {
public:
    void searchByName(string name, vector<shared_ptr<User>>& users) {
        for (auto& u : users) {
            if (u->getName().find(name) != string::npos) {
                cout << "Possible Match: " << u->getName() << " | ID: " << u->getID() << endl;
            }
        }
    }
    void seeIfHere(int ID, vector<shared_ptr<User>>& users) {
        shared_ptr<User> user = nullptr;
        try {
            user = findUserByID(ID, users);
        }
        catch (invalidUser& e) {
            cout << "Error: " << e.what() << endl;
            return;
        }

        // Get tm stuct to get day
        struct tm newtime;
        time_t now = time(0);
        localtime_s(&newtime, &now);
        Time t(newtime.tm_hour, newtime.tm_min);

        // If employee see if they have clocked in
        if (user->getType() == 'E') {
            shared_ptr<Employee> EmployeeUser = dynamic_pointer_cast<Employee>(user);

            // Check if here
            if (EmployeeUser->getScheduleTime(newtime.tm_wday - 1, 0).isSet() && !EmployeeUser->getScheduleTime(newtime.tm_wday - 1, 1).isSet()) {
                cout << "Employee is here!\n";
            }
            else {
                cout << "Employee is not here!\n";
            }

            // Check if late
            if (EmployeeUser->getScheduleTime(newtime.tm_wday - 1, 0).isSet() && EmployeeUser->getClockTime(newtime.tm_wday - 1, 0) > EmployeeUser->getScheduleTime(newtime.tm_wday - 1, 0)) {
                cout << "This employee was late\n";
            }
            cout << "Process   | Schedule | Actual\n";
            cout << "Clock In  | " << EmployeeUser->getClockTime(newtime.tm_wday - 1, 0) << " | " << EmployeeUser->getScheduleTime(newtime.tm_wday - 1, 0) << endl;
            cout << "Clock Out | " << EmployeeUser->getClockTime(newtime.tm_wday - 1, 1) << " | " << EmployeeUser->getScheduleTime(newtime.tm_wday - 1, 1) << endl;
        }
        else { // If not check if they are supposed to be here
            if (t > user->getScheduleTime(newtime.tm_wday - 1, 0) && user->getScheduleTime(newtime.tm_wday - 1, 1) > t) {
                cout << "This employee should be here\n";
            }
            else {
                cout << "This employee is not here\n";
            }
            cout << "Scheduled Arrival:   " << user->getScheduleTime(newtime.tm_wday - 1, 0) << endl;
            cout << "Scheduled Departure: " << user->getScheduleTime(newtime.tm_wday - 1, 1) << endl;
        }
    }
};

class Accountant : public virtual User, public virtual Administrative {
public:
    Accountant(int id, string user, string pass, bool perm, int Pay, string sched) : User(id, user, pass, perm, Pay, sched, 'A') {}
    void findPay(int ID, vector<shared_ptr<User>>& users) {
        shared_ptr<User> user = nullptr;
        try {
            user = findUserByID(ID, users);
        }
        catch (invalidUser& e) {
            cout << "Error: " << e.what() << endl;
            return;
        }

        cout << "ID: " << user->getID() << "\nName: " << user->getName() << "\nPay: " << user->getPay() << endl;
    }
};

class DutyManager : public virtual User, public virtual Administrative {
    // changeSchedule check if casual
protected:
    void changeScheduleHelper(shared_ptr<User> user) {
        user->viewSchedule(); // Display schedule
        while (true) { // Change schedule
            cout << "1) Monday\n2) Tuesday\n3) Wednesday\n4) Thursday\n5) Friday\n6) Saturday\n7) Sunday\n8) Exit\n";
            int input = validateNumericalInput<int>("What day do you want to change? ");
            if (input > 8 || input < 1) {
                cout << "Invalid input!\n";
                continue;
            }
            else if (input == 8) {
                return;
            }

            // Get input
            string strInput;
            cout << "Enter the start and end time in this format\nhour:minute hour:minute\n";
            cin.ignore();
            getline(cin, strInput);

            // Validation to ensure correct format
            regex timePattern(R"((\d{1,2}:\d{2}) (\d{1,2}:\d{2}))");
            while (!regex_match(strInput, timePattern)) {
                cout << "Invalid format. Please enter the time in 'hour:minute hour:minute' format: ";
                getline(cin, strInput);
            }

            // Handle input and change the schedule
            int times[4], iteration = 0;
            string temp = "";
            for (char c : strInput) {
                if (c == ':' || c == ' ') {
                    // Convert str to appropriate int space
                    times[iteration] = stoi(temp);

                    // Add iteration
                    iteration += 1;

                    // Clear string
                    temp = "";
                }
                else {
                    temp += c;
                }
            }
            // Handle final
            times[3] = stoi(temp);

            Time t1(times[0], times[1]), t2(times[2], times[3]);
            user->setSchedule(input - 1, 0, t1);
            user->setSchedule(input - 1, 1, t2);
        }
    }
public:
    DutyManager(int id, string user, string pass, bool perm, int Pay, string sched) : User(id, user, pass, perm, Pay, sched, 'D') {}
    virtual void changeSchedule(int ID, vector<shared_ptr<User>>& users) {
        try {
            shared_ptr<User> user = findUserByID(ID, users);
            if (!user->isPermanent()) {
                user->viewSchedule(); // Display schedule
                changeScheduleHelper(user); // Change schedule
                return;
            }
            else {
                cout << "This employee is not casual. Their schedule must be changed by a manager!\n";
                return;
            }
        }
        catch (const invalidUser& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    void findSchedule(int ID, vector<shared_ptr<User>>& users) {
        try {
            shared_ptr<User> user = findUserByID(ID, users);
            user->viewSchedule(); // Display schedule
        }
        catch (invalidUser& e) {
            cout << "Error: " << e.what() << endl;
            return;
        }
    }
};

class Manager : public DutyManager, public Accountant {
    // changeSchedule override dont check if casual
public:
    Manager(int id, string user, string pass, bool perm, int Pay, string sched) : DutyManager(id, user, pass, perm, Pay, sched), Accountant(id, user, pass, perm, Pay, sched), User(id, user, pass, perm, Pay, sched, 'M') {}
    void changeSchedule(int ID, vector<shared_ptr<User>>& users) override {
        try {
            shared_ptr<User> user = findUserByID(ID, users);

            user->viewSchedule(); // Display schedule
            changeScheduleHelper(user); // Change schedule
            return;
        }
        catch (invalidUser& e) {
            cout << "Error: " << e.what() << endl;
            return;
        }
    }
    void addEmployee(vector<shared_ptr<User>>& users) {
        // Get ID
        int ID = validateNumericalInput<int>("Enter the desired ID for the user: ");

        // Check if ID taken
        for (auto& user : users) {
            if (user->getID() == ID) {
                cout << "This ID is already taken!\n";
                return;
            }
        }

        // Get Name
        string user;
        cout << "Enter the name of this user: ";
        cin.ignore();
        getline(cin, user);

        // Get Pass
        string pass;
        cout << "Enter the pass of this user: ";
        getline(cin, pass);

        // Get permenant work status
        char tempVal;
        bool perm;
        cout << "Is this user a permanent or casual employee (P/C): ";
        cin >> tempVal;
        tempVal = toupper(tempVal);
        while (tempVal != 'P' && tempVal != 'C') {
            cout << "Invalid input. Please enter 'P' for permanent or 'C' for casual: ";
            cin >> tempVal;
            tempVal = toupper(tempVal);
        }
        perm = (tempVal == 'P');
        
        // Get Pay
        string outputStr = "What " + string((perm) ? "yearly" : "hourly") + " salary is this user on: ";
        int payRate = validateNumericalInput<int>(outputStr);

        // Get schedule
        string schedule = "";
        map<int, string> weekDays{
            {0, "Monday" },
            {1, "Tuesday" },
            {2, "Wednesday" },
            {3, "Thursday" },
            {4, "Friday" },
            {5, "Saturday" },
            {6, "Sunday" }
        };
        for (int i = 0; i < 7; i++) {
            string temp;
            cout << "Enter the start and end time in this format for the day " << weekDays[i] << "\nhour:minute hour:minute\n";
            getline(cin, temp);

            // Validation to ensure correct format
            regex timePattern(R"((\d{1,2}:\d{2}) (\d{1,2}:\d{2}))");
            while (!regex_match(temp, timePattern)) {
                cout << "Invalid format. Please enter the time in 'hour:minute hour:minute' format: ";
                getline(cin, temp);
            }
            schedule += temp + " ";
        }
        
        // Get what type of employee this user is
        // Push_back shared ptr to users vector
        cout << "Is this user an Employee, Accountant, Duty Manager, or Manager (E/A/D/M): ";
        cin >> tempVal;
        tempVal = toupper(tempVal);
        while (tempVal != 'E' && tempVal != 'A' && tempVal != 'D' && tempVal != 'M') {
            cout << "Invalid input. Please enter 'E' for Employee, 'A' for Accountant, 'D' for Duty Manager, or 'M' for Manager: ";
            cin >> tempVal;
            tempVal = toupper(tempVal);
        }

        switch (tempVal) {
        case 'E': {
            users.push_back(make_shared<Employee>(ID, user, pass, perm, payRate, schedule));
            break;
        }
        case 'A': {
            users.push_back(make_shared<Accountant>(ID, user, pass, perm, payRate, schedule));
            break;
        }
        case 'D': {
            users.push_back(make_shared<DutyManager>(ID, user, pass, perm, payRate, schedule));
            break;
        }
        case 'M': {
            users.push_back(make_shared<Manager>(ID, user, pass, perm, payRate, schedule));
            break;
        }
        }
    }
};

vector<shared_ptr<User>> loadFromFile() {
    // FORMAT: type,id,username,password,perm/casual,Pay,clockInTimeMonday clockOutTimeMonday ... clockInTimeSunday clockOutTimeSunday
    vector<shared_ptr<User>> users = {};
    string line;
    ifstream infile("Users.txt");
    if (!infile) {
        cout << "Unable to open Users.txt\n";
        return users;
    }
    while (getline(infile, line)) {
        stringstream ss(line);
        string type, idStr, username, password, permStr, payStr, schedule;
        int id, pay;
        bool perm;

        // Split input line by commas
        getline(ss, type, ',');
        getline(ss, idStr, ',');
        getline(ss, username, ',');
        getline(ss, password, ',');
        getline(ss, permStr, ',');
        getline(ss, payStr, ',');

        // Read rest of line as schedule
        getline(ss, schedule);

        // Convert to appropriate types
        id = stoi(idStr);
        pay = stoi(payStr);
        perm = (permStr == "perm");

        // Create shared ptrs and add to vector
        if (type == "E") {
            users.push_back(make_shared<Employee>(id, username, password, perm, pay, schedule));
        }
        else if (type == "A") {
            users.push_back(make_shared<Accountant>(id, username, password, perm, pay, schedule));
        }
        else if (type == "D") {
            users.push_back(make_shared<DutyManager>(id, username, password, perm, pay, schedule));
        }
        else if (type == "M") {
            users.push_back(make_shared<Manager>(id, username, password, perm, pay, schedule));
        }
        else {
            cerr << "Unknown user type: " << type << endl;
        }
    }
    return users;
}

void saveToFile(vector<shared_ptr<User>> users) {
    ofstream outfile("Users.txt");
    if (!outfile) {
        cout << "Unable to open Users.txt\n";
        return;
    }

    for (const auto& user : users) {
        outfile << user->getType() << ",";
        outfile << user->getID() << ",";
        outfile << user->getName() << ",";
        outfile << user->getPass() << ",";
        outfile << (user->isPermanent() ? "perm" : "casual") << ",";
        outfile << user->getPay() << ",";
        outfile << user->getScheduleRaw() << "\n";
    }
}

void employeeMenu(shared_ptr<Employee> user) {
    while (true) {
        cout << "-==== MENU ====-\n1) View Schedule\n2) View Pay\n3) Clock In\n4) Clock Out\n5) Logout\n";
        int input = validateNumericalInput<int>("");

        switch (input) {
        case 1: {
            user->viewSchedule();
            break;
        }
        case 2: {
            user->viewPay();
            break;
        }
        case 3: {
            // Verify that they haven't already clocked in
            struct tm newtime;
            time_t now = time(0);
            localtime_s(&newtime, &now);
            if (user->getClockTime(newtime.tm_wday - 1, 0).isSet()) {
                cout << "You are already clocked in!\n";
                break;
            }

            user->clockIn();
            break;
        }
        case 4: {
            // Verify that they haven't already clocked in and they havent yet clocked out
            struct tm newtime;
            time_t now = time(0);
            localtime_s(&newtime, &now);
            if (!user->getClockTime(newtime.tm_wday - 1, 0).isSet()) {
                cout << "You must be already clocked in!\n";
                break;
            }
            if (user->getClockTime(newtime.tm_wday - 1, 1).isSet()) {
                cout << "You musn't be clocked out already!\n";
                break;
            }

            user->clockOut();
            break;
        }
        case 5: {
            return;
        }
        }
    }
}
void accountantMenu(shared_ptr<Accountant> user, vector<shared_ptr<User>> users) {
    while (true) {
        cout << "-==== MENU ====-\n1) View Schedule\n2) View Pay\n3) Search Employee By Name\n4) Find Employee Current Pay\n5) See if user is here\n6) Logout\n";
        int input = validateNumericalInput<int>("");

        switch (input) {
        case 1: {
            user->viewSchedule();
            break;
        }
        case 2: {
            user->viewPay();
            break;
        }
        case 3: {
            cout << "Enter the name you would like to search for: ";
            string name;
            cin.ignore();
            getline(cin, name);
            user->searchByName(name, users);
            break;
        }
        case 4: {
            int ID = validateNumericalInput<int>("Enter the ID of the use you want to search for: ");

            user->findPay(ID, users);
            break;
        }
        case 5: {
            int ID = validateNumericalInput<int>("Enter the ID of the user you want to search for: ");

            user->seeIfHere(ID, users);
            break;
        }
        case 6: {
            return;
        }
        }
    }
}
void dutyManagerMenu(shared_ptr<DutyManager> user, vector<shared_ptr<User>> users) {
    while (true) {
        cout << "-==== MENU ====-\n1) View Schedule\n2) View Pay\n3) Search Employee By Name\n4) Change Schedule\n5) Find Schedule\n6) See if users is here\n7) Logout\n";
        int input = validateNumericalInput<int>("");

        switch (input) {
        case 1: {
            user->viewSchedule();
            break;
        }
        case 2: {
            user->viewPay();
            break;
        }
        case 3: {
            cout << "Enter the name you would like to search for: ";
            string name;
            cin.ignore();
            getline(cin, name);
            user->searchByName(name, users);
            break;
        }
        case 4: {
            int ID = validateNumericalInput<int>("Enter the ID of the user that is to have their schedule changed: ");

            user->changeSchedule(ID, users);
            break;
        }
        case 5: {
            int ID = validateNumericalInput<int>("Enter the ID of the user you want to find the schedule of: ");

            user->findSchedule(ID, users);
            break;
        }
        case 6: {
            int ID = validateNumericalInput<int>("Enter the ID of the user you want to search for: ");

            user->seeIfHere(ID, users);
            break;
        }
        case 7: {
            return;
        }
        }
    }
}
void managerMenu(shared_ptr<Manager> user, vector<shared_ptr<User>> users) {
    while (true) {
        cout << "-==== MENU ====-\n1) View Schedule\n2) View Pay\n3) Search Employee By Name\n4) Find Employee Current Pay\n5) Change Schedule\n6) See if user is here\n7) Find Schedule\n8) Add Employee\n9) Close Program\n10) Logout\n";
        int input = validateNumericalInput<int>("");

        switch (input) {
        case 1: {
            user->viewSchedule();
            break;
        }
        case 2: {
            user->viewPay();
            break;
        }
        case 3: {
            cout << "Enter the name you would like to search for: ";
            string name;
            cin.ignore();
            getline(cin, name);
            user->searchByName(name, users);
            break;
        }
        case 4: {
            int ID = validateNumericalInput<int>("Enter the ID of the use you want to search for: ");

            user->findPay(ID, users);
            break;
        }
        case 5: {
            int ID = validateNumericalInput<int>("Enter the ID of the user that is to have their schedule changed: ");

            user->changeSchedule(ID, users);
            break;
        }
        case 6: {
            int ID = validateNumericalInput<int>("Enter the ID of the user you want to search for: ");

            user->seeIfHere(ID, users);
            break;
        }
        case 7: {
            int ID = validateNumericalInput<int>("Enter the ID of the user you want to find the schedule of: ");

            user->findSchedule(ID, users);
            break;
        }
        case 8: {
            user->addEmployee(users);
            break;
        }
        case 9: {
            saveToFile(users);
            exit(0);
        }
        case 10: {
            return;
        }
        }
    }
}

int main() {
    vector<shared_ptr<User>> users = loadFromFile();
    if (users.empty()) {
        cout << "No users found. Creating a default manager user.\n";

        // Create Default Manager
        users.push_back(make_shared<Manager>(1, "Manager", "password", true, 100000, "8:0 16 : 0 8 : 0 16 : 0 8 : 0 16 : 0 8 : 0 16 : 0 8 : 0 16 : 0 0 : 0 0 : 0 0 : 0 0 : 0"));
        
        // Output the created manager user details
        cout << "A manager user has been created with the following ID and Password:\n";
        cout << "ID: 1 | Password: password\n";
    }

    while (true) {
        cout << "-==== LOGIN ====-\nStart by entering your Employee ID: ";
        int ID = validateNumericalInput<int>("");

        shared_ptr<User> currentUser = nullptr;
        try {
            currentUser = findUserByID(ID, users);
        } catch (invalidUser& e) {
            cout << "Error: " << e.what() << endl;
            currentUser = nullptr;
            continue;
        }

        cout << "Password: ";
        string pass;
        cin.ignore();
        getline(cin, pass);
        if (pass != currentUser->getPass()) {
            cout << "Invalid Password!\n";
            currentUser = nullptr;
            continue;
        }

        // Login is successful
        cout << "Welcome " << currentUser->getName() << endl;
        // dynamic cast currentUser and go to function associated with class
        switch (currentUser->getType()) {
        case 'E': {
            employeeMenu(dynamic_pointer_cast<Employee>(currentUser));
            break;
        }
        case 'A': {
            accountantMenu(dynamic_pointer_cast<Accountant>(currentUser), users);
            break;
        }
        case 'D': {
            dutyManagerMenu(dynamic_pointer_cast<DutyManager>(currentUser), users);
            break;
        }
        case 'M': {
            managerMenu(dynamic_pointer_cast<Manager>(currentUser), users);
            break;
        }
        }
    }
}