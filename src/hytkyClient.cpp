#include "hytkyClient.h"

hytkyClient::hytkyClient() {

}

void hytkyClient::setup(std::string hubAddress) {

    olentoClient::setup(hubAddress, "hytky", "sense");

}

