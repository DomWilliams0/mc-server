#ifndef MC_SERVER_CONNECTION_H
#define MC_SERVER_CONNECTION_H

enum class ConnectionState {
    kHandshaking = 0,
    kStatus = 1,
    kLogin = 2,
};


class BaseConnection {

};

class ConnectionHandshaking : public BaseConnection {

};

class ConnectionStatus : public BaseConnection {

};

class ConnectionLogin : public BaseConnection {

};

class ConnectionPlay : public BaseConnection {

};


#endif
