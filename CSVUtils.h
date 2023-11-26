#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include <string>
#include <vector>

struct UserData {
    std::string username;
    std::vector<std::string> followingList;
    std::vector<std::string> followerList;
};

void readCSV(const std::string& filename, std::vector<UserData> &userDataList);
void writeCSV(const std::string& filename, const std::vector<UserData>& userDataList);
void updateFollowingList(std::vector<UserData>& userDataList, const std::string& targetUser, const std::string& newFollowingUser);
void addUser(std::vector<UserData>& userDataList, const std::string& username);
void updateFollowerList(std::vector<UserData>& userDataList, const std::string& targetUser, const std::string& newFollowerUser);
bool hasCSVChanged(const std::string& filename, const std::vector<UserData>& userDataList);
void printUserDataList(const std::vector<UserData>& userDataList);
std::string readCSVToString(const std::string& filename);
void convertStringToUserDataList(const std::string& csvString,std::vector<UserData> &userDataList);
bool containsUser(const std::vector<UserData>& userDataList, const std::string& username);

void getUsernamesWithFollowingList(const std::vector<UserData>& userList, const std::string& username2,std::vector<std::string> &usernames);


#endif // CSV_UTILS_H
