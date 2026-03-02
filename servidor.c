#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>  //por ahora no en uso, pero se mantiene x el ejemplo.
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> // para usar el host especifico (inet_addr).

#define SA struct sockaddr
#define MAX 1024
#define PORT 8080  // Puerto por defecto, puedes cambiarlo a tu necesidad.
char buffer[MAX];   // Buffer para datos.

// Función de cifrado usando la clave pública como coeficiente.
int encrypt(int x, int key) {
    return (x + key)%100;  // Cifrado: c = (x + public_key) mod 100.
}

void func(int connfd) {
    int key;            // Variable para la llave pública del cliente.
    int num_array[20];  // Arreglo para los 20 números aleatorios.
    int i, encrypted;   // Variables para el índice y el número cifrado.

    // Recibir la llave pública del cliente.
    bzero(buffer, MAX); // Limpiar el buffer antes de usarlo.
    if (recv(connfd, buffer, sizeof(buffer),0) <= 0) {
        printf("Error: No se pudo recibir la llave pública.\n");
        return;
    }
    key = atoi(buffer); // Convertir la cadena recibida a un número entero (llave pública).
    printf("Llave pública recibida: \033[32m%d\033[0m\n", key);

    // Generar y cifrar los 20 números aleatorios entre 0 y 26.
    printf("Los números enviados sin encriptar son: ");
    for (i = 0; i < 20; i++) {
        int rnum = rand() % 27;  // Generamos un número aleatorio entre 0 y 26.
        printf("\033[45m%d\033[0m, ",rnum);
        encrypted = encrypt(rnum, key);  // Cifrar el número con la llave pública.
        num_array[i] = encrypted;  // Almacenamos el número cifrado en el arreglo.
    }
    printf("\n");

    // Enviar el arreglo de números cifrados al cliente.
    if (send(connfd, num_array, sizeof(num_array),0) < 0) {  // Enviar el arreglo.
        printf("Error: No se pudo enviar los números cifrados.\n");
        return;
    }
    printf("20 números cifrados enviados correctamente.\n");
}

int main() {
    /*  Declaraciones   */
    int sockfd, connfd, len;            // Se declaran los descriptores de socket y conexión. len se utiliza como 
    struct sockaddr_in servaddr, cli;   // Estructura para información del servidor y del cliente(Familia IP, Nº Puerto, Host IP)

    // Crear el socket.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);  // Tipo TCP.
    if (sockfd == -1) {
        printf("Error al crear el socket...\n");
        exit(0);
    }
    else
        printf("Socket creado exitosamente..\n");

    bzero(&servaddr, sizeof(servaddr));     // A forma de eliminar información indeseada, se "limpia" los elementos de la estructura.

    // Asignar la IP y el puerto.
    servaddr.sin_family = AF_INET;                  // Familia de direcciones IPv4.
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   // Acepta cualquier IP.
    servaddr.sin_port = htons(PORT);                // Asignar el puerto, convertido a formato de red.

    // Enlazar el socket a la IP y al puerto especificados.
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("Error al hacer el bind del socket...\n");
        exit(0);
    }
    else
        printf("Socket enlazado exitosamente..\n");

    // Escuchar por conexiones entrantes.
    if ((listen(sockfd, 5)) != 0) {
        printf("Error al escuchar...\n");
        exit(0);
    }
    else
        printf("Servidor escuchando en el puerto:%d...\n", PORT);

    len = sizeof(cli);

    // Aceptar la conexión entrante.
    connfd = accept(sockfd, (SA*)&cli, &len);  // Aceptar una conexión del cliente.
    if (connfd < 0) {
        printf("Error al aceptar la conexión...\n");
        exit(0);
    }
    else
        printf("Conexión aceptada desde %s\n", inet_ntoa(cli.sin_addr));

    // Llamar a la función para manejar la comunicación.
    func(connfd);

    printf("Cerrando conexión.\n");
    shutdown(connfd,SHUT_WR);

    // Cerrar el socket.
    close(sockfd);
    return 0;
}