#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
    struct Fields {
        TaskHolder<web::WebResponse> m_listener;
        CCLabelBMFont* m_label{nullptr};
    };

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) {
            return false;
        }

        const auto creatorInfoMenu{this->getChildByID("creator-info-menu")};
        if (creatorInfoMenu) {
            const auto verifierLabel{CCLabelBMFont::create(". . .", "goldFont.fnt")};

            verifierLabel->setID("verifier-name_spr");
            verifierLabel->setScale(0.45);

            const auto creatorName{creatorInfoMenu->getChildByID("creator-name")};
            if (creatorName) {
                verifierLabel->setPosition(creatorName->getPositionX(), creatorName->getPositionY() - creatorName->getContentHeight() + 3);
                m_fields->m_label = verifierLabel;

                creatorInfoMenu->addChild(verifierLabel);
            } else {
                log::warn("Could not find creator-name");
            }
        } else {
            log::warn("Could not find creator-info-menu");
        }

        if (m_fields->m_label) {
            if (level -> m_levelID) {
                auto webReq = web::WebRequest();
                std::string url = fmt::format("https://api.aredl.net/v2/api/aredl/levels/{}", level->m_levelID);

                m_fields->m_listener.spawn(
                    webReq.get(url), [this](const web::WebResponse& res) {
                        int status{res.code()};
                        if (status == 200) {
                            auto data{res.json()};
                            if (!data) {
                                m_fields -> m_label->setVisible(false);
                                return;
                            }

                            const auto& parsed = data.unwrap();

                            std::string verifier{parsed["verifications"][0]["submitted_by"]["global_name"].asString().unwrapOr("-")};
                            m_fields->m_label->setString(fmt::format("Verified: {}", verifier).c_str());
                        } else {
                            switch (status) {
                                case 404:
                                    m_fields -> m_label->setVisible(false);
                                    break;
                                case 429:
                                    m_fields -> m_label->setString("Too many requests");
                                    break;
                                default:
                                    m_fields -> m_label->setString("Server connection issues");
                                    log::warn("unexpected serverside issue occurred ({})", status);
                                    break;
                            }
                        }
                    });
            } else {
                m_fields -> m_label->setVisible(false);
            }
        } else {
            log::warn("Label is absent");
        }

        return true;
    }
};