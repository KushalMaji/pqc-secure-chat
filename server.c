#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <oqs/oqs.h>

#define PORT 8080

int main() {

    int server_fd, client_fd;
    struct sockaddr_in address;

    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_768);

    if (kem == NULL) {
        printf("ML-KEM not available!\n");
        return 1;
    }

    uint8_t *public_key = malloc(kem->length_public_key);
    uint8_t *ciphertext = malloc(kem->length_ciphertext);
    uint8_t *shared_secret = malloc(kem->length_shared_secret);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd,
         (struct sockaddr *)&address,
         sizeof(address));

    listen(server_fd, 1);

    printf("Server waiting...\n");

    client_fd = accept(server_fd, NULL, NULL);

    printf("Client connected.\n");

    recv(client_fd,
         public_key,
         kem->length_public_key,
         0);

    OQS_KEM_encaps(
        kem,
        ciphertext,
        shared_secret,
        public_key
    );

    send(client_fd,
         ciphertext,
         kem->length_ciphertext,
         0);

    printf("\nShared Secret:\n");

    for (size_t i = 0; i < 32; i++) {
        printf("%02X", shared_secret[i]);
    }

    printf("\n");

    free(public_key);
    free(ciphertext);
    free(shared_secret);

    OQS_KEM_free(kem);

    close(client_fd);
    close(server_fd);

    return 0;
}
