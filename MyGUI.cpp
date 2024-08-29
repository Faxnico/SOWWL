void __thiscall SLayout::Init(void)
{
    this->unk_0c = true;

    sowwl_xlink::CStdStr<char> s{};
    const auto firstRootWidget = this->rootWidgets_50.front();

    // ...
    this->WStr(firstRootWidget, "LayoutType", &s);
    if (s.str_00.compare(0, s.str_00.length_14, "2", 1) == 0) { this->unk_20 = 2; }
    else if (s.str_00.compare(0, s.str_00.length_14, "3", 1) == 0) { this->unk_20 = 3; }
    else { this->unk_20 = theLand.terrain_504 != nullptr ? 3 : 1; }

    // ...
    this->WStr(firstRootWidget, "NSDMe", &s);
    if (s.str_00.compare(0, s.str_00.length_14, "1", 1) == 0) {
        this->unk_28 |= 0b100;
        this->unk_2c = CUnitList::MustBeMyID(-1);
    }

    // ...
    this->WStr(firstRootWidget, "NSDNoCourMess", &s);
    if (s.str_00.compare(0, s.str_00.length_14, "1", 1) == 0) {
        this->unk_28 |= 0b1000; // IsNoMsg?
    }

    // ...
    for (auto widget : this->rootWidgets_50) { this->SetClick(widget); }

    // ...
    if ((this->unk_28 & 0b100) == 0) {
        this->unk_2c = theApp.vars_1b8->FindInt(eVarScreenUnitID, -1);
    }

    // ...
    this->WStr(firstRootWidget, "NSDAlign", &s);
    if (s.str_00.compare(0, s.str_00.length_14, "MouseCenter", 11) == 0) {
        this->unk_28 |= 0b1; // IsMenu?
        const auto mousePos = MyGUI::Singleton<MyGUI::InputManager>::getInstance().getMousePosition();
        auto x = std::max(0, mousePos.x - firstRootWidget->width_20 / 2);
        auto y = std::max(0, mousePos.y - firstRootWidget->height_24 / 2);
        if (firstRootWidget->width_20 + x > theApp.screenWidth_08c)
            x = theApp.screenWidth_08c - firstRootWidget->width_20;
        if (firstRootWidget->height_24 + y > theApp.screenHeight_090)
            y = theApp.screenHeight_090 - firstRootWidget->height_24;
        firstRootWidget->setPosition(x, y);   
    }
    else if (s.str_00.compare(0, s.str_00.length_14, "ToolWindow", 10) == 0) {
        if (SLayout::gToolBar != nullptr) {
            this->unk_28 |= 0b10;
            auto x = SLayout::gToolBar->absLeft_28 + SLayout::gToolBar->width_20 - firstRootWidget->width_20;
            auto y = SLayout::gToolBar->absTop_2c - firstRootWidget->height_24;
            firstRootWidget->setPosition(x, y);
        }
    }

    // ...
    this->WStr(firstRootWidget, "ToolBarType", &s);
    if (s.str_00.compare(0, s.str_00.length_14, "1", 1) == 0) {
        MyGUI::types::TSize<int> v67{
            firstRootWidget->width_20,
            firstRootWidget->height_24
        };
        if (firstRootWidget->top_1c + firstRootWidget->height_24 != theApp.screenHeight_090)
        {
            v67.height = theApp.screenHeight_090 - firstRootWidget->top_1c;
            firstRootWidget->setSize(v67);
        }        
    }
    else if (s.str_00.compare(0, s.str_00.length_14, "2", 1) == 0) {
        MyGUI::types::TPoint<int> v67{
            theApp.screenWidth_08c - firstRootWidget->width_20,
            0
        };
        firstRootWidget->setPosition(v67);
    }

    // ...
    this->WStr(firstRootWidget, "NSDSmooth", &s);
    if (s.str_00.length_14 != 0) {
        firstRootWidget->setAlpha(0.f);
        firstRootWidget->setVisible_v0c(true);
        firstRootWidget->castType<MyGUI::Window>(true)->setVisibleSmooth(true);
    }
}

void SLayout::Update(void)
{
    for (const auto& o : this->unk_10) {
        if (o->unk_11 == true) {
            if (o->cond_18 != nullptr) { // SCond *
                const auto v = theApp.vars_1b8->Condition(o->cond_18);
                if (v != o->unk_0c->getVisible()) {
                    o->unk_0c->setVisible(v);                    
                }
            }
        }
    }
}

void SLayout::SetClick(MyGUI::Widget *widget)
{
    sowwl_xlink::CStdStr<char> s{};
    SLayout::WStr(widget, "NSDNoClickComm", &s);
    if (s.str_00.length_14 == 0) {
        SLayout::WStr(widget, "OnRightClick", &s);
    }
    if (s.str_00.length_14 != 0) {
        widget->eventMouseButtonReleased += MyGUI::newDelegate(this, &SLayout::notifyMouseRelease);
    }
    this->RunComm(widget, "COMMAND");
    // ...
    SLayout::WStr(widget, "NSDGridSize", &s);
    if (s.str_00.length_14 != 0) {
        int cols, rows;
        CUtil::ParseGet(s, ' ', cols, 1);
        CUtil::ParseGet(s, ' ', rows, 1);
        // Calculate horizontal and vertical divisions based on the parsed values.
        const auto horDiv = (widget->width_20 - 2 * cols + 2) / cols; 
        const auto vertDiv = (widget->height_24 - 2 * rows + 2) / rows;
        // Determine the minimum division value.
        const auto minDiv = std::min(horDiv, vertDiv);
        // Calculate the spacing or padding based on the minDiv value.
        const auto horSpacing = (widget->width_20 - cols * minDiv) / (cols - 1);
        const auto vertSpacing = (widget->height_24 - rows * minDiv) / (rows - 1);
        // Store the calculated values in the layout
        layout->minDiv_00 = minDiv;
        layout->horSpacing_04 = horSpacing;
        layout->vertSpacing_08 = vertSpacing;
    }
    // ...
    SLayout::WStr(widget, "NSDGrid", &s);
    if (s.str_00.length_14 != 0) {
        int x, y;
        CUtil::ParseGet(s, ' ', x, 1);
        CUtil::ParseGet(s, ' ', y, 1);
        widget->setSize(MyGUI::types::TSize<int>{ layout->minDiv_00, layout->minDiv_00 });
        widget->setPosition(MyGUI::types::TPoint<int>{
            x * layout->horSpacing_04 + layout->minDiv_00 * x,
            y * layout->vertSpacing_08 + layout->minDiv_00 * y
        });
    }
    // ...
    SLayout::WStr(widget, "NSDSpecial", &s);
    if (s.str_00.compare(0, s.str_00.length_14, "Compass", 7) == 0) {
        MyGUI::types::TSize<int> widgetSize;
        widgetSize.width = widgetSize.height = std::max(widget->width_20, widget->height_24);        
        widget->setSize(widgetSize);
        auto parentWidget = widget->getParent();
        widget->setPosition(MyGUI::types::TPoint<int>{
            (parentWidget->width_20 - widgetSize.width) / 2,
            (parentWidget->height_24 - widgetSize.height) / 2
        });
    }
    // ...
    bool bTip{ true };
    if (!widget->/*baseWidgetInput_50.*/getNeedToolTip()) {
        SLayout::WStr(widget, "NSDTip", &s);
        bTip = s.str_00.length_14 != 0;
    }
    if (bTip) {
        widget->setNeedToolTip(true);
        widget->eventToolTip += MyGUI::newDelegate(this, &SLayout::notifyToolTipW);

        auto o = CMGObj::CreateNewObj(widget, this);
        if (o != nullptr) { this->unk_10.push_back(o); o->Init(); }
    }
    // ...
    SLayout::WStr(widget, "ToolTip", &s);
    if (s.str_00.length_14 != 0) {
        layout->tooltip_4c = new MyGUI::ToolTip(s.str_00.c_str());
        layout->tooltip_4c->hide();        
    }
    // ...
    for (unsigned int i = 0; i != widget->getChildCount(); ++i) {
        this->SetClick(widget->getChildAt(i));
    }
}

static CMGObj* CMGObj::CreateNewObj(MyGUI::Widget* widget, SLayout* layout) {
    if (widget->castType<MyGUI::Button>()) {
        return new CMGButt(widget, layout);
    }
    if (widget->castType<MyGUI::HyperTextBox>()) {
        return new CMGHyper(widget, layout);
    }
    if (widget->castType<MyGUI::TreeControl>()) {
        return new CMGTree(widget, layout);
    }
    if (widget->castType<MyGUI::ListBox>()) {
        return CMGList::Create(widget, layout);
    }
    if (widget->castType<MyGUI::ProgressBar>()) {
        return new CMGProg(widget, layout);
    }
    if (widget->castType<MyGUI::MultiListBox>()) {
        return new CMGMulti(widget, layout);
    }
    if (widget->castType<MyGUI::ComboBox>()) {
        return new CMGCombo(widget, layout);
    }
    if (widget->castType<MyGUI::EditBox>()) {
        return new CMGEdit(widget, layout);
    }
    if (widget->castType<MyGUI::ScrollBar>()) {
        return new CMGScroll(widget, layout);
    }
    if (widget->castType<MyGUI::ImageBox>()) {
        return new CMGImage(widget, layout);
    }
    sowwl_xlink::CStdStr<char> s{};
    SLayout::WStr(widget, "CheckList", &s);
    if (widget->castType<MyGUI::TextBox>()) {
        if (s.str_00.length_14 == 0) {
            return new CMGText(widget, layout);
        }        
    }
    if (s.str_00.length_14 != 0) {
        return new CMGCheckList(widget, new MyGUI::CheckListWindow(s.str_00.c_str(), widget), layout);
    }  
    
    return nullptr;
}



void __thiscall CNSDMyGUI::Init(void)
{
    mBaseMrg_124 = std::make_unique<CNSDBaseMgr>();
    mBaseMrg_124->Direct3D_4c = thePR.Direct3D_026D6;
    mBaseMrg_124->Direct3DDevice_50 = thePR.Direct3DDevice_026DA;
    mBaseMrg_124->hInstance_54 = theApp.hInstance_1a8;
    mBaseMrg_124->hWnd_48 = theApp.mainWnd_0f0->hWnd_30;    
    createGui(CUtil::m_homedir + "Base/Layout", CUtil::m_tmpdir + "MyGUI.log");
    MyGUI::Singleton<MyGUI::ResourceManager>::getInstance().load("SOWWLResources.xml");
    CWarApp::SetLanguage();
}

void __thiscall CNSDMyGUI::Free(void)
{
    destroyGui();
    delete MyGUI::Singleton<class tools::MsgBoxMgr>::getInstancePtr();
    mBaseMrg_124.reset();
}

void __thiscall CNSDMyGUI::Draw(void)
{
    if (!mBaseMrg_124->layouts_98.empty())
    {
        thePR.SetD3DPipeline(1);
        thePR.NewFrame();
        thePR.SetColor(thePR.Graphics.MakeColor(0, 0, 0));
        thePR.GameEngine_7820C.Viewport_A1F4.Clear();
        thePR.RenderFrame();
        thePR.Flip();
    }
}

void __thiscall CNSDMyGUI::Render(void)
{
    for (auto layout : mBaseMrg_124->layouts_98) {
        layout->Update();
    }

    begin();
    mBaseMrg_124->render();
    end();
}

void __thiscall CNSDMyGUI::begin(void)
{
    auto d3dDevice = thePR.Direct3DDevice_026DA;

    // Backup Render States
    d3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &alphaBlendEnable_00);  // 27
    d3dDevice->GetRenderState(D3DRS_SRCBLEND, &srcBlend_38);                  // 19
    d3dDevice->GetRenderState(D3DRS_DESTBLEND, &destBlend_3C);                // 20
    d3dDevice->GetRenderState(D3DRS_ZWRITEENABLE, &zWriteEnable_40);          // 14
    d3dDevice->GetRenderState(D3DRS_ZENABLE, &zEnable_44);                    // 7
    d3dDevice->GetRenderState(D3DRS_LIGHTING, &lighting_48);                  // 137
    d3dDevice->GetRenderState(D3DRS_CULLMODE, &cullMode_4C);                  // 22
    d3dDevice->GetRenderState(D3DRS_ALPHATESTENABLE, &alphaTestEnable_50);    // 15

    // Backup Texture Stage States (Stage 0)
    d3dDevice->GetTextureStageState(0, D3DTSS_COLOROP, &colorOp_04);          // 1
    d3dDevice->GetTextureStageState(0, D3DTSS_COLORARG0, &colorArg0_08);      // 26
    d3dDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &colorArg1_0C);      // 2
    d3dDevice->GetTextureStageState(0, D3DTSS_COLORARG2, &colorArg2_10);      // 3
    d3dDevice->GetTextureStageState(0, D3DTSS_ALPHAOP, &alphaOp_14);          // 4
    d3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG0, &alphaArg0_18);      // 27
    d3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG1, &alphaArg1_1C);      // 5
    d3dDevice->GetTextureStageState(0, D3DTSS_ALPHAARG2, &alphaArg2_20);      // 6

    // Backup Sampler States (Stage 0)
    d3dDevice->GetSamplerState(0, D3DSAMP_MINFILTER, &minFilter_24);          // 6
    d3dDevice->GetSamplerState(0, D3DSAMP_MAGFILTER, &magFilter_28);          // 5
    d3dDevice->GetSamplerState(0, D3DSAMP_MIPFILTER, &mipFilter_2C);          // 7
    d3dDevice->GetSamplerState(0, D3DSAMP_ADDRESSU, &addressU_30);            // 1
    d3dDevice->GetSamplerState(0, D3DSAMP_ADDRESSV, &addressV_34);            // 2

    // Backup FVF (Flexible Vertex Format)
    d3dDevice->GetFVF(&fvf_54);

    // Backup Transformations
    d3dDevice->GetTransform(D3DTS_WORLD, &worldMatrix_64[0][0]);             // 256
    d3dDevice->GetTransform(D3DTS_VIEW, &viewMatrix_A4[0][0]);               // 2
    d3dDevice->GetTransform(D3DTS_PROJECTION, &projectionMatrix_E4[0][0]);   // 3
}

void __thiscall CNSDMyGUI::end(void)
{
    auto d3dDevice = thePR.Direct3DDevice_026DA;
    
    // Apply Render States
    d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, alphaBlendEnable_00);
    d3dDevice->SetRenderState(D3DRS_SRCBLEND, srcBlend_38);
    d3dDevice->SetRenderState(D3DRS_DESTBLEND, destBlend_3C);
    d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, zWriteEnable_40);
    d3dDevice->SetRenderState(D3DRS_ZENABLE, zEnable_44);
    d3dDevice->SetRenderState(D3DRS_LIGHTING, lighting_48);
    d3dDevice->SetRenderState(D3DRS_CULLMODE, cullMode_4C);
    d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, alphaTestEnable_50);

    // Apply Texture Stage States (Stage 0)
    d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, colorOp_04);
    d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG0, colorArg0_08);
    d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, colorArg1_0C);
    d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, colorArg2_10);
    d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, alphaOp_14);
    d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG0, alphaArg0_18);
    d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, alphaArg1_1C);
    d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, alphaArg2_20);

    // Apply Sampler States (Stage 0)
    d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, minFilter_24);
    d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, magFilter_28);
    d3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, mipFilter_2C);
    d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, addressU_30);
    d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, addressV_34);

    // Apply FVF (Flexible Vertex Format)
    d3dDevice->SetFVF(fvf_54);

    // Apply Transformations
    d3dDevice->SetTransform(D3DTS_WORLD, &worldMatrix_64[0][0]);
    d3dDevice->SetTransform(D3DTS_VIEW, &viewMatrix_A4[0][0]);
    d3dDevice->SetTransform(D3DTS_PROJECTION, &projectionMatrix_E4[0][0]);
}

void CNSDMyGUI::LoadLayout(std::string layoutName)
{
    this->CloseScreen();

    const auto file = layoutName + ".layout";
    auto rootWidgets = MyGUI::LayoutManager::getInstance().loadLayout(file);

    auto layout = new SLayout();
    layout->rootWidgets_50 = rootWidgets; // TODO: std::move
    layout->file_30 = file;

    const auto us = layout->rootWidgets_50.front().userData_38.getUserString("ToolBarType");
    var_7d = true;
    if (us.empty())
    {
        const auto us = layout->rootWidgets_50.front().userData_38.getUserString("NSDAlign");
        if (us.compare(0, us.length_14, "ToolWindow", 10)) { var_7d = false; }
    }

    if (var_7d)
    {
        if (!layout->rootWidgets_50.empty())
        {
            mBaseMrg_124->layouts_98.insert(layout);

            if (theApp.warRenderLoop_e2 && !theApp.closingLevel_e7) {
                this->ManageModalWidget(layout->rootWidgets_50[0]);
            }

            layout->Init();
            this->baseMgr_124->_windowResized(false);

            if (this->IsActive() == 3 && !theApp.IsMulti()) {
                theApp.paused_0d = true;
            }
        }
        else
        {
            delete layout;
        }
    }
    else
    {
        delete layout;
        this->LoadToolbar(layoutName);
    }
}

void CNSDMyGUI::ManageModalWidget(MyGUI::Widget* widget) {
    auto& inputManager = MyGUI::Singleton<MyGUI::InputManager>::getInstance();
    auto& msgBoxMgr = tools::MsgBoxMgr::getInstance();

    auto& modalWidgets = inputManager.mVectorModalRootWidget;

    if (msgBoxMgr.hasAny()) {
        const auto messageCount = msgBoxMgr.mMessages.size();
        const auto widgetCount = modalWidgets.size();

        if (widgetCount >= messageCount) {
            // Find the appropriate position to insert the widget
            auto it = modalWidgets.begin();
            std::advance(it, widgetCount - messageCount);

            // Insert the widget in the calculated position
            modalWidgets.insert(it, widget);

            inputManager.removeWidgetModal(widget);
        }
    } else {
        // If there are no messages or not enough widgets, add widget to modal
        inputManager.addWidgetModal(widget);
    }
}

void CNSDMyGUI::LoadToolbar(std::string toolbarName)
{
    for (const auto layout : this->mBaseMrg_124.toolbars_a4)
    {
        const auto us = layout->rootWidgets_50.front().userData_38.getUserString("NSDSpecial");

        auto strcmpres = std::char_traits<char>::compare(us.c_str(), "keep", std::min(us.length_14, 4));
        //us.compare(0, us.length_14, "keep", 4);
        bool closeToolbar = false;
        if (strcmpres != 0 || us.length_14 < 4 || us.length_14 != 4) {
            closeToolbar = layout->file_30.compare(0, us.length_14, toolbarName.c_str(), toolbarName.length_14) == 0;
        }
        if (closeToolbar) { this->CloseToolbar(); return; }
    }

    this->CloseToolbar();
    auto layout = new Layout();
    mBaseMrg_124->toolbars_a4.insert(layout);
    const auto file = toolbarName + ".layout";
    auto rootWidgets = MyGUI::LayoutManager::getInstance().loadLayout(file);
    layout->rootWidgets_50 = rootWidgets; // TODO: std::move
    layout->file_30 = file;
    layout->Init();
    this->baseMgr_124->_windowResized(false);
    if (mBaseMrg_124->toolbars_a4.size() == 1) {
        SLayout::gToolBar = mBaseMrg_124->toolbars_a4.front();
    }
}

void CNSDMyGUI::PopScreen(std::string name) // TODO
{
    auto layout = new Layout();
    const auto file = name + ".layout";
    auto rootWidgets = MyGUI::LayoutManager::getInstance().loadLayout(file);
    layout->rootWidgets_50 = rootWidgets; // TODO: std::move
    layout->file_30 = file;

    if (layout->rootWidgets_50.empty()) {
        delete layout;
        return;
    }

    const auto us = layout->rootWidgets_50.front().userData_38.getUserString("ToolBarType");
    var_79 = true;
    if (us.empty())
    {
        const auto us = layout->rootWidgets_50.front().userData_38.getUserString("NSDAlign");
        if (us.compare(0, us.length_14, "ToolWindow", 10)) { var_79 = false; }
    }

    if (var_79)
    {
        delete layout;
        this->LoadToolbar(name);
    }
    else
    {
        if (!mBaseMrg_124->layouts_98.empty() && layout->rootWidgets_50[0].getLayer().getName() == mBaseMrg_124->layouts_98[0].getLayer().getName()) {
            for (const auto w : mBaseMrg_124->layouts_98.front()->rootWidgets_50) {
                w->setVisible_v0c(false);
            }
        }
        else {
             this->ManageModalWidget(layout->rootWidgets_50[0]);
        }

        mBaseMrg_124->layouts_98.insert(layout);
        if (theApp.warRenderLoop_e2 && !theApp.closingLevel_e7) {
            this->ManageModalWidget(layout->rootWidgets_50[0]);
        }

        layout->Init();
        this->baseMgr_124->_windowResized(false);

        if (this->IsActive() == 3 && !theApp.IsMulti()) {
            theApp.paused_0d = true;
        }        
    }
}

void CNSDMyGUI::CloseToolbar(std::string name)
{
    while (mBaseMrg_124->toolbars_a4.size > 1)
    {
        auto layout = mBaseMrg_124->toolbars_a4.front();
        sowwl_xlink::CStdStr<char> s{};
        SLayout::WStr(layout->rootWidgets_50[0], "ToolBarType", &s);
        if (s.str_00.length_14 != 0)
            break;
        delete layout;
        mBaseMrg_124->toolbars_a4.pop_front();
    }
}


void CNSDMyGUI::CloseScreen(void)
{
    if (mBaseMrg_124 != nullptr)
    {
        if (!mBaseMrg_124->layouts_98.empty()) {
            auto layout = mBaseMrg_124->layouts_98.front();
            layout->unk_24 = true;
            auto& inputManager = MyGUI::Singleton<MyGUI::InputManager>::getInstance();
            if (theApp.warRenderLoop_e2) {
                inputManager.removeWidgetModal(layout->rootWidgets_50[0]);
            }
            delete layout;
            mBaseMrg_124->layouts_98.pop_front();
            for (const auto w : mBaseMrg_124->layouts_98.front()->rootWidgets_50) {
                w->setVisible_v0c(true);
            }

            if (CUtil::m_camp == nullptr) {
                theApp.Pause(this->IsActive() == 3);
            }
        }
    }
}

void CNSDMyGUI::CloseAll(void)
{
    for (const auto &l : mBaseMrg_124->layouts_98) {
        delete l;
    }
    mBaseMrg_124->layouts_98.clear();

    for (const auto &l : mBaseMrg_124->toolbars_a4) {
        delete l;
    }
    mBaseMrg_124->toolbars_a4.clear();

    SLayout::gToolBar = nullptr;
}




void CWarApp::SetLanguage(void)
{
    auto& langMgr = MyGUI::Singleton<MyGUI::LanguageManager>::getInstance();
    std::vector<std::string> langList;
    langMgr.getLanguageList(langList);
    const auto optLang = opts[sowwl_xlink::EOptionValue::eOVLanguage].val;
    if (optLang >= 0 && optLang < langList.size()) {
        langMgr.setCurrentLanguage(langList[optLang]);
    }
}

void CWarApp::ResetGUI(void)
{
    mMyGUI_a8->CloseAll();
    if (mMyGUI_a8->mBaseMgr_124 != nullptr)
        mMyGUI_a8->mBaseMgr_124->destroyGui();

    delete MyGUI::Singleton<tools::MsgBoxMgr>::getInstancePtr();

    delete mMyGUI_a8->mBaseMgr_124;
    mMyGUI_a8->mBaseMgr_124 = nullptr;

    if (mMyGUI_a8 != nullptr) { // why again??
        delete mMyGUI_a8->mBaseMgr_124;
        delete mMyGUI_a8;
    }

    mMyGUI_a8 = new CNSDMyGUI();
    mMyGUI_a8->Init();
}
