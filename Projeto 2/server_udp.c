#include "functions_declarations.h"

struct status_of_messages {

	uint16_t messages_from_client;
	uint16_t messages_send_to_client;
};

struct status_of_messages order_of_messages;
 
int main()
{
	int socket_udp, client_socket, server_address_size, bind_connection;
	struct sockaddr_in server_address;
	char file_name[] = "clientes.txt";
	
	creating_a_file_to_store_profile_information(file_name);

	socket_udp = socket(AF_INET, SOCK_DGRAM, 0);
	check_socket_creation(socket_udp);
	
	server_address = address_creation(server_address);

	bind_connection = bind(socket_udp, (SA*)&server_address, sizeof(server_address));
	check_bind_connection(bind_connection);

	operating(socket_udp, file_name);

	close(socket_udp);
	exit(0);
}

void creating_a_file_to_store_profile_information(char* file_name){
    FILE* file = fopen(file_name, "w");
    if (file == NULL) {
        perror("## Erro ao criar o arquivo");
        return;
    }
	fclose(file);
    perfis_fixos(file_name);
}

void check_socket_creation(int socket){
	if (socket == -1) {
		printf("## Falha na criação do Socket...\n");
		exit(0);
	}
	else
		printf("## Socket Criado com Sucesso...\n");
}

void check_bind_connection(int bind_connection){
	if (bind_connection != 0) {
		printf("## Falha na Ligação do Socket...\n");
		exit(0);
	}
	else
		printf("## Socket Vinculado com Sucesso...\n");
}

struct sockaddr_in address_creation(struct sockaddr_in server_address){
	bzero(&server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT);
	return server_address;

}

void operating(int socket_udp, char* file_name){
	fd_set read_fds;
	int close_server;

	printf("## Servidor UDP iniciado. Aguardando conexões...\n");

    while (1) {
        FD_ZERO(&read_fds);

        FD_SET(socket_udp, &read_fds);
        int max_fd = socket_udp;

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("Erro ao executar select");
            exit(1);
        }

        if (FD_ISSET(socket_udp, &read_fds)) {
			close_server = receiving_the_operation_chosen_by_the_user(socket_udp, file_name);
			if (close_server == -1)
				break;
        }
    }
}

int receiving_the_operation_chosen_by_the_user(int client_socket, char* file_name)
{
	char* payload;
	int operation;
	struct sockaddr_in client_address;
	order_of_messages.messages_from_client = 0;
	order_of_messages.messages_send_to_client = 0;

	payload = receve_udp_message(client_socket, &client_address, &order_of_messages.messages_from_client);
	memcpy(&operation, payload, sizeof(int));

	if (operation == 1)
		to_register_a_new_user(file_name, payload);
	else if (operation == 2)
		send_all_users_from_a_course_to_the_client(file_name, payload, client_socket, client_address);
	else if (operation == 3)
		send_all_users_with_a_skill_to_the_client(file_name, payload, client_socket, client_address);
	else if (operation == 4)
		send_all_users_with_the_same_graduation_date_to_the_client(file_name, payload, client_socket, client_address);
	else if (operation == 5)
		send_all_users_informations_to_client(file_name, payload, client_socket, client_address);
	else if (operation == 6)
		send_all_user_information(file_name, payload, client_socket,client_address);
	else if (operation == 7)
		delete_a_user_from_dataset(file_name, payload, client_socket, client_address);
	else if (operation == 8)
		return -1;
	else if (operation == 10)
		sent_image_part1(client_socket, client_address);
	else
		return 0;

	return 0;
}

void to_register_a_new_user(char* file_name, char* payload){
	struct profile new_user;
	int bytes_read = 4;
	
	new_user.email = message_deserialization(&bytes_read, payload);
	new_user.name = message_deserialization(&bytes_read, payload);
	new_user.last_name = message_deserialization(&bytes_read, payload);
	new_user.residence = message_deserialization(&bytes_read, payload);
	new_user.academic_education = message_deserialization(&bytes_read, payload);
	new_user.graduation_year = message_deserialization(&bytes_read, payload);
	new_user.skill = message_deserialization(&bytes_read, payload);
	
	if (busca_no_arquivo(file_name, new_user.email) == 0){
		printf("\n##  Novo Usuário Cadatrado No Servidor  ##");
		printf("\nE-mail: %s", new_user.email);
		printf("\nNome: %s", new_user.name);
		printf("\nSobrenome: %s", new_user.last_name);
		printf("\nResidência: %s", new_user.residence);
		printf("\nFormação Acadêmica: %s", new_user.academic_education);
		printf("\nAno de formatura: %s", new_user.graduation_year);
		printf("\nHabilidades: %s", new_user.skill);

		escrita_no_arquivo(file_name, new_user);
	}
	else
		printf("\n##  Perfil já cadastrado no sistema  ##\n");

	free(new_user.email);
	free(new_user.name);
	free(new_user.last_name);
	free(new_user.residence);
	free(new_user.academic_education);
	free(new_user.graduation_year);
	free(new_user.skill);
}

int busca_no_arquivo(char *arquivo, char *palavra) {
	FILE *ponteiro_arquivo;
	int numeracao_da_linha = 1;
	int resultado = 0;
	char caracteres_do_texto[200];
	
	if((ponteiro_arquivo = fopen(arquivo, "r")) == NULL) { // Não foi possível abrir o arquivo.
		return(-1);
	}

	while(fgets(caracteres_do_texto, 200, ponteiro_arquivo) != NULL) {
		if(strcmp(caracteres_do_texto, palavra) == 0) {
			resultado++;
		}
		numeracao_da_linha++;
	}

	// Fechando o arquivo.
	if(ponteiro_arquivo) {
		fclose(ponteiro_arquivo);
	}

	if(resultado == 0) { // Informação não foi encontrada no arquivo.
		return(0);
	}
	else{ // retornando a linha da informação buscada.
		return numeracao_da_linha;
	}
	
}

int escrita_no_arquivo(char *arquivo, struct profile profile){
	FILE *ponteiro_arquivo;
	ponteiro_arquivo = fopen(arquivo, "a");  // Cria ou abre um arquivo texto para gravação no modo de append.
	
	if (ponteiro_arquivo == NULL){ // Se não conseguiu abrir ou criar o arquivo
   		printf("Problemas com a inicialização do arquivo. \n");
   		return -1;
	}
	
	fprintf(ponteiro_arquivo,"%s\n", profile.email);
	fprintf(ponteiro_arquivo,"%s\n", profile.name);
	fprintf(ponteiro_arquivo,"%s\n", profile.last_name);
	fprintf(ponteiro_arquivo,"%s\n", profile.residence);
	fprintf(ponteiro_arquivo,"%s\n", profile.academic_education);
	fprintf(ponteiro_arquivo,"%s\n", profile.graduation_year); 
	fprintf(ponteiro_arquivo,"%s\n", profile.skill);

	// Fechando o arquivo.
	if(ponteiro_arquivo) {
		fclose(ponteiro_arquivo);
	}

	return 0;
}

void send_all_users_from_a_course_to_the_client(char* file_name, char* receve_buffer, int client_socket, struct sockaddr_in client_address){
	int count = 4;
	int user_count = 0;
	char* buffer;
	struct profile* user_list;

	char* course = message_deserialization(&count, receve_buffer);

	user_list = listagem_de_perfis_por_campo(file_name, course, &user_count);

	buffer = serialization_of_user_list(user_list, user_count);

	sendto(client_socket, (char *)buffer, MAX_BUFF_SIZE, MSG_CONFIRM, (const struct sockaddr *) &client_address, sizeof(client_address));

	free(buffer);
}

struct profile* listagem_de_perfis_por_campo(char* arquivo, char* campo, int* user_count){

	FILE *ponteiro_arquivo;
	struct profile* user_list = malloc(sizeof(struct profile) * 5);
	ponteiro_arquivo = fopen(arquivo, "r"); // Abrindo o arquivo em modo leitura.
	char caracteres_do_texto[200];
	int listagem = 0;
	

	if (ponteiro_arquivo == NULL){ // Se não conseguiu abrir ou criar o arquivo
   		printf("\nProblemas com a inicialização do arquivo. \n");
	}

	while(fgets(caracteres_do_texto, 200, ponteiro_arquivo) != NULL){
		char* string_email = malloc(MAX);
		char* string_nome = malloc(MAX);
		char* string_sobrenome = malloc(MAX);
		char* string_residencia = malloc(MAX);
		char* string_formacao = malloc(MAX); 
		char* string_ano_de_formatura = malloc(MAX);
		char* string_habilidades = malloc(MAX);

		strcpy(string_email, caracteres_do_texto);
		fgets(caracteres_do_texto, 200, ponteiro_arquivo);
		strcpy(string_nome, caracteres_do_texto);
		fgets(caracteres_do_texto, 200, ponteiro_arquivo);
		strcpy(string_sobrenome, caracteres_do_texto);
		fgets(caracteres_do_texto, 200, ponteiro_arquivo);
		strcpy(string_residencia, caracteres_do_texto);
		fgets(caracteres_do_texto, 200, ponteiro_arquivo);
		strcpy(string_formacao, caracteres_do_texto);
		fgets(caracteres_do_texto, 200, ponteiro_arquivo);
		strcpy(string_ano_de_formatura, caracteres_do_texto);
		fgets(caracteres_do_texto, 200, ponteiro_arquivo);
		strcpy(string_habilidades, caracteres_do_texto);

		if (strstr(string_formacao, campo) != NULL || strstr(string_habilidades, campo) != NULL){
			listagem ++;
			
			if (listagem == 1){
				printf("\n##  Perfis Encontrados  ## \n");
			}

			user_list[*user_count].email = remove_newline(string_email);
			user_list[*user_count].name =remove_newline(string_nome);
			user_list[*user_count].last_name = remove_newline(string_sobrenome);
			user_list[*user_count].residence = remove_newline(string_residencia);
			user_list[*user_count].academic_education = remove_newline(string_formacao);
			user_list[*user_count].graduation_year = remove_newline(string_ano_de_formatura);
			user_list[*user_count].skill = remove_newline(string_habilidades);
			(*user_count) ++;

		}
		if (strstr(string_ano_de_formatura, campo) != NULL){
			listagem ++;
			if (listagem == 1){
				printf("\n## Perfis Encontrados ## \n");
			}

			user_list[*user_count].email = remove_newline(string_email);
			user_list[*user_count].name =remove_newline(string_nome);
			user_list[*user_count].last_name = remove_newline(string_sobrenome);
			user_list[*user_count].residence = remove_newline(string_residencia);
			user_list[*user_count].academic_education = remove_newline(string_formacao);
			user_list[*user_count].graduation_year = remove_newline(string_ano_de_formatura);
			user_list[*user_count].skill = remove_newline(string_habilidades);
			(*user_count) ++;
		}
		
	}

	if (listagem == 0){
		printf("\n## Não foram encontradas pessoas com o seu pedido. ##\n");
	}
	
	// Fechando o arquivo.
	if(ponteiro_arquivo) {
		fclose(ponteiro_arquivo);
	}
	return user_list;
}

char* serialization_of_user_list(struct profile* user_list, int user_count){
	char* buff = malloc(MAX_BUFF_SIZE);
	int message_size = 0;

	memcpy(buff, &user_count, sizeof(int));
	message_size += sizeof(int);

	for (int i = 0; i < user_count; i ++){

		message_serialization(&message_size, (user_list[i]).email, buff);
		message_serialization(&message_size, (user_list[i]).name, buff);
		message_serialization(&message_size, (user_list[i]).last_name, buff);
		message_serialization(&message_size, (user_list[i]).residence, buff);
		message_serialization(&message_size, (user_list[i]).academic_education, buff);
		message_serialization(&message_size, (user_list[i]).graduation_year, buff);
		message_serialization(&message_size, (user_list[i]).skill, buff);
	}
	return buff;
}

void send_all_users_with_a_skill_to_the_client(char* file_name, char* receve_buffer, int client_socket, struct sockaddr_in client_address){
	int count = 4;
	int user_count = 0;
	char* buffer;
	struct profile* user_list;

	char* skill = message_deserialization(&count, receve_buffer);

	user_list = listagem_de_perfis_por_campo(file_name, skill, &user_count);

	buffer = serialization_of_user_list(user_list, user_count);

	sendto(client_socket, (char *)buffer, MAX_BUFF_SIZE, MSG_CONFIRM, (const struct sockaddr *) &client_address, sizeof(client_address));
	free(buffer);
}

void send_all_users_with_the_same_graduation_date_to_the_client(char* file_name, char* receve_buffer, int client_socket, struct sockaddr_in client_address){
	int count = 4;
	int user_count = 0;
	char* buffer;
	struct profile* user_list;

	char* graduation_date = message_deserialization(&count, receve_buffer);

	user_list = listagem_de_perfis_por_campo(file_name, graduation_date, &user_count);

	buffer = serialization_of_user_list(user_list, user_count);

	sendto(client_socket, (char *)buffer, MAX_BUFF_SIZE, MSG_CONFIRM, (const struct sockaddr *) &client_address, sizeof(client_address));
	free(buffer);
}

void send_all_users_informations_to_client(char* file_name, char* receve_buffer, int client_socket, struct sockaddr_in client_address){
	int user_found, ciclos, remainder, payload_length;
	char* payload = malloc(MAX_PAYLOAD_LENGTH);
	struct profile* user_list;

	user_found = 0;
	user_list = listar_perfis(file_name, &user_found);

	ciclos = user_found / 5;
	remainder = user_found % 5;

	payload_length = 0;
	memcpy(payload, &user_found, sizeof(int));
	payload_length += sizeof(int);

	send_udp_message(client_socket, client_address, payload, payload_length, &(order_of_messages.messages_send_to_client));
	payload_length = 0;

	for (int i = 0; i < ciclos; i ++){
		for(int j = i * 5; j < (i + 1) * 5; j ++){

			message_serialization(&payload_length, user_list[j].email, payload);
			message_serialization(&payload_length, user_list[j].name, payload);
			message_serialization(&payload_length, user_list[j].last_name, payload);
			message_serialization(&payload_length, user_list[j].residence, payload);
			message_serialization(&payload_length, user_list[j].academic_education, payload);
			message_serialization(&payload_length, user_list[j].graduation_year, payload);
			message_serialization(&payload_length, user_list[j].skill, payload);
		}
		send_udp_message(client_socket, client_address, payload, payload_length, &(order_of_messages.messages_send_to_client));
		payload_length = 0;
	}

	if (remainder > 0){
		for(int i = ciclos * 5; i < user_found; i ++){

			message_serialization(&payload_length, user_list[i].email, payload);
			message_serialization(&payload_length, user_list[i].name, payload);
			message_serialization(&payload_length, user_list[i].last_name, payload);
			message_serialization(&payload_length, user_list[i].residence, payload);
			message_serialization(&payload_length, user_list[i].academic_education, payload);
			message_serialization(&payload_length, user_list[i].graduation_year, payload);
			message_serialization(&payload_length, user_list[i].skill, payload);
		}

		send_udp_message(client_socket, client_address, payload, payload_length, &(order_of_messages.messages_send_to_client));
	}

	free(payload);
	deallocate_memory_from_profiles(user_list, user_found);
}

struct profile* listar_perfis(char* arquivo, int* user_count){
	FILE *ponteiro_arquivo;
	ponteiro_arquivo = fopen(arquivo, "r"); // Abrindo o arquivo em modo leitura.
	char* caracteres_do_texto = malloc(MAXSKILLS + NULL_TERMINATOR);
	int struct_list_limit = 5;
	struct profile* user_list = malloc(sizeof(struct profile) * 5);

	if (ponteiro_arquivo == NULL){ // Se não conseguiu abrir ou criar o arquivo
   		printf("Problemas com a inicialização do arquivo. \n");
	}
	int i = 1;
	// Listagem dos perfis
	while(fgets(caracteres_do_texto, MAX + NULL_TERMINATOR, ponteiro_arquivo) != NULL) {

		char* string_email = malloc(MAX + NULL_TERMINATOR);
		char* string_nome = malloc(MAX + NULL_TERMINATOR);
		char* string_sobrenome = malloc(MAX + NULL_TERMINATOR);
		char* string_residencia = malloc(MAX + NULL_TERMINATOR);
		char* string_formacao = malloc(MAX + NULL_TERMINATOR); 
		char* string_ano_de_formatura = malloc(MAX + NULL_TERMINATOR);
		char* string_habilidades = malloc(MAXSKILLS + NULL_TERMINATOR);

		strcpy(string_email, caracteres_do_texto);
		fgets(caracteres_do_texto, MAX + NULL_TERMINATOR, ponteiro_arquivo);
		strcpy(string_nome, caracteres_do_texto);
		fgets(caracteres_do_texto, MAX + NULL_TERMINATOR, ponteiro_arquivo);
		strcpy(string_sobrenome, caracteres_do_texto);
		fgets(caracteres_do_texto, MAX + NULL_TERMINATOR, ponteiro_arquivo);
		strcpy(string_residencia, caracteres_do_texto);
		fgets(caracteres_do_texto, MAX + NULL_TERMINATOR, ponteiro_arquivo);
		strcpy(string_formacao, caracteres_do_texto);
		fgets(caracteres_do_texto, MAX + NULL_TERMINATOR, ponteiro_arquivo);
		strcpy(string_ano_de_formatura, caracteres_do_texto);
		fgets(caracteres_do_texto, MAXSKILLS + NULL_TERMINATOR, ponteiro_arquivo);
		strcpy(string_habilidades, caracteres_do_texto);


		user_list[*user_count].email = remove_newline(string_email);
		user_list[*user_count].name = remove_newline(string_nome);
		user_list[*user_count].last_name = remove_newline(string_sobrenome);
		user_list[*user_count].residence = remove_newline(string_residencia);
		user_list[*user_count].academic_education = remove_newline(string_formacao);
		user_list[*user_count].graduation_year = remove_newline(string_ano_de_formatura);
		user_list[*user_count].skill = remove_newline(string_habilidades);
		(*user_count) ++;

		if ((*user_count) == struct_list_limit){
			struct_list_limit *= 2;
			user_list = realloc(user_list, struct_list_limit * sizeof(struct profile));
		}
		i++;
		if(i == 8){
			printf("\n");
			
			i = 1;
		}
	}

	// Fechando o arquivo.
	if(ponteiro_arquivo) {
		fclose(ponteiro_arquivo);
	}

	free(caracteres_do_texto);
	return user_list;
}

void send_all_user_information(char* file_name, char* receve_buffer, int client_socket, struct sockaddr_in client_address){
	int count = 4;
	int user_found;
	char* buff = malloc(MAX_BUFF_SIZE);
	struct profile* user_informatio;

	char* email = message_deserialization(&count, receve_buffer);

	user_informatio = buscar_perfil(file_name, email, &user_found);
	send_user_to_client(client_socket, user_informatio, user_found, client_address);
	free(buff);
}

void remove_extra_spaces(char* str) {
    size_t len = strcspn(str, " \n");  // Obtém o comprimento da substring sem espaços e quebras de linha
    str[len] = '\0';  // Termina a string no último caractere válido
}

struct profile* buscar_perfil(char* arquivo, char* email, int* user_found){
	*user_found = 0;
	FILE *ponteiro_arquivo;
	ponteiro_arquivo = fopen(arquivo, "r"); // Abrindo o arquivo em modo leitura.
	char* caracteres_do_texto = malloc(MAX);
	struct profile* user = malloc(sizeof(struct profile));

	if (ponteiro_arquivo == NULL){ // Se não conseguiu abrir ou criar o arquivo
   		printf("Problemas com a inicialização do arquivo. \n");
	}
	remove_extra_spaces(email);
	while(fgets(caracteres_do_texto, MAX, ponteiro_arquivo) != NULL) {
		remove_extra_spaces(caracteres_do_texto);
		if((strncmp(caracteres_do_texto, email, strlen(caracteres_do_texto))) == 0) {
			printf("Acessando informações do perfil: \n");

			user->email = malloc(MAX);
			memcpy(user->email, caracteres_do_texto, MAX);
			printf("%s",user->email);
			user->email = remove_newline(user->email);

			user->name = malloc(MAX);
			fgets(user->name, MAX, ponteiro_arquivo);
			printf("%s",user->name);
			user->name = remove_newline(user->name);

			user->last_name = malloc(MAX);
			fgets(user->last_name, MAX, ponteiro_arquivo);
			printf("%s",user->last_name);
			user->last_name = remove_newline(user->last_name);

			user->residence = malloc(MAX);
			fgets(user->residence, MAX, ponteiro_arquivo);
			printf("%s",user->residence);
			user->residence = remove_newline(user->residence);

			user->academic_education = malloc(MAX);
			fgets(user->academic_education, MAX, ponteiro_arquivo);
			printf("%s",user->academic_education);
			user->academic_education = remove_newline(user->academic_education);

			user->graduation_year = malloc(MAX);
			fgets(user->graduation_year, MAX, ponteiro_arquivo);
			printf("%s",user->graduation_year);
			user->graduation_year = remove_newline(user->graduation_year);

			user->skill = malloc(MAXSKILLS);
			fgets(user->skill, MAXSKILLS, ponteiro_arquivo);
			printf("%s",user->skill);
			user->skill = remove_newline(user->skill);

			*user_found = 1;
			break;
		}
	}
	free(caracteres_do_texto);
	if (*user_found == 0){
		printf("E-mail não encontrado. \n");
		return user;
	}
	// Fechando o arquivo.
	if(ponteiro_arquivo) {
		fclose(ponteiro_arquivo);
	}
	return user;
}

void send_user_to_client(int client_socket, struct profile* user, int user_found, struct sockaddr_in client_address){
	char* buffer = malloc(MAX_BUFF_SIZE);
	int message_size = 0;
	printf("%d \n", user_found);
	memcpy(buffer, &user_found, sizeof(int));
	message_size += sizeof(int);

	if (user_found == 1){

		message_serialization(&message_size, user->email, buffer);
		message_serialization(&message_size, user->name, buffer);
		message_serialization(&message_size, user->last_name, buffer);
		message_serialization(&message_size, user->residence, buffer);
		message_serialization(&message_size, user->academic_education, buffer);
		message_serialization(&message_size, user->graduation_year, buffer);
		message_serialization(&message_size, user->skill, buffer);
	}

	sendto(client_socket, (char *)buffer, MAX_BUFF_SIZE, MSG_CONFIRM, (const struct sockaddr *) &client_address, sizeof(client_address));
}

void delete_a_user_from_dataset(char* file_name, char* receve_buffer, int client_socket, struct sockaddr_in client_address){
	int count = 4;
	int user_removed;
	char* buffer = malloc(MAX_BUFF_SIZE);

	char* email = message_deserialization(&count, receve_buffer);
	printf("\nEmail: %s\n", email);
	user_removed = remover_perfil(file_name, email);

	memcpy(buffer, &user_removed, sizeof(int));

	sendto(client_socket, (char *)buffer, MAX_BUFF_SIZE, MSG_CONFIRM, (const struct sockaddr *) &client_address, sizeof(client_address));

	free(email);
	free(buffer);
}

int remover_perfil(char* arquivo, char* email){
	FILE *ponteiro_arquivo;
	FILE *ponteiro_arquivo_novo;
	char comparação[MAXSKILLS + NULL_TERMINATOR];
	ponteiro_arquivo = fopen(arquivo, "r"); // Abrindo o arquivo em modo leitura.
	ponteiro_arquivo_novo = fopen("clientes_provisorio.txt", "w"); // Abrindo o arquivo em modo escrita.
	char caracteres_do_texto[MAXSKILLS + NULL_TERMINATOR];
	int cliente_removido = 0;

	if (ponteiro_arquivo == NULL || ponteiro_arquivo_novo == NULL){ // Se não conseguiu abrir ou criar o arquivo
   		printf("Problemas com a inicialização do arquivo. \n");
	}

	while (fgets(caracteres_do_texto, MAXSKILLS + NULL_TERMINATOR, ponteiro_arquivo) != NULL) {
		strcpy(comparação, caracteres_do_texto);
		remove_newline(comparação);

        if((strcmp(comparação, email)) == 0) { // O e-mail foi encontrado.
            cliente_removido = 1;
			for(int i = 1; i <= 6; i++){
				fgets(caracteres_do_texto, MAXSKILLS + NULL_TERMINATOR, ponteiro_arquivo); // Essas linhas serão lidas, mas não copiadas para o novo arquivo.
			}
        }
        else {
            fprintf(ponteiro_arquivo_novo, "%s", caracteres_do_texto); // Copiando as linhas.
        }
    }

	if (cliente_removido == 0){
		printf("O cliente não está cadastrado no sistema. \n");
		// Fecha os arquivos
    	fclose(ponteiro_arquivo);
    	fclose(ponteiro_arquivo_novo);

    	// Substitui o arquivo original pelo novo arquivo
    	remove("clientes.txt");
    	rename("clientes_provisorio.txt", "clientes.txt");
		return 0;
	}
	else{
		printf("Perfil removido com sucesso. \n");
		// Fecha os arquivos
    	fclose(ponteiro_arquivo);
    	fclose(ponteiro_arquivo_novo);

    	// Substitui o arquivo original pelo novo arquivo
    	remove("clientes.txt");
    	rename("clientes_provisorio.txt", "clientes.txt");
		return 1;
	}
}

void perfis_fixos(char* arquivo){
	struct profile profile1, profile2, profile3, profile4, profile5;

	profile1.email = "eduardo@gmail.com";
	profile1.name = "Eduardo";
	profile1.last_name = "Gomes";
	profile1.residence = "Cosmópolis";
	profile1.academic_education = "Engenharia de Software";
	profile1.graduation_year = "2018";
	profile1.skill = "CSS, HTML, Projeto de Sistemas";

	if (busca_no_arquivo(arquivo, profile1.email) == 0){
		escrita_no_arquivo(arquivo, profile1);
	}

	profile2.email = "carlos@gmail.com";
	profile2.name = "Carlos";
	profile2.last_name = "Pereira";
	profile2.residence = "Paulínia";
	profile2.academic_education = "Engenharia de Computação";
	profile2.graduation_year = "2018";
	profile2.skill = "Ciência de Dados, Computação em Nuvem";

	if (busca_no_arquivo(arquivo, profile2.email) == 0){
		escrita_no_arquivo(arquivo, profile2);
	}

	profile3.email  = "wanessa@gmail.com";
	profile3.name = "Wanessa";
	profile3.last_name = "Teixeira";
	profile3.residence = "Jaguariúna";
	profile3.academic_education = "Ciência da Computação";
	profile3.graduation_year = "2015";
	profile3.skill = "Internet das Coisas, Computação em Nuvem";

	if (busca_no_arquivo(arquivo, profile3.email) == 0){
		escrita_no_arquivo(arquivo, profile3);
	}

	profile4.email = "beatriz@gmail.com";
	profile4.name = "Beatriz";
	profile4.last_name = "Camargo";
	profile4.residence = "Limeira";
	profile4.academic_education= "Engenharia de Computação";
	profile4.graduation_year = "2015";
	profile4.skill = "Ciência de Dados, Computação em Nuvem, Internet das Coisas";

	if (busca_no_arquivo(arquivo, profile4.email) == 0){
		escrita_no_arquivo(arquivo, profile4);
	}

	// Perfil 5
	profile5.email = "camila@gmail.com";
	profile5.name = "Camila";
	profile5.last_name = "Souza";
	profile5.residence = "Serra Negra";
	profile5.academic_education = "Engenharia de Software";
	profile5.graduation_year = "2016";
	profile5.skill = "Ciência de Dados, Computação em Nuvem";

	if (busca_no_arquivo(arquivo, profile5.email) == 0){
		escrita_no_arquivo(arquivo, profile5);
	}

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

void send_image_part2(int client_socket, struct sockaddr_in client_address, char* email){
	char* image_path = malloc(MAX_BUFF_SIZE);
	sprintf(image_path, "%s.jpg", email);
	FILE* file = fopen(image_path, "rb");
	if (file == NULL) {
        printf("Erro ao abrir o arquivo, pois o e-mail informado não está cadastrado no sistema. \n");
		long file_size = -1;
        sendto(client_socket, &file_size, sizeof(file_size), 0, (struct sockaddr*)&client_address, sizeof(client_address));
	}
	else{
    	fseek(file, 0, SEEK_END);
    	long file_size = ftell(file);
    	rewind(file);
    	sendto(client_socket, &file_size, sizeof(file_size), 0, (struct sockaddr*)&client_address, sizeof(client_address));
    	char buffer[MAX_BUFF_SIZE];
    	size_t bytes_read, bytes_sent;
    	while ((bytes_read = fread(buffer, sizeof(char), MAX_BUFF_SIZE, file)) > 0) {
        	bytes_sent = sendto(client_socket, buffer, bytes_read, 0, (struct sockaddr*)&client_address, sizeof(client_address));
        	if (bytes_sent < 0) {
            	perror("Erro ao enviar dados para o cliente. \n");
            	exit(1);
        	}
    	}
    fclose(file);
	free(image_path);
	}
}

void sent_image_part1(int client_socket, struct sockaddr_in client_address){
	while (1) {
        printf("Aguardando conexão...\n");
		
        // Recebe o e-mail do perfil do cliente
        char email[MAX_BUFF_SIZE];
        socklen_t client_address_length = sizeof(client_address);
        ssize_t bytes_received = recvfrom(client_socket, email, MAX_BUFF_SIZE, 0, (struct sockaddr*)&client_address, &client_address_length);
        if (bytes_received == -1) {
            perror("Erro ao receber dados do cliente. \n");
        }
		else{
			email[bytes_received] = '\0';
        	printf("Conexão estabelecida com o e-mail: %s \n", email);

        	send_image_part2(client_socket, client_address, email);
			break;
		}
    }
}
