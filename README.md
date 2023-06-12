# MC883-Laboratorio_de_Redes_de_Computadores
Os projetos foram desenvolvidos pela a minha equipe durante a diciplina MC833 em 2023, nos quais foi colocado em prática os conhecimento adquiridos este curso curso.
O principal foco, em ambos projetos, foi a comunicação cliente-servidor através de sockets. Foi implementada oito operações que o cliente pode exigir do servidor, como:

* Cadastrar um novo perfil utilizando o email como identificador; 
* Listar todas as pessoas (email e nome) formadas em um determinado curso;
* Listar todas as pessoas (email e nome) que possuam uma determinada habilidade;
* Listar todas as pessoas (email, nome e curso) formadas em um determinado ano;
* Listar todas as informações de todos os perfis;
* Dado o email de um perfil, retornar suas informações;
* Remover um perfil a partir de seu identificador (email);
* Fazer o download da imagem de um perfil a partir de seu identificador (somente no projeto
2, com UDP).

O projeto 1, é um servidor TCP paralelo que pode se comunicar com varios clientes ao mesmo tempo, enquanto o projeto 2, é um servidor UDP interativo que se comunica com um cliente de cada vez.
