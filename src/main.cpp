#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/CustomizeObjectLayer.hpp>
#include <Geode/modify/CCTextInputNode.hpp>
#include <Geode/modify/EditorUI.hpp>

#include "textInputArea.hpp"



// keybinds
#ifdef GEODE_IS_DESKTOP
	#include <geode.custom-keybinds/include/Keybinds.hpp>
	$execute {
		keybinds::BindManager::get()->registerBindable({
			"newline"_spr, "Insert new line",
			"Insert a new line symbol in text object",
			{ keybinds::Keybind::create(KEY_Enter, keybinds::Modifier::None) },
			"New Line"
		});
	}
#endif // GEODE_IS_DESKTOP



class $modify(MyCustomizeObjectLayer, CustomizeObjectLayer) {

	void initKeybinds() {
		#ifdef GEODE_IS_DESKTOP
			this->template addEventListener<keybinds::InvokeBindFilter>([this](keybinds::InvokeBindEvent* event) {
				if (event->isDown()) {
					onNewLineBtn(nullptr);
					return ListenerResult::Stop;
				}
				return ListenerResult::Propagate;
			}, "newline"_spr);
		#endif // GEODE_IS_DESKTOP
	}


	bool init(GameObject *p0, CCArray *p1) {
		if(!CustomizeObjectLayer::init(p0, p1)) return false;
		
		if (!m_textInput) return true;

		// keybinds 
		initKeybinds();

		// resolve font
		auto fontIndex = LevelEditorLayer::get()->m_levelSettings->m_fontIndex;
		std::string fontFile;

		if (fontIndex <= 0) {
			fontFile = "bigFont.fnt";
		} else if (fontIndex < 10) {
			fontFile = "gjFont0" + std::to_string(fontIndex) + ".fnt";
		} else {
			fontFile = "gjFont" + std::to_string(fontIndex) + ".fnt";
		}

		// setup my input
		auto inp = RaZooMTextInputArea::create(250, 116, "Text...", 0.5, 17.5, fontFile.c_str());
		inp->setLabelPlaceholderColor(ccc3(120, 170, 240));

		m_mainLayer->addChild(inp);
		inp->setPosition(ccp(284.5, 177));
		inp->setZOrder(20);
		
		inp->setString(m_textInput->getString());
		inp->setVisible(m_textInput->isVisible());
		inp->m_delegate = this;
		inp->setTag(1);
		inp->setID("text-input");
		inp->setUserObject("kerning-refresh-fix"_spr, CCBool::create(true));
		m_textInput->removeFromParent();
		m_textInput = inp;

		inp->m_kerningAmount = m_kerningAmount;
		inp->refreshLabel();

		#ifndef GEODE_IS_DESKTOP
			// add 'newline' button
			if (auto menu = m_mainLayer->getChildByID("clear-text-menu")) {
				auto btnSpr = CircleButtonSprite::create(
					CCSprite::create("newline.png"_spr), 
					CircleBaseColor::Blue,
					CircleBaseSize::Medium
				);
				btnSpr->setScale(0.557);
				menu->addChildAtPosition(CCMenuItemSpriteExtra::create(
					btnSpr, this, menu_selector(MyCustomizeObjectLayer::onNewLineBtn)
				), Anchor::Center, ccp(0, -35));
			}
		#endif // GEODE_IS_DESKTOP

		// rearrange other elements
		if (auto slider = m_mainLayer->getChildByID("kerning-slider")) {
			slider->setPosition(ccp(360, 100));
		}
		if (auto label = m_mainLayer->getChildByID("kerning-label")) {
			label->setPosition(ccp(190, 102));
		}
		if (auto trashCan = m_mainLayer->getChildByID("clear-text-menu")) {
			trashCan->setPositionX(436);
		}
		if (auto bg = m_mainLayer->getChildByID("text-input-bg")) {
			static_cast<CCNodeRGBA*>(bg)->setOpacity(0);
		}

		return true;
	}


	void onNewLineBtn(CCObject*) {
		if (auto inp = typeinfo_cast<RaZooMTextInputArea*>(m_textInput)) {
			inp->insertTextAtCursor("\n", true);
		}
	}
};



class $modify(EditorUI) {
	static void onModify(auto& self) {
		(void) self.setHookPriorityPre("EditorUI::onPlaytest", Priority::VeryEarlyPre);
    }

	void onPlaytest(CCObject* sender) {
		if (!CCScene::get()->getChildByID("CustomizeObjectLayer")) {
			EditorUI::onPlaytest(sender);
		}
	}
};



class $modify(CCTextInputNode) {
	void refreshLabel() {
		CCTextInputNode::refreshLabel();

		if (getUserObject("kerning-refresh-fix"_spr)) {
			if (auto lines = m_textArea->m_label->m_lines) {
				for (int i = 0; i < lines->count(); i++) {
					auto lab = static_cast<CCLabelBMFont*>(lines->objectAtIndex(i));
					lab->setExtraKerning(m_kerningAmount);
					lab->updateLabel();
				}
			}
		}
	}
};
