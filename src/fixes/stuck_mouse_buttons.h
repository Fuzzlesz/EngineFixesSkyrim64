#pragma once

namespace Fixes::StuckMouseButtons
{
    namespace detail
    {
        class MenuOpenCloseEventSink : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
        {
        public:
            static MenuOpenCloseEventSink* GetSingleton()
            {
                static MenuOpenCloseEventSink singleton;
                return &singleton;
            }

        private:
            MenuOpenCloseEventSink() = default;
            MenuOpenCloseEventSink(const MenuOpenCloseEventSink&) = delete;
            MenuOpenCloseEventSink& operator=(const MenuOpenCloseEventSink&) = delete;

            static void TrySendMouseUp(RE::IMenu* a_menu)
            {
                if (!a_menu || !a_menu->uiMovie || !a_menu->UsesCursor())
                    return;

                auto* cursor = RE::MenuCursor::GetSingleton();
                if (!cursor)
                    return;

                RE::GFxMouseEvent mouseUp(
                    RE::GFxEvent::EventType::kMouseUp,
                    0,
                    cursor->cursorPosX,
                    cursor->cursorPosY,
                    0.0f,
                    0);

                a_menu->uiMovie->HandleEvent(mouseUp);
            }

            RE::BSEventNotifyControl ProcessEvent(
                const RE::MenuOpenCloseEvent*              a_event,
                RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override
            {
                if (!a_event || !a_event->opening)
                    return RE::BSEventNotifyControl::kContinue;

                SKSE::GetTaskInterface()->AddTask([]() {
                    auto* ui = RE::UI::GetSingleton();
                    if (!ui)
                        return;

                    auto& stack = ui->menuStack;
                    if (stack.size() >= 2)
                        TrySendMouseUp(stack[1].get());
                });

                return RE::BSEventNotifyControl::kContinue;
            }
        };
    }

    inline void Install()
    {
        RE::UI::GetSingleton()->AddEventSink(detail::MenuOpenCloseEventSink::GetSingleton());
        logger::info("installed stuck mouse buttons fix"sv);
    }
}