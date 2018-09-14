#include <stdio.h>
#include "sqlite/sqlite3.h"
#include <stdlib.h>

// define array length
#define STR_MAX_LEN 1024
#define DATETIME_LEN 19

// define return status type
typedef enum bk_ret bk_ret_t;
enum bk_ret {
    OK = 0,
    ERROR,
    AGAIN,
};

// define user data structure
typedef struct user_s user_t;
struct user_s {
    int id;
    char name[STR_MAX_LEN];
    char password[STR_MAX_LEN];
    char created_date[DATETIME_LEN];
    int age;
    int level;
    float amount;
};

// define operation data structure
typedef struct operation_s operation_t;
struct operation_s {
    int id;
    char name[STR_MAX_LEN];
    char operate_date[DATETIME_LEN];
    char user_id;
    float amount;
};

// declare user crud function
static bk_ret_t user_query(char *filter, int limit, int offset, user_t *users);
static bk_ret_t user_create(user_t user);
static bk_ret_t user_update(user_t user);
static bk_ret_t user_delete(int user_id);

// declare operation crud function
static bk_ret_t operation_query(char *filter, int limit, int offset, operation_t *ops);
static bk_ret_t operation_create(operation_t op);
static bk_ret_t operation_update(operation_t op);
static bk_ret_t operation_delete(int op_id);


enum BK_CLI {
    GET_USER=0,
    ADD_USER,
    UPD_USER,
    DEL_USER,

    GET_OP,
    ADD_OP,
    UPD_OP,
    DEL_OP
};

#define EXP_GET 'get_user'
#define EXP_ADD 'add_user'
#define EXP_UPD 'upd_user'
#define EXP_DEL 'del_user'

static bk_ret_t r = OK;

// service
int main(int argc, char **argv) {
    // sock init, program init, process init
    // skip

    if (argc !=2) {
        print('ERROR: invalid arguments');
        exit(1);
    }

    if (EXP_GET == argv[1]) {
        user_t user_list[10];
        user_t *p_user_list = user_list;
        char *example_filter = "name=\"bob\", age>20";
        r = user_query(example_filter, 10, 0, p_user_list);
        if (r == OK) {
            int i = 0;
            for (;i<10; i++) {
                user_t *u = p_user_list + i;
                if (u == NULL) {
                    break; //reach end
                }
                print('user_id=[%d], name=[%s], password=[%s], created_date=[%s], age=[%d], level=[%d], amount=[%f]\n',
                      u->id, u->name, u->password, u->created_date, u->age, u->level, u->amount);
            }
        }
    }

    else if (EXP_ADD == argv[1]) {
        user_t example_user = {NULL, 'alice', 'pwd123pwd', '2018-09-14 22:24:23', 25, 1, 100.00};
        r = user_add(example_user);
        if (r == OK) {
            print('add user %s succeed', example_user.name);
        }
        else {
            print('add user %s failed', example_user.name);
        }
    }

    else if (EXP_UPD == argv[1]) {
        user_t example_user = {0, 'pony', 'pwd123pwd', '2018-09-14 22:24:23', 25, 1, 100.00};
        r = user_upd(example_user);
        if (r == OK) {
            print('upd user %s succeed', example_user.name);
        }
        else {
            print('upd user %s failed', example_user.name);
        }
    }

    else if (EXP_DEL == argv[1]) {
        int example_id = 0;
        r = user_delete(example_id);
        if (r == OK) {
            print('delete user %d succeed', example_id);
        }
        else {
            print('delete user %d failed', example_id);
        }
    }

    else {
        print('unsupported command, exit');
        exit(0);
    }

    return 0;
}