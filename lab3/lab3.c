#include <stdio.h>

struct inquere {
    char identificator[5];
    char path[200];
    char protocol[9];
};
struct time {
    unsigned short int day;
    char month[3];
    unsigned short int year;
    unsigned short int hour;
    unsigned short int minute;
    unsigned short int second;
    char timecode[5];
};
struct nasa_request {
    char remote_addr[50];
    struct time local_time;
    struct inquere request;
    unsigned short int status;
    char bytes_send[6];
};

void fscanf_querie(FILE *filename, struct nasa_request *inquiry) {
    fscanf(filename, "%50s - - [%hd/%3s/%hd:%hd:%2hd:%hd %5s] \"%5s %200s %8s\" %hu %5s", inquiry->remote_addr,
           &inquiry->local_time.day, inquiry->local_time.month, &inquiry->local_time.year,
           &inquiry->local_time.hour, &inquiry->local_time.minute, &inquiry->local_time.second,
           inquiry->local_time.timecode, inquiry->request.identificator, inquiry->request.path,
           inquiry->request.protocol, &inquiry->status, inquiry->bytes_send);
}

void fprintf_querie(FILE *filename, struct nasa_request *inquiry) {
    fprintf(filename, "%s - - [%02d/%s/%04d:%02d:%02d:%02d %s] \"%s %s %s\" %hu %s\n", inquiry->remote_addr,
            inquiry->local_time.day, inquiry->local_time.month, inquiry->local_time.year,
            inquiry->local_time.hour, inquiry->local_time.minute, inquiry->local_time.second,
            inquiry->local_time.timecode, inquiry->request.identificator, inquiry->request.path,
            inquiry->request.protocol, inquiry->status, inquiry->bytes_send);
}

int check_querie(struct nasa_request *inquiry, FILE *filename) {
    if (inquiry->status / 100 == 5) {
        fprintf_querie(filename, inquiry);
        return 1;
    }
    return 0;
}

unsigned short int minute_difference(struct time *self, struct time *other) {
    if ((self->day == other->day) && (self->hour == other->hour)) {
        return self->minute - other->minute;
    } else if (self->day == other->day) {
        return self->hour * 60 + self->minute -
               (other->hour * 60 + other->minute);
    } else {
        return self->day * 1440 + self->hour * 60 + self->minute -
               (self->day * 1440 + other->hour * 60 + other->minute);
    }
}

int main() {
    FILE *logsfile, *errorsfile;
    unsigned short int range_minutes, abs_minutes, ind = 0, i;
    printf("Введите длительность временного окна в минутах - ");
    scanf("%hu", &range_minutes);
    while ((!range_minutes) || (range_minutes > 44640)) {
        printf("Введите корректное значение - ");
        scanf("%hu", &range_minutes);
    }
    unsigned int bufer_queue[range_minutes];
    for (i = 0; i < range_minutes; i++) {
        bufer_queue[i] = 0;
    }


    unsigned int sum_of_requests = 0, maximum_sum = 0, amount_of_error_queries = 0;
    logsfile = fopen("NASA_access_log_Jul95", "r");
    errorsfile = fopen("NASA_access_log_Jul95 ERRORS", "w");
    if (logsfile == NULL) {
        printf("NASA file not found\n");
        return 0;
    }
    struct nasa_request querie;
    fscanf_querie(logsfile, &querie);
    sum_of_requests++;
    bufer_queue[0]++;
    struct time previous_time = querie.local_time, current_time, answer_time;
    amount_of_error_queries = (check_querie(&querie, errorsfile)) ?
                              amount_of_error_queries + 1 : amount_of_error_queries;
    printf("...обработка...\n");
    while (fgetc(logsfile) != EOF) {
        fscanf_querie(logsfile, &querie);
        sum_of_requests++;
        amount_of_error_queries = (check_querie(&querie, errorsfile)) ? amount_of_error_queries + 1
                                                                      : amount_of_error_queries;
        current_time = querie.local_time;
        abs_minutes = minute_difference(&current_time, &previous_time);
        if ((ind + abs_minutes) % range_minutes == (ind + abs_minutes)) {
            for (i = ind + 1; i <= ind + abs_minutes; i++) {
                sum_of_requests = sum_of_requests - bufer_queue[i];
                bufer_queue[i] = 0;
            }
        } else if ((ind + abs_minutes) / range_minutes == 1) {
            for (i = ind + 1; i < range_minutes; i++) {
                sum_of_requests = sum_of_requests - bufer_queue[i];
                bufer_queue[i] = 0;
            }
            for (i = 0; i <= (ind + abs_minutes) % range_minutes; i++) {
                sum_of_requests = sum_of_requests - bufer_queue[i];
                bufer_queue[i] = 0;
            }
        } else {
            for (i = 0; i < range_minutes; i++) {
                sum_of_requests = sum_of_requests - bufer_queue[i];
                bufer_queue[i] = 0;
            }
        }
        ind = (ind + abs_minutes) % range_minutes;
        bufer_queue[ind]++;
        if (sum_of_requests > maximum_sum) {
            maximum_sum = sum_of_requests;
            answer_time = current_time;
        }
        previous_time = current_time;
    }

    if (amount_of_error_queries) {
        printf("Неудачные запросы (%d) записаны в файл \"NASA_access_log_Jul95 ERRORS\"\n", amount_of_error_queries);
    } else {
        printf("Неудачных запросов не обнаружено\n");
    }
    unsigned short absolute_time_lower_bound =
            answer_time.day * 24 * 60 + answer_time.hour * 60 + answer_time.minute - range_minutes + 1;
    printf("Больше всего запросов (%d)\nбыло получено в промежуток ", maximum_sum);
    printf("[%02d/%s/%04d:%02dh:%02dm -",
           absolute_time_lower_bound / 1440, querie.local_time.month, querie.local_time.year,
           absolute_time_lower_bound % 1440 / 60, absolute_time_lower_bound % 60);
    printf(" %02d/%s/%04d:%02dh:%02dm]",
           answer_time.day, answer_time.month, answer_time.year, answer_time.hour, answer_time.minute);
    fclose(errorsfile);
    fclose(logsfile);
    return 0;

}