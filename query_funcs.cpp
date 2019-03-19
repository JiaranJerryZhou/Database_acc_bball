#include "query_funcs.h"

/*
This function inserts a new player into the player table
 */
void add_player(connection *C, int team_id, int jersey_num, string first_name,
                string last_name, int mpg, int ppg, int rpg, int apg,
                double spg, double bpg) {
  work W(*C);
  string prevMax = "SELECT MAX(PLAYER_ID) FROM PLAYER;";
  result res(W.exec(prevMax));
  auto r = res.begin();
  int max = r[0].as<int>();
  int player_id = max + 1;
  string to_insert = "INSERT INTO PLAYER (PLAYER_ID, TEAM_ID, UNIFORM_NUM, "
                     "FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG) ";
  to_insert += "VALUES (" + to_string(player_id) + ", " + to_string(team_id) +
               ", " + to_string(jersey_num) + ", " + W.quote(first_name) +
               ", " + W.quote(last_name) + ", " + to_string(mpg) + ", " +
               to_string(ppg) + ", " + to_string(rpg) + ", " + to_string(apg) +
               ", " + to_string(spg) + ", " + to_string(bpg) + ");";
  W.exec(to_insert);
  W.commit();
}

/*
  This function inserts a new team into the team table
 */
void add_team(connection *C, string name, int state_id, int color_id, int wins,
              int losses) {
  work W(*C);
  string prevMax = "SELECT MAX(TEAM_ID) FROM TEAM;";
  result res(W.exec(prevMax));
  auto r = res.begin();
  int max = r[0].as<int>();
  int team_id = max + 1;
  string to_insert =
      "INSERT INTO TEAM (TEAM_ID,NAME,STATE_ID,COLOR_ID,WINS,LOSSES) ";
  to_insert += "VALUES (" + to_string(team_id) + ", " + W.quote(name) + ", " +
               to_string(state_id) + ", " + to_string(color_id) + ", " +
               to_string(wins) + ", " + to_string(losses) + ");";
  W.exec(to_insert);
  W.commit();
}

/*
This function inserts a new state to the state table
 */
void add_state(connection *C, string name) {
  work W(*C);
  string prevMax = "SELECT MAX(STATE_ID) FROM STATE;";
  result res(W.exec(prevMax));
  auto r = res.begin();
  int max = r[0].as<int>();
  int state_id = max + 1;
  string to_insert = "INSERT INTO STATE (STATE_ID,NAME) ";
  to_insert += "VALUES (" + to_string(state_id) + ", " + W.quote(name) + ");";
  W.exec(to_insert);
  W.commit();
}

/*
This function inserts a new color to the color table
 */
void add_color(connection *C, string name) {
  work W(*C);
  string prevMax = "SELECT MAX(COLOR_ID) FROM COLOR;";
  result res(W.exec(prevMax));
  auto r = res.begin();
  int max = r[0].as<int>();
  int color_id = max + 1;
  string to_insert = "INSERT INTO COLOR (COLOR_ID,NAME) ";
  to_insert += "VALUES (" + to_string(color_id) + ", " + W.quote(name) + ");";
  W.exec(to_insert);
  W.commit();
}

/*
show all attributes of each player with average statistics that fall
between the min and max (inclusive) for each enabled statistic
 */
void query1(connection *C, int use_mpg, int min_mpg, int max_mpg, int use_ppg,
            int min_ppg, int max_ppg, int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg, int use_spg, double min_spg,
            double max_spg, int use_bpg, double min_bpg, double max_bpg) {
  work W(*C);
  string query = "SELECT * FROM PLAYER ";
  // this flag is used to determine if AND is necessary
  bool flag = false;
  if (use_mpg || use_ppg || use_rpg || use_apg || use_spg || use_bpg) {
    query += "WHERE ";
  }
  if (use_mpg) {
    flag = true;
    query += "(MPG >= " + to_string(min_mpg) +
             " AND MPG <= " + to_string(max_mpg) + ") ";
  }
  if (use_ppg) {
    if (flag) {
      query += "AND (PPG >= " + to_string(min_ppg) +
               " AND PPG <= " + to_string(max_ppg) + ") ";
    } else {
      query += "(PPG >= " + to_string(min_ppg) +
               " AND PPG <= " + to_string(max_ppg) + ") ";
    }
    flag = true;
  }
  if (use_rpg) {
    if (flag) {
      query += "AND (RPG >= " + to_string(min_rpg) +
               " AND RPG <= " + to_string(max_rpg) + ") ";
    } else {
      query += "(RPG >= " + to_string(min_rpg) +
               " AND RPG <= " + to_string(max_rpg) + ") ";
    }
    flag = true;
  }
  if (use_apg) {
    if (flag) {
      query += "AND (APG >= " + to_string(min_apg) +
               " AND APG <= " + to_string(max_apg) + ") ";
    } else {
      query += "(APG >= " + to_string(min_apg) +
               " AND APG <= " + to_string(max_apg) + ") ";
    }
    flag = true;
  }
  if (use_spg) {
    if (flag) {
      query += "AND (SPG >= " + to_string(min_spg) +
               " AND SPG <= " + to_string(max_spg) + ") ";
    } else {
      query += "(SPG >= " + to_string(min_spg) +
               " AND SPG <= " + to_string(max_spg) + ") ";
    }
    flag = true;
  }
  if (use_bpg) {
    if (flag) {
      query += "AND (BPG >= " + to_string(min_bpg) +
               " AND BPG <= " + to_string(max_bpg) + ")";
    } else {
      query += "(BPG >= " + to_string(min_bpg) +
               " AND BPG <= " + to_string(max_bpg) + ")";
    }
    flag = true;
  }
  query += ";";
  result res(W.exec(query));
  cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG "
          "SPG BPG"
       << endl;
  for (auto r : res) {
    // this flag is to used to handle the space and precision
    int count = 0;
    for (auto field : r) {
      if (count == 0) {
        cout << field;
      } else if (count >= 9) {
        std::cout.precision(1);
        std::cout.setf(std::ios::fixed, std::ios::floatfield);
        cout << " " << field.as<double>();
      } else {
        cout << " " << field;
      }
      count++;
    }
    cout << endl;
  }
}

/*
show the name of each team with the indicated uniform color
 */
void query2(connection *C, string team_color) {
  work W(*C);
  string query = "SELECT TEAM.NAME FROM TEAM, COLOR WHERE ";
  query += "TEAM.COLOR_ID=COLOR.COLOR_ID AND ";
  query += "COLOR.NAME=" + W.quote(team_color) + ";";
  result res(W.exec(query));
  cout << "NAME" << endl;
  for (auto r : res) {
    cout << r[0] << endl;
  }
}

/*
show the first and last name of each player that plays for the indicated
team, ordered from highest to lowest ppg (points per game)
 */
void query3(connection *C, string team_name) {
  work W(*C);
  string query =
      "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME FROM PLAYER, TEAM WHERE ";
  query += "PLAYER.TEAM_ID=TEAM.TEAM_ID AND ";
  query += "TEAM.NAME=" + W.quote(team_name);
  query += " ORDER BY PPG DESC;";
  result res(W.exec(query));
  cout << "FIRST_NAME LAST_NAME" << endl;
  for (auto r : res) {
    cout << r[0] << " " << r[1] << endl;
  }
}

/*
show first name, last name, and jersey number of each player that
plays in the indicated state and wears the indicated uniform color
 */
void query4(connection *C, string team_state, string team_color) {
  work W(*C);
  string query = "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME, "
                 "PLAYER.UNIFORM_NUM FROM PLAYER, TEAM, STATE, COLOR WHERE ";
  query += "PLAYER.TEAM_ID=TEAM.TEAM_ID AND TEAM.COLOR_ID=COLOR.COLOR_ID AND "
           "TEAM.STATE_ID=STATE.STATE_ID ";
  query += "AND STATE.NAME=" + W.quote(team_state);
  query += "AND COLOR.NAME=" + W.quote(team_color);
  result res(W.exec(query));
  cout << "FIRST_NAME LAST_NAME UNIFORM_NUM" << endl;
  for (auto r : res) {
    cout << r[0] << " " << r[1] << " " << r[2] << endl;
  }
}

/*
  show first name and last name of each player, and team name and
number of wins for each team that has won more than the indicated number of
games
 */
void query5(connection *C, int num_wins) {
  work W(*C);
  string query = "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME, TEAM.NAME, "
                 "TEAM.WINS FROM PLAYER, TEAM WHERE ";
  query += "PLAYER.TEAM_ID=TEAM.TEAM_ID AND ";
  query += "TEAM.WINS > " + to_string(num_wins) + ";";
  result res(W.exec(query));
  cout << "FIRST_NAME LAST_NAME NAME WINS" << endl;
  for (auto r : res) {
    cout << r[0] << " " << r[1] << " " << r[2] << " " << r[3] << endl;
  }
}
