# SSO-Demo

## About

This is a simple source code for Single-Sign On simulation developed solely for the purpose of study. The main goal of this project is to illustrate the necessary steps for a client to connect to a server and the trust negotiation between two entity inorder to establish a stable and secure connection.
This project will only focus on the negotiation process, thus only one computer is used to execute the file; to be more specific, only one source code will be utilized for the process.

## File structure

### Folder

Beside the default Visual Studio folder, there are two noticeable folder, client and server, representing their respected side.

The sever folder contain the Identification Provider (IdP) and the saved opened session of its client while the client side consist of only the cookies file.

### Code

`main.cpp` is the representation of both the client and server process of SSO with their functions named with the first letter initial of respected files. For example, the client will have functions such as `cSaveCookies`, `cGrantAccess`... while server will be `sIdPCheck`, `sCheckSavedSession`...

## File configuration

- Session time: The time where a login session expired. This can be configure in the top of the `main.cpp` file where the number is calculated in seconds
- IdP account: Each account on the server are saved in the Identification Provider Database (`IdP_DB.txt`). The `;` seperate the accounts and each has their own distinctive information such as their ID, Username, Password, and Roles($\geq 1$) formated in the order aformemtioned.

## Work flow

- On initial, the program will check for the previously saved session in the cookie.
- If one is found and still valid, not expired, the program will continue the session, the program end. __Note__ that the session expired time will not get extended.
- If none is found or one has expired, the user is required to enter their credential, username and password, again.
- If the user enter the verified correct credential, the client will get the user's authorization and grant access to the user.
- If the user false to provide a valid credential, the user is denied of any accessing.
