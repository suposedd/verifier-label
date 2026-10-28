#pragma once

#include "Geode/Geode.hpp"

using namespace geode::prelude;

enum class FetchStatus{ok, notFound, rateLimited, error};

struct VerificationResult {
    FetchStatus status{FetchStatus::error};
    std::string verifier;
    std::string videoURL;
};

void fetch(TaskHolder<web::WebResponse>& holder, int id, bool isTwoPlayer, std::function<void(VerificationResult)> onResult);