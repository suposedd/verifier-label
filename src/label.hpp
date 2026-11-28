#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

CCMenuItemSpriteExtra *buildLabel(CCNode *menu, bool isLegacy, const std::function<void()> &onClick);
CCMenuItemSpriteExtra* buildYTButton(CCNode* menu, CCNode* label, const std::function<void()>& onClick);
CCMenuItemSpriteExtra* build2PButton(CCNode* menu, CCNode* label, const std::function<void()>& onClick);