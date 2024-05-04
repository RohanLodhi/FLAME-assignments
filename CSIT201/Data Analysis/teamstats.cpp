#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include "json.hpp"

using json = nlohmann::json;

struct TeamData {
    int points;
    std::string teamName;
    int success_raids;
    int unsuccess_raids;
    int superRaids;
    int tackles;
    int all_outs;
    double ratio;
};

struct LinearRegressionResult {
    double slope;
    double intercept;
};

double calculateCorrelation(const std::vector<int>& x, const std::vector<int>& y) {
    if (x.size() != y.size() || x.empty()) return 0.0;  // safety check

    int n = x.size();
    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0, sumY2 = 0;

    for (int i = 0; i < n; i++) {
        sumX += x[i];
        sumY += y[i];
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
        sumY2 += y[i] * y[i];
    }

    double numerator = n * sumXY - sumX * sumY;
    double denominator = sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));

    return (denominator == 0) ? 0 : (numerator / denominator);
}

class KabaddiAnalysis {
private:
    std::vector<TeamData> teamsData;

    static int partition(std::vector<TeamData>& arr, int low, int high) {
        int pivot = arr[high].points;  // pivot
        int i = (low - 1);  // Index of smaller element

        for (int j = low; j <= high - 1; j++) {
            // If current element is smaller than the pivot
            if (arr[j].points > pivot) {
                i++;  // increment index of smaller element
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[i + 1], arr[high]);
        return (i + 1);
    }

    static void quickSort(std::vector<TeamData>& arr, int low, int high) {
        if (low < high) {
            int pi = partition(arr, low, high);

            quickSort(arr, low, pi - 1);
            quickSort(arr, pi + 1, high);
        }
    }

    LinearRegressionResult calculateLinearRegression(const std::vector<int>& x, const std::vector<int>& y) {
        int n = static_cast<int>(x.size());
        double sumX = std::accumulate(x.begin(), x.end(), 0.0);
        double sumY = std::accumulate(y.begin(), y.end(), 0.0);
        double sumX2 = std::inner_product(x.begin(), x.end(), x.begin(), 0.0);
        double sumXY = std::inner_product(x.begin(), x.end(), y.begin(), 0.0);

        double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
        double intercept = (sumY - slope * sumX) / n;

        return {slope, intercept};
    }

    void merge(std::vector<TeamData>& arr, int l, int m, int r) {
        int n1 = m - l + 1;
        int n2 = r - m;

        // Create temp arrays
        std::vector<TeamData> L(n1), R(n2);

        // Copy data to temp arrays L[] and R[]
        for (int i = 0; i < n1; i++)
            L[i] = arr[l + i];
        for (int j = 0; j < n2; j++)
            R[j] = arr[m + 1 + j];

        // Merge the temp arrays back into arr[l..r]
        int i = 0, j = 0, k = l;
        while (i < n1 && j < n2) {
            if (L[i].ratio >= R[j].ratio) {
                arr[k] = L[i];
                i++;
            } else {
                arr[k] = R[j];
                j++;
            }
            k++;
        }

        // Copy the remaining elements of L[], if there are any
        while (i < n1) {
            arr[k] = L[i];
            i++;
            k++;
        }

        // Copy the remaining elements of R[], if there are any
        while (j < n2) {
            arr[k] = R[j];
            j++;
            k++;
        }
    }

    // Main function that sorts arr[l..r] using merge()
    void mergeSort(std::vector<TeamData>& arr, int l, int r) {
        if (l < r) {
            int m = l + (r - l) / 2;

            // Sort first and second halves
            mergeSort(arr, l, m);
            mergeSort(arr, m + 1, r);

            merge(arr, l, m, r);
        }
    }


public:
    KabaddiAnalysis(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }

        json j;
        file >> j; // Deserialize the JSON data directly from the file

        for (const auto& item : j) {
            TeamData team;
            team.teamName = item["bio"]["team_name"];
            team.points = item["over_all_stats"]["points"];
            team.success_raids = item["over_all_stats"]["success_raids"];
            team.unsuccess_raids = item["over_all_stats"]["unsuccess_raids"];
            team.superRaids = item["over_all_stats"]["super_raids"];
            team.tackles = item["over_all_stats"]["tackles"];
            team.all_outs = item["over_all_stats"]["all_outs"];
            teamsData.push_back(team);
        }

        file.close();
    }

    void displayAllTeamData() {
        for (const auto& team : teamsData) {
            std::cout << "Team Name: " << team.teamName
                      << ", Points: " << team.points
                      << ", Successful Raids: " << team.success_raids
                      << ", Unsuccessful Raids: " << team.unsuccess_raids
                      << ", Super Raids: " << team.superRaids
                      << ", Tackles: " << team.tackles
                      << ", All Outs: " << team.all_outs
                      << std::endl;
        }
    }

    void analyzeCorrelation() {
        std::vector<int> points, successRaids;
        for (const auto& team : teamsData) {
            points.push_back(team.points);
            successRaids.push_back(team.success_raids);
        }

        double correlation = calculateCorrelation(successRaids, points);
        std::cout << "Correlation between Successful Raids and Points: " << correlation << std::endl;
    }

    void sortTeamsByPoints() {
        quickSort(teamsData, 0, teamsData.size() - 1);
        for (const auto& team : teamsData) {
            std::cout << "Team Name: " << team.teamName << ", Points: " << team.points << std::endl;
        }
    }

    void predictPointsFromRaids() {
        std::vector<int> raidCounts, points;
        for (const auto& team : teamsData) {
            raidCounts.push_back(team.success_raids);
            points.push_back(team.points);
        }

        LinearRegressionResult regression = calculateLinearRegression(raidCounts, points);
        std::cout << "Prediction model (Points from Raids): Points = " 
                  << regression.slope << " * SuccessfulRaids + " << regression.intercept << std::endl;

        // Example prediction for a new team
        int newTeamRaids = 250; // Hypothetical successful raid count for a new team
        double predictedPoints = regression.slope * newTeamRaids + regression.intercept;
        std::cout << "Predicted points for " << newTeamRaids << " successful raids: " << predictedPoints << std::endl;
    }

    void optimizeStrategy() {
        std::vector<int> tackleCounts, points;
        for (const auto& team : teamsData) {
            tackleCounts.push_back(team.tackles);
            points.push_back(team.points);
        }

        LinearRegressionResult regression = calculateLinearRegression(tackleCounts, points);
        std::cout << "Optimization model (Points from Tackles): Points = " 
                  << regression.slope << " * Tackles + " << regression.intercept << std::endl;

        // Strategy recommendation based on slope
        if (regression.slope > 0.2) {
            std::cout << "Increasing tackles is predicted to increase points." << std::endl;
        } else {
            std::cout << "Increasing tackles does not significantly increase points; focus may be better placed elsewhere." << std::endl;
        }
    }
    void sortByRaidToTackleRatio() {
        // Calculate the ratio for each team
        for (auto& team : teamsData) {
            team.ratio = team.tackles == 0 ? std::numeric_limits<double>::infinity() :
                         static_cast<double>(team.success_raids) / team.tackles;
        }

        // Sort using mergeSort
        mergeSort(teamsData, 0, teamsData.size() - 1);

        // Print the sorted data
        std::cout << "Teams sorted by Raid to Tackle Ratio:" << std::endl;
        for (const auto& team : teamsData) {
            std::cout << "Team: " << team.teamName 
                      << ", Ratio: " << team.ratio 
                      << ", Points: " << team.points 
                      << std::endl;
        }
    }
};

int main() {
    KabaddiAnalysis ka("./stats_team.json");
    ka.analyzeCorrelation();
    ka.predictPointsFromRaids();
    ka.optimizeStrategy();
    ka.sortTeamsByPoints();
    ka.sortByRaidToTackleRatio();
    return 0;
}