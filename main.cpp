#include <cstdio>
#include <MyLib.h>


namespace lst {
    typedef __int32_t data_t;
    typedef __uint16_t next_t;
    typedef __uint16_t prev_t;

    enum err_constants {
        ALL_RIGHT,
        CONSTR_EMPTY_SIZE,
        CALLOC_ERR,

    };
}


struct list_t {
    __uint16_t size;
    lst::data_t *data;
    lst::next_t *next;
    lst::prev_t *prev;
    lst::next_t head;
    lst::next_t tail;
    lst::next_t free;
};

int list_Construct (list_t *list);

int main () {
    struct list_t list = {};
    list.size = 64;

    return 0;
}


int list_Construct (list_t *list) {
    // Выдаёт предупреждение, если указан желаемый некорректный размер
    if (list->size <= 0) {
        PRINT_WARNING ("Constructor: Empty list")
        return lst::CONSTR_EMPTY_SIZE;
    }

    if ((list->data = (lst::data_t *) calloc (list->size, sizeof (lst::data_t))) == nullptr) {
        PRINT_ERROR ("Failed to create data array. Clear the list")
    } else if ((list->next = (lst::next_t *) calloc (list->size, sizeof (lst::next_t))) == nullptr) {
        PRINT_ERROR ("Failed to create next array. Clear the list")
        free (list->data);
    } else if ((list->prev = (lst::prev_t *) calloc (list->size, sizeof (lst::prev_t))) == nullptr) {
        PRINT_ERROR ("Failed to create prev array. Clear the list")
        free (list->next);
        free (list->data);
    } else {



        return lst::ALL_RIGHT;
    }
    return lst::CALLOC_ERR;
}