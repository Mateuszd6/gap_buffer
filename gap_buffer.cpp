#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <ncurses.h>

#define DEBUG
#define LOGGING
#include "../debugGoodies/debug_goodies.h"

typedef uint8_t uint8;

#define IS_NULL(EXPR) ((EXPR) == nullptr)

// TODO: Summary
#define IS_EARLIER_IN_ARR(ARR, FIRST, SECOND) (FIRST - ARR < SECOND - ARR)

struct gap_buffer
{
    uint8* buffer;
    size_t alloced_mem_size;

    // Navigation pointers:
    uint8* gap_start;
    uint8* gap_end;
    // uint8* buffer_end;
    uint8* curr_point;

    void initialize();
    void make_gap();

    void cursor_backwards();
    void cursor_forward();
    void insert_at_point(uint8 character);

    void DEBUG_print_state() const;
};

void gap_buffer::initialize()
{
    alloced_mem_size = 80;
    buffer = (uint8*)std::malloc(sizeof(uint8) * alloced_mem_size);
    // TODO(Cleanup): Panic on fail!
    // buffer_end = buffer;

    gap_start = buffer;
    gap_end = buffer + alloced_mem_size;

    curr_point = buffer;
}

void gap_buffer::cursor_backwards()
{
    if (curr_point - buffer > 0) // TODO: USe Fancy macro.
    {
        if (curr_point == gap_end + 1) // TODO(Cleanup): Is this legal -> gap_end + 1.
            curr_point = gap_start;
        else
            curr_point--;
    }
    else
        PANIC("Already at the begin of the line!");
}

void gap_buffer::cursor_forward()
{
    if ((buffer + alloced_mem_size) - curr_point > 0)  // TODO: USe Fancy macro.
    {
        if (curr_point == gap_start)
            curr_point = gap_end + 1; // TODO(Cleanup): Is this legal -> gap_end + 1.
        else
            curr_point++;
    }
    else
        PANIC("Already at the end of the line!");
}

#if 0
if (IS_EARLIER_IN_ARR(buffer, curr_point, gap_end) &&
    (IS_EARLIER_IN_ARR(buffer, gap_start, curr_point) || gap_start == curr_point))
#endif

void gap_buffer::insert_at_point(uint8 character) // LATIN2 characters only.
{
    // If cursor is inside the buffer.
    if (curr_point == gap_start)
    {
        *curr_point = character;
        curr_point++;
        gap_start++;
    }
    else if (IS_EARLIER_IN_ARR(buffer, curr_point, gap_start))
    {
        auto diff = gap_start - curr_point;
        memcpy(gap_end - diff, curr_point, sizeof(uint8) * diff);
        gap_start -= diff;
        gap_end -= diff;
        insert_at_point(character);
    }
    else if (IS_EARLIER_IN_ARR(buffer, gap_start, curr_point))
    {
        PANIC("Unsupported!");
    }
    else UNREACHABLE();
}

void gap_buffer::DEBUG_print_state() const
{
    char print_buffer[alloced_mem_size + 5];
    auto print_buffer_idx = 0LLU;
    auto in_gap = false;
    auto printed_chars = 0LLU;

    auto ptr = buffer;
    for (auto i = 0ULL; i < alloced_mem_size; ++i)
    {
        if (ptr == gap_start)
            in_gap = true;

        if (ptr == gap_end)
            in_gap = false;

        print_buffer[print_buffer_idx++] = (in_gap ? '?' : *ptr);
        printed_chars++;

        ptr++;
    }
    // print_buffer[print_buffer_idx++] = '0';
    // ASSERT(print_buffer_idx < alloced_mem_size); // TODO: Or <= ????
    for (auto i = print_buffer_idx; i < alloced_mem_size; ++i)
        print_buffer[print_buffer_idx++] = '_';
    print_buffer[print_buffer_idx++] = '\n';
    print_buffer[print_buffer_idx++] = '\0';
    printf(print_buffer);

    print_buffer_idx = 0;
    ptr = buffer;
    for (auto i = 0ULL; i <= alloced_mem_size; ++i)
    {
        if (ptr == curr_point)
            print_buffer[print_buffer_idx++] = '^';
        else if (ptr == gap_start)
            print_buffer[print_buffer_idx++] = 'b';
        else if (ptr == gap_end)
            print_buffer[print_buffer_idx++] = 'e';
        else
            print_buffer[print_buffer_idx++] = ' ';
        ptr++;
    }
    print_buffer[print_buffer_idx++] = '\n';
    print_buffer[print_buffer_idx++] = '\0';
    printf(print_buffer);

}

// ---------------------------------

int main()
{
    std::printf("Size of gap_buffer struct: %ld\n", sizeof(gap_buffer));

    gap_buffer test_line;
    test_line.initialize();
    test_line.DEBUG_print_state();
#if 1
    test_line.insert_at_point('M');
    test_line.insert_at_point('a');
    test_line.insert_at_point('t');
    test_line.insert_at_point('e');
    test_line.insert_at_point('u');
    test_line.insert_at_point('s');
    test_line.insert_at_point('z');
    test_line.DEBUG_print_state();
    test_line.cursor_backwards();
    test_line.DEBUG_print_state();
    test_line.insert_at_point('Z');
    test_line.DEBUG_print_state();
#endif

    while(1)
    {
        char c;
        scanf("%c", &c);
        // printf("%d\n\n", static_cast<int>(c));

        switch(static_cast<int>(c))
        {
            case 113:
            {
                test_line.cursor_backwards();
            } break;

            case 119:
            {
                test_line.cursor_forward();
            } break;

            default:
            {
                if ('a' <= c && c <= 'z')
                    test_line.insert_at_point(c);
            } break;
        }

        system("clear");
        test_line.DEBUG_print_state();
    }
    return 0;
}
