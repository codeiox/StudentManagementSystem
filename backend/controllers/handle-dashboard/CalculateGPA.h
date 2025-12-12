#ifndef CALCULATEGPA_H
#define CALCULATEGPA_H

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

double calculateOverAllGPA(std::vector<std::pair<std::string, int>> list_of_grades);
int calculateTotalCredits(const std::vector<std::pair<std::string, int>>& list_of_grades);

int calculateAccumulatedCredit(const std::vector<std::pair<std::string, int>>& list_of_grades);

#endif  // CALCULATEGPA_H