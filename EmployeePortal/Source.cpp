#include <iostream>
#include <string>
#include <ctime>
#include <vector>


using namespace std;


class invalidUser : public exception {
private:
	string message;
public:
	invalidUser(string msg) : message(msg) {}
	const char* what() const throw() {
		return message.c_str();
	}
};

class Time {
private:
	int hour, minute;
public:
	Time() : hour(-1), minute(-1) {}
	Time(int hour, int minute) {
		this->hour = hour;
		this->minute = minute;
	}
	friend std::ostream& operator<<(std::ostream& os, Time& t);
	void operator()(int hour, int minute) {
		this->hour = hour;
		this->minute = minute;
	}
};

std::ostream& operator<<(std::ostream& os, Time& t) {
	os << t.hour << ":" << t.minute;
	return os;
}

class User {
private:
	int ID, pay;
	string username, password;
	bool permanent;
	Time schedule[7][2]; // 7 Days per Week | [Clock In Time, Clock Out Time]
	vector<string> alerts;
	void showSchedule(User* user) {
		for (int i = 0; i < 7; i++) {
			cout << "In: " << user->schedule[i][0] << " | Out: " << user->schedule[i][1] << endl;
		}
	}
public:
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
	}
	bool login(string user, string pass) {
		if (user == username && pass == password) {
			return true;
		}
		return false;
	}
	void viewSchedule() {

	}
	void viewPay() {

	}
	void viewAlerts() {

	}

	string getName() { return username; }
	int getID() { return ID; }
	int getPay() { return pay; }
	bool isPermanent() { return permanent; }
};

User* findUserByID(int ID, vector<unique_ptr<User>>& users); // Function prototype

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

		clockedIn[newtime.tm_wday - 1][index](newtime.tm_hour, newMin);
	}
public:
	Employee(int id, string user, string pass, bool perm, int Pay, string sched) : User(id, user, pass, perm, Pay, sched) {}
	void clockIn() { recordTime(0); }
	void clockOut() { recordTime(1); }
};

class Administrative {
public:
	void searchByName(string name, vector<unique_ptr<User>>& users) {
		for (auto& u : users) {
			if (u->getName().find(name) != string::npos) {
				cout << "Possible Match: " << u->getName() << " | ID: " << u->getID() << endl;;
			}
		}
	}
};

class Accountant : public virtual User, public virtual Administrative {
public:
	Accountant(int id, string user, string pass, bool perm, int Pay, string sched) : User(id, user, pass, perm, Pay, sched) {}
	void findPay(int ID, vector<unique_ptr<User>>& users) {
		User* user = nullptr;
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
	virtual void changeSchedule(int ID, vector<unique_ptr<User>>& users) {
		User* user = nullptr;
		try {
			user = findUserByID(ID, users);
		}
		catch (invalidUser& e) {
			cout << "Error: " << e.what() << endl;
			return;
		}

		if (!user->isPermanent()) {
			// Change schedule
		}
		else {
			cout << "This employee is not casual. Their schedule must be changed by a manager!\n";
			return;
		}
	}
	void findSchedule(int ID, vector<unique_ptr<User>>& users) {
		User* user = nullptr;
		try {
			user = findUserByID(ID, users);
		}
		catch (invalidUser& e) {
			cout << "Error: " << e.what() << endl;
			return;
		}
		// Display schedule
	}
};

class Manager : public DutyManager, public Accountant {
	// changeSchedule override dont check if casual
	// Add employee
public:
	Manager(int id, string user, string pass, bool perm, int Pay, string sched) : DutyManager(id, user, pass, perm, Pay, sched), Accountant(id, user, pass, perm, Pay, sched), User(id, user, pass, perm, Pay, sched) {}
	void changeSchedule(int ID, vector<unique_ptr<User>>& users) override {
		User* user = nullptr;
		try {
			user = findUserByID(ID, users);
		}
		catch (invalidUser& e) {
			cout << "Error: " << e.what() << endl;
			return;
		}
		// Change schedule
	}
	void addEmployee(vector<unique_ptr<User>>& users) {

	}
};

vector<unique_ptr<User>> loadFromFile() {
	// FORMAT: type,id,username,password,perm/casual,Pay,clockInTimeMonday clockOutTimeMonday ... clockInTimeSunday clockOutTimeSunday
	// Seperate file for Manager, Employee, ...
	return {};
}

int main() {
	vector<unique_ptr<User>> users = loadFromFile();
	User* currentUser = nullptr;

	// Examples
	users.push_back(make_unique<Employee>(1, "emp_user", "password", false, 70000, "9:00 17:00 9:00 17:00 0:00 0:00 0:00 0:00 0:00 0:00 0:00 0:00 0:00 0:00"));
	users.push_back(make_unique<Accountant>(2, "acc_user", "password", true, 80000, "9:00 17:00 9:00 17:00 9:00 17:00 9:00 17:00 9:00 17:00 0:00 0:00 0:00 0:00"));
	users.push_back(make_unique<DutyManager>(3, "dm_user", "password", false, 90000, "10:00 18:00 10:00 18:00 10:00 18:00 10:00 18:00 10:00 18:00 0:00 0:00 0:00 0:00"));
	users.push_back(make_unique<Manager>(4, "mgr_user", "password", true, 100000, "8:00 16:00 8:00 16:00 8:00 16:00 8:00 16:00 8:00 16:00 0:00 0:00 0:00 0:00"));

	return 0;
}