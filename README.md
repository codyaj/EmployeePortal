[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

# Employee Management Portal
This project is a command-line application designed to manage employee schedules, pay, and clock-in functionalities. The portal supports different user roles such as employees, accountants, managers, and duty managers, each with specific permissions.

## Roadmap

- [ ] **Enhanced Logging Details:** Improve logging mechanism to include more detailed information.
- [ ] **Inclusion of More Guard Clauses:** Ensuring critical conditions are met before proceeding.
- [ ] **Implement More Robust Error Handling:** Cover more edge cases and provide more informative error messages to users.
- [ ] **Optimizing Loops:** Review loops for potential optimizations.

## User Roles and Permissions
* **Employees:**
  * **Check Schedule:** Employees can view their schedule.
  * **View Pay Information:** Employees can view their salary.
  * **Clock In:** Employees can clock in and out of their shifts.
* **Accountants:**
  * **View Salary Information:** Access details regarding users salary.
* **Duty Managers:**
  * **Change Casual Employee Schedule:** Access and adjust schedules as required if user is employed as a casual.
* **Managers:**
  * **Add Employees:** Add employees to the database.
  * **Change Employee Schedule:** Access and adjust schedules as required.
  * **View Salary Information:** Access details regarding users salary.

## Installation
**Visual Studio Installation:** [CLICK HERE](https://learn.microsoft.com/en-us/visualstudio/version-control/git-clone-repository?view=vs-2022)

**Other:**
1. **Clone the repository**
   ```
   git clone https://github.com/your-username/employee-management-portal.git
   cd employee-management-portal/EmployeePortal/
   ```
2. **Build (using gpp)**
   ```
   gpp Source.cpp -o Source.exe
   ```
3. **Run the application**
   ```
   ./Source.exe
   ```

## Usage
1. **Log in with your credentials.** Depending on your role you will have access to different functionalities.
2. **Follow the on screen prompts** to navigate through the portal and perform various actions based on your role.

## License
This project is licensed under the MIT License. See the [LICENSE](https://github.com/codyaj/EmployeePortal/blob/master/LICENSE.txt) file for details.

## Contact
Questions or suggestions can be given to me at:
* **Email:** cajackson05@icloud.com
* **GitHub:** [codyaj](https://github.com/codyaj)

<!-- Images -->
[license-shield]: https://img.shields.io/github/license/othneildrew/Best-README-Template.svg?style=for-the-badge
[license-url]: https://github.com/codyaj/EmployeePortal/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/codyaj/
