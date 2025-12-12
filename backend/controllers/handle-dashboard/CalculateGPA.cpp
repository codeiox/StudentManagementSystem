#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

double calculateOverAllGPA(std::vector<std::pair<std::string, int>> list_of_grades) {
    if (list_of_grades.empty()) {
        throw std::runtime_error("list of grades is empty");
    }

    double totalQualityPoints = 0.0;  // grade_points * credits
    int totalCredits = 0;
    // lambda function
    auto gradeToPoints = [](const std::string& grade) {
        if (grade == "A") return 4.0;
        if (grade == "A-") return 3.7;
        if (grade == "B+") return 3.3;
        if (grade == "B") return 3.0;
        if (grade == "B-") return 2.7;
        if (grade == "C+") return 2.3;
        if (grade == "C") return 2.0;
        if (grade == "C-") return 1.7;
        if (grade == "D+") return 1.3;
        if (grade == "D") return 1.0;
        if (grade == "D-") return 0.7;
        return 0.0;  // F or unknown
    };

    for (const auto& [grade, credit] : list_of_grades) {
        double points = gradeToPoints(grade);
        totalQualityPoints += points * credit;
        totalCredits += credit;
    }

    if (totalCredits == 0) {
        throw std::runtime_error("total credits cannot be zero");
    }

    return totalQualityPoints / totalCredits;
}

// This is specifically for calculating over all GPA and it's not a Accumulated Credit!
int calculateTotalCredits(const std::vector<std::pair<std::string, int>>& list_of_grades) {
    int totalCredits = 0;

    for (const auto& [grade, credit] : list_of_grades) {
        totalCredits += credit;
    }

    return totalCredits;
}

int calculateAccumulatedCredit(const std::vector<std::pair<std::string, int>>& list_of_grades) {
    int totalAccumulatedCredit = 0;
    for (const auto& [grade, credit] : list_of_grades) {
        if (grade == "F") continue;  // skip one set if grade is F
        totalAccumulatedCredit += credit;
    }
    return totalAccumulatedCredit;
}
