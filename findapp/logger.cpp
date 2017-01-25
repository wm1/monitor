#include "precomp.h"

Logger::Logger(int _interval_in_seconds)
{
        interval_in_seconds      = _interval_in_seconds;
        max_entries_before_flush = FLUSH_INTERVAL_IN_MINUTES * 60 / interval_in_seconds;

        SYSTEMTIME now;
        GetLocalTime(&now);

        last_entry.timestamp           = now;
        last_entry.duration_in_seconds = 0;
        last_entry.window_title        = L"";
        last_entry.command_line        = L"";

        // open file for:
        //   a - appending
        //   t - text mode. LF characters are translated to CR-LF combinations on output
        //   S - optimized for, but not restricted to, sequential access from disk.
        //   _SH_DENYWR	- Denies write access to the file.
        //
        file = _fsopen("monitor.log", "atS", _SH_DENYWR);
}

Logger::~Logger()
{
        Flush();
        fclose(file);
}

void Logger::AddEntry(PCWSTR window_title, PCWSTR command_line)
{
        if (file == NULL)
                return;

        SYSTEMTIME now;
        GetLocalTime(&now);

        if (last_entry.window_title == window_title && last_entry.command_line == command_line)
        {
                last_entry.duration_in_seconds += interval_in_seconds;
                return;
        }

        entries.push_back(last_entry);

        last_entry.timestamp           = now;
        last_entry.duration_in_seconds = interval_in_seconds;
        last_entry.window_title        = window_title;
        last_entry.command_line        = command_line;

        if (entries.size() >= max_entries_before_flush)
                Flush();
}

void Logger::Flush()
{
        if (file == NULL)
                return;

        entries.push_back(last_entry);
        for (auto p = entries.begin(); p != entries.end(); p++)
        {
                Entry&      data = (*p);
                SYSTEMTIME& now  = data.timestamp;
                fprintf(file, "%02d:%02d:%02d,%d,%S", now.wHour, now.wMinute, now.wSecond,
                        data.duration_in_seconds,
                        data.window_title.c_str());
                fprintf(file, ",%S", data.command_line.c_str()); // unicode output is not working 100%. move endline out so that it wont get eaten
                fprintf(file, "\n");
        }

        entries.clear();
}
