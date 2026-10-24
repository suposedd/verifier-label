#include "label.hpp"

using namespace geode::prelude;

CCLabelBMFont* buildLabel(CCNode* layer) {
    const auto creatorInfoMenu{layer->getChildByID("creator-info-menu")};

    if (!creatorInfoMenu) {
        log::warn("Could not find creator-info-menu");
        return nullptr;
    }

    const auto creatorName{creatorInfoMenu->getChildByID("creator-name")};

    if (!creatorName) {
        log::warn("Creator name label was not found");
        return nullptr;
    }

    const auto verifierName{CCLabelBMFont::create(". . .", "goldFont.fnt")};

    if (!verifierName) {
        log::warn("Failed to create the verifier label");
        return nullptr;
    }

    auto scale{Mod::get()->getSettingValue<float>("scale")};
    auto yoffset{Mod::get()->getSettingValue<float>("y-offset")};
    auto xoffset{Mod::get()->getSettingValue<float>("x-offset")};

    verifierName->setID("verifier-name_spr");
    verifierName->setScale(scale);

    verifierName->setPosition(creatorName->getPositionX() + xoffset,
                              creatorName->getPositionY() - creatorName->getContentHeight() + yoffset);

    creatorInfoMenu->addChild(verifierName);

    return verifierName;
}