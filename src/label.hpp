#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

CCLabelBMFont* buildLabel(CCNode* menu);
CCMenuItemSpriteExtra* buildYTButton(CCNode* menu, CCNode* label, const std::function<void()>& onClick);
CCMenuItemSpriteExtra* build2PButton(CCNode* menu, CCNode* label, const std::function<void()>& onClick);