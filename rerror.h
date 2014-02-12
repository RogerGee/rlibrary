// rerror.h - provides error types for the entire rlibrary
#ifndef RERROR_H
#define RERROR_H
#include "rstring.h"

namespace rtypes
{
    struct rlib_error
    {
        virtual const char* message() const
        { return "An unspecified error occurred in rlibrary."; }

        static int code()
        { return 1; }
    };
    struct rlib_error_message : rlib_error
    {
        rlib_error_message(const char* sourceMessage)
            : _msg(sourceMessage) {}
        rlib_error_message(const char* sourceMessage,int errCode)
            : _msg(sourceMessage), _code(errCode) {}

        virtual const char* message() const
        { return _msg.c_str(); }

        int code() const
        { return _code; }
    protected:
        str _msg;
        int _code;
    };
    struct rlib_system_error : rlib_error_message
    {
        rlib_system_error(int errCode)
            : rlib_error_message("An unspecified system error occurred in rlibrary.",errCode) {}
    };
    struct operation_successful_error : rlib_error
    {
        virtual const char* message() const
        { return "The operation completed successfully."; }

        static int code()
        { return 0; }
    };
    struct undefined_operation_error : rlib_error
    {
        virtual const char* message() const
        { return "The specified operation was undefined."; }
    };
    struct bad_resource_assignment_error : rlib_error
    {
        virtual const char* message() const
        { return "The rresource could not assign the specified value because it does not have enough bit-width."; }
    };
    struct out_of_bounds_error : rlib_error
    {
        virtual const char* message() const
        { return "The index was not within the bounds of the array."; }
    };
    struct element_not_found_error : rlib_error
    {
        virtual const char* message() const
        { return "The specified element was not found."; }
    };
    struct empty_container_error : rlib_error
    {
        virtual const char* message() const
        { return "No element could be retrieved because the container was empty."; }
    };
    struct bad_iterator_error : rlib_error
    {
        virtual const char* message() const
        { return "The iterator referred to a bad position in the container."; }
    };
    struct does_not_exist_error : rlib_error
    {
        virtual const char* message() const
        { return "The specified element does not exist."; }
    };
    struct already_exists_error : rlib_error
    {
        virtual const char* message() const
        { return "The specified element already exists."; }
    };
    struct bad_path_error : rlib_error
    {
        virtual const char* message() const
        { return "The specified path was incorrect."; }
    };
    struct access_denied_error : rlib_error
    {
        virtual const char* message() const
        { return "Access was denied to the specified resource."; }
    };
    struct sharing_error : rlib_error
    {
        virtual const char* message() const
        { return "Sharing access was denied to the specified resource."; }
    };
    struct directory_creation_not_supported_error : rlib_error
    {
        virtual const char* message() const
        { return "The underlying filesystem does not support directory creation."; }
    };
    struct directory_not_empty_error : rlib_error
    {
        virtual const char* message() const
        { return "The operation cannot complete because the specified directory is not empty."; }
    };
    struct out_of_memory_error : rlib_error
    {
        virtual const char* message() const
        { return "The operation could not complete because the system ran out of memory."; }
    };
    struct out_of_space_error : rlib_error
    {
        virtual const char* message() const
        { return "The operation could not complete because the storage medium ran out of space."; }
    };
    struct fault_error : rlib_error
    {
        virtual const char* message() const
        { return "The process tried to read data outside of its address-range."; }
    };
    struct resource_in_use_error : rlib_error
    {
        virtual const char* message() const
        { return "The specified resource is currently in use by another process or the system."; }
    };
    struct io_error : rlib_error
    {
        virtual const char* message() const
        { return "An IO error occurred during the operation."; }
    };
    struct invalid_resource_error : rlib_error
    {
        virtual const char* message() const
        { return "The operation failed because the specified system resource value was invalid"; }
    };
    struct bad_environment_error : rlib_error
    {
        virtual const char* message() const
        { return "The operation failed because the environment was incorrect."; }
    };
    struct object_not_initialized_error : rlib_error
    {
        virtual const char* message() const
        { return "The operation cannot be performed on the object in its current state."; }
    };
}

#endif
