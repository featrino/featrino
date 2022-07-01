#pragma once

#include "common/Aliases.hpp"
#include "common/NetworkRequest.hpp"
#include "providers/twitch/TwitchEmotes.hpp"

#include <QJsonArray>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QUrlQuery>
#include <boost/optional.hpp>

#include <functional>
#include <vector>

namespace chatterino {

using GQLFailureCallback = std::function<void()>;
template <typename... T>
using ResultCallback = std::function<void(T...)>;

class IGQL
{
public:
    // https://github.com/lay295/TwitchChannelPoints/blob/master/TwitchChannelPoints/frmMain.cs#L151
    virtual void getUserPoints(QString userName,
                               ResultCallback<int> successCallback,
                               GQLFailureCallback failureCallback) = 0;

    virtual void update(QString oauthToken) = 0;
};

class GQL final : public IGQL
{
public:
    // https://dev.twitch.tv/docs/api/reference#get-users
    void getUserPoints(QString userName, ResultCallback<int> successCallback,
                       GQLFailureCallback failureCallback) final;

    void update(QString oauthToken) final;

    static void initialize();

private:
    NetworkRequest makeRequest(QString payload);

    QString oauthToken;
};

// initializeGQL sets the GQL instance to _instance
// from a normal application, this should never be called, and will instead be handled by calling GQL::initialize()
void initializeGQL(IGQL *_instance);

IGQL *getGQL();

}  // namespace chatterino
