// CSVUtils.cpp
#include "CSVUtils.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <sstream>
namespace fs = std::filesystem;



void readCSV(const std::string& filename, std::vector<UserData>& userDataList) {

  std::ofstream outputFile(filename, std::ios::app);

    if (outputFile.is_open()) {
        outputFile.close();
    } else {
        std::cerr << "Error creating the client file." << std::endl;
    }
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
          userData.followingList.insert(followingUser);
        }
      }

      // Read follower_list
      std::string followerLabel;
      iss >> followerLabel;

      if (followerLabel == "follower_list:") {
        std::string followerUser;
        while (iss >> followerUser && followerUser.back() != ';') {
          userData.followerList.insert(followerUser);
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
    it->followingList.insert(newFollowingUser);
  }
  auto it1 = std::find_if(userDataList.begin(), userDataList.end(),
                         [&targetUser, &newFollowingUser](const UserData& user) {
                           return user.username == newFollowingUser;
                         });
  if (it1 != userDataList.end()) {
    it1->followerList.insert(targetUser);
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
    it->followerList.insert(newFollowerUser);
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

void findAllUsers(const std::string& directory, std::vector<std::string>& clientNames) {
    

    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().filename().string().find("client_") == 0) {
                std::string fileName = entry.path().filename().string();
                std::string clientName = fileName.substr(7, fileName.size() - 11);
                
                clientNames.push_back(clientName);
            }
        }
    } catch (const std::filesystem::filesystem_error& ex) {
        std::cerr << "Error accessing the directory: " << ex.what() << std::endl;
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
        userData.followingList.insert(followingUser);
      }
    }

    // Read follower_list
    std::string followerLabel;
    iss >> followerLabel;

    if (followerLabel == "follower_list:") {
      std::string followerUser;
      while (iss >> followerUser && followerUser.back() != ';') {
        userData.followerList.insert(followerUser);
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