//problems so far:
// my friend appears on my friend list but, i am not on his/hers.(ek friend list ka function chahiye request accept k liye)(future scope)
//display messages shouldn't quit on it's own. (done)
//Add friend k baad k pop up message chahiye, friend added wala. (done)
// Add friend me check hona chahiye if friend exists.(done)
//same record se new registeration me bhi same, registered succesfully dikha raha h, but data vahi h. 
// isme registeration k liye copy generate na ho ye dekhna h
// message same name k users ko broadcast ho rahe h
//apne aap ko message kr pa rahe h.
//same user password store ho rahe h baar baar.
//user k naam me agar space h, toh password enter nahi ho raha
//jo log h abhi nahi, vo bhi friends bn jaa rahe h.(done)
//adjacency list is for usernode(social graph)
//addfriend() should have bidirectional edge so that if i add someone as friend i am on their friend list as well(done)
// removeFriend() should be bidrectional as well(done)
//addfriend() should be updated to check if a user exists.(done)
//show history of sent messages.
// remove friend k baad message. (done)


// future roles:
// debugging
// BSF/dfs
// sound
// password encryption
// file handling

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#define MAX_USERS 100
#define HASH_SIZE 101

unsigned int hash(char* str);
int registerUser(char* username, char* password);
int loginUser(char* username, char* password);
int addFriend(char* user, char* friend);
void removeFromFriendList(UserNode* user, char* friendName);
int removeFriend(char* user, char* friend);
void sendMessage(char* sender, char* receiver, char* message);
void displayFriends(char* username);
void xorCipher(char* message, char* key);
void displayMessages(char* username);
void clearScreen();

typedef struct User {
    char username[50];
    char password[50];
    struct User* next;//(hash adjacency list)if both user have same index they are chained as a linked list in the same bucket
} User;

typedef struct Friend {
    char username[50];
    struct Friend* next;//next friend in the friend list
} Friend;

typedef struct Message {
    char sender[50];
    char message[256];
    struct Message* next;//next msg in the inbox
} Message;

typedef struct UserNode {
    char username[50];
    Friend* friends;
    Message* messages;
    struct UserNode* next;//chaained graph's adjacency list//or for dfs/bfs
} UserNode;

User* hashTable[HASH_SIZE];
UserNode* userGraph[MAX_USERS];
int userCount = 0;

unsigned int hash(char* str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash << 5) + *str++;
    }
    return hash % HASH_SIZE;
}

void clearScreen() {
    #ifdef _WIN32
        Sleep(3000);
        system("cls");
    #else
        sleep(3);
        system("clear");
    #endif
}

int registerUser(char* username, char* password) {
    unsigned int index = hash(username);
    User* newUser = (User*)malloc(sizeof(User));
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    newUser->next = hashTable[index];
    hashTable[index] = newUser;

    UserNode* newNode = (UserNode*)malloc(sizeof(UserNode));
    strcpy(newNode->username, username);
    newNode->friends = NULL;
    newNode->messages = NULL;
    newNode->next = userGraph[userCount];
    userGraph[userCount++] = newNode;
    printf("User registered successfully!\n");
    clearScreen();
    return 1;
}

int loginUser(char* username, char* password) {
    unsigned int index = hash(username);
    User* currentUser = hashTable[index];
    while (currentUser) {
        if (strcmp(currentUser->username, username) == 0 &&
            strcmp(currentUser->password, password) == 0) {
            clearScreen();
            return 1;
        }
        currentUser = currentUser->next;
    }
    return 0;
}

// int addFriend(char* user, char* friend) {
//     for (int i = 0; i < userCount; i++) {
//         if (strcmp(userGraph[i]->username, user) == 0) {
//             Friend* newFriend = (Friend*)malloc(sizeof(Friend));
//             strcpy(newFriend->username, friend);
//             newFriend->next = userGraph[i]->friends;
//             userGraph[i]->friends = newFriend;
//             printf("%s added as a friend.",friend);
//             clearScreen();
//             return 1;
//         }
//     }
//     return 0;
// }

int addFriend(char* user, char* friend) {
    UserNode* userNode = NULL;
    UserNode* friendNode = NULL;

    // Find both the user and friend in the graph
    for (int i = 0; i < userCount; i++) {
        if (strcmp(userGraph[i]->username, user) == 0) {
            userNode = userGraph[i];
        }
        if (strcmp(userGraph[i]->username, friend) == 0) {
            friendNode = userGraph[i];
        }
    }

    // If either user or friend is not found, return failure
    if (!userNode || !friendNode) {
        printf("Error: One or both users not found.\n");
        return 0;
    }

    // Check if friendship already exists
    Friend* temp = userNode->friends;
    while (temp) {
        if (strcmp(temp->username, friend) == 0) {
            printf("Friendship already exists!\n");
            return 0;
        }
        temp = temp->next;
    }

    // Add friend to user's list
    Friend* newFriend1 = (Friend*)malloc(sizeof(Friend));
    strcpy(newFriend1->username, friend);
    newFriend1->next = userNode->friends;
    userNode->friends = newFriend1;

    // Add user to friend's list (making friendship bidirectional)
    Friend* newFriend2 = (Friend*)malloc(sizeof(Friend));
    strcpy(newFriend2->username, user);
    newFriend2->next = friendNode->friends;
    friendNode->friends = newFriend2;

    printf("%s and %s are now friends!\n", user, friend);
    clearScreen();
    return 1;
}


// int removeFriend(char* user, char* friend) {
//     for (int i = 0; i < userCount; i++) {
//         if (strcmp(userGraph[i]->username, user) == 0) {
//             Friend* current = userGraph[i]->friends;
//             Friend* previous = NULL;
//             while (current) {
//                 if (strcmp(current->username, friend) == 0) {
//                     if (previous) {
//                         previous->next = current->next;
//                     } else {
//                         userGraph[i]->friends = current->next;
//                     }
//                     free(current);
//                     clearScreen();
//                     return 1;
//                 }
//                 previous = current;
//                 current = current->next;
//             }
//         }
//     }
//     return 0;
// }

// Function to remove a friend from a user's friend list
void removeFromFriendList(UserNode* user, char* friendName){
    Friend* current = user->friends;
    Friend* previous = NULL;
    while (current) {
        if (strcmp(current->username, friendName) == 0) {
            if (previous) {
                previous->next = current->next;
            } else {
                user->friends = current->next;
            }
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

int removeFriend(char* user, char* friend) {
    UserNode* userNode = NULL;
    UserNode* friendNode = NULL;

    // Find both user and friend in userGraph
    for (int i = 0; i < userCount; i++) {
        if (strcmp(userGraph[i]->username, user) == 0) {
            userNode = userGraph[i];
        }
        if (strcmp(userGraph[i]->username, friend) == 0) {
            friendNode = userGraph[i];
        }
    }

    // If either user or friend is not found, return failure
    if (!userNode || !friendNode) {
        printf("Error: One or both users not found.\n");
        return 0;
    }


    // Remove `friend` from `user`'s friend list
    removeFromFriendList(userNode, friend);

    // Remove `user` from `friend`'s friend list (ensuring bidirectional removal)
    removeFromFriendList(friendNode, user);

    printf("%s and %s are no longer friends.\n", user, friend);
    clearScreen();
    return 1;
}



void sendMessage(char* sender, char* receiver, char* message) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(userGraph[i]->username, receiver) == 0) {
            Message* newMessage = (Message*)malloc(sizeof(Message));
            strcpy(newMessage->sender, sender);
            strcpy(newMessage->message, message);
            newMessage->next = NULL;

            if (userGraph[i]->messages == NULL) {
                userGraph[i]->messages = newMessage;
            } else {
                Message* temp = userGraph[i]->messages;
                while (temp->next) {
                    temp = temp->next;
                }
                temp->next = newMessage;
            }
            printf("Message sent to %s\n", receiver);
            clearScreen();
            return;
        }
    }
    printf("Receiver not found!\n");
    clearScreen();
}

void displayFriends(char* username) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(userGraph[i]->username, username) == 0) {
            Friend* currentFriend = userGraph[i]->friends;
            printf("Friends of %s:\n", username);
            while (currentFriend) {
                printf("%s\n", currentFriend->username);
                currentFriend = currentFriend->next;
            }
            clearScreen();
            return;
        }
    }
    printf("User not found!\n");
    clearScreen();
}

void xorCipher(char* message, char* key) {
    int keyLen = strlen(key);
    for (int i = 0; i < strlen(message); i++) {
        message[i] ^= key[i % keyLen];
    }
}

void displayMessages(char* username) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(userGraph[i]->username, username) == 0) {
            Message* currentMessage = userGraph[i]->messages;
            printf("Messages for %s:\n", username);
            while (currentMessage) {
                char decryptedMessage[256];
                strcpy(decryptedMessage, currentMessage->message);
                xorCipher(decryptedMessage, currentMessage->sender);
                printf("From %s: %s\n", currentMessage->sender, decryptedMessage);
                currentMessage = currentMessage->next;
            }
            int ch=0;
            do{
            printf("Enter 1 to return to home.\n");
            scanf("%d",&ch);
            }while(ch!=1);
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
            return;
        }
    }
    printf("User not found!\n");
    clearScreen();
}


void loadingBar() {
    printf("Loading...");
    for (int i = 0; i < 10; i++) {
        fflush(stdout);
        #ifdef _WIN32
            Sleep(100);
        #else
            sleep(1);
        #endif
        printf(".");
    }
    printf("\n");
    clearScreen();
}

int main() {
    int choice;
    char username[50], password[50], friend[50], message[256];
    while (1) {
        printf("\033[1;31m");
        printf("==================================\n");
        printf("|          \\     /               |\n");
        printf("|           \\ _ /                |\n");
        printf("|         --('v')--              |\n");
        printf("|          ((   ))               |\n");
        printf("|          --\"-\"--               |\n");
        printf("|   Welcome to Zingle-Zingle     |\n");
        printf("==================================\n");
        printf("\033[0m");
        loadingBar();
        printf("1. Login\n2. New User Registration\n3. Exit\n\n");
        printf("Your Choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                printf("Enter username: ");
                scanf("%s", username);
                printf("Enter password: ");
                scanf("%s", password);
                if (loginUser(username, password)) {
                    printf("Login successful!\n");
                    int userChoice;
                    while (1) {
                        printf("1. Add Friend\n2. Remove Friend\n3. Message\n4. Display Friends\n5. Display Messages\n6. Logout\n");
                        scanf("%d", &userChoice);
                        if (userChoice == 6) {
                            clearScreen();
                            break;
                        }
                        switch (userChoice) {
                            case 1:
                                printf("Enter friend's username: ");
                                scanf("%s", friend);
                                addFriend(username, friend);
                                break;
                            case 2:
                                printf("Enter friend's username to remove: ");
                                scanf("%s", friend);
                                if (removeFriend(username, friend)) {
                                    printf("Friend removed successfully!\n");
                                } else {
                                    printf("Friend not found!\n");
                                }
                                break;
                            case 3:
                                printf("Enter receiver's username: ");
                                scanf("%s", friend);
                                printf("Enter message: ");
                                scanf(" %[^\n]%*c", message);
                                xorCipher(message, username);
                                sendMessage(username, friend, message);
                                break;
                            case 4:
                                displayFriends(username);
                                break;
                            case 5:
                                displayMessages(username);
                                break;
                            default:
                                printf("Invalid choice!\n");
                                clearScreen();
                        }
                    }
                } else {
                    printf("Invalid username or password!\n");
                    clearScreen();
                }
                break;
            case 2:
                printf("Enter username: ");
                scanf("%s", username);
                printf("Enter password: ");
                scanf("%s", password);
                registerUser(username, password);
                break;
            case 3:
                exit(0);
            default:
                printf("Invalid choice!\n");
                clearScreen();
        }
    }
    return 0;
}