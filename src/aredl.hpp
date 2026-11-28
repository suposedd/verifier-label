#pragma once

#include "Geode/Geode.hpp"
#include "Geode/utils/web.hpp"

using namespace geode::prelude;

enum class FetchStatus{ok, notFound, rateLimited, error};

struct VerificationResult {
    FetchStatus status{FetchStatus::error};
    std::vector<std::string> verifiers;
    std::string videoURL;
};

void fetch(TaskHolder<web::WebResponse> &holder, const std::string& id, bool isTwoPlayer,
            bool isPlatformer,
            const std::function<void(VerificationResult)>& onResult);