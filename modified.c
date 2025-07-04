// Problems:
// same user register honge toh message broadcast hoga
// aur same user register na krne de toh hash chaining ka koi mtlb nahi 
// kyunki username se hash value determine ho rahi h.(so need a unique value for hashing(maybe email/mobile))
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
    #include<conio.h>
#else
    #include <unistd.h>
    #include<termios.h>
#endif


#ifdef _WIN32
    void getPassword(char* password, int maxLen) {
        int i = 0;
        char ch;
        while ((ch = _getch()) != '\r' && i < maxLen - 1) {
            if ((ch == '\b' || ch == 127) && i > 0) {
                i--;
                printf("\b \b");
            } else if (ch != '\b' && ch != 127) {
                password[i++] = ch;
                printf("*");
            }
        }
        password[i] = '\0';
        printf("\n");
    }
#else
    void getPassword(char* password, int maxLen) {
        struct termios old_props, new_props;
        tcgetattr(STDIN_FILENO, &old_props);
        new_props = old_props;
        new_props.c_lflag &= ~(ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_props);

        int i = 0;
        char ch;
        while ((ch = getchar()) != '\n' && ch != EOF && i < maxLen - 1) {
            if ((ch == '\b' || ch == 127) && i > 0) {
                i--;
                printf("\b \b");
            } else if (ch != '\b' && ch != 127) {
                password[i++] = ch;
                printf("*");
            }
            fflush(stdout);
        }
        password[i] = '\0';
        tcsetattr(STDIN_FILENO, TCSANOW, &old_props);
        printf("\n");
    }
#endif


#define MAX_USERS 100
#define HASH_SIZE 101

typedef struct User {
    char username[50];
    char password[50];
    struct User* next;
} User;

typedef struct Friend {
    char username[50];
    struct Friend* next;
} Friend;

typedef struct Message {
    char sender[50];
    char message[256];
    struct Message* next;
} Message;

typedef struct UserNode {
    char username[50];
    Friend* friends;
    Message* messages;
    struct UserNode* next;
} UserNode;

User* hashTable[HASH_SIZE];
UserNode* userGraph[MAX_USERS];
int userCount = 0;

unsigned int hash(char* str);
int registerUser(char* username, char* password);
int loginUser(char* username, char* password);
int addFriend(char* user, char* friend);
void removeFromFriendList(UserNode* user, char* friendName);
int removeFriend(char* user, char* friend);
void sendMessage(char* sender, char* receiver, char* message);
void displayFriends(char* username);
void displayMessages(char* username);
void xorCipher(char* message, char* key);
void clearScreen();

unsigned int hash(char* str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash << 5) + *str++;
    }
    return hash % HASH_SIZE;
}

void clearScreen() {
    #ifdef _WIN32
        Sleep(2000);
        system("cls");
    #else
        sleep(2);
        system("clear");
    #endif
}

// void hashPassword(char* input, char* output) {
//     char key[] = "ZingleKey";
//     int len = strlen(input);
//     for (int i = 0; i < len; i++) {
//         output[i] = input[i] ^ key[i % strlen(key)];
//     }
//     output[len] = '\0';
// }

int registerUser(char* username, char* password) {
    if (userCount == MAX_USERS) {
    printf("Error: Maximum number of users reached (%d).\n", MAX_USERS);
    clearScreen();
    return 0;
    }
    unsigned int index = hash(username);
    User* newUser = (User*)malloc(sizeof(User));
    strcpy(newUser->username, username);
    // char hashedPass[50];
    // hashPassword(password, hashedPass);
    // strcpy(newUser->password, hashedPass);
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
    // char hashedInput[50];
    // hashPassword(password, hashedInput);
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

int addFriend(char* user, char* friend) {
    UserNode* userNode = NULL;
    UserNode* friendNode = NULL;

    for (int i = 0; i < userCount; i++) {
        if (strcmp(userGraph[i]->username, user) == 0) {
            userNode = userGraph[i];
        }
        if (strcmp(userGraph[i]->username, friend) == 0) {
            friendNode = userGraph[i];
        }
    }

    if (!friendNode) {
    printf("Error: User '%s' does not exist.\n", friend);
    clearScreen();
    return 0;
    }

    Friend* temp = userNode->friends;
    while (temp) {
        if (strcmp(temp->username, friend) == 0) {
            printf("You are already friends with %s\n",friend);
            clearScreen();
            return 0;
        }
        temp = temp->next;
    }

    Friend* newFriend1 = (Friend*)malloc(sizeof(Friend));
    strcpy(newFriend1->username, friend);
    newFriend1->next = userNode->friends;
    userNode->friends = newFriend1;

    Friend* newFriend2 = (Friend*)malloc(sizeof(Friend));
    strcpy(newFriend2->username, user);
    newFriend2->next = friendNode->friends;
    friendNode->friends = newFriend2;

    printf("%s and %s are now friends!\n", user, friend);
    clearScreen();
    return 1;
}

void removeFromFriendList(UserNode* userNode, char* friendName) {
    Friend* current = userNode->friends;
    Friend* previous = NULL;
    while (current) {
        if (strcmp(current->username, friendName) == 0) {
            if (previous) {
                previous->next = current->next;
            } else {
                userNode->friends = current->next;
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

    for (int i = 0; i < userCount; i++) {
        if (strcmp(userGraph[i]->username, user) == 0) {
            userNode = userGraph[i];
        }
        if (strcmp(userGraph[i]->username, friend) == 0) {
            friendNode = userGraph[i];
        }
    }

    if (!friendNode) {
    printf("Error: User '%s' does not exist.\n", friend);
    clearScreen();
    return 0;
    }

    Friend* temp = userNode->friends;
    int found = 0;
    while (temp) {
        if (strcmp(temp->username, friend) == 0) {
            found = 1;
            break;
        }
        temp = temp->next;
    }

    if (!found) {
        printf("Error: '%s' is not in your friend list.\n", friend);
        clearScreen();
        return 0;
    }

    removeFromFriendList(userNode, friend);
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

void xorCipher(char* message, char* key) {
    int keyLen = strlen(key);
    for (int i = 0; i < strlen(message); i++) {
        message[i] ^= key[i % keyLen];
    }
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
    for (int i = 0; i < HASH_SIZE; i++) {
        hashTable[i] = NULL;
    }
    for (int i = 0; i < MAX_USERS; i++) {
        userGraph[i] = NULL;
    }
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
                scanf(" %[^\n]%*c", username);
                printf("Enter password: ");
                getPassword(password, 50);
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
                                scanf(" %[^\n]%*c", friend);
                                addFriend(username, friend);
                                break;
                            case 2:
                                printf("Enter friend's username to remove: ");
                                scanf(" %[^\n]%*c", friend);
                                if (removeFriend(username, friend)) {
                                    printf("Friend removed successfully!\n");
                                } else {
                                    printf("Friend not found!\n");
                                }
                                break;
                            case 3:
                                printf("Enter receiver's username: ");
                                scanf(" %[^\n]%*c", friend);
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
                scanf(" %[^\n]%*c", username);
                printf("Enter password: ");
                getPassword(password, 50);
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