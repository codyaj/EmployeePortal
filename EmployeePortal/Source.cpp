#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <memory>
#include <stdexcept>
#include <map>
#include <regex>

#include <typeinfo> // remove

using namespace std;

class invalidUser : public runtime_error {
public:
    invalidUser(string msg) : runtime_error(msg) {}
};

class Time {
private:
    int hour, minute;
public:
    Time() : hour(0), minute(0) {}
    Time(int hour, int minute) : hour(hour), minute(minute) {}

    friend ostream& operator<<(ostream& os, const Time& t) {
        os << t.hour << ":" << t.minute;
        return os;
    }

    void setTime(int hour, int minute) {
        this->hour = hour;
        this->minute = minute;
    }
};

class User {
private:
    int ID, pay;
    string username, password;
    bool permanent;
    Time schedule[7][2]; // 7 Days per Week | [Clock In Time, Clock Out Time]
    vector<string> alerts;
public:
    virtual ~User() = default; // Makes the user class a polymorphic type to allow for dynamic_cast
    User(int id, string user, string pass, bool perm, int Pay, string sched) : ID(id), username(user), password(pass), permanent(perm), pay(Pay) {
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
    bool login(string user, string pass) const {
        return user == username && pass == password;
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
    void viewPay() const {
        cout << "Pay: " << pay << endl;
    }
    void viewAlerts() const {
        for (const auto& alert : alerts) {
            cout << alert << endl;
        }
    }

    string getName() { return username; }
    int getID() { return ID; }
    int getPay() { return pay; }
    bool isPermanent() { return permanent; }
};

shared_ptr<User> findUserByID(int ID, vector<shared_ptr<User>>& users) {
    for (auto& u : users) {
        if (u->getID() == ID) {
            return u;
        }
    }
    throw invalidUser("User not found");
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
    Employee(int id, string user, string pass, bool perm, int Pay, string sched) : User(id, user, pass, perm, Pay, sched) {}
    void clockIn() { recordTime(0); }
    void clockOut() { recordTime(1); }
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
};

class Accountant : public virtual User, public virtual Administrative {
public:
    Accountant(int id, string user, string pass, bool perm, int Pay, string sched) : User(id, user, pass, perm, Pay, sched) {}
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
private:
    void x() {} // Helper function - Display Current Schedule, Ask what to change, Change.
public:
    DutyManager(int id, string user, string pass, bool perm, int Pay, string sched) : User(id, user, pass, perm, Pay, sched) {}
    virtual void changeSchedule(int ID, vector<shared_ptr<User>>& users) {
        try {
            shared_ptr<User> user = findUserByID(ID, users);
            if (!user->isPermanent()) {
                user->viewSchedule(); // Display schedule
                // Change schedule
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
    // Add employee
public:
    Manager(int id, string user, string pass, bool perm, int Pay, string sched) : DutyManager(id, user, pass, perm, Pay, sched), Accountant(id, user, pass, perm, Pay, sched), User(id, user, pass, perm, Pay, sched) {}
    void changeSchedule(int ID, vector<shared_ptr<User>>& users) override {
        try {
            shared_ptr<User> user = findUserByID(ID, users);

            user->viewSchedule(); // Display schedule
            // Change schedule
        }
        catch (invalidUser& e) {
            cout << "Error: " << e.what() << endl;
            return;
        }
    }
    void addEmployee(vector<shared_ptr<User>>& users) {
        // Get ID
        int ID;
        cout << "Enter the desired ID for the user: ";
        while (!(cin >> ID) || ID <= 0) {
            cout << "Invalid input. Please enter a positive integer for the ID: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

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
        int payRate;
        cout << "What " << ((perm) ? "yearly" : "hourly") << " salary is this user on: ";
        while (!(cin >> payRate) || payRate <= 0) {
            cout << "Invalid input. Please enter a positive integer for the pay rate: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

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
            cout << "Enter the start and end time in this format for the day " << weekDays[i] << "\nhour:minute hour:minute";
            cin >> temp;
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
    // Seperate file for Manager, Employee, ...
    return {};
}

void saveToFile(vector<shared_ptr<User>> users) {

}

int main() {
    vector<shared_ptr<User>> users = loadFromFile();

    // Examples
    users.push_back(make_shared<Employee>(1, "emp_user", "password", false, 70000, "9:00 17:00 9:00 17:00 0:00 0:00 0:00 0:00 0:00 0:00 17:00 1:00 17:00 1:00"));
    users.push_back(make_shared<Accountant>(2, "acc_user", "password", true, 80000, "9:00 17:00 9:00 17:00 9:00 17:00 9:00 17:00 9:00 17:00 0:00 0:00 0:00 0:00"));
    users.push_back(make_shared<DutyManager>(3, "dm_user", "password", false, 90000, "10:00 18:00 10:00 18:00 10:00 18:00 10:00 18:00 10:00 18:00 0:00 0:00 0:00 0:00"));
    users.push_back(make_shared<Manager>(4, "mgr_user", "password", true, 100000, "8:00 16:00 8:00 16:00 8:00 16:00 8:00 16:00 8:00 16:00 0:00 0:00 0:00 0:00"));

    auto currentUser = dynamic_pointer_cast<Manager>(users[3]); // Manager
    currentUser->addEmployee(users);

    for (auto& user : users) {
        cout << user->getName() << endl; 
        user->viewSchedule();
    }

    return 0;
}