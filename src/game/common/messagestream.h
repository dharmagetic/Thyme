/**
 * @file
 *
 * @author OmniBlade
 *
 * @brief Class for handling message streams.
 *
 * @copyright Thyme is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#ifndef MESSAGESTREAM_H
#define MESSAGESTREAM_H

#include "always.h"
#include "gamemessagelist.h"

enum GameMessageDisposition
{
    KEEP_MESSAGE = 0x0,
    DESTROY_MESSAGE = 0x1,
};

class GameMessageTranslator
{
public:
    virtual GameMessageDisposition Translate_Game_Message(const GameMessage *msg) = 0;
    virtual ~GameMessageTranslator() {}
};

class MessageStream : public GameMessageList
{
private:
    class TranslatorData
    {
        friend class MessageStream;

    private:
        TranslatorData() : m_next(nullptr), m_prev(nullptr), m_id(0), m_translator(nullptr), m_priority(0) {}
        ~TranslatorData() { delete m_translator; }

    private:
        TranslatorData *m_next;
        TranslatorData *m_prev;
        unsigned m_id;
        GameMessageTranslator *m_translator;
        unsigned m_priority;
    };

public:
    MessageStream() : m_firstTranslator(nullptr), m_lastTranslator(nullptr), m_nextTranslatorID(1) {}
    virtual ~MessageStream();

    virtual GameMessage *Append_Message(GameMessage::MessageType type);
    virtual GameMessage *Insert_Message(GameMessage::MessageType type, GameMessage *msg);

    unsigned Attach_Translator(GameMessageTranslator *translator, unsigned priority);
    GameMessageTranslator *Find_Translator(unsigned id);
    void Remove_Translator(unsigned id);
    void Propagate_Messages();

#ifndef THYME_STANDALONE
    static void Hook_Me();
    GameMessage *Hook_Append_Message(GameMessage::MessageType type);
    GameMessage *Hook_Insert_Message(GameMessage::MessageType type, GameMessage *msg);
#endif

private:
    MessageStream::TranslatorData *m_firstTranslator;
    MessageStream::TranslatorData *m_lastTranslator;
    unsigned m_nextTranslatorID;
};

#ifndef THYME_STANDALONE
#include "hooker.h"

extern MessageStream *&g_theMessageStream;

inline void MessageStream::Hook_Me()
{
    Hook_Method(0x0040D960, &Hook_Append_Message);
    Hook_Method(0x0040DA00, &Hook_Insert_Message);
    Hook_Method(0x0040DAA0, &Attach_Translator);
    Hook_Method(0x0040DB60, &Find_Translator);
    Hook_Method(0x0040DB90, &Remove_Translator);
    Hook_Method(0x0040DBF0, &Propagate_Messages);
}
#else
extern MessageStream *g_theMessageStream;
#endif

#endif