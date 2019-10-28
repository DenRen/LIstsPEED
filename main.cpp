#include <cstdio>
#include <MyLib.h>
#include <Super_Stack_Library.h>
#include <cstdlib>

#define CHECK_LIST1A(arg, arg1)  \
if (!lst::dmp_ON) {                                                                                 \
        if (checking_list (list) != lst::ALL_RIGHT) {                                               \
            *state_func = lst::REALLOC_ERR;                                                         \
            return -1;                                                                              \
        }                                                                                           \
    } else {                                                                                        \
        char string[30];                                                                            \
        sprintf (string, arg, arg1);                                                                \
        if (checking_list (list, string) != lst::ALL_RIGHT) {                                       \
            *state_func = lst::REALLOC_ERR;                                                         \
            return -1;                                                                              \
        }                                                                                           \
    }

#define CHECK_LIST2A(arg, arg1, arg2)  \
if (!lst::dmp_ON) {                                                                                 \
        if (checking_list (list) != lst::ALL_RIGHT) {                                               \
            *state_func = lst::REALLOC_ERR;                                                         \
            return -1;                                                                              \
        }                                                                                           \
    } else {                                                                                        \
        char string[30];                                                                            \
        sprintf (string, arg, arg1, arg2);                                                          \
        if (checking_list (list, string) != lst::ALL_RIGHT) {                                       \
            *state_func = lst::REALLOC_ERR;                                                         \
            return -1;                                                                              \
        }                                                                                           \
    }

#define CHECK_LIST1A_V(arg, arg1)  \
if (!lst::dmp_ON) {                                                                                 \
        if (checking_list (list) != lst::ALL_RIGHT) {                                               \
            *state_func = lst::REALLOC_ERR;                                                         \
            return;                                                                                 \
        }                                                                                           \
    } else {                                                                                        \
        char string[30];                                                                            \
        sprintf (string, arg, arg1);                                                                \
        if (checking_list (list, string) != lst::ALL_RIGHT) {                                       \
            *state_func = lst::REALLOC_ERR;                                                         \
            return;                                                                                 \
        }                                                                                           \
    }

namespace lst {
    typedef __int32_t data_t;
    typedef __int16_t num_t;   // Тип номера списка (обязательно signed)

    enum err_constants {
        ALL_RIGHT,
        CONSTR_EMPTY_SIZE,
        INCORRECT_EXPN,
        CALLOC_ERR,
        REALLOC_ERR,
        NO_ELEMENT
    };
    bool dmp_ON = true;
    bool dmp_DOT_ON = true;
    bool dmp_DOT_SHOW_AT_ONCE = false;
    bool dmp_Creat_Animatoin = true;
    bool dmp_stick = true;

    void create_Animation_dot ();
}

struct list_t {
    lst::num_t size;    // Максимальное возможное кол-во элементов в списке
    double expn;        // Коэффициент расширения
    lst::data_t *data;  // Указатель непосредственно на данные
    lst::num_t *next;   // Служебный массив номеров следующих элементов
    lst::num_t *prev;   // Служебный массив номеров предыдущих элементов
    lst::num_t head;    // Номер первого элемента в списке (если 0, то список пуст)
    lst::num_t tail;    // Номер последнего элемента в списке (если 0, то список пуст, если head, то 1 элемент в списке)
    lst::num_t free;    // Первый номер свободной ячейки в списке
    lst::num_t count;   // Количество занятых ячеек

    FILE *fileDump;     // При включенном dmp_ON сюда будет записваться Dump
};

int list_Construct (list_t *list);

void list_Destruct (list_t *list);

lst::num_t add_right (list_t *list, lst::data_t value, lst::num_t number, __uint8_t *state_func);

lst::num_t add_left (list_t *list, lst::data_t value, lst::num_t number, __uint8_t *state_func);

lst::num_t add_end (list_t *list, lst::data_t value, __uint8_t *state_func);

lst::num_t add_begin (list_t *list, lst::data_t value, __uint8_t *state_func);

void del_elem (list_t *list, lst::num_t number, __uint8_t *state_func);

lst::num_t find_elem (list_t *list, lst::num_t value);

bool isfree (list_t *list, lst::num_t number, __uint8_t *state_func);

void Dump_list (list_t *list, FILE *file = stdout, char *string = nullptr);

void Dump_list_dot (list_t *list, char *string);

int checking_list (list_t *list, char *string = nullptr);

inline __uint64_t min_size_data (double expn) {
    return (__uint64_t) (1 / expn) + 1;
}

int Verificator_List (list_t *list);

int main () {
    struct list_t list = {};
    list.size = 8;
    list.expn = 1.2;
    list.fileDump = stdout;

    if (list_Construct (&list) != lst::ALL_RIGHT) {
        list_Destruct (&list);
        return 0;
    }

    Dump_list (&list, stdout);
    __uint8_t state_func = 0;
    for (int i = 10; i < 70; i += 10)
        add_end (&list, i, &state_func);

    del_elem (&list, 5, &state_func);
    add_right (&list, 5, 3, &state_func);
    add_right (&list, -57, 5, &state_func);
    add_right (&list, 7, 5, &state_func);
    add_end (&list, 50, &state_func);
    add_left (&list, 56, 4, &state_func);
    del_elem (&list, 1, &state_func);

    printf ("%d", find_elem (&list, 7));

    list_Destruct (&list);

    if (lst::dmp_Creat_Animatoin)
        lst::create_Animation_dot ();

    return 0;
}

lst::num_t find_elem (list_t *list, lst::num_t value) {
    lst::num_t ind = 0, tail = list->tail;
    for (ind = list->head; ind != tail; ind = list->next[ind])
        if (list->data[ind] == value)
            return ind;

    return list->data[ind] == value ? ind : -1;
}

bool isfree (list_t *list, lst::num_t number, __uint8_t *state_func) {
    if (checking_list (list) != lst::ALL_RIGHT) {
        *state_func = lst::REALLOC_ERR;
        return -1;
    }
    lst::num_t temp_size = list->size, __free = 0;
    for (__free = list->free; list->next[__free] != temp_size; __free = list->next[__free])
        if (__free == number)
            return true;
    return __free == number;
}

lst::num_t add_right (list_t *list, lst::data_t value, lst::num_t number, __uint8_t *state_func) {
    CHECK_LIST2A ("ADD to RIGHT of %d value == %d", number, value)

    if (number == list->tail) {
        lst::num_t new_num = add_end (list, value, state_func);
        return new_num;
    }

    lst::num_t temp_free = list->free;
    list->free = list->next[temp_free];

    list->data[temp_free] = value;

    lst::num_t temp_next = list->next[number];
    list->next[number] = temp_free;
    list->next[temp_free] = temp_next;

    list->prev[temp_free] = number;
    list->prev[temp_next] = temp_free;

    list->count++;
    return temp_free;
}

lst::num_t add_end (list_t *list, lst::data_t value, __uint8_t *state_func) {
    // Спросить, что быстрее  list->data[list->free] = value или temp = list->free
    // list->data[temp] = value. Таких обращений к дате сотни. Если что, то переписать.
    CHECK_LIST1A ("ADD to END value == %d", value)

    lst::num_t temp_free = list->free;
    list->free = list->next[temp_free];

    list->data[temp_free] = value;
    list->next[temp_free] = list->head;
    list->prev[temp_free] = list->tail;

    list->next[list->tail] = temp_free;
    list->tail = temp_free;

    list->count++;
    return temp_free;
}

lst::num_t add_left (list_t *list, lst::data_t value, lst::num_t number, __uint8_t *state_func) {
    CHECK_LIST2A ("ADD to LEFT of %d value == %d", number, value)

    if (number == list->head) {
        lst::num_t new_num = add_begin (list, value, state_func);
        return new_num;
    }

    lst::num_t temp_free = list->free;
    list->free = list->next[temp_free];

    list->data[temp_free] = value;

    list->next[temp_free] = number;
    list->prev[temp_free] = list->prev[number];

    list->next[list->prev[number]] = temp_free;
    list->prev[number] = temp_free;

    list->count++;
    return temp_free;
}

lst::num_t add_begin (list_t *list, lst::data_t value, __uint8_t *state_func) {
    // Спросить, что быстрее  list->data[list->free] = value или temp = list->free
    // list->data[temp] = value. Таких обращений к дате сотни. Если что, то переписать.

    CHECK_LIST1A ("ADD TO BEGIN value == %d", value)

    lst::num_t temp_free = list->free;
    list->free = list->next[temp_free];

    list->prev[list->head] = temp_free;

    list->data[temp_free] = value;
    list->next[temp_free] = list->head;
    list->prev[temp_free] = temp_free;

    list->head = temp_free;

    list->count++;
    return temp_free;
}

void del_elem (list_t *list, lst::num_t number, __uint8_t *state_func) {
    CHECK_LIST1A_V ("DEL num == %d", number)

    if (number == list->tail)
        list->tail = list->prev[number];
    else if (number == list->head)
        list->head = list->next[number];

    list->next[list->prev[number]] = list->next[number];
    list->prev[list->next[number]] = list->prev[number];

    list->next[number] = list->free;
    list->free = number;

    list->count--;
    *state_func = lst::ALL_RIGHT;
}

int list_Construct (list_t *list) {
    // Выдаёт предупреждение, если указан желаемый некорректный размер 11:30
    if (list->size <= 0) {
        PRINT_WARNING ("Constructor: Incorrect value size")
        return lst::CONSTR_EMPTY_SIZE;
    } else if (list->expn <= 0) {
        PRINT_ERROR ("Constructor: Incorrect value expansion")
        return lst::INCORRECT_EXPN;
    }

    if (list->size < min_size_data (list->expn))    // Проверка на минимальный размер массива списка
        list->size = min_size_data (list->expn);


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
        list->free = 0;
        list->count = 0;


        for (lst::num_t i = 0; i < list->size; i++)
            list->next[i] = i + 1;

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

int checking_list (list_t *list, char *string) {
    assert (Verificator_List (list));
    if (list->count == list->size) {
        lst::data_t *temp_data = nullptr;
        lst::num_t *temp_next = nullptr;
        lst::num_t *temp_prev = nullptr;

        auto new_size = (lst::num_t) (((double) list->size) * (1 + list->expn));

        if ((temp_data = (lst::data_t *) realloc (list->data, new_size * sizeof (lst::data_t))) == nullptr) {
            PRINT_ERROR ("Failed to recreate data array. Clear the list")
        } else if ((temp_next = (lst::num_t *) realloc (list->next, new_size * sizeof (lst::num_t))) == nullptr) {
            PRINT_ERROR ("Failed to recreate next array. Clear the list")
        } else if ((temp_prev = (lst::num_t *) realloc (list->prev, new_size * sizeof (lst::num_t))) == nullptr) {
            PRINT_ERROR ("Failed to recreate prev array. Clear the list")
        } else {    // Начальные значения переменных и массивов
            list->data = temp_data;
            list->next = temp_next;
            list->prev = temp_prev;

            for (lst::num_t i = list->size; i < new_size; i++) {
                list->data[i] = 0;
                list->next[i] = i + 1;
                list->prev[i] = 0;
            }

            list->size = new_size;

            if (lst::dmp_ON)
                Dump_list (list, list->fileDump, string);

            return lst::ALL_RIGHT;
        }
        return lst::REALLOC_ERR;
    }
    if (lst::dmp_ON)
        Dump_list (list, list->fileDump, string);

    return lst::ALL_RIGHT;
}

// Единственная функция, которая не поместилась в экран
void Dump_list (list_t *list, FILE *file, char *string) {
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
*/  // Сначала в файл

    __uint8_t length_number = 3;
    lst::num_t num = list->size;
    //fprintf (file, "%4c│", ' ');
    fprintf (file, "  N │");

    for (__uint16_t i = 0; i < num; i++)
        fprintf (file, "%3d |", i);

    fprintf (file, " list->head%5d", list->head);

    fprintf (file, "\n────┼");
    for (__uint16_t i = 0; i < num - 1; i++)
        fprintf (file, "────┼");
    fprintf (file, "────┤\n");

    fprintf (file, "data|");                            //data
    for (__uint16_t i = 0; i < num; i++)
        fprintf (file, "%4d|", list->data[i]);

    fprintf (file, " list->tail%5d", list->tail);
    fprintf (file, "\n────┼");
    for (__uint16_t i = 0; i < num - 1; i++)
        fprintf (file, "────┼");
    fprintf (file, "────┤\n");

    fprintf (file, "next|");                            // next
    for (__uint16_t i = 0; i < num; i++)
        fprintf (file, "%4d|", list->next[i]);

    fprintf (file, " list->free%5d", list->free);
    fprintf (file, "\n────┼");
    for (__uint16_t i = 0; i < num - 1; i++)
        fprintf (file, "────┼");
    fprintf (file, "────┤\n");

    fprintf (file, "prev|");                            //prev
    for (__uint16_t i = 0; i < num; i++)
        fprintf (file, "%4d|", list->prev[i]);

    fprintf (file, " list->count%4d", list->count);
    fprintf (file, "\n────┼");
    for (__uint16_t i = 0; i < num - 1; i++)
        fprintf (file, "────┴");
    fprintf (file, "────┘\n");

    fprintf (file, "\n");

    ////////////////////////////////////////////////    //DOT

    if (lst::dmp_DOT_ON)
        Dump_list_dot (list, string);

}

namespace Dldot {  // function only for Dump_list_dot
    inline void recreate_dot_dir () {
        system ("rm -rf dot_dir/ 2> /dev/null");
        system ("mkdir dot_dir");
        system ("touch dot_dir/README.txt; echo This folder constatly deleted! > dot_dir/README.txt");
    }

    inline bool fill_main_inf_in_file (list_t *list, char *string) {    // Заполняет dot основной информацией о list
        FILE *file_dot = fopen ("temp_file.dot", "w");
        if (file_dot == nullptr) {
            PRINT_ERROR ("DONT CREATE FILE_DOT")
            return false;
        }

        fprintf (file_dot, "digraph G {\n"
                           "    rankdir = LR;\n"
                           "    node[shape=ellipse, fontsize=50, color = red];\n"
                           "    edge[fontsize=50, color = blue, fillcolor = blue];\n");
        lst::num_t num = 0;
        lst::num_t size = list->size;

        for (num = 0; num < size; num++)                                        // Идентифицирую все элементы списка
            fprintf (file_dot, "\t%d [label = \"%d\"];\n", num, list->data[num]);

        fprintf (file_dot, "head -> %d[label = \"%d\"];\n",
                 list->head, list->head);                                       // Рисую граф из используемых ячеек
        for (num = list->next[list->head]; num != list->tail; num = list->next[num])
            fprintf (file_dot, "\t%d -> %d[label = \"%d\"]\n", list->prev[num], num, list->prev[num]);
        fprintf (file_dot, "\t%d -> %d[label = \"%d\"]\n", list->prev[num], num, list->prev[num]);
        fprintf (file_dot, "\t%d -> tail[label = \"%d\"]\n", num, list->tail);

        fprintf (file_dot, "free -> %d[label = \"%d\"];\n",
                 list->free, list->free);                                       // Рисую граф из неиспользуемых ячеек
        for (num = list->free; num < size - 1; num = list->next[num])
            fprintf (file_dot, "\t%d -> %d[label = \"%d\"]\n", num, list->next[num], list->next[num]);

        // Немного костыльно инициализирую ещё один элемент, который хранит кол-во занятых ячеек под номером size
        fprintf (file_dot, "\t%d [label = \"%d\"];\n", size, list->count);
        fprintf (file_dot, "count -> %d;\n", size);

        // Пишет команду, которая будет исполнена над этим списком
        fprintf (file_dot, "    node[shape = rectangle, fontsize=40, color = black];\n");
        fprintf (file_dot, "%d -> \"%s\"[shape = rectangle, style = invis];\n", size, string);
        fprintf (file_dot, "}");

        fclose (file_dot);
        return true;
    }

    inline void bonding_pictures (size_t number_calls) {    // Склеивает предыдущую и нынешнюю фотку
        char comand[1024] = "";
        sprintf (comand, "cd dot_dir;");
        // Получаем высоту и ширину предыдущей картинки
        sprintf (comand, R"(%s height=$(identify -format "%%h" "%lu.png") > /dev/null;)",
                 comand, number_calls - 1);
        sprintf (comand, R"(%s wight=$(identify -format "%%w" "%lu.png") > /dev/null;)",
                 comand, number_calls - 1);
        sprintf (comand, "%s wight=$(($wight+$wight/11));height=$(($height+$height/11));", comand);
        // Дорисовываем под предыдущеё картинкой чёрный прямоугольник такого же размера
        sprintf (comand, "%s ffmpeg -y -i %lu.png -i %lu.png"
                         " -filter_complex scale=$wight:$height,tile=1x2 %lu.png 2> /dev/null;",
                 comand, number_calls - 1, number_calls, number_calls - 1);
        // Вставляем в этот прямоугольник новую картинку
        sprintf (comand, "%sffmpeg -y -i %lu.png -i %lu.png -filter_complex overlay=0:$height %lu.png 2> /dev/null;",
                 comand, number_calls - 1, number_calls, number_calls - 1);
        sprintf (comand, "%s cd ..;", comand);
        printf ("\n%s\n", comand);
        system (comand);
    }
}

void Dump_list_dot (list_t *list, char *string) {
    static size_t number_calls = 0;
    static bool first = false;

    // Удаляю папку со старыми данными и создаю пустую новую для хранения результата (фото, видео, GIF)
    if (!first) {
        Dldot::recreate_dot_dir ();
        first = true;
        return;
    }

    if (!Dldot::fill_main_inf_in_file (list, string))
        return;

    char comand[256] = "";
    sprintf (comand, "dot -Tpng -Gsize=%zu,16\\! -Gdpi=100 temp_file.dot -o dot_dir/%zu.png",
             10 + number_calls, number_calls);
    if (lst::dmp_DOT_SHOW_AT_ONCE)
        sprintf (comand, "%s; fim -a dot_dir/%zu.png", comand, number_calls);
    //printf ("%s", comand);
    system (comand);

    // Склеиваем картинки и удаялем предыдущие
    if (number_calls > 1 && lst::dmp_stick)
        Dldot::bonding_pictures (number_calls);

    number_calls++;
}

void lst::create_Animation_dot () {
    system ("rm dot_dir/video.mp4 2> /dev/null; rm dot_dir/out.gif 2> /dev/null");
    system ("ffmpeg -r 1 -i dot_dir/%d.png dot_dir/video.mp4 2> /dev/null");
    system ("ffmpeg -i dot_dir/video.mp4 dot_dir/out.gif 2> /dev/null");
}

int Verificator_List (list_t *list) {
    lst::num_t 
    return 0;
}