

/********************************************
 *  library  
 *******************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#define MAX_NAME_LEN 15
#define MAX_PASS_LEN 15
#define MAX_USERS 10
#define MAX_MESSAGES_LOGIN 4
#define MAX_MESSAGES 20
#define MAX_MSG_LEN 15

typedef struct 
{
	int match;
	char to[MAX_NAME_LEN];
	char from[MAX_NAME_LEN];
	char msg[MAX_MSG_LEN];
	char is_draft;
} Message;


typedef struct 
{
	char name[MAX_NAME_LEN];
	char password[MAX_PASS_LEN];
	int index; // index into the listOfMessages
	int first_login;
	int msg_count;
} User;

User *current_user;
User *listOfUsers[MAX_USERS];
int user_count;
int msg_count_login;


size_t
receive_until( char *dst, char delim, size_t max) {
    size_t len = 0 ;
    size_t rx  = 0 ;
    char c = 0 ;
    while ( len < max ) {
        dst[len] = 0x00 ;
        if ( (rx = read(0, &c, 1)) == 0 ) {
            len = 0 ;
            break ;
        }
        if ( c == delim ) {
            break ;
        }
        dst[len] = c ;
        len ++ ;
    }
    return len ;
}


int login(char *name, char *passwd);
int logout() ;

void create_user(int tries);
void list_users();
int get_user__index(char *name);

/***************************************
 *   message.c 
 **************************************/

/*

Author: James Nuttall (james%cromulence.co)

Copyright (c) 2015 Cromulence LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/



// dirty grouping of global variables
typedef struct
{
	Message listOfMessages[MAX_USERS][MAX_MESSAGES];
	void (*printer)();
} GLOBALS;

GLOBALS globs;

void print_message(int user_offset, int msg_offset)
{
	if (!strcmp(globs.listOfMessages[user_offset][msg_offset].to, ""))
		return;
	printf("******************\n");
	printf("To: %s\nFrom: %s \nMsg: %s\n", globs.listOfMessages[user_offset][msg_offset].to, globs.listOfMessages[user_offset][msg_offset].from, globs.listOfMessages[user_offset][msg_offset].msg);		
}

// if draft == 1, this is a draft message
int add_message(char *dest, char *src, char *msg, char draft)
{
	globs.printer = print_message; // easy init

	char found = -1; // this will have the user's index if it is found

	// does the dest exist?
	for (int j = 0; j < user_count; j++)
	{
		if (!strcmp(dest, listOfUsers[j]->name))
		{
			found = j;
			break;
		}
	}
	if (found == -1)
	{
		printf("User does not exist.\n");
		return 0;
	}

	int target = -1;
	int index = -1;
	if (draft == 1)
		index = get_user__index(src);
	else
		index = get_user__index(dest);


	if (current_user->first_login == 1)
	{
		current_user->first_login = 0;

		// setting the target to the current msg_count allows a write past the end of the MAX_MESSAGES for this user
		// if this is the last user, it allows an overwrite of the function pointer 'printer'
		// ping #ifndef PATCHED
		if (current_user->msg_count >= MAX_MESSAGES)
		{
			target = current_user->msg_count; // a naive person might assume this is zero...that's the problem
			goto label;
		}
		// pong #endif
	}
	
	// add messages into blank spots
	#ifdef PATCHED
	for (int i = 0; i < MAX_MESSAGES; i++) 
	#else
	for (int i = 0; i < MAX_MESSAGES+1; i++) 
	#endif
	{
		if (globs.listOfMessages[index][i].match != 0xbeef)
		{
			// this message is not valid, replace it
			target = i;
			goto label;
		}
	}
label:
	{
		// without this check, users can write past the end of of their message queue
		// very simple buffer overrun
		if (target == -1)
		{
			//printf("target is neg 1 name: %s %d", current_user->name, current_user->msg_count);
			return 0;
		}

		Message *cur = &(globs.listOfMessages[index][target]);

		cur->match = 0xbeef;
		strcpy(cur->from, src);
		strcpy(cur->to, dest);
		strcpy(cur->msg, msg);
		cur->is_draft = draft;

		msg_count_login++;
		current_user->msg_count++;
		return 1;
	}
	return 0;
}

void list_drafts(char *name)
{
	int index = get_user__index(name);
	int found = 0;
	for (int i = 0; i < MAX_MESSAGES; i++)
	{
		if (globs.listOfMessages[index][i].match != 0xbeef) 
		{
			continue;
		}
		if (!strcmp(name, globs.listOfMessages[index][i].from) && (globs.listOfMessages[index][i].is_draft == 1))
		{
			found = 1;
			globs.printer(index, i);
		}
	}
	if (found == 0)
		printf("No drafts for this user.\n");
}

void list_inbox(char *name)
{
	// print all messages to the current user
	int index = get_user__index(name);
	int found = 0;
	for (int i = 0; i < MAX_MESSAGES; i++)
	{
		if (globs.listOfMessages[index][i].match != 0xbeef) 
		{
			continue;
		}
		if (!strcmp(name, globs.listOfMessages[index][i].to) && (globs.listOfMessages[index][i].is_draft == 0))
		{
			found = 1;
			globs.printer(index, i);
		}
	}
	if (found == 0)
		printf("No messages for this user.\n");
}

void list_all_messages()
{
	// print all messages
	int found = 0;
	for (int j = 0; j < user_count; j++)
	{
		for (int i = 0; i < MAX_MESSAGES; i++)
		{
			found = 1;
			globs.printer(j, i);
		}
	}
	if (found == 0)
		printf("No messages.\n");
}

int get_draft_count(char *name)
{
	int index = get_user__index(name);
	int count = 0;
	for (int i = 0; i < MAX_MESSAGES; i++)
	{
		if ((!strcmp(name, globs.listOfMessages[index][i].from)) && (globs.listOfMessages[index][i].is_draft == 1))
		{
			count += 1;
		}
	}
	return count;
}

int get_inbox_count(char *name)
{
	int count = 0;
	// print all messages to the current user
	int index = get_user__index(name);
	for (int i = 0; i < MAX_MESSAGES; i++)
	{
		if ((!strcmp(name, globs.listOfMessages[index][i].to)) && (globs.listOfMessages[index][i].is_draft == 0))
		{
			count++;
		}
	}
	return count;
}

int get_total_count()
{
	int count = 0;
	for (int j = 0; j < user_count; j++)
	{
		for (int i = 0; i < listOfUsers[j]->msg_count; i++)
		{
			count++;
		}
	}
	return count;
}

void print_draft_for_send(char *name)
{
	int my_index = get_user__index(name);
	int i = 0;
	for (i = 0; i < MAX_MESSAGES; i++)
	{
		if (globs.listOfMessages[my_index][i].match == 0xbeef)
		{
			// list all of this user's drafts
			if (!strcmp(name, globs.listOfMessages[my_index][i].from) && (globs.listOfMessages[my_index][i].is_draft == 1))
			{
				printf("%d: To: %s Msg: %s\n", i, globs.listOfMessages[my_index][i].to, globs.listOfMessages[my_index][i].msg);
			}
		}
	}

	char buf[4] = {0};
	size_t l = 0;
	while (l < 1)
		l = receive_until( buf, '\n', 3);

	int choice = atoi(&buf[0]);
	if (choice >= MAX_MESSAGES || choice < 0)
	{
		printf("Out of range.\n");
		return;
	}

	if (strcmp(name, globs.listOfMessages[my_index][choice].from) || (globs.listOfMessages[my_index][choice].is_draft != 1))
	{
		printf("Not a valid selection.\n");
		return;
	}

	// pointer to the current draft message
	Message *cur = &(globs.listOfMessages[my_index][choice]);

	// get the dest user's index
	int dst_index = get_user__index(globs.listOfMessages[my_index][choice].to);

	int target = -1;
	for (int i = 0; i < MAX_MESSAGES; i++)
	{
		if (globs.listOfMessages[dst_index][i].match != 0xbeef)
		{
			// this message is not valid, replace it
			target = i;
			break;
		}
	}
	if (target == -1)
	{
		printf("No space left for this user.\n");
		return;
	}
	// pointer to destination message 
	Message *dst_msg = &(globs.listOfMessages[dst_index][target]);

	strcpy(dst_msg->from, cur->from);
	strcpy(dst_msg->to, cur->to);
	strcpy(dst_msg->msg, cur->msg);
	dst_msg->is_draft = 0;
	dst_msg->match = 0xbeef;

	// remove from sender's array
	bzero(cur->to, MAX_NAME_LEN);
	bzero(cur->from, MAX_NAME_LEN);
	bzero(cur->msg, MAX_MSG_LEN);
	cur->match = 0;
}

void print_draft_for_del(char *name)
{
	int index = get_user__index(name);
	int i = 0;
	for (i = 0; i < MAX_MESSAGES; i++)
	{
		// for each message that is FROM this user, list it with a number
		if (!strcmp(name, globs.listOfMessages[index][i].from) && (globs.listOfMessages[index][i].is_draft == 1))
		{
			printf("%d: To: %s Msg: %s\n", i, globs.listOfMessages[index][i].to, globs.listOfMessages[index][i].msg);
		}
	}

	char buf[3] = {0};
	size_t l = 0;
	while (l < 1)
		l = receive_until( buf, '\n', 2);

	int choice = atoi(&buf[0]);
	if (choice >= MAX_MESSAGES || choice < 0)
	{
		printf("Out of range.\n");
		return;
	}

	if (strcmp(name, globs.listOfMessages[index][choice].from) || (globs.listOfMessages[index][choice].is_draft != 1))
	{
		printf("Not a valid selection.\n");
		return;
	}

	bzero(globs.listOfMessages[index][choice].to, MAX_NAME_LEN);
	bzero(globs.listOfMessages[index][choice].from, MAX_NAME_LEN);
	bzero(globs.listOfMessages[index][choice].msg, MAX_MSG_LEN);
	globs.listOfMessages[index][choice].match = 0;
	current_user->msg_count--;
}



/***************************************
 *   service.c 
 **************************************/

/*

Author: James Nuttall (james%cromulence.co)

Copyright (c) 2015 Cromulence LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/


int bad_login_count;

void print_A()
{
	printf("******************\n");
	printf("1. Create User\n");
	printf("2. Login\n");
	printf("3. Exit\n");
	printf(": \n");
}


int handle_A_input()
{
	char buf[3] = {0};
	// same for all users
	// handle create user
	// handle login
	// handle exit
	size_t l = 0;
	while (l < 1)
		l = receive_until( buf, '\n', 2);

	if (buf[0] == '1')
	{
		// CREATE USER
		create_user(0);
	}
	else if (buf[0] == '2')
	{
		// LOGIN

		list_users();

		char name[MAX_NAME_LEN];
		char pswd[MAX_PASS_LEN];

		printf("Username: \n");
		size_t len = receive_until( name, '\n', MAX_NAME_LEN - 1);
		name[len++] = 0;
		printf("Password: \n");
		len = receive_until( pswd, '\n', MAX_PASS_LEN - 1);
		pswd[len++] = 0;

		msg_count_login = 0;

		if (login(name, pswd) == 1)
		{
			// successful login
			
			return 2;
		}
		else
		{
			printf("Bad login.\n");
			bad_login_count++;
			if (bad_login_count >= 3)
			{
				create_user(bad_login_count); // this allows a user to be re-created
				bad_login_count = 0;
			}
		}
	}
	else if (buf[0] == '3')
	{
		// EXIT
		printf("The end.\n");
		exit(0);
	}
	else
		printf("unknown input: %c %c\n", buf[0], buf[1]);
	return 0;
}

void print_B()
{
	printf("********%s********\n", current_user->name);
	printf("1. Create Message\n");
	printf("2. Send Message\n");
	printf("3. List All [%d]\n", get_total_count());
	printf("4. List Drafts [%d]\n", get_draft_count(current_user->name));
	printf("5. List Inbox [%d]\n", get_inbox_count(current_user->name));
	printf("6. Delete Draft\n");
	printf("7. Logout\n");
	printf("8. Exit\n");
	printf(": \n");
}

int handle_B_input()
{
	char buf[3] = {0};
	size_t l = 0;
	while (l < 1)
		l = receive_until( buf, '\n', 2);

	if (buf[0] == '7')
	{
		// LOGOUT
		printf("Logging out of user %s\n", current_user->name);
		logout();
		return 2;
	}
	else if (buf[0] == '8')
	{
		// EXIT
		printf("The end.\n");
		exit(0);
	}
	else if (buf[0] == '2')
	{
		// SEND MESSAGE from drafts list
		if (get_draft_count(current_user->name) == 0)
		{
			printf("Must create draft first.\n");
			return 0;
		}

		// list all drafts with number in front
		// user enters valid number
		// that message is changed from drafts 1->0
		print_draft_for_send(current_user->name);
	}
	else if (buf[0] == '1')
	{
		// ADD A MESSAGE TO DRAFTS
		if (msg_count_login >= MAX_MESSAGES_LOGIN)
		{
			printf("Reached maximum messages for this session.\n");
			return 0;
		}

		printf("To:\n");

		char dest[MAX_NAME_LEN];
		size_t len = receive_until( dest, '\n', MAX_NAME_LEN - 1);
		dest[len++] = 0;

		printf("Message:\n");
		char msg_buf[MAX_MSG_LEN];
		len = receive_until( msg_buf, '\n', MAX_MSG_LEN - 1);
		msg_buf[len++] = 0;

		if (add_message(dest, current_user->name, msg_buf, 1) != 1)
			printf("add_message failed\n");
	}
	else if (buf[0] == '3')
	{
		// LIST MESSAGES
		// list all messages in summary
		if (get_total_count() == 0)
		{
			printf("No messages.\n");
			return 0;
		}

		list_all_messages();
	}
	else if (buf[0] == '4')
	{
		// LIST DRAFTS
		list_drafts(current_user->name);
	}
	else if (buf[0] == '5')
	{
		// LIST INCOMING MSGS
		list_inbox(current_user->name);
	}
	else if (buf[0] == '6')
	{
		// DELETE MESSAGE from drafts list
		if (get_draft_count(current_user->name) == 0)
		{
			printf("Must create draft first.\n");
			return 0;
		}
		print_draft_for_del(current_user->name);
	}
	else
		printf("unknown input: %c %c", buf[0], buf[1]);
	return 0;
}

int main()
{
	msg_count_login = 0;
	user_count = 0;
	bad_login_count = 0;

A_menu:
	while (1)
	{
		print_A();
		if (handle_A_input() == 2)
			goto B_menu;
	}

B_menu:
	while (1)
	{
		print_B();
		if (handle_B_input() == 2)
			goto A_menu;
	}
	
	return 0;
}


/***************************************
 *   user.c 
 **************************************/

/*

Author: James Nuttall (james%cromulence.co)

Copyright (c) 2015 Cromulence LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/



// successful login: 1, currentUser is correct
// bad login: 0
int login(char *name, char *passwd)
{
	// compare username and password with users
	for (int i = 0; i < user_count; i++)
	{
		if ((strcmp(name, listOfUsers[i]->name) == 0) && (!strcmp(passwd, listOfUsers[i]->password)))
		{
			current_user = listOfUsers[i];
			return 1;
		}
	}
	return 0;
}

int logout()
{
	current_user->first_login = 0;
	return 1;
}

int get_user__index(char *name)
{
	for (int i = 0; i < user_count; i++)
	{
		if (strcmp(name, listOfUsers[i]->name) == 0)
		{
			return i;
		}
	}
	return -1;
}

void create_user(int tries)
{	
	char buf[MAX_NAME_LEN];
	char buf2[MAX_PASS_LEN];
	printf("Username: \n");
	size_t len = receive_until( buf, '\n', MAX_NAME_LEN - 1);
	buf[len++] = 0;

	int found = -1;
	// does this user already exist?
	if (strlen(buf) < 1) 
	{
		return;
	}
	for (int j = 0; j < user_count; j++)
	{
		if (!strcmp(buf, listOfUsers[j]->name))
		{
			found = j;
			break;
		}
	}

	// user tried a bad password 3x, reset password
	if (tries > 0)
	{
		if (found == -1) 
		{
			// Failed login, but username not found. Do nothing
			return;
		}
		
		// this user already found, put this user on top of the last one
		// replace password
		// first login

		listOfUsers[found]->first_login = 1;
		printf("Password: \n");
		len = receive_until( buf2, '\n', MAX_PASS_LEN - 1);
		buf2[len++] = 0;
		bzero(listOfUsers[found]->password, MAX_PASS_LEN);
		strncpy(listOfUsers[found]->password, buf2, len);
		return;
	}

	if (found == -1)
	{
		// this user doesn't exist, are we full already?
		if (user_count >= MAX_USERS)
		{
			printf("Maximum number of users reached.\n");
			return;
		}
		else
		{
			listOfUsers[user_count] = (User*)malloc(sizeof(User));
			strncpy(listOfUsers[user_count]->name, buf, len);

			listOfUsers[user_count]->index = user_count;
			listOfUsers[user_count]->first_login = 1;
			listOfUsers[user_count]->msg_count = 0;
			
			printf("Password: \n");
			len = receive_until( buf2, '\n', MAX_PASS_LEN - 1);
			buf2[len++] = 0;
			strncpy(listOfUsers[user_count++]->password, buf2, len);
		}
	}
}

void list_users()
{
	for (int i = 0; i < user_count; i++)
	{
		printf("%s -- %s\n", listOfUsers[i]->name, listOfUsers[i]->password);
	}
}
