#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include <sstream>

#include "exerciser.h"

using namespace std;
using namespace pqxx;

// this function is used to populate the color table
void populateColor(connection *C) {
  ifstream file;
  file.open("color.txt");
  if (!file.good()) {
    cout << "connot open file" << endl;
    exit(EXIT_FAILURE);
  }
  while (!file.eof()) {
    string line;
    string color_id, name;
    getline(file, line);
    stringstream ss(line);
    ss >> color_id;
    if (ss.str().length() == 0) {
      break;
    }
    ss >> name;
    work W(*C);
    // cout << color_id << endl;
    // cout << name << endl;
    string to_insert = "INSERT INTO COLOR (COLOR_ID,NAME) "
                       "VALUES (" +
                       color_id + ", " + W.quote(name) + ");";

    W.exec(to_insert);
    W.commit();
  }
  file.close();
}

// this function is used to populate the state table
void populateState(connection *C) {
  ifstream file;
  file.open("state.txt");
  if (!file.good()) {
    cout << "connot open file" << endl;
    exit(EXIT_FAILURE);
  }
  while (!file.eof()) {
    string line;
    string state_id, name;
    getline(file, line);
    stringstream ss(line);
    if (ss.str().length() == 0) {
      break;
    }
    ss >> state_id;
    ss >> name;
    work W(*C);
    string to_insert = "INSERT INTO STATE (STATE_ID,NAME) "
                       "VALUES (" +
                       state_id + ", " + W.quote(name) + ");";

    W.exec(to_insert);
    W.commit();
  }
  file.close();
}

// this function is used to populate the team table
void populateTeam(connection *C) {
  ifstream file;
  file.open("team.txt");
  if (!file.good()) {
    cout << "connot open file" << endl;
    exit(EXIT_FAILURE);
  }
  while (!file.eof()) {
    string line;
    string team_id, name, state_id, color_id, wins, losses;
    getline(file, line);
    stringstream ss(line);
    if (ss.str().length() == 0) {
      break;
    }
    ss >> team_id;
    ss >> name;
    ss >> state_id;
    ss >> color_id;
    ss >> wins;
    ss >> losses;
    work W(*C);
    string to_insert =
        "INSERT INTO TEAM (TEAM_ID,NAME,STATE_ID,COLOR_ID,WINS,LOSSES) "
        "VALUES (" +
        team_id + ", " + W.quote(name) + ", " + state_id + ", " + color_id +
        ", " + wins + ", " + losses + ");";

    W.exec(to_insert);
    W.commit();
  }
  file.close();
}

// this function is used to populate the player table
void populatePlayer(connection *C) {
  ifstream file;
  file.open("player.txt");
  if (!file.good()) {
    cout << "connot open file" << endl;
    exit(EXIT_FAILURE);
  }
  while (!file.eof()) {
    string line;
    string player_id, team_id, uniform_num, first_name, last_name, mpg, ppg,
        rpg, apg, spg, bpg;
    getline(file, line);
    stringstream ss(line);
    if (ss.str().length() == 0) {
      break;
    }
    ss >> player_id;
    ss >> team_id;
    ss >> uniform_num;
    ss >> first_name;
    ss >> last_name;
    ss >> mpg;
    ss >> ppg;
    ss >> rpg;
    ss >> apg;
    ss >> spg;
    ss >> bpg;
    work W(*C);
    string to_insert = "INSERT INTO PLAYER "
                       "(PLAYER_ID,TEAM_ID,UNIFORM_NUM,FIRST_NAME,LAST_NAME,"
                       "MPG,PPG,RPG,APG,SPG,BPG) "
                       "VALUES (" +
                       player_id + ", " + team_id + ", " + uniform_num + ", " +
                       W.quote(first_name) + ", " + W.quote(last_name) + ", " +
                       mpg + ", " + ppg + ", " + rpg + ", " + apg + ", " + spg +
                       ", " + bpg + ");";

    W.exec(to_insert);
    W.commit();
  }
  file.close();
}

int main(int argc, char *argv[]) {

  // Allocate & initialize a Postgres connection object
  connection *C;

  try {
    // Establish a connection to the database
    // Parameters: database name, user name, user password
    C = new connection("dbname=acc_bball user=postgres password=passw0rd");
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return 1;
    }
  } catch (const std::exception &e) {
    cerr << e.what() << std::endl;
    return 1;
  }

  // TODO: create PLAYER, TEAM, STATE, and COLOR tables in the ACC_BBALL
  // database
  //      load each table with rows from the provided source txt files
  work W(*C);
  // drop tables at the beginning
  string dropTables = "DROP TABLE IF EXISTS PLAYER, TEAM, STATE, COLOR;";
  W.exec(dropTables);
  string makeColor = "CREATE TABLE COLOR("
                     "COLOR_ID INT PRIMARY KEY                  NOT NULL,"
                     "NAME                 TEXT                 NOT NULL);";
  string makeState = "CREATE TABLE STATE("
                     "STATE_ID INT PRIMARY KEY                  NOT NULL,"
                     "NAME                 TEXT                 NOT NULL);";
  string makeTeam = "CREATE TABLE TEAM("
                    "TEAM_ID	INT PRIMARY KEY NOT NULL,"
                    "NAME		TEXT    NOT NULL,"
                    "STATE_ID	INT	REFERENCES STATE(STATE_ID) NOT NULL ,"
                    "COLOR_ID	INT	REFERENCES COLOR(COLOR_ID) NOT NULL ,"
                    "WINS		INT     NOT NULL,"
                    "LOSSES		INT	NOT NULL);";
  string makePlayer = "CREATE TABLE PLAYER("
                      "PLAYER_ID   INT PRIMARY KEY               NOT NULL,"
                      "TEAM_ID     INT REFERENCES TEAM(TEAM_ID)  NOT NULL,"
                      "UNIFORM_NUM INT                           NOT NULL,"
                      "FIRST_NAME  TEXT                          NOT NULL,"
                      "LAST_NAME   TEXT                          NOT NULL,"
                      "MPG         INT                           NOT NULL,"
                      "PPG         INT                           NOT NULL,"
                      "RPG         INT                           NOT NULL,"
                      "APG         INT                           NOT NULL,"
                      "SPG         DOUBLE PRECISION             NOT NULL,"
                      "BPG         DOUBLE PRECISION             NOT NULL);";
  // make four tables and commit
  // must make state and color before others
  W.exec(makeState);
  W.exec(makeColor);
  W.exec(makeTeam);
  W.exec(makePlayer);
  W.commit();
  // populate the four tables
  populateColor(C);
  populateState(C);
  populateTeam(C);
  populatePlayer(C);

  exercise(C);

  // Close database connection
  C->disconnect();

  return 0;
}
