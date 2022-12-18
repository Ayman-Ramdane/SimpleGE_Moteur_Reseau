#include "chickendodge/pch/precomp.h"

#include "networkplayerserver.h"

#include "chickendodge/messages/networklogin.h"
#include "chickendodge/messages/networkscore.h"
#include "chickendodge/messages/networkstart.h"
#include <chickendodge/messages/networkleaderboardmessage.h>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>

using namespace SimpleGE;

namespace ChickenDodge
{
  void NetworkPlayerServerComponent::OnMessage(Network::Connection& connection, const BaseMessage& msg)
  {
    fmt::print("Message de {}\n", connection.GetID());
    if (msg.Is<NetworkLogin>())
    {
      OnNetworkLogin(connection, msg.Get<NetworkLogin>());
    }
    if (msg.Is<NetworkInputChanged>())
    {
      auto otherPlayer = clients.find(connection.GetID())->second.otherPlayer.lock();

      if (otherPlayer != nullptr)
      {
        NetworkSystem::Send(otherPlayer.get(), msg);
      }
    }
    if (msg.Is<NetworkScore>())
    {
      // Le message est envoyé par le player.cpp lors d'une collision avec un rubis
      // Sauvegarder le message dans une variable
      const auto& scoreMsg = msg.Get<NetworkScore>();

      // Sauvegarder l'id de connexion dans une variable locale
      auto currentId = connection.GetID();

      // Récupérer le nom du joueur
      std::string name = clients.find(currentId)->second.name;

      // Sauvegarder les valeurs reçues dans des variables locales
      int score = scoreMsg.playerScore;
      std::string msgName = scoreMsg.playerName;
      std::pair<std::string, int> data = {name, score};

      // Vérifier que le nom reçu correspond à celui de la connexion
      if (name == msgName)
      {
        // Insérer le score si le tableau des meneurs est vide
        if (scores.empty())
        {
          scores.insert(data);
        }
        else
        {
          // Récupérer le score le plus élevé dans le tableau
          int highScore = 0;
          for (auto& pair : scores)
          {
            int currentScore = pair.second;
            if (currentScore > highScore)
            {
              highScore = currentScore;
            }
          }

          // Si le score reçu est supérieur au maximum mettre à jour le tableau des meneurs
          if (score > highScore)
          {
            // Vérifier si le nom du joueur est déjà présent sur le tableau des meneurs
            auto iter = scores.find(name);

            // Si le nom est déjà présent, remplacer l'ancienne valeur du score
            if (iter != scores.end())
            {
              iter->second = score;
            }
            // Sinon insérer le score du nouveau joueur
            else
            {
              scores.insert(data);
            }
          }
        }

        // Convertir le tableau des meneurs en vector pour l'envoi vers les clients
        leaderBoard.reserve(scores.size());
        for (auto& pair : scores)
        {
          leaderBoard.emplace_back(pair);
        }

        // Créer le message de contenant le tableau des meneurs
        NetworkLeaderboardMessage msg(leaderBoard);

        // Envoyer le tableau des meneurs à tous les clients
        for (auto& client : clients)
        {
          auto& socket = client.second;
          NetworkSystem::Send(socket.player.get(), msg);
        }
      }
    }
  }

  void NetworkPlayerServerComponent::OnNetworkLogin(Network::Connection& connection, const NetworkLogin& msg)
  {
    auto id = connection.GetID();
    auto& socketData = clients.find(id)->second;
    socketData.name = msg.name;

    // Si aucun joueur n'est en attente, on place le nouveau
    // joueur en attente.
    if (pendingPlayers.empty())
    {
      pendingPlayers.insert(id);
      return;
    }

    // Si il y a des joueurs en attente, on associe un de
    // ces joueurs à celui-ci.
    auto otherId = *pendingPlayers.begin();
    pendingPlayers.erase(otherId);

    auto& otherSocketData = clients.find(otherId)->second;
    socketData.otherPlayer = otherSocketData.player;
    otherSocketData.otherPlayer = socketData.player;

    // On envoie alors la liste des joueurs de la partie
    // à chacun des participants.
    NetworkStart p1(0, {otherSocketData.name, socketData.name});
    NetworkStart p2(1, {otherSocketData.name, socketData.name});

    NetworkSystem::Send(otherSocketData.player.get(), p1);
    NetworkSystem::Send(socketData.player.get(), p2);

    // Envoyer le tableau des meneurs aux clients lors de la connexion s'il n'est pas vide
    if (!leaderBoard.empty())
    {
      NetworkLeaderboardMessage msg(leaderBoard);
      NetworkSystem::Send(otherSocketData.player.get(), msg);
      NetworkSystem::Send(socketData.player.get(), msg);
    }
  }

  void NetworkPlayerServerComponent::OnConnectImpl(const Network::ConnectionRef& connection)
  {
    fmt::print("Connexion de {}\n", connection->GetID());
    Expects(clients.find(connection->GetID()) == clients.end());
    clients.emplace(connection->GetID(), connection);
  }

  void NetworkPlayerServerComponent::OnDisconnectImpl(gsl::not_null<Network::Connection*> connection)
  {
    auto id = connection->GetID();
    fmt::print("Déconnexion de {}\n", id);
    Expects(clients.find(id) != clients.end());

    auto& socketData = clients.find(id)->second;
    auto otherPlayer = socketData.otherPlayer.lock();
    if (otherPlayer != nullptr)
    {
      clients.erase(otherPlayer->GetID());
    }

    pendingPlayers.erase(id);
    clients.erase(id);
  }
} // namespace ChickenDodge