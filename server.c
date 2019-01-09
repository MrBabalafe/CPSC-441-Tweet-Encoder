#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define PORT 24536
#define MAX_MESSAGE_LENGTH 100

struct data {
	char value[MAX_MESSAGE_LENGTH];
	char key[7];
};
struct data words[MAX_MESSAGE_LENGTH];

void sequence_hash(char*, int, int*);
void word_sum(char*, int, int*);
void your_hash(char*, int, int*);
void print_menu();

int socket2;

int main() {

	struct sockaddr_in address;
	char snd_message[MAX_MESSAGE_LENGTH*3];
	char rcv_message [MAX_MESSAGE_LENGTH];
	int menu_choice;
	int valid_choice = 0;
	
	//Main Menu; gets user to pick desired hashing method.
	while(!valid_choice) {
		print_menu();
		scanf("%d", &menu_choice);
		if(menu_choice == 1) {
			printf("Sequential Index selected!\n");
			valid_choice = 1;
		} else if(menu_choice == 2) {
			printf("Word Sum selected!\n");
			valid_choice = 1;
		} else if (menu_choice == 3) {
			printf("Your Hash selected!\n");
			valid_choice = 1;
		} else {
			printf("Invalid menu selection. Try again\n");
		}
	}

	//ADDRESS INITIALIZATION
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	//SOCKET CREATION
	int socket1;
	socket1 = socket(AF_INET, SOCK_STREAM, 0);
	if(socket1 < 0) {
		printf("socket creation failed!\n");
	} else {
		printf("Server Socket created!\n");
	}

	//BIND
	int bind_status;
	bind_status = bind(socket1, (struct sockaddr*) &address, sizeof(struct sockaddr_in));
	if(bind_status < 0) {
		printf("bind() failed!\n");
		exit(-1);
	} else {
		printf("Bind successful!\n");
	}

	//LISTEN
	int listen_status;
	listen_status = listen(socket1, 5);
	if(listen_status < 0) {
		printf("listen() failed!\n");
		exit(-1);
	} else {
		printf("Now Listening!\n");
	}
	//Fill send and receive messages with 0s just to be safe
	bzero(snd_message, MAX_MESSAGE_LENGTH*3);
	bzero(rcv_message, MAX_MESSAGE_LENGTH);
	int key = 1;

	while(1) {
		socket2 = accept(socket1, NULL, NULL);
		if(socket2 < 0) {
			printf("accept() failed\n");
			exit(-1);
		}

		while(recv(socket2, rcv_message, MAX_MESSAGE_LENGTH, 0) > 0) {
			printf("Received word: %s\n", rcv_message);
			strcpy(snd_message, rcv_message);
			//printf("key before sequence: %d\n", key);
			if(menu_choice == 1) {
				sequence_hash(snd_message, strlen(snd_message), &key);
			} else if(menu_choice == 2) {
				word_sum(snd_message, strlen(snd_message), &key);
			} else if(menu_choice == 3) {
				your_hash(snd_message, strlen(snd_message), &key);
			}
			
			//printf("key after sequence: %d\n", key);
			send(socket2, snd_message, strlen(snd_message), 0);
			printf("Sent word: %s\n", snd_message);

			//Set send and receive back to 0 just to be safe
			bzero(snd_message, MAX_MESSAGE_LENGTH*3);
			bzero(rcv_message, MAX_MESSAGE_LENGTH);
			
		}
		close(socket2);
	}
	close(socket1);
}

//Function for the Sequential Index hashing method
void sequence_hash(char* message, int length, int* key) {
	//Copy the message sent to a temporary string and set up other neccesary variables
	char temp[length];
	strcpy(temp, message);
	char* token = temp;
	int temp_key = *key;
	bzero(message, MAX_MESSAGE_LENGTH*3);

	//Create variables used to determine if user wants to decode
	
	int decode = 0;
	char first_two_chars[2];
	bzero(first_two_chars, 2);
	
	while((token = strtok(token, " ")) != NULL) {

		strncpy(first_two_chars, token, 2);
		if(strcmp(first_two_chars, "0x") == 0) {
			//printf("Start Decoding\n");
			for(int i =0; i < temp_key-1; i++) {
				if(strcmp(words[i].key, token) == 0) {
					strcat(message, words[i].value);
					strcat(message, " ");
				}
			}

		} else {
			//Adds each word in the sentence to the data struct, with an incremental key
			int same = 0;
			for(int i=0; i < temp_key-1; i++) {
				//If current token is already in the struct, just the encoded value to the final string
				if(strcmp(words[i].value, token) == 0) {
					same = 1;
					//Append the encoded value to the final string
					strcat(message, words[i].key);
					strcat(message, " ");
				} 
			}
			//If current token is not already in words struct, add it to the struct
			if(same == 0) {
				strcpy(words[temp_key-1].value, token);
				snprintf(words[temp_key-1].key, 7, "0x%04x", temp_key);
				
				//Append the encoded value to the final string
				strcat(message, words[temp_key-1].key);
				strcat(message, " ");
				temp_key++;
				*key += 1;
			} else {
				printf("%s was written already\n", token);
			}
		}
		token = NULL;
	}
}

//Fumction for the Word Sum hashing method
void word_sum(char* message, int length, int* key) {
	char temp[length];
	strcpy(temp, message);
	char* token = temp;
	int temp_key = *key;
	bzero(message, MAX_MESSAGE_LENGTH*3);

	//Create variables used to determine if user wants to decode
	int decode = 0;
	char first_two_chars[2];
	bzero(first_two_chars, 2);
	
	while((token = strtok(token, " ")) != NULL) {

		strncpy(first_two_chars, token, 2);
		if(strcmp(first_two_chars, "0x") == 0) {
			//printf("Start Decoding\n");
			for(int i =0; i < temp_key-1; i++) {
				if(strcmp(words[i].key, token) == 0) {
					strcat(message, words[i].value);
					strcat(message, " ");
				}
			}

		} else {
			//Adds each word in the sentence to the data struct, with a key based on adding all ASCII characters together
			int same = 0;
			for(int i = 0; i < temp_key-1; i++) {
				if(strcmp(words[i].value, token) == 0) {
					same = 1;

					//Appends the encoded value to the final string
					strcat(message, words[i].key);
					strcat(message, " ");
				}
			}
			//If the current token isn't in the struct, add it and then append the encoded value to the final string
			if(same == 0) {
				strcpy(words[temp_key-1].value, token);
				int ascii_sum = 0;

				//Add up all the ascii characters to get the encoded value
				for(int i = 0; i < strlen(token); i++) {
					ascii_sum += token[i];
				}
				snprintf(words[temp_key-1].key, 7, "0x%04x", ascii_sum);
				
				//Append the encoded value to the final string
				strcat(message, words[temp_key-1].key);
				strcat(message, " ");
				temp_key++;
				*key+= 1;

			} else {
				printf("%s was written already\n", token);
			}
		}
		token = NULL;
	}
}

//Function for the Your Sum hashing method
void your_hash(char* message, int length, int* key) {
	char temp[length];
	strcpy(temp, message);
	char* token = temp;
	int temp_key = *key;
	bzero(message, MAX_MESSAGE_LENGTH*3);

	//Create variables used to determine if user wants to decode
	int decode = 0;
	char first_two_chars[2];
	bzero(first_two_chars, 2);

	while((token = strtok(token, " ")) != NULL) {

		strncpy(first_two_chars, token, 2);
		if(strcmp(first_two_chars, "0x") == 0) {
			//printf("Start Decoding\n");
			for(int i =0; i < temp_key-1; i++) {
				if(strcmp(words[i].key, token) == 0) {
					strcat(message, words[i].value);
					strcat(message, " ");
				}
			}
		} else {
			//Adds each word in the sentence to the data struct, with a key based on adding all ASCII characters together
			int same = 0;
			for(int i = 0; i < temp_key-1; i++) {
				if(strcmp(words[i].value, token) == 0) {
					same = 1;

					//Appends the encoded value to the final string
					strcat(message, words[i].key);
					strcat(message, " ");
				}
			}
			//If the current token isn't in the struct, add it and then append the encoded value to the final string
			if(same == 0) {
				strcpy(words[temp_key-1].value, token);
				int ascii_sum = 0;

				//Add up all the ascii characters and multiply by i to get the encoded value
				for(int i = 0; i < strlen(token); i++) {
					ascii_sum += (token[i] * (i+1));
				}
				snprintf(words[temp_key-1].key, 7, "0x%04x", ascii_sum);
				
				//Append the encoded value to the final string
				strcat(message, words[temp_key-1].key);
				strcat(message, " ");
				temp_key++;
				*key+= 1;

			} else {
				printf("%s was written already\n", token);
			}
		}
		token = NULL;
	}
}

void print_menu() {
	printf("\n");
	printf("Choose a hashing method:\n");
	printf("1. Sequential Index\n");
	printf("2. Word Sum\n");
	printf("3. Your Hash\n");
	printf("Choice: ");
}