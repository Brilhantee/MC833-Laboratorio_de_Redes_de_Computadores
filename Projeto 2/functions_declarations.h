#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_CLIENTS 5
#define MAX 128
// five profiles plus profiles_number
#define MAX_PAYLOAD_LENGTH 5264
// payload plus checksum plus order_number plus total length
#define MAX_BUFF_SIZE 5272
#define MAXSKILLS 256
#define PORT 8087
#define NULL_TERMINATOR 2
#define SA struct sockaddr
#define MESSAGE_SIZE_FIELD_LENGTH 4
#define CHECKSUM_FIELD_LENGTH 2
#define ORDER_NUMBER_FIELD_LENGTH 2

struct profile {
	char* email;
	char* name;
	char* last_name;
	char* residence;
	char* academic_education;
	char* graduation_year;
	char* skill;
};

// functions declarations from server.
void creating_a_file_to_store_profile_information(char* file_name);
void check_socket_creation();
void check_bind_connection();
void check_listen_connection();
void check_accept_connection();
int receiving_the_operation_chosen_by_the_user(int client_socket, char* file_name);
void operating(int socket, char* file_name);
int escrita_no_arquivo(char *arquivo, struct profile profile);
int busca_no_arquivo(char *arquivo, char *palavra);
struct profile* listagem_de_perfis_por_campo(char* arquivo, char* campo, int* user_count);
char* deserialization(int* count, char* receve_buffer);struct sockaddr_in address_creation();
int receiving_the_operation_chosen_by_the_user(int client_socket, char* file_name);
void to_register_a_new_user(char* file_name, char* receve_buffer);
void send_all_users_from_a_course_to_the_client(char* file_name, char* receve_buffer, int client_socket, struct sockaddr_in client_address);
char* serialization_of_user_list(struct profile* user_list, int user_count);
void serialization(int* count, char* message, char* buff);
void send_all_users_with_a_skill_to_the_client(char* file_name, char* receve_buffer, int client_socket, struct sockaddr_in client_address);
void send_all_users_with_the_same_graduation_date_to_the_client(char* file_name, char* receve_buffer, int client_socket, struct sockaddr_in client_address);
void send_all_users_informations_to_client(char* file_name, char* receve_buffer, int client_socket, struct sockaddr_in client_address);
struct profile* listar_perfis(char* arquivo, int* user_count);
void send_all_user_information(char* file_name, char* receve_buffer, int client_socket, struct sockaddr_in client_address);
struct profile* buscar_perfil(char* arquivo, char* email, int* user_found);
void send_user_to_client(int client_socket, struct profile* user, int user_found, struct sockaddr_in client_address);
void delete_a_user_from_dataset(char* file_name, char* receve_buffer, int client_socket, struct sockaddr_in client_address);
int remover_perfil(char* arquivo, char* email);
void perfis_fixos();
void sent_image_part1(int client_socket, struct sockaddr_in client_address);
void send_image_part2(int client_socket, struct sockaddr_in client_address, char* email);
void remove_extra_spaces(char* str);


// functions declarations from the client.
struct sockaddr_in address_creation();
void check_connection();
void choose_the_operations_that_will_be_done_on_the_server(int server_socket, struct sockaddr_in server_address);
void prints_the_operations_options_for_the_user();
char* get_operation_chosen_by_the_user();
char* get_input_from_user();
void register_new_profile_on_the_server(int server_socket, struct sockaddr_in server_address);
struct profile* get_new_profile_from_the_user();
char* get_skills_from_user_();
char* serialization_of_a_profile(struct profile* user_form, int* message_size);
void serialization_operation(int* message_size, int operation, char* buff);
void serialization_information(int* count, char* message, char* buff);
void deallocate_memory_from_profiles(struct profile* new_profile, int amount_of_profiles);
void get_all_profile_with_the_same_academic_education(int server_socket, struct sockaddr_in server_address);
void send_filter_operation_to_server(char* filter, int server_socket, int operation, struct sockaddr_in server_address);
void print_names_and_email_from_profile_list_to_user(struct profile* profile_list, int number_of_profiles);
struct profile* get_profiles_from_server(int server_socket, int* number_of_prefiles);
void get_all_profiles_with_a_skill(int server_socket, struct sockaddr_in server_address);
void get_all_profiles_with_same_graduation_year(int server_socket, struct sockaddr_in server_address);
void print_names_email_and_academic_formation_from_profile_list_to_user(struct profile* profile_list, int number_of_profiles);
char* get_the_date_from_user();
void get_all_data_from_server(int server_socket,  struct sockaddr_in server_address);
struct profile* get_all_users_from_server(int server_socket, int* number_of_users, struct sockaddr_in server_address);
void get_all_user_information(int server_socket, struct sockaddr_in server_address);
struct profile* get_user_from_server(int server_socket, int* user_found);
void delete_a_user_from_server_dataset(int server_socket, struct sockaddr_in server_address);
char* get_email_from_user();
void end_the_connection_with_the_server(int server_socket, struct sockaddr_in server_address);
void request_image(int server_socket, struct sockaddr_in server_address);
void open_image(const char* filename);


// fuctions declarations from both files.
unsigned short calculate_checksum(const char* data, int length);
int send_udp_message(int socket, const struct sockaddr_in server_address, char* payload, int payload_length, uint16_t* order_number);
char* receve_udp_message(int socket_udp, struct sockaddr_in* client_address, uint16_t* order_number);
void send_udp_ack(int sock_udp, struct sockaddr_in client_address, char* message, int message_length);
int receive_udp_ack(int socket, struct sockaddr_in* client_address, char* last_sent_message, int ACK_SIZE);
char* message_deserialization(int* bytes_read, char* buffer);
int number_deserialization(int* bytes_read, char* buffer);
void message_serialization(int* bytes_read, char* message, char* buffer);
void number_serialization(int* bytes_read, char* buffer, int number);
char* retira(char* phare);

int send_udp_message(int socket, const struct sockaddr_in server_address, char* payload, int payload_length, uint16_t* order_number) {

    if (payload_length > MAX_PAYLOAD_LENGTH) {
        printf("**  Erro: tamanho da carga útil excede o máximo permitido.  **\n");
        return -1;
    }

    size_t total_length = 4 + 2 + 2 + payload_length;

    char* message = (char*)malloc(total_length);
    if (message == NULL) {
        printf("\n** Erro: falha ao alocar memória para a mensagem.  **\n");
        return -1;
    }

	memcpy(message, &total_length, 4);

    uint16_t checksum = calculate_checksum(payload, payload_length);

	memcpy(message + 4, &checksum, 2);

    (*order_number) = (*order_number) + 1;
    
	memcpy(message + 4 + 2, order_number, 2);

    memcpy(message + 8, payload, payload_length);

    if (sendto(socket, message, total_length, 0, (const struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
        printf("\n**  Erro: falha ao enviar a mensagem.  **\n");
        free(message);
        return -1;
    }

    struct sockaddr_in client_address;
    receive_udp_ack(socket, &client_address, message, total_length);
    free(message);
    return 0;
}

char* receve_udp_message(int socket_udp, struct sockaddr_in* client_address, uint16_t* last_order_number) {
    char buffer[MAX_BUFF_SIZE];
    socklen_t client_address_len = sizeof(*client_address);

    int recv_len = recvfrom(socket_udp, buffer, sizeof(buffer), 0, (struct sockaddr*) client_address, &client_address_len);
    if (recv_len == -1) {
        perror("Erro ao receber mensagem");
        return NULL;
    }

    if (recv_len < (MESSAGE_SIZE_FIELD_LENGTH + CHECKSUM_FIELD_LENGTH + ORDER_NUMBER_FIELD_LENGTH)) {
        fprintf(stderr, "Mensagem inválida\n");
        return NULL;
    }

    int total_message_length;
	memcpy(&total_message_length, buffer, 4);

    if (total_message_length != recv_len) {
        fprintf(stderr, "Tamanho da mensagem incorreto\n");
        return NULL;
    }

    unsigned short* checksum_ptr = (unsigned short*)(buffer + MESSAGE_SIZE_FIELD_LENGTH);
    unsigned short checksum = *checksum_ptr;

    unsigned short calculated_checksum = calculate_checksum(buffer + (MESSAGE_SIZE_FIELD_LENGTH + CHECKSUM_FIELD_LENGTH + ORDER_NUMBER_FIELD_LENGTH), recv_len - (MESSAGE_SIZE_FIELD_LENGTH + CHECKSUM_FIELD_LENGTH + ORDER_NUMBER_FIELD_LENGTH));
    if (calculated_checksum != checksum) {
        fprintf(stderr, "Checksum inválido\n");
        return NULL;
    }

    uint16_t order_number = *(uint16_t*)(buffer + MESSAGE_SIZE_FIELD_LENGTH + CHECKSUM_FIELD_LENGTH);


    if (order_number <= *last_order_number) {
        fprintf(stderr, "Mensagem duplicada\n");
        return NULL;
    }
    *last_order_number = order_number;

    send_udp_ack(socket_udp, *client_address, buffer, recv_len);
    int payload_length = total_message_length - (MESSAGE_SIZE_FIELD_LENGTH + CHECKSUM_FIELD_LENGTH + ORDER_NUMBER_FIELD_LENGTH);
    char* payload = (char*)malloc(payload_length + 1);
    if (payload == NULL) {
        fprintf(stderr, "Erro ao alocar memória\n");
        return NULL;
    }

    memcpy(payload, buffer + (MESSAGE_SIZE_FIELD_LENGTH + CHECKSUM_FIELD_LENGTH + ORDER_NUMBER_FIELD_LENGTH), payload_length);
    payload[payload_length] = '\0';

    return payload;
}

unsigned short calculate_checksum(const char* data, int length) {
    unsigned long sum = 0;
    const unsigned short* data_ptr = (unsigned short*)data;

    while (length > 1) {
        sum += *data_ptr++;
        length -= 2;
    }

    if (length > 0) {
        sum += *((unsigned char*)data_ptr);
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}

void send_udp_ack(int sock_udp, struct sockaddr_in client_address, char* message, int message_length) {

    ssize_t sent_bytes = sendto(sock_udp, message, message_length, 0, (struct sockaddr*) &client_address, sizeof(client_address));
    if (sent_bytes == -1) {
        perror("\n** Erro ao enviar ACK** \n");
    }
}

int receive_udp_ack(int socket, struct sockaddr_in* client_address, char* last_sent_message, int ACK_SIZE) {
    char ack_buffer[ACK_SIZE];
    socklen_t client_address_len = sizeof(*client_address);

    int recv_len = recvfrom(socket, ack_buffer, ACK_SIZE, 0, (struct sockaddr*) client_address, &client_address_len);
    if (recv_len == -1) {
        perror("\n**  Erro ao receber ACK  **");
        return -1;
    }

    if (recv_len != ACK_SIZE) {
        fprintf(stderr, "\n**  ACK inválido  **\n");
        return -1;
    }

    if (memcmp(ack_buffer, last_sent_message, ACK_SIZE) != 0) {
        fprintf(stderr, "\n**  ACK incorreto  **\n");
        return -1;
    }
    return 0;
}

char* message_deserialization(int* bytes_read, char* buffer){
	int message_length;

	memcpy(&message_length, buffer + *bytes_read, sizeof(int));
	*bytes_read += sizeof(int);

	char* message = malloc(message_length + 1);
    if (message == NULL) {
        printf("\nErro: falha na alocação de memória.\n");
        exit(0);
    }

	memcpy(message, buffer + *bytes_read, message_length);
	*bytes_read += message_length;

    message[message_length] = '\0';
	
	return message;
}

int number_deserialization(int* bytes_read, char* buffer){
	int number;

	memcpy(&number, buffer + *bytes_read, sizeof(int));
	*bytes_read += sizeof(int);
	
	return number;
}

void message_serialization(int* bytes_read, char* message, char* buffer){
	int message_length = strlen(message);

	memcpy(buffer + *bytes_read, &message_length, sizeof(int));
	*bytes_read += sizeof(int);

	memcpy(buffer + *bytes_read, message, message_length);
	*bytes_read += message_length;
}

void number_serialization(int* bytes_read, char* buffer, int number){

	memcpy(buffer+ *bytes_read, &number, sizeof(int));
	*bytes_read += sizeof(int);
}

char* remove_newline(char* phrase){
    size_t newlinePos = strcspn(phrase, "\n");
    phrase[newlinePos] = '\0';

    return phrase;
}