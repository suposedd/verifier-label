#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
    struct Fields {
        TaskHolder<web::WebResponse> m_listener;
    };

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) {
            return false;
        }

        // test
        const std::string url = "https://api.aredl.net/v2/api/aredl/levels/26681070";

        auto webReq = web::WebRequest();

        m_fields->m_listener.spawn(
            webReq.get(url), [](web::WebResponse res) {
                log::info("{}", res.string().unwrapOr("FAILED TO FETCH"));
            });

        if (const auto creatorInfoMenu{this->getChildByID("creator-info-menu")}) {
            const auto verifierLabel{CCLabelBMFont::create("Verified: nobody", "goldFont.fnt")};

            verifierLabel->setID("verifier-name_spr");
            verifierLabel->setScale(0.45);

            if (const auto creatorName{creatorInfoMenu->getChildByID("creator-name")}) {
                verifierLabel->setPosition(creatorName->getPositionX(), creatorName->getPositionY() - creatorName->getContentHeight() + 3);
                creatorInfoMenu->addChild(verifierLabel);
            } else {
                log::warn("Could not find creator-name");
            }
        } else {
            log::warn("Could not find creator-info-menu");
        }

        return true;
    }
};