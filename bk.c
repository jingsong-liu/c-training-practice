#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>

// define array length
#define STR_MAX_LEN 1024
#define DATETIME_LEN 20 // byte length of "2000-00-00 00:00:00"

// define Data base file name
#define DB_FILE "./bk.db"

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
static bk_ret_t user_query(char *filter, int limit, int offset, user_t *users, int *len);
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
        int len = 0;
        char *example_filter = "name=\"bob\", age>20";
        r = user_query(example_filter, 10, 0, p_user_list, &len);
        if (r == OK) {
            int i = 0;
            for (;i<len; i++) {
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


static bk_ret_t user_query(char *filter, int limit, int offset, user_t *users, int *len) {
    sqlite3 *db = NULL;
    char *zErrMsg = 0;
    int nrow = 0, ncolumn = 0;
    char **azResult;

    int rc;
    rc = sqlite3_open(DB_FILE, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s/n", sqlite3_errmsg(db));
        return ERROR;
    }
    else {
        printf('open sqlite 3 db succeed');
    }

    char sql[STR_MAX_LEN] = {0};
    sprintf(sql, "SELECT * FROM user_tbl WHERE %s LIMIT %s OFFSET %s", filter, limit, offset);
    printf(sql);

    rc = sqlite3_get_table( db , sql , &azResult , &nrow , &ncolumn , &zErrMsg );

    if (rc) {
        fprintf(stderr, "get table failed: %s/n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ERROR;
    }
    else {
        int i;
        for(i=0; i<( nrow + 1 ) * ncolumn ; i++ ) {
            printf( "azResult[%d] = %s/n", i , azResult[i]);
            // azResult is query result, save it into users
            user_t *u = users + i;
            sscanf(azResult[i], "%d %s %s %s %d %d %d",
            &u->id, &u->name, &u->password, &u->created_date, &u->age, &u->level, &u->amount);
        }
        *len = i;
        sqlite3_close(db);
        return OK;
    }

}

static bk_ret_t user_create(user_t user) {
    sqlite3 *db = NULL;
    char *zErrMsg = 0;
    int nrow = 0, ncolumn = 0;
    char **azResult;

    int rc;
    rc = sqlite3_open(DB_FILE, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s/n", sqlite3_errmsg(db));
        return ERROR;
    }
    else {
        printf('open sqlite 3 db succeed');
    }

    char sql[STR_MAX_LEN] = {0};
    sprintf(sql, "INSERT INTO user_tbl(name, password, created_date, age, level, amount) VALUES(%s %s %s %d %d %f)",
            user.name, user.password, user.created_date, user.age, user.level, user.amount);
    printf(sql);

    rc = sqlite3_exec(db , sql, NULL, NULL, &zErrMsg);
    if (rc) {
        fprintf(stderr, "get table failed: %s/n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ERROR;
    }
    else {
        printf('insert user succeed');
        return OK;
    }
}
// TODO
static bk_ret_t user_update(user_t user);
// TODO
static bk_ret_t user_delete(int user_id);