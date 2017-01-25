class Logger
{
public:
        Logger(int _interval_in_seconds);
        ~Logger();
        void AddEntry(PCWSTR window_title, PCWSTR command_line);

private:
        FILE* file;
        int   interval_in_seconds;
        int   max_entries_before_flush;
        int   count;

        struct Entry
        {
                SYSTEMTIME   timestamp;
                int          duration_in_seconds;
                std::wstring window_title;
                std::wstring command_line;
        };

        Entry              last_entry;
        std::vector<Entry> entries;

        void Flush();
};
