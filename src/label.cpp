#include "label.hpp"

using namespace geode::prelude;

CCLabelBMFont* buildLabel(CCNode* menu) {
    if (!menu) {
        log::warn("Could not find creator-info-menu");
        return nullptr;
    }

    const auto creatorName{menu->getChildByID("creator-name")};

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

    menu->addChild(verifierName);

    return verifierName;
}

CCMenuItemSpriteExtra* buildYTButton(CCNode* menu, CCNode* label, const std::string &url) {
    if (!menu || !label) {
        return nullptr;
    }

    auto scale{Mod::get()->getSettingValue<float>("scale")};

    auto ytButton{
        CCMenuItemExt::createSpriteExtraWithFrameName(
            "gj_ytIcon_001.png", scale, [url](CCMenuItemSpriteExtra *btn) {
                web::openLinkInBrowser(url);
            })
    };

    if (!ytButton) {
        log::warn("Could not create yt button");
        return nullptr;
    }

    ytButton->setID("ytbutton_spr");
    ytButton->setPosition(
        label->getPositionX() + label->getContentWidth() * scale / 2 + 10,
        label->getPositionY());

    menu->addChild(ytButton);
    return ytButton;
}

CCMenuItemSpriteExtra* build2PButton(CCNode* menu, CCNode* label) {
    if (!menu || !label) {
        return nullptr;
    }

    auto scale{Mod::get()->getSettingValue<float>("scale")};

    auto btnText{CCLabelBMFont::create("2P", "bigFont.fnt")};

    if (!btnText) {
        log::warn("not able to create text for sum reason");
        return nullptr;
    }

    auto twoPlrSprite{
        CircleButtonSprite::create(btnText, CircleBaseColor::Green, CircleBaseSize::Tiny)
    };

    if (!twoPlrSprite) {
        log::warn("not able to create sprite for sum reason");
        return nullptr;
    }

    twoPlrSprite->setScale(scale + 0.1f);

    auto twoPlrButton{CCMenuItemExt::createSpriteExtra(twoPlrSprite, nullptr)};

    if (!twoPlrButton) {
        return nullptr;
    }

    twoPlrButton->setID("twoplayerbutton_spr");

    twoPlrButton->setPosition(
        label->getPositionX() - label->getContentWidth() * scale / 2 - 10,
        label->getPositionY());

    menu->addChild(twoPlrButton);
    return twoPlrButton;
}