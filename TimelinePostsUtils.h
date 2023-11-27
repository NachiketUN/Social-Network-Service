// TimelinePostsUtils.h
#ifndef TIMELINE_POSTS_UTILS_H
#define TIMELINE_POSTS_UTILS_H

#include <string>
#include <vector>

struct TimelinePosts {
    std::string timestamp;
    std::string username;
    std::string content;
    bool syncProcessing;
    bool clientProcessing;
};

void readTimelinePosts(const std::string& filename, std::vector<TimelinePosts> &postsList);
void readTimelinePosts(const std::string& filename, std::vector<TimelinePosts> &postsList, std::string &username);
void writeTimelinePosts(const std::string& filename, const std::vector<TimelinePosts>& postsList);
void appendTimelinePosts(const std::string& filename, const std::vector<TimelinePosts>& postsList);

void printTimelinePostsList(const std::vector<TimelinePosts>& postsList);
std::string processTimelinePosts(std::vector<TimelinePosts>& postsList, std::string &username);
void convertStringToTimelinePosts(std::string postsString, std::vector<TimelinePosts> &postsList);



#endif // TIMELINE_POSTS_UTILS_H
