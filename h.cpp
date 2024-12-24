#include <iostream>
#include <vector>
#include <string>
#include <fstream>

class EmployeeNode {
public:
    int id; // Unique identifier
    std::string name;
    std::string position;
    std::vector<EmployeeNode*> subordinates;

    EmployeeNode(int id, const std::string& name, const std::string& position)
        : id(id), name(name), position(position) {}

    void addSubordinate(EmployeeNode* subordinate) {
        subordinates.push_back(subordinate);
    }

    void displayHierarchy(int level = 0) {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        std::cout << position << " (ID: " << id << "): " << name << "\n";

        for (EmployeeNode* subordinate : subordinates) {
            subordinate->displayHierarchy(level + 1);
        }
    }

    // Search by position and display all employees with that position
    void findAllByPosition(const std::string& position, std::vector<EmployeeNode*>& foundEmployees) {
        if (this->position == position) {
            foundEmployees.push_back(this);
        }

        for (EmployeeNode* subordinate : subordinates) {
            subordinate->findAllByPosition(position, foundEmployees);
        }
    }

    // Search by ID
    EmployeeNode* findById(int searchId) {
        if (this->id == searchId) {
            return this;
        }

        for (EmployeeNode* subordinate : subordinates) {
            EmployeeNode* found = subordinate->findById(searchId);
            if (found) {
                return found;
            }
        }
        return nullptr;
    }

    // Delete an employee by ID
    bool deleteEmployee(int deleteId) {
        for (auto it = subordinates.begin(); it != subordinates.end(); ++it) {
            if ((*it)->id == deleteId) {
                delete *it; // Delete the employee and their subtree
                subordinates.erase(it); // Remove from subordinates list
                return true;
            } else if ((*it)->deleteEmployee(deleteId)) {
                return true;
            }
        }
        return false;
    }

    // Update an employee's name and/or position by ID
    bool updateEmployee(int updateId, const std::string& newName, const std::string& newPosition) {
        EmployeeNode* employeeToUpdate = findById(updateId);
        if (employeeToUpdate) {
            if (!newName.empty()) {
                employeeToUpdate->name = newName;
            }
            if (!newPosition.empty()) {
                employeeToUpdate->position = newPosition;
            }
            return true; // Update was successful
        }
        return false; // Employee not found
    }

    // Promote an employee
    bool promoteEmployee(int promoteId, const std::string& newPosition) {
        EmployeeNode* employeeToPromote = findById(promoteId);
        if (employeeToPromote && !newPosition.empty()) {
            employeeToPromote->position = newPosition;
            return true;
        }
        return false;
    }

    // Demote an employee
    bool demoteEmployee(int demoteId, const std::string& newPosition) {
        EmployeeNode* employeeToDemote = findById(demoteId);
        if (employeeToDemote && !newPosition.empty()) {
            employeeToDemote->position = newPosition;
            return true;
        }
        return false;
    }

    // Destructor to recursively delete subordinates
    ~EmployeeNode() {
        for (EmployeeNode* subordinate : subordinates) {
            delete subordinate;
        }
    }

    // Function to write all employee information to a file
    void writeEmployeeInfoToFile(const std::string& filename) {
        std::ofstream outFile(filename);
        if (!outFile) {
            std::cerr << "Error opening file for writing.\n";
            return;
        }
        writeEmployeeInfo(outFile);
        outFile.close();
    }

    // Helper function to recursively write employee information
    void writeEmployeeInfo(std::ofstream& outFile, int level = 0) {
        for (int i = 0; i < level; ++i) outFile << "  ";
        outFile << position << " (ID: " << id << "): " << name << "\n";

        for (EmployeeNode* subordinate : subordinates) {
            subordinate->writeEmployeeInfo(outFile, level + 1);
        }
    }
};

// Enum for action types
enum ActionType {
    ADD_SUBORDINATE,
    PROMOTE,
    DEMOTE,
    DELETE,
    UPDATE
};

// Function to manage employee actions
bool manageEmployee(EmployeeNode* ceo, ActionType action, int id, const std::string& newName = "", const std::string& newPosition = "", int supervisorId = -1) {
    switch (action) {
        case ADD_SUBORDINATE: {
            if (ceo) {
                EmployeeNode* supervisor = ceo->findById(supervisorId);
                if (supervisor) {
                    EmployeeNode* subordinate = new EmployeeNode(id, newName, newPosition);
                    supervisor->addSubordinate(subordinate);
                    std::cout << "Subordinate added successfully.\n";
                    return true;
                } else {
                    std::cout << "Supervisor not found.\n";
                }
            } else {
                std::cout << "No company exists.\n";
            }
            return false;
        }
        case PROMOTE: {
            return ceo && ceo->promoteEmployee(id, newPosition);
        }
        case DEMOTE: {
            return ceo && ceo->demoteEmployee(id, newPosition);
        }
        case DELETE: {
            return ceo && ceo->deleteEmployee(id);
        }
        case UPDATE: {
            int updateId = id;
            if (ceo && ceo->updateEmployee(updateId, newName, newPosition)) {
                std::cout << "Employee updated successfully.\n";
            } else {
                std::cout << "Employee not found.\n";
            }
            return true;
        }
        default:
            return false;
    }
}

void searchEmployee(EmployeeNode* ceo) {
    int searchChoice;
    std::cout << "Choose a search option:\n";
    std::cout << "1. Search by ID\n";
    std::cout << "2. Search by Position\n";
    std::cout << "Enter your choice: ";
    std::cin >> searchChoice;

    if (searchChoice == 1) {
        int searchId;
        std::cout << "Enter ID to search: ";
        std::cin >> searchId;

        if (ceo) {
            EmployeeNode* foundEmployee = ceo->findById(searchId);
            if (foundEmployee) {
                std::cout << "Employee found: ID: " << foundEmployee->id 
                          << ", Name: " << foundEmployee->name 
                          << ", Position: " << foundEmployee->position << "\n";
            } else {
                std::cout << "No employee found with that ID.\n";
            }
        } else {
            std::cout << "No company exists.\n";
        }
    } else if (searchChoice == 2) {
        std::string searchPosition;
        std::cin.ignore();
        std::cout << "Enter position to search: ";
        std::getline(std::cin, searchPosition);

        if (ceo) {
            std::vector<EmployeeNode*> foundEmployees;
            ceo->findAllByPosition(searchPosition, foundEmployees);

            std::cout << "\nEmployees with position '" << searchPosition << "':\n";
            if (foundEmployees.empty()) {
                std::cout << "No employees found.\n";
            } else {
                for (EmployeeNode* emp : foundEmployees) {
                    std::cout << "ID: " << emp->id 
                              << ", Name: " << emp->name 
                              << ", Position: " << emp->position << "\n";
                }
            }
        } else {
            std::cout << "No company exists.\n";
        }
    } else {
        std::cout << "Invalid choice. Please try again.\n";
    }
}

int main() {
    EmployeeNode* ceo = nullptr;

    int operation;

    do {
        system("cls");
        std::cout << "+======================================+\n";
        std::cout << "|           H I E R A R C H Y          |\n";
        std::cout << "|             S Y S T E A M            |\n";
        std::cout << "+======================================+";
        std::cout << "\nChoose an operation\n";
        std::cout << "1. Create Company\n";
        std::cout << "2. Search Employee \n";
        std::cout << "3. Display Hierarchy\n";
        std::cout << "4. Write Employee Information to File\n";
        std::cout << "5. Manage Employee \n";
        std::cout << "0. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> operation;

        switch (operation) {
            case 1: {
                if (ceo) {
                    delete ceo;
                }
                system("cls");
                std::cout << "+======================================+\n";
                std::cout << "|         Enter Company details        |\n";
                std::cout << "+======================================+\n";
                std::cout << "Enter Company Information\n";
                int memberId;
                std::string memberName, memberPosition;
                std::cout << "ID: ";
                std::cin >> memberId;
                std::cin.ignore(); // Clear newline from input buffer
                std::cout << "Name: ";
                std::getline(std::cin, memberName);
                std::cout << "Position: ";
                std::getline(std::cin, memberPosition);

                ceo = new EmployeeNode(memberId, memberName, memberPosition);

                int numEmployees;
                std::cout << "\nEnter the number of employees under the CEO: ";
                std::cin >> numEmployees;

                for (int i = 0; i < numEmployees; ++i) {
                    int id;
                    std::string name, position;
                    std::cout << "\nEnter details for employee " << (i + 1) << ":\n";
                    std::cout << "ID: ";
                    std::cin >> id;
                    std::cin.ignore();
                    std::cout << "Name: ";
                    std::getline(std::cin, name);
                    std::cout << "Position: ";
                    std::getline(std::cin, position);

                    EmployeeNode* employee = new EmployeeNode(id, name, position);
                    ceo->addSubordinate(employee);

                    int numSubordinates;
                    std::cout << "Enter the number of subordinates for " << name << ": ";
                    std::cin >> numSubordinates;

                    for (int j = 0; j < numSubordinates; ++j) {
                        int subId;
                        std::string subName, subPosition;
                        std::cout << "\nEnter details for subordinate " << (j + 1) << " of " << name << ":\n";
                        std::cout << "ID: ";
                        std::cin >> subId;
                        std::cin.ignore();
                        std::cout << "Name: ";
                        std::getline(std::cin, subName);
                        std::cout << "Position: ";
                        std::getline(std::cin, subPosition);

                        EmployeeNode* subordinate = new EmployeeNode(subId, subName, subPosition);
                        employee->addSubordinate(subordinate);
                    }
                }

                std::cout << "Company created successfully!\n";
                break;
            }
            case 2: {
                system("cls");
                std::cout << "+======================================+\n";
                std::cout << "|                Employee              |\n";
                std::cout << "+======================================+\n";
                searchEmployee(ceo);
                break;
            }
            case 3: {
                system("cls");
                std::cout << "+======================================+\n";
                std::cout << "|          Company Hierarchy           |\n";
                std::cout << "+======================================+\n";
                if (ceo) {
                    ceo->displayHierarchy();
                } else {
                    std::cout << "No company exists.\n";
                }
                break;
            }
            case 4: {
                if (ceo) {
                    system("cls");
                    std::string filename;
                    std::cout << "Enter filename to save employee information: ";
                    std::cin >> filename;

                    ceo->writeEmployeeInfoToFile(filename);
                    std::cout << "Employee information written to " << filename << " successfully.\n";
                } else {
                    std::cout << "No company exists.\n";
                }
                break;
            }
            case 5: {
                system("cls");
                int actionChoice;
                std::cout << "+======================================+\n";
                std::cout << "|     management information system    |\n";
                std::cout << "+======================================+\n";
                std::cout << "Choose an action\n";
                std::cout << "1. Add Subordinate\n";
                std::cout << "2. Promote Employee\n";
                std::cout << "3. Demote Employee\n";
                std::cout << "4. Delete Employee\n";
                std::cout << "5. Update Employee\n";
                std::cout << "Enter your choice: ";
                std::cin >> actionChoice;

                int id;
                std::string name, position;
                int supervisorId = -1; // Only needed for adding subordinates

                switch (actionChoice) {
                    case 1: {
                        system("cls");
                        std::cout << "+======================================+\n";
                        std::cout << "|            Add Subordinate           |\n";
                        std::cout << "+======================================+\n";
                        std::cout << "Enter ID of the subordinate: ";
                        std::cin >> id;
                        std::cin.ignore(); // Clear newline
                        std::cout << "Enter name of the subordinate: ";
                        std::getline(std::cin, name);
                        std::cout << "Enter position of the subordinate: ";
                        std::getline(std::cin, position);
                        std::cout << "Enter ID of the supervisor: ";
                        std::cin >> supervisorId;

                        manageEmployee(ceo, ADD_SUBORDINATE, id, name, position, supervisorId);
                        break;
                    }
                    case 2: {
                        system("cls");
                        std::cout << "+======================================+\n";
                        std::cout << "|            Promote Employee          |\n";
                        std::cout << "+======================================+\n";
                        std::cout << "Enter ID of the employee to promote: ";
                        std::cin >> id;
                        std::cin.ignore();
                        std::cout << "Enter new position: ";
                        std::getline(std::cin, position);

                        if (manageEmployee(ceo, PROMOTE, id, "", position)) {
                            std::cout << "Employee promoted successfully.\n";
                        } else {
                            std::cout << "Employee not found or promotion failed.\n";
                        }
                        break;
                    }
                    case 3: {
                        system("cls");
                        std::cout << "+======================================+\n";
                        std::cout << "|             Demote Employee          |\n";
                        std::cout << "+======================================+\n";
                        std::cout << "Enter ID of the employee to demote: ";
                        std::cin >> id;
                        std::cin.ignore();
                        std::cout << "Enter new position: ";
                        std::getline(std::cin, position);

                        if (manageEmployee(ceo, DEMOTE, id, "", position)) {
                            std::cout << "Employee demoted successfully.\n";
                        } else {
                            std::cout << "Employee not found or demotion failed.\n";
                        }
                        break;
                    }
                    case 4: {
                        system("cls");
                        std::cout << "+======================================+\n";
                        std::cout << "|            Delete Employee           |\n";
                        std::cout << "+======================================+\n";
                        std::cout << "Enter ID of the employee to delete: ";
                        std::cin >> id;

                        if (manageEmployee(ceo, DELETE, id)) {
                            std::cout << "Employee deleted successfully.\n";
                        } else {
                            std::cout << "Employee not found or deletion failed.\n";
                        }
                        break;
                    }
                    case 5: {
                        system("cls");
                        std::cout << "+======================================+\n";
                        std::cout << "|             Update Employee          |\n";
                        std::cout << "+======================================+\n";
                        int updateId;
                        std::string newName, newPosition;
                        std::cout << "Enter ID of the employee to update: ";
                        std::cin >> updateId;
                        std::cin.ignore();
                        std::cout << "Enter new name (or leave empty): ";
                        std::getline(std::cin, newName);
                        std::cout << "Enter new position (or leave empty): ";
                        std::getline(std::cin, newPosition);

                        manageEmployee(ceo, UPDATE, updateId, newName, newPosition);
                        break;
                    }
                    default:
                        std::cout << "Invalid action choice.\n";
                }
                break;
            }
            case 0:
                system("cls");
                std::cout << "Exiting...\n";
                break;
            default:
                std::cout << "Invalid choice. Try again.\n";
        }
    } while (operation != 0);

    // Clean up memory
    delete ceo;

    return 0;
}