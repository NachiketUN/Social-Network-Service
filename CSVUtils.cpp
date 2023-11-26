// CSVUtils.cpp
#include "CSVUtils.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

void readCSV(const std::string& filename, std::vector<UserData>& userDataList) {
  std::ifstream file(filename);

  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      std::istringstream iss(line);
      UserData userData;

      // Read username
      iss >> userData.username;

      // Skip the delimiter
      char delimiter;
      iss >> delimiter;

      // Read following_list
      std::string followingLabel;
      iss >> followingLabel;

      if (followingLabel == "following_list:") {
        std::string followingUser;
        while (iss >> followingUser && followingUser.back() != ';') {
          userData.followingList.push_back(followingUser);
        }
      }

      // Read follower_list
      std::string followerLabel;
      iss >> followerLabel;

      if (followerLabel == "follower_list:") {
        std::string followerUser;
        while (iss >> followerUser && followerUser.back() != ';') {
          userData.followerList.push_back(followerUser);
        }
      }

      userDataList.push_back(userData);
    }

    file.close();
  } else {
    std::cerr << "Unable to open file: " << filename << std::endl;
  }
}

void writeCSV(const std::string& filename,
              const std::vector<UserData>& userDataList) {
  std::ofstream file(filename);

  if (file.is_open()) {
    for (const auto& userData : userDataList) {
      file << userData.username << " ; following_list:";
      for (const auto& followingUser : userData.followingList) {
        file << " " << followingUser;
      }

      file << " ; follower_list:";
      for (const auto& followerUser : userData.followerList) {
        file << " " << followerUser;
      }

      file << " ;\n";
    }

    file.close();
  } else {
    std::cerr << "Unable to open file: " << filename << std::endl;
  }
}

void updateFollowingList(std::vector<UserData>& userDataList,
                         const std::string& targetUser,
                         const std::string& newFollowingUser) {
  auto it = std::find_if(userDataList.begin(), userDataList.end(),
                         [&targetUser](const UserData& user) {
                           return user.username == targetUser;
                         });

  if (it != userDataList.end()) {
    it->followingList.push_back(newFollowingUser);
  }
}

void addUser(std::vector<UserData>& userDataList, const std::string& username) {
  // Check if the user already exists
  auto it = std::find_if(
      userDataList.begin(), userDataList.end(),
      [&username](const UserData& user) { return user.username == username; });

  // Add the user if not found
  if (it == userDataList.end()) {
    UserData newUser;
    newUser.username = username;
    userDataList.push_back(newUser);
  }
}

void updateFollowerList(std::vector<UserData>& userDataList,
                        const std::string& targetUser,
                        const std::string& newFollowerUser) {
  // Find the user with the target username
  auto it = std::find_if(userDataList.begin(), userDataList.end(),
                         [&targetUser](const UserData& user) {
                           return user.username == targetUser;
                         });

  // If user is found, update the follower list
  if (it != userDataList.end()) {
    it->followerList.push_back(newFollowerUser);
  }
}

void printUserDataList(const std::vector<UserData>& userDataList) {
  std::cout << "UserDataList:\n";

  for (const auto& userData : userDataList) {
    std::cout << "Username: " << userData.username << "\n";
    std::cout << "Following List:";
    for (const auto& followingUser : userData.followingList) {
      std::cout << " " << followingUser;
    }
    std::cout << "\n";

    std::cout << "Follower List:";
    for (const auto& followerUser : userData.followerList) {
      std::cout << " " << followerUser;
    }
    std::cout << "\n\n";
  }
}

std::string readCSVToString(const std::string& filename) {
  std::ifstream file(filename);

  if (file.is_open()) {
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
  } else {
    std::cerr << "Unable to open file: " << filename << std::endl;
    return "";  // Return an empty string if there's an error
  }
}

void convertStringToUserDataList(const std::string& csvString,
                                 std::vector<UserData>& userDataList) {
  std::istringstream iss(csvString);

  std::string line;
  while (std::getline(iss, line)) {
    std::istringstream iss(line);
    UserData userData;

    // Read username
    iss >> userData.username;

    // Skip the delimiter
    char delimiter;
    iss >> delimiter;

    // Read following_list
    std::string followingLabel;
    iss >> followingLabel;

    if (followingLabel == "following_list:") {
      std::string followingUser;
      while (iss >> followingUser && followingUser.back() != ';') {
        userData.followingList.push_back(followingUser);
      }
    }

    // Read follower_list
    std::string followerLabel;
    iss >> followerLabel;

    if (followerLabel == "follower_list:") {
      std::string followerUser;
      while (iss >> followerUser && followerUser.back() != ';') {
        userData.followerList.push_back(followerUser);
      }
    }

    userDataList.push_back(userData);
  }

}

bool containsUser(const std::vector<UserData>& userDataList, const std::string& username) {
    return std::any_of(userDataList.begin(), userDataList.end(),
                       [&username](const UserData& userData) { return userData.username == username; });
}

void getUsernamesWithFollowingList(const std::vector<UserData>& userList, const std::string& username2, std::vector<std::string>& usernames) {
    for (const auto& userData : userList) {
        // Check if username2 is in the followingList of the current user
        if (std::find(userData.followingList.begin(), userData.followingList.end(), username2) != userData.followingList.end()) {
            usernames.push_back(userData.username);
        }
    }
}