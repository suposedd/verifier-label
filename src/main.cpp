#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) {
            return false;
        }

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