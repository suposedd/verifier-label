#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/utils/web.hpp>

#include "label.hpp"
#include "aredl.hpp"

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
    struct Fields {
        TaskHolder<web::WebResponse> m_listener;
        TaskHolder<web::WebResponse> m_twoPlayerListener;

        VerificationResult m_solo;
        VerificationResult m_twoPlayer;
        bool m_showingTwoPlayer{false};

        CCNode* m_menu{nullptr};
        CCMenuItemSpriteExtra *m_button{nullptr};
        CCLabelBMFont *m_label{nullptr};
        CCMenuItemSpriteExtra *m_ytButton{nullptr};
        CCMenuItemSpriteExtra *m_twoPlayerButton{nullptr};
    };

    const VerificationResult &currentResult() {
        return m_fields->m_showingTwoPlayer ? m_fields->m_twoPlayer : m_fields->m_solo;
    }

    bool init(GJGameLevel *level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        if (!Mod::get()->getSettingValue<bool>("enabled")) return true;
        if (!level->m_levelID) return true;
        if (m_level->m_demon.value() < 1) return true;
        if (m_level->m_demonDifficulty < 5) return true;

        auto creatorInfoMenu{this->getChildByID("creator-info-menu")};

        m_fields->m_button = buildLabel(creatorInfoMenu, m_level->m_demonDifficulty == 5, [this] {
            FLAlertLayer::create("Verifiers",
                fmt::format("{}", fmt::join(currentResult().verifiers, "\n")),
                "CLOSE")->show();
        });

        if (!m_fields->m_button) {
            return true;
        }

        m_fields->m_label = typeinfo_cast<CCLabelBMFont *>(m_fields->m_button->getNormalImage());
        m_fields->m_menu = creatorInfoMenu;

        if (!m_fields->m_label) {
            return true;
        }

        if (!m_fields->m_menu) {
            return true;
        }

        if (Mod::get()->getSettingValue<bool>("enable-yt-button")) {
            m_fields->m_ytButton = buildYTButton(m_fields->m_menu, m_fields->m_button, [this] {
               web::openLinkInBrowser(currentResult().videoURL);
            });

            if (m_fields->m_ytButton) {
                m_fields->m_ytButton->setVisible(false);
            }
        }

        fetch(m_fields->m_listener, std::to_string(level->m_levelID), false, level->isPlatformer(),
            [this, level] (VerificationResult result) {
                m_fields->m_solo = result;
                refresh();

                if (m_level->m_twoPlayerMode) {
                    fetch(m_fields->m_twoPlayerListener, std::to_string(level->m_levelID), true, level->isPlatformer(),
                        [this](VerificationResult result) {
                            m_fields->m_twoPlayer = result;

                            if (result.status != FetchStatus::ok) return;

                            if (m_fields->m_solo.status == FetchStatus::notFound) {
                                m_fields->m_showingTwoPlayer = true;
                                refresh();
                                return;
                            }

                            m_fields->m_twoPlayerButton = build2PButton(m_fields->m_menu, m_fields->m_button, [this] {
                                m_fields->m_showingTwoPlayer = !m_fields->m_showingTwoPlayer;
                                refresh();
                            });
                        });
                }
            });

        return true;
    }

    void refresh() {
        auto label{m_fields->m_label};
        auto button{m_fields->m_button};
        bool twoPlayer{m_fields->m_showingTwoPlayer};
        const auto &result{currentResult()};
        bool showYT{false};

        switch (result.status) {
            case FetchStatus::ok: {
                label->setVisible(true);

                if (!twoPlayer) {
                    label->setString(fmt::format("Verified: {}", result.verifiers[0]).c_str());
                } else {
                    if (result.verifiers.size() > 2) {
                        label->setString(fmt::format("(2P) {} +{}",
                            result.verifiers[0],
                            result.verifiers.size() - 1).c_str());
                    } else {
                        label->setString(fmt::format("(2P) {}", fmt::join(result.verifiers, " & ")).c_str());
                    }
                }

                showYT = !result.videoURL.empty();
                break;
            }
            case FetchStatus::notFound: {
                label->setVisible(false);
                break;
            }
            case FetchStatus::rateLimited: {
                label->setVisible(true);
                label->setString("Too many requests");
                break;
            }
            case FetchStatus::error: {
                label->setVisible(true);
                label->setString("AREDL connection issues");
                break;
            }
        }

        button->setContentSize(label->getScaledContentSize());
        label->setPosition(button->getContentSize() / 2);
        button->setEnabled(twoPlayer && result.status == FetchStatus::ok && result.verifiers.size() > 2);

        float halfWidth{button->getScaledContentWidth() / 2};

        if (m_fields->m_ytButton) {
            m_fields->m_ytButton->setVisible(showYT);
            m_fields->m_ytButton->setPositionX(button->getPositionX() + halfWidth + 10);
        }

        if (m_fields->m_twoPlayerButton) {
            if (auto text{typeinfo_cast<CCLabelBMFont*>(m_fields->m_twoPlayerButton->getChildByIDRecursive("mode-text"_spr))}) {
                text->setString(twoPlayer ? "1P" : "2P");
            }

            m_fields->m_twoPlayerButton->setPositionX(button->getPositionX() - halfWidth - 10);
        }
    }
};
