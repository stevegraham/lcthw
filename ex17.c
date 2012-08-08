#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_DATA 512
#define MAX_ROWS 100

struct Address {
  int  id;
  int  set;
  char name[MAX_DATA];
  char email[MAX_DATA];
};

struct Database {
  struct Address rows[MAX_ROWS];
};

struct Connection {
  FILE   * file;
  struct Database *db;
};

void die(const char * message) {
  if(errno) {
    perror(message);
  } else {
    printf("ERROR: %s\n", message);
  }
  exit(1);
}

void Address_print(struct Address * address) {
  printf("%d %s %s\n", address->id, address->name, address->email);
}

void Database_load(struct Connection * connection) {
  int rc = fread(connection->db, sizeof(struct Database), 1, connection->file);
  if(rc != 1) die("Failed to load database");
}

struct Connection * Database_open(const char * filename, char mode) {
  struct Connection * connection = malloc(sizeof(struct Connection));
  if(!connection) die("Memory error");

  connection->db = malloc(sizeof(struct Database));
  if(!connection->db) die("Memory error");

  if(mode == 'c') {
    connection->file = fopen(filename, "w");
  } else {
    connection->file = fopen(filename, "r+");
    if(connection->file) Database_load(connection);
  }

  if(!connection->file) die("Failed to open the file");

  return connection;
}

void Database_close(struct Connection * connection) {
  if(connection) {
    if(connection->file) fclose(connection->file);
    if(connection->db)   free(connection->db);

    free(connection);
  }
}

void Database_write(struct Connection * connection) {
  rewind(connection->file);

  int rc = fwrite(connection->db, sizeof(struct Database), 1, connection->file);
  if(rc != 1) die("Failed to write database");

  rc = fflush(connection->file);
  if(rc == -1) die("Cannot flush database");
}

void Database_create(struct Connection * connection) {
  int i = 0;

  for(i = 0; i < MAX_ROWS; i++) {
    struct Address addr     = { .id = i, .set = 0 };
    connection->db->rows[i] = addr;
  }
}

void Database_set(struct Connection * connection, int id, const char * name, const char * email) {
  struct Address *address = &connection->db->rows[id];
  if(address->set) die("Already set. Delete it first.");

  address->set = 1;
  // Warning BUG
  char * res = strncpy(address->name, name, MAX_DATA);
  // Demonstrate strncpy bug
  if(!res) die("Name copy failed");

  res = strncpy(address->email, email, MAX_DATA);
  if(!res) die("Email copy failed");
}

void Database_get(struct Connection * connection, int id) {
  struct Address * address = &connection->db->rows[id];

  if(address->set) {
    Address_print(address);
  } else {
    die("ID is not set");
  }
}

void Database_delete(struct Connection * connection, int id) {
  struct Address address = { .id = id, .set = 0 };
  connection->db->rows[id] = address;
}

void Database_list(struct Connection * connection) {
  int i = 0;
  struct Database * db = connection->db;

  for(i = 0; i < MAX_ROWS; i++) {
    struct Address * current = &db->rows[i];

    if(current->set) Address_print(current);
  }
}

int main(int argc, char * argv[]) {

  if(argc < 3) die("USAGE: ex17 <db_file> <action> [action params]");

  char * filename = argv[1];
  char   action   = argv[2][0];

  struct Connection * connection = Database_open(filename, action);
  int id = 0;

  if(argc > 3) id = atoi(argv[3]);
  if(id >= MAX_ROWS) die("There's not that many records");

  switch(action) {
    case 'c':
      Database_create(connection);
      Database_write(connection);
      break;

    case 'g':
      if(argc != 4) die("Need an id to get");

      Database_get(connection, id);
      break;

    case 's':
      if(argc != 6) die("Need id, name, and email to set");

      Database_set(connection, id, argv[4], argv[5]);
      Database_write(connection);
      break;

    case 'd':
      if(argc != 4) die("Need id to delete");

      Database_delete(connection, id);
      Database_write(connection);
      break;

    case 'l':
      Database_list(connection);
      break;

    default:
      die("Invalid action. Only: c=create, g=get, s=save, d=delete, l=list");
  }

  Database_close(connection);

  return 0;
}


