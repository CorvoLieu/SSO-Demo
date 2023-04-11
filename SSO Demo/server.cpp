#include "server.h"

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

UserAttributes sGetAttributes(string id_user) {
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
	ConnectionResponse rs{ "", "", "" };

	string id_session, id_user;
	int expire_date;
	getline(sessions, id_session);
	getline(sessions, id_user);
	sessions >> expire_date;
	sessions.ignore();
	if (s.id_session == id_session && stoi(s.expire_date) == expire_date && id_user == s.id_user)
	{
		rs = s;
	}

	sessions.close();

	return rs;
}
