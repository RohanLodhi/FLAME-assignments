#include <iostream>
#include <fstream>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;



class KabaddiAnalysis {
private:
    struct TeamData {
        int matchId;
        int teamId; 
        std::string teamName;
        int tossWinner;
        std::string matchOutcome;
        std::string homeOrAway; 
        int totalRaids;
        int successfulRaids;
        int superRaids;
        int doOrDieRaids = 0;  
        int doOrDieRaidsSuccessful = 0;  
        int totalTackles;
        int successfulTackles;
        int points;
        int all_outs; 
        int bonus_points;
    };

    std::vector<TeamData> teamsData;

    double mean(const std::vector<double>& data) {
        return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
    }

    double variance(const std::vector<double>& data) {
        double m = mean(data);
        double accum = 0.0;
        std::for_each(data.begin(), data.end(), [&](const double d) {
            accum += (d - m) * (d - m);
        });
        return accum / (data.size() - 1);
    }

    double tTest(const std::vector<double>& sample1, const std::vector<double>& sample2) {
        double mean1 = mean(sample1);
        double mean2 = mean(sample2);
        double var1 = variance(sample1);
        double var2 = variance(sample2);
        double se = sqrt(var1 / sample1.size() + var2 / sample2.size());
        return (mean1 - mean2) / se;
    }

public:
    void loadFromJson(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }

        json j;
        file >> j;

        for (auto& match : j) {
            TeamData teamData;
            int matchId = match["match_detail"]["match_id"].get<int>();
            int tossWinner = match["match_detail"]["toss"]["winner"].is_null() ? -1 : match["match_detail"]["toss"]["winner"].get<int>();
            std::string winningTeam = match["match_detail"]["result"]["winning_team"].is_null() ? "" : match["match_detail"]["result"]["winning_team"].get<std::string>();
            std::string homeTeamId = match["teams"]["home_team_id"].is_null() ? "" : match["teams"]["home_team_id"].get<std::string>();

            for (auto& team : match["teams"]["team"]) {
                teamData.matchId = matchId;
                teamData.teamId = stoi(team["id"].get<std::string>());
                teamData.teamName = team["name"].is_null() ? "Unknown Team" : team["name"].get<std::string>();
                teamData.tossWinner = tossWinner;
                teamData.matchOutcome = team["name"].get<std::string>() == winningTeam ? "win" : "lose";
                teamData.homeOrAway = homeTeamId.empty() || homeTeamId == "-1" ? "unknown" : (stoi(homeTeamId) == teamData.teamId ? "home" : "away");
                teamData.totalRaids = team["stats"]["raids"]["total"].is_null() ? 0 : team["stats"]["raids"]["total"].get<int>();
                teamData.successfulRaids = team["stats"]["raids"]["successful"].is_null() ? 0 : team["stats"]["raids"]["successful"].get<int>();
                teamData.superRaids = team["stats"]["raids"]["super_raids"].is_null() ? 0 : team["stats"]["raids"]["super_raids"].get<int>();
                teamData.totalTackles = team["stats"]["tackles"]["total"].is_null() ? 0 : team["stats"]["tackles"]["total"].get<int>();
                teamData.successfulTackles = team["stats"]["tackles"]["successful"].is_null() ? 0 : team["stats"]["tackles"]["successful"].get<int>();
                teamData.points = team["stats"]["points"]["total"].is_null() ? 0 : team["stats"]["points"]["total"].get<int>();
                teamData.all_outs = team["stats"]["points"]["all_out"].is_null() ? 0 : team["stats"]["points"]["all_out"].get<int>();
                teamData.all_outs = team["stats"]["points"]["extras"].is_null() ? 0 : team["stats"]["points"]["extras"].get<int>();

                for (auto& event : match["events"]["event"]){
                    bool home = event["do_or_die"] == true ? true : false;
                    if(home){
                        teamData.doOrDieRaids++;
                        if(event["event_id"] == 1) teamData.doOrDieRaidsSuccessful++;
                    }
                }

                teamsData.push_back(teamData);
            }
        }
    }


    void displayAllTeamData() {
        for (const auto& team : teamsData) {
            std::cout << "Match ID: " << team.matchId
                    << ", Team Name: " << team.teamName
                    << ", Toss Winner: " << (team.teamId == team.tossWinner ? "Yes" : "No")
                    << ", Match Outcome: " << team.matchOutcome
                    << ", Total Raids: " << team.totalRaids
                    << ", Successful Raids: " << team.successfulRaids
                    << ", Super Raids: " << team.superRaids
                    << ", Total Tackles: " << team.totalTackles
                    << ", Successful Tackles: " << team.successfulTackles
                    << std::endl;
        }
    }

    void tossVsWin() {
        double i = 0, j = 0, correlation;
        for (const auto& team : teamsData) {
            team.teamId == team.tossWinner && team.matchOutcome == "win" ? i++ : j++;
        }
        correlation = i/j;
        std::cout << "Toss vs Win correlation: " << correlation << std::endl;
    }

    void homeAwayVsMatchOutcome() {
        int homeWins = 0, totalHomeGames = 0;
        for (const auto& team : teamsData) {
            if (team.homeOrAway == "home") {
                totalHomeGames++;
                if (team.matchOutcome == "win") {
                    homeWins++;
                }
            }
        }
        std::cout << "Home advantage in matches: " << static_cast<double>(homeWins) / totalHomeGames << std::endl;
    }

    void superRaidsVsOutcome() {
        double winWithSuperRaids = 0, totalWithSuperRaids = 0, totalMatches = 0;
        for (const auto& team : teamsData) {
            totalMatches++;
            if (team.superRaids > 0) { // only consider matches with at least one super raid
                totalWithSuperRaids++;
                if (team.matchOutcome == "win") {
                    winWithSuperRaids++;
                }
            }
        }
        double correlation = totalWithSuperRaids ? winWithSuperRaids / totalWithSuperRaids : 0;
        std::cout << "Impact of Super Raids on match outcome: " << correlation << std::endl;
        std::cout << totalMatches << " " << totalMatches - totalWithSuperRaids << std::endl;
    }

    // Analyze the impact of do or die raids on match outcomes
    void doOrDieRaidsVsOutcome() {
        double totalDoOrDie = 0, totalSuccessDoOrDie = 0, totalSuccessRaids = 0, totalRaids = 0;
        for (const auto& team : teamsData) {
            totalDoOrDie += team.doOrDieRaids;
            totalSuccessDoOrDie += team.doOrDieRaidsSuccessful;
            totalSuccessRaids += team.successfulRaids;
            totalRaids += team.totalRaids;
        }
        double correlation1 = (totalSuccessDoOrDie/totalDoOrDie)*100;
        double correlation2 = totalSuccessRaids/totalRaids;
        std::cout << "Percentage of successful Do or Die Raids " << correlation1 << std::endl;
        std::cout << "Percentage of successful Normal Raids " << correlation2 << std::endl;
    }

    // Analyze the correlation between total tackles and match outcomes
    void tacklesVsOutcome() {
        double winWithTackles = 0, totalWithTackles = 0;
        for (const auto& team : teamsData) {
            if (team.totalTackles > 0) { // only consider matches with at least one tackle
                totalWithTackles++;
                if (team.matchOutcome == "win") {
                    winWithTackles++;
                }
            }
        }
        double correlation = totalWithTackles ? winWithTackles / totalWithTackles : 0;
        std::cout << "Correlation between tackles and winning: " << correlation << std::endl;
    }

    void hypothesisTest() {
        std::vector<double> attackPoints, defensePoints, extraPoints, alloutPoints;
        for (const auto& data : teamsData) {
            attackPoints.push_back(data.successfulRaids);
            defensePoints.push_back(data.successfulTackles);
            extraPoints.push_back(data.bonus_points);
            alloutPoints.push_back(data.all_outs);
        }

        double tAttackDefense = tTest(attackPoints, defensePoints);
        double tDefenseExtra = tTest(defensePoints, extraPoints);
        double tDefenseAllout = tTest(defensePoints, alloutPoints);
        double tAlloutExtra = tTest(alloutPoints, extraPoints);

        std::cout << "T-Statistic for Attack vs. Defense: " << tAttackDefense << std::endl;
        std::cout << "T-Statistic for Defense vs. Extra: " << tDefenseExtra << std::endl;
        std::cout << "T-Statistic for Defense vs. Allout: " << tDefenseAllout << std::endl;
        std::cout << "T-Statistic for Allout vs. Extra: " << tAlloutExtra << std::endl;
    }

    void exportData() {
        std::ofstream out("match_data.csv");
        out << "MatchID,AttackPoints,DefensePoints,Outcome\n";
        for (const auto& match : teamsData) {
            out << match.matchId << "," << match.successfulRaids << "," << match.successfulTackles << "," << match.matchOutcome << "\n";
        }
        out.close();
    }

};

int main() {
    KabaddiAnalysis analysis;
    analysis.loadFromJson("./stats_match.json");
    analysis.displayAllTeamData();
    analysis.tossVsWin();
    analysis.homeAwayVsMatchOutcome();
    analysis.superRaidsVsOutcome();
    analysis.doOrDieRaidsVsOutcome();
    // analysis.tacklesVsOutcome();
    analysis.hypothesisTest();
    analysis.exportData();
    return 0;
}
