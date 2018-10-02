/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015-2016 Nicholas Fraser
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */

/*
 * This is the MPack 0.8.2 amalgamation package.
 *
 * http://github.com/ludocode/mpack
 */

#define MPACK_INTERNAL 1
#define MPACK_EMIT_INLINE_DEFS 1

#include "mpack.h"


/* mpack-platform.c */


// We define MPACK_EMIT_INLINE_DEFS and include mpack.h to emit
// standalone definitions of all (non-static) inline functions in MPack.

#define MPACK_INTERNAL 1
#define MPACK_EMIT_INLINE_DEFS 1

/* #include "mpack-platform.h" */
/* #include "mpack.h" */


#if MPACK_DEBUG && MPACK_STDIO
#include <stdarg.h>
#endif



#if MPACK_DEBUG

#if MPACK_STDIO
void mpack_assert_fail_format(const char* format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    buffer[sizeof(buffer) - 1] = 0;
    mpack_assert_fail(buffer);
}

void mpack_break_hit_format(const char* format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    buffer[sizeof(buffer) - 1] = 0;
    mpack_break_hit(buffer);
}
#endif

#if !MPACK_CUSTOM_ASSERT
void mpack_assert_fail(const char* message) {
    MPACK_UNUSED(message);

    #if MPACK_STDIO
    fprintf(stderr, "%s\n", message);
    #endif

    #if !MPACK_NO_BUILTINS
    #if defined(__GNUC__) || defined(__clang__)
    __builtin_trap();
    #elif defined(WIN32)
    __debugbreak();
    #endif
    #endif

    #if (defined(__GNUC__) || defined(__clang__)) && !MPACK_NO_BUILTINS
    __builtin_abort();
    #elif MPACK_STDLIB
    abort();
    #endif

    MPACK_UNREACHABLE;
}
#endif

#if !MPACK_CUSTOM_BREAK

// If we have a custom assert handler, break wraps it by default.
// This allows users of MPack to only implement mpack_assert_fail() without
// having to worry about the difference between assert and break.
//
// MPACK_CUSTOM_BREAK is available to define a separate break handler
// (which is needed by the unit test suite), but this is not offered in
// mpack-config.h for simplicity.

#if MPACK_CUSTOM_ASSERT
void mpack_break_hit(const char* message) {
    mpack_assert_fail(message);
}
#else
void mpack_break_hit(const char* message) {
    MPACK_UNUSED(message);

    #if MPACK_STDIO
    fprintf(stderr, "%s\n", message);
    #endif

    #if defined(__GNUC__) || defined(__clang__) && !MPACK_NO_BUILTINS
    __builtin_trap();
    #elif defined(WIN32) && !MPACK_NO_BUILTINS
    __debugbreak();
    #elif MPACK_STDLIB
    abort();
    #endif
}
#endif

#endif

#endif



// The below are adapted from the C wikibook:
//     https://en.wikibooks.org/wiki/C_Programming/Strings

#ifndef mpack_memcmp
int mpack_memcmp(const void* s1, const void* s2, size_t n) {
     const unsigned char *us1 = (const unsigned char *) s1;
     const unsigned char *us2 = (const unsigned char *) s2;
     while (n-- != 0) {
         if (*us1 != *us2)
             return (*us1 < *us2) ? -1 : +1;
         us1++;
         us2++;
     }
     return 0;
}
#endif

#ifndef mpack_memcpy
void* mpack_memcpy(void* MPACK_RESTRICT s1, const void* MPACK_RESTRICT s2, size_t n) {
    char* MPACK_RESTRICT dst = (char *)s1;
    const char* MPACK_RESTRICT src = (const char *)s2;
    while (n-- != 0)
        *dst++ = *src++;
    return s1;
}
#endif

#ifndef mpack_memmove
void* mpack_memmove(void* s1, const void* s2, size_t n) {
    char *p1 = (char *)s1;
    const char *p2 = (const char *)s2;
    if (p2 < p1 && p1 < p2 + n) {
        p2 += n;
        p1 += n;
        while (n-- != 0)
            *--p1 = *--p2;
    } else
        while (n-- != 0)
            *p1++ = *p2++;
    return s1;
}
#endif

#ifndef mpack_memset
void* mpack_memset(void* s, int c, size_t n) {
    unsigned char *us = (unsigned char *)s;
    unsigned char uc = (unsigned char)c;
    while (n-- != 0)
        *us++ = uc;
    return s;
}
#endif

#ifndef mpack_strlen
size_t mpack_strlen(const char* s) {
    const char* p = s;
    while (*p != '\0')
        p++;
    return (size_t)(p - s);
}
#endif



#if defined(MPACK_MALLOC) && !defined(MPACK_REALLOC)
void* mpack_realloc(void* old_ptr, size_t used_size, size_t new_size) {
    if (new_size == 0) {
        if (old_ptr)
            MPACK_FREE(old_ptr);
        return NULL;
    }

    void* new_ptr = MPACK_MALLOC(new_size);
    if (new_ptr == NULL)
        return NULL;

    mpack_memcpy(new_ptr, old_ptr, used_size);
    MPACK_FREE(old_ptr);
    return new_ptr;
}
#endif

/* mpack-common.c */

#define MPACK_INTERNAL 1

/* #include "mpack-common.h" */

#if MPACK_DEBUG && MPACK_STDIO
#include <stdarg.h>
#endif

const char* mpack_error_to_string(mpack_error_t error) {
    #if MPACK_STRINGS
    switch (error) {
        #define MPACK_ERROR_STRING_CASE(e) case e: return #e
        MPACK_ERROR_STRING_CASE(mpack_ok);
        MPACK_ERROR_STRING_CASE(mpack_error_io);
        MPACK_ERROR_STRING_CASE(mpack_error_invalid);
        MPACK_ERROR_STRING_CASE(mpack_error_type);
        MPACK_ERROR_STRING_CASE(mpack_error_too_big);
        MPACK_ERROR_STRING_CASE(mpack_error_memory);
        MPACK_ERROR_STRING_CASE(mpack_error_bug);
        MPACK_ERROR_STRING_CASE(mpack_error_data);
        #undef MPACK_ERROR_STRING_CASE
        default: break;
    }
    mpack_assert(0, "unrecognized error %i", (int)error);
    return "(unknown mpack_error_t)";
    #else
    MPACK_UNUSED(error);
    return "";
    #endif
}

const char* mpack_type_to_string(mpack_type_t type) {
    #if MPACK_STRINGS
    switch (type) {
        #define MPACK_TYPE_STRING_CASE(e) case e: return #e
        MPACK_TYPE_STRING_CASE(mpack_type_nil);
        MPACK_TYPE_STRING_CASE(mpack_type_bool);
        MPACK_TYPE_STRING_CASE(mpack_type_float);
        MPACK_TYPE_STRING_CASE(mpack_type_double);
        MPACK_TYPE_STRING_CASE(mpack_type_int);
        MPACK_TYPE_STRING_CASE(mpack_type_uint);
        MPACK_TYPE_STRING_CASE(mpack_type_str);
        MPACK_TYPE_STRING_CASE(mpack_type_bin);
        MPACK_TYPE_STRING_CASE(mpack_type_ext);
        MPACK_TYPE_STRING_CASE(mpack_type_array);
        MPACK_TYPE_STRING_CASE(mpack_type_map);
        #undef MPACK_TYPE_STRING_CASE
        default: break;
    }
    mpack_assert(0, "unrecognized type %i", (int)type);
    return "(unknown mpack_type_t)";
    #else
    MPACK_UNUSED(type);
    return "";
    #endif
}

int mpack_tag_cmp(mpack_tag_t left, mpack_tag_t right) {

    // positive numbers may be stored as int; convert to uint
    if (left.type == mpack_type_int && left.v.i >= 0) {
        left.type = mpack_type_uint;
        left.v.u = left.v.i;
    }
    if (right.type == mpack_type_int && right.v.i >= 0) {
        right.type = mpack_type_uint;
        right.v.u = right.v.i;
    }

    if (left.type != right.type)
        return (int)left.type - (int)right.type;

    switch (left.type) {
        case mpack_type_nil:
            return 0;

        case mpack_type_bool:
            return (int)left.v.b - (int)right.v.b;

        case mpack_type_int:
            if (left.v.i == right.v.i)
                return 0;
            return (left.v.i < right.v.i) ? -1 : 1;

        case mpack_type_uint:
            if (left.v.u == right.v.u)
                return 0;
            return (left.v.u < right.v.u) ? -1 : 1;

        case mpack_type_array:
        case mpack_type_map:
            if (left.v.n == right.v.n)
                return 0;
            return (left.v.n < right.v.n) ? -1 : 1;

        case mpack_type_str:
        case mpack_type_bin:
            if (left.v.l == right.v.l)
                return 0;
            return (left.v.l < right.v.l) ? -1 : 1;

        case mpack_type_ext:
            if (left.exttype == right.exttype) {
                if (left.v.l == right.v.l)
                    return 0;
                return (left.v.l < right.v.l) ? -1 : 1;
            }
            return (int)left.exttype - (int)right.exttype;

        // floats should not normally be compared for equality. we compare
        // with memcmp() to silence compiler warnings, but this will return
        // equal if both are NaNs with the same representation (though we may
        // want this, for instance if you are for some bizarre reason using
        // floats as map keys.) i'm not sure what the right thing to
        // do is here. check for NaN first? always return false if the type
        // is float? use operator== and pragmas to silence compiler warning?
        // please send me your suggestions.
        // note also that we don't convert floats to doubles, so when this is
        // used for ordering purposes, all floats are ordered before all
        // doubles.
        case mpack_type_float:
            return mpack_memcmp(&left.v.f, &right.v.f, sizeof(left.v.f));
        case mpack_type_double:
            return mpack_memcmp(&left.v.d, &right.v.d, sizeof(left.v.d));

        default:
            break;
    }
    
    mpack_assert(0, "unrecognized type %i", (int)left.type);
    return false;
}



#if MPACK_READ_TRACKING || MPACK_WRITE_TRACKING

#ifndef MPACK_TRACKING_INITIAL_CAPACITY
// seems like a reasonable number. we grow by doubling, and it only
// needs to be as long as the maximum depth of the message.
#define MPACK_TRACKING_INITIAL_CAPACITY 8
#endif

mpack_error_t mpack_track_init(mpack_track_t* track) {
    track->count = 0;
    track->capacity = MPACK_TRACKING_INITIAL_CAPACITY;
    track->elements = (mpack_track_element_t*)MPACK_MALLOC(sizeof(mpack_track_element_t) * track->capacity);
    if (track->elements == NULL)
        return mpack_error_memory;
    return mpack_ok;
}

mpack_error_t mpack_track_grow(mpack_track_t* track) {
    mpack_assert(track->elements, "null track elements!");
    mpack_assert(track->count == track->capacity, "incorrect growing?");

    size_t new_capacity = track->capacity * 2;

    mpack_track_element_t* new_elements = (mpack_track_element_t*)mpack_realloc(track->elements,
            sizeof(mpack_track_element_t) * track->count, sizeof(mpack_track_element_t) * new_capacity);
    if (new_elements == NULL)
        return mpack_error_memory;

    track->elements = new_elements;
    track->capacity = new_capacity;
    return mpack_ok;
}

mpack_error_t mpack_track_push(mpack_track_t* track, mpack_type_t type, uint64_t count) {
    mpack_assert(track->elements, "null track elements!");
    mpack_log("track pushing %s count %i\n", mpack_type_to_string(type), (int)count);

    // maps have twice the number of elements (key/value pairs)
    if (type == mpack_type_map)
        count *= 2;

    // grow if needed
    if (track->count == track->capacity) {
        mpack_error_t error = mpack_track_grow(track);
        if (error != mpack_ok)
            return error;
    }

    // insert new track
    track->elements[track->count].type = type;
    track->elements[track->count].left = count;
    ++track->count;
    return mpack_ok;
}

mpack_error_t mpack_track_pop(mpack_track_t* track, mpack_type_t type) {
    mpack_assert(track->elements, "null track elements!");
    mpack_log("track popping %s\n", mpack_type_to_string(type));

    if (track->count == 0) {
        mpack_break("attempting to close a %s but nothing was opened!", mpack_type_to_string(type));
        return mpack_error_bug;
    }

    mpack_track_element_t* element = &track->elements[track->count - 1];

    if (element->type != type) {
        mpack_break("attempting to close a %s but the open element is a %s!",
                mpack_type_to_string(type), mpack_type_to_string(element->type));
        return mpack_error_bug;
    }

    if (element->left != 0) {
        mpack_break("attempting to close a %s but there are %" PRIu64 " %s left",
                mpack_type_to_string(type), element->left,
                (type == mpack_type_map || type == mpack_type_array) ? "elements" : "bytes");
        return mpack_error_bug;
    }

    --track->count;
    return mpack_ok;
}

mpack_error_t mpack_track_peek_element(mpack_track_t* track, bool read) {
    MPACK_UNUSED(read);
    mpack_assert(track->elements, "null track elements!");

    // if there are no open elements, that's fine, we can read/write elements at will
    if (track->count == 0)
        return mpack_ok;

    mpack_track_element_t* element = &track->elements[track->count - 1];

    if (element->type != mpack_type_map && element->type != mpack_type_array) {
        mpack_break("elements cannot be %s within an %s", read ? "read" : "written",
                mpack_type_to_string(element->type));
        return mpack_error_bug;
    }

    if (element->left == 0) {
        mpack_break("too many elements %s for %s", read ? "read" : "written",
                mpack_type_to_string(element->type));
        return mpack_error_bug;
    }

    return mpack_ok;
}

mpack_error_t mpack_track_element(mpack_track_t* track, bool read) {
    mpack_error_t error = mpack_track_peek_element(track, read);
    if (track->count > 0 && error == mpack_ok)
        --track->elements[track->count - 1].left;
    return error;
}

mpack_error_t mpack_track_bytes(mpack_track_t* track, bool read, uint64_t count) {
    MPACK_UNUSED(read);
    mpack_assert(track->elements, "null track elements!");

    if (track->count == 0) {
        mpack_break("bytes cannot be %s with no open bin, str or ext", read ? "read" : "written");
        return mpack_error_bug;
    }

    mpack_track_element_t* element = &track->elements[track->count - 1];

    if (element->type == mpack_type_map || element->type == mpack_type_array) {
        mpack_break("bytes cannot be %s within an %s", read ? "read" : "written",
                mpack_type_to_string(element->type));
        return mpack_error_bug;
    }

    if (element->left < count) {
        mpack_break("too many bytes %s for %s", read ? "read" : "written",
                mpack_type_to_string(element->type));
        return mpack_error_bug;
    }

    element->left -= count;
    return mpack_ok;
}

mpack_error_t mpack_track_str_bytes_all(mpack_track_t* track, bool read, uint64_t count) {
    mpack_error_t error = mpack_track_bytes(track, read, count);
    if (error != mpack_ok)
        return error;

    mpack_track_element_t* element = &track->elements[track->count - 1];

    if (element->type != mpack_type_str) {
        mpack_break("the open type must be a string, not a %s", mpack_type_to_string(element->type));
        return mpack_error_bug;
    }

    if (element->left != 0) {
        mpack_break("not all bytes were read; the wrong byte count was requested for a string read.");
        return mpack_error_bug;
    }

    return mpack_ok;
}

mpack_error_t mpack_track_check_empty(mpack_track_t* track) {
    if (track->count != 0) {
        mpack_break("unclosed %s", mpack_type_to_string(track->elements[0].type));
        return mpack_error_bug;
    }
    return mpack_ok;
}

mpack_error_t mpack_track_destroy(mpack_track_t* track, bool cancel) {
    mpack_error_t error = cancel ? mpack_ok : mpack_track_check_empty(track);
    if (track->elements) {
        MPACK_FREE(track->elements);
        track->elements = NULL;
    }
    return error;
}
#endif



static bool mpack_utf8_check_impl(const uint8_t* str, size_t count, bool allow_null) {
    while (count > 0) {
        uint8_t lead = str[0];

        // NUL
        if (!allow_null && lead == '\0') // we don't allow NUL bytes in MPack C-strings
            return false;

        // ASCII
        if (lead <= 0x7F) {
            ++str;
            --count;

        // 2-byte sequence
        } else if ((lead & 0xE0) == 0xC0) {
            if (count < 2) // truncated sequence
                return false;

            uint8_t cont = str[1];
            if ((cont & 0xC0) != 0x80) // not a continuation byte
                return false;

            str += 2;
            count -= 2;

            uint32_t z = ((uint32_t)(lead & ~0xE0) << 6) |
                          (uint32_t)(cont & ~0xC0);

            if (z < 0x80) // overlong sequence
                return false;

        // 3-byte sequence
        } else if ((lead & 0xF0) == 0xE0) {
            if (count < 3) // truncated sequence
                return false;

            uint8_t cont1 = str[1];
            if ((cont1 & 0xC0) != 0x80) // not a continuation byte
                return false;
            uint8_t cont2 = str[2];
            if ((cont2 & 0xC0) != 0x80) // not a continuation byte
                return false;

            str += 3;
            count -= 3;

            uint32_t z = ((uint32_t)(lead  & ~0xF0) << 12) |
                         ((uint32_t)(cont1 & ~0xC0) <<  6) |
                          (uint32_t)(cont2 & ~0xC0);

            if (z < 0x800) // overlong sequence
                return false;
            if (z >= 0xD800 && z <= 0xDFFF) // surrogate
                return false;

        // 4-byte sequence
        } else if ((lead & 0xF8) == 0xF0) {
            if (count < 4) // truncated sequence
                return false;

            uint8_t cont1 = str[1];
            if ((cont1 & 0xC0) != 0x80) // not a continuation byte
                return false;
            uint8_t cont2 = str[2];
            if ((cont2 & 0xC0) != 0x80) // not a continuation byte
                return false;
            uint8_t cont3 = str[3];
            if ((cont3 & 0xC0) != 0x80) // not a continuation byte
                return false;

            str += 4;
            count -= 4;

            uint32_t z = ((uint32_t)(lead  & ~0xF8) << 18) |
                         ((uint32_t)(cont1 & ~0xC0) << 12) |
                         ((uint32_t)(cont2 & ~0xC0) <<  6) |
                          (uint32_t)(cont3 & ~0xC0);

            if (z < 0x10000) // overlong sequence
                return false;
            if (z > 0x10FFFF) // codepoint limit
                return false;

        } else {
            return false; // continuation byte without a lead, or lead for a 5-byte sequence or longer
        }
    }
    return true;
}

bool mpack_utf8_check(const char* str, size_t bytes) {
    return mpack_utf8_check_impl((const uint8_t*)str, bytes, true);
}

bool mpack_utf8_check_no_null(const char* str, size_t bytes) {
    return mpack_utf8_check_impl((const uint8_t*)str, bytes, false);
}

bool mpack_str_check_no_null(const char* str, size_t bytes) {
    for (size_t i = 0; i < bytes; ++i)
        if (str[i] == '\0')
            return false;
    return true;
}


/* mpack-writer.c */

#define MPACK_INTERNAL 1

/* #include "mpack-writer.h" */

#if MPACK_WRITER

#if MPACK_WRITE_TRACKING
static void mpack_writer_flag_if_error(mpack_writer_t* writer, mpack_error_t error) {
    if (error != mpack_ok)
        mpack_writer_flag_error(writer, error);
}

void mpack_writer_track_push(mpack_writer_t* writer, mpack_type_t type, uint64_t count) {
    if (writer->error == mpack_ok)
        mpack_writer_flag_if_error(writer, mpack_track_push(&writer->track, type, count));
}

void mpack_writer_track_pop(mpack_writer_t* writer, mpack_type_t type) {
    if (writer->error == mpack_ok)
        mpack_writer_flag_if_error(writer, mpack_track_pop(&writer->track, type));
}

void mpack_writer_track_element(mpack_writer_t* writer) {
    if (writer->error == mpack_ok)
        mpack_writer_flag_if_error(writer, mpack_track_element(&writer->track, false));
}

void mpack_writer_track_bytes(mpack_writer_t* writer, size_t count) {
    if (writer->error == mpack_ok)
        mpack_writer_flag_if_error(writer, mpack_track_bytes(&writer->track, false, count));
}
#endif

static void mpack_writer_clear(mpack_writer_t* writer) {
    writer->flush = NULL;
    writer->error_fn = NULL;
    writer->teardown = NULL;
    writer->context = NULL;

    writer->buffer = NULL;
    writer->size = 0;
    writer->used = 0;
    writer->error = mpack_ok;

    #if MPACK_WRITE_TRACKING
    mpack_memset(&writer->track, 0, sizeof(writer->track));
    #endif
}

void mpack_writer_init(mpack_writer_t* writer, char* buffer, size_t size) {
    mpack_assert(buffer != NULL, "cannot initialize writer with empty buffer");
    mpack_writer_clear(writer);
    writer->buffer = buffer;
    writer->size = size;

    #if MPACK_WRITE_TRACKING
    mpack_writer_flag_if_error(writer, mpack_track_init(&writer->track));
    #endif

    mpack_log("===========================\n");
    mpack_log("initializing writer with buffer size %i\n", (int)size);
}

void mpack_writer_init_error(mpack_writer_t* writer, mpack_error_t error) {
    mpack_writer_clear(writer);
    writer->error = error;

    mpack_log("===========================\n");
    mpack_log("initializing writer in error state %i\n", (int)error);
}

void mpack_writer_set_flush(mpack_writer_t* writer, mpack_writer_flush_t flush) {
    MPACK_STATIC_ASSERT(MPACK_WRITER_MINIMUM_BUFFER_SIZE >= MPACK_MAXIMUM_TAG_SIZE,
            "minimum buffer size must fit any tag!");

    if (writer->size < MPACK_WRITER_MINIMUM_BUFFER_SIZE) {
        mpack_break("buffer size is %i, but minimum buffer size for flush is %i",
                (int)writer->size, MPACK_WRITER_MINIMUM_BUFFER_SIZE);
        mpack_writer_flag_error(writer, mpack_error_bug);
        return;
    }

    writer->flush = flush;
}

#ifdef MPACK_MALLOC
typedef struct mpack_growable_writer_t {
    char** target_data;
    size_t* target_size;
} mpack_growable_writer_t;

static void mpack_growable_writer_flush(mpack_writer_t* writer, const char* data, size_t count) {

    // This is an intrusive flush function which modifies the writer's buffer
    // in response to a flush instead of emptying it in order to add more
    // capacity for data. This removes the need to copy data from a fixed buffer
    // into a growable one, improving performance.
    //
    // There are three ways flush can be called:
    //   - flushing the buffer during writing (used is zero, count is all data, data is buffer)
    //   - flushing extra data during writing (used is all flushed data, count is extra data, data is not buffer)
    //   - flushing during teardown (used and count are both all flushed data, data is buffer)
    //
    // In the first two cases, we grow the buffer by at least double, enough
    // to ensure that new data will fit. We ignore the teardown flush.

    if (data == writer->buffer) {

        // teardown, do nothing
        if (writer->used == count)
            return;

        // otherwise leave the data in the buffer and just grow
        writer->used = count;
        count = 0;
    }

    mpack_log("flush size %i used %i data %p buffer %p\n",
            (int)count, (int)writer->used, data, writer->buffer);

    mpack_assert(data == writer->buffer || writer->used + count > writer->size,
            "extra flush for %i but there is %i space left in the buffer! (%i/%i)",
            (int)count, (int)writer->size - (int)writer->used, (int)writer->used, (int)writer->size);

    // grow to fit the data
    // TODO: this really needs to correctly test for overflow
    size_t new_size = writer->size * 2;
    while (new_size < writer->used + count)
        new_size *= 2;

    mpack_log("flush growing buffer size from %i to %i\n", (int)writer->size, (int)new_size);

    // grow the buffer
    char* new_buffer = (char*)mpack_realloc(writer->buffer, writer->used, new_size);
    if (new_buffer == NULL) {
        mpack_writer_flag_error(writer, mpack_error_memory);
        return;
    }
    writer->buffer = new_buffer;
    writer->size = new_size;

    // append the extra data
    if (count > 0) {
        mpack_memcpy(writer->buffer + writer->used, data, count);
        writer->used += count;
    }

    mpack_log("new buffer %p, used %i\n", new_buffer, (int)writer->used);
}

static void mpack_growable_writer_teardown(mpack_writer_t* writer) {
    mpack_growable_writer_t* growable_writer = (mpack_growable_writer_t*)writer->context;

    if (mpack_writer_error(writer) == mpack_ok) {

        // shrink the buffer to an appropriate size if the data is
        // much smaller than the buffer
        if (writer->used < writer->size / 2) {
            char* buffer = (char*)mpack_realloc(writer->buffer, writer->used, writer->used);
            if (!buffer) {
                MPACK_FREE(writer->buffer);
                mpack_writer_flag_error(writer, mpack_error_memory);
                return;
            }
            writer->buffer = buffer;
            writer->size = writer->used;
        }

        *growable_writer->target_data = writer->buffer;
        *growable_writer->target_size = writer->used;
        writer->buffer = NULL;

    } else if (writer->buffer) {
        MPACK_FREE(writer->buffer);
        writer->buffer = NULL;
    }

    writer->context = NULL;
}

static char* mpack_writer_get_reserved(mpack_writer_t* writer) {
    // This is in a separate function in order to avoid false strict aliasing
    // warnings. We aren't actually violating strict aliasing (the reserved
    // space is only ever dereferenced as an mpack_growable_writer_t.)
    return (char*)writer->reserved;
}

void mpack_writer_init_growable(mpack_writer_t* writer, char** target_data, size_t* target_size) {
    mpack_assert(target_data != NULL, "cannot initialize writer without a destination for the data");
    mpack_assert(target_size != NULL, "cannot initialize writer without a destination for the size");

    *target_data = NULL;
    *target_size = 0;

    MPACK_STATIC_ASSERT(sizeof(mpack_growable_writer_t) <= sizeof(writer->reserved),
            "not enough reserved space for growable writer!");
    mpack_growable_writer_t* growable_writer = (mpack_growable_writer_t*)mpack_writer_get_reserved(writer);

    growable_writer->target_data = target_data;
    growable_writer->target_size = target_size;

    size_t capacity = MPACK_BUFFER_SIZE;
    char* buffer = (char*)MPACK_MALLOC(capacity);
    if (buffer == NULL) {
        mpack_writer_init_error(writer, mpack_error_memory);
        return;
    }

    mpack_writer_init(writer, buffer, capacity);
    mpack_writer_set_context(writer, growable_writer);
    mpack_writer_set_flush(writer, mpack_growable_writer_flush);
    mpack_writer_set_teardown(writer, mpack_growable_writer_teardown);
}
#endif

#if MPACK_STDIO
static void mpack_file_writer_flush(mpack_writer_t* writer, const char* buffer, size_t count) {
    FILE* file = (FILE*)writer->context;
    size_t written = fwrite((const void*)buffer, 1, count, file);
    if (written != count)
        mpack_writer_flag_error(writer, mpack_error_io);
}

static void mpack_file_writer_teardown(mpack_writer_t* writer) {
    FILE* file = (FILE*)writer->context;

    if (file) {
        int ret = fclose(file);
        writer->context = NULL;
        if (ret != 0)
            mpack_writer_flag_error(writer, mpack_error_io);
    }

    MPACK_FREE(writer->buffer);
    writer->buffer = NULL;
}

void mpack_writer_init_file(mpack_writer_t* writer, const char* filename) {
    mpack_assert(filename != NULL, "filename is NULL");

    size_t capacity = MPACK_BUFFER_SIZE;
    char* buffer = (char*)MPACK_MALLOC(capacity);
    if (buffer == NULL) {
        mpack_writer_init_error(writer, mpack_error_memory);
        return;
    }

    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        MPACK_FREE(buffer);
        mpack_writer_init_error(writer, mpack_error_io);
        return;
    }

    mpack_writer_init(writer, buffer, capacity);
    mpack_writer_set_context(writer, file);
    mpack_writer_set_flush(writer, mpack_file_writer_flush);
    mpack_writer_set_teardown(writer, mpack_file_writer_teardown);
}
#endif

void mpack_writer_flag_error(mpack_writer_t* writer, mpack_error_t error) {
    mpack_log("writer %p setting error %i: %s\n", writer, (int)error, mpack_error_to_string(error));

    if (writer->error == mpack_ok) {
        writer->error = error;
        if (writer->error_fn)
            writer->error_fn(writer, writer->error);
    }
}

MPACK_STATIC_INLINE void mpack_writer_flush_unchecked(mpack_writer_t* writer) {
    // This is a bit ugly; we reset used before calling flush so that
    // a flush function can distinguish between flushing the buffer
    // versus flushing external data. see mpack_growable_writer_flush()
    size_t used = writer->used;
    writer->used = 0;
    writer->flush(writer, writer->buffer, used);
}

// Ensures there are at least count bytes free in the buffer. This
// will flag an error if the flush function fails to make enough
// room in the buffer.
static bool mpack_writer_ensure(mpack_writer_t* writer, size_t count) {
    mpack_assert(count != 0, "cannot ensure zero bytes!");
    mpack_assert(count <= MPACK_WRITER_MINIMUM_BUFFER_SIZE,
            "cannot ensure %i bytes, this is more than the minimum buffer size %i!",
            (int)count, (int)MPACK_WRITER_MINIMUM_BUFFER_SIZE);
    mpack_assert(count > mpack_writer_buffer_left(writer),
            "request to ensure %i bytes but there are already %i left in the buffer!",
            (int)count, (int)mpack_writer_buffer_left(writer));

    mpack_log("ensuring %i bytes, %i left\n", (int)count, (int)mpack_writer_buffer_left(writer));

    if (mpack_writer_error(writer) != mpack_ok)
        return false;

    if (writer->flush == NULL) {
        mpack_writer_flag_error(writer, mpack_error_too_big);
        return false;
    }

    mpack_writer_flush_unchecked(writer);
    if (mpack_writer_error(writer) != mpack_ok)
        return false;

    if (mpack_writer_buffer_left(writer) >= count)
        return true;

    mpack_writer_flag_error(writer, mpack_error_io);
    return false;
}

// Writes encoded bytes to the buffer when we already know the data
// does not fit in the buffer (i.e. it straddles the edge of the
// buffer.) If there is a flush function, it is guaranteed to be
// called; otherwise mpack_error_too_big is raised.
static void mpack_write_native_straddle(mpack_writer_t* writer, const char* p, size_t count) {
    mpack_assert(count == 0 || p != NULL, "data pointer for %i bytes is NULL", (int)count);

    if (mpack_writer_error(writer) != mpack_ok)
        return;
    mpack_log("big write for %i bytes from %p, %i space left in buffer\n",
            (int)count, p, (int)(writer->size - writer->used));
    mpack_assert(count > writer->size - writer->used,
            "big write requested for %i bytes, but there is %i available "
            "space in buffer. should have called mpack_write_native() instead",
            (int)count, (int)(writer->size - writer->used));

    // we'll need a flush function
    if (!writer->flush) {
        mpack_writer_flag_error(writer, mpack_error_too_big);
        return;
    }

    // flush the buffer
    mpack_writer_flush_unchecked(writer);
    if (mpack_writer_error(writer) != mpack_ok)
        return;

    // note that an intrusive flush function (such as mpack_growable_writer_flush())
    // may have changed size and/or reset used to a non-zero value. we treat both as
    // though they may have changed, and there may still be data in the buffer.

    // flush the extra data directly if it doesn't fit in the buffer
    if (count > writer->size - writer->used) {
        writer->flush(writer, p, count);
        if (mpack_writer_error(writer) != mpack_ok)
            return;
    } else {
        mpack_memcpy(writer->buffer + writer->used, p, count);
        writer->used += count;
    }
}

// Writes encoded bytes to the buffer, flushing if necessary.
MPACK_STATIC_INLINE void mpack_write_native(mpack_writer_t* writer, const char* p, size_t count) {
    mpack_assert(count == 0 || p != NULL, "data pointer for %i bytes is NULL", (int)count);

    if (writer->size - writer->used < count) {
        mpack_write_native_straddle(writer, p, count);
    } else {
        mpack_memcpy(writer->buffer + writer->used, p, count);
        writer->used += count;
    }
}

mpack_error_t mpack_writer_destroy(mpack_writer_t* writer) {

    // clean up tracking, asserting if we're not already in an error state
    #if MPACK_WRITE_TRACKING
    mpack_track_destroy(&writer->track, writer->error != mpack_ok);
    #endif

    // flush any outstanding data
    if (mpack_writer_error(writer) == mpack_ok && writer->used != 0 && writer->flush != NULL) {
        writer->flush(writer, writer->buffer, writer->used);
        writer->flush = NULL;
    }

    if (writer->teardown) {
        writer->teardown(writer);
        writer->teardown = NULL;
    }

    return writer->error;
}

void mpack_write_tag(mpack_writer_t* writer, mpack_tag_t value) {
    switch (value.type) {
        case mpack_type_nil:    mpack_write_nil   (writer);            break;
        case mpack_type_bool:   mpack_write_bool  (writer, value.v.b); break;
        case mpack_type_float:  mpack_write_float (writer, value.v.f); break;
        case mpack_type_double: mpack_write_double(writer, value.v.d); break;
        case mpack_type_int:    mpack_write_int   (writer, value.v.i); break;
        case mpack_type_uint:   mpack_write_uint  (writer, value.v.u); break;

        case mpack_type_str: mpack_start_str(writer, value.v.l); break;
        case mpack_type_bin: mpack_start_bin(writer, value.v.l); break;
        case mpack_type_ext: mpack_start_ext(writer, value.exttype, value.v.l); break;

        case mpack_type_array: mpack_start_array(writer, value.v.n); break;
        case mpack_type_map:   mpack_start_map(writer, value.v.n);   break;

        default:
            mpack_assert(0, "unrecognized type %i", (int)value.type);
            break;
    }
}

MPACK_STATIC_INLINE void mpack_write_byte_element(mpack_writer_t* writer, char value) {
    mpack_writer_track_element(writer);
    if (mpack_writer_buffer_left(writer) >= 1 || mpack_writer_ensure(writer, 1))
        writer->buffer[writer->used++] = value;
}

void mpack_write_nil(mpack_writer_t* writer) {
    mpack_write_byte_element(writer, (char)0xc0);
}

void mpack_write_bool(mpack_writer_t* writer, bool value) {
    mpack_write_byte_element(writer, (char)(0xc2 | (value ? 1 : 0)));
}

void mpack_write_true(mpack_writer_t* writer) {
    mpack_write_byte_element(writer, (char)0xc3);
}

void mpack_write_false(mpack_writer_t* writer) {
    mpack_write_byte_element(writer, (char)0xc2);
}

void mpack_write_object_bytes(mpack_writer_t* writer, const char* data, size_t bytes) {
    mpack_writer_track_element(writer);
    mpack_write_native(writer, data, bytes);
}

/*
 * Encode functions
 */

MPACK_STATIC_INLINE void mpack_encode_fixuint(char* p, uint8_t value) {
    mpack_assert(value <= 127);
    mpack_store_u8(p, value);
}

MPACK_STATIC_INLINE void mpack_encode_u8(char* p, uint8_t value) {
    mpack_assert(value > 127);
    mpack_store_u8(p, 0xcc);
    mpack_store_u8(p + 1, value);
}

MPACK_STATIC_INLINE void mpack_encode_u16(char* p, uint16_t value) {
    mpack_assert(value > UINT8_MAX);
    mpack_store_u8(p, 0xcd);
    mpack_store_u16(p + 1, value);
}

MPACK_STATIC_INLINE void mpack_encode_u32(char* p, uint32_t value) {
    mpack_assert(value > UINT16_MAX);
    mpack_store_u8(p, 0xce);
    mpack_store_u32(p + 1, value);
}

MPACK_STATIC_INLINE void mpack_encode_u64(char* p, uint64_t value) {
    mpack_assert(value > UINT32_MAX);
    mpack_store_u8(p, 0xcf);
    mpack_store_u64(p + 1, value);
}

MPACK_STATIC_INLINE void mpack_encode_fixint(char* p, int8_t value) {
    // this can encode positive or negative fixints
    mpack_assert(value >= -32);
    mpack_store_i8(p, value);
}

MPACK_STATIC_INLINE void mpack_encode_i8(char* p, int8_t value) {
    mpack_assert(value < -32);
    mpack_store_u8(p, 0xd0);
    mpack_store_i8(p + 1, value);
}

MPACK_STATIC_INLINE void mpack_encode_i16(char* p, int16_t value) {
    mpack_assert(value < INT8_MIN);
    mpack_store_u8(p, 0xd1);
    mpack_store_i16(p + 1, value);
}

MPACK_STATIC_INLINE void mpack_encode_i32(char* p, int32_t value) {
    mpack_assert(value < INT16_MIN);
    mpack_store_u8(p, 0xd2);
    mpack_store_i32(p + 1, value);
}

MPACK_STATIC_INLINE void mpack_encode_i64(char* p, int64_t value) {
    mpack_assert(value < INT32_MIN);
    mpack_store_u8(p, 0xd3);
    mpack_store_i64(p + 1, value);
}

MPACK_STATIC_INLINE void mpack_encode_float(char* p, float value) {
    mpack_store_u8(p, 0xca);
    mpack_store_float(p + 1, value);
}

MPACK_STATIC_INLINE void mpack_encode_double(char* p, double value) {
    mpack_store_u8(p, 0xcb);
    mpack_store_double(p + 1, value);
}

MPACK_STATIC_INLINE void mpack_encode_fixarray(char* p, uint8_t count) {
    mpack_assert(count <= 15);
    mpack_store_u8(p, (uint8_t)(0x90 | count));
}

MPACK_STATIC_INLINE void mpack_encode_array16(char* p, uint16_t count) {
    mpack_assert(count > 15);
    mpack_store_u8(p, 0xdc);
    mpack_store_u16(p + 1, count);
}

MPACK_STATIC_INLINE void mpack_encode_array32(char* p, uint32_t count) {
    mpack_assert(count > UINT16_MAX);
    mpack_store_u8(p, 0xdd);
    mpack_store_u32(p + 1, count);
}

MPACK_STATIC_INLINE void mpack_encode_fixmap(char* p, uint8_t count) {
    mpack_assert(count <= 15);
    mpack_store_u8(p, (uint8_t)(0x80 | count));
}

MPACK_STATIC_INLINE void mpack_encode_map16(char* p, uint16_t count) {
    mpack_assert(count > 15);
    mpack_store_u8(p, 0xde);
    mpack_store_u16(p + 1, count);
}

MPACK_STATIC_INLINE void mpack_encode_map32(char* p, uint32_t count) {
    mpack_assert(count > UINT16_MAX);
    mpack_store_u8(p, 0xdf);
    mpack_store_u32(p + 1, count);
}

MPACK_STATIC_INLINE void mpack_encode_fixstr(char* p, uint8_t count) {
    mpack_assert(count <= 31);
    mpack_store_u8(p, (uint8_t)(0xa0 | count));
}

MPACK_STATIC_INLINE void mpack_encode_str8(char* p, uint8_t count) {
    // TODO: str8 had no counterpart in MessagePack 1.0; there was only
    // fixraw, raw16 and raw32. This should not be used in compatibility mode.
    mpack_assert(count > 31);
    mpack_store_u8(p, 0xd9);
    mpack_store_u8(p + 1, count);
}

MPACK_STATIC_INLINE void mpack_encode_str16(char* p, uint16_t count) {
    mpack_assert(count > UINT8_MAX);
    mpack_store_u8(p, 0xda);
    mpack_store_u16(p + 1, count);
}

MPACK_STATIC_INLINE void mpack_encode_str32(char* p, uint32_t count) {
    mpack_assert(count > UINT16_MAX);
    mpack_store_u8(p, 0xdb);
    mpack_store_u32(p + 1, count);
}

MPACK_STATIC_INLINE void mpack_encode_bin8(char* p, uint8_t count) {
    mpack_store_u8(p, 0xc4);
    mpack_store_u8(p + 1, count);
}

MPACK_STATIC_INLINE void mpack_encode_bin16(char* p, uint16_t count) {
    mpack_assert(count > UINT8_MAX);
    mpack_store_u8(p, 0xc5);
    mpack_store_u16(p + 1, count);
}

MPACK_STATIC_INLINE void mpack_encode_bin32(char* p, uint32_t count) {
    mpack_assert(count > UINT16_MAX);
    mpack_store_u8(p, 0xc6);
    mpack_store_u32(p + 1, count);
}

MPACK_STATIC_INLINE void mpack_encode_fixext1(char* p, int8_t exttype) {
    mpack_store_u8(p, 0xd4);
    mpack_store_i8(p + 1, exttype);
}

MPACK_STATIC_INLINE void mpack_encode_fixext2(char* p, int8_t exttype) {
    mpack_store_u8(p, 0xd5);
    mpack_store_i8(p + 1, exttype);
}

MPACK_STATIC_INLINE void mpack_encode_fixext4(char* p, int8_t exttype) {
    mpack_store_u8(p, 0xd6);
    mpack_store_i8(p + 1, exttype);
}

MPACK_STATIC_INLINE void mpack_encode_fixext8(char* p, int8_t exttype) {
    mpack_store_u8(p, 0xd7);
    mpack_store_i8(p + 1, exttype);
}

MPACK_STATIC_INLINE void mpack_encode_fixext16(char* p, int8_t exttype) {
    mpack_store_u8(p, 0xd8);
    mpack_store_i8(p + 1, exttype);
}

MPACK_STATIC_INLINE void mpack_encode_ext8(char* p, int8_t exttype, uint8_t count) {
    mpack_assert(count != 1 && count != 2 && count != 4 && count != 8 && count != 16);
    mpack_store_u8(p, 0xc7);
    mpack_store_u8(p + 1, count);
    mpack_store_i8(p + 2, exttype);
}

MPACK_STATIC_INLINE void mpack_encode_ext16(char* p, int8_t exttype, uint16_t count) {
    mpack_assert(count > UINT8_MAX);
    mpack_store_u8(p, 0xc8);
    mpack_store_u16(p + 1, count);
    mpack_store_i8(p + 3, exttype);
}

MPACK_STATIC_INLINE void mpack_encode_ext32(char* p, int8_t exttype, uint32_t count) {
    mpack_assert(count > UINT16_MAX);
    mpack_store_u8(p, 0xc9);
    mpack_store_u32(p + 1, count);
    mpack_store_i8(p + 5, exttype);
}



/*
 * Write functions
 */

// This is a macro wrapper to the encode functions to encode
// directly into the buffer. If mpack_writer_ensure() fails
// it will flag an error so we don't have to do anything.
#define MPACK_WRITE_ENCODED(encode_fn, size, ...) do {                                    \
    if (mpack_writer_buffer_left(writer) >= size || mpack_writer_ensure(writer, size)) {  \
        encode_fn(writer->buffer + writer->used, __VA_ARGS__);                            \
        writer->used += size;                                                             \
    }                                                                                     \
} while (0)

void mpack_write_u8(mpack_writer_t* writer, uint8_t value) {
    #if MPACK_OPTIMIZE_FOR_SIZE
    mpack_write_u64(writer, value);
    #else
    mpack_writer_track_element(writer);
    if (value <= 127) {
        MPACK_WRITE_ENCODED(mpack_encode_fixuint, MPACK_TAG_SIZE_FIXUINT, value);
    } else {
        MPACK_WRITE_ENCODED(mpack_encode_u8, MPACK_TAG_SIZE_U8, value);
    }
    #endif
}

void mpack_write_u16(mpack_writer_t* writer, uint16_t value) {
    #if MPACK_OPTIMIZE_FOR_SIZE
    mpack_write_u64(writer, value);
    #else
    mpack_writer_track_element(writer);
    if (value <= 127) {
        MPACK_WRITE_ENCODED(mpack_encode_fixuint, MPACK_TAG_SIZE_FIXUINT, (uint8_t)value);
    } else if (value <= UINT8_MAX) {
        MPACK_WRITE_ENCODED(mpack_encode_u8, MPACK_TAG_SIZE_U8, (uint8_t)value);
    } else {
        MPACK_WRITE_ENCODED(mpack_encode_u16, MPACK_TAG_SIZE_U16, value);
    }
    #endif
}

void mpack_write_u32(mpack_writer_t* writer, uint32_t value) {
    #if MPACK_OPTIMIZE_FOR_SIZE
    mpack_write_u64(writer, value);
    #else
    mpack_writer_track_element(writer);
    if (value <= 127) {
        MPACK_WRITE_ENCODED(mpack_encode_fixuint, MPACK_TAG_SIZE_FIXUINT, (uint8_t)value);
    } else if (value <= UINT8_MAX) {
        MPACK_WRITE_ENCODED(mpack_encode_u8, MPACK_TAG_SIZE_U8, (uint8_t)value);
    } else if (value <= UINT16_MAX) {
        MPACK_WRITE_ENCODED(mpack_encode_u16, MPACK_TAG_SIZE_U16, (uint16_t)value);
    } else {
        MPACK_WRITE_ENCODED(mpack_encode_u32, MPACK_TAG_SIZE_U32, value);
    }
    #endif
}

void mpack_write_u64(mpack_writer_t* writer, uint64_t value) {
    mpack_writer_track_element(writer);

    if (value <= 127) {
        MPACK_WRITE_ENCODED(mpack_encode_fixuint, MPACK_TAG_SIZE_FIXUINT, (uint8_t)value);
    } else if (value <= UINT8_MAX) {
        MPACK_WRITE_ENCODED(mpack_encode_u8, MPACK_TAG_SIZE_U8, (uint8_t)value);
    } else if (value <= UINT16_MAX) {
        MPACK_WRITE_ENCODED(mpack_encode_u16, MPACK_TAG_SIZE_U16, (uint16_t)value);
    } else if (value <= UINT32_MAX) {
        MPACK_WRITE_ENCODED(mpack_encode_u32, MPACK_TAG_SIZE_U32, (uint32_t)value);
    } else {
        MPACK_WRITE_ENCODED(mpack_encode_u64, MPACK_TAG_SIZE_U64, value);
    }
}

void mpack_write_i8(mpack_writer_t* writer, int8_t value) {
    #if MPACK_OPTIMIZE_FOR_SIZE
    mpack_write_i64(writer, value);
    #else
    mpack_writer_track_element(writer);
    if (value >= -32) {
        // we encode positive and negative fixints together
        MPACK_WRITE_ENCODED(mpack_encode_fixint, MPACK_TAG_SIZE_FIXINT, (int8_t)value);
    } else {
        MPACK_WRITE_ENCODED(mpack_encode_i8, MPACK_TAG_SIZE_I8, (int8_t)value);
    }
    #endif
}

void mpack_write_i16(mpack_writer_t* writer, int16_t value) {
    #if MPACK_OPTIMIZE_FOR_SIZE
    mpack_write_i64(writer, value);
    #else
    mpack_writer_track_element(writer);
    if (value >= -32) {
        if (value <= 127) {
            // we encode positive and negative fixints together
            MPACK_WRITE_ENCODED(mpack_encode_fixint, MPACK_TAG_SIZE_FIXINT, (int8_t)value);
        } else if (value <= UINT8_MAX) {
            MPACK_WRITE_ENCODED(mpack_encode_u8, MPACK_TAG_SIZE_U8, (uint8_t)value);
        } else {
            MPACK_WRITE_ENCODED(mpack_encode_u16, MPACK_TAG_SIZE_U16, (uint16_t)value);
        }
    } else if (value >= INT8_MIN) {
        MPACK_WRITE_ENCODED(mpack_encode_i8, MPACK_TAG_SIZE_I8, (int8_t)value);
    } else {
        MPACK_WRITE_ENCODED(mpack_encode_i16, MPACK_TAG_SIZE_I16, (int16_t)value);
    }
    #endif
}

void mpack_write_i32(mpack_writer_t* writer, int32_t value) {
    #if MPACK_OPTIMIZE_FOR_SIZE
    mpack_write_i64(writer, value);
    #else
    mpack_writer_track_element(writer);
    if (value >= -32) {
        if (value <= 127) {
            // we encode positive and negative fixints together
            MPACK_WRITE_ENCODED(mpack_encode_fixint, MPACK_TAG_SIZE_FIXINT, (int8_t)value);
        } else if (value <= UINT8_MAX) {
            MPACK_WRITE_ENCODED(mpack_encode_u8, MPACK_TAG_SIZE_U8, (uint8_t)value);
        } else if (value <= UINT16_MAX) {
            MPACK_WRITE_ENCODED(mpack_encode_u16, MPACK_TAG_SIZE_U16, (uint16_t)value);
        } else {
            MPACK_WRITE_ENCODED(mpack_encode_u32, MPACK_TAG_SIZE_U32, (uint32_t)value);
        }
    } else if (value >= INT8_MIN) {
        MPACK_WRITE_ENCODED(mpack_encode_i8, MPACK_TAG_SIZE_I8, (int8_t)value);
    } else if (value >= INT16_MIN) {
        MPACK_WRITE_ENCODED(mpack_encode_i16, MPACK_TAG_SIZE_I16, (int16_t)value);
    } else {
        MPACK_WRITE_ENCODED(mpack_encode_i32, MPACK_TAG_SIZE_I32, value);
    }
    #endif
}

void mpack_write_i64(mpack_writer_t* writer, int64_t value) {
    #if MPACK_OPTIMIZE_FOR_SIZE
    if (value > 127) {
        // for non-fix positive ints we call the u64 writer to save space
        mpack_write_u64(writer, (uint64_t)value);
        return;
    }
    #endif

    mpack_writer_track_element(writer);
    if (value >= -32) {
        #if MPACK_OPTIMIZE_FOR_SIZE
        MPACK_WRITE_ENCODED(mpack_encode_fixint, MPACK_TAG_SIZE_FIXINT, (int8_t)value);
        #else
        if (value <= 127) {
            MPACK_WRITE_ENCODED(mpack_encode_fixint, MPACK_TAG_SIZE_FIXINT, (int8_t)value);
        } else if (value <= UINT8_MAX) {
            MPACK_WRITE_ENCODED(mpack_encode_u8, MPACK_TAG_SIZE_U8, (uint8_t)value);
        } else if (value <= UINT16_MAX) {
            MPACK_WRITE_ENCODED(mpack_encode_u16, MPACK_TAG_SIZE_U16, (uint16_t)value);
        } else if (value <= UINT32_MAX) {
            MPACK_WRITE_ENCODED(mpack_encode_u32, MPACK_TAG_SIZE_U32, (uint32_t)value);
        } else {
            MPACK_WRITE_ENCODED(mpack_encode_u64, MPACK_TAG_SIZE_U64, (uint64_t)value);
        }
        #endif
    } else if (value >= INT8_MIN) {
        MPACK_WRITE_ENCODED(mpack_encode_i8, MPACK_TAG_SIZE_I8, (int8_t)value);
    } else if (value >= INT16_MIN) {
        MPACK_WRITE_ENCODED(mpack_encode_i16, MPACK_TAG_SIZE_I16, (int16_t)value);
    } else if (value >= INT32_MIN) {
        MPACK_WRITE_ENCODED(mpack_encode_i32, MPACK_TAG_SIZE_I32, (int32_t)value);
    } else {
        MPACK_WRITE_ENCODED(mpack_encode_i64, MPACK_TAG_SIZE_I64, value);
    }
}

void mpack_write_float(mpack_writer_t* writer, float value) {
    mpack_writer_track_element(writer);
    MPACK_WRITE_ENCODED(mpack_encode_float, MPACK_TAG_SIZE_FLOAT, value);
}
void mpack_write_double(mpack_writer_t* writer, double value) {
    mpack_writer_track_element(writer);
    MPACK_WRITE_ENCODED(mpack_encode_double, MPACK_TAG_SIZE_DOUBLE, value);
}

void mpack_start_array(mpack_writer_t* writer, uint32_t count) {
    mpack_writer_track_element(writer);

    if (count <= 15) {
        MPACK_WRITE_ENCODED(mpack_encode_fixarray, MPACK_TAG_SIZE_FIXARRAY, (uint8_t)count);
    } else if (count <= UINT16_MAX) {
        MPACK_WRITE_ENCODED(mpack_encode_array16, MPACK_TAG_SIZE_ARRAY16, (uint16_t)count);
    } else {
        MPACK_WRITE_ENCODED(mpack_encode_array32, MPACK_TAG_SIZE_ARRAY32, (uint32_t)count);
    }

    mpack_writer_track_push(writer, mpack_type_array, count);
}

void mpack_start_map(mpack_writer_t* writer, uint32_t count) {
    mpack_writer_track_element(writer);

    if (count <= 15) {
        MPACK_WRITE_ENCODED(mpack_encode_fixmap, MPACK_TAG_SIZE_FIXMAP, (uint8_t)count);
    } else if (count <= UINT16_MAX) {
        MPACK_WRITE_ENCODED(mpack_encode_map16, MPACK_TAG_SIZE_MAP16, (uint16_t)count);
    } else {
        MPACK_WRITE_ENCODED(mpack_encode_map32, MPACK_TAG_SIZE_MAP32, (uint32_t)count);
    }

    mpack_writer_track_push(writer, mpack_type_map, count);
}

void mpack_start_str(mpack_writer_t* writer, uint32_t count) {
    mpack_writer_track_element(writer);

    if (count <= 31) {
        MPACK_WRITE_ENCODED(mpack_encode_fixstr, MPACK_TAG_SIZE_FIXSTR, (uint8_t)count);
    } else if (count <= UINT8_MAX) {
        // TODO: str8 had no counterpart in MessagePack 1.0; there was only
        // fixraw, raw16 and raw32. This should not be used in compatibility mode.
        MPACK_WRITE_ENCODED(mpack_encode_str8, MPACK_TAG_SIZE_STR8, (uint8_t)count);
    } else if (count <= UINT16_MAX) {
        MPACK_WRITE_ENCODED(mpack_encode_str16, MPACK_TAG_SIZE_STR16, (uint16_t)count);
    } else {
        MPACK_WRITE_ENCODED(mpack_encode_str32, MPACK_TAG_SIZE_STR32, (uint32_t)count);
    }

    mpack_writer_track_push(writer, mpack_type_str, count);
}

void mpack_start_bin(mpack_writer_t* writer, uint32_t count) {
    mpack_writer_track_element(writer);

    if (count <= UINT8_MAX) {
        MPACK_WRITE_ENCODED(mpack_encode_bin8, MPACK_TAG_SIZE_BIN8, (uint8_t)count);
    } else if (count <= UINT16_MAX) {
        MPACK_WRITE_ENCODED(mpack_encode_bin16, MPACK_TAG_SIZE_BIN16, (uint16_t)count);
    } else {
        MPACK_WRITE_ENCODED(mpack_encode_bin32, MPACK_TAG_SIZE_BIN32, (uint32_t)count);
    }

    mpack_writer_track_push(writer, mpack_type_bin, count);
}

void mpack_start_ext(mpack_writer_t* writer, int8_t exttype, uint32_t count) {
    mpack_writer_track_element(writer);

    if (count == 1) {
        MPACK_WRITE_ENCODED(mpack_encode_fixext1, MPACK_TAG_SIZE_FIXEXT1, exttype);
    } else if (count == 2) {
        MPACK_WRITE_ENCODED(mpack_encode_fixext2, MPACK_TAG_SIZE_FIXEXT2, exttype);
    } else if (count == 4) {
        MPACK_WRITE_ENCODED(mpack_encode_fixext4, MPACK_TAG_SIZE_FIXEXT4, exttype);
    } else if (count == 8) {
        MPACK_WRITE_ENCODED(mpack_encode_fixext8, MPACK_TAG_SIZE_FIXEXT8, exttype);
    } else if (count == 16) {
        MPACK_WRITE_ENCODED(mpack_encode_fixext16, MPACK_TAG_SIZE_FIXEXT16, exttype);
    } else if (count <= UINT8_MAX) {
        MPACK_WRITE_ENCODED(mpack_encode_ext8, MPACK_TAG_SIZE_EXT8, exttype, (uint8_t)count);
    } else if (count <= UINT16_MAX) {
        MPACK_WRITE_ENCODED(mpack_encode_ext16, MPACK_TAG_SIZE_EXT16, exttype, (uint16_t)count);
    } else {
        MPACK_WRITE_ENCODED(mpack_encode_ext32, MPACK_TAG_SIZE_EXT32, exttype, (uint32_t)count);
    }

    mpack_writer_track_push(writer, mpack_type_ext, count);
}



/*
 * Compound helpers and other functions
 */

void mpack_write_str(mpack_writer_t* writer, const char* data, uint32_t count) {
    mpack_assert(data != NULL, "data for string of length %i is NULL", (int)count);
    mpack_start_str(writer, count);
    mpack_write_bytes(writer, data, count);
    mpack_finish_str(writer);
}

void mpack_write_bin(mpack_writer_t* writer, const char* data, uint32_t count) {
    mpack_assert(data != NULL, "data pointer for bin of %i bytes is NULL", (int)count);
    mpack_start_bin(writer, count);
    mpack_write_bytes(writer, data, count);
    mpack_finish_bin(writer);
}

void mpack_write_ext(mpack_writer_t* writer, int8_t exttype, const char* data, uint32_t count) {
    mpack_assert(data != NULL, "data pointer for ext of type %i and %i bytes is NULL", exttype, (int)count);
    mpack_start_ext(writer, exttype, count);
    mpack_write_bytes(writer, data, count);
    mpack_finish_ext(writer);
}

void mpack_write_bytes(mpack_writer_t* writer, const char* data, size_t count) {
    mpack_assert(data != NULL, "data pointer for %i bytes is NULL", (int)count);
    mpack_writer_track_bytes(writer, count);
    mpack_write_native(writer, data, count);
}

void mpack_write_cstr(mpack_writer_t* writer, const char* cstr) {
    mpack_assert(cstr != NULL, "cstr pointer is NULL");
    size_t length = mpack_strlen(cstr);
    if (length > UINT32_MAX)
        mpack_writer_flag_error(writer, mpack_error_invalid);
    mpack_write_str(writer, cstr, (uint32_t)length);
}

void mpack_write_cstr_or_nil(mpack_writer_t* writer, const char* cstr) {
    if (cstr)
        mpack_write_cstr(writer, cstr);
    else
        mpack_write_nil(writer);
}

void mpack_write_utf8(mpack_writer_t* writer, const char* str, uint32_t length) {
    mpack_assert(str != NULL, "data for string of length %i is NULL", (int)length);
    if (!mpack_utf8_check(str, length)) {
        mpack_writer_flag_error(writer, mpack_error_invalid);
        return;
    }
    mpack_write_str(writer, str, length);
}

void mpack_write_utf8_cstr(mpack_writer_t* writer, const char* cstr) {
    mpack_assert(cstr != NULL, "cstr pointer is NULL");
    size_t length = mpack_strlen(cstr);
    if (length > UINT32_MAX)
        mpack_writer_flag_error(writer, mpack_error_invalid);
    mpack_write_utf8(writer, cstr, (uint32_t)length);
}

void mpack_write_utf8_cstr_or_nil(mpack_writer_t* writer, const char* cstr) {
    if (cstr)
        mpack_write_utf8_cstr(writer, cstr);
    else
        mpack_write_nil(writer);
}

#endif


/* mpack-reader.c */

#define MPACK_INTERNAL 1

/* #include "mpack-reader.h" */

#if MPACK_READER

static void mpack_reader_skip_using_fill(mpack_reader_t* reader, size_t count);

void mpack_reader_init(mpack_reader_t* reader, char* buffer, size_t size, size_t count) {
    mpack_assert(buffer != NULL, "buffer is NULL");

    mpack_memset(reader, 0, sizeof(*reader));
    reader->buffer = buffer;
    reader->size = size;
    reader->left = count;

    #if MPACK_READ_TRACKING
    mpack_reader_flag_if_error(reader, mpack_track_init(&reader->track));
    #endif

    mpack_log("===========================\n");
    mpack_log("initializing reader with buffer size %i\n", (int)size);
}

void mpack_reader_init_error(mpack_reader_t* reader, mpack_error_t error) {
    mpack_memset(reader, 0, sizeof(*reader));
    reader->error = error;

    mpack_log("===========================\n");
    mpack_log("initializing reader error state %i\n", (int)error);
}

void mpack_reader_init_data(mpack_reader_t* reader, const char* data, size_t count) {
    mpack_assert(data != NULL, "data is NULL");

    mpack_memset(reader, 0, sizeof(*reader));
    reader->left = count;

    // unfortunately we have to cast away the const to store the buffer,
    // but we won't be modifying it because there's no fill function.
    // the buffer size is left at 0 to ensure no fill function can be
    // set or used (see mpack_reader_set_fill().)
    #ifdef __cplusplus
    reader->buffer = const_cast<char*>(data);
    #else
    reader->buffer = (char*)(uintptr_t)data;
    #endif

    #if MPACK_READ_TRACKING
    mpack_reader_flag_if_error(reader, mpack_track_init(&reader->track));
    #endif

    mpack_log("===========================\n");
    mpack_log("initializing reader with data size %i\n", (int)count);
}

void mpack_reader_set_fill(mpack_reader_t* reader, mpack_reader_fill_t fill) {
    MPACK_STATIC_ASSERT(MPACK_READER_MINIMUM_BUFFER_SIZE >= MPACK_MAXIMUM_TAG_SIZE,
            "minimum buffer size must fit any tag!");

    if (reader->size == 0) {
        mpack_break("cannot use fill function without a writeable buffer!");
        mpack_reader_flag_error(reader, mpack_error_bug);
        return;
    }

    if (reader->size < MPACK_READER_MINIMUM_BUFFER_SIZE) {
        mpack_break("buffer size is %i, but minimum buffer size for fill is %i",
                (int)reader->size, MPACK_READER_MINIMUM_BUFFER_SIZE);
        mpack_reader_flag_error(reader, mpack_error_bug);
        return;
    }

    reader->fill = fill;
}

void mpack_reader_set_skip(mpack_reader_t* reader, mpack_reader_skip_t skip) {
    mpack_assert(reader->size != 0, "cannot use skip function without a writeable buffer!");
    #if MPACK_OPTIMIZE_FOR_SIZE
    MPACK_UNUSED(reader);
    MPACK_UNUSED(skip);
    #else
    reader->skip = skip;
    #endif
}

#if MPACK_STDIO
static size_t mpack_file_reader_fill(mpack_reader_t* reader, char* buffer, size_t count) {
    return fread((void*)buffer, 1, count, (FILE*)reader->context);
}

#if !MPACK_OPTIMIZE_FOR_SIZE
static void mpack_file_reader_skip(mpack_reader_t* reader, size_t count) {
    if (mpack_reader_error(reader) != mpack_ok)
        return;
    FILE* file = (FILE*)reader->context;

    // We call ftell() to test whether the stream is seekable
    // without causing a file error.
    if (ftell(file) >= 0) {
        mpack_log("seeking forward %i bytes\n", (int)count);
        if (fseek(file, (long int)count, SEEK_CUR) == 0)
            return;
        mpack_log("fseek() didn't return zero!\n");
        if (ferror(file)) {
            mpack_reader_flag_error(reader, mpack_error_io);
            return;
        }
    }

    // If the stream is not seekable, fall back to the fill function.
    mpack_reader_skip_using_fill(reader, count);
}
#endif

static void mpack_file_reader_teardown(mpack_reader_t* reader) {
    FILE* file = (FILE*)reader->context;

    if (file) {
        int ret = fclose(file);
        reader->context = NULL;
        if (ret != 0)
            mpack_reader_flag_error(reader, mpack_error_io);
    }

    MPACK_FREE(reader->buffer);
    reader->buffer = NULL;
    reader->size = 0;
    reader->fill = NULL;
}

void mpack_reader_init_file(mpack_reader_t* reader, const char* filename) {
    mpack_assert(filename != NULL, "filename is NULL");

    size_t capacity = MPACK_BUFFER_SIZE;
    char* buffer = (char*)MPACK_MALLOC(capacity);
    if (buffer == NULL) {
        mpack_reader_init_error(reader, mpack_error_memory);
        return;
    }

    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        MPACK_FREE(buffer);
        mpack_reader_init_error(reader, mpack_error_io);
        return;
    }

    mpack_reader_init(reader, buffer, capacity, 0);
    mpack_reader_set_context(reader, file);
    mpack_reader_set_fill(reader, mpack_file_reader_fill);
    #if !MPACK_OPTIMIZE_FOR_SIZE
    mpack_reader_set_skip(reader, mpack_file_reader_skip);
    #endif
    mpack_reader_set_teardown(reader, mpack_file_reader_teardown);
}
#endif

mpack_error_t mpack_reader_destroy(mpack_reader_t* reader) {

    // clean up tracking, asserting if we're not already in an error state
    #if MPACK_READ_TRACKING
    mpack_reader_flag_if_error(reader, mpack_track_destroy(&reader->track, mpack_reader_error(reader) != mpack_ok));
    #endif

    if (reader->teardown)
        reader->teardown(reader);
    reader->teardown = NULL;

    return reader->error;
}

size_t mpack_reader_remaining(mpack_reader_t* reader, const char** data) {
    if (mpack_reader_error(reader) != mpack_ok)
        return 0;

    #if MPACK_READ_TRACKING
    if (mpack_reader_flag_if_error(reader, mpack_track_check_empty(&reader->track)) != mpack_ok)
        return 0;
    #endif

    if (data)
        *data = reader->buffer + reader->pos;
    return reader->left;
}

void mpack_reader_flag_error(mpack_reader_t* reader, mpack_error_t error) {
    mpack_log("reader %p setting error %i: %s\n", reader, (int)error, mpack_error_to_string(error));

    if (reader->error == mpack_ok) {
        reader->error = error;
        reader->left = 0;
        if (reader->error_fn)
            reader->error_fn(reader, error);
    }
}

// A helper to call the reader fill function. This makes sure it's
// implemented and guards against overflow in case it returns -1.
static size_t mpack_fill(mpack_reader_t* reader, char* p, size_t count) {
    mpack_assert(reader->fill != NULL, "mpack_fill() called with no fill function?");

    size_t ret = reader->fill(reader, p, count);
    if (ret == ((size_t)(-1)))
        return 0;

    return ret;
}

// Fills the buffer when there is already some data in the buffer. The
// existing data is moved to the start of the buffer.
static void mpack_partial_fill(mpack_reader_t* reader) {
    mpack_memmove(reader->buffer, reader->buffer + reader->pos, reader->left);
    reader->pos = 0;
    reader->left += mpack_fill(reader, reader->buffer + reader->left, reader->size - reader->left);
}

bool mpack_reader_ensure_straddle(mpack_reader_t* reader, size_t count) {
    mpack_assert(count != 0, "cannot ensure zero bytes!");
    mpack_assert(reader->error == mpack_ok, "reader cannot be in an error state!");

    if (count <= reader->left) {
        mpack_assert(0,
                "big ensure requested for %i bytes, but there are %i bytes "
                "left in buffer. call mpack_reader_ensure() instead",
                (int)count, (int)reader->left);
        mpack_reader_flag_error(reader, mpack_error_bug);
        return false;
    }

    // we'll need a fill function to get more data. if there's no
    // fill function, the buffer should contain an entire MessagePack
    // object, so we raise mpack_error_invalid instead of mpack_error_io
    // on truncated data.
    if (reader->fill == NULL) {
        mpack_reader_flag_error(reader, mpack_error_invalid);
        return false;
    }

    mpack_assert(count <= reader->size, "cannot ensure byte count %i larger than buffer size %i",
            (int)count, (int)reader->size);

    // re-fill as much as possible
    mpack_partial_fill(reader);

    if (reader->left < count) {
        mpack_reader_flag_error(reader, mpack_error_io);
        return false;
    }

    return true;
}

// Reads count bytes into p. Used when there are not enough bytes
// left in the buffer to satisfy a read.
void mpack_read_native_big(mpack_reader_t* reader, char* p, size_t count) {
    mpack_assert(count == 0 || p != NULL, "data pointer for %i bytes is NULL", (int)count);

    if (mpack_reader_error(reader) != mpack_ok) {
        mpack_memset(p, 0, count);
        return;
    }

    mpack_log("big read for %i bytes into %p, %i left in buffer, buffer size %i\n",
            (int)count, p, (int)reader->left, (int)reader->size);

    if (count <= reader->left) {
        mpack_assert(0,
                "big read requested for %i bytes, but there are %i bytes "
                "left in buffer. call mpack_read_native() instead",
                (int)count, (int)reader->left);
        mpack_reader_flag_error(reader, mpack_error_bug);
        mpack_memset(p, 0, count);
        return;
    }

    // we'll need a fill function to get more data. if there's no
    // fill function, the buffer should contain an entire MessagePack
    // object, so we raise mpack_error_invalid instead of mpack_error_io
    // on truncated data.
    if (reader->fill == NULL) {
        mpack_reader_flag_error(reader, mpack_error_invalid);
        mpack_memset(p, 0, count);
        return;
    }

    if (reader->size == 0) {
        // somewhat debatable what error should be returned here. when
        // initializing a reader with an in-memory buffer it's not
        // necessarily a bug if the data is blank; it might just have
        // been truncated to zero. for this reason we return the same
        // error as if the data was truncated.
        mpack_reader_flag_error(reader, mpack_error_io);
        mpack_memset(p, 0, count);
        return;
    }

    // flush what's left of the buffer
    if (reader->left > 0) {
        mpack_log("flushing %i bytes remaining in buffer\n", (int)reader->left);
        mpack_memcpy(p, reader->buffer + reader->pos, reader->left);
        count -= reader->left;
        p += reader->left;
        reader->pos += reader->left;
        reader->left = 0;
    }

    // we read only in multiples of the buffer size. read the middle portion, if any
    size_t middle = count - (count % reader->size);
    if (middle > 0) {
        mpack_log("reading %i bytes in middle\n", (int)middle);
        if (mpack_fill(reader, p, middle) < middle) {
            mpack_reader_flag_error(reader, mpack_error_io);
            mpack_memset(p, 0, count);
            return;
        }
        count -= middle;
        p += middle;
        if (count == 0)
            return;
    }

    // fill the buffer
    reader->pos = 0;
    reader->left = mpack_fill(reader, reader->buffer, reader->size);
    mpack_log("filled %i bytes into buffer\n", (int)reader->left);
    if (reader->left < count) {
        mpack_reader_flag_error(reader, mpack_error_io);
        mpack_memset(p, 0, count);
        return;
    }

    // serve the remainder
    mpack_log("serving %i remaining bytes from %p to %p\n", (int)count, reader->buffer+reader->pos,p);
    mpack_memcpy(p, reader->buffer + reader->pos, count);
    reader->pos += count;
    reader->left -= count;
}

void mpack_skip_bytes(mpack_reader_t* reader, size_t count) {
    if (mpack_reader_error(reader) != mpack_ok)
        return;
    mpack_log("skip requested for %i bytes\n", (int)count);
    mpack_reader_track_bytes(reader, count);

    // check if we have enough in the buffer already
    if (reader->left >= count) {
        mpack_log("skipping %i bytes still in buffer\n", (int)count);
        reader->left -= count;
        reader->pos += count;
        return;
    }

    // we'll need at least a fill function to skip more data. if there's
    // no fill function, the buffer should contain an entire MessagePack
    // object, so we raise mpack_error_invalid instead of mpack_error_io
    // on truncated data. (see mpack_read_native_big())
    if (reader->fill == NULL) {
        mpack_log("reader has no fill function!\n");
        mpack_reader_flag_error(reader, mpack_error_invalid);
        return;
    }

    // discard whatever's left in the buffer
    mpack_log("discarding %i bytes still in buffer\n", (int)reader->left);
    count -= reader->left;
    reader->pos += reader->left;
    reader->left = 0;

    #if !MPACK_OPTIMIZE_FOR_SIZE
    // use the skip function if we've got one, and if we're trying
    // to skip a lot of data. if we only need to skip some tiny
    // fraction of the buffer size, it's probably better to just
    // fill the buffer and skip from it instead of trying to seek.
    if (reader->skip && count > reader->size / 16) {
        mpack_log("calling skip function for %i bytes\n", (int)count);
        reader->skip(reader, count);
        return;
    }
    #endif

    mpack_reader_skip_using_fill(reader, count);
}

static void mpack_reader_skip_using_fill(mpack_reader_t* reader, size_t count) {
    mpack_assert(reader->fill != NULL, "missing fill function!");
    mpack_assert(reader->left == 0, "there are bytes left in the buffer!");
    mpack_assert(reader->error == mpack_ok, "should not have called this in an error state (%i)", reader->error);
    mpack_log("skip using fill for %i bytes\n", (int)count);

    // fill and discard multiples of the buffer size
    while (count > reader->size) {
        mpack_log("filling and discarding buffer of %i bytes\n", (int)reader->size);
        if (mpack_fill(reader, reader->buffer, reader->size) < reader->size) {
            mpack_reader_flag_error(reader, mpack_error_io);
            return;
        }
        count -= reader->size;
    }

    // fill the buffer as much as possible
    reader->pos = 0;
    reader->left = mpack_fill(reader, reader->buffer, reader->size);
    if (reader->left < count) {
        mpack_reader_flag_error(reader, mpack_error_io);
        return;
    }
    mpack_log("filled %i bytes into buffer; discarding %i bytes\n", (int)reader->left, (int)count);
    reader->pos += count;
    reader->left -= count;
}

void mpack_read_bytes(mpack_reader_t* reader, char* p, size_t count) {
    mpack_assert(p != NULL, "destination for read of %i bytes is NULL", (int)count);
    mpack_reader_track_bytes(reader, count);
    mpack_read_native(reader, p, count);
}

void mpack_read_utf8(mpack_reader_t* reader, char* p, size_t byte_count) {
    mpack_assert(p != NULL, "destination for read of %i bytes is NULL", (int)byte_count);
    mpack_reader_track_str_bytes_all(reader, byte_count);
    mpack_read_native(reader, p, byte_count);

    if (mpack_reader_error(reader) == mpack_ok && !mpack_utf8_check(p, byte_count))
        mpack_reader_flag_error(reader, mpack_error_type);
}

static void mpack_read_cstr_unchecked(mpack_reader_t* reader, char* buf, size_t buffer_size, size_t byte_count) {
    mpack_assert(buf != NULL, "destination for read of %i bytes is NULL", (int)byte_count);
    mpack_assert(buffer_size >= 1, "buffer size is zero; you must have room for at least a null-terminator");

    if (mpack_reader_error(reader)) {
        buf[0] = 0;
        return;
    }

    if (byte_count > buffer_size - 1) {
        mpack_reader_flag_error(reader, mpack_error_too_big);
        buf[0] = 0;
        return;
    }

    mpack_reader_track_str_bytes_all(reader, byte_count);
    mpack_read_native(reader, buf, byte_count);
    buf[byte_count] = 0;
}

void mpack_read_cstr(mpack_reader_t* reader, char* buf, size_t buffer_size, size_t byte_count) {
    mpack_read_cstr_unchecked(reader, buf, buffer_size, byte_count);

    // check for null bytes
    if (mpack_reader_error(reader) == mpack_ok && !mpack_str_check_no_null(buf, byte_count)) {
        buf[0] = 0;
        mpack_reader_flag_error(reader, mpack_error_type);
    }
}

void mpack_read_utf8_cstr(mpack_reader_t* reader, char* buf, size_t buffer_size, size_t byte_count) {
    mpack_read_cstr_unchecked(reader, buf, buffer_size, byte_count);

    // check encoding
    if (mpack_reader_error(reader) == mpack_ok && !mpack_utf8_check_no_null(buf, byte_count)) {
        buf[0] = 0;
        mpack_reader_flag_error(reader, mpack_error_type);
    }
}

#ifdef MPACK_MALLOC
// Reads native bytes with error callback disabled. This allows MPack reader functions
// to hold an allocated buffer and read native data into it without leaking it in
// case of a non-local jump (longjmp, throw) out of an error handler.
static void mpack_read_native_noerrorfn(mpack_reader_t* reader, char* p, size_t count) {
    mpack_assert(reader->error == mpack_ok, "cannot call if an error is already flagged!");
    mpack_reader_error_t error_fn = reader->error_fn;
    reader->error_fn = NULL;
    mpack_read_native(reader, p, count);
    reader->error_fn = error_fn;
}

char* mpack_read_bytes_alloc_impl(mpack_reader_t* reader, size_t count, bool null_terminated) {

    // track the bytes first in case it jumps
    mpack_reader_track_bytes(reader, count);
    if (mpack_reader_error(reader) != mpack_ok)
        return NULL;

    // cannot allocate zero bytes. this is not an error.
    if (count == 0 && null_terminated == false)
        return NULL;

    // allocate data
    char* data = (char*)MPACK_MALLOC(count + (null_terminated ? 1 : 0)); // TODO: can this overflow?
    if (data == NULL) {
        mpack_reader_flag_error(reader, mpack_error_memory);
        return NULL;
    }

    // read with error callback disabled so we don't leak our buffer
    mpack_read_native_noerrorfn(reader, data, count);

    // report flagged errors
    if (mpack_reader_error(reader) != mpack_ok) {
        MPACK_FREE(data);
        if (reader->error_fn)
            reader->error_fn(reader, mpack_reader_error(reader));
        return NULL;
    }

    if (null_terminated)
        data[count] = '\0';
    return data;
}
#endif

// internal inplace reader for when it straddles the end of the buffer
static const char* mpack_read_bytes_inplace_big(mpack_reader_t* reader, size_t count) {

    // we should only arrive here from inplace straddle; this should already be checked
    mpack_assert(mpack_reader_error(reader) == mpack_ok, "already in error state? %s",
            mpack_error_to_string(mpack_reader_error(reader)));
    mpack_assert(reader->left < count, "already enough bytes in buffer: %i left, %i count", (int)reader->left, (int)count);

    // we'll need a fill function to get more data. if there's no
    // fill function, the buffer should contain an entire MessagePack
    // object, so we raise mpack_error_invalid instead of mpack_error_io
    // on truncated data.
    if (reader->fill == NULL) {
        mpack_reader_flag_error(reader, mpack_error_invalid);
        return NULL;
    }

    // make sure the buffer is big enough to actually fit the data
    if (count > reader->size) {
        mpack_reader_flag_error(reader, mpack_error_too_big);
        return NULL;
    }

    // re-fill as much as possible
    mpack_partial_fill(reader);

    if (reader->left < count) {
        mpack_reader_flag_error(reader, mpack_error_io);
        return NULL;
    }
    reader->pos += count;
    reader->left -= count;
    return reader->buffer;
}

// read inplace without tracking (since there are different
// tracking modes for different inplace readers)
static const char* mpack_read_bytes_inplace_notrack(mpack_reader_t* reader, size_t count) {
    if (mpack_reader_error(reader) != mpack_ok)
        return NULL;

    // if we have enough bytes already in the buffer, we can return it directly.
    if (reader->left >= count) {
        reader->pos += count;
        reader->left -= count;
        return reader->buffer + reader->pos - count;
    }

    return mpack_read_bytes_inplace_big(reader, count);
}

const char* mpack_read_bytes_inplace(mpack_reader_t* reader, size_t count) {
    mpack_reader_track_bytes(reader, count);
    return mpack_read_bytes_inplace_notrack(reader, count);
}

const char* mpack_read_utf8_inplace(mpack_reader_t* reader, size_t count) {
    mpack_reader_track_str_bytes_all(reader, count);
    const char* str = mpack_read_bytes_inplace_notrack(reader, count);

    if (mpack_reader_error(reader) == mpack_ok && !mpack_utf8_check(str, count)) {
        mpack_reader_flag_error(reader, mpack_error_type);
        return NULL;
    }

    return str;
}

// Decodes a tag from a byte buffer. The size of the bytes buffer
// must be at least MPACK_MINIMUM_TAG_SIZE.
static size_t mpack_parse_tag(mpack_reader_t* reader, mpack_tag_t* tag) {
    mpack_assert(reader->error == mpack_ok, "reader cannot be in an error state!");

    if (!mpack_reader_ensure(reader, 1))
        return 0;
    uint8_t type = mpack_load_u8(reader->buffer + reader->pos);

    // unfortunately, by far the fastest way to parse a tag is to switch
    // on the first byte, and to explicitly list every possible byte. so for
    // infix types, the list of cases is quite large.
    //
    // in size-optimized builds, we switch on the top four bits first to
    // handle most infix types with a smaller jump table to save space.

    #if MPACK_OPTIMIZE_FOR_SIZE
    switch (type >> 4) {

        // positive fixnum
        case 0x0: case 0x1: case 0x2: case 0x3:
        case 0x4: case 0x5: case 0x6: case 0x7:
            tag->type = mpack_type_uint;
            tag->v.u = type;
            return 1;

        // negative fixnum
        case 0xe: case 0xf:
            tag->type = mpack_type_int;
            tag->v.i = (int32_t)(int8_t)type;
            return 1;

        // fixmap
        case 0x8:
            tag->type = mpack_type_map;
            tag->v.n = type & ~0xf0;
            return 1;

        // fixarray
        case 0x9:
            tag->type = mpack_type_array;
            tag->v.n = type & ~0xf0;
            return 1;

        // fixstr
        case 0xa: case 0xb:
            tag->type = mpack_type_str;
            tag->v.l = type & ~0xe0;
            return 1;

        // not one of the common infix types
        default:
            break;

    }
    #endif

    // handle individual type tags
    switch (type) {

        #if !MPACK_OPTIMIZE_FOR_SIZE
        // positive fixnum
        case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
        case 0x08: case 0x09: case 0x0a: case 0x0b: case 0x0c: case 0x0d: case 0x0e: case 0x0f:
        case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
        case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x1e: case 0x1f:
        case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x26: case 0x27:
        case 0x28: case 0x29: case 0x2a: case 0x2b: case 0x2c: case 0x2d: case 0x2e: case 0x2f:
        case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
        case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
        case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
        case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5e: case 0x5f:
        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67:
        case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6e: case 0x6f:
        case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x76: case 0x77:
        case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7e: case 0x7f:
            tag->type = mpack_type_uint;
            tag->v.u = type;
            return 1;

        // negative fixnum
        case 0xe0: case 0xe1: case 0xe2: case 0xe3: case 0xe4: case 0xe5: case 0xe6: case 0xe7:
        case 0xe8: case 0xe9: case 0xea: case 0xeb: case 0xec: case 0xed: case 0xee: case 0xef:
        case 0xf0: case 0xf1: case 0xf2: case 0xf3: case 0xf4: case 0xf5: case 0xf6: case 0xf7:
        case 0xf8: case 0xf9: case 0xfa: case 0xfb: case 0xfc: case 0xfd: case 0xfe: case 0xff:
            tag->type = mpack_type_int;
            tag->v.i = (int8_t)type;
            return 1;

        // fixmap
        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
        case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8e: case 0x8f:
            tag->type = mpack_type_map;
            tag->v.n = type & ~0xf0;
            return 1;

        // fixarray
        case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
        case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9e: case 0x9f:
            tag->type = mpack_type_array;
            tag->v.n = type & ~0xf0;
            return 1;

        // fixstr
        case 0xa0: case 0xa1: case 0xa2: case 0xa3: case 0xa4: case 0xa5: case 0xa6: case 0xa7:
        case 0xa8: case 0xa9: case 0xaa: case 0xab: case 0xac: case 0xad: case 0xae: case 0xaf:
        case 0xb0: case 0xb1: case 0xb2: case 0xb3: case 0xb4: case 0xb5: case 0xb6: case 0xb7:
        case 0xb8: case 0xb9: case 0xba: case 0xbb: case 0xbc: case 0xbd: case 0xbe: case 0xbf:
            tag->type = mpack_type_str;
            tag->v.l = type & ~0xe0;
            return 1;
        #endif

        // nil
        case 0xc0:
            tag->type = mpack_type_nil;
            return 1;

        // bool
        case 0xc2: case 0xc3:
            tag->type = mpack_type_bool;
            tag->v.b = type & 1;
            return 1;

        // bin8
        case 0xc4:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_BIN8))
                return 0;
            tag->type = mpack_type_bin;
            tag->v.l = mpack_load_u8(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_BIN8;

        // bin16
        case 0xc5:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_BIN16))
                return 0;
            tag->type = mpack_type_bin;
            tag->v.l = mpack_load_u16(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_BIN16;

        // bin32
        case 0xc6:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_BIN32))
                return 0;
            tag->type = mpack_type_bin;
            tag->v.l = mpack_load_u32(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_BIN32;

        // ext8
        case 0xc7:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_EXT8))
                return 0;
            tag->type = mpack_type_ext;
            tag->v.l = mpack_load_u8(reader->buffer + reader->pos + 1);
            tag->exttype = mpack_load_i8(reader->buffer + reader->pos + 2);
            return MPACK_TAG_SIZE_EXT8;

        // ext16
        case 0xc8:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_EXT16))
                return 0;
            tag->type = mpack_type_ext;
            tag->v.l = mpack_load_u16(reader->buffer + reader->pos + 1);
            tag->exttype = mpack_load_i8(reader->buffer + reader->pos + 3);
            return MPACK_TAG_SIZE_EXT16;

        // ext32
        case 0xc9:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_EXT32))
                return 0;
            tag->type = mpack_type_ext;
            tag->v.l = mpack_load_u32(reader->buffer + reader->pos + 1);
            tag->exttype = mpack_load_i8(reader->buffer + reader->pos + 5);
            return MPACK_TAG_SIZE_EXT32;

        // float
        case 0xca:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_FLOAT))
                return 0;
            tag->type = mpack_type_float;
            tag->v.f = mpack_load_float(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_FLOAT;

        // double
        case 0xcb:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_DOUBLE))
                return 0;
            tag->type = mpack_type_double;
            tag->v.d = mpack_load_double(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_DOUBLE;

        // uint8
        case 0xcc:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_U8))
                return 0;
            tag->type = mpack_type_uint;
            tag->v.u = mpack_load_u8(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_U8;

        // uint16
        case 0xcd:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_U16))
                return 0;
            tag->type = mpack_type_uint;
            tag->v.u = mpack_load_u16(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_U16;

        // uint32
        case 0xce:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_U32))
                return 0;
            tag->type = mpack_type_uint;
            tag->v.u = mpack_load_u32(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_U32;

        // uint64
        case 0xcf:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_U64))
                return 0;
            tag->type = mpack_type_uint;
            tag->v.u = mpack_load_u64(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_U64;

        // int8
        case 0xd0:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_I8))
                return 0;
            tag->type = mpack_type_int;
            tag->v.i = mpack_load_i8(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_I8;

        // int16
        case 0xd1:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_I16))
                return 0;
            tag->type = mpack_type_int;
            tag->v.i = mpack_load_i16(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_I16;

        // int32
        case 0xd2:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_I32))
                return 0;
            tag->type = mpack_type_int;
            tag->v.i = mpack_load_i32(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_I32;

        // int64
        case 0xd3:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_I64))
                return 0;
            tag->type = mpack_type_int;
            tag->v.i = mpack_load_i64(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_I64;

        // fixext1
        case 0xd4:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_FIXEXT1))
                return 0;
            tag->type = mpack_type_ext;
            tag->v.l = 1;
            tag->exttype = mpack_load_i8(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_FIXEXT1;

        // fixext2
        case 0xd5:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_FIXEXT2))
                return 0;
            tag->type = mpack_type_ext;
            tag->v.l = 2;
            tag->exttype = mpack_load_i8(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_FIXEXT2;

        // fixext4
        case 0xd6:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_FIXEXT4))
                return 0;
            tag->type = mpack_type_ext;
            tag->v.l = 4;
            tag->exttype = mpack_load_i8(reader->buffer + reader->pos + 1);
            return 2;

        // fixext8
        case 0xd7:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_FIXEXT8))
                return 0;
            tag->type = mpack_type_ext;
            tag->v.l = 8;
            tag->exttype = mpack_load_i8(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_FIXEXT8;

        // fixext16
        case 0xd8:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_FIXEXT16))
                return 0;
            tag->type = mpack_type_ext;
            tag->v.l = 16;
            tag->exttype = mpack_load_i8(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_FIXEXT16;

        // str8
        case 0xd9:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_STR8))
                return 0;
            tag->type = mpack_type_str;
            tag->v.l = mpack_load_u8(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_STR8;

        // str16
        case 0xda:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_STR16))
                return 0;
            tag->type = mpack_type_str;
            tag->v.l = mpack_load_u16(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_STR16;

        // str32
        case 0xdb:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_STR32))
                return 0;
            tag->type = mpack_type_str;
            tag->v.l = mpack_load_u32(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_STR32;

        // array16
        case 0xdc:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_ARRAY16))
                return 0;
            tag->type = mpack_type_array;
            tag->v.n = mpack_load_u16(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_ARRAY16;

        // array32
        case 0xdd:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_ARRAY32))
                return 0;
            tag->type = mpack_type_array;
            tag->v.n = mpack_load_u32(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_ARRAY32;

        // map16
        case 0xde:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_MAP16))
                return 0;
            tag->type = mpack_type_map;
            tag->v.n = mpack_load_u16(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_MAP16;

        // map32
        case 0xdf:
            if (!mpack_reader_ensure(reader, MPACK_TAG_SIZE_MAP32))
                return 0;
            tag->type = mpack_type_map;
            tag->v.n = mpack_load_u32(reader->buffer + reader->pos + 1);
            return MPACK_TAG_SIZE_MAP32;

        // reserved
        case 0xc1:
            break;

        #if MPACK_OPTIMIZE_FOR_SIZE
        // any other bytes should have been handled by the infix switch
        default:
            mpack_assert(0, "unreachable");
            break;
        #endif
    }

    // unrecognized type
    mpack_reader_flag_error(reader, mpack_error_invalid);
    return 0;
}

mpack_tag_t mpack_read_tag(mpack_reader_t* reader) {
    mpack_log("reading tag\n");

    // make sure we can read a tag
    if (mpack_reader_error(reader) != mpack_ok)
        return mpack_tag_nil();
    if (mpack_reader_track_element(reader) != mpack_ok)
        return mpack_tag_nil();

    mpack_tag_t tag;
    mpack_memset(&tag, 0, sizeof(tag));
    size_t count = mpack_parse_tag(reader, &tag);
    if (count == 0)
        return mpack_tag_nil();

    #if MPACK_READ_TRACKING
    mpack_error_t track_error = mpack_ok;

    switch (tag.type) {
        case mpack_type_map:
        case mpack_type_array:
            track_error = mpack_track_push(&reader->track, tag.type, tag.v.l);
            break;
        case mpack_type_str:
        case mpack_type_bin:
        case mpack_type_ext:
            track_error = mpack_track_push(&reader->track, tag.type, tag.v.n);
            break;
        default:
            break;
    }

    if (track_error != mpack_ok) {
        mpack_reader_flag_error(reader, track_error);
        return mpack_tag_nil();
    }
    #endif

    // the tag is guaranteed to have been read out of
    // the buffer, so we advance past it
    reader->pos += count;
    reader->left -= count;

    return tag;
}

mpack_tag_t mpack_peek_tag(mpack_reader_t* reader) {
    mpack_log("peeking tag\n");

    // make sure we can peek a tag
    if (mpack_reader_error(reader) != mpack_ok)
        return mpack_tag_nil();
    if (mpack_reader_track_peek_element(reader) != mpack_ok)
        return mpack_tag_nil();

    mpack_tag_t tag;
    mpack_memset(&tag, 0, sizeof(tag));
    if (mpack_parse_tag(reader, &tag) == 0)
        return mpack_tag_nil();
    return tag;
}

void mpack_discard(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (mpack_reader_error(reader))
        return;
    switch (var.type) {
        case mpack_type_str:
            mpack_skip_bytes(reader, var.v.l);
            mpack_done_str(reader);
            break;
        case mpack_type_bin:
            mpack_skip_bytes(reader, var.v.l);
            mpack_done_bin(reader);
            break;
        case mpack_type_ext:
            mpack_skip_bytes(reader, var.v.l);
            mpack_done_ext(reader);
            break;
        case mpack_type_array: {
            for (; var.v.n > 0; --var.v.n) {
                mpack_discard(reader);
                if (mpack_reader_error(reader))
                    break;
            }
            mpack_done_array(reader);
            break;
        }
        case mpack_type_map: {
            for (; var.v.n > 0; --var.v.n) {
                mpack_discard(reader);
                mpack_discard(reader);
                if (mpack_reader_error(reader))
                    break;
            }
            mpack_done_map(reader);
            break;
        }
        default:
            break;
    }
}

#if MPACK_READ_TRACKING
void mpack_done_type(mpack_reader_t* reader, mpack_type_t type) {
    if (mpack_reader_error(reader) == mpack_ok)
        mpack_reader_flag_if_error(reader, mpack_track_pop(&reader->track, type));
}
#endif

#if MPACK_STDIO
static void mpack_print_element(mpack_reader_t* reader, size_t depth, FILE* file) {
    mpack_tag_t val = mpack_read_tag(reader);
    if (mpack_reader_error(reader) != mpack_ok)
        return;
    switch (val.type) {

        case mpack_type_nil:
            fprintf(file, "null");
            break;
        case mpack_type_bool:
            fprintf(file, val.v.b ? "true" : "false");
            break;

        case mpack_type_float:
            fprintf(file, "%f", val.v.f);
            break;
        case mpack_type_double:
            fprintf(file, "%f", val.v.d);
            break;

        case mpack_type_int:
            fprintf(file, "%" PRIi64, val.v.i);
            break;
        case mpack_type_uint:
            fprintf(file, "%" PRIu64, val.v.u);
            break;

        case mpack_type_bin:
            fprintf(file, "<binary data of length %u>", val.v.l);
            mpack_skip_bytes(reader, val.v.l);
            mpack_done_bin(reader);
            break;

        case mpack_type_ext:
            fprintf(file, "<ext data of type %i and length %u>", val.exttype, val.v.l);
            mpack_skip_bytes(reader, val.v.l);
            mpack_done_ext(reader);
            break;

        case mpack_type_str:
            putc('"', file);
            for (size_t i = 0; i < val.v.l; ++i) {
                char c;
                mpack_read_bytes(reader, &c, 1);
                if (mpack_reader_error(reader) != mpack_ok)
                    return;
                switch (c) {
                    case '\n': fprintf(file, "\\n"); break;
                    case '\\': fprintf(file, "\\\\"); break;
                    case '"': fprintf(file, "\\\""); break;
                    default: putc(c, file); break;
                }
            }
            putc('"', file);
            mpack_done_str(reader);
            break;

        case mpack_type_array:
            fprintf(file, "[\n");
            for (size_t i = 0; i < val.v.n; ++i) {
                for (size_t j = 0; j < depth + 1; ++j)
                    fprintf(file, "    ");
                mpack_print_element(reader, depth + 1, file);
                if (mpack_reader_error(reader) != mpack_ok)
                    return;
                if (i != val.v.n - 1)
                    putc(',', file);
                putc('\n', file);
            }
            for (size_t i = 0; i < depth; ++i)
                fprintf(file, "    ");
            putc(']', file);
            mpack_done_array(reader);
            break;

        case mpack_type_map:
            fprintf(file, "{\n");
            for (size_t i = 0; i < val.v.n; ++i) {
                for (size_t j = 0; j < depth + 1; ++j)
                    fprintf(file, "    ");
                mpack_print_element(reader, depth + 1, file);
                if (mpack_reader_error(reader) != mpack_ok)
                    return;
                fprintf(file, ": ");
                mpack_print_element(reader, depth + 1, file);
                if (mpack_reader_error(reader) != mpack_ok)
                    return;
                if (i != val.v.n - 1)
                    putc(',', file);
                putc('\n', file);
            }
            for (size_t i = 0; i < depth; ++i)
                fprintf(file, "    ");
            putc('}', file);
            mpack_done_map(reader);
            break;
    }
}

void mpack_print_file(const char* data, size_t len, FILE* file) {
    mpack_assert(data != NULL, "data is NULL");
    mpack_assert(file != NULL, "file is NULL");

    mpack_reader_t reader;
    mpack_reader_init_data(&reader, data, len);

    int depth = 2;
    for (int i = 0; i < depth; ++i)
        fprintf(file, "    ");
    mpack_print_element(&reader, depth, file);
    putc('\n', file);

    size_t remaining = mpack_reader_remaining(&reader, NULL);

    if (mpack_reader_destroy(&reader) != mpack_ok)
        fprintf(file, "<mpack parsing error %s>\n", mpack_error_to_string(mpack_reader_error(&reader)));
    else if (remaining > 0)
        fprintf(file, "<%i extra bytes at end of mpack>\n", (int)remaining);
}
#endif

#endif


/* mpack-expect.c */

#define MPACK_INTERNAL 1

/* #include "mpack-expect.h" */

#if MPACK_EXPECT


// Helpers

MPACK_STATIC_INLINE uint8_t mpack_expect_native_u8(mpack_reader_t* reader) {
    if (mpack_reader_error(reader) != mpack_ok)
        return 0;
    uint8_t type;
    if (!mpack_reader_ensure(reader, sizeof(type)))
        return 0;
    type = mpack_load_u8(reader->buffer + reader->pos);
    reader->pos += sizeof(type);
    reader->left -= sizeof(type);
    return type;
}

#if !MPACK_OPTIMIZE_FOR_SIZE
MPACK_STATIC_INLINE uint16_t mpack_expect_native_u16(mpack_reader_t* reader) {
    if (mpack_reader_error(reader) != mpack_ok)
        return 0;
    uint16_t type;
    if (!mpack_reader_ensure(reader, sizeof(type)))
        return 0;
    type = mpack_load_u16(reader->buffer + reader->pos);
    reader->pos += sizeof(type);
    reader->left -= sizeof(type);
    return type;
}

MPACK_STATIC_INLINE uint32_t mpack_expect_native_u32(mpack_reader_t* reader) {
    if (mpack_reader_error(reader) != mpack_ok)
        return 0;
    uint32_t type;
    if (!mpack_reader_ensure(reader, sizeof(type)))
        return 0;
    type = mpack_load_u32(reader->buffer + reader->pos);
    reader->pos += sizeof(type);
    reader->left -= sizeof(type);
    return type;
}
#endif

MPACK_STATIC_INLINE uint8_t mpack_expect_type_byte(mpack_reader_t* reader) {
    mpack_reader_track_element(reader);
    return mpack_expect_native_u8(reader);
}


// Basic Number Functions

uint8_t mpack_expect_u8(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_uint) {
        if (var.v.u <= UINT8_MAX)
            return (uint8_t)var.v.u;
    } else if (var.type == mpack_type_int) {
        if (var.v.i >= 0 && var.v.i <= UINT8_MAX)
            return (uint8_t)var.v.i;
    }
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0;
}

uint16_t mpack_expect_u16(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_uint) {
        if (var.v.u <= UINT16_MAX)
            return (uint16_t)var.v.u;
    } else if (var.type == mpack_type_int) {
        if (var.v.i >= 0 && var.v.i <= UINT16_MAX)
            return (uint16_t)var.v.i;
    }
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0;
}

uint32_t mpack_expect_u32(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_uint) {
        if (var.v.u <= UINT32_MAX)
            return (uint32_t)var.v.u;
    } else if (var.type == mpack_type_int) {
        if (var.v.i >= 0 && var.v.i <= UINT32_MAX)
            return (uint32_t)var.v.i;
    }
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0;
}

uint64_t mpack_expect_u64(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_uint) {
        return var.v.u;
    } else if (var.type == mpack_type_int) {
        if (var.v.i >= 0)
            return (uint64_t)var.v.i;
    }
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0;
}

int8_t mpack_expect_i8(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_uint) {
        if (var.v.u <= INT8_MAX)
            return (int8_t)var.v.u;
    } else if (var.type == mpack_type_int) {
        if (var.v.i >= INT8_MIN && var.v.i <= INT8_MAX)
            return (int8_t)var.v.i;
    }
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0;
}

int16_t mpack_expect_i16(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_uint) {
        if (var.v.u <= INT16_MAX)
            return (int16_t)var.v.u;
    } else if (var.type == mpack_type_int) {
        if (var.v.i >= INT16_MIN && var.v.i <= INT16_MAX)
            return (int16_t)var.v.i;
    }
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0;
}

int32_t mpack_expect_i32(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_uint) {
        if (var.v.u <= INT32_MAX)
            return (int32_t)var.v.u;
    } else if (var.type == mpack_type_int) {
        if (var.v.i >= INT32_MIN && var.v.i <= INT32_MAX)
            return (int32_t)var.v.i;
    }
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0;
}

int64_t mpack_expect_i64(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_uint) {
        if (var.v.u <= INT64_MAX)
            return (int64_t)var.v.u;
    } else if (var.type == mpack_type_int) {
        return var.v.i;
    }
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0;
}

float mpack_expect_float(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_uint)
        return (float)var.v.u;
    else if (var.type == mpack_type_int)
        return (float)var.v.i;
    else if (var.type == mpack_type_float)
        return var.v.f;
    else if (var.type == mpack_type_double)
        return (float)var.v.d;
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0.0f;
}

double mpack_expect_double(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_uint)
        return (double)var.v.u;
    else if (var.type == mpack_type_int)
        return (double)var.v.i;
    else if (var.type == mpack_type_float)
        return (double)var.v.f;
    else if (var.type == mpack_type_double)
        return var.v.d;
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0.0;
}

float mpack_expect_float_strict(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_float)
        return var.v.f;
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0.0f;
}

double mpack_expect_double_strict(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_float)
        return (double)var.v.f;
    else if (var.type == mpack_type_double)
        return var.v.d;
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0.0;
}


// Ranged Number Functions
//
// All ranged functions are identical other than the type, so we
// define their content with a macro. The prototypes are still written
// out in full to support ctags/IDE tools.

#define MPACK_EXPECT_RANGE_IMPL(name, type_t)                           \
                                                                        \
    /* make sure the range is sensible */                               \
    mpack_assert(min_value <= max_value,                                \
            "min_value %i must be less than or equal to max_value %i",  \
            min_value, max_value);                                      \
                                                                        \
    /* read the value */                                                \
    type_t val = mpack_expect_##name(reader);                           \
    if (mpack_reader_error(reader) != mpack_ok)                         \
        return min_value;                                               \
                                                                        \
    /* make sure it fits */                                             \
    if (val < min_value || val > max_value) {                           \
        mpack_reader_flag_error(reader, mpack_error_type);              \
        return min_value;                                               \
    }                                                                   \
                                                                        \
    return val;

uint8_t mpack_expect_u8_range(mpack_reader_t* reader, uint8_t min_value, uint8_t max_value) {MPACK_EXPECT_RANGE_IMPL(u8, uint8_t)}
uint16_t mpack_expect_u16_range(mpack_reader_t* reader, uint16_t min_value, uint16_t max_value) {MPACK_EXPECT_RANGE_IMPL(u16, uint16_t)}
uint32_t mpack_expect_u32_range(mpack_reader_t* reader, uint32_t min_value, uint32_t max_value) {MPACK_EXPECT_RANGE_IMPL(u32, uint32_t)}
uint64_t mpack_expect_u64_range(mpack_reader_t* reader, uint64_t min_value, uint64_t max_value) {MPACK_EXPECT_RANGE_IMPL(u64, uint64_t)}

int8_t mpack_expect_i8_range(mpack_reader_t* reader, int8_t min_value, int8_t max_value) {MPACK_EXPECT_RANGE_IMPL(i8, int8_t)}
int16_t mpack_expect_i16_range(mpack_reader_t* reader, int16_t min_value, int16_t max_value) {MPACK_EXPECT_RANGE_IMPL(i16, int16_t)}
int32_t mpack_expect_i32_range(mpack_reader_t* reader, int32_t min_value, int32_t max_value) {MPACK_EXPECT_RANGE_IMPL(i32, int32_t)}
int64_t mpack_expect_i64_range(mpack_reader_t* reader, int64_t min_value, int64_t max_value) {MPACK_EXPECT_RANGE_IMPL(i64, int64_t)}

float mpack_expect_float_range(mpack_reader_t* reader, float min_value, float max_value) {MPACK_EXPECT_RANGE_IMPL(float, float)}
double mpack_expect_double_range(mpack_reader_t* reader, double min_value, double max_value) {MPACK_EXPECT_RANGE_IMPL(double, double)}

uint32_t mpack_expect_map_range(mpack_reader_t* reader, uint32_t min_value, uint32_t max_value) {MPACK_EXPECT_RANGE_IMPL(map, uint32_t)}
uint32_t mpack_expect_array_range(mpack_reader_t* reader, uint32_t min_value, uint32_t max_value) {MPACK_EXPECT_RANGE_IMPL(array, uint32_t)}


// Matching Number Functions

void mpack_expect_uint_match(mpack_reader_t* reader, uint64_t value) {
    if (mpack_expect_u64(reader) != value)
        mpack_reader_flag_error(reader, mpack_error_type);
}

void mpack_expect_int_match(mpack_reader_t* reader, int64_t value) {
    if (mpack_expect_i64(reader) != value)
        mpack_reader_flag_error(reader, mpack_error_type);
}


// Other Basic Types

void mpack_expect_nil(mpack_reader_t* reader) {
    if (mpack_expect_type_byte(reader) != 0xc0)
        mpack_reader_flag_error(reader, mpack_error_type);
}

bool mpack_expect_bool(mpack_reader_t* reader) {
    uint8_t type = mpack_expect_type_byte(reader);
    if ((type & ~1) != 0xc2)
        mpack_reader_flag_error(reader, mpack_error_type);
    return (bool)(type & 1);
}

void mpack_expect_true(mpack_reader_t* reader) {
    if (mpack_expect_bool(reader) != true)
        mpack_reader_flag_error(reader, mpack_error_type);
}

void mpack_expect_false(mpack_reader_t* reader) {
    if (mpack_expect_bool(reader) != false)
        mpack_reader_flag_error(reader, mpack_error_type);
}


// Compound Types

uint32_t mpack_expect_map(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_map)
        return var.v.n;
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0;
}

void mpack_expect_map_match(mpack_reader_t* reader, uint32_t count) {
    if (mpack_expect_map(reader) != count)
        mpack_reader_flag_error(reader, mpack_error_type);
}

bool mpack_expect_map_or_nil(mpack_reader_t* reader, uint32_t* count) {
    mpack_assert(count != NULL, "count cannot be NULL");

    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_nil) {
        *count = 0;
        return false;
    }
    if (var.type == mpack_type_map) {
        *count = var.v.n;
        return true;
    }
    mpack_reader_flag_error(reader, mpack_error_type);
    *count = 0;
    return false;
}

bool mpack_expect_map_max_or_nil(mpack_reader_t* reader, uint32_t max_count, uint32_t* count) {
    mpack_assert(count != NULL, "count cannot be NULL");

    bool has_map = mpack_expect_map_or_nil(reader, count);
    if (has_map && *count > max_count) {
        *count = 0;
        mpack_reader_flag_error(reader, mpack_error_type);
        return false;
    }
    return has_map;
}

uint32_t mpack_expect_array(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_array)
        return var.v.n;
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0;
}

void mpack_expect_array_match(mpack_reader_t* reader, uint32_t count) {
    if (mpack_expect_array(reader) != count)
        mpack_reader_flag_error(reader, mpack_error_type);
}

bool mpack_expect_array_or_nil(mpack_reader_t* reader, uint32_t* count) {
    mpack_assert(count != NULL, "count cannot be NULL");

    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_nil) {
        *count = 0;
        return false;
    }
    if (var.type == mpack_type_array) {
        *count = var.v.n;
        return true;
    }
    mpack_reader_flag_error(reader, mpack_error_type);
    *count = 0;
    return false;
}

bool mpack_expect_array_max_or_nil(mpack_reader_t* reader, uint32_t max_count, uint32_t* count) {
    mpack_assert(count != NULL, "count cannot be NULL");

    bool has_array = mpack_expect_array_or_nil(reader, count);
    if (has_array && *count > max_count) {
        *count = 0;
        mpack_reader_flag_error(reader, mpack_error_type);
        return false;
    }
    return has_array;
}

#ifdef MPACK_MALLOC
void* mpack_expect_array_alloc_impl(mpack_reader_t* reader, size_t element_size, uint32_t max_count, uint32_t* out_count, bool allow_nil) {
    mpack_assert(out_count != NULL, "out_count cannot be NULL");
    *out_count = 0;

    uint32_t count;
    bool has_array = true;
    if (allow_nil)
        has_array = mpack_expect_array_max_or_nil(reader, max_count, &count);
    else
        count = mpack_expect_array_max(reader, max_count);
    if (mpack_reader_error(reader))
        return NULL;

    // size 0 is not an error; we return NULL for no elements.
    if (count == 0) {
        // we call mpack_done_array() automatically ONLY if we are using
        // the _or_nil variant. this is the only way to allow nil and empty
        // to work the same way.
        if (allow_nil && has_array)
            mpack_done_array(reader);
        return NULL;
    }

    void* p = MPACK_MALLOC(element_size * count);
    if (p == NULL) {
        mpack_reader_flag_error(reader, mpack_error_memory);
        return NULL;
    }

    *out_count = count;
    return p;
}
#endif


// Str, Bin and Ext Functions

uint32_t mpack_expect_str(mpack_reader_t* reader) {
    #if MPACK_OPTIMIZE_FOR_SIZE
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_str)
        return var.v.l;
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0;
    #else
    uint8_t type = mpack_expect_type_byte(reader);
    uint32_t count;

    if ((type >> 5) == 5) {
        count = type & (uint8_t)~0xe0;
    } else if (type == 0xd9) {
        count = mpack_expect_native_u8(reader);
    } else if (type == 0xda) {
        count = mpack_expect_native_u16(reader);
    } else if (type == 0xdb) {
        count = mpack_expect_native_u32(reader);
    } else {
        mpack_reader_flag_error(reader, mpack_error_type);
        return 0;
    }

    #if MPACK_READ_TRACKING
    mpack_reader_flag_if_error(reader, mpack_track_push(&reader->track, mpack_type_str, count));
    #endif
    return count;
    #endif
}

size_t mpack_expect_str_buf(mpack_reader_t* reader, char* buf, size_t bufsize) {
    mpack_assert(buf != NULL, "buf cannot be NULL");

    size_t length = mpack_expect_str(reader);
    if (mpack_reader_error(reader))
        return 0;

    if (length > bufsize) {
        mpack_reader_flag_error(reader, mpack_error_too_big);
        return 0;
    }

    mpack_read_bytes(reader, buf, length);
    if (mpack_reader_error(reader))
        return 0;

    mpack_done_str(reader);
    return length;
}

size_t mpack_expect_utf8(mpack_reader_t* reader, char* buf, size_t size) {
    mpack_assert(buf != NULL, "buf cannot be NULL");

    size_t length = mpack_expect_str_buf(reader, buf, size);

    if (!mpack_utf8_check(buf, length)) {
        mpack_reader_flag_error(reader, mpack_error_type);
        return 0;
    }

    return length;
}

uint32_t mpack_expect_bin(mpack_reader_t* reader) {
    mpack_tag_t var = mpack_read_tag(reader);
    if (var.type == mpack_type_bin)
        return var.v.l;
    mpack_reader_flag_error(reader, mpack_error_type);
    return 0;
}

size_t mpack_expect_bin_buf(mpack_reader_t* reader, char* buf, size_t bufsize) {
    mpack_assert(buf != NULL, "buf cannot be NULL");

    size_t binsize = mpack_expect_bin(reader);
    if (mpack_reader_error(reader))
        return 0;
    if (binsize > bufsize) {
        mpack_reader_flag_error(reader, mpack_error_too_big);
        return 0;
    }
    mpack_read_bytes(reader, buf, binsize);
    if (mpack_reader_error(reader))
        return 0;
    mpack_done_bin(reader);
    return binsize;
}

void mpack_expect_cstr(mpack_reader_t* reader, char* buf, size_t bufsize) {
    uint32_t length = mpack_expect_str(reader);
    mpack_read_cstr(reader, buf, bufsize, length);
    mpack_done_str(reader);
}

void mpack_expect_utf8_cstr(mpack_reader_t* reader, char* buf, size_t bufsize) {
    uint32_t length = mpack_expect_str(reader);
    mpack_read_utf8_cstr(reader, buf, bufsize, length);
    mpack_done_str(reader);
}

#ifdef MPACK_MALLOC
static char* mpack_expect_cstr_alloc_unchecked(mpack_reader_t* reader, size_t maxsize, size_t* out_length) {
    mpack_assert(out_length != NULL, "out_length cannot be NULL");
    *out_length = 0;

    // make sure argument makes sense
    if (maxsize < 1) {
        mpack_break("maxsize is zero; you must have room for at least a null-terminator");
        mpack_reader_flag_error(reader, mpack_error_bug);
        return NULL;
    }

    if (maxsize > UINT32_MAX)
        maxsize = UINT32_MAX;

    size_t length = mpack_expect_str_max(reader, (uint32_t)maxsize - 1);
    char* str = mpack_read_bytes_alloc_impl(reader, length, true);
    mpack_done_str(reader);

    if (str)
        *out_length = length;
    return str;
}

char* mpack_expect_cstr_alloc(mpack_reader_t* reader, size_t maxsize) {
    size_t length;
    char* str = mpack_expect_cstr_alloc_unchecked(reader, maxsize, &length);

    if (str && !mpack_str_check_no_null(str, length)) {
        MPACK_FREE(str);
        mpack_reader_flag_error(reader, mpack_error_type);
        return NULL;
    }

    return str;
}

char* mpack_expect_utf8_cstr_alloc(mpack_reader_t* reader, size_t maxsize) {
    size_t length;
    char* str = mpack_expect_cstr_alloc_unchecked(reader, maxsize, &length);

    if (str && !mpack_utf8_check_no_null(str, length)) {
        MPACK_FREE(str);
        mpack_reader_flag_error(reader, mpack_error_type);
        return NULL;
    }

    return str;
}
#endif

void mpack_expect_str_match(mpack_reader_t* reader, const char* str, size_t len) {
    mpack_assert(str != NULL, "str cannot be NULL");

    // expect a str the correct length
    if (len > UINT32_MAX)
        mpack_reader_flag_error(reader, mpack_error_type);
    mpack_expect_str_length(reader, (uint32_t)len);
    if (mpack_reader_error(reader))
        return;
    mpack_reader_track_bytes(reader, len);

    // check each byte one by one (matched strings are likely to be very small)
    for (; len > 0; --len) {
        if (mpack_expect_native_u8(reader) != *str++) {
            mpack_reader_flag_error(reader, mpack_error_type);
            return;
        }
    }

    mpack_done_str(reader);
}

void mpack_expect_tag(mpack_reader_t* reader, mpack_tag_t expected) {
    mpack_tag_t actual = mpack_read_tag(reader);
    if (!mpack_tag_equal(actual, expected))
        mpack_reader_flag_error(reader, mpack_error_type);
}

#ifdef MPACK_MALLOC
char* mpack_expect_bin_alloc(mpack_reader_t* reader, size_t maxsize, size_t* size) {
    mpack_assert(size != NULL, "size cannot be NULL");
    *size = 0;

    if (maxsize > UINT32_MAX)
        maxsize = UINT32_MAX;

    size_t length = mpack_expect_bin_max(reader, (uint32_t)maxsize);
    char* data = mpack_read_bytes_alloc(reader, length);
    mpack_done_bin(reader);

    if (data)
        *size = length;
    return data;
}
#endif

size_t mpack_expect_enum(mpack_reader_t* reader, const char* strings[], size_t count) {

    // read the string in-place
    size_t keylen = mpack_expect_str(reader);
    const char* key = mpack_read_bytes_inplace(reader, keylen);
    mpack_done_str(reader);
    if (mpack_reader_error(reader) != mpack_ok)
        return count;

    // find what key it matches
    for (size_t i = 0; i < count; ++i) {
        const char* other = strings[i];
        size_t otherlen = mpack_strlen(other);
        if (keylen == otherlen && mpack_memcmp(key, other, keylen) == 0)
            return i;
    }

    // no matches
    mpack_reader_flag_error(reader, mpack_error_type);
    return count;
}

size_t mpack_expect_enum_optional(mpack_reader_t* reader, const char* strings[], size_t count) {
    if (mpack_reader_error(reader) != mpack_ok)
        return count;

    mpack_assert(count != 0, "count cannot be zero; no strings are valid!");
    mpack_assert(strings != NULL, "strings cannot be NULL");

    // the key is only recognized if it is a string
    if (mpack_peek_tag(reader).type != mpack_type_str) {
        mpack_discard(reader);
        return count;
    }

    // read the string in-place
    size_t keylen = mpack_expect_str(reader);
    const char* key = mpack_read_bytes_inplace(reader, keylen);
    mpack_done_str(reader);
    if (mpack_reader_error(reader) != mpack_ok)
        return count;

    // find what key it matches
    for (size_t i = 0; i < count; ++i) {
        const char* other = strings[i];
        size_t otherlen = mpack_strlen(other);
        if (keylen == otherlen && mpack_memcmp(key, other, keylen) == 0)
            return i;
    }

    // no matches
    return count;
}

size_t mpack_expect_key_uint(mpack_reader_t* reader, bool found[], size_t count) {
    if (mpack_reader_error(reader) != mpack_ok)
        return count;

    if (count == 0) {
        mpack_break("count cannot be zero; no keys are valid!");
        mpack_reader_flag_error(reader, mpack_error_bug);
        return count;
    }
    mpack_assert(found != NULL, "found cannot be NULL");

    // the key is only recognized if it is an unsigned int
    if (mpack_peek_tag(reader).type != mpack_type_uint) {
        mpack_discard(reader);
        return count;
    }

    // read the key
    uint64_t value = mpack_expect_u64(reader);
    if (mpack_reader_error(reader) != mpack_ok)
        return count;

    // unrecognized keys are fine, we just return count
    if (value >= count)
        return count;

    // check if this key is a duplicate
    if (found[value]) {
        mpack_reader_flag_error(reader, mpack_error_invalid);
        return count;
    }

    found[value] = true;
    return (size_t)value;
}

size_t mpack_expect_key_cstr(mpack_reader_t* reader, const char* keys[], bool found[], size_t count) {
    size_t i = mpack_expect_enum_optional(reader, keys, count);

    // unrecognized keys are fine, we just return count
    if (i == count)
        return count;

    // check if this key is a duplicate
    mpack_assert(found != NULL, "found cannot be NULL");
    if (found[i]) {
        mpack_reader_flag_error(reader, mpack_error_invalid);
        return count;
    }

    found[i] = true;
    return i;
}

#endif


/* mpack-node.c */

#define MPACK_INTERNAL 1

/* #include "mpack-node.h" */

#if MPACK_NODE



/*
 * Tree Parsing
 */

// fix up the alloc size to make sure it exactly fits the
// maximum number of nodes it can contain (the allocator will
// waste it back anyway, but we round it down just in case)

#ifdef MPACK_MALLOC

#define MPACK_NODES_PER_PAGE \
    ((MPACK_NODE_PAGE_SIZE - sizeof(mpack_tree_page_t)) / sizeof(mpack_node_data_t) + 1)

#define MPACK_PAGE_ALLOC_SIZE \
    (sizeof(mpack_tree_page_t) + sizeof(mpack_node_data_t) * (MPACK_NODES_PER_PAGE - 1))

#endif

typedef struct mpack_level_t {
    mpack_node_data_t* child;
    size_t left; // children left in level
} mpack_level_t;

typedef struct mpack_tree_parser_t {
    mpack_tree_t* tree;
    const char* data;

    // We keep track of the number of "possible nodes" left in the data rather
    // than the number of bytes.
    //
    // When a map or array is parsed, we ensure at least one byte for each child
    // exists and subtract them right away. This ensures that if ever a map or
    // array declares more elements than could possibly be contained in the data,
    // we will error out immediately rather than allocating storage for them.
    //
    // For example malicious data that repeats 0xDE 0xFF 0xFF would otherwise
    // cause us to run out of memory. With this, the parser can only allocate
    // as many nodes as there are bytes in the data (plus the paging overhead,
    // 12%.) An error will be flagged immediately if and when there isn't enough
    // data left to fully read all children of all open compound types on the
    // parsing stack.
    //
    // Once an entire message has been parsed (and there are no nodes left to
    // parse whose bytes have been subtracted), this matches the number of left
    // over bytes in the data.
    size_t possible_nodes_left;

    mpack_node_data_t* nodes;
    size_t nodes_left; // nodes left in current page/pool

    size_t level;
    size_t depth;
    mpack_level_t* stack;
    bool stack_owned;
} mpack_tree_parser_t;

MPACK_STATIC_INLINE uint8_t mpack_tree_u8(mpack_tree_parser_t* parser) {
    if (parser->possible_nodes_left < sizeof(uint8_t)) {
        mpack_tree_flag_error(parser->tree, mpack_error_invalid);
        return 0;
    }
    uint8_t val = mpack_load_u8(parser->data);
    parser->data += sizeof(uint8_t);
    parser->possible_nodes_left -= sizeof(uint8_t);
    return val;
}

MPACK_STATIC_INLINE uint16_t mpack_tree_u16(mpack_tree_parser_t* parser) {
    if (parser->possible_nodes_left < sizeof(uint16_t)) {
        mpack_tree_flag_error(parser->tree, mpack_error_invalid);
        return 0;
    }
    uint16_t val = mpack_load_u16(parser->data);
    parser->data += sizeof(uint16_t);
    parser->possible_nodes_left -= sizeof(uint16_t);
    return val;
}

MPACK_STATIC_INLINE uint32_t mpack_tree_u32(mpack_tree_parser_t* parser) {
    if (parser->possible_nodes_left < sizeof(uint32_t)) {
        mpack_tree_flag_error(parser->tree, mpack_error_invalid);
        return 0;
    }
    uint32_t val = mpack_load_u32(parser->data);
    parser->data += sizeof(uint32_t);
    parser->possible_nodes_left -= sizeof(uint32_t);
    return val;
}

MPACK_STATIC_INLINE uint64_t mpack_tree_u64(mpack_tree_parser_t* parser) {
    if (parser->possible_nodes_left < sizeof(uint64_t)) {
        mpack_tree_flag_error(parser->tree, mpack_error_invalid);
        return 0;
    }
    uint64_t val = mpack_load_u64(parser->data);
    parser->data += sizeof(uint64_t);
    parser->possible_nodes_left -= sizeof(uint64_t);
    return val;
}

MPACK_STATIC_INLINE int8_t  mpack_tree_i8 (mpack_tree_parser_t* parser) {return (int8_t) mpack_tree_u8(parser); }
MPACK_STATIC_INLINE int16_t mpack_tree_i16(mpack_tree_parser_t* parser) {return (int16_t)mpack_tree_u16(parser);}
MPACK_STATIC_INLINE int32_t mpack_tree_i32(mpack_tree_parser_t* parser) {return (int32_t)mpack_tree_u32(parser);}
MPACK_STATIC_INLINE int64_t mpack_tree_i64(mpack_tree_parser_t* parser) {return (int64_t)mpack_tree_u64(parser);}

MPACK_STATIC_INLINE void mpack_skip_exttype(mpack_tree_parser_t* parser) {
    // the exttype is stored right before the data. we
    // skip it and get it out of the data when needed.
    mpack_tree_i8(parser);
}

MPACK_STATIC_INLINE float mpack_tree_float(mpack_tree_parser_t* parser) {
    union {
        float f;
        uint32_t i;
    } u;
    u.i = mpack_tree_u32(parser);
    return u.f;
}

MPACK_STATIC_INLINE double mpack_tree_double(mpack_tree_parser_t* parser) {
    union {
        double d;
        uint64_t i;
    } u;
    u.i = mpack_tree_u64(parser);
    return u.d;
}

static void mpack_tree_push_stack(mpack_tree_parser_t* parser, mpack_node_data_t* first_child, size_t total) {

    // No need to push empty containers
    if (total == 0)
        return;

    // Make sure we have enough room in the stack
    if (parser->level + 1 == parser->depth) {
        #ifdef MPACK_MALLOC
        size_t new_depth = parser->depth * 2;
        mpack_log("growing stack to depth %i\n", (int)new_depth);

        // Replace the stack-allocated parsing stack
        if (!parser->stack_owned) {
            mpack_level_t* new_stack = (mpack_level_t*)MPACK_MALLOC(sizeof(mpack_level_t) * new_depth);
            if (!new_stack) {
                mpack_tree_flag_error(parser->tree, mpack_error_memory);
                return;
            }
            mpack_memcpy(new_stack, parser->stack, sizeof(mpack_level_t) * parser->depth);
            parser->stack = new_stack;
            parser->stack_owned = true;

        // Realloc the allocated parsing stack
        } else {
            mpack_level_t* new_stack = (mpack_level_t*)mpack_realloc(parser->stack,
                    sizeof(mpack_level_t) * parser->depth, sizeof(mpack_level_t) * new_depth);
            if (!new_stack) {
                mpack_tree_flag_error(parser->tree, mpack_error_memory);
                return;
            }
            parser->stack = new_stack;
        }
        parser->depth = new_depth;
        #else
        mpack_tree_flag_error(parser->tree, mpack_error_too_big);
        return;
        #endif
    }

    // Push the contents of this node onto the parsing stack
    ++parser->level;
    parser->stack[parser->level].child = first_child;
    parser->stack[parser->level].left = total;
}

static void mpack_tree_parse_children(mpack_tree_parser_t* parser, mpack_node_data_t* node) {
    mpack_type_t type = node->type;
    size_t total = node->len;

    // Calculate total elements to read
    if (type == mpack_type_map) {
        if ((uint64_t)total * 2 > (uint64_t)SIZE_MAX) {
            mpack_tree_flag_error(parser->tree, mpack_error_too_big);
            return;
        }
        total *= 2;
    }

    // Each node is at least one byte. Count these bytes now to make
    // sure there is enough data left.
    if (total > parser->possible_nodes_left) {
        mpack_tree_flag_error(parser->tree, mpack_error_invalid);
        return;
    }
    parser->possible_nodes_left -= total;

    // If there are enough nodes left in the current page, no need to grow
    if (total <= parser->nodes_left) {
        node->value.children = parser->nodes;
        parser->nodes += total;
        parser->nodes_left -= total;

    } else {

        #ifdef MPACK_MALLOC

        // We can't grow if we're using a fixed pool (i.e. we didn't start with a page)
        if (!parser->tree->next) {
            mpack_tree_flag_error(parser->tree, mpack_error_too_big);
            return;
        }

        // Otherwise we need to grow, and the node's children need to be contiguous.
        // This is a heuristic to decide whether we should waste the remaining space
        // in the current page and start a new one, or give the children their
        // own page. With a fraction of 1/8, this causes at most 12% additional
        // waste. Note that reducing this too much causes less cache coherence and
        // more malloc() overhead due to smaller allocations, so there's a tradeoff
        // here. This heuristic could use some improvement, especially with custom
        // page sizes.

        mpack_tree_page_t* page;

        if (total > MPACK_NODES_PER_PAGE || parser->nodes_left > MPACK_NODES_PER_PAGE / 8) {
            page = (mpack_tree_page_t*)MPACK_MALLOC(
                    sizeof(mpack_tree_page_t) + sizeof(mpack_node_data_t) * (total - 1));
            if (page == NULL) {
                mpack_tree_flag_error(parser->tree, mpack_error_memory);
                return;
            }
            mpack_log("allocated seperate page %p for %i children, %i left in page of %i total\n",
                    page, (int)total, (int)parser->nodes_left, (int)MPACK_NODES_PER_PAGE);

            node->value.children = page->nodes;

        } else {
            page = (mpack_tree_page_t*)MPACK_MALLOC(MPACK_PAGE_ALLOC_SIZE);
            if (page == NULL) {
                mpack_tree_flag_error(parser->tree, mpack_error_memory);
                return;
            }
            mpack_log("allocated new page %p for %i children, wasting %i in page of %i total\n",
                    page, (int)total, (int)parser->nodes_left, (int)MPACK_NODES_PER_PAGE);

            node->value.children = page->nodes;
            parser->nodes = page->nodes + total;
            parser->nodes_left = MPACK_NODES_PER_PAGE - total;
        }

        page->next = parser->tree->next;
        parser->tree->next = page;

        #else
        // We can't grow if we don't have an allocator
        mpack_tree_flag_error(parser->tree, mpack_error_too_big);
        return;
        #endif
    }

    mpack_tree_push_stack(parser, node->value.children, total);
}

static void mpack_tree_parse_bytes(mpack_tree_parser_t* parser, mpack_node_data_t* node) {
    size_t length = node->len;
    if (length > parser->possible_nodes_left) {
        mpack_tree_flag_error(parser->tree, mpack_error_invalid);
        return;
    }
    node->value.bytes = parser->data;
    parser->data += length;
    parser->possible_nodes_left -= length;
}

static void mpack_tree_parse_node(mpack_tree_parser_t* parser, mpack_node_data_t* node) {

    // read the type. we've already accounted for this byte in
    // possible_nodes_left, so we know it is in bounds and don't
    // need to subtract it.
    uint8_t type = mpack_load_u8(parser->data);
    parser->data += sizeof(uint8_t);

    // as with mpack_read_tag(), the fastest way to parse a node is to switch
    // on the first byte, and to explicitly list every possible byte. we switch
    // on the first four bits in size-optimized builds.

    #if MPACK_OPTIMIZE_FOR_SIZE
    switch (type >> 4) {

        // positive fixnum
        case 0x0: case 0x1: case 0x2: case 0x3:
        case 0x4: case 0x5: case 0x6: case 0x7:
            node->type = mpack_type_uint;
            node->value.u = type;
            return;

        // negative fixnum
        case 0xe: case 0xf:
            node->type = mpack_type_int;
            node->value.i = (int8_t)type;
            return;

        // fixmap
        case 0x8:
            node->type = mpack_type_map;
            node->len = type & ~0xf0;
            mpack_tree_parse_children(parser, node);
            return;

        // fixarray
        case 0x9:
            node->type = mpack_type_array;
            node->len = type & ~0xf0;
            mpack_tree_parse_children(parser, node);
            return;

        // fixstr
        case 0xa: case 0xb:
            node->type = mpack_type_str;
            node->len = type & ~0xe0;
            mpack_tree_parse_bytes(parser, node);
            return;

        // not one of the common infix types
        default:
            break;
    }
    #endif

    switch (type) {

        #if !MPACK_OPTIMIZE_FOR_SIZE
        // positive fixnum
        case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
        case 0x08: case 0x09: case 0x0a: case 0x0b: case 0x0c: case 0x0d: case 0x0e: case 0x0f:
        case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
        case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x1e: case 0x1f:
        case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x26: case 0x27:
        case 0x28: case 0x29: case 0x2a: case 0x2b: case 0x2c: case 0x2d: case 0x2e: case 0x2f:
        case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
        case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
        case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
        case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5e: case 0x5f:
        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67:
        case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6e: case 0x6f:
        case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x76: case 0x77:
        case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7e: case 0x7f:
            node->type = mpack_type_uint;
            node->value.u = type;
            return;

        // negative fixnum
        case 0xe0: case 0xe1: case 0xe2: case 0xe3: case 0xe4: case 0xe5: case 0xe6: case 0xe7:
        case 0xe8: case 0xe9: case 0xea: case 0xeb: case 0xec: case 0xed: case 0xee: case 0xef:
        case 0xf0: case 0xf1: case 0xf2: case 0xf3: case 0xf4: case 0xf5: case 0xf6: case 0xf7:
        case 0xf8: case 0xf9: case 0xfa: case 0xfb: case 0xfc: case 0xfd: case 0xfe: case 0xff:
            node->type = mpack_type_int;
            node->value.i = (int8_t)type;
            return;

        // fixmap
        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
        case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8e: case 0x8f:
            node->type = mpack_type_map;
            node->len = type & ~0xf0;
            mpack_tree_parse_children(parser, node);
            return;

        // fixarray
        case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
        case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9e: case 0x9f:
            node->type = mpack_type_array;
            node->len = type & ~0xf0;
            mpack_tree_parse_children(parser, node);
            return;

        // fixstr
        case 0xa0: case 0xa1: case 0xa2: case 0xa3: case 0xa4: case 0xa5: case 0xa6: case 0xa7:
        case 0xa8: case 0xa9: case 0xaa: case 0xab: case 0xac: case 0xad: case 0xae: case 0xaf:
        case 0xb0: case 0xb1: case 0xb2: case 0xb3: case 0xb4: case 0xb5: case 0xb6: case 0xb7:
        case 0xb8: case 0xb9: case 0xba: case 0xbb: case 0xbc: case 0xbd: case 0xbe: case 0xbf:
            node->type = mpack_type_str;
            node->len = type & ~0xe0;
            mpack_tree_parse_bytes(parser, node);
            return;
        #endif

        // nil
        case 0xc0:
            node->type = mpack_type_nil;
            return;

        // bool
        case 0xc2: case 0xc3:
            node->type = mpack_type_bool;
            node->value.b = type & 1;
            return;

        // bin8
        case 0xc4:
            node->type = mpack_type_bin;
            node->len = mpack_tree_u8(parser);
            mpack_tree_parse_bytes(parser, node);
            return;

        // bin16
        case 0xc5:
            node->type = mpack_type_bin;
            node->len = mpack_tree_u16(parser);
            mpack_tree_parse_bytes(parser, node);
            return;

        // bin32
        case 0xc6:
            node->type = mpack_type_bin;
            node->len = mpack_tree_u32(parser);
            mpack_tree_parse_bytes(parser, node);
            return;

        // ext8
        case 0xc7:
            node->type = mpack_type_ext;
            node->len = mpack_tree_u8(parser);
            mpack_skip_exttype(parser);
            mpack_tree_parse_bytes(parser, node);
            return;

        // ext16
        case 0xc8:
            node->type = mpack_type_ext;
            node->len = mpack_tree_u16(parser);
            mpack_skip_exttype(parser);
            mpack_tree_parse_bytes(parser, node);
            return;

        // ext32
        case 0xc9:
            node->type = mpack_type_ext;
            node->len = mpack_tree_u32(parser);
            mpack_skip_exttype(parser);
            mpack_tree_parse_bytes(parser, node);
            return;

        // float
        case 0xca:
            node->type = mpack_type_float;
            node->value.f = mpack_tree_float(parser);
            return;

        // double
        case 0xcb:
            node->type = mpack_type_double;
            node->value.d = mpack_tree_double(parser);
            return;

        // uint8
        case 0xcc:
            node->type = mpack_type_uint;
            node->value.u = mpack_tree_u8(parser);
            return;

        // uint16
        case 0xcd:
            node->type = mpack_type_uint;
            node->value.u = mpack_tree_u16(parser);
            return;

        // uint32
        case 0xce:
            node->type = mpack_type_uint;
            node->value.u = mpack_tree_u32(parser);
            return;

        // uint64
        case 0xcf:
            node->type = mpack_type_uint;
            node->value.u = mpack_tree_u64(parser);
            return;

        // int8
        case 0xd0:
            node->type = mpack_type_int;
            node->value.i = mpack_tree_i8(parser);
            return;

        // int16
        case 0xd1:
            node->type = mpack_type_int;
            node->value.i = mpack_tree_i16(parser);
            return;

        // int32
        case 0xd2:
            node->type = mpack_type_int;
            node->value.i = mpack_tree_i32(parser);
            return;

        // int64
        case 0xd3:
            node->type = mpack_type_int;
            node->value.i = mpack_tree_i64(parser);
            return;

        // fixext1
        case 0xd4:
            node->type = mpack_type_ext;
            node->len = 1;
            mpack_skip_exttype(parser);
            mpack_tree_parse_bytes(parser, node);
            return;

        // fixext2
        case 0xd5:
            node->type = mpack_type_ext;
            node->len = 2;
            mpack_skip_exttype(parser);
            mpack_tree_parse_bytes(parser, node);
            return;

        // fixext4
        case 0xd6:
            node->type = mpack_type_ext;
            node->len = 4;
            mpack_skip_exttype(parser);
            mpack_tree_parse_bytes(parser, node);
            return;

        // fixext8
        case 0xd7:
            node->type = mpack_type_ext;
            node->len = 8;
            mpack_skip_exttype(parser);
            mpack_tree_parse_bytes(parser, node);
            return;

        // fixext16
        case 0xd8:
            node->type = mpack_type_ext;
            node->len = 16;
            mpack_skip_exttype(parser);
            mpack_tree_parse_bytes(parser, node);
            return;

        // str8
        case 0xd9:
            node->type = mpack_type_str;
            node->len = mpack_tree_u8(parser);
            mpack_tree_parse_bytes(parser, node);
            return;

        // str16
        case 0xda:
            node->type = mpack_type_str;
            node->len = mpack_tree_u16(parser);
            mpack_tree_parse_bytes(parser, node);
            return;

        // str32
        case 0xdb:
            node->type = mpack_type_str;
            node->len = mpack_tree_u32(parser);
            mpack_tree_parse_bytes(parser, node);
            return;

        // array16
        case 0xdc:
            node->type = mpack_type_array;
            node->len = mpack_tree_u16(parser);
            mpack_tree_parse_children(parser, node);
            return;

        // array32
        case 0xdd:
            node->type = mpack_type_array;
            node->len = mpack_tree_u32(parser);
            mpack_tree_parse_children(parser, node);
            return;

        // map16
        case 0xde:
            node->type = mpack_type_map;
            node->len = mpack_tree_u16(parser);
            mpack_tree_parse_children(parser, node);
            return;

        // map32
        case 0xdf:
            node->type = mpack_type_map;
            node->len = mpack_tree_u32(parser);
            mpack_tree_parse_children(parser, node);
            return;

        // reserved
        case 0xc1:
            mpack_tree_flag_error(parser->tree, mpack_error_invalid);
            return;

        #if MPACK_OPTIMIZE_FOR_SIZE
        // any other bytes should have been handled by the infix switch
        default:
            mpack_assert(0, "unreachable");
            return;
        #endif
    }

}

static void mpack_tree_parse_elements(mpack_tree_parser_t* parser) {
    mpack_log("parsing tree elements\n");

    // we loop parsing nodes until the parse stack is empty. we break
    // by returning out of the function.
    while (true) {
        mpack_node_data_t* node = parser->stack[parser->level].child;
        --parser->stack[parser->level].left;
        ++parser->stack[parser->level].child;

        mpack_tree_parse_node(parser, node);

        if (mpack_tree_error(parser->tree) != mpack_ok)
            break;

        // pop empty stack levels, exiting the outer loop when the stack is empty.
        // (we could tail-optimize containers by pre-emptively popping empty
        // stack levels before reading the new element, this way we wouldn't
        // have to loop. but we eventually want to use the parse stack to give
        // better error messages that contain the location of the error, so
        // it needs to be complete.)
        while (parser->stack[parser->level].left == 0) {
            if (parser->level == 0)
                return;
            --parser->level;
        }
    }
}

static void mpack_tree_parse(mpack_tree_t* tree, const char* data, size_t length,
        mpack_node_data_t* initial_nodes, size_t initial_nodes_count)
{
    mpack_log("starting parse\n");

    if (length == 0) {
        mpack_tree_flag_error(tree, mpack_error_invalid);
        return;
    }
    if (initial_nodes_count == 0) {
        mpack_break("initial page has no nodes!");
        mpack_tree_flag_error(tree, mpack_error_bug);
        return;
    }
    tree->root = initial_nodes;

    // Setup parser
    mpack_tree_parser_t parser;
    mpack_memset(&parser, 0, sizeof(parser));
    parser.tree = tree;
    parser.data = data;
    parser.nodes = initial_nodes + 1;
    parser.nodes_left = initial_nodes_count - 1;

    // We read nodes in a loop instead of recursively for maximum
    // performance. The stack holds the amount of children left to
    // read in each level of the tree.

    // Even when we have a malloc() function, it's much faster to
    // allocate the initial parsing stack on the call stack. We
    // replace it with a heap allocation if we need to grow it.
    #ifdef MPACK_MALLOC
    #define MPACK_NODE_STACK_LOCAL_DEPTH MPACK_NODE_INITIAL_DEPTH
    parser.stack_owned = false;
    #else
    #define MPACK_NODE_STACK_LOCAL_DEPTH MPACK_NODE_MAX_DEPTH_WITHOUT_MALLOC
    #endif
    mpack_level_t stack_local[MPACK_NODE_STACK_LOCAL_DEPTH]; // no VLAs in VS 2013
    parser.depth = MPACK_NODE_STACK_LOCAL_DEPTH;
    parser.stack = stack_local;
    parser.possible_nodes_left = length;
    #undef MPACK_NODE_STACK_LOCAL_DEPTH

    // configure the root node
    --parser.possible_nodes_left;
    tree->node_count = 1;
    parser.level = 0;
    parser.stack[0].child = tree->root;
    parser.stack[0].left = 1;

    mpack_tree_parse_elements(&parser);

    #ifdef MPACK_MALLOC
    if (parser.stack_owned)
        MPACK_FREE(parser.stack);
    #endif

    // now that there are no longer any nodes to read, possible_nodes_left
    // is the number of bytes left in the data.
    if (mpack_tree_error(tree) == mpack_ok)
        tree->size = length - parser.possible_nodes_left;
    mpack_log("parsed tree of %i bytes, %i bytes left\n", (int)tree->size, (int)parser.possible_nodes_left);
    mpack_log("%i nodes in final page\n", (int)parser.nodes_left);
}



/*
 * Tree functions
 */

mpack_node_t mpack_tree_root(mpack_tree_t* tree) {
    return mpack_node(tree, (mpack_tree_error(tree) != mpack_ok) ? &tree->nil_node : tree->root);
}

static void mpack_tree_init_clear(mpack_tree_t* tree) {
    mpack_memset(tree, 0, sizeof(*tree));
    tree->nil_node.type = mpack_type_nil;
}

#ifdef MPACK_MALLOC
void mpack_tree_init(mpack_tree_t* tree, const char* data, size_t length) {
    mpack_tree_init_clear(tree);

    MPACK_STATIC_ASSERT(MPACK_NODE_PAGE_SIZE >= sizeof(mpack_tree_page_t),
            "MPACK_NODE_PAGE_SIZE is too small");

    MPACK_STATIC_ASSERT(MPACK_PAGE_ALLOC_SIZE <= MPACK_NODE_PAGE_SIZE,
            "incorrect page rounding?");

    // allocate first page
    mpack_tree_page_t* page = (mpack_tree_page_t*)MPACK_MALLOC(MPACK_PAGE_ALLOC_SIZE);
    mpack_log("allocated initial page %p of size %i count %i\n",
            page, (int)MPACK_PAGE_ALLOC_SIZE, (int)MPACK_NODES_PER_PAGE);
    if (page == NULL) {
        tree->error = mpack_error_memory;
        return;
    }
    page->next = NULL;
    tree->next = page;

    mpack_log("===========================\n");
    mpack_log("initializing tree with data of size %i\n", (int)length);

    mpack_tree_parse(tree, data, length, page->nodes, MPACK_NODES_PER_PAGE);
}
#endif

void mpack_tree_init_pool(mpack_tree_t* tree, const char* data, size_t length,
        mpack_node_data_t* node_pool, size_t node_pool_count)
{
    mpack_tree_init_clear(tree);
    #ifdef MPACK_MALLOC
    tree->next = NULL;
    #endif

    mpack_log("===========================\n");
    mpack_log("initializing tree with data of size %i and pool of count %i\n", (int)length, (int)node_pool_count);

    mpack_tree_parse(tree, data, length, node_pool, node_pool_count);
}

void mpack_tree_init_error(mpack_tree_t* tree, mpack_error_t error) {
    mpack_tree_init_clear(tree);
    tree->error = error;

    mpack_log("===========================\n");
    mpack_log("initializing tree error state %i\n", (int)error);
}

#if MPACK_STDIO
typedef struct mpack_file_tree_t {
    char* data;
    size_t size;
    char buffer[MPACK_BUFFER_SIZE];
} mpack_file_tree_t;

static void mpack_file_tree_teardown(mpack_tree_t* tree) {
    mpack_file_tree_t* file_tree = (mpack_file_tree_t*)tree->context;
    MPACK_FREE(file_tree->data);
    MPACK_FREE(file_tree);
}

static bool mpack_file_tree_read(mpack_tree_t* tree, mpack_file_tree_t* file_tree, const char* filename, size_t max_size) {

    // open the file
    FILE* file = fopen(filename, "rb");
    if (!file) {
        mpack_tree_init_error(tree, mpack_error_io);
        return false;
    }

    // get the file size
    errno = 0;
    int error = 0;
    fseek(file, 0, SEEK_END);
    error |= errno;
    long size = ftell(file);
    error |= errno;
    fseek(file, 0, SEEK_SET);
    error |= errno;

    // check for errors
    if (error != 0 || size < 0) {
        fclose(file);
        mpack_tree_init_error(tree, mpack_error_io);
        return false;
    }
    if (size == 0) {
        fclose(file);
        mpack_tree_init_error(tree, mpack_error_invalid);
        return false;
    }

    // make sure the size is less than max_size
    // (this mess exists to safely convert between long and size_t regardless of their widths)
    if (max_size != 0 && (((uint64_t)LONG_MAX > (uint64_t)SIZE_MAX && size > (long)SIZE_MAX) || (size_t)size > max_size)) {
        fclose(file);
        mpack_tree_init_error(tree, mpack_error_too_big);
        return false;
    }

    // allocate data
    file_tree->data = (char*)MPACK_MALLOC(size);
    if (file_tree->data == NULL) {
        fclose(file);
        mpack_tree_init_error(tree, mpack_error_memory);
        return false;
    }

    // read the file
    long total = 0;
    while (total < size) {
        size_t read = fread(file_tree->data + total, 1, (size_t)(size - total), file);
        if (read <= 0) {
            fclose(file);
            mpack_tree_init_error(tree, mpack_error_io);
            MPACK_FREE(file_tree->data);
            return false;
        }
        total += read;
    }

    fclose(file);
    file_tree->size = (size_t)size;
    return true;
}

void mpack_tree_init_file(mpack_tree_t* tree, const char* filename, size_t max_size) {

    // the C STDIO family of file functions use long (e.g. ftell)
    if (max_size > LONG_MAX) {
        mpack_break("max_size of %" PRIu64 " is invalid, maximum is LONG_MAX", (uint64_t)max_size);
        mpack_tree_init_error(tree, mpack_error_bug);
        return;
    }

    // allocate file tree
    mpack_file_tree_t* file_tree = (mpack_file_tree_t*) MPACK_MALLOC(sizeof(mpack_file_tree_t));
    if (file_tree == NULL) {
        mpack_tree_init_error(tree, mpack_error_memory);
        return;
    }

    // read all data
    if (!mpack_file_tree_read(tree, file_tree, filename, max_size)) {
        MPACK_FREE(file_tree);
        return;
    }

    mpack_tree_init(tree, file_tree->data, file_tree->size);
    mpack_tree_set_context(tree, file_tree);
    mpack_tree_set_teardown(tree, mpack_file_tree_teardown);
}
#endif

mpack_error_t mpack_tree_destroy(mpack_tree_t* tree) {
    #ifdef MPACK_MALLOC
    mpack_tree_page_t* page = tree->next;
    while (page) {
        mpack_tree_page_t* next = page->next;
        mpack_log("freeing page %p\n", page);
        MPACK_FREE(page);
        page = next;
    }
    #endif

    if (tree->teardown)
        tree->teardown(tree);
    tree->teardown = NULL;

    return tree->error;
}

void mpack_tree_flag_error(mpack_tree_t* tree, mpack_error_t error) {
    if (tree->error == mpack_ok) {
        mpack_log("tree %p setting error %i: %s\n", tree, (int)error, mpack_error_to_string(error));
        tree->error = error;
        if (tree->error_fn)
            tree->error_fn(tree, error);
    }

}



/*
 * Node misc functions
 */

void mpack_node_flag_error(mpack_node_t node, mpack_error_t error) {
    mpack_tree_flag_error(node.tree, error);
}

mpack_tag_t mpack_node_tag(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return mpack_tag_nil();

    mpack_tag_t tag;
    mpack_memset(&tag, 0, sizeof(tag));

    tag.type = node.data->type;
    switch (node.data->type) {
        case mpack_type_nil:                                            break;
        case mpack_type_bool:    tag.v.b = node.data->value.b;          break;
        case mpack_type_float:   tag.v.f = node.data->value.f;          break;
        case mpack_type_double:  tag.v.d = node.data->value.d;          break;
        case mpack_type_int:     tag.v.i = node.data->value.i;          break;
        case mpack_type_uint:    tag.v.u = node.data->value.u;          break;

        case mpack_type_str:     tag.v.l = node.data->len;     break;
        case mpack_type_bin:     tag.v.l = node.data->len;     break;

        case mpack_type_ext:
            tag.v.l = node.data->len;
            // the exttype of an ext node is stored in the byte preceding the data
            tag.exttype = (int8_t)*(node.data->value.bytes - 1);
            break;

        case mpack_type_array:   tag.v.n = node.data->len;  break;
        case mpack_type_map:     tag.v.n = node.data->len;  break;

        default:
            mpack_assert(0, "unrecognized type %i", (int)node.data->type);
            break;
    }
    return tag;
}

#if MPACK_STDIO
static void mpack_node_print_element(mpack_node_t node, size_t depth, FILE* file) {
    mpack_node_data_t* data = node.data;
    switch (data->type) {

        case mpack_type_nil:
            fprintf(file, "null");
            break;
        case mpack_type_bool:
            fprintf(file, data->value.b ? "true" : "false");
            break;

        case mpack_type_float:
            fprintf(file, "%f", data->value.f);
            break;
        case mpack_type_double:
            fprintf(file, "%f", data->value.d);
            break;

        case mpack_type_int:
            fprintf(file, "%" PRIi64, data->value.i);
            break;
        case mpack_type_uint:
            fprintf(file, "%" PRIu64, data->value.u);
            break;

        case mpack_type_bin:
            fprintf(file, "<binary data of length %u>", data->len);
            break;

        case mpack_type_ext:
            fprintf(file, "<ext data of type %i and length %u>",
                    mpack_node_exttype(node), data->len);
            break;

        case mpack_type_str:
            {
                putc('"', file);
                const char* bytes = mpack_node_data(node);
                for (size_t i = 0; i < data->len; ++i) {
                    char c = bytes[i];
                    switch (c) {
                        case '\n': fprintf(file, "\\n"); break;
                        case '\\': fprintf(file, "\\\\"); break;
                        case '"': fprintf(file, "\\\""); break;
                        default: putc(c, file); break;
                    }
                }
                putc('"', file);
            }
            break;

        case mpack_type_array:
            fprintf(file, "[\n");
            for (size_t i = 0; i < data->len; ++i) {
                for (size_t j = 0; j < depth + 1; ++j)
                    fprintf(file, "    ");
                mpack_node_print_element(mpack_node_array_at(node, i), depth + 1, file);
                if (i != data->len - 1)
                    putc(',', file);
                putc('\n', file);
            }
            for (size_t i = 0; i < depth; ++i)
                fprintf(file, "    ");
            putc(']', file);
            break;

        case mpack_type_map:
            fprintf(file, "{\n");
            for (size_t i = 0; i < data->len; ++i) {
                for (size_t j = 0; j < depth + 1; ++j)
                    fprintf(file, "    ");
                mpack_node_print_element(mpack_node_map_key_at(node, i), depth + 1, file);
                fprintf(file, ": ");
                mpack_node_print_element(mpack_node_map_value_at(node, i), depth + 1, file);
                if (i != data->len - 1)
                    putc(',', file);
                putc('\n', file);
            }
            for (size_t i = 0; i < depth; ++i)
                fprintf(file, "    ");
            putc('}', file);
            break;
    }
}

void mpack_node_print_file(mpack_node_t node, FILE* file) {
    mpack_assert(file != NULL, "file is NULL");
    int depth = 2;
    for (int i = 0; i < depth; ++i)
        fprintf(file, "    ");
    mpack_node_print_element(node, depth, file);
    putc('\n', file);
}
#endif



/*
 * Node Data Functions
 */

void mpack_node_check_utf8(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return;
    mpack_node_data_t* data = node.data;
    if (data->type != mpack_type_str || !mpack_utf8_check(data->value.bytes, data->len))
        mpack_node_flag_error(node, mpack_error_type);
}

void mpack_node_check_utf8_cstr(mpack_node_t node) {
    if (mpack_node_error(node) != mpack_ok)
        return;
    mpack_node_data_t* data = node.data;
    if (data->type != mpack_type_str || !mpack_utf8_check_no_null(data->value.bytes, data->len))
        mpack_node_flag_error(node, mpack_error_type);
}

size_t mpack_node_copy_data(mpack_node_t node, char* buffer, size_t bufsize) {
    if (mpack_node_error(node) != mpack_ok)
        return 0;

    mpack_assert(bufsize == 0 || buffer != NULL, "buffer is NULL for maximum of %i bytes", (int)bufsize);

    mpack_type_t type = node.data->type;
    if (type != mpack_type_str && type != mpack_type_bin && type != mpack_type_ext) {
        mpack_node_flag_error(node, mpack_error_type);
        return 0;
    }

    if (node.data->len > bufsize) {
        mpack_node_flag_error(node, mpack_error_too_big);
        return 0;
    }

    mpack_memcpy(buffer, node.data->value.bytes, node.data->len);
    return (size_t)node.data->len;
}

size_t mpack_node_copy_utf8(mpack_node_t node, char* buffer, size_t bufsize) {
    if (mpack_node_error(node) != mpack_ok)
        return 0;

    mpack_assert(bufsize == 0 || buffer != NULL, "buffer is NULL for maximum of %i bytes", (int)bufsize);

    mpack_type_t type = node.data->type;
    if (type != mpack_type_str) {
        mpack_node_flag_error(node, mpack_error_type);
        return 0;
    }

    if (node.data->len > bufsize) {
        mpack_node_flag_error(node, mpack_error_too_big);
        return 0;
    }

    if (!mpack_utf8_check(node.data->value.bytes, node.data->len)) {
        mpack_node_flag_error(node, mpack_error_type);
        return 0;
    }

    mpack_memcpy(buffer, node.data->value.bytes, node.data->len);
    return (size_t)node.data->len;
}

void mpack_node_copy_cstr(mpack_node_t node, char* buffer, size_t bufsize) {

    // we can't break here because the error isn't recoverable; we
    // have to add a null-terminator.
    mpack_assert(buffer != NULL, "buffer is NULL");
    mpack_assert(bufsize >= 1, "buffer size is zero; you must have room for at least a null-terminator");

    if (mpack_node_error(node) != mpack_ok) {
        buffer[0] = '\0';
        return;
    }

    if (node.data->type != mpack_type_str) {
        buffer[0] = '\0';
        mpack_node_flag_error(node, mpack_error_type);
        return;
    }

    if (node.data->len > bufsize - 1) {
        buffer[0] = '\0';
        mpack_node_flag_error(node, mpack_error_too_big);
        return;
    }

    if (!mpack_str_check_no_null(node.data->value.bytes, node.data->len)) {
        buffer[0] = '\0';
        mpack_node_flag_error(node, mpack_error_type);
        return;
    }

    mpack_memcpy(buffer, node.data->value.bytes, node.data->len);
    buffer[node.data->len] = '\0';
}

void mpack_node_copy_utf8_cstr(mpack_node_t node, char* buffer, size_t bufsize) {

    // we can't break here because the error isn't recoverable; we
    // have to add a null-terminator.
    mpack_assert(buffer != NULL, "buffer is NULL");
    mpack_assert(bufsize >= 1, "buffer size is zero; you must have room for at least a null-terminator");

    if (mpack_node_error(node) != mpack_ok) {
        buffer[0] = '\0';
        return;
    }

    if (node.data->type != mpack_type_str) {
        buffer[0] = '\0';
        mpack_node_flag_error(node, mpack_error_type);
        return;
    }

    if (node.data->len > bufsize - 1) {
        buffer[0] = '\0';
        mpack_node_flag_error(node, mpack_error_too_big);
        return;
    }

    if (!mpack_utf8_check_no_null(node.data->value.bytes, node.data->len)) {
        buffer[0] = '\0';
        mpack_node_flag_error(node, mpack_error_type);
        return;
    }

    mpack_memcpy(buffer, node.data->value.bytes, node.data->len);
    buffer[node.data->len] = '\0';
}

#ifdef MPACK_MALLOC
char* mpack_node_data_alloc(mpack_node_t node, size_t maxlen) {
    if (mpack_node_error(node) != mpack_ok)
        return NULL;

    // make sure this is a valid data type
    mpack_type_t type = node.data->type;
    if (type != mpack_type_str && type != mpack_type_bin && type != mpack_type_ext) {
        mpack_node_flag_error(node, mpack_error_type);
        return NULL;
    }

    if (node.data->len > maxlen) {
        mpack_node_flag_error(node, mpack_error_too_big);
        return NULL;
    }

    char* ret = (char*) MPACK_MALLOC((size_t)node.data->len);
    if (ret == NULL) {
        mpack_node_flag_error(node, mpack_error_memory);
        return NULL;
    }

    mpack_memcpy(ret, node.data->value.bytes, node.data->len);
    return ret;
}

char* mpack_node_cstr_alloc(mpack_node_t node, size_t maxlen) {
    if (mpack_node_error(node) != mpack_ok)
        return NULL;

    // make sure maxlen makes sense
    if (maxlen < 1) {
        mpack_break("maxlen is zero; you must have room for at least a null-terminator");
        mpack_node_flag_error(node, mpack_error_bug);
        return NULL;
    }

    if (node.data->type != mpack_type_str) {
        mpack_node_flag_error(node, mpack_error_type);
        return NULL;
    }

    if (node.data->len > maxlen - 1) {
        mpack_node_flag_error(node, mpack_error_too_big);
        return NULL;
    }

    if (!mpack_str_check_no_null(node.data->value.bytes, node.data->len)) {
        mpack_node_flag_error(node, mpack_error_type);
        return NULL;
    }

    char* ret = (char*) MPACK_MALLOC((size_t)(node.data->len + 1));
    if (ret == NULL) {
        mpack_node_flag_error(node, mpack_error_memory);
        return NULL;
    }

    mpack_memcpy(ret, node.data->value.bytes, node.data->len);
    ret[node.data->len] = '\0';
    return ret;
}

char* mpack_node_utf8_cstr_alloc(mpack_node_t node, size_t maxlen) {
    if (mpack_node_error(node) != mpack_ok)
        return NULL;

    // make sure maxlen makes sense
    if (maxlen < 1) {
        mpack_break("maxlen is zero; you must have room for at least a null-terminator");
        mpack_node_flag_error(node, mpack_error_bug);
        return NULL;
    }

    if (node.data->type != mpack_type_str) {
        mpack_node_flag_error(node, mpack_error_type);
        return NULL;
    }

    if (node.data->len > maxlen - 1) {
        mpack_node_flag_error(node, mpack_error_too_big);
        return NULL;
    }

    if (!mpack_utf8_check_no_null(node.data->value.bytes, node.data->len)) {
        mpack_node_flag_error(node, mpack_error_type);
        return NULL;
    }

    char* ret = (char*) MPACK_MALLOC((size_t)(node.data->len + 1));
    if (ret == NULL) {
        mpack_node_flag_error(node, mpack_error_memory);
        return NULL;
    }

    mpack_memcpy(ret, node.data->value.bytes, node.data->len);
    ret[node.data->len] = '\0';
    return ret;
}
#endif


/*
 * Compound Node Functions
 */

static mpack_node_data_t* mpack_node_map_int_impl(mpack_node_t node, int64_t num) {
    if (mpack_node_error(node) != mpack_ok)
        return NULL;

    if (node.data->type != mpack_type_map) {
        mpack_node_flag_error(node, mpack_error_type);
        return NULL;
    }

    mpack_node_data_t* found = NULL;

    for (size_t i = 0; i < node.data->len; ++i) {
        mpack_node_data_t* key = mpack_node_child(node, i * 2);

        if ((key->type == mpack_type_int && key->value.i == num) ||
            (key->type == mpack_type_uint && num >= 0 && key->value.u == (uint64_t)num))
        {
            if (found) {
                mpack_node_flag_error(node, mpack_error_data);
                return NULL;
            }
            found = mpack_node_child(node, i * 2 + 1);
        }
    }

    if (found)
        return found;

    return NULL;
}

static mpack_node_data_t* mpack_node_map_uint_impl(mpack_node_t node, uint64_t num) {
    if (mpack_node_error(node) != mpack_ok)
        return NULL;

    if (node.data->type != mpack_type_map) {
        mpack_node_flag_error(node, mpack_error_type);
        return NULL;
    }

    mpack_node_data_t* found = NULL;

    for (size_t i = 0; i < node.data->len; ++i) {
        mpack_node_data_t* key = mpack_node_child(node, i * 2);

        if ((key->type == mpack_type_uint && key->value.u == num) ||
            (key->type == mpack_type_int && key->value.i >= 0 && (uint64_t)key->value.i == num))
        {
            if (found) {
                mpack_node_flag_error(node, mpack_error_data);
                return NULL;
            }
            found = mpack_node_child(node, i * 2 + 1);
        }
    }

    if (found)
        return found;

    return NULL;
}

static mpack_node_data_t* mpack_node_map_str_impl(mpack_node_t node, const char* str, size_t length) {
    if (mpack_node_error(node) != mpack_ok)
        return NULL;

    mpack_assert(length == 0 || str != NULL, "str of length %i is NULL", (int)length);

    if (node.data->type != mpack_type_map) {
        mpack_node_flag_error(node, mpack_error_type);
        return NULL;
    }

    mpack_node_data_t* found = NULL;

    for (size_t i = 0; i < node.data->len; ++i) {
        mpack_node_data_t* key = mpack_node_child(node, i * 2);

        if (key->type == mpack_type_str && key->len == length && mpack_memcmp(str, key->value.bytes, length) == 0) {
            if (found) {
                mpack_node_flag_error(node, mpack_error_data);
                return NULL;
            }
            found = mpack_node_child(node, i * 2 + 1);
        }
    }

    if (found)
        return found;

    return NULL;
}

static mpack_node_t mpack_node_wrap_lookup(mpack_tree_t* tree, mpack_node_data_t* data) {
    if (!data) {
        if (tree->error == mpack_ok)
            mpack_tree_flag_error(tree, mpack_error_data);
        return mpack_tree_nil_node(tree);
    }
    return mpack_node(tree, data);
}

static mpack_node_t mpack_node_wrap_lookup_optional(mpack_tree_t* tree, mpack_node_data_t* data) {
    if (!data)
        return mpack_tree_nil_node(tree);
    return mpack_node(tree, data);
}

mpack_node_t mpack_node_map_int(mpack_node_t node, int64_t num) {
    return mpack_node_wrap_lookup(node.tree, mpack_node_map_int_impl(node, num));
}

mpack_node_t mpack_node_map_int_optional(mpack_node_t node, int64_t num) {
    return mpack_node_wrap_lookup_optional(node.tree, mpack_node_map_int_impl(node, num));
}

mpack_node_t mpack_node_map_uint(mpack_node_t node, uint64_t num) {
    return mpack_node_wrap_lookup(node.tree, mpack_node_map_uint_impl(node, num));
}

mpack_node_t mpack_node_map_uint_optional(mpack_node_t node, uint64_t num) {
    return mpack_node_wrap_lookup_optional(node.tree, mpack_node_map_uint_impl(node, num));
}

mpack_node_t mpack_node_map_str(mpack_node_t node, const char* str, size_t length) {
    return mpack_node_wrap_lookup(node.tree, mpack_node_map_str_impl(node, str, length));
}

mpack_node_t mpack_node_map_str_optional(mpack_node_t node, const char* str, size_t length) {
    return mpack_node_wrap_lookup_optional(node.tree, mpack_node_map_str_impl(node, str, length));
}

mpack_node_t mpack_node_map_cstr(mpack_node_t node, const char* cstr) {
    mpack_assert(cstr != NULL, "cstr is NULL");
    return mpack_node_map_str(node, cstr, mpack_strlen(cstr));
}

mpack_node_t mpack_node_map_cstr_optional(mpack_node_t node, const char* cstr) {
    mpack_assert(cstr != NULL, "cstr is NULL");
    return mpack_node_map_str_optional(node, cstr, mpack_strlen(cstr));
}

bool mpack_node_map_contains_int(mpack_node_t node, int64_t num) {
    return mpack_node_map_int_impl(node, num) != NULL;
}

bool mpack_node_map_contains_uint(mpack_node_t node, uint64_t num) {
    return mpack_node_map_uint_impl(node, num) != NULL;
}

bool mpack_node_map_contains_str(mpack_node_t node, const char* str, size_t length) {
    return mpack_node_map_str_impl(node, str, length) != NULL;
}

bool mpack_node_map_contains_cstr(mpack_node_t node, const char* cstr) {
    mpack_assert(cstr != NULL, "cstr is NULL");
    return mpack_node_map_contains_str(node, cstr, mpack_strlen(cstr));
}

size_t mpack_node_enum_optional(mpack_node_t node, const char* strings[], size_t count) {
    if (mpack_node_error(node) != mpack_ok)
        return count;

    // the value is only recognized if it is a string
    if (mpack_node_type(node) != mpack_type_str)
        return count;

    // fetch the string
    const char* key = mpack_node_str(node);
    size_t keylen = mpack_node_strlen(node);
    mpack_assert(mpack_node_error(node) == mpack_ok, "these should not fail");

    // find what key it matches
    for (size_t i = 0; i < count; ++i) {
        const char* other = strings[i];
        size_t otherlen = mpack_strlen(other);
        if (keylen == otherlen && mpack_memcmp(key, other, keylen) == 0)
            return i;
    }

    // no matches
    return count;
}

size_t mpack_node_enum(mpack_node_t node, const char* strings[], size_t count) {
    size_t value = mpack_node_enum_optional(node, strings, count);
    if (value == count)
        mpack_node_flag_error(node, mpack_error_type);
    return value;
}

#endif


