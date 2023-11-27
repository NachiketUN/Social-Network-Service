// TimelinePostsUtils.cpp
#include "TimelinePostsUtils.h"

#include <fstream>
#include <iostream>
#include <sstream>

void readTimelinePosts(const std::string& filename, std::vector<TimelinePosts> &postsList, std::string &username){
  std::ifstream file(filename);

  if (file.is_open()) {
    TimelinePosts posts;
    std::string line;

    while (std::getline(file, line)) {
      if (line.empty()) {
        continue;  // Skip empty lines
      }

      std::istringstream iss(line);
      char type;
      iss >> type;

      switch (type) {
        case 'T':

          iss >> std::ws;
          std::getline(iss, posts.timestamp);
          break;
        case 'U':
          iss >> posts.username;
          break;
        case 'W':
          iss >> posts.content;
          break;
        case 'S':
          int syncProcessing;
          iss >> syncProcessing;
          posts.syncProcessing = static_cast<bool>(syncProcessing);
          break;
        case 'C':
          int clientProcessing;
          iss >> clientProcessing;
          posts.clientProcessing = static_cast<bool>(clientProcessing);
          if(posts.username == username){
            postsList.push_back(posts);  // Push the posts to the list after reading 'C'
          }
          break;
        default:
          std::cerr << "Unknown type: " << type << std::endl;
      }
    }

    file.close();
  } else {
    std::cerr << "Unable to open file: " << filename << std::endl;
  }
}

void readTimelinePosts(const std::string& filename,
                       std::vector<TimelinePosts>& postsList) {
  std::ifstream file(filename);

  if (file.is_open()) {
    TimelinePosts posts;
    std::string line;

    while (std::getline(file, line)) {
      if (line.empty()) {
        continue;  // Skip empty lines
      }

      std::istringstream iss(line);
      char type;
      iss >> type;

      switch (type) {
        case 'T':

          iss >> std::ws;
          std::getline(iss, posts.timestamp);
          break;
        case 'U':
          iss >> posts.username;
          break;
        case 'W':
          iss >> posts.content;
          break;
        case 'S':
          int syncProcessing;
          iss >> syncProcessing;
          posts.syncProcessing = static_cast<bool>(syncProcessing);
          break;
        case 'C':
          int clientProcessing;
          iss >> clientProcessing;
          posts.clientProcessing = static_cast<bool>(clientProcessing);
          postsList.push_back(
              posts);  // Push the posts to the list after reading 'C'
          break;
        default:
          std::cerr << "Unknown type: " << type << std::endl;
      }
    }

    file.close();
  } else {
    std::cerr << "Unable to open file: " << filename << std::endl;
  }
}

void writeTimelinePosts(const std::string& filename,
                        const std::vector<TimelinePosts>& postsList) {
  std::ofstream file(filename);

  if (file.is_open()) {
    for (const auto& posts : postsList) {
      file << "T " << posts.timestamp << "\n";
      file << "U " << posts.username << "\n";
      file << "W " << posts.content << "\n";
      file << "S " << static_cast<int>(posts.syncProcessing) << "\n";
      file << "C " << static_cast<int>(posts.clientProcessing) << "\n\n";
    }

    file.close();
  } else {
    std::cerr << "Unable to open file: " << filename << std::endl;
  }
}

void printTimelinePostsList(const std::vector<TimelinePosts>& postsList) {
  for (const auto& posts : postsList) {
    std::cout << "Timestamp: " << posts.timestamp << "\n";
    std::cout << "User Status: " << posts.username << "\n";
    std::cout << "Content: " << posts.content << "\n";
    std::cout << "System Status: " << posts.syncProcessing << "\n";
    std::cout << "clientProcessing: " << posts.clientProcessing << "\n\n";
  }
}

// std::string convertTimelinePostsToString(std::vector<TimelinePosts>&
// postsList) {
//     std::ostringstream result;

//     for (auto& posts : postsList) {
//         if(posts.syncProcessing == true) continue;
//         result << "T " << posts.timestamp << "\n";
//         result << "U " << posts.username << "\n";
//         result << "W " << posts.content << "\n";
//         result << "S " << "1" << "\n";
//         // result << "S " << static_cast<int>(posts.systemStatus) << "\n";
//         result << "C " << "0" << "\n\n";

//         // Modify the systemStatus in the copy
//         posts.syncProcessing = true;
//     }

//     return result.str();
// }

std::string processTimelinePosts(std::vector<TimelinePosts>& postsList, std::string &username) {
  std::ostringstream result;

  for (auto& posts : postsList) {
    if (posts.syncProcessing == true || posts.username != username) continue;
    result << "T " << posts.timestamp << "\n";
    result << "U " << posts.username << "\n";
    result << "W " << posts.content << "\n";
    result << "S "
           << "1"
           << "\n";
    // result << "S " << static_cast<int>(posts.systemStatus) << "\n";
    result << "C "
           << "0"
           << "\n\n";

    // Modify the systemStatus in the copy
    posts.syncProcessing = true;
  }

  return result.str();
}

void convertStringToTimelinePosts(std::string postsString,
                                  std::vector<TimelinePosts>& postsList) {
  std::istringstream iss(postsString);

  TimelinePosts posts;
  std::string line;

  while (std::getline(iss, line)) {
    if (line.empty()) {
      continue;  // Skip empty lines
    }

    std::istringstream iss(line);
    char type;
    iss >> type;

    switch (type) {
      case 'T':

        iss >> std::ws;
        std::getline(iss, posts.timestamp);
        break;
      case 'U':
        iss >> posts.username;
        break;
      case 'W':
        iss >> posts.content;
        break;
      case 'S':
        int syncProcessing;
        iss >> syncProcessing;
        posts.syncProcessing = static_cast<bool>(syncProcessing);
        break;
      case 'C':
        int clientProcessing;
        iss >> clientProcessing;
        posts.clientProcessing = static_cast<bool>(clientProcessing);
        postsList.push_back(
            posts);  // Push the posts to the list after reading 'C'
        break;
      default:
        std::cerr << "Unknown type: " << type << std::endl;
    }
  }

//   while (std::getline(iss, line)) {
//     if (line.empty()) {
//       continue;  // Skip empty lines
//     }

//     TimelinePosts posts;
//     std::istringstream lineIss(line);

//     lineIss >> std::ws;  // Skip leading whitespaces
//     char type;
//     lineIss >> type;

//     switch (type) {
//       case 'T':
//         lineIss >> posts.timestamp;
//         break;
//       case 'U':
//         lineIss >> posts.username;
//         break;
//       case 'W':
//         lineIss >> posts.content;
//         break;
//       case 'S':
//         int syncProcessing;  // Updated variable name
//         lineIss >> syncProcessing;
//         posts.syncProcessing = static_cast<bool>(syncProcessing);
//         break;
//       case 'C':
//         int clientProcessing;  // Updated variable name
//         lineIss >> clientProcessing;
//         posts.clientProcessing = static_cast<bool>(clientProcessing);
//         postsList.push_back(
//             posts);  // Push the posts to the list after reading 'C'
//         break;
//       default:
//         std::cerr << "Unknown type: " << type << std::endl;
//     }
//   }
}

void appendTimelinePosts(const std::string& filename,
                         const std::vector<TimelinePosts>& postsList) {
  std::ofstream file(filename, std::ios::app);

  if (file.is_open()) {
    for (const auto& posts : postsList) {
        // std::cout<<"in append"<<posts.username<<std::endl;
      file << "T " << posts.timestamp << "\n";
      file << "U " << posts.username << "\n";
      file << "W " << posts.content << "\n";
      file << "S " << static_cast<int>(posts.syncProcessing) << "\n";
      file << "C " << static_cast<int>(posts.clientProcessing) << "\n\n";
    }

    file.close();
  } else {
    std::cerr << "Unable to open file: " << filename << std::endl;
  }
}