/**
 * Copyright (c) Sjors Gielen, 2010
 * See LICENSE for license.
 */

#ifndef NETWORK_H
#define NETWORK_H

#include "user.h"
#include <vector>
#include <string>
#include <map>

class Server;

struct ServerConfig;

struct NetworkConfig {
  std::string name;
  std::string displayName;
  std::string nickName;
  std::string userName;
  std::string fullName;
  std::string password;
  std::vector<ServerConfig*> servers;
  bool autoConnect;
};

class NetworkListener
{
  public:
    virtual void ircEvent(const std::string &event, const std::string &origin,
                          const std::vector<std::string> &params, Network *n ) = 0;
};

class Network
{

  friend class Server;

  public:
                    Network( const NetworkConfig *c );
                   ~Network();

    static std::string toString(const Network *n);
    void               addListener( NetworkListener *nl ) {
      networkListeners_.push_back(nl);
    }

    enum DisconnectReason {
      UnknownReason,
      ShutdownReason,
      ConfigurationReloadReason,
      SwitchingServersReason,
      ErrorReason,
      AdminRequestReason,
    };

    bool                        autoConnectEnabled() const;
    const std::vector<ServerConfig*> &servers() const;
    User                       *user();
    Server                     *activeServer() const;
    const NetworkConfig        *config() const;
    int                         serverUndesirability( const ServerConfig *sc ) const;
    std::string                 networkName() const;
    std::vector<std::string>    joinedChannels() const;
    bool                        isIdentified(const std::string &user) const;
    bool                        isKnownUser(const std::string &user) const;

    void connectToNetwork( bool reconnect = false );
    void disconnectFromNetwork( DisconnectReason reason = UnknownReason );
    void joinChannel( std::string channel );
    void leaveChannel( std::string channel );
    void say( std::string destination, std::string message );
    void action( std::string destination, std::string message );
    void names( std::string channel );
    void ctcp( std::string destination, std::string message );
    void sendWhois( std::string destination );
    void flagUndesirableServer( const ServerConfig *sc );
    void serverIsActuallyOkay( const ServerConfig *sc );
    void addDescriptors(fd_set *in_set, fd_set *out_set, int *maxfd);
    void processDescriptors(fd_set *in_set, fd_set *out_set);

  private:
    void connectToServer( ServerConfig *conf, bool reconnect );

    Server               *activeServer_;
    const NetworkConfig  *config_;
    std::map<const ServerConfig*,int> undesirables_;
    User                 *me_;
    bool                  deleteServer_;
    std::vector<std::string>        identifiedUsers_;
    std::map<std::string,std::vector<std::string> > knownUsers_;
    std::vector<NetworkListener*>   networkListeners_;

    void onFailedConnection();
    void joinedChannel(const std::string &user, const std::string &receiver);
    void kickedChannel(const std::string &user, const std::string&, const std::string&, const std::string &receiver);
    void partedChannel(const std::string &user, const std::string &, const std::string &receiver);
    void slotQuit(const std::string &origin, const std::string&, const std::string &receiver);
    void slotWhoisReceived(const std::string &origin, const std::string &nick, bool identified);
    void slotNickChanged( const std::string &origin, const std::string &nick, const std::string &receiver );
    void slotNamesReceived(const std::string&, const std::string&, const std::vector<std::string> &names, const std::string &receiver );
    void slotIrcEvent(const std::string&, const std::string&, const std::vector<std::string>&);
};

#endif
