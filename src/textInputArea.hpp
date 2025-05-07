
class RaZooMTextInputArea : public CCTextInputNode {
protected:
    TextArea* m_textArea;

public:
    static RaZooMTextInputArea* create(float width, float height, const char* placeholder, 
                    float textScale, float lineHeight, const char* fontFile, bool unlimitedLineWidth=true) {

        auto ret = new RaZooMTextInputArea();
        if (ret && ret->init(width, height, placeholder, textScale, lineHeight, fontFile, unlimitedLineWidth)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }


    bool init(float width, float height, const char *placeholder, float textScale, float lineHeight, 
                    const char* fontFile, bool unlimitedLineWidth) {

        if (!CCTextInputNode::init(width, height, placeholder, "(?)", 1, nullptr)) return false;

        ignoreAnchorPointForPosition(true);

        m_textArea = TextArea::create("(?)", fontFile, textScale, (unlimitedLineWidth ? 9999999 : width), ccp(0, 0.5), lineHeight, true);
        addTextArea(m_textArea);
        m_textArea->setAnchorPoint(ccp(0, 0.5));
        m_textArea->setPosition(ccp(-width + 20, 20) / 2);

        auto bg = CCScale9Sprite::create("square02_001.png", { 0, 0, 80, 80 });
        bg->setScale(.5);
        bg->setOpacity(90);
        bg->setContentSize(ccp(width, height) * 2);
        addChild(bg);
        bg->setZOrder(-1);

        if (m_cursor) { // fix cursor pos (visual)
            if (auto ch = m_cursor->getChildByTag(0)) {
                ch->setPositionX(m_textArea->getPositionX());
            }
        }

        m_allowedChars = std::string(m_allowedChars) + "\n";
        m_filterSwearWords = false;

        return true;
    }


    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
        if (!nodeIsVisible(this)) {
            onClickTrackNode(false);
            return false;
        }

        auto size = getContentSize();
        auto pos = convertToNodeSpace(touch->getLocation()) + getAnchorPoint() * size;

        if (pos.x < 0 || pos.x > size.width || pos.y < 0 || pos.y > size.height) {
            onClickTrackNode(false);
            return false;
        }

        return CCTextInputNode::ccTouchBegan(touch, event);
    }

    // // If the sender doesn't want to attach to the IME, return true
    // bool onTextFieldAttachWithIME(CCTextFieldTTF* tField) override {
    //     tField->setString("RaZooM");
    //     auto ret = CCTextInputNode::onTextFieldAttachWithIME(tField);
    //     log::debug("onTextFieldAttachWithIME {}", ret);
    //     return ret;
    // }


    // // If the sender doesn't want to detach from the IME, return true;
    // bool onTextFieldDetachWithIME(CCTextFieldTTF* tField) override {
    //     tField->setString("RaZooM 23");
    //     auto ret = CCTextInputNode::onTextFieldDetachWithIME(tField);
    //     log::debug("onTextFieldDetachWithIME {}", ret);
    //     return ret;
    // }


    void insertTextAtCursor(std::string insertStr, bool onlyIfFocused) {
        if (onlyIfFocused && !m_selected) return;

        auto text = std::string(getString());
        const int cursorPos = m_textField->m_uCursorPos; // -1 means at the very end
        const int pos = (cursorPos < 0 || cursorPos > text.size()) ? text.size() : cursorPos;

        text.insert(pos, insertStr);
        
        if (cursorPos >= 0) {
            m_textField->m_uCursorPos += insertStr.size();
        }

        setString(text);
    }
};
