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
        if (!LevelInfoLayer::init(level, challenge)) return false;
        if (!Mod::get()->getSettingValue<bool>("enabled")) return true;
        if (!level -> m_levelID) return true;
        if (m_level->m_demon.value() < 1) return true;
        if (m_level->m_demonDifficulty < 5) return true; // easier than insane demon

        const auto creatorInfoMenu{this->getChildByID("creator-info-menu")};

        if (!creatorInfoMenu) {
            log::warn("Could not find creator-info-menu");
            return true;
        }

        const auto creatorName{creatorInfoMenu->getChildByID("creator-name")};

        if (!creatorName) {
            log::warn("Creator name label was not found");
            return true;
        }

        const auto verifierLabel{CCLabelBMFont::create(". . .", "goldFont.fnt")};

        if (!verifierLabel) {
            log::warn("Failed to create the verifier label");
            return true;
        }

        auto scale{Mod::get()->getSettingValue<float>("scale")};
        auto yoffset{Mod::get()->getSettingValue<float>("y-offset")};
        auto xoffset{Mod::get()->getSettingValue<float>("x-offset")};

        verifierLabel->setID("verifier-name_spr");
        verifierLabel->setScale(scale);

        verifierLabel->setPosition(creatorName->getPositionX() + xoffset, creatorName->getPositionY() - creatorName->getContentHeight() + yoffset);
        m_fields->m_label = verifierLabel;

        creatorInfoMenu->addChild(verifierLabel);

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

        return true;
    }
};