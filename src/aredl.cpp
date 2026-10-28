#include "aredl.hpp"

#include "Geode/utils/web.hpp"

using namespace geode::prelude;

void fetch(TaskHolder<web::WebResponse> &holder, int id, bool isTwoPlayer,
           std::function<void(VerificationResult)> onResult) {
    std::string url = fmt::format("https://api.aredl.net/v2/api/aredl/levels/{}", id);

    auto webReq = web::WebRequest();

    holder.spawn(
        webReq.get(url), [onResult](const web::WebResponse &res) {
            VerificationResult result{};
            int status{res.code()};

            switch (status) {
                case 200: {
                    auto data{res.json()};
                    if (!data) {
                        log::warn("could not parse aredl response");
                        return;
                    }

                    const auto &parsed = data.unwrap();

                    result.verifier = parsed["verifications"][0]["submitted_by"]["global_name"].asString().unwrapOr("-");
                    result.videoURL = parsed["verifications"][0]["video_url"].asString().unwrapOr("");
                    result.status = FetchStatus::ok;
                    break;
                }
                case 404: {
                    result.status = FetchStatus::notFound;
                    break;
                }
                case 429: {
                    result.status = FetchStatus::rateLimited;
                    break;
                }
                default:
                    log::warn("unexpected serverside issue occurred ({})", status);
                    break;
            }

            onResult(result);
        });
}
