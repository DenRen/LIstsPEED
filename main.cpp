#include <cstdio>
#include <MyLib.h>
#include <Super_Stack_Library.h>

namespace lst {
    typedef __int32_t data_t;
    typedef __int16_t num_t;   // Тип номера списка (обязательно signed)

    enum err_constants {
        ALL_RIGHT,
        CONSTR_EMPTY_SIZE,
        CALLOC_ERR,
        NO_ELEMENT
    };

}


struct list_t {
    lst::num_t size;    // Максимальное возможное кол-во элементов в списке
    lst::data_t *data;  // Указатель непосредственно на данные
    lst::num_t *next;  // Служебный массив номеров следующих элементов
    lst::num_t *prev;  // Служебный массив номеров предыдущих элементов
    lst::num_t head;   // Номер первого элемента в списке (если 0, то список пуст)
    lst::num_t tail;   // Номер последнего элемента в списке (если 0, то список пуст, если head, то 1 элемент в списке)
    lst::num_t free;   // Первый номер свободной ячейки в списке
    lst::num_t count;  // Количество занятых ячеек
};

int list_Construct (list_t *list);

void list_Destruct (list_t *list);

lst::num_t real2phys (lst::num_t number);

lst::num_t phys2real (lst::num_t number);

lst::num_t add_right (list_t *list, lst::data_t value, lst::num_t number, __uint8_t *state_func);

lst::num_t add_left (list_t *list, lst::data_t value, lst::num_t number, __uint8_t *state_func);

lst::num_t add_end (list_t *list, lst::data_t value, __uint8_t *state_func);

lst::num_t del_elem (list_t *list, __uint8_t *state_func);

lst::num_t find_elem (list_t *list, lst::num_t value, __uint8_t *state_func);

void Dump_list (list_t *list, FILE *file);

int main () {
    struct list_t list = {};
    list.size = 10;

    if (list_Construct (&list) != lst::ALL_RIGHT) {
        list_Destruct (&list);
        return 0;
    }

    Dump_list (&list, stdout);
    __uint8_t state_func = 0;
    for (int i = 10; i < 190; i += 10) {
        add_end (&list, i, &state_func);
        Dump_list (&list, stdout);
    }


    list_Destruct (&list);


    return 0;
}

// Список очень гибкий и не выдаёт ошибки, если добавлять его некуда, а записывает в
// первый логический элемент. Этим можно пользоваться очень хитро, но при этом
// за "переполнением" должен следить программист, сравнивая list.count с list.size
lst::num_t add_right (list_t *list, lst::data_t value, lst::num_t number, __uint8_t *state_func) {

    if (number == list->tail)
        return add_end (list, value, state_func);  // Физ номер, куда был встален новый элемент

    lst::num_t temp_free = list->free;
    lst::num_t temp_tail = list->tail;

    list->data[temp_free] = value;  //
    list->tail = temp_free;         //

    if (temp_free == list->head && list->count > 1)
        list->head = list->next[temp_free];

    list->free = list->next[temp_free];

    list->next[temp_free] = list->head;

    list->prev[temp_free] = temp_tail;
    list->next[temp_tail] = temp_free;

    list->prev[list->head] = list->tail;

    list->count++;
    *state_func = lst::ALL_RIGHT;


    lst::num_t temp = list->next[number];
    list->next[number] = number_new;
    list->next[number_new] = temp;


    return number_new;
}

lst::num_t add_end (list_t *list, lst::data_t value, __uint8_t *state_func) {
    // Спросить, что быстрее  list->data[list->free] = value или temp = list->free
    // list->data[temp] = value. Таких обращений к дате сотни. Если что, то переписать.

    lst::num_t temp_free = list->free;
    lst::num_t temp_tail = list->tail;

    list->data[temp_free] = value;  //
    list->tail = temp_free;         //

    if (temp_free == list->head && list->count > 1)
        list->head = list->next[temp_free];

    list->free = list->next[temp_free];

    list->next[temp_free] = list->head;

    list->prev[temp_free] = temp_tail;
    list->next[temp_tail] = temp_free;

    list->prev[list->head] = list->tail;

    list->count++;
    *state_func = lst::ALL_RIGHT;
    return temp_free;
}

int list_Construct (list_t *list) {
    // Выдаёт предупреждение, если указан желаемый некорректный размер
    if (list->size <= 0) {
        PRINT_WARNING ("Constructor: Incorrect value size")
        return lst::CONSTR_EMPTY_SIZE;
    }

    // Контроллируется создание каждого массива
    if ((list->data = (lst::data_t *) calloc (list->size, sizeof (lst::data_t))) == nullptr) {
        PRINT_ERROR ("Failed to create data array. Clear the list")
    } else if ((list->next = (lst::num_t *) calloc (list->size, sizeof (lst::num_t))) == nullptr) {
        PRINT_ERROR ("Failed to create next array. Clear the list")
        free (list->data);
    } else if ((list->prev = (lst::num_t *) calloc (list->size, sizeof (lst::num_t))) == nullptr) {
        PRINT_ERROR ("Failed to create prev array. Clear the list")
        free (list->next);
        free (list->data);
    } else {    // Начальные значения переменных и массивов
        list->head = list->tail = 0;
        list->free = list->count = 0;

        for (lst::num_t i = 0; i < list->size; i++)
            list->next[i] = i + 1;
        list->next[list->size - 1] = 0;

        /*list->prev[0] = list->size - 1;
        for (lst::num_t i = 0; i < list->size - 1; i++)
            list->prev[i + 1] = i;*/

        return lst::ALL_RIGHT;
    }
    return lst::CALLOC_ERR;
}

void list_Destruct (list_t *list) {
    // Очищает и обнуляет всё, кроме list->size
    // Поэтому после деструктора этот list можно
    // использовать повторно в конструкторе

    free (list->prev);
    list->prev = nullptr;

    free (list->next);
    list->next = nullptr;

    free (list->data);
    list->data = nullptr;

    list->free = list->count = 0;
    list->head = list->tail = 0;
}

inline lst::num_t real2phys (lst::num_t number) {
    return number + 1;  // Написать для отрицательных номеров
}

inline lst::num_t phys2real (lst::num_t number) {
    return number - 1;
}


void Dump_list (list_t *list, FILE *file) {
/* IBM864
133) ─
134) │
135) ┼
136) ┤
137) ┬
138) ├
139) ┴
140) ┐
141) ┌
142) └
143) ┘
*/
    __uint8_t length_number = 3;

    //fprintf (file, "%4c│", ' ');
    fprintf (file, "  N │");

    for (__uint16_t i = 0; i < list->size; i++)
        fprintf (file, "%3d |", i);

    fprintf (file, " list->head%5d", list->head);

    fprintf (file, "\n────┼");
    for (__uint16_t i = 0; i < list->size - 1; i++)
        fprintf (file, "────┼");
    fprintf (file, "────┤\n");

    // data
    fprintf (file, "data|");
    for (__uint16_t i = 0; i < list->size; i++)
        fprintf (file, "%4d|", list->data[i]);

    fprintf (file, " list->tail%5d", list->tail);
    fprintf (file, "\n────┼");
    for (__uint16_t i = 0; i < list->size - 1; i++)
        fprintf (file, "────┼");
    fprintf (file, "────┤\n");

    // next
    fprintf (file, "next|");
    for (__uint16_t i = 0; i < list->size; i++)
        fprintf (file, "%4d|", list->next[i]);


    fprintf (file, " list->free%5d", list->free);
    fprintf (file, "\n────┼");
    for (__uint16_t i = 0; i < list->size - 1; i++)
        fprintf (file, "────┼");
    fprintf (file, "────┤\n");

    // prev
    fprintf (file, "prev|");
    for (__uint16_t i = 0; i < list->size; i++)
        fprintf (file, "%4d|", list->prev[i]);

    fprintf (file, " list->count%4d", list->count);
    fprintf (file, "\n────┼");
    for (__uint16_t i = 0; i < list->size - 1; i++)
        fprintf (file, "────┴");
    fprintf (file, "────┘\n");

    fprintf (file, "\n");
}
