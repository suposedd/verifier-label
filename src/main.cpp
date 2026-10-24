#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/utils/web.hpp>

#include "label.hpp"

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
    struct Fields {
        TaskHolder<web::WebResponse> m_listener;
        CCLabelBMFont *m_label{nullptr};
        std::string m_videoURL{};
    };

    bool init(GJGameLevel *level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;
        if (!Mod::get()->getSettingValue<bool>("enabled")) return true;
        if (!level->m_levelID) return true;
        if (m_level->m_demon.value() < 1) return true;
        if (m_level->m_demonDifficulty < 5) return true;

        const auto creatorInfoMenu{this->getChildByID("creator-info-menu")};
        auto scale{Mod::get()->getSettingValue<float>("scale")};

        m_fields->m_label = buildLabel(this);

        if (!m_fields->m_label) {
            return true;
        }

        auto webReq = web::WebRequest();
        std::string url = fmt::format("https://api.aredl.net/v2/api/aredl/levels/{}", level->m_levelID);

        m_fields->m_listener.spawn(
            webReq.get(url), [this, scale, creatorInfoMenu](const web::WebResponse &res) {
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
                    m_fields->m_videoURL = parsed["verifications"][0]["video_url"].asString().unwrapOr("");

                    m_fields->m_label->setString(fmt::format("Verified: {}", verifier).c_str());

                    if (Mod::get()->getSettingValue<bool>("enable-yt-button") && !m_fields->m_videoURL.empty()) {
                        auto ytButton{
                            CCMenuItemExt::createSpriteExtraWithFrameName(
                                "gj_ytIcon_001.png", scale, [this](CCMenuItemSpriteExtra *btn) {
                                    web::openLinkInBrowser(m_fields->m_videoURL);
                                })
                        };

                        if (!ytButton) {
                            log::warn("Could not create yt button");
                            return;
                        }

                        ytButton->setPosition(
                            m_fields->m_label->getPositionX() + m_fields->m_label->getContentWidth() * scale / 2 + 10,
                            m_fields->m_label->getPositionY());

                        creatorInfoMenu->addChild(ytButton);
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
