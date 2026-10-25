#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/utils/web.hpp>

#include "label.hpp"

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
    struct Fields {
        TaskHolder<web::WebResponse> m_listener;
        CCLabelBMFont *m_label{nullptr};
        CCLabelBMFont *m_twoPlayerLabel{nullptr};
    };

    bool init(GJGameLevel *level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        if (!Mod::get()->getSettingValue<bool>("enabled")) return true;
        if (!level->m_levelID) return true;
        if (m_level->m_demon.value() < 1) return true;
        if (m_level->m_demonDifficulty < 5) return true;

        auto creatorInfoMenu{this->getChildByID("creator-info-menu")};

        m_fields->m_label = buildLabel(creatorInfoMenu);

        if (!m_fields->m_label) {
            return true;
        }

        auto webReq = web::WebRequest();
        std::string url = fmt::format("https://api.aredl.net/v2/api/aredl/levels/{}", level->m_levelID);

        m_fields->m_listener.spawn(
            webReq.get(url), [this, creatorInfoMenu, level](const web::WebResponse &res) {
                int status{res.code()};
                if (status == 200) {
                    auto data{res.json()};
                    if (!data) {
                        m_fields->m_label->setVisible(false);
                        return;
                    }

                    const auto &parsed = data.unwrap();

                    std::string verifier{
                        parsed["verifications"][0]["submitted_by"]["global_name"].asString().unwrapOr("-")
                    };
                    std::string videoURL = parsed["verifications"][0]["video_url"].asString().unwrapOr("");

                    m_fields->m_label->setString(fmt::format("Verified: {}", verifier).c_str());

                    if (Mod::get()->getSettingValue<bool>("enable-yt-button") && !videoURL.empty()) {
                        buildYTButton(creatorInfoMenu, m_fields->m_label, videoURL);
                    }
                } else {
                    switch (status) {
                        case 404:
                            m_fields->m_label->setVisible(false);
                            break;
                        case 429:
                            m_fields->m_label->setString("Too many requests");
                            break;
                        default:
                            m_fields->m_label->setString("Server connection issues");
                            log::warn("unexpected serverside issue occurred ({})", status);
                            break;
                    }
                }
            });

        return true;
    }
};
