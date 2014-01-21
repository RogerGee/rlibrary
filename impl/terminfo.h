// terminfo.h - rlibrary terminfo implementation
#ifndef RLIB_TERMINFO_H
#define RLIB_TERMINFO_H
#include "rstringstream.h"
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

        /* terminfo_lookup_error
         *  dummy class to indicate that a 
         * capability was not found for the 
         * current terminal
         */
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
            word operator [](term_numeric_cap) const;
            terminfo_sequence operator [](term_string_cap) const;

            int boolean_cap_count() const
            { return _booleanCaps.size(); }
            int numeric_cap_count() const
            { return _numericCaps.size(); }
            int string_cap_count() const
            { return _stringCaps.size(); }
        private:
            str _name; // name string in format name1|name2

            dynamic_array<bool> _booleanCaps;
            dynamic_array<word> _numericCaps;
            dynamic_array<string> _stringCaps;
        };
    }
}

#endif