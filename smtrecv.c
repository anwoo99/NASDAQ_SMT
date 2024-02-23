#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <dirent.h>
#include "nassmt.h"

#define MAX_STACK_FRAMES 128

typedef struct
{
    char errname[512];
    char reason[1024];
    char where[1024];
} ERROR_INFO;

FEP *fep;
static char *exchange_name;
static char *port_name;
static int is_analyze = 0;
static char NASDAQ_EMI_FILENAME[16][256];
static char DEFAULT_SMARTLOG_DIR[] = "/mdfsvc/fep/log/Smart/raw";

/* Unix Domain Socket */
int domain_socket = 0;
struct sockaddr_un target_addr;
char *domain_filename;

int start_analyze();
int start_receive();
void process_arguments(int argc, char **argv);
void signal_handler(int signo);
void print_trace(ERROR_INFO *error_info);
void stopit(int signo);
static void print_progress(long bytesRead, long totalFileSize, char *filename);
void usage(const char *program_name)
{
    printf("Usage:\n");
    printf("  %s [Options]\n", program_name);
    printf("  %s <exchange_name> <port_name>\n", program_name);
    printf("\nOptions:\n");
    printf("  --analyze <filename>      Perform analysis\n");
    printf("  -a <filename>             Perform analysis\n");
    printf("\n  --analyze all             Perform analysis on all files in %s\n", DEFAULT_SMARTLOG_DIR);
    printf("  -a all                    Perform analysis on all files in %s\n", DEFAULT_SMARTLOG_DIR);
    printf("\nArguments:\n");
    printf("  <exchange_name>  Name of the exchange\n");
    printf("  <port_name>      Port name for the exchange\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        usage(basename(argv[0]));
    }

    process_arguments(argc, argv);

    if ((fep = fep_open(exchange_name, MD_NORDWR)) == NULL)
    {
        if (isatty(1))
            fprintf(stderr, "Cannot open FEP interface for exchange '%s'\n", exchange_name);
        return (-1);
    }

    signal(SIGSEGV, signal_handler); // Segmentaion Fault 발생 시, 오류 발생 지점 logging
    signal(SIGINT, stopit);
    signal(SIGQUIT, stopit);
    signal(SIGTERM, stopit);

    if (is_analyze)
        start_analyze();
    else
        start_receive();

    return 0;
}

/*
 * Function: start_analyze()
 * -------------------------
 * 바이너리 형식의 Test File을 읽고, MoldUDP64 메세지의 Block 요소들만 추출한 후, TR_PAKCET 구조체 형식의 데이터를 Unix Domain Socket을 사용하여 'nasfep'으로 전송
 *
 * Returns:
 *    0은 성공, -1은 실패
 */
int start_analyze()
{
    int retv;
    int ii = 0;
    char filename[256];

    while (strlen(NASDAQ_EMI_FILENAME[ii]) != 0)
    {
        memset(filename, 0x00, sizeof(filename));
        sprintf(filename, "%s/%s", DEFAULT_SMARTLOG_DIR, NASDAQ_EMI_FILENAME[ii]);

        // Open the binary file for reading
        FILE *file = fopen(filename, "rb");
        if (file == NULL)
        {
            fep_log(fep, FL_ERROR, "Error opening file(%s)", filename);
            exit(EXIT_FAILURE);
        }

        // For tracking file reading progress
        long totalFileSize, bytesRead = 0;
        long curr_read_percent = 0, bef_read_percent = 0;

        // Obtain total file size
        fseek(file, 0, SEEK_END);
        totalFileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        // Initialize message buffer and TR_PACKET
        MSGBUFF msgbuff;
        DWN_PACKET dwn_packet;
        char send_b[TR_PACKET_LEN];
        TR_PACKET *tr_packet = (TR_PACKET *)send_b;

        initialize_msg_buff(&msgbuff);
        initialize_tr_packet(tr_packet);

        // Set up the domain socket
        if (socket_setting() < 0)
        {
            fep_log(fep, FL_ERROR, "Failed to set socket");
            exit(EXIT_FAILURE);
        }

        // Main loop to read and process the file
        while ((msgbuff.read_size = fread(&msgbuff.buffer[msgbuff.rest_size], 1, sizeof(msgbuff.buffer) - msgbuff.rest_size, file)) > 0)
        {
            msgbuff.rest_size += msgbuff.read_size;
            bytesRead += msgbuff.read_size;

            // Process MoldUDP64 message blocks
            while (1)
            {
                retv = parser_moldudp64_dwn_packet(&msgbuff, &dwn_packet, FLAG_MSG_BLOCK);

                if (retv < 0)
                {
                    fep_log(fep, FL_ERROR, "Error parsing MoldUDP64 message block.");
                    fclose(file);
                    return (-1);
                }
                else if (retv & MSG_BUFFER_SCARCED)
                {
                    break;
                }
                else
                {
                    // Allocate TR_PACKET and send to domain socket
                    allocate_tr_packet(tr_packet, dwn_packet.msg_block.data, dwn_packet.msg_block.msgl);

                    if (sendto(domain_socket, send_b, TR_PACKET_LEN, 0, (struct sockaddr *)&target_addr, sizeof(target_addr)) < 0)
                    {
                        // fep_log(fep, FL_ERROR, "sendto() to '%s' error(%d|%s)", domain_filename, errno, strerror(errno));
                        fep_sleep(3000000);
                        continue;
                    }
                }
            }

            print_progress(bytesRead, totalFileSize, NASDAQ_EMI_FILENAME[ii]);
        }

        // Close the file
        fclose(file);
        printf("\n\n Finished reading file '%s'.\n\n", NASDAQ_EMI_FILENAME[ii]);
        ii++;
    }
    return 0;
}

int start_receive()
{
    /**************************/
    /* 인프라 구축 완료 시 개발 */
    /**************************/

    return (0);
}

int socket_setting()
{
    domain_socket = socket(PF_FILE, SOCK_DGRAM, 0);

    if (domain_socket < 0)
    {
        fep_log(fep, FL_ERROR, "socket() error(%d.%s) !!! | file(%s)", errno, strerror(errno), domain_filename);
        return (-1);
    }
    fep_log(fep, FL_DEBUG, "socket() complete ..! | file(%s)", domain_filename);

    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sun_family = AF_UNIX;
    strcpy(target_addr.sun_path, domain_filename);

    return (0);
}

void get_all_files_in_dir(char *dirname)
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir(dirname);

    if (dir == NULL)
    {
        return;
    }

    int ii = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            strcpy(NASDAQ_EMI_FILENAME[ii], entry->d_name);
            ii++;
        }
    }

    closedir(dir);
}

void process_arguments(int argc, char **argv)
{
    int option;
    struct option long_options[] = {
        {"analyze", no_argument, 0, 'a'},
        {0, 0, 0, 0}};

    memset(NASDAQ_EMI_FILENAME, 0x00, sizeof(NASDAQ_EMI_FILENAME));

    // Process options
    while ((option = getopt_long(argc, argv, "a", long_options, NULL)) != -1)
    {
        switch (option)
        {
        case 'a':
            is_analyze = 1;
            exchange_name = "OSMT";
            domain_filename = "/mdfsvc/fep/que/SMTTEST";
            if (argc < 3)
            {
                usage(basename(argv[0]));
            }
            if (strcmp(argv[2], "all") == 0)
            {
                get_all_files_in_dir(DEFAULT_SMARTLOG_DIR);
            }
            else
            {
                strcpy(NASDAQ_EMI_FILENAME[0], argv[2]);
            }

            break;
        default:
            usage(basename(argv[0]));
        }
    }

    // Process the remaining arguments
    if (optind + 2 != argc && !is_analyze)
    {
        usage(basename(argv[0]));
    }

    // Extract and process the remaining arguments
    if (!is_analyze)
    {
        exchange_name = argv[optind];
        port_name = argv[optind + 1];
    }
}

void stopit(int signo)
{
    printf("\n");
    fep_close(fep);
    exit(0);
}

void print_trace(ERROR_INFO *error_info)
{
    void *array[MAX_STACK_FRAMES];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(array, MAX_STACK_FRAMES);
    strings = backtrace_symbols(array, size);

    int offset = 0;
    for (i = 0; i < size; i++)
    {
        int printed = snprintf(&error_info->where[offset], sizeof(error_info->where) - offset, "%s\n", strings[i]);
        if (printed > 0)
        {
            offset += printed;
        }
    }

    free(strings);
}

void signal_handler(int signo)
{
    ERROR_INFO error_info;

    memset(&error_info, 0x00, sizeof(ERROR_INFO));

    switch (signo)
    {
    case SIGSEGV:
        strncpy(error_info.errname, "Segmentation Fault", sizeof(error_info.errname) - 1);
        print_trace(&error_info);
        break;
    default:
        break;
    }

    fep_log(fep, FL_MUST,
            "\n############################################################\n"
            "1) Error: %s\n"
            "2) Where:\n%s"
            "############################################################",
            error_info.errname, error_info.where);

    stopit(signo);
}

static void format_size(long size, char *result, int result_size)
{
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double rest = (double)size;

    while (rest > 1024 && unit_index < sizeof(units) / sizeof(units[0]) - 1)
    {
        rest /= 1024;
        unit_index++;
    }

    snprintf(result, result_size, "%.1f %s", rest, units[unit_index]);
}

static void print_progress(long bytesRead, long totalFileSize, char *filename)
{
    char currentReadSize[20];
    char currentTotalSize[20];
    int i;

    // 프로그레스 바 길이 설정
    const int bar_length = 50;

    // 현재 진행률 계산
    int progress = (int)(bar_length * bytesRead / totalFileSize);

    format_size(bytesRead, currentReadSize, sizeof(currentReadSize));
    format_size(totalFileSize, currentTotalSize, sizeof(currentTotalSize));

    // 터미널에 프로그레스 바 출력
    printf("\r\033[K %s %3d%%[", filename, (int)(100 * bytesRead / totalFileSize));
    for (i = 0; i < bar_length; ++i)
    {
        if (i < progress)
        {
            printf("#");
        }
        else
        {
            printf("-");
        }
    }
    printf("] Read: %s | Total: %s\n", currentReadSize, currentTotalSize);

    // fflush(stdout);
    printf("\033[A"); // 첫 커서로 이동
}
