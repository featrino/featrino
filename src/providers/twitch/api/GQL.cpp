#include "providers/twitch/api/GQL.hpp"

#include "common/Outcome.hpp"
#include "common/QLogging.hpp"

#include <QJsonDocument>

namespace chatterino {

static IGQL *instance = nullptr;

void GQL::getUserPoints(QString userName, ResultCallback<int> successCallback,
                        GQLFailureCallback failureCallback)
{
    QString query =
        "{\"operationName\": \"ChannelPointsContext\",\"variables\": { "
        "\"channelLogin\": \"" +
        userName +
        "\"}, \"extensions\": { "
        "\"persistedQuery\": { \"version\": 1, "
        "\"sha256Hash\": "
        "\"9988086babc615a918a1e9a722ff41d98847acac822645209ac7379eecb27152\"}}"
        "}";

    this->makeRequest(query)
        .onSuccess([successCallback, failureCallback](auto result) -> Outcome {
            try
            {
                auto root = result.parseJson();
                auto community =
                    root.value("data").toObject().value("community").toObject();
                auto self = community.value("channel")
                                .toObject()
                                .value("self")
                                .toObject();
                auto points = self.value("communityPoints")
                                  .toObject()
                                  .value("balance")
                                  .toInt();
                successCallback(points);
                return Success;
            }
            catch (const std::exception &ex)
            {
                failureCallback();
                return Failure;
            }
        })
        .onError([failureCallback](auto /*result*/) {
            // TODO: make better xd
            failureCallback();
        })
        .execute();
}

NetworkRequest GQL::makeRequest(QString payload)
{
    if (this->oauthToken.isEmpty())
    {
        qCDebug(chatterinoTwitch)
            << "GQL::makeRequest called without an oauth token set BabyRage";
        // return boost::none;
    }

    return NetworkRequest("https://gql.twitch.tv/gql/",
                          NetworkRequestType::Post)
        .timeout(5 * 1000)
        .header("Content-Type", "application/json")
        .header("Client-ID", "kimne78kx3ncx6brgo4mv6wki5h1ko")
        .header("Authorization", "OAuth " + this->oauthToken)
        .payload(payload.toUtf8());
}

void GQL::update(QString oauthToken)
{
    this->oauthToken = std::move(oauthToken);
}

void GQL::initialize()
{
    assert(instance == nullptr);

    initializeGQL(new GQL());
}

void initializeGQL(IGQL *_instance)
{
    assert(_instance != nullptr);

    instance = _instance;
}

IGQL *getGQL()
{
    assert(instance != nullptr);

    return instance;
}

}  // namespace chatterino
