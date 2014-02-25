// terminfo.h - rlibrary terminfo implementation
#ifndef RLIB_TERMINFO_H
#define RLIB_TERMINFO_H
#include "rstream.h"
#include "rdynarray.h"

namespace rtypes
{
    namespace rimpl
    {
        /* terminfo_sequence_eval_error
         *  dummy class that indicates that a
         * terminfo string sequence was incorrect
         * and could not be evaluated
         */
        class terminfo_sequence_eval_error { /* ... */ };

        /* terminfo_sequence
         *  handles conversions from a terminfo sequence to
         * the appropriate escape sequence for the terminal
         */
        class terminfo_sequence
        {
        public:
            terminfo_sequence(const str& source);

            /* get_sequence
             *  returns the escape sequence for the handled
             * sequence with the specified parameters. If too
             * few parameters are provided, the default empty-string
             * parameter will be used
             */
            str get_sequence(const char* const* const parameters,int count) const;
        private:
            str _sequence;
        };

        /* BEGIN CAPS
         *  The order of the capabilities enumerated here is highly
         * significant because it allows the terminfo database to be
         * indexed accurately. The order is based of the ncurses
         * source /include/Caps from version 5.9.
         */

        /* term_boolean_cap
         *  enumerates the boolean capabilities
         * required by rlibrary from a terminal
         */
        enum term_boolean_cap
        {
        };

        /* term_numeric_cap
         *  enumerates the boolean capabilities
         */
        enum term_numeric_cap
        {
        };

        /*
         */
        enum term_string_cap
        {
        };

        /* END CAPS */

        /* dummy classes for terminfo errors
         *  - terminfo_environment_error: TERM environment not set up
         *  - terminfo_not_found_error: TERM was specified but not found in any of the standard search locations
         *  - terminfo_parse_error: TERMINFO file wasn't formatted as expected
         *  - terminfo_lookup_error: capability was not found
         */
        class terminfo_environment_error { /* ... */ };
        class terminfo_not_found_error { /* ... */ };
        class terminfo_parse_error { /* ... */ };
        class terminfo_lookup_error { /* ... */ };

        /* terminfo
         *  parses a compiled terminfo binary file
         * for the terminal whose name is the value
         * of the TERM environment variable.
         */
        class terminfo
        {
        public:
            terminfo();

            bool is_capable(term_boolean_cap) const;
            bool is_capable(term_numeric_cap) const;
            bool is_capable(term_string_cap) const;

            bool operator [](term_boolean_cap) const;
            uint16 operator [](term_numeric_cap) const;
            terminfo_sequence operator [](term_string_cap) const;

            /* this name list contains different aliases
             *  for a terminal separated by `|'
             */
            const str& get_name_list() const
            { return _name; }

            int boolean_cap_count() const
            { return _booleanCaps.size(); }
            int numeric_cap_count() const
            { return _numericCaps.size(); }
            int string_cap_count() const
            { return _stringCaps.size(); }
        private:
            str _name; // name string in format name1|name2

            dynamic_array<bool> _booleanCaps;
            dynamic_array<uint16> _numericCaps;
            dynamic_array<string> _stringCaps;

            bool _read(rbinstream&);
        };
    }
}

#endif
