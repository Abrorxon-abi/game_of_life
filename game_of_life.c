#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>

#define STR 25
#define COL 80

int input_matrix(int matr[STR][COL], int *fla);
void update_matrix(int matr1[STR][COL], int matr2[STR][COL]);
int count_neighbors(int matr1[STR][COL], int i, int j);
int decision(int neighbors, int condition);
void replace(int matr2[STR][COL], int matr1[STR][COL]);
int check(int matr1[STR][COL], int matr2[STR][COL]);
int change_speed(char control_button, int *flag, int time_mili_sec);
int count(int matr[STR][COL]);
void number_to_xy(int n, int width, int *x, int *y);
int input_speed(int *delay);

void show_header() {
    printf("\033[0d\033[2J");
    printf("╔═══╗╔══╗╔╗──╔╗╔═══╗───╔══╗╔══╗───╔╗──╔══╗╔══╗╔═══╗\n");
    printf("║╔══╝║╔╗║║║──║║║╔══╝───║╔╗║║╔═╝───║║──╚╗╔╝║╔═╝║╔══╝\n");
    printf("║║╔═╗║╚╝║║╚╗╔╝║║╚══╗───║║║║║╚═╗───║║───║║─║╚═╗║╚══╗\n");
    printf("║║╚╗║║╔╗║║╔╗╔╗║║╔══╝───║║║║║╔═╝───║║───║║─║╔═╝║╔══╝\n");
    printf("║╚═╝║║║║║║║╚╝║║║╚══╗───║╚╝║║║─────║╚═╗╔╝╚╗║║──║╚══╗\n");
    printf("╚═══╝╚╝╚╝╚╝──╚╝╚═══╝───╚══╝╚╝─────╚══╝╚══╝╚╝──╚═══╝\n");
    printf("Powered by: @ilynpois & @ponosheb & @credibed\n\n");
}

int main() {
    int matr1[STR][COL];
    int matr2[STR][COL];

    int time_mili_sec = 500;
    int stop = 0;
    int fla = 0;
    show_header();
    if (input_matrix(matr1, &fla)) {
        return printf("Ошибка при вводе файла."), 1;
    }
    if (fla == 0) {
        if (input_speed(&time_mili_sec)) {
            return printf("Ошибка при вводе файла."), 1;
        }
    }
    if (freopen("/dev/tty", "r", stdin)) initscr();
    nodelay(stdscr, true);
    while (stop != 1) {
        char control_button = getch();
        if (count(matr1) == 0) {
            stop = 1;
        }
        time_mili_sec = change_speed(control_button, &stop, time_mili_sec);
        usleep(time_mili_sec * 1000);
        clear();
        update_matrix(matr1, matr2);
        if (check(matr1, matr2) == 2000) {
            stop = 1;
        }
        replace(matr2, matr1);
    }
    endwin();
    return 0;
}

int input_speed(int *delay) {
    printf("\nУкажите скорость обновления экрана (в ms):\n> ");
    if (scanf("%d", delay) < 1)
        return 1;
    else
        return 0;
}

void number_to_xy(int n, int width, int *x, int *y) {
    if (width < 0) return;
    *x = n % width;
    *y = n / width + 1;
}

int input_matrix(int matr[STR][COL], int *fla) {
    if (!isatty(fileno(stdin))) {
        *fla = 1;
        for (int i = 0; i < STR; i++) {
            for (int j = 0; j < COL; j++) {
                scanf("%d", &matr[i][j]);
            }
        }
    } else {
        *fla = 0;
        char filename[100];
        FILE *file;
        printf("Введите имя файла (100 символов):\n> ");
        scanf("%s", filename);
        file = fopen(filename, "r");
        if (file == NULL) return 1;

        char c;
        int cnt = 0;
        int x, y;
        while ((c = getc(file)) != EOF) {
            number_to_xy(cnt, COL, &x, &y);
            if (c == '0') {
                matr[y][x] = 0;
                cnt += 1;
            }
            if (c == '1') {
                matr[y][x] = 1;
                cnt += 1;
            }
        }
        fclose(file);
    }
    return 0;
}

void update_matrix(int matr1[STR][COL], int matr2[STR][COL]) {
    for (int i = 0; i < STR; i++) {
        for (int j = 0; j < COL; j++) {
            matr2[i][j] = decision(count_neighbors(matr1, i, j), matr1[i][j]);
            if (matr2[i][j] == 1)
                printw("0");
            else
                printw(".");
        }
        printw("\n");
    }
}

int count_neighbors(int matr1[STR][COL], int i, int j) {
    int sum = 0;

    int i_minus = i - 1, j_minus = j - 1, i_plus = i + 1, j_plus = j + 1;

    if (i_minus < 0) i_minus = STR - 1;
    if (j_minus < 0) j_minus = COL - 1;
    if (i_plus > STR - 1) i_plus = i_plus % STR;
    if (j_plus > COL - 1) j_plus = j_plus % COL;

    sum += matr1[i_minus][j_minus];

    sum += matr1[i_minus][j];
    sum += matr1[i_minus][j_plus];
    sum += matr1[i][j_plus];
    sum += matr1[i_plus][j_plus];
    sum += matr1[i_plus][j];
    sum += matr1[i_plus][j_minus];
    sum += matr1[i][j_minus];

    return sum;
}

int decision(int neighbors, int condition) {
    int next_gen = -1;
    if ((neighbors == 2 || neighbors == 3) && condition == 1) {
        next_gen = 1;
    } else if (neighbors == 3 && condition == 0) {
        next_gen = 1;
    } else {
        next_gen = 0;
    }
    return next_gen;
}

void replace(int matr2[STR][COL], int matr1[STR][COL]) {
    for (int i = 0; i < STR; i++) {
        for (int j = 0; j < COL; j++) {
            matr1[i][j] = matr2[i][j];
        }
    }
}

int check(int matr1[STR][COL], int matr2[STR][COL]) {
    int ans = 0;
    for (int i = 0; i < STR; i++) {
        for (int j = 0; j < COL; j++) {
            if (matr1[i][j] == matr2[i][j]) ans++;
        }
    }
    return ans;
}

int change_speed(char control_button, int *flag, int time_mili_sec) {
    if (control_button == '1')
        time_mili_sec = 900;
    else if (control_button == '2')
        time_mili_sec = 400;
    else if (control_button == '3')
        time_mili_sec = 70;
    else if (control_button == 'q')
        *flag = 1;

    return time_mili_sec;
}

int count(int matr[STR][COL]) {
    int sum = 0;
    for (int i = 0; i < STR; i++) {
        for (int j = 0; j < COL; j++) {
            sum += matr[i][j];
        }
    }
    return sum;
}
