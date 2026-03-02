#include <arpa/inet.h>   // inet_addr(), inet_ntoa().
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>      // bzero().
#include <sys/socket.h>
#include <unistd.h>       // read(), write(), close().

#define MAX 1024
#define PORT 8080          // Puerto por defecto.
#define SA struct sockaddr

// Función para desencriptar el mensaje con la clave privada.
int decrypt(int x, int key) {
    // Desencriptación: d = (x - public_key + 100) mod 100.
    return ((x - key + 100)%100);
}

// Función que realiza el servicio comunicandose con el servidor.
void func(int sockfd, int public_key) {
    int buffer[MAX];
    int i, decrypted;
    int num_array[20];  // Para almacenar los números cifrados recibidos.

    while (recv(sockfd, buffer, sizeof(buffer),0) > 0){
        printf("Números cifrados recibidos.");
        
        for (i = 0; i < 20; i++) {
            printf("%d ", buffer[i]);
        }
        printf("\n");

        // Desencriptar y mostrar los números en la salida estándar.
        printf("Números desencriptados: ");
        for (i = 0; i < 20; i++) {
            decrypted = decrypt(buffer[i], public_key);
            printf("\033[45m%d\033[0m ", decrypted);
        }
        printf("\n");
    }
}

// Función principal que se encarga de la creación de la conexión.
int main() {
    /*  DECLARACIONES   */
    int sockfd;                     // Descriptor de archivo de socket
    struct sockaddr_in servaddr;    // Estructura para información del servidor (Familia IP, Nº Puerto, Host IP)
    char host[256];                 // Almacena la IP del host indicado por el usr.
    int port;                       // Numero de puerto

    // Pedir al usuario la IP y el puerto del servidor.
    printf("Ingrese la IP del servidor: ");
    fgets(host, sizeof(host), stdin);
    host[strcspn(host, "\n")] = '\0';  // Se elimina el salto de línea al final.

    printf("Ingrese el puerto: ");
    scanf("%d", &port);

    // Crear el socket.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Error al crear el socket...\n");
        exit(0);
    } else
        printf("Socket creado exitosamente..\n");

    bzero(&servaddr, sizeof(servaddr));

    // Asignar IP y puerto al cliente.
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(host);
    servaddr.sin_port = htons(port);

    // Conectar al servidor.
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Error al conectar con el servidor...\n");
        exit(0);
    } else
        printf("Conectado al servidor %s: %d\n", host, port);

    // Envío de clave pública
    int public_key = 5; // Clave publica.
    // Enviar la llave pública al servidor.
    char pub_key[10];
    snprintf(pub_key, sizeof(pub_key), "%d", public_key);  // Clave pública.
    if (send(sockfd, pub_key, sizeof(pub_key),0) < 0) {
        printf("Error al enviar la llave pública.\n");
        exit(0);
    }

    // Función para recibir e imprimir los números.
    func(sockfd, public_key);
    
    printf("Cerrando conexion.");
    // Cerrar el socket.
    close(sockfd);
    return 0;
}