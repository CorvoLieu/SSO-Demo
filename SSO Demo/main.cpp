#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>

#include "server.h"

using namespace std;

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

// Main function
int main()
{
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