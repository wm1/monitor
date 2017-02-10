#include "precomp.h"

Logger::Logger(int _interval_in_seconds)
{
        interval_in_seconds      = _interval_in_seconds;
        max_entries_before_flush = FLUSH_INTERVAL_IN_MINUTES * 60 / interval_in_seconds;
        count                    = 0;

        SYSTEMTIME now;
        GetLocalTime(&now);

        last_entry.timestamp           = now;
        last_entry.duration_in_seconds = 0;
        last_entry.window_title        = L"";
        last_entry.command_line        = L"";

        file = NULL;
        WCHAR buffer[MAX_PATH];
        if (GetEnvironmentVariable(L"LOCALAPPDATA", buffer, MAX_PATH) == 0)
        {
                printf("GetEnvironmentVariable failed with 0x%x\n", GetLastError());
                return;
        }
        std::wstring log_file_name = buffer;
        log_file_name += L"\\record-usage.csv";

        // open file for:
        //   a - appending
        //   t - text mode. LF characters are translated to CR-LF combinations on output
        //   S - optimized for, but not restricted to, sequential access from disk.
        //   _SH_DENYWR	- Denies write access to the file.
        //
        file = _wfsopen(log_file_name.c_str(), L"atS", _SH_DENYWR);
        if (file == NULL)
                printf("Open %S failed with 0x%x\n", log_file_name.c_str(), GetLastError());
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

        count++;
        if (count >= max_entries_before_flush)
                Flush();

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
                fprintf(file, "%04d-%02d-%02d %02d:%02d:%02d, %4d, %S",
                        now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond,
                        data.duration_in_seconds,
                        data.command_line.c_str());
                fprintf(file, ", %S", data.window_title.c_str()); // unicode output is not working 100%. move endline out so that it wont get eaten
                fprintf(file, "\n");
        }

        entries.clear();
        fflush(file);

        last_entry.duration_in_seconds = 0;
        count                          = 0;
}
