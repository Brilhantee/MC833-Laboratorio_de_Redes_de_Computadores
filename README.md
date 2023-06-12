# MC883-Laboratorio_de_Redes_de_Computadores
Os projetos foram desenvolvidos pela minha equipe durante a disciplina MC833 em 2023, com o objetivo de colocar em prática os conhecimentos adquiridos ao longo do curso. Ambos os projetos se concentraram na comunicação cliente-servidor por meio de sockets. Foram implementadas oito operações que o cliente pode solicitar ao servidor, incluindo:

* Cadastro de um novo perfil usando o e-mail como identificador.
* Listagem de todas as pessoas (e-mail e nome) formadas em um determinado curso.
* Listagem de todas as pessoas (e-mail e nome) que possuam uma determinada habilidade.
* Listagem de todas as pessoas (e-mail, nome e curso) formadas em um determinado ano.
* Listagem de todas as informações de todos os perfis.
* Dado o e-mail de um perfil, retorno das suas informações.
* Remoção de um perfil com base no seu identificador (e-mail).
* Download da imagem de um perfil com base no seu identificador (apenas no projeto 2, com UDP).

O projeto 1 consiste em um servidor TCP paralelo capaz de se comunicar com vários clientes simultaneamente, enquanto o projeto 2 é um servidor UDP interativo que se comunica com um cliente de cada vez.
