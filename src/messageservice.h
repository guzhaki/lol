//
// Lol Engine
//
// Copyright: (c) 2013 Benjamin "Touky" Huet <huet.benjamin@gmail.com>
//            (c) 2013 Sam Hocevar <sam@hocevar.net>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://www.wtfpl.net/ for more details.
//

#pragma once

//
// The Message Service class
// ----------------
//

namespace lol
{

//MessageBucket -- Utility enum for message service ---------------------------
struct MessageBucketBase : public StructSafeEnum
{
    enum Type
    {
        AppIn,
        AppOut,

        Bckt0,
        Bckt1,
        Bckt2,
        Bckt3,
        Bckt4,
        Bckt5,
        Bckt6,
        Bckt7,
        Bckt8,
        Bckt9,

        MAX
    };
protected:
    virtual bool BuildEnumMap(map<int64_t, String>& enum_map)
    {
        enum_map[AppIn] = "AppIn";
        enum_map[AppOut] = "AppOut";
        enum_map[Bckt0] = "Bckt0";
        enum_map[Bckt1] = "Bckt1";
        enum_map[Bckt2] = "Bckt2";
        enum_map[Bckt3] = "Bckt3";
        enum_map[Bckt4] = "Bckt4";
        enum_map[Bckt5] = "Bckt5";
        enum_map[Bckt6] = "Bckt6";
        enum_map[Bckt7] = "Bckt7";
        enum_map[Bckt8] = "Bckt8";
        enum_map[Bckt9] = "Bckt9";
        enum_map[MAX] = "MAX";
        return true;
    }
};
typedef SafeEnum<MessageBucketBase> MessageBucket;

//Message list container with time in it
struct MessageList
{
    MessageList(time_t timestamp, const String& message)
    {
        m_timestamp = timestamp;
        m_message = message;
    }

    time_t  m_timestamp;
    String  m_message;
};

/*
    A template class perhaps ?
*/
class MessageService
{
public:
    //CTor/DTor
    MessageService();
    ~MessageService();

    static char const *GetName() { return "<messageservice>"; }

    //Setup/Destroy
    static void Setup();
    static void Destroy();

    //Common interactions
    static bool Send(MessageBucket id, const String& message);
    static bool Send(MessageBucket id, const char* message);
    static bool FetchFirst(MessageBucket id, String& message);
    static bool FetchFirst(MessageBucket id, String& message, time_t &timestamp);
    static bool FetchAll(MessageBucket id, String& message);
    static bool FetchAll(MessageBucket id, String& message, time_t &first_timestamp);

private:
    array<array<MessageList> >  m_bucket;
};

extern MessageService *g_messageservice;

} /* namespace lol */

