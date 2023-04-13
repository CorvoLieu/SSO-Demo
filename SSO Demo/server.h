#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>

using namespace std;

int session_time = 30; // Time of the session in seconds

// User information
struct IdPUser {
	string username;
	string password;
	vector<string> roles;
	string id_user;
};

// Server side list of user
vector<IdPUser> ls;

// Sample data structures
struct UserAttributes {
	string username;
	vector<string> roles;
};

// Send this to IdP
struct ConnectionRequest {
	string username;
	string password;
};

// Receive from IdP
struct ConnectionResponse {
	string id_user;
	string id_session;
	string expire_date;
};

// Load IdP database
void loadIdP();

// Retrieval of user attributes
UserAttributes sGetAttributes(string id_user);

// Server create a new session and save it
void sCreateSession(ConnectionResponse& u);

// IdP Checking and process's data
ConnectionResponse sIdPCheck(ConnectionRequest request);

ConnectionResponse sCheckSavedSession(ConnectionResponse s);