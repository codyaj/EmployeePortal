#include <iostream>
#include <string>
#include <ctime>
#include <map>
#include <vector>


using namespace std;

class Time {
private:
	int hour = -1, minute = -1;
public:
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
	bool workStatus() { return permanent; }
};

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
	void clockIn() { recordTime(0); }
	void clockOut() { recordTime(1); }
};

class Administrative {
public:
	void searchByName(string name, vector<User>& users) {
		for (User u : users) {
			if (u.getName().find(name) != string::npos) {
				cout << "Possible Match: " << u.getName() << " | ID: " << u.getID() << endl;;
			}
		}
	}
};

class Accountant : public virtual User, public virtual Administrative {
public:
	void findPay(int ID, vector<User>& users) {
		for (User u : users) {
			if (u.getID() == ID) {
				cout << "ID: " << u.getID() << "\nName: " << u.getName() << "\nPay: " << u.getPay() << endl;
				return;
			}
		}
		cout << "No user found with ID: " << ID << endl;
	}
};

class DutyManager : public virtual User, public virtual Administrative {
	// changeSchedule check if casual
private:
	void x() {} // Helper function - Display Current Schedule, Ask what to change, Change.
public:
	virtual void changeSchedule(int ID, vector<User>& users) {
		for (User u : users) {
			if (u.getID() == ID) {
				if (!u.workStatus()) {
					// Change schedule
				}
				else {
					cout << "This employee is not casual. Their schedule must be changed by a manager!\n";
					return;
				}
			}
		}
		cout << "No user found with ID: " << ID << endl;
	}
	void findSchedule(int ID, vector<User>& users) {
		for (User u : users) {
			if (u.getID() == ID) {
				// Display Schedule
			}
		}
		cout << "No user found with ID: " << ID << endl;
	}
};

class Manager : public DutyManager, public Accountant {
	// changeSchedule override dont check if casual
	// Add employee
public:
	void changeSchedule(int ID, vector<User>& users) override {
		for (User u : users) {
			if (u.getID() == ID) {
				// Change schedule
			}
		}
		cout << "No user found with ID: " << ID << endl;
	}
	void addEmployee(vector<User>& users) {

	}
};

vector<User> loadFromFile() {
	// FORMAT: type,id,username,password,perm/casual,Pay,clockInTimeMonday clockOutTimeMonday ... clockInTimeSunday clockOutTimeSunday
	return {};
}

int main() {
	vector<User> users = loadFromFile();
	User* currentUser = nullptr;
	
}