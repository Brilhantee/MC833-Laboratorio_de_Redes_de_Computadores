#include "functions_declarations.h"

struct status_of_messages {

	uint16_t messages_from_server;
	uint16_t messages_send_to_server;
};

struct status_of_messages order_of_messages;

int main()
{
	int server_socket, connection;
	struct sockaddr_in server_address;

	server_socket = socket(AF_INET, SOCK_DGRAM, 0);
	check_socket_creation(server_socket);

	server_address = address_creation(server_address);

	connection = connect(server_socket, (SA*)&server_address, sizeof(server_address));
	check_connection(connection);

	choose_the_operations_that_will_be_done_on_the_server(server_socket, server_address);

	close(server_socket);
}

void check_socket_creation(int socket){
	if (socket == -1) {
		printf("## Falha na criação do Socket...\n");
		exit(0);
	}
	else
		printf("## Socket criado com sucesso...\n");
}

struct sockaddr_in address_creation(struct sockaddr_in server_address){
	bzero(&server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT);
	return server_address;
}

void check_connection(int connection){
	if (connection != 0) {
		printf("## Conexão com o servidor falhou...\n");
		exit(0);
	}
	else
		printf("## Conectado com o servidor..\n");
}

void choose_the_operations_that_will_be_done_on_the_server(int server_socket, struct sockaddr_in server_address){
	char* operation_chosen_by_the_user;
	struct profile client;

	printf("\n## Bem-vindo!\n");
	printf("\n## As operações que podem ser realizadas com o servidor são exibidas abaixo: \n");
	
	for(;;) {
		order_of_messages.messages_from_server = 0;
		order_of_messages.messages_send_to_server = 0;

		prints_the_operations_options_for_the_user();

		operation_chosen_by_the_user = get_operation_chosen_by_the_user();

		if ((strcmp(operation_chosen_by_the_user, "1")) == 0)
			register_new_profile_on_the_server(server_socket, server_address);

		else if ((strcmp(operation_chosen_by_the_user, "2")) == 0)
			get_all_profile_with_the_same_academic_education(server_socket, server_address);

		else if ((strcmp(operation_chosen_by_the_user, "3")) == 0)
			get_all_profiles_with_a_skill(server_socket, server_address);

		else if ((strcmp(operation_chosen_by_the_user, "4")) == 0)
			get_all_profiles_with_same_graduation_year(server_socket, server_address);

		else if ((strcmp(operation_chosen_by_the_user, "5")) == 0)
			get_all_data_from_server(server_socket, server_address);

		else if ((strcmp(operation_chosen_by_the_user, "6")) == 0)
			get_all_user_information(server_socket, server_address);

		else if ((strcmp(operation_chosen_by_the_user, "7")) == 0)
			delete_a_user_from_server_dataset(server_socket, server_address);
			
		else if ((strcmp(operation_chosen_by_the_user, "8")) == 0)
			break;

		else if ((strcmp(operation_chosen_by_the_user, "9")) == 0){
			end_the_connection_with_the_server(server_socket, server_address);
			break;
		}
		else if((strcmp(operation_chosen_by_the_user, "10")) == 0){
			request_image(server_socket, server_address);
		}
		else
			printf("\nNão foi possivel entender sua resposta.\nPor favor, escolha uma opção novamente\n");

		free(operation_chosen_by_the_user);


	}
}

void prints_the_operations_options_for_the_user(){

		printf("\nOperação 1: Cadastrar novo perfil.\n");
		printf("Operação 2: Filtrar usuários por curso.\n");
		printf("Operação 3: Filtrar usuários por habilidade.\n");
		printf("Operação 4: Filtrar usuários por ano de formatura.\n");
		printf("Operação 5: Listar todos os perfis.\n");
		printf("Operação 6: Acessar as informações de um perfil.\n");
		printf("Operação 7: Remover um perfil.\n");
		printf("Operação 8: Encerrar programa.\n");
		printf("Operação 9: Encerrar servidor e o programa.\n");
		printf("Operação 10: Realizar o download de uma imagem de um perfil.\n");
}

char* get_operation_chosen_by_the_user(){
	char* operation_chosen_by_the_user;

	printf("\n## Escolha qualquer operação que deseja fazer.\n## Para isso, digite o número da respectiva operação:\n");
	operation_chosen_by_the_user = get_input_from_user();

	return operation_chosen_by_the_user;
}

char* get_input_from_user(){
	char* phrase = NULL;
    size_t size = 0;
    getline(&phrase, &size, stdin);

	remove_newline(phrase);
	return phrase;
}

void register_new_profile_on_the_server(int server_socket,  struct sockaddr_in server_address){
	struct profile* new_profile;
	char* payload;
	int amount_of_profiles, payload_length = 0;
	
	bzero(&new_profile, sizeof(new_profile));

	new_profile = get_new_profile_from_the_user();
	
	payload = serialization_of_a_profile(new_profile, &payload_length);

	send_udp_message(server_socket, server_address, payload, payload_length, &(order_of_messages.messages_send_to_server));

	amount_of_profiles = 1;
	deallocate_memory_from_profiles(new_profile, amount_of_profiles);

	printf("\n## Novo usuário cadrastrado ##\n");
	printf("\n## Por favor, selecione a próxima operação: ##\n");
}

struct profile* get_new_profile_from_the_user(){
	struct profile* new_profile = malloc(sizeof(struct profile));

	printf("\nPor favor, insira o e-mail para o cadastro:\n");
	new_profile->email = get_input_from_user();

	printf("\nInforme o nome do novo perfil:\n");
	new_profile->name = get_input_from_user();

	printf("\nInforme o sobrenome do novo perfil:\n");
	new_profile->last_name = get_input_from_user();

	printf("\nInforme a residência do novo perfil:\n");
	new_profile->residence = get_input_from_user();

	printf("\nInforme a formação acadêmica do novo perfil:\n");
	new_profile->academic_education = get_input_from_user();

	printf("\nInforme o ano de formatura do novo perfil:\n");
	new_profile->graduation_year = get_input_from_user();

	printf("\nInforme as habilidades do novo perfil:\n");
	new_profile->skill = get_input_from_user();

	return new_profile;
}

char* serialization_of_a_profile(struct profile* profile, int* profile_length){
	int operation;
	char* serialized_profile = malloc(MAX_BUFF_SIZE);

	operation = 1;
	number_serialization(profile_length, serialized_profile, operation);

	message_serialization(profile_length, profile->email, serialized_profile);
	message_serialization(profile_length, profile->name, serialized_profile);
	message_serialization(profile_length, profile->last_name, serialized_profile);
	message_serialization(profile_length, profile->residence, serialized_profile);
	message_serialization(profile_length, profile->academic_education, serialized_profile);
	message_serialization(profile_length, profile->graduation_year, serialized_profile);
	message_serialization(profile_length, profile->skill, serialized_profile);
	
	return serialized_profile;
}

void deallocate_memory_from_profiles(struct profile* new_profile, int amount_of_profiles){
	for(int i = 0; i < amount_of_profiles; i ++){
		free(new_profile[i].email);
		free(new_profile[i].name);
		free(new_profile[i].last_name);
		free(new_profile[i].residence);
		free(new_profile[i].academic_education);
		free(new_profile[i].graduation_year);
		free(new_profile[i].skill);
	}
	free(new_profile);
}

void get_all_profile_with_the_same_academic_education(int server_socket, struct sockaddr_in server_address){
	char* filter;
	struct profile* profiles_with_the_same_academic_education;
	int number_of_profiles;

	printf("\nInforme a formação acadêmica pela qual você quer filtrar os perfis:\n");
	filter = get_input_from_user();

	send_filter_operation_to_server(filter, server_socket, 2, server_address);

	profiles_with_the_same_academic_education = get_profiles_from_server(server_socket, &number_of_profiles);
 	
	print_names_and_email_from_profile_list_to_user(profiles_with_the_same_academic_education, number_of_profiles);
}

void send_filter_operation_to_server(char* filter, int server_socket, int operation, struct sockaddr_in server_address){
	char message[MAX_BUFF_SIZE];
	bzero(&message, sizeof(message));
	int message_size = 0;

	number_serialization(&message_size, message, operation);

	message_serialization(&message_size, filter, message);

	send_udp_message(server_socket, server_address, message, message_size, &order_of_messages.messages_send_to_server);
}

struct profile* get_profiles_from_server(int server_socket, int* number_of_profiles){
	char buffer[MAX_BUFF_SIZE];
	char receve_buffer[MAX_BUFF_SIZE];
	char* message;
	int ciclos, remainder, count, bytes_read;
	struct sockaddr_in server_address;
	socklen_t len = sizeof(server_address);

	bytes_read = recvfrom(server_socket, (char *)buffer, MAX_BUFF_SIZE, MSG_WAITALL, ( struct sockaddr *) &server_address, &len);
	memcpy(receve_buffer, buffer, MAX_BUFF_SIZE);

	count = 0;
	memcpy(number_of_profiles, receve_buffer, sizeof(int));
	count += sizeof(int);

	ciclos = *number_of_profiles / 5;
	remainder = *number_of_profiles % 5;

	struct profile* profile_list = malloc(sizeof(struct profile) * (*number_of_profiles));

	for (int i = 0; i < ciclos; i ++){
		for(int j = i * 5; j < (i + 1) * 5; j ++){

			message = message_deserialization(&count, receve_buffer);
			profile_list[j].email = message;

			message = message_deserialization(&count, receve_buffer);
			profile_list[j].name = message;

			message = message_deserialization(&count, receve_buffer);
			profile_list[j].last_name = message;

			message = message_deserialization(&count, receve_buffer);
			profile_list[j].residence = message;

			message = message_deserialization(&count, receve_buffer);
			profile_list[j].academic_education = message;

			message = message_deserialization(&count, receve_buffer);
			profile_list[j].graduation_year = message;

			message = message_deserialization(&count, receve_buffer);
			profile_list[j].skill = message;
		}
		bytes_read = recvfrom(server_socket, (char *)buffer, MAX_BUFF_SIZE, MSG_WAITALL, ( struct sockaddr *) &server_address, &len);
		memcpy(receve_buffer, buffer, MAX_BUFF_SIZE);
		count = 0;
	}

	for(int i = ciclos * 5; i < *number_of_profiles; i ++){

		message = message_deserialization(&count, receve_buffer);
		profile_list[i].email = message;

		message = message_deserialization(&count, receve_buffer);
		profile_list[i].name = message;

		message = message_deserialization(&count, receve_buffer);
		profile_list[i].last_name = message;

		message = message_deserialization(&count, receve_buffer);
		profile_list[i].residence = message;

		message = message_deserialization(&count, receve_buffer);
		profile_list[i].academic_education = message;

		message = message_deserialization(&count, receve_buffer);
		profile_list[i].graduation_year = message;

		message = message_deserialization(&count, receve_buffer);
		profile_list[i].skill = message;
	}

	return profile_list;
}

void print_names_and_email_from_profile_list_to_user(struct profile* profile_list, int number_of_profiles){
	if (number_of_profiles == 0)
		printf("\n## Não há usuários registrados neste curso  ##\n");
	else{
		printf("\n##  Lista de usuários cadastrados neste curso.  ##\n");
		for (int i = 0; i < number_of_profiles; i ++){
			printf("\n## Usuário: %d\n", i + 1);
			printf("Nome: %s\n", profile_list[i].name);
			printf("E-mail: %s\n", profile_list[i].email);
		}
		printf("\n## Todos os usuários foram listados. ##\n");
	}
	printf("\n## Por favor, selecione a próxima operação: ##\n");
}

void get_all_profiles_with_a_skill(int server_socket, struct sockaddr_in server_address){
	char* filter;
	struct profile* profiles_with_the_skill;
	int number_of_profiles;

	printf("\nInforme a Habilidade pela qual você quer filtrar os perfis:\n");
	filter = get_input_from_user();

	send_filter_operation_to_server(filter, server_socket, 3, server_address);

	profiles_with_the_skill = get_profiles_from_server(server_socket, &number_of_profiles);
 	
	print_names_and_email_from_profile_list_to_user(profiles_with_the_skill, number_of_profiles);
}


void get_all_profiles_with_same_graduation_year(int server_socket, struct sockaddr_in server_address){
	char* filter;
	struct profile* profiles_with_same_graduation_year;
	int number_of_profiles;

	printf("\nInforme o ano de formatura pela qual você quer filtrar os perfis:\n");
	filter = get_input_from_user();

	send_filter_operation_to_server(filter, server_socket, 4, server_address);

	profiles_with_same_graduation_year = get_profiles_from_server(server_socket, &number_of_profiles);
 	
	print_names_email_and_academic_formation_from_profile_list_to_user(profiles_with_same_graduation_year, number_of_profiles);
}

void print_names_email_and_academic_formation_from_profile_list_to_user(struct profile* profile_list, int number_of_profiles){
	if (number_of_profiles == 0)
		printf("\n## Não há usuários registrados neste curso  ##\n");
	else{
		printf("\n##  Lista de usuários cadastrados neste curso.  ##\n");
		for (int i = 0; i < number_of_profiles; i ++){
			printf("\n## Usuário: %d\n", i + 1);
			printf("Nome: %s\n", profile_list[i].name);
			printf("E-mail: %s\n", profile_list[i].email);
			printf("Formação Acadêmica: %s\n", profile_list[i].academic_education);
		}
		printf("\n## Todos os usuários foram listados. ##\n");
	}
	printf("\n## Por favor, selecione a próxima operação: ##\n");
}

void get_all_data_from_server(int server_socket,  struct sockaddr_in server_address){
    struct profile* all_cliente;
	int number_of_users, operation = 5;
	char message[MAX_BUFF_SIZE];
	int message_size = 0;

	memcpy(message, &operation, sizeof(int));

	send_udp_message(server_socket, server_address, message, sizeof(int), &(order_of_messages.messages_send_to_server));

	all_cliente = get_all_users_from_server(server_socket, &number_of_users, server_address);
 	
	if (number_of_users == 0)
		printf("\n## Não há usuários registrados ##\n");
	else{
		printf("\n##  Lista de usuários  ##\n");
		for (int i = 0; i < number_of_users; i ++){
			printf("\n## Usuário: %d\n", i + 1);
			printf("E-mail: %s\n", all_cliente[i].email);
			printf("Nome: %s\n", all_cliente[i].name);
			printf("Sobrenome: %s\n", all_cliente[i].last_name);
			printf("Residência: %s\n", all_cliente[i].residence);
			printf("Formação Acadêmica: %s\n", all_cliente[i].academic_education);
			printf("Ano de formatura:: %s\n", all_cliente[i].graduation_year);
			printf("Habilidades: %s\n", all_cliente[i].skill);
		}
		printf("\n## Todos os usuários foram listados. ##\n");
	}
	printf("\n## Por favor, selecione a próxima operação: ##\n");
}

struct profile* get_all_users_from_server(int server_socket, int* profiles_number, struct sockaddr_in server_address){
	char* payload, *message;
	int ciclos, remainder, bytes_read, bytes_receve_from_server;
	socklen_t len = sizeof(server_address);
	
	payload = receve_udp_message(server_socket, &server_address,  &(order_of_messages.messages_from_server));

	memcpy(profiles_number, payload, sizeof(int));


	ciclos = *profiles_number / 5;
	remainder = *profiles_number % 5;

	struct profile* list_of_user_from_the_course = malloc(sizeof(struct profile) * (*profiles_number));

	for (int i = 0; i < ciclos; i ++){

		payload = receve_udp_message(server_socket, &server_address,  &(order_of_messages.messages_from_server));
		bytes_read = 0;

		for(int j = i * 5; j < (i + 1) * 5; j ++){

			message = message_deserialization(&bytes_read, payload);
			list_of_user_from_the_course[j].email = message;

			message = message_deserialization(&bytes_read, payload);
			list_of_user_from_the_course[j].name = message;

			message = message_deserialization(&bytes_read, payload);
			list_of_user_from_the_course[j].last_name = message;

			message = message_deserialization(&bytes_read, payload);
			list_of_user_from_the_course[j].residence = message;

			message = message_deserialization(&bytes_read, payload);
			list_of_user_from_the_course[j].academic_education = message;

			message = message_deserialization(&bytes_read, payload);
			list_of_user_from_the_course[j].graduation_year = message;

			message = message_deserialization(&bytes_read, payload);
			list_of_user_from_the_course[j].skill = message;
		}
	}
	if (remainder > 0){

		payload = receve_udp_message(server_socket, &server_address,  &(order_of_messages.messages_from_server));
		bytes_read = 0;

		for(int i = ciclos * 5; i < *profiles_number; i ++){

			message = message_deserialization(&bytes_read, payload);
			list_of_user_from_the_course[i].email = message;

			message = message_deserialization(&bytes_read, payload);
			list_of_user_from_the_course[i].name = message;

			message = message_deserialization(&bytes_read, payload);
			list_of_user_from_the_course[i].last_name = message;

			message = message_deserialization(&bytes_read, payload);
			list_of_user_from_the_course[i].residence = message;

			message = message_deserialization(&bytes_read, payload);
			list_of_user_from_the_course[i].academic_education = message;

			message = message_deserialization(&bytes_read, payload);
			list_of_user_from_the_course[i].graduation_year = message;

			message = message_deserialization(&bytes_read, payload);
			list_of_user_from_the_course[i].skill = message;
		}
	}

	return list_of_user_from_the_course;
}

void get_all_user_information(int server_socket, struct sockaddr_in server_address){
	char* email;
	struct profile* user_informations;
	int user_found;

	printf("\nInsira o e-mail do perfil que você está procurando:\n");
	email = get_input_from_user();

	send_filter_operation_to_server(email, server_socket, 6, server_address);

	user_informations = get_user_from_server(server_socket, &user_found);

	if(user_found == 1){
		printf("\n## Usuário encontrado  ##\n");
		printf("E-mail: %s\n", user_informations->email);
		printf("Nome: %s\n", user_informations->name);
		printf("Sobrenome: %s\n", user_informations->last_name);
		printf("Residência: %s\n", user_informations->residence);
		printf("Formação Acadêmica: %s\n", user_informations->academic_education);
		printf("Ano de formatura: %s\n", user_informations->graduation_year);
		printf("Habilidades: %s\n", user_informations->skill);
	}
	else
		printf("\n## Usuário não encontrado  ##\n");
	printf("\n## Por favor, selecione a próxima operação: ##\n");
}

struct profile* get_user_from_server(int server_socket, int* user_found){
	char buffer[MAX_BUFF_SIZE]; // Precisa ver quantos clientes enviar por transmissão
	char receve_buffer[MAX_BUFF_SIZE];
	struct profile* user_information = malloc(sizeof(struct profile));
	char* message;
	int count = 0, bytes_read;
	struct sockaddr_in server_address;
	socklen_t len = sizeof(server_address);

	bytes_read = recvfrom(server_socket, (char *)buffer, MAX_BUFF_SIZE, MSG_WAITALL, ( struct sockaddr *) &server_address, &len);

	memcpy(receve_buffer, buffer, sizeof(buffer));
	memcpy(user_found, receve_buffer, sizeof(int));
	count += sizeof(int);

	if (*user_found == 1){

		message = message_deserialization(&count, receve_buffer);
		user_information->email = message;

		message = message_deserialization(&count, receve_buffer);
		user_information->name = message;

		message = message_deserialization(&count, receve_buffer);
		user_information->last_name = message;

		message = message_deserialization(&count, receve_buffer);
		user_information->residence = message;

		message = message_deserialization(&count, receve_buffer);
		user_information->academic_education = message;

		message = message_deserialization(&count, receve_buffer);
		user_information->graduation_year = message;

		message = message_deserialization(&count, receve_buffer);
		user_information->skill = message;	
	}

	return user_information;
}

void delete_a_user_from_server_dataset(int server_socket, struct sockaddr_in server_address){
	char* email;
	int user_removed, bytes_read;
	char* buffer = malloc(MAX_BUFF_SIZE);
	socklen_t len = sizeof(server_address);
	

	printf("\nDigite o e-mail do perfil que você deseja excluir:\n");
	email = get_input_from_user();
	printf("\nE-mail: %s", email);

	send_filter_operation_to_server(email, server_socket, 7, server_address);

	bytes_read = recvfrom(server_socket, (char *)buffer, MAX_BUFF_SIZE, MSG_WAITALL, ( struct sockaddr *) &server_address, &len);

	memcpy(&user_removed, buffer, 4);
 	
	if (user_removed == 0)
		printf("\n## Não há um usuário registrado com esse e-mail  ##\n");
	else{
		printf("\n##  Usuário removido  ##\n");
	}
	printf("\n## Por favor, selecione a próxima operação: ##\n");
	free(buffer);
	free(email);
}

void end_the_connection_with_the_server(int server_socket, struct sockaddr_in server_address){
	char* message = malloc(sizeof(int));
	int close_connection_operation = 8;
	socklen_t len = sizeof(server_address);

	memcpy(message, &close_connection_operation, sizeof(int));

	send_udp_message(server_socket, server_address, message, sizeof(int), &order_of_messages.messages_send_to_server);
}

void open_image(const char* filename) {
    char comando[256];
    snprintf(comando, sizeof(comando), "xdg-open %s", filename);
    system(comando);
}

void request_image(int server_socket, struct sockaddr_in server_address){
	
	// Solicita o e-mail do perfil para download da imagem
    char* email;
	printf("Digite o e-mail do perfil para download da imagem: \n");
	email = get_input_from_user();
    email[strcspn(email, "\n")] = '\0';

	send_filter_operation_to_server(email, server_socket, 10, server_address);
	
    // Envia o e-mail do perfil para o servidor
    ssize_t bytes_sent = sendto(server_socket, email, strlen(email), 0, (struct sockaddr*)&server_address, sizeof(server_address));
    if (bytes_sent == -1) {
        perror("Erro ao enviar dados. \n");
        exit(1);
    }
	
    // Recebe o tamanho do arquivo do servidor

    long file_size;
    socklen_t server_address_length = sizeof(server_address);
    ssize_t bytes_received = recvfrom(server_socket, &file_size, sizeof(file_size), 0, (struct sockaddr*)&server_address, &server_address_length);
    if (bytes_received == -1) {
        perror("Erro ao receber dados. \n");
        exit(1);
    }
	
    if (file_size < 0) {
        printf("Imagem não encontrada para o e-mail informado. \n");
    }
	else{
    	printf("Tamanho do arquivo recebido: %ld bytes \n", file_size);

    	// Cria um arquivo local para armazenar a imagem
    	FILE* file = fopen("perfil.jpg", "wb");
    	if (file == NULL) {
        	perror("Erro ao criar arquivo local. \n");
    	}
		
    	// Recebe e armazena a imagem em pacotes
    	char buffer[MAX_BUFF_SIZE];
    	size_t total_bytes_received = 0;
    	ssize_t bytes_written;
    	while (total_bytes_received < file_size) {
			
        	ssize_t bytes_received = recvfrom(server_socket, buffer, MAX_BUFF_SIZE, 0, (struct sockaddr*)&server_address, &server_address_length);
			if (bytes_received == -1) {
            	perror("Erro ao receber dados. \n");
        	}

        	bytes_written = fwrite(buffer, sizeof(char), bytes_received, file);
			
        	if (bytes_written < bytes_received) {
            	perror("Erro ao escrever no arquivo local. \n");
        	}
        	total_bytes_received += bytes_received;
    	}
    	printf("Imagem recebida e salva como 'perfil.jpg' \n");
    	// Fecha o arquivo e o socket
    	open_image("perfil.jpg");
		fclose(file);
		
	}
	printf("\n## Por favor, selecione a próxima operação: ##\n");
}

