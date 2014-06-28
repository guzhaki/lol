//
// Lol Engine
//
// Copyright: (c) 2010-2013 Sam Hocevar <sam@hocevar.net>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://www.wtfpl.net/ for more details.
//

//
// File and buffer reading
// -----------------------
//

#if !defined __LOL_SYS_FILE_H__
#define __LOL_SYS_FILE_H__

#include <stdint.h>

namespace lol
{

LOL_SAFE_ENUM(FileAccess,
    Read = 0,
    Write
);

LOL_SAFE_ENUM(StreamType,
    StdIn,
    StdOut,
    StdErr,
    File,
    FileBinary
);

class File
{
public:
    File();
    File(File const &directory);
    File &operator =(File const &that);
    ~File();

    void Open(StreamType stream);
    void Open(String const &file, FileAccess mode, bool force_binary=false);
    bool IsValid() const;
    void Close();

    int Read(uint8_t *buf, int count);
    String ReadString();
    int Write(uint8_t const *buf, int count);
    int WriteString(const String &buf);
    long int GetPosFromStart();
    void SetPosFromStart(long int pos);
    long int GetSize();

private:
    class FileData *m_data;
};

class Directory
{
public:
    Directory(String const &name);
    Directory(Directory const &that);
    Directory &operator =(Directory const &that);
    ~Directory();

    void Open(FileAccess mode);
    bool IsValid() const;
    void Close();

private:
    bool GetContent(array<String>* files, array<Directory>* directories);
public:
    bool GetContent(array<String>& files, array<Directory>& directories);
    bool GetContent(array<Directory>& directories);
    bool GetContent(array<String>& files);
    String GetName();

    static String GetCurrent();
    static bool SetCurrent(String directory);

private:
    class DirectoryData*    m_data;
    String                  m_name;
};

} /* namespace lol */

#endif // __LOL_SYS_FILE_H__

