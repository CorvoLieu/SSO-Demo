#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>

using namespace std;

// Receive from IdP
struct ConnectionResponse {
	string id_user;
	string id_session;
	string expire_date;
};

// Client load saved cookies
ConnectionResponse cGetCookies()
{
	fstream cookies("../SSO Demo/client/Cookies.txt");
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

// Main function
int main()
{
	cout << "-- Client 2 --\n\n";
	// Check cookies for previous session
	auto savedCookie = cGetCookies();
	if (savedCookie.id_session == "")
	{
		// no cookies found
		cout << "No cookie found\n";
	}
	else
	{
		// Check session expire time
		if (stoi(savedCookie.expire_date) - time(nullptr) < 0)
		{
			cout << "The session expired. Log in to access.\n";
		}
		else
		{
			// Previous session still valid
			cout << "Welcome!\n";
		}
	}

	return 0;
}