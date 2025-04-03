#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <algorithm>
#include <iomanip>
using namespace std;

// Structure to represent weighted connections
struct Connection {
    string targetUser;
    int strength; // Friendship strength (1-10)
    
    Connection(string user, int str) : targetUser(user), strength(str) {}
};

// User structure
struct User {
    string username;
    string name;
    vector<string> posts;
    vector<Connection> followers; // Weighted followers
    vector<Connection> following; // Weighted following
    User(string uname, string n) : username(uname), name(n) {}
};

// Social Network Graph class
class InstagramGraph {
private:
    map<string, User*> users;
    
public:
    InstagramGraph() {} // Constructor
    
    ~InstagramGraph() { // Destructor to clean up memory
        for (auto& pair : users) {
            delete pair.second;
        }
    }
    
    void addUser(string username, string name);
    void deleteUser(string username);
    void addPost(string username, string content);
    void followUser(string follower, string followee, int strength);
    void displayUserProfile(string username);
    void displayAllUsers();
    bool searchUser(string username);
    void BFSTraversal(string startUsername);
    void DFSTraversal(string startUsername);
    void suggestFriends(string username);
    void displayStrongestConnections(string username);
    void displayConnectionStrength(string user1, string user2);
};

// Add a new user to the network
void InstagramGraph::addUser(string username, string name) {
    if (users.find(username) != users.end()) {
        cout << "Username " << username << " already exists!" << endl;
        return;
    }
    User* newUser = new User(username, name);
    users[username] = newUser;
    cout << "User " << username << " added successfully." << endl;
}

// Delete a user from the network
void InstagramGraph::deleteUser(string username) {
    if (users.find(username) == users.end()) {
        cout << "User " << username << " not found." << endl;
        return;
    }
    
    // Remove from followers' following lists
    for (auto& follower : users[username]->followers) {
        User* followerUser = users[follower.targetUser];
        auto it = remove_if(followerUser->following.begin(), 
                          followerUser->following.end(),
                          [username](const Connection& c) { 
                              return c.targetUser == username; 
                          });
        followerUser->following.erase(it, followerUser->following.end());
    }
    
    // Remove from followees' followers lists
    for (auto& following : users[username]->following) {
        User* followeeUser = users[following.targetUser];
        auto it = remove_if(followeeUser->followers.begin(),
                          followeeUser->followers.end(),
                          [username](const Connection& c) {
                              return c.targetUser == username;
                          });
        followeeUser->followers.erase(it, followeeUser->followers.end());
    }
    
    delete users[username];
    users.erase(username);
    cout << "User " << username << " deleted successfully." << endl;
}

// Follow a user with connection strength
void InstagramGraph::followUser(string follower, string followee, int strength = 5) {
    if (users.find(follower) == users.end() || users.find(followee) == users.end()) {
        cout << "One or both users don't exist!" << endl;
        return;
    }
    
    if (strength < 1 || strength > 10) {
        cout << "Strength must be between 1-10. Using default (5)." << endl;
        strength = 5;
    }
    
    // Check if already following
    auto it = find_if(users[follower]->following.begin(), 
                     users[follower]->following.end(),
                     [&followee](const Connection& c) { 
                         return c.targetUser == followee; 
                     });
    
    if (it != users[follower]->following.end()) {
        cout << follower << " is already following " << followee << endl;
        return;
    }
    
    // Add to following list with strength
    users[follower]->following.emplace_back(followee, strength);
    // Add to followers list with strength
    users[followee]->followers.emplace_back(follower, strength);
    cout << follower << " is now following " << followee 
         << " (strength: " << strength << "/10)" << endl;
}

// Add a new post for a user
void InstagramGraph::addPost(string username, string content) {
    if (users.find(username) == users.end()) {
        cout << "User " << username << " not found." << endl;
        return;
    } 
    users[username]->posts.push_back(content);
    cout << "Post added successfully to " << username << "'s profile." << endl;
}

// Display user profile
void InstagramGraph::displayUserProfile(string username) {
    if (users.find(username) == users.end()) {
        cout << "User " << username << " not found." << endl;
        return;
    }
    
    User* user = users[username];
    cout << "\nProfile: " << username << " (" << user->name << ")\n";
    cout << "Followers: " << user->followers.size() 
         << " | Following: " << user->following.size() << "\n";
    
    cout << "\nPosts (" << user->posts.size() << "):\n";
    for (const auto& post : user->posts) {
        cout << "- " << post << "\n";
    }
}

// Display all users in the network
void InstagramGraph::displayAllUsers() {
    cout << "\nUsers in the network (" << users.size() << "):\n";
    for (const auto& pair : users) {
        cout << " " << pair.first << " (" << pair.second->name << ")\n";
    }
}

// Search for a user
bool InstagramGraph::searchUser(string username) {
    return users.find(username) != users.end();
}

// BFS Traversal
void InstagramGraph::BFSTraversal(string startUsername) {
    if (users.find(startUsername) == users.end()) {
        cout << "User " << startUsername << " not found." << endl;
        return;
    }
    
    queue<string> q;
    map<string, bool> visited;
    
    q.push(startUsername);
    visited[startUsername] = true;
    
    cout << "\nBFS Traversal starting from @" << startUsername << ":\n";
    while (!q.empty()) {
        string currentUsername = q.front();
        q.pop();
        cout << currentUsername << " ";
    
        for (const auto& connection : users[currentUsername]->following) {
            if (!visited[connection.targetUser]) {
                q.push(connection.targetUser);
                visited[connection.targetUser] = true;
            }
        }
    }
    cout << endl;
}

// DFS Traversal
void InstagramGraph::DFSTraversal(string startUsername) {
    if (users.find(startUsername) == users.end()) {
        cout << "User " << startUsername << " not found." << endl;
        return;
    }
    
    map<string, bool> visited;
    stack<string> s;
    
    s.push(startUsername);
    
    cout << "\nDFS Traversal starting from " << startUsername << ":\n";
    
    while (!s.empty()) {
        string currentUsername = s.top();
        s.pop();
    
        if (!visited[currentUsername]) {
            cout << currentUsername << " ";
            visited[currentUsername] = true;
    
            for (const auto& connection : users[currentUsername]->following) {
                s.push(connection.targetUser);
            }
        }
    }
    cout << endl;
}

// Suggest Friends (based on common followers)
void InstagramGraph::suggestFriends(string username) {
    if (users.find(username) == users.end()) {
        cout << "User " << username << " not found." << endl;
        return;
    }
    
    map<string, int> suggestions;
    User* user = users[username];
    
    for (const auto& follower : user->followers) {
        User* followerUser = users[follower.targetUser];
        for (const auto& followerOfFollower : followerUser->followers) {
            if (followerOfFollower.targetUser != username &&
                find_if(user->following.begin(), user->following.end(),
                    [&followerOfFollower](const Connection& c) { 
                        return c.targetUser == followerOfFollower.targetUser; 
                    }) == user->following.end() &&
                find_if(user->followers.begin(), user->followers.end(),
                    [&followerOfFollower](const Connection& c) { 
                        return c.targetUser == followerOfFollower.targetUser; 
                    }) == user->followers.end()) {
                suggestions[followerOfFollower.targetUser]++;
            }
        }
    }
    
    cout << "\nSuggested friends for " << username << ":\n";
    if (suggestions.empty()) {
        cout << "No friend suggestions found.\n";
        return;
    }
    
    vector<pair<string, int>> sortedSuggestions(suggestions.begin(), suggestions.end());
    sort(sortedSuggestions.begin(), sortedSuggestions.end(),
        [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second > b.second;
        });
    
    for (const auto& suggestion : sortedSuggestions) {
        cout << " " << suggestion.first << " (common followers: " << suggestion.second << ")\n";
    }
}

// Display strongest connections (users they follow, sorted by strength)
void InstagramGraph::displayStrongestConnections(string username) {
    if (users.find(username) == users.end()) {
        cout << "User " << username << " not found." << endl;
        return;
    }
    
    User* user = users[username];
    cout << "\nStrongest connections for " << username << ":\n";
    
    if (user->following.empty()) {
        cout << "Not following anyone yet.\n";
        return;
    }
    
    vector<Connection> sortedFollowing = user->following;
    sort(sortedFollowing.begin(), sortedFollowing.end(),
        [](const Connection& a, const Connection& b) {
            return a.strength > b.strength;
        });
    
    for (const auto& connection : sortedFollowing) {
        cout << " " << connection.targetUser << " (strength: " << connection.strength << "/10)\n";
    }
}

// Display connection strength between two users
void InstagramGraph::displayConnectionStrength(string user1, string user2) {
    if (users.find(user1) == users.end() || users.find(user2) == users.end()) {
        cout << "One or both users don't exist." << endl;
        return;
    }
    
    int strength = 0;
    for (const auto& connection : users[user1]->following) {
        if (connection.targetUser == user2) {
            strength = connection.strength;
            break;
        }
    }
    
    if (strength > 0) {
        cout << "Connection strength between " << user1 << " and " << user2 
             << " is " << strength << "/10.\n";
    } else {
        cout << user1 << " is not following " << user2 << ".\n";
    }
}

int main() {
    InstagramGraph instagram;
    int choice;
    string username, name, content, user1, user2;
    int strength;
    
    cout << "=====================================\n";
    cout << "   Instagram Network Simulator   \n";
    cout << "=====================================\n";
    
    do {
        cout << "\nMenu:\n";
        cout << "1. Add User\n";
        cout << "2. Delete User\n";
        cout << "3. Add Post\n";
        cout << "4. Follow User\n";
        cout << "5. Display User Profile\n";
        cout << "6. Display All Users\n";
        cout << "7. BFS Traversal\n";
        cout << "8. DFS Traversal\n";
        cout << "9. Suggest Friends\n";
        cout << "10. Display Strongest Connections\n";
        cout << "11. Display Connection Strength\n";
        cout << "12. Search User\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                cout << "Enter username: ";
                cin >> username;
                cout << "Enter name: ";
                cin.ignore();
                getline(cin, name);
                instagram.addUser(username, name);
                break;
            case 2:
                cout << "Enter username to delete: ";
                cin >> username;
                instagram.deleteUser(username);
                break;
            case 3:
                cout << "Enter username: ";
                cin >> username;
                cout << "Enter post content: ";
                cin.ignore();
                getline(cin, content);
                instagram.addPost(username, content);
                break;
            case 4:
                cout << "Enter follower username: ";
                cin >> user1;
                cout << "Enter username to follow: ";
                cin >> user2;
                cout << "Enter connection strength (1-10, default 5): ";
                if (!(cin >> strength)) {
                    cin.clear();
                    strength = 5;
                }
                instagram.followUser(user1, user2, strength);
                break;
            case 5:
                cout << "Enter username: ";
                cin >> username;
                instagram.displayUserProfile(username);
                break;
            case 6:
                instagram.displayAllUsers();
                break;
            case 7:
                cout << "Enter starting username: ";
                cin >> username;
                instagram.BFSTraversal(username);
                break;
            case 8:
                cout << "Enter starting username: ";
                cin >> username;
                instagram.DFSTraversal(username);
                break;
            case 9:
                cout << "Enter username: ";
                cin >> username;
                instagram.suggestFriends(username);
                break;
            case 10:
                cout << "Enter username: ";
                cin >> username;
                instagram.displayStrongestConnections(username);
                break;
            case 11:
                cout << "Enter first username: ";
                cin >> user1;
                cout << "Enter second username: ";
                cin >> user2;
                instagram.displayConnectionStrength(user1, user2);
                break;
            case 12:
                cout << "Enter username to search: ";
                cin >> username;
                if (instagram.searchUser(username)) {
                    cout << "User " << username << " exists in the network.\n";
                } else {
                    cout << "User " << username << " does not exist in the network.\n";
                }
                break;
            case 0:
                cout << "Exiting program...\n";
                break;
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != 0);
    
    return 0;
}
