#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>

using namespace std;

int session_time = 20; // Time of the session in seconds

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
void loadIdP()
{
	if (!ls.empty())
		return;

	fstream IdP("server/IdP_DB.txt");

	string input;
	while (getline(IdP, input))
	{
		IdPUser newUser;
		newUser.id_user = input;
		getline(IdP, input);
		newUser.username = input;
		getline(IdP, input);
		newUser.password = input;
		getline(IdP, input);
		do
		{
			newUser.roles.push_back(input);
			getline(IdP, input);
		} while (input != ";");
		ls.push_back(newUser);
	}

	IdP.close();
}

// Client save user credential to cookies
void cSaveCookies(string username, ConnectionResponse response)
{
	fstream cookies("client/Cookies.txt");
	cookies.clear();

	cookies << response.id_session << '\n';
	cookies << response.id_user << '\n';
	cookies << response.expire_date << '\n';
	cookies << ';' << '\n';

	cookies.close();
}

// SSO client and server functions
void cGrantAccess(UserAttributes userAttributes) {
	cout << "Access granted for user " << userAttributes.username << std::endl;
	cout << "with roles:\n";
	for (auto r : userAttributes.roles)
		cout << r << '\n';
}

// Deny user accessing
void cDenyAccess() {
	std::cout << "Access denied" << std::endl;
}

// Client load saved cookies
ConnectionResponse cGetCookies()
{
	fstream cookies("client/Cookies.txt");
	ConnectionResponse rs =
	{
		"","",""
	};

	string info;
	if (getline(cookies, info))
	{
		rs.id_session = info;
		getline(cookies, info);
		rs.id_user = info;
		getline(cookies, info);
		rs.expire_date = info;
	}

	cookies.close();
	return rs;
}

// Request preparation with username and password
ConnectionRequest cPrepareForAuthenticate() {
	ConnectionRequest request;

	cout << "Enter username:\n";
	getline(cin, request.username);
	cout << "Enter password:\n";
	getline(cin, request.password);
	return request;
}

// Client get the response from server
bool cProcessResponse(ConnectionResponse response)
{
	if (response.id_user != "ERROR")
	{
		cout << "Sign in successfully!\n";
		return true;
	}
	return false;
}

// Retrieval of user attributes
UserAttributes sGetAttributes(string id_user)
{
	loadIdP();

	UserAttributes attributes;
	for (IdPUser u : ls)
	{
		if (u.id_user == id_user)
		{
			attributes.username = u.username;
			for (auto r : u.roles)
				attributes.roles.push_back(r);
		}
	}
	return attributes;
}

// Server create a new session and save it
void sCreateSession(ConnectionResponse& u)
{
	// Generate a new sesion
	string demo_id = "0007"; // Noramlly this will be a id generator on the server side
	u.id_session = demo_id;

	// Save session
	fstream sessions("server/open_session.txt");

	sessions << u.id_session << '\n';
	sessions << u.id_user << '\n';
	sessions << u.expire_date << '\n';
	sessions << ';' << '\n';

	sessions.close();

	return;
}

// IdP Checking and process's data
ConnectionResponse sIdPCheck(ConnectionRequest request)
{
	loadIdP();

	ConnectionResponse response;
	for (IdPUser u : ls)
	{
		if (request.username == u.username && request.password == u.password)
		{
			response.id_user = u.id_user;
			response.expire_date = to_string(time(nullptr) + session_time);
			sCreateSession(response);
			return response;
		}
	}

	response.id_user = "ERROR";

	return response;
}

ConnectionResponse sCheckSavedSession(ConnectionResponse s)
{
	fstream sessions("server/open_session.txt");
	ConnectionResponse rs{"", "", ""};

	string id_session;
	getline(sessions, id_session);
	if (s.id_session == id_session)
	{
		rs = s;
	}

	sessions.close();

	return rs;
}

// Main function
int main()
{
	cout << "-- Client 1 --\n\n";
	// Check cookies for previous session
	auto savedCookie = cGetCookies();
	ConnectionResponse session;
	if (savedCookie.id_session == "")
	{
		// Initiate request
		ConnectionRequest request = cPrepareForAuthenticate();

		// Receive response from IdP
		session = sIdPCheck(request);
	}
	else
	{
		// Check session expire time
		if (stoi(savedCookie.expire_date) - time(nullptr) < 0)
		{
			cout << "Previous session expired.\n";
			ConnectionRequest request = cPrepareForAuthenticate();
			 
			// Receive response from IdP
			session = sIdPCheck(request);
		}
		else
		{
			// Previous session still valid
			cout << "Previous session found.\n";
			session = sCheckSavedSession(savedCookie);
		}
	}

	// Verify response
	if (cProcessResponse(session)) {
		// Response is valid, retrieve user attributes	
		UserAttributes userAttributes = sGetAttributes(session.id_user);

		// Authenticate user and grant access
		cGrantAccess(userAttributes);
		// Save user's data to cookies
		cSaveCookies(userAttributes.username, session);
		// Save user's session
	}
	else {
		// Response is invalid, deny access
		cDenyAccess();
	}

	return 0;
}